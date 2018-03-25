#ifndef PURIFY_PROJECTION_KERNELS_H
#define PURIFY_PROJECTION_KERNELS_H

#include "purify/config.h"
#include "purify/logging.h"
#include "purify/types.h"

namespace purify {
namespace projection_kernels {
//! box kernel (which would also simulate an antenna)
std::function<t_complex(t_real, t_real, t_real)> const box_proj();
//! gaussian kernel
std::function<t_complex(t_real, t_real, t_real)> const gauss_proj(const t_real &sigma);
//! return factors to convert between pixels and lambda
t_real pixel_to_lambda(const t_real &cell, const t_uint &imsize, const t_real &oversample_ratio);
//! parabloic approximation for curvature
inline t_complex w_proj_approx(const t_real &u, const t_real &v, const t_real &w);

//! exact expression for curvature
inline t_complex w_proj_sphere(const t_real &u, const t_real &v, const t_real &w);
//! returns lambda for w projection kernel
std::function<t_complex(t_real, t_real, t_real)> const
w_projection_kernel_approx(const t_real &cellx, const t_real &celly, const t_uint &imsizex,
                           const t_uint &imsizey, const t_real &oversample_ratio);
//! returns lambda for spherical w projection kernel
std::function<t_complex(t_real, t_real, t_real)> const
w_projection_kernel_sphere(const t_real &cellx, const t_real &celly, const t_uint &imsizex,
                           const t_uint &imsizey, const t_real &oversample_ratio);
//! algorithm to calculate the wprojection kernel
Matrix<t_complex>
projection(const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv,
           const std::function<t_complex(t_real, t_real, t_real)> kernelw, const t_real u,
           const t_real v, const t_real w, const t_int &Ju, const t_int &Jv, const t_int &Jw);
} // namespace projection_kernels
} // namespace purify

#endif
