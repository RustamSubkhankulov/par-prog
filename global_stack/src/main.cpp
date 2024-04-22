#include <iostream>
#include <cmath>
#include <chrono>

#include "global_stack.hpp"
using namespace GSTACK;

int main() {

  Gstack_integrator::function func = [](double x) -> double { return std::sin(1./x); };
  std::pair<double, double> bound = std::make_pair(1E-5, 1.);

  Gstack_integrator integrator{func, bound};
  integrator.integrate();
  std::cout << "Integrator result: " << integrator.res() << std::endl;

  return 0;
}