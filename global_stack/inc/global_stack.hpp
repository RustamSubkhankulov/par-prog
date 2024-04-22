#ifndef GLOBAL_STACK_HPP 
#define GLOBAL_STACK_HPP

#include <thread>
#include <stack>
#include <mutex>
#include <utility>
#include <semaphore>
#include <functional>

namespace GSTACK {

class Gstack_integrator {

public:

  /* Type of integrated function */
  using function = std::function<double(double)>;

private:

  /* Precision of double comparison */
  static constexpr double Precision = 1E-6;

  /* Precision for break condition of integration */
  static constexpr double Eps = 1E-3;

  /* 
   * Maximum local stack size 
   * If local stack's size exceedes this value 
   * and theres is space available in global stack,
   * entries are moved from local stack to global
   */
  static constexpr unsigned int Max_local_sp = 2;

  struct Entry {

    double A;   // left bound
    double B;   // right bound
    double fA;  // f(A)
    double fB;  // f(B)
    double sAB; // approx. integral value on period [A;B]
  };

  /* Integrated function */
  function function_m;

  /* Boundaries of the integral */
  std::pair<double, double> bound_m;

  /* Number of aplication threads */
  static const unsigned int Appl_threads_num;

  /* Application thread main function */
  using appl_thread_function_t = std::function<void(void)>;
  appl_thread_function_t appl_thread_func;

  using Stack = std::stack<Entry>;
  
  /* Global stack with periods of integration */
  Stack gstack;

  /* Number of active processes */
  unsigned int nactive = 0;

  /* Result integral value */
  double integral_value = 0;

  /* 
   * State semaphore indicating non-zero 
   * amount of entries in global stack 
   */
  std::binary_semaphore sem_task_present{1};

  /* Access to global stack */
  std::mutex mtx_gstack;
  
  /* Access to result value of the integral */
  std::mutex mtx_int_val;

#ifdef VERBOSE
  /* IO mutex */
  std::mutex mtx_io;
#endif

public:

  Gstack_integrator(function function, std::pair<double, double> bound):
    function_m(function),
    bound_m(bound),
    appl_thread_func(std::bind(&Gstack_integrator::appl_thread_function, this)) 
    {}

  virtual ~Gstack_integrator() {}

  /* Getters: integrated function and boundaries */

  function get_function() const { return function_m; }
  std::pair<double, double> get_bound() const { return bound_m; }

  double get_bound_left()  const { return bound_m.first;  }
  double get_bound_right() const { return bound_m.second; }

  /* Setters: integrated function and boundaries */

  void set_function(function function) {
    function_m = function;
  }

  void set_bound(std::pair<double, double> bound)  {
    bound_m = bound;
  }

  /* Calculate integral  */
  void integrate();

  /* 
   * Get result value of the integral.
   * Returns last calculated integral value.
   * integrate() must be called before at least once.
   */
  double res() const { return integral_value; }

private:

  /* Application thread main function */
  void appl_thread_function();

  /* Pop entry from global stack if there are any left */
  Entry get_entry_from_gstack();

  /* 
   * Locally integrate one period in application 
   * thread using modified local stack algorithm 
   */
  void integrate_local(Entry entry);

  /*
   * Part of the local stack integration algorithm
   * Check for thee entrise to be moved from local
   * stack to global one and move them if there are any
   */
  void populate_gstack(Stack& lstack);

  /* 
   * Populate global stack with terminal entries 
   * on application thread end 
   */
  void populate_gstack_terminal();
};

}; // namespace GSTACK

#endif // GLOBAL_STACK_HPP