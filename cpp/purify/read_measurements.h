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
//! read in measurements from a vector of file names
utilities::vis_params read_measurements(const std::vector<std::string> &names);
//! read in and distribute measurements
utilities::vis_params
read_measurements(const std::vector<std::string> &names, sopt::mpi::Communicator const &comm);

}
}
#endif
