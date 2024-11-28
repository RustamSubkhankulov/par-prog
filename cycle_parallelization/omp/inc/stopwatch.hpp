#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

#include <chrono>

namespace UTILS
{

class stopwatch
{
  using time_point_t = decltype(std::chrono::steady_clock::now());
  time_point_t start_;

public:
  /* Starts stopwatch. */
  void start()
  {
    start_ = std::chrono::steady_clock::now();
  }

  /* Stops stopwatch and returns elapsed time in milliseconds. */
  double stop() const
  {
    time_point_t stop = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(stop - start_).count() / 1000.;
  }
};

} /* namespace UTILS */

#endif /* STOPWATCH_HPP */
