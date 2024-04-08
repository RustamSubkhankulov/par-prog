#include <iostream>

#include "mpi.h"

static const unsigned MAX = 100U;

int main(int argc, char **argv)
{
  int rank, size;

  /* Инициализация среды MPI */
  MPI_Init(&argc, &argv);
  
  /* Общее число процессов в коммуникаторе */
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  /* Ранг процесса */
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  int n = (MAX - 1) / (size + 1);
  
  unsigned ibeg = rank * n + 1;
  unsigned iend = (rank + 1) * n;
  
  for(unsigned i = ibeg; i <= ((iend > MAX)? MAX : iend); i++) {
    std::cout << "Process " << rank << ", " << i << "^2 = " << i*i << "\n";
  }
    
  /* Остановка среды MPI */
  MPI_Finalize();
}