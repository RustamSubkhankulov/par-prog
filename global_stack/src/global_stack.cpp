#include <cmath>
#include <thread>
#include <iostream>
#include <algorithm>
#include <type_traits>

#include "global_stack.hpp"

#ifdef VERBOSE
  #define VERBOSE_PRINT(...) appl_thread_print(__VA_ARGS__)
#else
  #define  VERBOSE_PRINT(...)
#endif

namespace GSTACK {

#ifndef NTHREADS
  /* 
   * Use number of concurrent threads supported 
   * by the implementation as number of application 
   * threads calculating integral in parallel.
   */
  const unsigned int Gstack_integrator::Appl_threads_num 
    = std::thread::hardware_concurrency();
#else 
    /* Use compile definition value */
  const unsigned int Gstack_integrator::Appl_threads_num = NTHREADS;
#endif

void Gstack_integrator::integrate() {
  
#ifdef TIME
  auto start_time = std::chrono::steady_clock::now();
#endif

  integral_value = 0;

  double A   = bound_m.first;
  double B   = bound_m.second;
  double fA  = function_m(A);
  double fB  = function_m(B);
  double sAB = (fA + fB) * (B - A) / 2;

#ifdef VERBOSE
  std::clog << "Integration [" << A << ";" << B << "] started \n";
#endif

  Entry initial_entry = {
    .A   = A,
    .B   = B,
    .fA  = fA,
    .fB  = fB,
    .sAB = sAB
  };

  gstack = Stack{}; // TODO: is it really necessary? 

  /* Initialize global stack with initial entry */
  gstack.push(initial_entry);

  /* Vector of application threads */
  std::vector<std::thread> tvec;

  /* 
   * At this points all counting semaphores
   * are initialized with value 1.
   */

#ifdef VERBOSE
  std::clog << "Running " << Appl_threads_num << " application threads.\n";
#endif

  /* Startup application threads */
  for (unsigned int thread_idx = 0; 
                    thread_idx < Appl_threads_num;
                    thread_idx++) {

    tvec.push_back(std::thread(appl_thread_func));
  }

  /* Wait for all of the application threads to end */
  for (auto thread = tvec.begin(); thread != tvec.end(); ++thread) {
    thread->join();
  }

#ifdef TIME

  auto stop_time = std::chrono::steady_clock::now();
  auto elapsed 
    = std::chrono::duration_cast<std::chrono::milliseconds> (stop_time - start_time).count();

  std::clog << "Total elapsed: " << elapsed / 1000. << " sec \n";

#endif

}

void Gstack_integrator::appl_thread_function() {

#ifdef TIME
  auto start_time = std::chrono::steady_clock::now();
#endif

  /* While there are entries in global stack */
  while (true) {

    /* Obtain entry from the global stack */
    Entry entry = get_entry_from_gstack();
    VERBOSE_PRINT("Got entry from gstack");

    /* Stop main loop if period is terminal */
    if (entry.A > entry.B) {
      
      VERBOSE_PRINT("Terminal entry obtained from gstack, stop");
      break;
    }

    /* Integrate another period locally */
    integrate_local(entry);

    /* Try-populate gstack with terminal periods */
    populate_gstack_terminal();
  }

#ifdef TIME

  auto stop_time = std::chrono::steady_clock::now();
  auto elapsed 
    = std::chrono::duration_cast<std::chrono::milliseconds> (stop_time - start_time).count();
  {
    std::lock_guard<std::mutex> io_guard(mtx_io);

    std::clog << "T #" << std::this_thread::get_id() << std::endl;
    std::clog << "Elapsed: " << elapsed / 1000. << " sec \n";
    std::clog << "-----" << std::endl;
  }
#endif

}

Gstack_integrator::Entry Gstack_integrator::get_entry_from_gstack() {

  /* Wait for the entries in global stack to appear */
  sem_task_present.acquire();

  /* Access to global stack */
  std::lock_guard<std::mutex> gstack_guard(mtx_gstack);

  /* Pop one entry frop global stack */
  Entry entry = gstack.top();
  gstack.pop();

  if (!gstack.empty()) {

    /* Give access to global stack to other threads */
    sem_task_present.release();
  }

  /* 
   * If period is not terminal, we increase number 
   * of threads, that have period to integrate
   */
  if (entry.A < entry.B || std::abs(entry.A - entry.B) < Precision) {
    nactive++;
  }

  return entry;
}

void Gstack_integrator::integrate_local(Entry entry) {

  /* Local stack */
  Stack lstack;

  while (true) {

    VERBOSE_PRINT("Integrating period localy");

    double C  = (entry.A + entry.B) / 2;
    double fC = function_m(C);

    double sAC = (entry.fA + fC) * (C - entry.A) / 2;
    double sCB = (entry.fB + fC) * (entry.B - C) / 2;

    double sACB = sAC + sCB;

    /* Desired accuracy is succeded*/
    if (std::abs(entry.sAB - sACB) < Eps * std::abs(sACB)) {

      VERBOSE_PRINT("Precision on period succeded");

      {
        /* 
         * Add computation result to the result 
         * integral value locking mutex 
         */
        std::lock_guard<std::mutex> res_guard(mtx_int_val);
        integral_value += sACB;
      }

      /* Nothing to integrate in local stack, break */
      if (lstack.empty()) {

        VERBOSE_PRINT("Local stack is empty, stop");
        break;
      }

      /* Else obtain another entry from local stack */
      entry = lstack.top();
      lstack.pop();

      VERBOSE_PRINT("Obtained new entry from lstack");

    } else { 

      /* Push [A;C] */
      lstack.push(Entry{entry.A, C, entry.fA, fC, sAC});
      VERBOSE_PRINT("Pushed period to local stack");

      /* entry now is [C;B] */
      entry.A   = C;
      entry.fA  = fC;
      entry.sAB = sCB;
    }

    populate_gstack(lstack);
  }
}

void Gstack_integrator::populate_gstack(Stack& lstack) {

  /* Access to global stack */
  std::lock_guard<std::mutex> gstack_guard(mtx_gstack);

  /* 
   * If higher local stack's size boundary is not exceeded,
   * or global stack is not empty, we do not populate gstack
   */
  if (lstack.size() <= Max_local_sp || !gstack.empty()) {
    return;
  }

  VERBOSE_PRINT("Populating gstack");

  while (!lstack.empty()) {
    /* 
     * Obtain entry from the local 
     * stack and move it tot the global 
     */
    Entry entry = lstack.top();
    gstack.push(entry);
    lstack.pop();
  }

  /* Give access to global stack to other threads */ 
  sem_task_present.release();
}

void Gstack_integrator::populate_gstack_terminal() {

  /* Access to global stack */
  std::lock_guard<std::mutex> gstack_guard(mtx_gstack);

  nactive--;

  /* Continue condition */
  if (nactive || !gstack.empty()) {
    return;
  }

  VERBOSE_PRINT("Populating gstack with terminal entries");

  /* A > B, terminal entry */
  Entry terminal_entry = {
    .A   = 2,
    .B   = 1,
    .fA  = 0,
    .fB  = 0,
    .sAB = 0,
  };

  /* 
   * Push terminal entries to global stack 
   * in amount of all application threads to
   * stop them all
   */
  for (unsigned int thread_idx = 0; 
                  thread_idx < Appl_threads_num;
                  thread_idx++) {

    gstack.push(terminal_entry);
  }

  /* Entries available in global stack */
  sem_task_present.release();
}

#ifdef VERBOSE

void Gstack_integrator::appl_thread_print(const std::string& msg) {
  
  std::lock_guard<std::mutex> io_guard(mtx_io);
  std::clog << "T #" << std::this_thread::get_id() << std::endl;
  std::clog << msg << std::endl;
  std::clog << "-----" << std::endl;
}

#endif // VERBOSE

}; // namespace GSTACK