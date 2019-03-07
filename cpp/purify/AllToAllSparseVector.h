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
std::vector<t_int> all_to_all_recv_sizes(const std::vector<t_int> &local_indices, const t_int nodes,
                                         const t_int N);
//! Finds sizes to be sent from each node for degridding
//! \param[in] recv_sizes: sizes recieved from each node for degridding
//! \param[in] comm: Communicator over which to distribute the vector
std::vector<t_int> all_to_all_send_sizes(const std::vector<t_int> &recv_sizes,
                                         const sopt::mpi::Communicator &comm);
//! \brief All to All distribute a sparse vector
class AllToAllSparseVector {
  AllToAllSparseVector(const IndexMapping &_mapping, const std::vector<t_int> &_send_sizes,
                       const std::vector<t_int> &_recv_sizes, const sopt::mpi::Communicator &_comm)
      : mapping(_mapping), send_sizes(_send_sizes), recv_sizes(_recv_sizes), comm(_comm) {}
  AllToAllSparseVector(const IndexMapping &_mapping, const std::vector<t_int> &_recv_sizes,
                       const sopt::mpi::Communicator &_comm)
      : mapping(_mapping),
        recv_sizes(_recv_sizes),
        send_sizes(all_to_all_send_sizes(_recv_sizes, _comm)),
        comm(_comm) {}
  AllToAllSparseVector(const std::vector<t_int> &local_indices,
                       const std::vector<t_int> &_recv_sizes, t_int ft_grid_size, const t_int start,
                       const sopt::mpi::Communicator &_comm)
      : AllToAllSparseVector(
            IndexMapping(_comm.all_to_allv<t_int>(local_indices, _recv_sizes), ft_grid_size, start),
            _recv_sizes, _comm) {}

 public:
  //! Constructs a functor to all to all a sparse factor
  //! \param[in] local_indices: indices that will be received by this process
  //! \param[in] ft_grid_size: Size of the vector
  //! \param[in] start: Starting index of vector
  //! \param[in] comm: Communicator over which to distribute the vector
  AllToAllSparseVector(const std::vector<t_int> &local_indices, t_int ft_grid_size, t_int start,
                       const sopt::mpi::Communicator &_comm)
      : AllToAllSparseVector(local_indices,
                             all_to_all_recv_sizes(local_indices, _comm.size(), ft_grid_size),
                             ft_grid_size, start, _comm) {}
  AllToAllSparseVector(const std::set<t_int> &local_indices, t_int ft_grid_size, t_int start,
                       const sopt::mpi::Communicator &_comm)
      : AllToAllSparseVector(std::vector<t_int>(local_indices.begin(), local_indices.end()),
                             ft_grid_size, start, _comm) {}
  template <class T0>
  AllToAllSparseVector(Eigen::SparseMatrixBase<T0> const &sparse, const t_int ft_grid_size,
                       const t_int start, const sopt::mpi::Communicator &_comm)
      : AllToAllSparseVector(non_empty_outers(sparse), ft_grid_size, start, _comm) {}

  template <class T0, class T1>
  void recv_grid(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
    assert(input.cols() == 1);
    Vector<typename T0::Scalar> buffer;
    mapping(input, buffer);
    assert(buffer.size() == std::accumulate(send_sizes.begin(), send_sizes.end(), 0));
    output.const_cast_derived() = comm.all_to_allv<typename T0::Scalar>(buffer, send_sizes);
  }

  template <class T0, class T1>
  void send_grid(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
    assert(input.cols() == 1);
    auto const buffer = comm.all_to_allv<typename T0::Scalar>(input, recv_sizes);
    mapping.adjoint(buffer, output.derived());
  }

 private:
  IndexMapping mapping;
  std::vector<t_int> send_sizes;
  std::vector<t_int> recv_sizes;
  sopt::mpi::Communicator comm;
};

}  // namespace purify
#endif
#endif
