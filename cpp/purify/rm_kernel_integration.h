#ifndef PURIFY_RM_KERNEL_INTEGRATION_H
#define PURIFY_RM_KERNEL_INTEGRATION_H
#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"

#include "purify/integration.h"
namespace purify {

namespace projection_kernels {
//! fourier integrand with sinc function
t_complex fourier_rm_kernel(const t_real x, const t_real dlambda2, const t_real u, const t_real du);
//! integrate fourier integrand to get rm gridding kernel
t_complex exact_rm_projection_integration(const t_real u, const t_real dlambda2, const t_real du,
                                          const t_real oversample_ratio,
                                          const std::function<t_complex(t_real)> &ftkernelu,
                                          const t_uint max_evaluations, const t_real absolute_error,
                                          const t_real relative_error,
                                          const integration::method method, t_uint &evaluations);
}  // namespace projection_kernels
// namespace projection_kernels
}  // namespace purify
#endif
