#include <iostream>

#include "mpi.h"
#include "mpi_support.hpp"

namespace MPI
{

void exit_on_mpi_failure(const int res, const std::source_location& sloc)
{
  if (res != MPI_SUCCESS)
  {
    std::cerr << "MPI failed at: " << sloc.file_name() << " " << sloc.function_name() << ":"
              << sloc.line() << std::endl;

    int eclass;
    char estring[MPI_MAX_ERROR_STRING + 1];

    MPI_Error_class(res, &eclass);
    MPI_Error_string(res, estring, NULL);

    std::cerr << "Error " << eclass << " : " << estring << std::endl;

    int initialized, finalized;
    MPI_Initialized(&initialized);
    MPI_Finalized(&finalized);

    if (initialized && !finalized)
    {
      MPI_Finalize();
    }

    std::exit(EXIT_FAILURE);
  }
}

} /* namespace MPI */
