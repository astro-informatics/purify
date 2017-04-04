#ifndef PURIFY_MAPPING_OPERATOR_H
#define PURIFY_MAPPING_OPERATOR_H

#include "purify/config.h"
#include <set>
#include "purify/types.h"

namespace purify {

class IndexMapping {
public:
  IndexMapping(const std::set<t_int> &indices, const t_int size) : indices(indices), size(size){};

  //! Creates a vector of elements equal to re-indexed input
  template <class T0, class T1>
  void operator()(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
    const_cast<Eigen::MatrixBase<T1> &>(output).derived().resize(indices.size());
    typename T0::Index i(0);
    for(auto const &index : indices) {
      assert(index >= 0 and index <= input.size());
      const_cast<Eigen::MatrixBase<T1> &>(output)(i++) = input(index);
    }
  }

  //! Vector of size `size` where non-zero elements are chosen from input at given indices
  template <class T0, class T1>
  void adjoint(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
    assert(indices.size() <= size);
    assert(input.size() == indices.size());
    auto &derived = output.const_cast_derived();
    derived = T1::Zero(size);
    typename T0::Index i(0);
    for(auto const &index : indices) {
      assert(index >= 0 and index <= output.size());
      derived(index) = input(i++);
    }
  }

private:
  std::set<t_int> indices;
  t_int size;
};

//! Indices of non empty outer indices
template <class T0> std::set<t_int> non_empty_outers(Eigen::SparseMatrixBase<T0> const &matrix) {
  std::set<t_int> result;
  for(typename T0::StorageIndex k = 0; k < matrix.outerSize(); ++k)
    for(typename T0::InnerIterator it(matrix, k); it; ++it)
      result.insert(it.col());
  return result;
}
}
#endif
