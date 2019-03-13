#ifndef PURIFY_DISTRIBUTE_ALLTOALL_OPERATOR_H
#define PURIFY_DISTRIBUTE_ALLTOALL_OPERATOR_H

#include "purify/config.h"
#ifdef PURIFY_MPI
#include "purify/types.h"
#include <numeric>
#include "purify/IndexMapping.h"
#include "sopt/mpi/communicator.h"
namespace purify {
//! Finds sizes to be recieved from each node for degridding
//! \param[in] local_indices: indices that will be received by this process
//! \param[in] N: Size of the ftgrid on each node
//! \param[in] comm: Communicator over which to distribute the vector
template <class STORAGE_INDEX_TYPE>
std::vector<t_int> all_to_all_recv_sizes(const std::vector<STORAGE_INDEX_TYPE> &local_indices,
                                         const t_int nodes, const t_int N) {
  std::vector<t_int> recv_sizes;
  t_int group = 0;
  t_int count = 0;
  if (static_cast<STORAGE_INDEX_TYPE>(N) * static_cast<STORAGE_INDEX_TYPE>(nodes) < 0)
    throw std::runtime_error(
        "Total number of pixels across FFT grids is less than 0. Please use index mapper with 64 "
        "bit int "
        "data types, i.e. long long int.");

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
//! Finds sizes to be sent from each node for degridding
//! \param[in] recv_sizes: sizes recieved from each node for degridding
//! \param[in] comm: Communicator over which to distribute the vector
std::vector<t_int> all_to_all_send_sizes(const std::vector<t_int> &recv_sizes,
                                         const sopt::mpi::Communicator &comm);
//! \brief All to All distribute a sparse vector
template <class STORAGE_INDEX_TYPE = t_int>
class AllToAllSparseVector {
  AllToAllSparseVector(const IndexMapping<STORAGE_INDEX_TYPE> &_mapping,
                       const std::vector<t_int> &_send_sizes, const std::vector<t_int> &_recv_sizes,
                       const sopt::mpi::Communicator &_comm)
      : mapping(_mapping), send_sizes(_send_sizes), recv_sizes(_recv_sizes), comm(_comm) {}
  AllToAllSparseVector(const IndexMapping<STORAGE_INDEX_TYPE> &_mapping,
                       const std::vector<t_int> &_recv_sizes, const sopt::mpi::Communicator &_comm)
      : mapping(_mapping),
        recv_sizes(_recv_sizes),
        send_sizes(all_to_all_send_sizes(_recv_sizes, _comm)),
        comm(_comm) {}
  AllToAllSparseVector(const std::vector<STORAGE_INDEX_TYPE> &local_indices,
                       const std::vector<t_int> &_recv_sizes, STORAGE_INDEX_TYPE ft_grid_size,
                       const STORAGE_INDEX_TYPE start, const sopt::mpi::Communicator &_comm)
      : AllToAllSparseVector(IndexMapping<STORAGE_INDEX_TYPE>(
                                 _comm.all_to_allv<STORAGE_INDEX_TYPE>(local_indices, _recv_sizes),
                                 ft_grid_size, start),
                             _recv_sizes, _comm) {}

 public:
  //! Constructs a functor to all to all a sparse factor
  //! \param[in] local_indices: indices that will be received by this process
  //! \param[in] ft_grid_size: Size of the vector
  //! \param[in] start: Starting index of vector
  //! \param[in] comm: Communicator over which to distribute the vector
  AllToAllSparseVector(const std::vector<STORAGE_INDEX_TYPE> &local_indices,
                       STORAGE_INDEX_TYPE ft_grid_size, STORAGE_INDEX_TYPE start,
                       const sopt::mpi::Communicator &_comm)
      : AllToAllSparseVector(
            local_indices,
            all_to_all_recv_sizes<STORAGE_INDEX_TYPE>(local_indices, _comm.size(), ft_grid_size),
            ft_grid_size, start, _comm) {}
  AllToAllSparseVector(const std::set<STORAGE_INDEX_TYPE> &local_indices,
                       STORAGE_INDEX_TYPE ft_grid_size, STORAGE_INDEX_TYPE start,
                       const sopt::mpi::Communicator &_comm)
      : AllToAllSparseVector(
            std::vector<STORAGE_INDEX_TYPE>(local_indices.begin(), local_indices.end()),
            ft_grid_size, start, _comm) {}
  template <class T0>
  AllToAllSparseVector(Eigen::SparseMatrixBase<T0> const &sparse,
                       const STORAGE_INDEX_TYPE ft_grid_size, const STORAGE_INDEX_TYPE start,
                       const sopt::mpi::Communicator &_comm)
      : AllToAllSparseVector(non_empty_outers(sparse), ft_grid_size,
                                                        start, _comm) {}

  template <class T0, class T1>
  void recv_grid(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
    assert(input.cols() == 1);
    Vector<typename T0::Scalar> buffer;
    mapping(input, buffer);
    assert(buffer.size() == std::accumulate(send_sizes.begin(), send_sizes.end(), 0));
    output.const_cast_derived() =
        comm.all_to_allv<typename T0::Scalar>(buffer, send_sizes, recv_sizes);
  }

  template <class T0, class T1>
  void send_grid(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
    assert(input.cols() == 1);
    auto const buffer = comm.all_to_allv<typename T0::Scalar>(input, recv_sizes, send_sizes);
    mapping.adjoint(buffer, output.derived());
  }

 private:
  IndexMapping<STORAGE_INDEX_TYPE> mapping;
  std::vector<t_int> send_sizes;
  std::vector<t_int> recv_sizes;
  sopt::mpi::Communicator comm;
};

}  // namespace purify
#endif
#endif
