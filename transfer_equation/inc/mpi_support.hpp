#ifndef MPI_SUPPORT_HPP
#define MPI_SUPPORT_HPP

void exit_on_mpi_failure(const int res, const char* file, const char* func, const int line);
#define EXIT_ON_MPI_FAILURE(RES) exit_on_mpi_failure(RES, __FILE__, __FUNCTION__, __LINE__)

void mpi_start_timer();
double mpi_stop_timer();

#endif // MPI_SUPPORT_HPP
