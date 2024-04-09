#include <cstdlib>
#include <iostream>

#include "mpi.h"
#include "mpi_support.hpp"

static const unsigned MAX = 100U;

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
  
  std::cout << "Process " << rank << " Comm size " << size << std::endl;

  int n = (MAX - 1) / (size + 1);
  
  unsigned ibeg = rank * n + 1;
  unsigned iend = (rank + 1) * n;
  
  for(unsigned i = ibeg; i <= ((iend > MAX)? MAX : iend); i++) {
    std::cout << "Process " << rank << ", " << i << "^2 = " << i*i << "\n";
  }
    
  std::cout << "Process " << rank << " finished.\n";

  /* Остановка среды MPI */
  res = MPI_Finalize();
  EXIT_ON_MPI_FAILURE(res);

  return 0;
}