#include <iostream>
#include <cmath>

#include "global_stack.hpp"
using namespace GSTACK;

int main() {

  Gstack_integrator::function func = [](double x) -> double { return std::sin(1./x); };
  std::pair<double, double> bound = std::make_pair(1., 2.);

  Gstack_integrator integrator{func, bound};
  integrator.integrate();
  std::cout << "integrator result: " << integrator.res() << std::endl;

  return 0;
}