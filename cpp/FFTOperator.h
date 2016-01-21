#ifndef PURIFY_FFT_OPERATOR_H
#define PURIFY_FFT_OPERATOR_H


#include "types.h"

#include <CCfits>
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <boost/math/special_functions/bessel.hpp>
#include <array>


namespace purify {

  //! This does something
  class Fft2d {
  public:
      Matrix<t_complex> shift(Matrix<t_complex> input);
      //! Performs fftshift on 2d matrix
      Matrix<t_complex> ishift(Matrix<t_complex> input);
      //! Performs ifftshift on 1d vector
      Matrix<t_complex> direct(const Matrix<t_complex>& input);
      //! Uses Eigen's 1D IFFT to perform 2D IFFT
      Matrix<t_complex> indirect(const Matrix<t_complex>& input);
      //! Converts from subscript to index for matrix.
   };
}

#endif
