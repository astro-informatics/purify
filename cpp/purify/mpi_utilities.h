#ifndef PURIFY_MPI_UTILITIES_H
#define PURIFY_MPI_UTILITIES_H

#include "purify/config.h"
#include <vector>
#include <sopt/mpi/communicator.h>
#include "purify/utilities.h"

namespace purify {
namespace utilities {
#ifdef PURIFY_MPI
//! \brief Regroups visibilities data according to input groups
//! \details All data for group with smallest key comes first, then next smallest key, etc.
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
#else
void regroup(utilities::vis_params &, std::vector<t_int> const &) {}
vis_params scatter_visibilities(vis_params const &params, std::vector<t_int> const &,
                                sopt::Communicator const &) {
  return params;
}
#endif
}
} // namespace purify
#endif
