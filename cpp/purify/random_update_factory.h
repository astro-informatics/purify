#ifndef RANDOM_UPDATE_FACTORY_H
#define RANDOM_UPDATE_FACTORY_H

#include "purify/config.h"

#include "purify/types.h"
#include "purify/convergence_factory.h"
#include "purify/logging.h"
#include "purify/utilities.h"

#ifdef PURIFY_MPI
#include "purify/mpi_utilities.h"
#include <sopt/mpi/communicator.h>
#endif
namespace purify {
namespace random_updater {
//! Creates lambda that controls random updates
std::function<bool()> random_updater(const sopt::mpi::Communicator& comm, const t_int total,
                                     const t_int update_size,
                                     const std::shared_ptr<bool> update_pointer,
                                     const std::string& update_name);
}  // namespace random_updater
}  // namespace purify
#endif
