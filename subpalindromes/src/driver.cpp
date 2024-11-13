#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include "subpalindromes.hpp"

static std::string getSource(const int argc, const char* const argv[]);
static void showHelp();

int main(const int argc, const char* const argv[])
{
  // Read source string, where we will search for the palindromes
  std::string source = getSource(argc, argv);

#ifdef VERBOSE
  std::cout << "Source string: " << source << std::endl;
#endif

  // Main call
#ifdef TRIVIAL
  auto result = ALGO::find_subpalindromes_trivial(source);
#else
  auto result = ALGO::find_subpalindromes_manaker(source);
#endif

  // Show results
  for (std::size_t pos = 0U; pos != result.size(); ++pos)
  {
    std::cout << "Position " << pos << std::endl;
    std::cout << "Odd-length subpalindromes count: " << result[pos].count_odd << std::endl;
    std::cout << "Even-length subpalindromes count: " << result[pos].count_even << std::endl;
    std::cout << std::endl;
  }

  return 0;
}

// Checks 'argc' and 'argv' and returns source string,
// where palindromes are to find
static std::string getSource(const int argc, const char* const argv[])
{
  if (argc != 2 || !std::strlen(argv[1]))
  {
    // Print help message
    showHelp();
    std::exit(EXIT_FAILURE);
  }

  return std::string(argv[1]);
}

// Prints help message with usage instructions.
static void showHelp()
{
  std::cout << "Usage: path-to-executable source-string" << std::endl;
}
