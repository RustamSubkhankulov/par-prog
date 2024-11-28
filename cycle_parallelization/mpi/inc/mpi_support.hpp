#ifndef MPI_SUPPORT_HPP
#define MPI_SUPPORT_HPP

#include <source_location>

#include "mpi.h"

namespace MPI
{

constexpr int Msg_tag = 42;

class stopwatch
{
  using time_point_t = double;
  time_point_t start_;

public:
  /* Starts stopwatch. */
  void start()
  {
    start_ = MPI_Wtime();
  }

  /* Stopts stopwatch and returns elapsed time in seconds. */
  double stop() const
  {
    time_point_t stop = MPI_Wtime();
    return stop - start_;
  }
};

void exit_on_mpi_failure(
  const int res,
  const std::source_location& sloc = std::source_location::current());

class MPI_env
{
  MPI_env(int argc, const char* const* argv)
  {
    /* MPI env initialization. */
    int res = MPI_Init(&argc, &argv);
    MPI::exit_on_mpi_failure(res);
  }

  ~MPI_env()
  {
    /* MPI env finalization. */
    int res = MPI_Finalize();
    MPI::exit_on_mpi_failure(res);
  }
};

} /* namespace MPI */

#endif // MPI_SUPPORT_HPP
