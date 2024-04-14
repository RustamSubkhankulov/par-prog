#include <cstdlib>
#include <iostream>
#include <new>
#include <cmath>

#include "mpi.h"
#include "mpi_support.hpp"

#include "comp_math.hpp"

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
    1e-3,     /* h        */
    5e-6,     /* tau      */
    10000008, /* x_points */
    100,      /* t_points */
    1e-2      /* a        */
  };

  try {

    comp_scheme.allocate();

  } catch (const std::bad_alloc& exc) {

    res = MPI_Finalize();
    EXIT_ON_MPI_FAILURE(res);
  }

  uint64_t x_points = comp_scheme.x_points();
  uint64_t t_points = comp_scheme.t_points();
  
  uint64_t x_points_per_proc = x_points / size;

  uint64_t x_idx_begin = x_points_per_proc * rank;
  uint64_t x_idx_end   = x_points_per_proc * (rank + 1);

  uint64_t compute_begin = (x_idx_begin == 0)? 
                            x_idx_begin + 1 : x_idx_begin;

  uint64_t compute_end = (x_idx_end == x_points)? 
                          x_idx_end - 1 : x_idx_end;

#ifdef DEBUG
  std::cout << "[" << rank << "]" << " x_idx_begin: "   << x_idx_begin
                                  << " x_idx_end: "     << x_idx_end
                                  << " compute_begin: " << compute_begin
                                  << " compute_end: "   << compute_end << "\n";
#endif

  /* u(0,x) = fi(x) */
  Comp_scheme::bound_func_type fi  = [](double arg) -> double { return 1000. * std::sin(arg); };
  comp_scheme.set_boundary_coord(x_idx_begin, x_idx_end, fi);

  /* u(t,0) = psi(t) */
  Comp_scheme::bound_func_type psi = [](double arg) { return 100. * arg; };

  if (rank == min_rank) {
    /* u(t,0) = u(t,x_points-1) = psi(t) */
    comp_scheme.set_boundary_time(0, psi);
  }

  if (rank == max_rank) {
    /* u(t,x_points-1) = psi(t) */
    comp_scheme.set_boundary_time(x_points-1, psi);
  }

  int lft_neigh = (rank == max_rank)? MPI_PROC_NULL : rank + 1;
  int rgt_neigh = (rank == min_rank)? MPI_PROC_NULL : rank - 1;

#ifdef DEBUG
  std::cout << "[" << rank << "]:" << " lft_neigh: " << ((lft_neigh == MPI_PROC_NULL)? 
                                      "MPI_PROC_NULL" : std::to_string(lft_neigh))

                                   << " rgt_neigh: " << ((rgt_neigh == MPI_PROC_NULL)? 
                                      "MPI_PROC_NULL" : std::to_string(rgt_neigh)) << "\n";
#endif

  if (rank == 0) {
    mpi_start_timer();
  }

  for (uint64_t t_idx = 0; t_idx < t_points-1; ++t_idx) {

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

  if (rank == 0) {
    std::cout << "Elapsed: " << mpi_stop_timer() << " sec \n";
  }

  if (rank == 0) {

    /* Receive calculated values from all other nodes */
    for (int node = 1; node < size; ++node) {

      uint64_t begin = x_points_per_proc * node;
      uint64_t end   = x_points_per_proc * (node + 1);

      for (uint64_t x_idx = begin; x_idx < end; ++x_idx) {

        double value_recv;
        res = MPI_Recv(&value_recv, 1, MPI_DOUBLE, node, Msg_tag, 
                                       MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        EXIT_ON_MPI_FAILURE(res);

        comp_scheme.set(x_idx, t_points-1, value_recv);
      }      
    }

  } else {

    /* Send calculated values to node №0 */
    for (uint64_t x_idx = x_idx_begin; x_idx < x_idx_end; ++x_idx) {

      double value_send = comp_scheme.get(x_idx, t_points-1);
      res = MPI_Send(&value_send, 1, MPI_DOUBLE, 0, Msg_tag, MPI_COMM_WORLD);
      EXIT_ON_MPI_FAILURE(res);
    }
  }

#ifdef PRINT
  if (rank == 0) {
    /* Print values */
    for (uint64_t x_idx = 0; x_idx < x_points; ++x_idx) {
      std::cout << "u[" << x_idx << "][" << comp_scheme.t_points()-1 << "]=" 
                  << comp_scheme.get(x_idx, comp_scheme.t_points()-1) << "\n";
    }
  }
#endif

  /* Free allocated arrays */
  comp_scheme.free();

  /* Остановка среды MPI */
  res = MPI_Finalize();
  EXIT_ON_MPI_FAILURE(res);

  return 0;
}