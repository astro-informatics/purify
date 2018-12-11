#ifndef PURIFY_CIMG_H
#define PURIFY_CIMG_H
#ifdef PURIFY_CImg
#include <CImg.h>
#ifdef Success
#undef Success
#endif
#ifdef Complex
#undef Complex
#endif
#ifdef Bool
#undef Bool
#endif
#ifdef None
#undef None
#endif
#ifdef Status
#undef Status
#endif
#endif

#include "purify/config.h"
#include "purify/types.h"
#ifdef PURIFY_CImg

namespace purify {
#ifdef PURIFY_CImg
//! Image type of CImg library
template <class T = t_real>
using CImage = cimg_library::CImg<T>;
template <class T = t_real>
using CImageList = cimg_library::CImgList<T>;
//! Display used to display CImg images
typedef cimg_library::CImgDisplay CDisplay;
#endif
namespace cimg {

//! Create image for displaying
template <class T>
CImage<typename T::Scalar> make_image(const Eigen::DenseBase<T> &x, const t_uint &rows,
                                      const t_uint &cols);
//! Create image using matrix/image dimensions
template <class T>
CImage<typename T::Scalar> make_image(const Eigen::DenseBase<T> &x);
//! Create display to display image of eigen Image/Matrix
template <class T>
CDisplay make_display(const Eigen::DenseBase<T> &x, const t_uint &rows, const t_uint &cols,
                      const std::string &name = "");
//! Create display using image dimensions
template <class T>
CDisplay make_display(const Eigen::DenseBase<T> &x, const std::string &name = "");

}  // namespace cimg
}  // namespace purify

namespace purify {
namespace cimg {

template <class T>
CImage<typename T::Scalar> make_image(const Eigen::DenseBase<T> &x, const t_uint &rows,
                                      const t_uint &cols) {
  assert(x.size() == rows * cols);
  auto image = CImage<typename T::Scalar>(rows, cols, 1, 1);
  Vector<typename T::Scalar>::Map(image.data(), x.size()) =
      Vector<typename T::Scalar>::Map(x.derived().data(), x.size());
  return image;
};
template <class T>
CImage<typename T::Scalar> make_image(const Eigen::DenseBase<T> &x) {
  return make_image<typename T::PlainObject>(x, x.rows(), x.cols());
}
template <class T>
CDisplay make_display(const Eigen::DenseBase<T> &x, const t_uint &rows, const t_uint &cols,
                      const std::string &name) {
  return CDisplay(cimg::make_image<typename T::PlainObject>(x.eval(), rows, cols), name.c_str());
}

template <class T>
CDisplay make_display(const Eigen::DenseBase<T> &x, const std::string &name) {
  return make_display<typename T::PlainObject>(x.eval(), x.rows(), x.cols(), name);
}

}  // namespace cimg
}  // namespace purify

#endif

#endif
