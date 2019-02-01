#ifndef PURIFY_DISTRIBUTE_H
#define PURIFY_DISTRIBUTE_H
#include "purify/config.h"
#include <iostream>
#include <stdio.h>
#include <string>
#ifdef PURIFY_MPI
#include <sopt/mpi/communicator.h>
#endif
#include "purify/types.h"
#include "purify/logging.h"
#include "purify/utilities.h"

namespace purify {
namespace distribute {
enum class plan { none, equal, radial, w_term };
//! Distribute visiblities into groups
std::vector<t_int> distribute_measurements(Vector<t_real> const &u, Vector<t_real> const &v,
                                           Vector<t_real> const &w, t_int const number_of_nodes,
                                           distribute::plan const distribution_plan = plan::equal,
                                           t_int const &grid_size = 128);

#ifdef PURIFY_MPI
inline std::vector<t_int> distribute_measurements(
    utilities::vis_params const &params, sopt::mpi::Communicator const &comm,
    distribute::plan const distribution_plan = plan::equal, t_int const &grid_size = 128) {
  return distribute_measurements(params.u, params.v, params.w, comm.size(), distribution_plan,
                                 grid_size);
}
#endif
//! patition w terms using k-means
std::tuple<std::vector<t_int>, std::vector<t_real>> kmeans_algo(
    const Vector<t_real> &w, const t_int number_of_nodes, const t_int iters,
    const std::function<t_real(t_real)> &cost = [](t_real x) { return x * x; });
#ifdef PURIFY_MPI
//! patition w terms using k-means over MPI
//! Details returns a tuple (indices for group, centre mean for each group)
std::tuple<std::vector<t_int>, std::vector<t_real>> kmeans_algo(
    const Vector<t_real> &w, const t_int number_of_nodes, const t_int iters,
    sopt::mpi::Communicator const &comm,
    const std::function<t_real(t_real)> &cost = [](t_real x) { return x * x; });
#endif
//! Distribute visibilities into nodes in order of w terms (useful for w-stacking)
Vector<t_int> w_distribution(Vector<t_real> const &u, const Vector<t_real> &v,
                             const Vector<t_real> &w);
Vector<t_int> w_distribution(Vector<t_real> const &w);
//! Distribute visiblities into nodes in order of distance from the centre
Vector<t_int> distance_distribution(Vector<t_real> const &u, Vector<t_real> const &v);
//! Distribute the visiblities into nodes in order of density
Vector<t_int> equal_distribution(Vector<t_real> const &u, Vector<t_real> const &v,
                                 t_int const &grid_size);
}  // namespace distribute
}  // namespace purify
#endif
