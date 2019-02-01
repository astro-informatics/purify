#include "purify/AllToAllSparseVector.h"

namespace purify {

std::vector<t_int> all_to_all_recv_sizes(const std::vector<t_int> &local_indices, const t_int nodes,
                                         const t_int N) {
  std::vector<t_int> recv_sizes;
  t_int group = 0;
  t_int count = 0;

  for (const auto &index : local_indices) {
    const t_int index_group = std::floor(static_cast<t_real>(index) / static_cast<t_real>(N));
    if (index_group < group)
      throw std::runtime_error("local indices are out of order for columns of gridding matrix");
    if (group != index_group) {
      recv_sizes.push_back(count);
      group++;
      while (group < index_group) {
        recv_sizes.push_back(0);
        group++;
      }
      count = 1;
    } else
      count++;
  }
  recv_sizes.push_back(count);
  group++;
  while (group < nodes) {
    recv_sizes.push_back(0);
    group++;
  }
  assert(group == nodes);
  assert(local_indices.size() == std::accumulate(recv_sizes.begin(), recv_sizes.end(), 0));
  return recv_sizes;
}

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
