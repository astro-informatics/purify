#ifndef PURIFY_CIMG_H
#define PURIFY_CIMG_H
#ifdef PURIFY_CImg
#include "purify/config.h"
#include "purify/types.h"

namespace purify {
namespace cimg {

//! Create image for displaying
template <class T>
CImage<typename T::Scalar>
make_image(const Eigen::DenseBase<T> &x, const t_uint &rows, const t_uint &cols);
//! Create image using matrix/image dimensions
template <class T> CImage<typename T::Scalar> make_image(const Eigen::DenseBase<T> &x);
//! Create display to display image of eigen Image/Matrix
template <class T>
CDisplay make_display(const Eigen::DenseBase<T> &x, const t_uint &rows, const t_uint &cols,
                      const std::string &name = "");
//! Create display using image dimensions
template <class T>
CDisplay make_display(const Eigen::DenseBase<T> &x, const std::string &name = "");

} // namespace cimg
} // namespace purify

namespace purify {
namespace cimg {

template <class T>
CImage<typename T::Scalar>
make_image(const Eigen::DenseBase<T> &x, const t_uint &rows, const t_uint &cols) {
  auto image = CImage<typename T::Scalar>(rows, cols, 1, 1);
  const Vector<typename T::Scalar> input
      = Vector<typename T::Scalar>::Map(x.derived().data(), x.size(), 1);
  for(t_uint i = 0; i < cols; i++) {
    for(t_uint j = 0; j < rows; j++) {
      const t_uint index = j + rows * i;
      image(j, cols - i - 1) = input(index);
    }
  }
  return image;
};
template <class T> CImage<typename T::Scalar> make_image(const Eigen::DenseBase<T> &x) {
  return make_image<typename T::PlainObject>(x, x.rows(), x.cols());
}
template <class T>
CDisplay make_display(const Eigen::DenseBase<T> &x, const t_uint &rows, const t_uint &cols,
                      const std::string &name) {
  return CDisplay(cimg::make_image<typename T::PlainObject>(x.eval(), rows, cols), name.c_str());
}

template <class T> CDisplay make_display(const Eigen::DenseBase<T> &x, const std::string &name) {
  return make_display<typename T::PlainObject>(x.eval(), x.rows(), x.cols(), name);
}

} // namespace cimg
} // namespace purify

#endif

#endif
