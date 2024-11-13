#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <algorithm>

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

#if defined(VERBOSE) && !defined(QUIET)
  std::cout << "Source string: " << source << std::endl;
#endif

#if defined(TRIVIAL) && !defined(QUIET)
  
  // Trivial algorithm, with results showing.
  auto result = ALGO::find_subpalindromes_trivial(source);

#elif defined(TRIVIAL)

  // Trivial algorithm, no results showing.
  ALGO::find_subpalindromes_trivial(source);

#elif !defined(QUIET)

  // Manaker's algorithm, with results showing.
  auto result = ALGO::find_subpalindromes_manaker(source);

#else

  // Manaker's algorithm, no results showing.
  ALGO::find_subpalindromes_manaker(source);
#endif

#ifndef QUIET
  // Show results
  for (const auto& pos : result)
  {
    std::cout << pos.odd << " " << pos.even << std::endl;
  }
#endif

  return 0;
}
