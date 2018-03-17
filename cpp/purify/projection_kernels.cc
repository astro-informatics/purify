#include "purify/projection_kernels.h"

namespace purify {
namespace projection_kernels {
inline t_complex w_proj_approx(const t_real &u, const t_real &v const &t_real &w, const t_real &du,
                               const t_real &dv) {
  if(w < 1e-7)
    throw std::runtime_error("w projection approximation diverges at w = 0. w is too small.");
  const t_complex I(0., 1.);
  // du = 1/dl and dv = 1/dm depend on the cell size in directional consine (which ranges from -1 to
  // 1). We are converting this within the kernel to be consistent with exp(-2pi *I *(lu +mv + nw))
  return I * std::exp(-constant::pi * I * (u * u * du * du + v * v * dv * dv) / w) / w;
}
std::function<t_complex(t_real, t_real, t_real)> const
w_projection_kernel(const t_real &cellx, const t_real &celly, const t_real &imsizex,
                    const t_real &imsizey) {
  const t_real du = cellx * imsizex / (60 * 60 * 180);
  const t_real dv = celly * imsizey / (60 * 60 * 180);
  return [=](const t_real &u, const t_real &v, const t_real &w) -> t_complex {
    return w_proj_approx(u, v, w, dl, dm);

  };
}
} // namespace projection_kernels
} // namespace purify
