#ifndef GLOBAL_STACK_HPP 
#define GLOBAL_STACK_HPP

#include <thread>
#include <stack>
#include <utility>
#include <semaphore>
#include <functional>

namespace GSTACK {

class Gstack_integrator {

public:

  using function = std::function<double(double)>;

private:

  struct Entry {

    double A;   // left bound
    double B;   // right bound
    double fA;  // f(A)
    double fB;  // f(B)
    double S;   // approx. integral value on period [A;B]
  };

  function function_m;
  std::pair<double, double> bound_m;

  static const unsigned int Appl_threads_num;

  using Stack = std::stack<Entry>;
  
  Stack gstack;
  unsigned int nactive = 0;
  double integral_value = 0;

  std::binary_semaphore sen_task_present{0};
  std::binary_semaphore sem_gstack{0};
  std::binary_semaphore sem_res{0};

public:

  Gstack_integrator(function function, std::pair<double, double> bound):
    function_m(function),
    bound_m(bound) {}

  virtual ~Gstack_integrator() {}

  function get_function() const { return function_m; }
  std::pair<double, double> get_bound() const { return bound_m; }

  double get_bound_left()  const { return bound_m.first;  }
  double get_bound_right() const { return bound_m.second; }

  void set_function(function function) {
    function_m = function;
  }

  void set_bound(std::pair<double, double> bound)  {
    bound_m = bound;
  }

  void integrate();
  double res() const { return integral_value; }

private:

  void appl_thread_func();
};

}; // namespace GSTACK

#endif // GLOBAL_STACK_HPP