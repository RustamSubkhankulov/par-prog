#include <cmath>
#include <cassert>
#include <cstdlib>
#include <new>
#include <iostream>

#ifndef QUIET
#include <fstream>
#endif

#include "mpi.h"
#include "mpi_support.hpp"

namespace
{
/* Array dimensions. */
const int Isize = 40000;
const int Jsize = 40000;

using column = double[Jsize];

void main_process_prepare(double (*a)[Jsize])
{
  for (int i = 0; i < Isize; i++)
  {
    for (int j = 0; j < Jsize; j++)
    {
      a[i][j] = 10 * i + j;
    }
  }
}

void main_process_send(int cluster_size, int secondary_rank, const double (*array)[Jsize])
{
  int idx_start = cluster_size * (secondary_rank - 1);
  int idx_end   = idx_start + cluster_size;

  for (int idx = idx_start; idx < idx_end; ++idx)
  {
    int res =
      MPI_Send(&array[idx], Jsize, MPI_DOUBLE, secondary_rank, MPI::Msg_tag, MPI_COMM_WORLD);
    MPI::exit_on_mpi_failure(res);
  }
}

void main_process_recv(int cluster_size, int secondary_rank, double (*array)[Jsize])
{
  int idx_start = cluster_size * (secondary_rank - 1);
  int idx_end   = idx_start + cluster_size;

  for (int idx = idx_start; idx < idx_end; ++idx)
  {
    int res = MPI_Recv(
      &array[idx],
      Jsize,
      MPI_DOUBLE,
      secondary_rank,
      MPI::Msg_tag,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE);
    MPI::exit_on_mpi_failure(res);
  }
}

#ifndef QUIET
void main_process_print(double (*a)[Jsize])
{
  auto result_file = std::ofstream("result.txt");
  if (!result_file.is_open())
  {
    int res = MPI_Finalize();
    MPI::exit_on_mpi_failure(res);
  }

  for (int i = 0; i < Isize; i++)
  {
    for (int j = 0; j < Jsize; j++)
    {
      result_file << a[i][j] << ' ';
    }

    result_file << std::endl;
  }
}
#endif /* !QUIET */

void main_process(int comm_size)
{
  auto array = new (std::nothrow) column[Isize];
  if (array == nullptr)
  {
    int res = MPI_Finalize();
    MPI::exit_on_mpi_failure(res);
  }

  /* Preparation - fill array with some data. */
  main_process_prepare(array);

#ifdef TIMING
  MPI::stopwatch sw;
  sw.start();
#endif

  /* Cluster size - number of rows per process. */
  int cluster_size = Isize / (comm_size - 1);

  /* Send data for computations to the secondary processes. */
  for (int secondary_rank = 1; secondary_rank < comm_size; ++secondary_rank)
  {
    main_process_send(cluster_size, secondary_rank, array);
  }

  /* Receive computation results from the secondary processes. */
  for (int secondary_rank = 1; secondary_rank < comm_size; ++secondary_rank)
  {
    main_process_recv(cluster_size, secondary_rank, array);
  }

#ifdef TIMING
  std::clog << "Total elapsed: " << sw.stop() << " sec \n";
#endif

#ifndef QUIET
  main_process_print(array);
#endif
}

void secondary_process_recv(int cluster_size, double (*array)[Jsize])
{
  for (int idx = 0; idx < cluster_size; ++idx)
  {
    int res =
      MPI_Recv(&array[idx], Jsize, MPI_DOUBLE, 0, MPI::Msg_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI::exit_on_mpi_failure(res);
  }
}

void secondary_process_send(int cluster_size, const double (*array)[Jsize])
{
  for (int idx = 0; idx < cluster_size; ++idx)
  {
    int res = MPI_Send(&array[idx], Jsize, MPI_DOUBLE, 0, MPI::Msg_tag, MPI_COMM_WORLD);
    MPI::exit_on_mpi_failure(res);
  }
}

void secondary_process(int comm_size, int secondary_rank)
{
  /* Cluster size - number of rows per process. */
  int cluster_size = Isize / (comm_size - 1);

  /* Allocate memory only for the rows, assigned to the current process. */
  auto array = new (std::nothrow) column[cluster_size];
  if (array == nullptr)
  {
    int res = MPI_Finalize();
    MPI::exit_on_mpi_failure(res);
  }

  /* Receive data, prepared for the computations, from the main process. */
  secondary_process_recv(cluster_size, array);

  /* Compute. */
  /* some code */

  /* Send computation results back to the main process. */
  secondary_process_send(cluster_size, array);
}

} /* anonymous namespace */

int main(int argc, const char* const* argv)
{
  /* MPI env initialization. */
  MPI_env mpi_env;

  /* Total number of the processes in the communicator. */
  int comm_size;
  res = MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI::exit_on_mpi_failure(res);

  /* We support 2+ processes in the communicator. */
  if (comm_size < 2)
  {
    std::cerr << "Communicator size " << comm_size << " is too small. Terminating.\n";
    return EXIT_FAILURE;
  }

  /* Processes rank. */
  int rank;
  res = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI::exit_on_mpi_failure(res);

  if (rank == 0)
  {
    /* Main process for the zero rank. */
    main_process(comm_size);
  }
  else
  {
    /* Secondary process for all other ranks. */
    secondary_process(comm_size, rank);
  }

  /* Impiclit MPI env finalization via 'MPI_env' dtor. */
  return 0;
}