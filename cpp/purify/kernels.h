#ifndef PURIFY_KERNELS_H
#define PURIFY_KERNELS_H

#include "purify/config.h"
#include "purify/types.h"
#include <array>
#include <map>
#include <tuple>
#include <boost/math/special_functions/bessel.hpp>
#include <boost/math/special_functions/sinc.hpp>

namespace purify {

namespace kernels {
enum class kernel { kb, gauss, box, pswf, kbmin, gauss_alt, kb_presample };
const std::map<std::string, kernel> kernel_from_string = {{"kb", kernel::kb},
                                                          {"gauss", kernel::gauss},
                                                          {"box", kernel::box},
                                                          {"pswf", kernel::pswf},
                                                          {"kbmin", kernel::kbmin},
                                                          {"kb_presample", kernel::kb_presample},
                                                          {"gauss_alt", kernel::gauss_alt}};

//! Kaiser-Bessel kernel
t_real kaiser_bessel(const t_real x, const t_real J);
//! More general Kaiser-Bessel kernel
t_real kaiser_bessel_general(const t_real x, const t_real J, const t_real alpha);
//!  Fourier transform of more general Kaiser-Bessel kernel
t_real ft_kaiser_bessel_general(const t_real x, const t_real J, const t_real alpha);
//! Fourier transform of kaiser bessel kernel
t_real ft_kaiser_bessel(const t_real x, const t_real J);
//! Gaussian kernel
t_real gaussian(const t_real x, const t_real J);
//! Fourier transform of Gaussian kernel
t_real ft_gaussian(const t_real x, const t_real J);
//! \brief Calculates Horner's Rule the standard PSWF for radio astronomy, with a support of J = 6
//! and alpha = 1.
//! \param[in] eta0: value to evaluate
//! \param[in] J: support size of gridding kernel
//! \param[in] alpha: type of special PSWF to calculate
//! \details The tailored prolate spheroidal wave functions for gridding radio astronomy.
//! Details are explained in Optimal Gridding of Visibility Data in Radio
//! Astronomy, F. R. Schwab 1983.
t_real calc_for_pswf(const t_real x, const t_real J, const t_real alpha);
//! PSWF kernel
t_real pswf(const t_real x, const t_real J);
//! Fourier transform of PSWF kernel
t_real ft_pswf(const t_real x, const t_real J);
//! Calculates samples of a kernel
Vector<t_real> kernel_samples(const t_real total_samples,
                              const std::function<t_real(t_real)> kernelu, const t_real J);
//! zeroth order interpolates from samples of kernel
t_real kernel_zero_interp(const Vector<t_real> &samples, const t_real x, const t_real J);
//! linearly interpolates from samples of kernel
t_real kernel_linear_interp(const Vector<t_real> &samples, const t_real x, const t_real J);
//! Box car function for kernel
t_real pill_box(const t_real x, const t_real J);
//! Fourier transform of box car function, a Sinc function
t_real ft_pill_box(const t_real x, const t_real J);
//! Fourier transform of general Gaussian kernel
t_real gaussian_general(const t_real x, const t_real J, const t_real sigma);
//! Fourier transform of general Gaussian kernel
t_real ft_gaussian_general(const t_real x, const t_real J, const t_real sigma);
}  // namespace kernels
std::tuple<std::function<t_real(t_real)>, std::function<t_real(t_real)>,
           std::function<t_real(t_real)>, std::function<t_real(t_real)>>
create_kernels(const kernels::kernel kernel_name, const t_uint Ju_, const t_uint Jv_,
               const t_real ftsizeu_, const t_real ftsizev_, const t_real oversample_ratio);
std::tuple<std::function<t_real(t_real)>, std::function<t_real(t_real)>> create_radial_ftkernel(
    const kernels::kernel kernel_name_, const t_uint Ju_, const t_real oversample_ratio);
}  // namespace purify

#endif
