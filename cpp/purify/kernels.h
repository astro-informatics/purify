#ifndef PURIFY_KERNELS_H
#define PURIFY_KERNELS_H

#include "purify/config.h"
#include <array>
#include <boost/math/special_functions/bessel.hpp>
#include <boost/math/special_functions/sinc.hpp>
#include "purify/types.h"

namespace purify {

namespace kernels {

//! Kaiser-Bessel kernel
t_real kaiser_bessel(const t_real &x, const t_int &J);
//! More general Kaiser-Bessel kernel
t_real kaiser_bessel_general(const t_real &x, const t_int &J, const t_real &alpha);
//!  Fourier transform of more general Kaiser-Bessel kernel
t_real ft_kaiser_bessel_general(const t_real &x, const t_int &J, const t_real &alpha);
//! Fourier transform of kaiser bessel kernel
t_real ft_kaiser_bessel(const t_real &x, const t_int &J);
//! Gaussian kernel
t_real gaussian(const t_real &x, const t_int &J);
//! Fourier transform of Gaussian kernel
t_real ft_gaussian(const t_real &x, const t_int &J);
//! \brief Calculates Horner's Rule the standard PSWF for radio astronomy, with a support of J = 6
//! and alpha = 1.
//! \param[in] eta0: value to evaluate
//! \param[in] J: support size of gridding kernel
//! \param[in] alpha: type of special PSWF to calculate
//! \details The tailored prolate spheroidal wave functions for gridding radio astronomy.
//! Details are explained in Optimal Gridding of Visibility Data in Radio
//! Astronomy, F. R. Schwab 1983.
t_real calc_for_pswf(const t_real &x, const t_int &J, const t_real &alpha);
//! PSWF kernel
t_real pswf(const t_real &x, const t_int &J);
//! Fourier transform of PSWF kernel
t_real ft_pswf(const t_real &x, const t_int &J);
//! Calculates samples of a kernel
Vector<t_real> kernel_samples(const t_int &total_samples,
                              const std::function<t_real(t_real)> kernelu, const t_int &J);
//! linearly interpolates from samples of kernel
t_real kernel_linear_interp(const Vector<t_real> &samples, const t_real &x, const t_int &J);
//! Box car function for kernel
t_real pill_box(const t_real &x, const t_int &J);
//! Fourier transform of box car function, a Sinc function
t_real ft_pill_box(const t_real &x, const t_int &J);
//! Fourier transform of general Gaussian kernel
t_real gaussian_general(const t_real &x, const t_int &J, const t_real &sigma);
//! Fourier transform of general Gaussian kernel
t_real ft_gaussian_general(const t_real &x, const t_int &J, const t_real &sigma);
}
}

#endif
