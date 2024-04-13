#include <cstdlib>
#include <iostream>
#include <new>
#include <cmath>
#include <chrono>

#include "comp_math.hpp"

int main()
{  
  Comp_scheme comp_scheme{
    1e-3,     /* h        */
    5e-6,     /* tau      */
    10000008, /* x_points */
    100,      /* t_points */
    1e-2      /* a        */
  };

  comp_scheme.allocate();

  uint64_t x_points = comp_scheme.x_points();
  uint64_t t_points = comp_scheme.t_points();  

  /* u(0,x) = fi(x) */
  double h = comp_scheme.h();
  Comp_scheme::bound_func_type fi  = (double(*)(double))&std::sin;

  for (uint64_t x_idx = 0; x_idx < x_points; ++x_idx) {
    comp_scheme.set(x_idx, 0, fi(x_idx * h));
  }

  /* u(t,0) = psi(t) */
  double tau = comp_scheme.tau();
  Comp_scheme::bound_func_type psi = [](uint64_t t_idx) { return t_idx; };

  for (uint64_t t_idx = 0; t_idx < t_points; ++t_idx) {
    comp_scheme.set(0, t_idx, psi(t_idx * tau));
  }

  auto start_time = std::chrono::steady_clock::now();

  for (uint64_t t_idx = 1; t_idx < t_points; ++t_idx) {

    /* Compute chunk */
    comp_scheme.compute_range(1, x_points - 1, t_idx);
  }

  auto stop_time = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (stop_time - start_time).count();
  std::cout << "Elapsed: " << elapsed / 1000. << " sec \n";

  /* Free allocated arrays */
  comp_scheme.free();
  return 0;
}