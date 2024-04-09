#include <cstdlib>
#include <iostream>

#include "mpi.h"
#include "mpi_support.hpp"

static const unsigned Msg_tag = 5U;

int main(int argc, char **argv)
{
  int res, rank, size;

  /* Инициализация среды MPI */
  res = MPI_Init(&argc, &argv);
  EXIT_ON_MPI_FAILURE(res);

  /* Общее число процессов в коммуникаторе */
  res = MPI_Comm_size(MPI_COMM_WORLD, &size);
  EXIT_ON_MPI_FAILURE(res);
  
  /* Ранг процесса */
  res = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  EXIT_ON_MPI_FAILURE(res);
  
  /* do smth */

  /* Остановка среды MPI */
  res = MPI_Finalize();
  EXIT_ON_MPI_FAILURE(res);

  return 0;
}