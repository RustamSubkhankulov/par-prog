#include <iostream>

#include "mpi.h"
#include "mpi_support.hpp"

void exit_on_mpi_failure(const int res, const char* file, const char* func, const int line) {

  if (res != MPI_SUCCESS) {

    std::cerr << "MPI failed at: " << file << " " << func << ":" << line << std::endl;
    
    int eclass;
    char estring[MPI_MAX_ERROR_STRING];

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