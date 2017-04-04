
#ifndef PURIFY_DISTRIBUTE_OPERATOR_H
#define PURIFY_DISTRIBUTE_OPERATOR_H

#include "purify/config.h"
#ifdef PURIFY_MPI
#include <numeric>
#include "purify/types.h"
#include "sopt/mpi/communicator.h"
#include "purify/IndexMapping.h"

namespace purify {
  class DistributeSparseVector {

    DistributeSparseVector(const IndexMapping& mapping, const std::vector<t_int> &sizes, const t_int local_size, const sopt::mpi::Communicator &comm)
      : mapping(mapping), sizes(sizes), local_size(local_size), comm(comm) {};
    DistributeSparseVector(const std::vector<t_int> & local_indices, std::vector<t_int> const &sizes, t_int root_size, const sopt::mpi::Communicator &comm) 
      : DistributeSparseVector(IndexMapping(comm.gather(local_indices, sizes), root_size), sizes, local_indices.size(), comm){}

    public:
    DistributeSparseVector(const std::vector<t_int> & local_indices, t_int root_size, const sopt::mpi::Communicator &comm) 
      : DistributeSparseVector(local_indices, comm.gather<t_int>(local_indices.size()), root_size, comm) {}
    template<class T0>
    DistributeSparseVector(Eigen::SparseMatrixBase<T0> const &sparse, const sopt::mpi::Communicator &comm)
      : DistributeSparseVector(non_empty_outers(sparse), sparse.cols(), comm) {}


    template<class T0, class T1> void scatter(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
      assert(input.cols() == 1);
      if(not comm.is_root())
        return scatter(output);
      Vector<typename T0::Scalar> buffer;
      mapping(input, buffer);
      assert(buffer.size() == std::accumulate(sizes.begin(), sizes.end(), 0));
      output.const_cast_derived() = comm.scatterv(buffer, sizes);
    }

    template<class T1> void scatter(Eigen::MatrixBase<T1> const &output) const {
      if(comm.is_root())
        throw std::runtime_error("This function should not be called by root");
      output.const_cast_derived() = comm.scatterv<typename T1::Scalar>(local_size);
    }

    template<class T0, class T1> void gather(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
      assert(input.cols() == 1);
      if(not comm.is_root())
        return gather(input);
      auto const buffer = comm.gather<typename T0::Scalar>(input, sizes);
      mapping.adjoint(buffer, output.derived());
    }

    template<class T1> void gather(Eigen::MatrixBase<T1> const &input) const {
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
}
#endif
#endif
