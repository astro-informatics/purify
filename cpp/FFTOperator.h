#ifndef PURIFY_FFT_OPERATOR_H
#define PURIFY_FFT_OPERATOR_H


#include "types.h"

#include <fftw3.h>
#include <unsupported/Eigen/src/FFT/ei_fftw_impl.h>
#include <unsupported/Eigen/FFT>


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
      Matrix<t_complex> forward(const Matrix<t_complex>& input);
      //! Uses Eigen's 1D IFFT to perform 2D IFFT
      Matrix<t_complex> inverse(const Matrix<t_complex>& input);
      //! Performs fftshift on 2d matrix
      Matrix<t_complex> shift(const Matrix<t_complex>& input);
      //! Performs ifftshift on 2d matrix
      Matrix<t_complex> ishift(const Matrix<t_complex>& input);
   };

  class FFTOperator: protected Eigen::FFT<t_real, Eigen::internal::fftw_impl<t_real>>
  {
  public:
    //! Uses Eigen's perform 2D FFT
    Matrix<t_complex> forward(const Matrix<t_complex>& input);
    //! Uses Eigen's perform 2D IFFT
    Matrix<t_complex> inverse(const Matrix<t_complex>& input);
    //! Set up multithread fft
    void set_up_multithread();

  protected:
    t_int rigor_flag_ = FFTW_PATIENT;
  public:
    t_int const &rigor_flag() {return rigor_flag_;};

    FFTOperator &rigor_flag(t_int const &rigor_flag){
      t_int rigor_flag_ = rigor_flag;
      return *this;
    }
    
  };
}

#endif
