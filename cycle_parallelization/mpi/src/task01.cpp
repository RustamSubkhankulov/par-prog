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
const int Isize = 24000;
const int Jsize = 24000;

using row = double[Jsize];

void main_process_prepare(row* array)
{
  for (int i = 0; i < Isize; i++)
  {
    for (int j = 0; j < Jsize; j++)
    {
      array[i][j] = 10 * i + j;
    }
  }
}

void process_compute(int cluster_size, row* cluster)
{
  for (int i = 0; i < cluster_size; i++)
  {
    for (int j = 0; j < Jsize; j++)
    {
      cluster[i][j] = sin(2 * cluster[i][j]);
    }
  }
}

void main_process_send(int cluster_size, int rank, const row* cluster)
{
  int res = MPI_Send(
    &cluster[cluster_size * rank],
    Jsize * cluster_size,
    MPI_DOUBLE,
    rank,
    MPI::Msg_tag,
    MPI_COMM_WORLD);
  MPI::exit_on_mpi_failure(res);
}

void main_process_recv(int cluster_size, int rank, row* cluster)
{
  int res = MPI_Recv(
    &cluster[cluster_size * rank],
    Jsize * cluster_size,
    MPI_DOUBLE,
    rank,
    MPI::Msg_tag,
    MPI_COMM_WORLD,
    MPI_STATUS_IGNORE);
  MPI::exit_on_mpi_failure(res);
}

#ifndef QUIET
void main_process_print(row* array)
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
  auto array = new row[Isize];

  /* Preparation - fill array with some data. */
  main_process_prepare(array);

#ifdef TIMING
  MPI::stopwatch sw;
  sw.start();
#endif

  /* Cluster size - number of rows per process. */
  int cluster_size = Isize / comm_size;

  /* Send data for computations to the secondary processes. */
  for (int rank = 1; rank < comm_size; ++rank)
  {
    main_process_send(cluster_size, rank, array);
  }

  /* Main computations. */
  process_compute(cluster_size, array);

  /* Receive computation results from the secondary processes. */
  for (int rank = 1; rank < comm_size; ++rank)
  {
    main_process_recv(cluster_size, rank, array);
  }

#ifdef TIMING
  std::clog << "Total elapsed: " << sw.stop() << " sec \n";
#endif

#ifndef QUIET
  main_process_print(array);
#endif
}

void secondary_process_recv(int cluster_size, row* cluster)
{
  int res = MPI_Recv(
    cluster,
    Jsize * cluster_size,
    MPI_DOUBLE,
    0,
    MPI::Msg_tag,
    MPI_COMM_WORLD,
    MPI_STATUS_IGNORE);
  MPI::exit_on_mpi_failure(res);
}

void secondary_process_send(int cluster_size, const row* cluster)
{
  int res = MPI_Send(cluster, Jsize * cluster_size, MPI_DOUBLE, 0, MPI::Msg_tag, MPI_COMM_WORLD);
  MPI::exit_on_mpi_failure(res);
}

void secondary_process(int comm_size)
{
  /* Cluster size - number of rows per process. */
  int cluster_size = Isize / comm_size;

  /* Allocate memory only for the rows, assigned to the current process. */
  auto cluster = new row[cluster_size];

  /* Receive data, prepared for the computations, from the main process. */
  secondary_process_recv(cluster_size, cluster);

  /* Main computations. */
  process_compute(cluster_size, cluster);

  /* Send computation results back to the main process. */
  secondary_process_send(cluster_size, cluster);
}

} /* anonymous namespace */

int main(int argc, char** argv)
{
  /* MPI env initialization. */
  MPI::MPI_env mpi_env(argc, argv);

  /* Total number of the processes in the communicator. */
  int comm_size;
  int res = MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI::exit_on_mpi_failure(res);

  /* Processes rank. */
  int rank;
  res = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI::exit_on_mpi_failure(res);

  /* Number of rows must be divisible by number of secondary processes. */
  if (Isize % comm_size != 0)
  {
    if (rank == 0)
    {
      std::cerr << "Total number of rows " << Isize << " cannot be divided among " << comm_size
                << "nodes.\n";
    }
    return EXIT_FAILURE;
  }

  if (rank == 0)
  {
    /* Main process for the zero rank. */
    main_process(comm_size);
  }
  else
  {
    /* Secondary process for all other ranks. */
    secondary_process(comm_size);
  }

  /* Impiclit MPI env finalization via 'MPI_env' dtor. */
  return 0;
}
