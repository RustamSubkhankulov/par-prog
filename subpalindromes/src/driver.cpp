#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <algorithm>

#ifdef PARALLEL
#include <thread>
#include <omp.h>
#endif

#include "subpalindromes.hpp"

int main()
{
  // Read source string, where we will search for the palindromes
  std::string source;
  if (!(std::cin >> source))
  {
    std::cout << "Invalid input format." << std::endl;
    std::cout << "Please, enter a single string." << std::endl;
    return EXIT_FAILURE;
  }

#ifdef VERBOSE
  std::cout << "Source string: " << source << std::endl;
#endif

#ifdef PARALLEL
  // Set number of threads for OpenMP
  omp_set_num_threads(static_cast<int>(std::thread::hardware_concurrency()));
#endif

  // Main call
#ifdef TRIVIAL
  auto result = ALGO::find_subpalindromes_trivial(source);
#else
  auto result = ALGO::find_subpalindromes_manaker(source);
#endif

  // Show results
  for (std::size_t pos = 0U; pos != source.size(); ++pos)
  {
    std::cout << "Position " << pos << std::endl;
    std::cout << "Odd-length subpalindromes count: " << result[pos].odd << std::endl;
    std::cout << "Even-length subpalindromes count: " << result[pos].even << std::endl;
    std::cout << std::endl;
  }

  return 0;
}
