#include "purify/config.h"
#include "purify/kernels.h"

namespace purify {

namespace kernels {
t_real kaiser_bessel(const t_real &x, const t_int &J) {
  /*
    kaiser bessel gridding kernel
  */
  t_real alpha = 2.34 * J; // value said to be optimal in Fessler et. al. 2003
  return kaiser_bessel_general(x, J, alpha);
}

t_real kaiser_bessel_general(const t_real &x, const t_int &J, const t_real &alpha) {
  /*
    kaiser bessel gridding kernel
  */
  t_real a = 2 * x / J;
  return boost::math::cyl_bessel_i(0, std::real(alpha * std::sqrt(1 - a * a)))
         / boost::math::cyl_bessel_i(0, alpha);
}

t_real ft_kaiser_bessel_general(const t_real &x, const t_int &J, const t_real &alpha) {
  /*
    Fourier transform of kaiser bessel gridding kernel
  */

  t_complex eta = std::sqrt(
      static_cast<t_complex>((constant::pi * x * J) * (constant::pi * x * J) - alpha * alpha));
  t_real normalisation = 38828.11016883; // Factor that keeps it consistent with fessler formula

  return std::real(std::sin(eta) / eta) / normalisation; // simple way of doing the calculation, the
                                                         // boost bessel funtions do not support
                                                         // complex valued arguments
}

t_real ft_kaiser_bessel(const t_real &x, const t_int &J) {
  /*
    Fourier transform of kaiser bessel gridding kernel
  */

  t_real alpha = 2.34 * J; // value said to be optimal in Fessler et. al. 2003
  return ft_kaiser_bessel_general(x, J, alpha);
}

t_real gaussian(const t_real &x, const t_int &J) {
  /*
    Guassian gridding kernel

    x:: value to evaluate
    J:: support size
  */
  t_real sigma = 0.31 * std::pow(J, 0.52); // Optimal sigma according to fessler et al.
  return gaussian_general(x, J, sigma);
}

t_real ft_gaussian(const t_real &x, const t_int &J) {
  /*
    Fourier transform of guassian gridding kernel

    x:: value to evaluate
    J:: support size of gridding kernel
  */
  t_real sigma = 0.31 * std::pow(J, 0.52);
  return ft_gaussian_general(x, J, sigma);
}

t_real calc_for_pswf(const t_real &eta0, const t_int &J, const t_real &alpha) {
  // polynomial coefficients for prolate spheriodal wave function rational approximation
  const std::array<t_real, 6> p1
      = {{8.203343e-2, -3.644705e-1, 6.278660e-1, -5.335581e-1, 2.312756e-1, 2 * 0.0}};
  const std::array<t_real, 6> p2
      = {{4.028559e-3, -3.697768e-2, 1.021332e-1, -1.201436e-1, 6.412774e-2, 2 * 0.0}};
  const std::array<t_real, 3> q1 = {{1., 8.212018e-1, 2.078043e-1}};
  const std::array<t_real, 3> q2 = {{1., 9.599102e-1, 2.918724e-1}};

  if(J != 6 or alpha != 1) {
    return 0;
  }
  // Calculating numerator and denominator using Horner's rule.
  // PSWF = numerator / denominator

  auto fraction = [](t_real eta, std::array<t_real, 6> const &p, std::array<t_real, 3> const &q) {
    auto const p_size = sizeof(p) / sizeof(p[0]) - 1;
    auto const q_size = sizeof(q) / sizeof(q[0]) - 1;

    auto numerator = p[p_size];
    for(t_int i = 1; i <= p_size; ++i) {
      numerator = eta * numerator + p[p_size - i];
    }

    auto denominator = q[q_size];
    for(t_int i = 1; i <= q_size; ++i) {
      denominator = eta * denominator + q[q_size - i];
    }
    return numerator / denominator;
  };
  if(0 <= std::abs(eta0) and std::abs(eta0) <= 0.75)
    return fraction(eta0 * eta0 - 0.75 * 0.75, p1, q1);
  if(0.75 < std::abs(eta0) and std::abs(eta0) <= 1)
    return fraction(eta0 * eta0 - 1 * 1, p2, q2);

  return 0;
}

t_real pswf(const t_real &x, const t_int &J) {
  /*
    Calculates the standard PSWF for radio astronomy, with a support of J = 6 and alpha = 1.

    x:: value to evaluate
    J:: support size of gridding kernel

    The tailored prolate spheroidal wave functions for gridding radio astronomy.
    Details are explained in Optimal Gridding of Visibility Data in Radio
    Astronomy, F. R. Schwab 1983.

  */
  const t_real eta0 = 2 * x / J;
  const t_real alpha = 1;
  return calc_for_pswf(eta0, J, alpha) * std::pow(1 - eta0 * eta0, alpha);
}

t_real ft_pswf(const t_real &x, const t_int &J) {
  /*
    Calculates the fourier transform of the standard PSWF for radio astronomy, with a support of J =
    6 and alpha = 1.

    x:: value to evaluate
    J:: support size of gridding kernel

    The tailored prolate spheroidal wave functions for gridding radio astronomy.
    Details are explained in Optimal Gridding of Visibility Data in Radio
    Astronomy, F. R. Schwab 1983.

  */

  const t_real alpha = 1;
  const t_real eta0 = 2 * x;

  return calc_for_pswf(eta0, J, alpha);
}

Vector<t_real> kernel_samples(const t_int &total_samples,
                              const std::function<t_real(t_real)> kernelu, const t_int &J) {
  /*
    Pre-calculates samples of a kernel, that can be used with linear interpolation (see Rapid
    gridding reconstruction with a minimal oversampling ratio, Beatty et. al. 2005)
  */
  Vector<t_real> samples(total_samples);
  for(t_real i = 0; i < total_samples; ++i) {
    samples(i) = kernelu(i / total_samples * J - J / 2);
  }
  return samples;
}

t_real kernel_linear_interp(const Vector<t_real> &samples, const t_real &x, const t_int &J) {
  /*
    Calculates kernel using linear interpolation between pre-calculated samples. (see Rapid gridding
    reconstruction with a minimal oversampling ratio, Beatty et. al. 2005)
  */
  t_int total_samples = samples.size();

  t_real i_effective = (x + J / 2) * total_samples / J;

  t_real i_0 = floor(i_effective);
  t_real i_1 = ceil(i_effective);
  // case where i_effective is a sample point
  if(std::abs(i_0 - i_1) == 0) {
    return samples(i_0);
  }
  // linearly interpolate from nearest neighbour
  t_real y_0;
  t_real y_1;
  if(i_0 < 0 or i_0 >= total_samples) {
    y_0 = 0;
  } else {
    y_0 = samples(i_0);
  }
  if(i_1 < 0 or i_1 >= total_samples) {
    y_1 = 0;
  } else {
    y_1 = samples(i_1);
  }
  t_real output = y_0 + (y_1 - y_0) / (i_1 - i_0) * (i_effective - i_0);
  return output;
}
t_real pill_box(const t_real &x, const t_int &J) {
  /*
    Gaussian gridding kernel

    x:: value to evaluate
    J:: support size
  */
  return 1. / static_cast<t_real>(J);
}
t_real ft_pill_box(const t_real &x, const t_int &J) {
  /*
    Gaussian gridding kernel

    x:: value to evaluate
    J:: support size
  */
  return boost::math::sinc_pi(J * x * constant::pi);
}
t_real gaussian_general(const t_real &x, const t_int &J, const t_real &sigma) {
  /*
    Gaussian gridding kernel

    x:: value to evaluate
    J:: support size
    sigma:: standard deviation of Gaussian kernel (in pixels)
  */

  t_real a = x / sigma;
  return std::exp(-a * a * 0.5);
}

t_real ft_gaussian_general(const t_real &x, const t_int &J, const t_real &sigma) {
  /*
    Fourier transform of Gaussian gridding kernel

    x:: value to evaluate
    J:: support size of gridding kernel
    sigma:: standard deviation of Gaussian kernel (in pixels)
  */

  t_real a = x * sigma * constant::pi;
  return std::sqrt(constant::pi / 2) / sigma * std::exp(-a * a * 2);
}
}
}
