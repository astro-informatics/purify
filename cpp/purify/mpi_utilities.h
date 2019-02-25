#ifndef PURIFY_MPI_UTILITIES_H
#define PURIFY_MPI_UTILITIES_H

#include "purify/config.h"
#include <vector>
#include "purify/utilities.h"
#include <sopt/linear_transform.h>

#ifdef PURIFY_MPI
#include <sopt/mpi/communicator.h>
#endif
namespace purify {
namespace utilities {
#ifdef PURIFY_MPI
//! \brief Regroups visibilities data according to input groups
//! \details All data for group with smallest key comes first, then next
//! smallest key, etc.
void regroup(utilities::vis_params &params, std::vector<t_int> const &groups,
             const t_int max_groups);
void regroup(vis_params &uv_params, std::vector<t_int> &image_index,
             std::vector<t_int> const &groups_, const t_int max_groups);
//! \brief regroup and distributes data
vis_params regroup_and_scatter(vis_params const &params, std::vector<t_int> const &groups,
                               sopt::mpi::Communicator const &comm);
//! \brief regroup and distributes data to and from all nodes
std::tuple<vis_params, std::vector<t_int>> regroup_and_all_to_all(
    vis_params const &params, const std::vector<t_int> &image_index,
    std::vector<t_int> const &groups, sopt::mpi::Communicator const &comm);
//! \brief without image index
vis_params regroup_and_all_to_all(vis_params const &params, std::vector<t_int> const &groups,
                                  sopt::mpi::Communicator const &comm);
//! \brief distribute data according to input order
//! \brief Can be called by any proc
vis_params scatter_visibilities(vis_params const &params, std::vector<t_int> const &sizes,
                                sopt::mpi::Communicator const &comm);

//! \brief Receives data scattered from root
//! \details Should only be called by non-root processes
vis_params scatter_visibilities(sopt::mpi::Communicator const &comm);
//! \brief Sends and recieves data between all nodes
//! \details Should be called by all procs
vis_params all_to_all_visibilities(vis_params const &params, std::vector<t_int> const &sizes,
                                   sopt::mpi::Communicator const &comm);
//! \brief distribute from root to all comm
utilities::vis_params distribute_params(utilities::vis_params const &params,
                                        sopt::mpi::Communicator const &comm);
//! \brief calculate cell size when visibilies are distributed
utilities::vis_params set_cell_size(const sopt::mpi::Communicator &comm,
                                    utilities::vis_params const &uv_vis, const t_real &cell_x,
                                    const t_real &cell_y);
//! \brief distribute data, sort into w-stacks using MPI, then distribute the stacks
utilities::vis_params w_stacking(utilities::vis_params const &params,
                                 sopt::mpi::Communicator const &comm, const t_int iters,
                                 const std::function<t_real(t_real)> &cost);
//! \brief distribute data, sort into w-stacks using MPI, then distribute the stacks for all to all
//! operator
std::tuple<utilities::vis_params, std::vector<t_int>> w_stacking_with_all_to_all(
    utilities::vis_params const &params, const t_real du, const t_int min_support,
    const t_int max_support, sopt::mpi::Communicator const &comm, const t_int iters,
    const std::function<t_real(t_real)> &cost);
#endif
//! \brief Calculate step size using MPI (does not include factor of 1e-3)
//! \param[in] vis: Vector of measurement data
//! \param[in] measurements: Shared pointer to measurement linear operator
//! \param[in] wavelets: Shared pointer to SARA wavelet linear operator
//! \param[in] sara_size: Size of sara dictionary of MPI node.
//! \details Please use this function to calculate the step size for PADMM, Forward Backward, etc.
//! Especially when using MPI.
//! \note There is an issue with using the adjoint wavelet transform of the SARA basis.
//! When there are more nodes than wavelets, there will be nodes with an empty Vector<T>
//! of wavelet coefficients. This can cause some functions to break, like .maxCoeff().
template <class T>
t_real step_size(T const &vis, const std::shared_ptr<sopt::LinearTransform<T> const> &measurements,
                 const std::shared_ptr<sopt::LinearTransform<T> const> &wavelets,
                 const t_uint sara_size) {
  // measurement operator may use different number of nodes than wavelet operator
  // so needs to be done separately
  const T dimage = measurements->adjoint() * vis;
  return (sara_size > 0) ? (wavelets->adjoint() * dimage).cwiseAbs().maxCoeff() : 0.;
};
}  // namespace utilities
}  // namespace purify
#endif
