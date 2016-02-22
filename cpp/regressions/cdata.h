#ifndef PURIFY_TOOLS_FOR_TESTS_CDATA
#define PURIFY_TOOLS_FOR_TESTS_CDATA

#include <Eigen/Core>
#include "sopt/linear_transform.h"
#include "types.h"

namespace purify {
// Wraps calls to sampling and wavelets to C style
template <class T> struct CData {
  typedef Eigen::Matrix<T, Eigen::Dynamic, 1> t_Vector;
  typename t_Vector::Index nin, nout;
  sopt::LinearTransform<t_Vector> const &transform;
  t_uint direct_calls, adjoint_calls;
};

template <class T> void direct_transform(void *out, void *in, void **data) {
  CData<T> const &cdata = *(CData<T> *)data;
  typedef Eigen::Matrix<T, Eigen::Dynamic, 1> t_Vector;
  auto const input = t_Vector::Map((T*) in, cdata.nin);
  auto output = t_Vector::Map((T*) out, cdata.nout);
  output = (cdata.transform * input).eval();
  ++(((CData<T> *)data)->direct_calls);
}
template <class T> void adjoint_transform(void *out, void *in, void **data) {
  CData<T> const &cdata = *(CData<T> *)data;
  typedef Eigen::Matrix<T, Eigen::Dynamic, 1> t_Vector;
  auto const input = t_Vector::Map((T*) in, cdata.nout);
  auto output = t_Vector::Map((T*) out, cdata.nin);
  output = (cdata.transform.adjoint() * input).eval();
  ++(((CData<T> *)data)->adjoint_calls);
}

} /* sopt */
#endif
