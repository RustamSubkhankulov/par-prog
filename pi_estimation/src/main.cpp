#include <cstdlib>
#include <iomanip>
#include <iostream>

#include "mpi.h"
#include "mpi_support.hpp"

static const unsigned Msg_tag = 5U;

static const unsigned long long Nbin = 10000000000U;
static const long double Step = 1. / Nbin;

long double mpi_get_pi_estimation(int size) {

  long double sum = 0;

  for (unsigned proc_idx = 0; proc_idx < (unsigned)size-1; ++proc_idx) {

    long double payload;
    int res = MPI_Recv(&payload, 1, MPI_LONG_DOUBLE, MPI_ANY_SOURCE, Msg_tag, 
                                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    EXIT_ON_MPI_FAILURE(res);

    sum += payload;
  }

  return sum / Nbin;
}

void main_process(int size) {

  double start = MPI_Wtime();
  std::cout << "Started at: " << start << std::endl;

  long double pi = mpi_get_pi_estimation(size);
  auto prev_prec = std::cout.precision();
  std::cout << "Pi est: " << std::setprecision(8) << pi 
                          << std::setprecision(prev_prec) << std::endl;

  double finish = MPI_Wtime();
  std::cout << "Finished at: " << finish << std::endl;

  std::cout << "Elapsed: " << finish - start << std::endl;
}

void secondary_process(int size, int rank) {

  long double local_sum = 0;
  unsigned long long bins_per_proc = Nbin / (size - 1);

  long double x = ((rank - 1) * bins_per_proc - 0.5) * Step;

  for (unsigned long long bin_idx = 0; bin_idx < bins_per_proc; ++bin_idx) {

    x += Step;
    local_sum += 4. / (1. + x*x);
  }

  int res = MPI_Send(&local_sum, 1, MPI_LONG_DOUBLE, 0, Msg_tag, MPI_COMM_WORLD);
  EXIT_ON_MPI_FAILURE(res);
}

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
  
  if (rank == 0) {
    main_process(size);
  } else {
    secondary_process(size, rank);
  }

  /* Остановка среды MPI */
  res = MPI_Finalize();
  EXIT_ON_MPI_FAILURE(res);

  return 0;
}
