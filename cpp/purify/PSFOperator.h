#ifndef PURIFY_PSF_OPERATOR_H
#define PURIFY_PSF_OPERATOR_H

#include "purify/config.h"
#include "purify/FFTOperator.h"
#include "purify/kernels.h"
#include "purify/types.h"
#include "purify/utilities.h"

#include <iostream>
#include <string>

namespace purify {

//! This does something
class PSFOperator {
public:
  PSFOperator(const Image<t_complex> &point_spread_function);

  //! forward convolution with psf
  Image<t_complex> forward(const Image<t_complex> &image);
  //! backward convolution with psf
  Image<t_complex> adjoint(const Image<t_complex> &image);

protected:
  const t_int ftsize_u;
  const t_int ftsize_v;
  Image<t_complex> psf_fft;
  Image<t_complex> psf_adj_fft;
  FFTOperator fftop;
  t_real norm;
  //! Convolution operator
  Image<t_complex>
  convolution_with_psf(const Image<t_complex> &image, const Image<t_complex> &psf_fft);
  //! power method
  t_real power_method(const t_int &niters, const t_real &relative_difference = 1e-9);
};
}

#endif
