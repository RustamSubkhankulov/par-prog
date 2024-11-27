#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>

#ifdef TIMING
#include "stopwatch.hpp"
#endif /* TIMING */

namespace
{
/* Array dimensions. */
const int Isize = 40000;
const int Jsize = 40000;
} // namespace

int main()
{
  using column = double[Jsize];
  auto a       = new column[Isize];

  /* Preparing - fill array with some data. */
  for (int i = 0; i < Isize; i++)
  {
    for (int j = 0; j < Jsize; j++)
    {
      a[i][j] = 10 * i + j;
    }
  }

#ifdef TIMING
  UTILS::stopwatch sw;
  sw.start();
#endif /* TIMING */

  /* Main computational cycle. */
  for (int i = 0; i < Isize; i++)
  {
    for (int j = 0; j < Jsize; j++)
    {
      a[i][j] = sin(2 * a[i][j]);
    }
  }

#ifdef TIMING
  std::clog << "Total elapsed: " << sw.stop() / 1000. << " sec \n";
#endif /* TIMING */

#ifndef QUIET
  auto result_file = std::ofstream("result.txt");
  assert(result_file.is_open());

  for (int i = 0; i < Isize; i++)
  {
    for (int j = 0; j < Jsize; j++)
    {
      result_file << a[i][j] << ' ';
    }

    result_file << std::endl;
  }
#endif /* !QUIET */

  delete[] a;
}
