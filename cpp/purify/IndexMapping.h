#ifndef PURIFY_MAPPING_OPERATOR_H
#define PURIFY_MAPPING_OPERATOR_H

#include "purify/config.h"
#include <set>
#include <vector>
#include "purify/types.h"

namespace purify {

class IndexMapping {
public:
  IndexMapping(const std::vector<t_int> &indices, const t_int N) : indices(indices), N(N){};
  template <class ITER>
  IndexMapping(const ITER &first, const ITER &end, const t_int N)
      : IndexMapping(std::vector<t_int>(first, end), N) {}
  IndexMapping(const std::set<t_int> &indices, const t_int N)
      : IndexMapping(indices.begin(), indices.end(), N) {}

  //! Creates a vector of elements equal to re-indexed inpu
  template <class T0, class T1>
  void operator()(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
    auto &derived = output.const_cast_derived();
    derived.resize(indices.size());
    typename T0::Index i(0);
    for(auto const &index : indices) {
      assert(index >= 0 and index < input.size());
      assert(derived.size() > i);
      derived(i++) = input(index);
    }
  }

  //! Vector of size `N` where non-zero elements are chosen from input at given indices
  template <class T0, class T1>
  void adjoint(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
    assert(static_cast<size_t>(input.size()) == indices.size());
    auto &derived = output.const_cast_derived();
    derived = T1::Zero(N, 1);
    typename T0::Index i(0);
    for(auto const &index : indices) {
      assert(index >= 0 and index < output.size());
      derived(index) += input(i++);
    }
  }

  t_int rows() const { return indices.size(); }
  t_int cols() const { return N; }

private:
  std::vector<t_int> indices;
  t_int N;
};

//! Indices of non empty outer indices
template <class T0> std::set<t_int> non_empty_outers(Eigen::SparseMatrixBase<T0> const &matrix) {
  std::set<t_int> result;
  for(typename T0::StorageIndex k = 0; k < matrix.derived().outerSize(); ++k)
    for(typename T0::InnerIterator it(matrix.derived(), k); it; ++it)
      result.insert(it.col());
  return result;
}

//! Indices of non empty outer indices
template <class T0>
Sparse<typename T0::Scalar> compress_outer(Eigen::SparseMatrixBase<T0> const &matrix) {
  static_assert(T0::IsRowMajor, "Not tested for col major");
  auto const indices = non_empty_outers(matrix);

  std::vector<t_int> mapping(matrix.cols(), 0);
  t_int i(0);
  for(auto const &index : indices)
    mapping[index] = i++;

  std::vector<t_int> rows(G.rows() + 1);
  std::vector<t_int> cols(G.nonZeros());
  rows(G.rows()) = G.nonZeros();
  t_int index = 0;
  for(t_int k = 0; k < G.outerSize(); ++k) {
    rows[k] = index;
    for(Sparse<t_complex>::InnerIterator it(G, k); it; ++it) {
      cols[index] = mapping[it.col()];
      index++;
    }
  }
  return Map<typename T0::Derived>(matrix.rows(), indices.size(), matrix.nonZeros(), rows.data(),
                                   cols.data(), matrix.derived().valuePtr());
}
} // namespace purify
#endif
