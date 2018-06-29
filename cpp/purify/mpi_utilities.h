#ifndef PURIFY_MPI_UTILITIES_H
#define PURIFY_MPI_UTILITIES_H

#include "purify/config.h"
#include <vector>
#include <sopt/mpi/communicator.h>
#include <sopt/linear_transform.h>
#include "purify/utilities.h"

namespace purify {
namespace utilities {
#ifdef PURIFY_MPI
//! \brief Regroups visibilities data according to input groups
//! \details All data for group with smallest key comes first, then next
//! smallest key, etc.
void regroup(utilities::vis_params &params, std::vector<t_int> const &groups);
//! \brief regroup and distributes data
vis_params regroup_and_scatter(vis_params const &params, std::vector<t_int> const &groups,
                               sopt::mpi::Communicator const &comm);
//! \brief distribute data according to input order
//! \brief Can be called by any proc
vis_params scatter_visibilities(vis_params const &params, std::vector<t_int> const &sizes,
                                sopt::mpi::Communicator const &comm);

//! \brief Receives data scattered from root
//! \details Should only be called by non-root processes
vis_params scatter_visibilities(sopt::mpi::Communicator const &comm);

//! \brief distribute from root to all comm
utilities::vis_params
distribute_params(utilities::vis_params const &params, sopt::mpi::Communicator const &comm);
//! \brief calculate cell size when visibilies are distributed
utilities::vis_params set_cell_size(const sopt::mpi::Communicator &comm,
                                    utilities::vis_params const &uv_vis, const t_real &cell_x,
                                    const t_real &cell_y);
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
t_real step_size(T const & vis,
    const std::shared_ptr<sopt::LinearTransform<T> const> &measurements,
    const std::shared_ptr<sopt::LinearTransform<T> const> &wavelets,
    const t_uint sara_size){
  //measurement operator may use different number of nodes than wavelet operator
  //so needs to be done separately
    const T dimage = measurements->adjoint() * vis;
      return (sara_size > 0) ?
      (wavelets->adjoint() * dimage).cwiseAbs().maxCoeff(): 
      0.;

};
}
} // namespace purify
#endif
