#include <cstdlib>
#include <iostream>

#include "mpi.h"
#include "mpi_support.hpp"

static const unsigned Msg_tag = 5U;

void first_process() {

  std::cout << "Sending " << NTIMES << " messages there and back again\n";

  double start = MPI_Wtime();
  std::cout << "Started at: " << start << std::endl;

  for (int msg_cnt = 0; msg_cnt < NTIMES; ++msg_cnt) {
    
    int res = MPI_Send(&msg_cnt, 1, MPI_INT, 1, Msg_tag, MPI_COMM_WORLD);
    EXIT_ON_MPI_FAILURE(res);

    int payload;

    res = MPI_Recv(&payload, 1, MPI_INT, 1, Msg_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    EXIT_ON_MPI_FAILURE(res);
  }

  double finish = MPI_Wtime();
  std::cout << "Finished at: " << finish << std::endl;

  std::cout << "Delay per transmission: " << (finish - start) / (2 * NTIMES) << std::endl;
}

void second_process() {

  for (int msg_cnt = 0; msg_cnt < NTIMES; ++msg_cnt) {
    
    int payload;

    int res = MPI_Recv(&payload, 1, MPI_INT, 0, Msg_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    EXIT_ON_MPI_FAILURE(res);

    res = MPI_Send(&payload, 1, MPI_INT, 0, Msg_tag, MPI_COMM_WORLD);
    EXIT_ON_MPI_FAILURE(res);
  }
}

int main(int argc, char **argv)
{
  int res, rank;

  /* Инициализация среды MPI */
  res = MPI_Init(&argc, &argv);
  EXIT_ON_MPI_FAILURE(res);
  
  /* Ранг процесса */
  res = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  EXIT_ON_MPI_FAILURE(res);
  
  if (rank == 0) {
    first_process();
  } else {
    second_process();
  }

  /* Остановка среды MPI */
  res = MPI_Finalize();
  EXIT_ON_MPI_FAILURE(res);

  return 0;
}