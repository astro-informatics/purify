#ifndef PURIFY_CONVOLUTION_H
#define PURIFY_CONVOLUTION_H

namespace purify {
namespace convol {

//! zero pad a vector by a given amount
template <class T> inline Vector<T> zero_pad(const Vector<T> &input, const t_int padding);
//! zero pad a matrix by a given amount
template <class T>
inline Matrix<T> zero_pad(const Matrix<T> &input, const t_int paddingv, const t_int paddingu);
//! 1d linear convoluiton of entire signal
template <class T>
inline Vector<T> linear_convol_1d(const Vector<T> &kernelf, const Vector<T> &kernelg);
//! perform linear convolution between two separable kernels and a 2d kernel (vectors)
template <class T>
Matrix<T>
linear_convol_2d(const Vector<T> &kernelfu, const Vector<T> &kernelfv, const Matrix<T> &kernelg);
//! perform linear convolution between two separable kernels and a 2d kernel
template <class T>
Matrix<T>
linear_convol_2d(const std::function<T(t_int)> &kernelu, const std::function<T(t_int)> &kernelv,
                 const std::function<T(t_int, t_int)> &kernelw, const t_uint &Jfu,
                 const t_uint &Jfv, const t_uint &Jgu, const t_uint &Jgv);

template <class T> inline Vector<T> zero_pad(const Vector<T> &input, const t_int padding) {
  if(padding < 1)
    throw std::runtime_error("Padding must be 1 or greater for convolution.");
  Vector<T> output = Vector<T>::Zero(input.size() + 2 * padding);
  output.segment(padding, input.size()) = input;
  return output;
}
template <class T>
inline Matrix<T> zero_pad(const Matrix<T> &input, const t_int paddingv, const t_int paddingu) {
  if((paddingu < 1) or (paddingv < 1))
    throw std::runtime_error("Padding must be 1 or greater for convolution.");
  Matrix<T> output = Matrix<T>::Zero(input.rows() + 2 * paddingv, input.cols() + 2 * paddingu);
  output.block(paddingv, paddingu, input.rows(), input.cols()) = input;
  return output;
}
template <class T>
inline Vector<T> linear_convol_1d(const Vector<T> &kernelf, const Vector<T> &kernelg) {
  // assumes that Jf has smallest support with kernelf as the filter
  // assumes that kernelg is zero padded
  const t_int Jf = kernelf.size();
  Vector<T> output = Vector<T>::Zero(kernelg.size() - Jf + 1);
  for(t_int j = 0; j < output.size(); j++)
    output(j)
        = (kernelf.segment(0, Jf).array() * kernelg.segment(output.size() - j - 1, Jf).array())
              .sum();
  return output;
}

template <class T>
Matrix<T>
linear_convol_2d(const Vector<T> &kernelfu, const Vector<T> &kernelfv, const Matrix<T> &kernelg) {
  //! performing convolution for separable kernel
  Matrix<T> buffer = Matrix<T>::Zero(kernelfv.size() + kernelg.rows() - 1, kernelg.cols());
  //! performing convolution for separable kernel
  for(t_uint i = 0; i < kernelg.cols(); i++)
    buffer.col(i) = linear_convol_1d<T>(kernelfv, zero_pad<T>(kernelg.col(i), kernelfv.size() - 1));
  Matrix<T> output
      = Matrix<T>::Zero(kernelfv.size() + kernelg.rows() - 1, kernelfu.size() + kernelg.cols() - 1);
  for(t_uint i = 0; i < output.rows(); i++)
    output.row(i) = linear_convol_1d<T>(kernelfu, zero_pad<T>(buffer.row(i), kernelfu.size() - 1));
  return output;
}
//! perform linear convolution between two separable kernels and a 2d kernel
template <class T>
Matrix<T>
linear_convol_2d(const std::function<T(t_int)> &kernelu, const std::function<T(t_int)> &kernelv,
                 const std::function<T(t_int, t_int)> &kernelw, const t_uint &Jfu,
                 const t_uint &Jfv, const t_uint &Jgu, const t_uint &Jgv) {
  Vector<T> kernelfu = Vector<T>::Zero(Jfu);
  Vector<T> kernelfv = Vector<T>::Zero(Jfv);
  Matrix<T> kernelg = Matrix<T>::Zero(Jgv, Jgu);
  for(t_int i = 0; i < kernelg.cols(); i++)
    for(t_int j = 0; j < kernelg.rows(); j++)
      kernelg(j, i) = kernelw(j, i);
  for(t_int i = 0; i < kernelfu.size(); i++)
    kernelfu(i) = kernelu(i);
  for(t_int i = 0; i < kernelfv.size(); i++)
    kernelfv(i) = kernelv(i);
  return linear_convol_2d<T>(kernelfu, kernelfv, kernelg);
}

} // namespace convol
} // namespace purify
#endif
