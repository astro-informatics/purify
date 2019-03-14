#ifndef PURIFY_MAPPING_OPERATOR_H
#define PURIFY_MAPPING_OPERATOR_H

#include "purify/config.h"
#include "purify/types.h"
#include <set>
#include <vector>

namespace purify {
template <class STORAGE_INDEX_TYPE = t_int>
class IndexMapping {
 public:
  IndexMapping(const std::vector<STORAGE_INDEX_TYPE> &indices, const STORAGE_INDEX_TYPE N,
               const STORAGE_INDEX_TYPE start = 0)
      : indices(indices), N(N), start(start){};
  template <class ITER>
  IndexMapping(const ITER &first, const ITER &end, const STORAGE_INDEX_TYPE N,
               const STORAGE_INDEX_TYPE start = 0)
      : IndexMapping(std::vector<STORAGE_INDEX_TYPE>(first, end), N, start) {}
  IndexMapping(const std::set<STORAGE_INDEX_TYPE> &indices, const STORAGE_INDEX_TYPE N,
               const STORAGE_INDEX_TYPE start = 0)
      : IndexMapping(indices.begin(), indices.end(), N, start) {}

  //! Creates a vector of elements equal to re-indexed inpu
  template <class T0, class T1>
  void operator()(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
    auto &derived = output.const_cast_derived();
    derived.resize(indices.size());
    typename T0::Index i(0);
    for (auto const &index : indices) {
      assert(index >= start and index < (N + start));
      assert(derived.size() > i);
      derived(i++) = input(index - start);
    }
  }

  //! Vector of size `N` where non-zero elements are chosen from input at given indices
  template <class T0, class T1>
  void adjoint(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
    assert(static_cast<size_t>(input.size()) == indices.size());
    auto &derived = output.const_cast_derived();
    derived = T1::Zero(N, 1);
    typename T0::Index i(0);
    for (STORAGE_INDEX_TYPE const &index : indices) {
      assert(index >= start and index < (N + start));
      derived(index - start) += input(i++);
    }
  }

  t_int rows() const { return indices.size(); }
  STORAGE_INDEX_TYPE cols() const { return N; }

 private:
  std::vector<STORAGE_INDEX_TYPE> indices;
  STORAGE_INDEX_TYPE start;
  STORAGE_INDEX_TYPE N;
};

//! Indices of non empty outer indices
template <class T0>
std::set<typename T0::StorageIndex> non_empty_outers(Eigen::SparseMatrixBase<T0> const &matrix) {
  std::set<typename T0::StorageIndex> result;
  for (typename T0::StorageIndex k = 0; k < matrix.derived().outerSize(); ++k)
    for (typename T0::InnerIterator it(matrix.derived(), k); it; ++it) result.insert(it.col());
  return result;
}

//! Indices of non empty outer indices
template <class T0>
Sparse<typename T0::Scalar> compress_outer(T0 const &matrix) {
  static_assert(T0::IsRowMajor, "Not tested for col major");
  auto const indices = non_empty_outers(matrix);

  SparseVector<typename T0::StorageIndex, typename T0::StorageIndex> mapping(matrix.cols());
  mapping.reserve(indices.size());
  t_int i(0);
  for (auto const &index : indices) mapping.coeffRef(index) = i++;

  std::vector<typename Sparse<typename T0::Scalar>::StorageIndex> rows(matrix.rows() + 1, 0);
  std::vector<typename Sparse<typename T0::Scalar>::StorageIndex> cols(matrix.nonZeros(), 0);
  rows[matrix.rows()] = matrix.nonZeros();
  t_int index = 0;
  for (typename T0::StorageIndex k = 0; k < matrix.outerSize(); ++k) {
    rows[k] = index;
    for (typename Sparse<typename T0::Scalar, typename T0::StorageIndex>::InnerIterator it(matrix, k); it; ++it) {
      cols[index] = mapping.coeff(it.col());
      index++;
    }
  }
  return Eigen::MappedSparseMatrix<typename T0::Scalar, Eigen::RowMajor, typename Sparse<typename T0::Scalar>::StorageIndex>(
      matrix.rows(), indices.size(), matrix.nonZeros(), rows.data(), cols.data(),
      const_cast<typename T0::Scalar *>(matrix.derived().valuePtr()));
}
}  // namespace purify
#endif
