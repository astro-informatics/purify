#ifndef PURIFY_FFT_OPERATOR_H
#define PURIFY_FFT_OPERATOR_H

#include "purify/config.h"
#include "purify/types.h"

#ifdef PURIFY_OPENMP
#include <omp.h>
#endif
#include <fftw3.h>

#include <unsupported/Eigen/FFT>
#include <unsupported/Eigen/src/FFT/ei_fftw_impl.h>

namespace purify {

//! This does something
class Fft2d {
private:
  //! Performs fftshift on vector
  Vector<t_complex> fftshift_1d(const Vector<t_complex> input);
  //! Performs ifftshift on vector
  Vector<t_complex> ifftshift_1d(const Vector<t_complex> input);

public:
  //! Uses Eigen's 1D FFT to perform 2D FFT
  Matrix<t_complex> forward(const Matrix<t_complex> &input);
  //! Uses Eigen's 1D IFFT to perform 2D IFFT
  Matrix<t_complex> inverse(const Matrix<t_complex> &input);
  //! Performs fftshift on 2d matrix
  Matrix<t_complex> shift(const Matrix<t_complex> &input);
  //! Performs ifftshift on 2d matrix
  Matrix<t_complex> ishift(const Matrix<t_complex> &input);
};

class FFTOperator : protected Eigen::FFT<t_real, Eigen::internal::fftw_impl<t_real>> {
public:
  //! Uses Eigen's perform 2D FFT
  Matrix<t_complex> forward(const Matrix<t_complex> &input, bool only_plan = false);
  //! Uses Eigen's perform 2D IFFT
  Matrix<t_complex> inverse(const Matrix<t_complex> &input, bool only_plan = false);
  //! Set up multithread fft
  void set_up_multithread();
  //! Set up plan
  void init_plan(const Matrix<t_complex> &input);

protected:
  t_int fftw_flag_ = (FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);

public:
  t_int const &fftw_flag() { return fftw_flag_; };

  FFTOperator &fftw_flag(t_int const &fftw_flag) {
    fftw_flag_ = fftw_flag;
    return *this;
  }
};
}

#endif
