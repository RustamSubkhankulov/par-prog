#include <iostream>

#include "mpi.h"
#include "mpi_support.hpp"

static double timer_start;

void mpi_start_timer() {

  timer_start = MPI_Wtime();
  std::clog << "MPI timer started at " << timer_start << std::endl;
}

double mpi_stop_timer() {

  double timer_end = MPI_Wtime();
  std::clog  << "MPI timer stopped at " << timer_end << std::endl;

  return timer_end - timer_start;
}

void exit_on_mpi_failure(const int res, const char* file, const char* func, const int line) {

  if (res != MPI_SUCCESS) {

    std::cerr << "MPI failed at: " << file << " " << func << ":" << line << std::endl;
    
    int eclass;
    char estring[MPI_MAX_ERROR_STRING+1];

    MPI_Error_class(res, &eclass);
    MPI_Error_string(res, estring, NULL);

    std::cerr << "Error " << eclass << " : " << estring << std::endl;

    int initialized, finalized;
    MPI_Initialized(&initialized);
    MPI_Finalized(&finalized);

    if (initialized && !finalized) {
      MPI_Finalize();
    }

    std::exit(EXIT_FAILURE);
  }
}