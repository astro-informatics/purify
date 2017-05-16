#ifndef PURIFY_DISTRIBUTE_OPERATOR_H
#define PURIFY_DISTRIBUTE_OPERATOR_H

#include "purify/config.h"
#ifdef PURIFY_MPI
#include <numeric>
#include "sopt/mpi/communicator.h"
#include "purify/IndexMapping.h"
#include "purify/types.h"

namespace purify {
//! \brief Gathers and scatters a vector
//! \details Only the root node will ever have the full vector. Other nodes only get those indices
//! they ask for.
class DistributeSparseVector {

  DistributeSparseVector(const IndexMapping &_mapping, const std::vector<t_int> &_sizes,
                         const t_int _local_size, const sopt::mpi::Communicator &_comm)
      : mapping(_mapping), sizes(_sizes), local_size(_local_size), comm(_comm) {}
  DistributeSparseVector(const std::vector<t_int> &local_indices, std::vector<t_int> const &_sizes,
                         t_int global_size, const sopt::mpi::Communicator &_comm)
      : DistributeSparseVector(IndexMapping(_comm.gather(local_indices, _sizes), global_size),
                               _sizes, static_cast<t_int>(local_indices.size()), _comm) {}

public:
  //! Constructs a functor to distribute and gather a vector
  //! \param[in] local_indices: indices that will be received by this process
  //! \param[in] global_size: Size of the vector as a whole
  //! \param[in] comm: Communicator over which to distribute the vector
  DistributeSparseVector(const std::vector<t_int> &local_indices, t_int global_size,
                         const sopt::mpi::Communicator &_comm)
      : DistributeSparseVector(local_indices,
                               _comm.gather(static_cast<t_int>(local_indices.size())), global_size,
                               _comm) {}
  DistributeSparseVector(const std::set<t_int> &local_indices, t_int global_size,
                         const sopt::mpi::Communicator &_comm)
      : DistributeSparseVector(std::vector<t_int>(local_indices.begin(), local_indices.end()),
                               global_size, _comm) {}
  template <class T0>
  DistributeSparseVector(Eigen::SparseMatrixBase<T0> const &sparse,
                         const sopt::mpi::Communicator &_comm)
      : DistributeSparseVector(non_empty_outers(sparse), sparse.cols(), _comm) {}

  template <class T0, class T1>
  void scatter(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
    assert(input.cols() == 1);
    if(not comm.is_root())
      return scatter(output);
    Vector<typename T0::Scalar> buffer;
    mapping(input, buffer);
    assert(buffer.size() == std::accumulate(sizes.begin(), sizes.end(), 0));
    output.const_cast_derived() = comm.scatterv(buffer, sizes);
  }

  template <class T1> void scatter(Eigen::MatrixBase<T1> const &output) const {
    if(comm.is_root())
      throw std::runtime_error("This function should not be called by root");
    output.const_cast_derived() = comm.scatterv<typename T1::Scalar>(local_size);
  }

  template <class T0, class T1>
  void gather(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
    assert(input.cols() == 1);
    if(not comm.is_root())
      return gather(input);
    auto const buffer = comm.gather<typename T0::Scalar>(input, sizes);
    mapping.adjoint(buffer, output.derived());
  }

  template <class T1> void gather(Eigen::MatrixBase<T1> const &input) const {
    if(comm.is_root())
      throw std::runtime_error("This function should not be called by root");
    comm.gather<typename T1::Scalar>(input);
  }

private:
  IndexMapping mapping;
  std::vector<t_int> sizes;
  t_int local_size;
  sopt::mpi::Communicator comm;
};
} // namespace purify
#endif
#endif
