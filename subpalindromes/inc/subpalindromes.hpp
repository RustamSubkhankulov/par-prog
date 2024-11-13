#ifndef SUBPALINDROMES_HPP
#define SUBPALINDROMES_HPP

#include <string>
#include <utility>
#include <vector>
#include <cstdint>

namespace ALGO
{

struct subpali_info
{
  std::vector<std::size_t> odd;
  std::vector<std::size_t> even;

  void reserve(std::size_t new_cap)
  {
    odd.reserve(new_cap);
    even.reserve(new_cap);
  }
};

template <typename CharT>
subpali_info find_subpalindromes_trivial(const std::basic_string<CharT>& source)
{
  auto len = source.size();

  subpali_info results;
  results.odd  = std::vector<std::size_t>(len, 1);
  results.even = std::vector<std::size_t>(len, 0);

  // Odd-length subpalindromes
  for (std::size_t idx = 0U; idx != len; ++idx)
  {
    auto& count_odd = results.odd[idx];

    while (idx >= count_odd && idx + count_odd < len
           && source[idx - count_odd] == source[idx + count_odd])
    {
      ++count_odd;
    }
  }

  // Even-length subpalindromes
  for (std::size_t idx = 0U; idx != len; ++idx)
  {
    auto& count_even = results.even[idx];

    while (idx >= count_even + 1U && idx + count_even < len
           && source[idx - count_even - 1U] == source[idx + count_even])
    {
      ++count_even;
    }
  }

  return results;
}

template <typename CharT>
subpali_info find_subpalindromes_manaker(const std::basic_string<CharT>& source)
{
  subpali_info results;
  results.reserve(source.size());

  auto num = static_cast<intmax_t>(source.size());

  // Odd-length subpalindromes
  {
    // Left and right borders of the rightmost subpalindrome
    intmax_t l = 0, r = -1;

    for (intmax_t idx = 0; idx < num; ++idx)
    {
      intmax_t k =
        (idx > r) ? 1 : std::min(static_cast<intmax_t>(results.odd[l + r - idx]), r - idx + 1);

      while (idx + k < num && idx >= k && source[idx + k] == source[idx - k])
      {
        ++k;
      }

      results.odd[idx] = k;
      if (idx + k - 1 > r)
      {
        // Update left and right borders
        l = idx - k + 1;
        r = idx + k - 1;
      }
    }
  }

  // Even-length subpalindromes
  {
    // Left and right borders of the rightmost subpalindrome
    intmax_t l = 0, r = -1;

    for (intmax_t idx = 0; idx < num; ++idx)
    {
      intmax_t k =
        (idx > r) ? 0 : std::min(static_cast<intmax_t>(results.even[l + r - idx + 1]), r - idx + 1);

      while (idx + k < num && idx >= k + 1 && source[idx + k] == source[idx - k - 1])
      {
        ++k;
      }

      results.even[idx] = k;
      if (idx + k - 1 > r)
      {
        // Update left and right borders
        l = idx - k;
        r = idx + k - 1;
      }
    }
  }

  return results;
}

} // namespace ALGO

#endif /* SUBPALINDROMES_HPP */
