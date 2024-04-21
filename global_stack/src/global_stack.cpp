#include <algorithm>
#include <thread>
#include <iostream>
#include <type_traits>

#include "global_stack.hpp"

namespace GSTACK {

const unsigned int Gstack_integrator::Appl_threads_num 
  = std::thread::hardware_concurrency();

void Gstack_integrator::integrate() {
  
  std::vector<std::thread> tvec;

  std::function<void(void)> appl_thread_func 
    = std::bind(&Gstack_integrator::appl_thread_func, this);

  Entry initial_entry = {
    .A = bound_m.first,
    .B = bound_m.second,
    .fA = function_m(bound_m.first),
    .fB = function_m(bound_m.second),
    .S = //
  };
  gstack.push({get_bound_left(), get_bound_right(), })

  for (unsigned int thread_idx = 0; 
                    thread_idx < Appl_threads_num;
                    thread_idx++) {

    tvec.push_back(std::thread(appl_thread_func));
  }

  for (auto thread = tvec.begin(); thread != tvec.end(); ++thread) {
    thread->join();
  }
}

void Gstack_integrator::appl_thread_func() {

  while (true) {


  }
}

}; // namespace GSTACK