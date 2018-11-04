#ifndef PURIFY_WKERNEL_INTEGRATION_H
#define PURIFY_WKERNEL_INTEGRATION_H

#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"

#include "purify/integration.h"
namespace purify {
namespace projection_kernels {
//! exact fourier phase
t_complex phase(const t_real u, const t_real v, const t_real l, const t_real m);
//! exact chirp expression
t_complex chirp(const t_real w, const t_real l, const t_real m);
//! numerical integration of chirp and radial symmetric kernel in the image domain using
//! hankel transform
t_complex exact_w_projection_integration_1d(const t_real &u, const t_real &v, const t_real &w,
                                            const t_real du,
                                            const std::function<t_complex(t_real)> &ftkerneluv,
                                            const t_uint &max_evaluations,
                                            const t_real &absolute_error,
                                            const t_real &relative_error,
                                            const integration::method method, t_uint &evaluations);
//! numerical integration of chirp and kernel in image domain
t_complex exact_w_projection_integration(const t_real &u, const t_real &v, const t_real &w,
                                         const t_real du, const t_real dv,
                                         const std::function<t_complex(t_real)> &ftkernelu,
                                         const std::function<t_complex(t_real)> &ftkernelv,
                                         const t_uint &max_evaluations,
                                         const t_real &absolute_error, const t_real &relative_error,
                                         const integration::method method, const bool radial,
                                         t_uint &evaluations);
}  // namespace projection_kernels
}  // namespace purify
#endif
