#ifndef PURIFY_MAPPING_OPERATOR_H
#define PURIFY_MAPPING_OPERATOR_H

#include "purify/config.h"
#include "purify/types.h"
#include <set>

namespace purify {
  class IndexMapping {

    public:
    IndexMapping(const std::set<t_int> & indices, const t_int size) : indices(indices), size(size) {};

    //! Creates a vector of elements of input with given indices
    template<class T0, class T1> void operator()(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
      const_cast<Eigen::MatrixBase<T1>&>(output).derived().resize(indices.size());
      typename T0::Index i(0);
      for(auto const &index: indices) {
        assert(index >= 0 and index <= input.size());
        const_cast<Eigen::MatrixBase<T1>&>(output)(i++) = input(index);
      }
    }

    //! Creates a vector of elements of input with given indices
    template<class T0, class T1> void adjoint(Eigen::MatrixBase<T0> const &input, Eigen::MatrixBase<T1> const &output) const {
      assert(indices.size() <= size);
      assert(input.size() == indices.size());
      const_cast<Eigen::MatrixBase<T1>&>(output) = T1::Zero(size);
      typename T0::Index i(0);
      for(auto const &index: indices) {
        assert(index >= 0 and index <= output.size());
        const_cast<Eigen::MatrixBase<T1>&>(output)(index) = input(i++);
      }
    }

    private:
    std::set<t_int> indices;
    t_int size;
  };
}
#endif
