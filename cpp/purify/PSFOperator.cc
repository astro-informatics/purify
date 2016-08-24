#include "purify/config.h"
#include "purify/PSFOperator.h"
#include "purify/logging.h"

namespace purify {

PSFOperator::PSFOperator(const Image<t_complex> &point_spread_function)
    : ftsize_u(static_cast<t_int>(point_spread_function.cols())),
      ftsize_v(static_cast<t_int>(point_spread_function.rows())) {
  psf_fft = fftop.forward(point_spread_function).array();
  psf_adj_fft = fftop.forward(point_spread_function.matrix().adjoint()).array();
  norm = std::sqrt(PSFOperator::power_method(20));
}

t_real PSFOperator::power_method(const t_int &niters, const t_real &relative_difference) {
  /*
   Attempt at coding the power method, returns the largest eigen value of a linear operator
    niters:: max number of iterations
    relative_difference:: percentage difference at which eigen value has converged
  */
  t_real estimate_eigen_value = 1;
  t_real old_value = 0;
  Image<t_complex> estimate_eigen_vector
      = Image<t_complex>::Random(floor(ftsize_v * 0.5), floor(ftsize_u * 0.5));
  estimate_eigen_vector = estimate_eigen_vector / estimate_eigen_vector.matrix().norm();
  for(t_int i = 0; i < niters; ++i) {
    auto new_estimate_eigen_vector
        = PSFOperator::adjoint(PSFOperator::forward(estimate_eigen_vector));
    estimate_eigen_value = new_estimate_eigen_vector.matrix().norm();
    estimate_eigen_vector = new_estimate_eigen_vector / estimate_eigen_value;
    if(relative_difference > std::abs(old_value - estimate_eigen_value) / old_value)
      break;
    old_value = estimate_eigen_value;
  }
  return old_value;
}

Image<t_complex> PSFOperator::forward(const Image<t_complex> &image) {
  return PSFOperator::convolution_with_psf(image, psf_fft) / norm;
}

Image<t_complex> PSFOperator::adjoint(const Image<t_complex> &image) {
  return PSFOperator::convolution_with_psf(image, psf_adj_fft) / norm;
}
Image<t_complex> PSFOperator::convolution_with_psf(const Image<t_complex> &image,
                                                   const Image<t_complex> &ft_of_psf) {
  /*
          Performs convolution with point spread function
  */
  const t_int imsizex = image.cols();
  const t_int imsizey = image.rows();
  const t_int x_start = floor(ftsize_u * 0.5 - imsizex * 0.5);
  const t_int y_start = floor(ftsize_v * 0.5 - imsizey * 0.5);
  Image<t_complex> padded_image = Matrix<t_complex>::Zero(ftsize_v, ftsize_u);
  // Zero pad image to be the same size as PSF
  padded_image.block(y_start, x_start, imsizey, imsizex) = image;

  return fftop.inverse(fftop.forward(padded_image).array() * ft_of_psf)
      .array()
      .block(y_start, x_start, imsizey, imsizex);
}
}
