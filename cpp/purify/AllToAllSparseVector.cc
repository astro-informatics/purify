#include "purify/AllToAllSparseVector.h"

namespace purify {

std::vector<t_int> all_to_all_send_sizes(const std::vector<t_int> &recv_sizes,
                                         const sopt::mpi::Communicator &comm) {
  std::vector<t_int> send_sizes(recv_sizes.size(), 0);
  for (t_int i = 0; i < comm.size(); i++) {
    if (i == comm.rank())
      send_sizes = comm.gather<t_int>(recv_sizes[i], i);
    else
      comm.gather<t_int>(recv_sizes[i], i);
  }
  return send_sizes;
}
}  // namespace purify
