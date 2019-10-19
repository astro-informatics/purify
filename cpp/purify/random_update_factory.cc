#include "purify/random_update_factory.h"
#include <algorithm>
#include <random>

namespace purify {
namespace random_updater {
std::function<bool()> random_updater(const sopt::mpi::Communicator& comm, const t_int total,
                                     const t_int update_size,
                                     const std::shared_ptr<bool> update_pointer,
                                     const std::string& update_name) {
  if (update_size > comm.size())
    throw std::runtime_error(
        "Number of random updates cannot be greater than number of MPI processors in the "
        "communicator.");
  std::shared_ptr<std::vector<t_int>> ind = std::make_shared<std::vector<t_int>>(total, 0);
  for (t_int i = 0; i < comm.size(); i++) (*ind)[i] = i;
  std::random_device rng;
  std::shared_ptr<std::mt19937> urng = std::make_shared<std::mt19937>(rng());
  return [update_pointer, update_size, update_name, ind, comm, urng]() -> bool {
    if (comm.is_root()) {
      std::shuffle(std::begin(*ind), std::end(*ind), *urng);
    };
    *ind = comm.broadcast(*ind);
    *update_pointer = false;
    for (t_int i = 0; i < update_size; i++)
      if (comm.rank() == ind->at(i)) {
        SOPT_DEBUG("Process {} doing {}", comm.rank(), update_name);
        *update_pointer = true;
      }
    return *update_pointer;
  };
}
}  // namespace random_updater
}  // namespace purify
