#include <cmath>
#include <thread>
#include <iostream>
#include <algorithm>
#include <type_traits>

#include "global_stack.hpp"

namespace GSTACK {

/* 
 * Use number of concurrent threads supported 
 * by the implementation as number of application 
 * threads calculating integral in parallel.
 */
const unsigned int Gstack_integrator::Appl_threads_num 
  = std::thread::hardware_concurrency();

void Gstack_integrator::integrate() {
  
  double A   = bound_m.first;
  double B   = bound_m.second;
  double fA  = function_m(A);
  double fB  = function_m(B);
  double sAB = (fA + fB) * (B - A) / 2;

#ifdef VERBOSE
  {
    std::cout << "Integrate A: "   << A   << 
                          " B: "   << B   << 
                          " fA: "  << fA  <<
                          " fB: "  << fB  << 
                          " sAB: " << sAB << std::endl;
  }
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
}

void Gstack_integrator::appl_thread_function() {

  /* While there are entries in global stack */
  while (true) {

    /* Try to obtain entry from the global stack */
    Entry entry = get_entry_from_gstack();

#ifdef VERBOSE
    {
      std::lock_guard<std::mutex> io_guard(mtx_io);
      std::cout << "T #" << std::this_thread::id << std::endl;
      std::cout << "Got entry A: "   << entry.A   << 
                            " B: "   << entry.B   << 
                            " fA: "  << entry.fA  << 
                            " fB: "  << entry.fB  << 
                            " sAB: " << entry.sAB << std::endl;
    }
#endif

    /* Stop main loop if period is terminal */
    if (entry.A > entry.B) {
      
#ifdef VERBOSE
      {
        std::lock_guard<std::mutex> io_guard(mtx_io);
        std::cout << "T #" << std::this_thread::id << std::endl;
        std::cout << "Got terminal entry, stop \n";
      }
#endif

      break;
    }

    /* Integrate another period locally */
    integrate_local(entry);

    /* Try-populate gstack with terminal periods */
    populate_gstack_terminal();
  }
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

#ifdef VERBOSE
    {
      std::lock_guard<std::mutex> io_guard(mtx_io);
      std::cout << "T #" << std::this_thread::id << std::endl;
      std::cout << "Integrating localy A: "  << entry.A   <<
                                     " B: "  << entry.B   <<
                                    " fA: "  << entry.fA  <<
                                    " fB: "  << entry.fB  << 
                                    " sAB: " << entry.sAB << std::endl;
    }
#endif

    double C  = (entry.A + entry.B) / 2;
    double fC = function_m(C);

    double sAC = (entry.fA + fC) * (C - entry.A) / 2;
    double sCB = (entry.fB + fC) * (entry.B - C) / 2;

    double sACB = sAC + sCB;

    /* Desired accuracy is succeded*/
    if (std::abs(entry.sAB - sACB) < Eps * sACB) {

#ifdef VERBOSE
      {
        std::lock_guard<std::mutex> io_guard(mtx_io);
        std::cout << "T #" << std::this_thread::id << std::endl;
        std::cout << "Precision succeded sAB: " << entry.sAB <<
                                      " sACB: " << sACB << std::endl;
      }
#endif

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
        break;

#ifdef VERBOSE
        {
          std::lock_guard<std::mutex> io_guard(mtx_io);
          std::cout << "T #" << std::this_thread::id << std::endl;
          std::cout << "Local stack is empty, stop \n";
        }
#endif
      }

      /* Else obtain another entry from local stack */
      entry = lstack.top();
      lstack.pop();

    } else { 

      /* Push [A;C] */
      lstack.push(Entry{entry.A, C, entry.fA, fC, sAC});

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
  if (lstack.size() < Max_local_sp || !gstack.empty()) {
    return;
  }

#ifdef VERBOSE
  {
    std::lock_guard<std::mutex> io_guard(mtx_io);
    std::cout << "T #" << std::this_thread::id << std::endl;
    std::cout << "Populating gstack \n";
  }
#endif

  while (lstack.size() > Max_local_sp) {
    /* 
     * Obtain entry from the local 
     * stack and move it tot the global 
     */
    Entry entry = lstack.top();
    gstack.push(entry);
    lstack.pop();
  }
}

void Gstack_integrator::populate_gstack_terminal() {

  /* Access to global stack */
  std::lock_guard<std::mutex> gstack_guard(mtx_gstack);

  nactive--;

  /* Continue condition */
  if (nactive || !gstack.empty()) {
    return;
  }

#ifdef VERBOSE
  {
    std::lock_guard<std::mutex> io_guard(mtx_io);
    std::cout << "T #" << std::this_thread::id << std::endl;
    std::cout << "Populating gstack with terminal entries \n";
  }
#endif

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

}; // namespace GSTACK