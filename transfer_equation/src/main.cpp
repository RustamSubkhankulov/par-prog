#include <cstdlib>
#include <iostream>
#include <new>
#include <cmath>

#include "mpi.h"
#include "mpi_support.hpp"

#include "comp_math.cpp"

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

  int min_rank = 0;
  int max_rank = size - 1;

  /* Ранг процесса */
  res = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  EXIT_ON_MPI_FAILURE(res);
  
  Comp_scheme comp_scheme{
    1e-3,   /* h        */
    5e-6,   /* tau      */
    100000, /* x_points */
    20000,  /* t_points */
    1e-2    /* a        */
  };

  try {

    comp_scheme.allocate();

  } catch (const std::bad_alloc& exc) {

    res = MPI_Finalize();
    EXIT_ON_MPI_FAILURE(res);
  }

  uint64_t x_points = comp_scheme.x_points();
  uint64_t x_points_per_proc = x_points / size;

  uint64_t x_idx_begin = x_points_per_proc * rank;
  uint64_t x_idx_end   = x_points_per_proc * (rank + 1);

  uint64_t compute_begin = (x_idx_begin == 0)? 
                            x_idx_begin + 1 : x_idx_begin;

  uint64_t compute_end = (x_idx_end == x_points)? 
                          x_idx_end - 1 : x_idx_end;

  std::cout << "[" << rank << "]" << " x_idx_begin: "   << x_idx_begin
                                  << " x_idx_end: "     << x_idx_end
                                  << " compute_begin: " << compute_begin
                                  << " compute_end: "   << compute_end << "\n";

  /* u(0,x) = fi(x) */
  Comp_scheme::bound_func_type fi  = (double(*)(double))&std::sin;

  for (uint64_t x_idx = x_idx_begin; x_idx < x_idx_end; ++x_idx) {
    comp_scheme.set(x_idx, 0, fi(x_idx));
  }

  uint64_t t_points = comp_scheme.t_points();  

  if (rank == 0) {

    /* u(t,0) = psi(t) */
    double tau = comp_scheme.tau();

    Comp_scheme::bound_func_type psi = 
      [tau](uint64_t t_idx) { return t_idx * tau; };

    for (uint64_t t_idx = 0; t_idx < t_points; ++t_idx) {

      comp_scheme.set(0, t_idx, psi(t_idx));
    }
  }

  int lft_neigh = (rank == max_rank)? MPI_PROC_NULL : rank + 1;
  int rgt_neigh = (rank == min_rank)? MPI_PROC_NULL : rank - 1;

  std::cout << "[" << rank << "]:" << " lft_neigh: " << ((lft_neigh == MPI_PROC_NULL)? 
                                      "MPI_PROC_NULL" : std::to_string(lft_neigh))

                                   << " rgt_neigh: " << ((rgt_neigh == MPI_PROC_NULL)? 
                                      "MPI_PROC_NULL" : std::to_string(rgt_neigh)) << "\n";

  for (uint64_t t_idx = 1; t_idx < t_points; ++t_idx) {

    /* Send points from previous iteration to neighbours */

    double right_send = comp_scheme.get(x_idx_begin, t_idx-1);
    res = MPI_Send(&right_send, 1, MPI_DOUBLE, rgt_neigh, Msg_tag, MPI_COMM_WORLD);
    EXIT_ON_MPI_FAILURE(res);

    double left_send = comp_scheme.get(x_idx_end-1, t_idx-1);
    res = MPI_Send(&left_send, 1, MPI_DOUBLE, lft_neigh, Msg_tag, MPI_COMM_WORLD);
    EXIT_ON_MPI_FAILURE(res);

    /* Receive points from previous iteration from neighbours */

    double right_recv = 0;
    res = MPI_Recv(&right_recv, 1, MPI_DOUBLE, rgt_neigh, Msg_tag, 
                                   MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    EXIT_ON_MPI_FAILURE(res);

    double left_recv = 0;
    res = MPI_Recv(&left_recv, 1, MPI_DOUBLE, lft_neigh, Msg_tag, 
                                  MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    EXIT_ON_MPI_FAILURE(res);

    /* Fill in neccessary previous iteration points for computing own chunk */

    if (x_idx_begin != 0) {
      comp_scheme.set(x_idx_begin - 1, t_idx - 1, right_recv);
    }

    if (x_idx_end != x_points) {
      comp_scheme.set(x_idx_end, t_idx - 1, left_recv);
    }

    /* Compute chunk */
    comp_scheme.compute_range(compute_begin, compute_end, t_idx);
  }

  /* Wait for all of the chunks to be calculated */
  MPI_Barrier(MPI_COMM_WORLD);

  /* Free allocated arrays */
  comp_scheme.free();

  /* Остановка среды MPI */
  res = MPI_Finalize();
  EXIT_ON_MPI_FAILURE(res);

  return 0;
}