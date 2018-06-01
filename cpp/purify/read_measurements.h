#ifndef READ_MEASUREMENTS_H
#define READ_MEASUREMENTS_H
#include "purify/config.h"

#include "purify/types.h"

#include "purify/utilities.h"
#include "purify/distribute.h"
#include <sopt/mpi/communicator.h>


namespace purify {
  namespace read_measurements {
    enum class format {vis, uvfits, ms};
//! read in signle measurement file
utilities::vis_params read_measurements(const std::string &name, const bool w_term = false, const stokes pol = stokes::I);
//! read in measurements from a vector of file names
utilities::vis_params read_measurements(const std::vector<std::string> &names, const bool w_term = false,
    const stokes pol = stokes::I);
#ifdef PURIFY_MPI
//! read in and distribute measurements
utilities::vis_params read_measurements(const std::string &name, sopt::mpi::Communicator const & comm,
    const distribute::plan plan = distribute::plan::radial, const bool w_term = false, const stokes pol = stokes::I);
//! read in and distribute mutliple measurements
utilities::vis_params
read_measurements(const std::vector<std::string> &names, sopt::mpi::Communicator const &comm,
    const distribute::plan plan = distribute::plan::radial, const bool w_term = false, const stokes pol = stokes::I);
#endif
}
}
#endif
