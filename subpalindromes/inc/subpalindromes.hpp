#ifndef SUBPALINDROMES_HPP
#define SUBPALINDROMES_HPP

#include <string>
#include <utility>
#include <vector>

namespace ALGO
{

struct subpali_info
{
  std::size_t count_odd  = 1U;
  std::size_t count_even = 0U;
};

template <typename CharT>
std::vector<subpali_info> find_subpalindromes_trivial(const std::basic_string<CharT>& source)
{
  std::size_t len = source.size();
  std::vector<subpali_info> results(len);

  for (std::size_t idx = 0U; idx != len; ++idx)
  {
    auto& count_odd = results[idx].count_odd;

    while (idx >= count_odd && idx + count_odd < len
           && source[idx - count_odd] == source[idx + count_odd])
    {
      ++count_odd;
    }
  }

  for (std::size_t idx = 0U; idx != len; ++idx)
  {
    auto& count_even = results[idx].count_even;

    while (idx >= count_even + 1U && idx + count_even < len
           && source[idx - count_even + 1U] == source[idx + count_even])
    {
      ++count_even;
    }
  }

  return results;
}

template <typename CharT>
std::vector<subpali_info> find_subpalindromes_manaker(const std::basic_string<CharT>& source)
{
  return std::vector<subpali_info>{};
}

} // namespace ALGO

#endif /* SUBPALINDROMES_HPP */
