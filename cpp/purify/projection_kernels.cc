#include "purify/projection_kernels.h"
#include "purify/convolution.h"

namespace purify {
namespace projection_kernels {
inline t_complex w_proj_approx(const t_real &u, const t_real &v, const t_real &w, const t_real &du,
                               const t_real &dv) {
  if(std::abs(w) < 1e-12)
    throw std::runtime_error("w projection approximation diverges at w = 0. w is too small.");
  const t_complex I(0., 1.);
  // du = 1/dl and dv = 1/dm depend on the cell size in directional consine (which ranges from -1 to
  // 1). We are converting this within the kernel to be consistent with exp(-2pi *I *(lu +mv + nw))
  return I * std::exp(-constant::pi * I * (u * u * du * du + v * v * dv * dv) / w) / w;
}
std::function<t_complex(t_real, t_real, t_real)> const
w_projection_kernel(const t_real &cellx, const t_real &celly, const t_uint &imsizex,
                    const t_uint &imsizey, const t_real &oversample_ratio) {
  const t_real du = cellx * imsizex * oversample_ratio / (60 * 60 * 180);
  const t_real dv = celly * imsizey * oversample_ratio / (60 * 60 * 180);
  return [=](const t_real &u, const t_real &v, const t_real &w) -> t_complex {
    return w_proj_approx(u, v, w, du, dv);

  };
}

Matrix<t_complex>
projection(const std::function<t_real(t_real)> kernelv, const std::function<t_real(t_real)> kernelu,
           const std::function<t_complex(t_real, t_real, t_real)> kernelw, const t_real u,
           const t_real v, const t_real w, const t_int &Ju, const t_int &Jv, const t_int &Jw) {

  // w_projection convolution setup
  t_real const du = u - std::floor(u - Ju * 0.5);
  const auto convol_kernelu = [=](const t_int &ju) -> t_complex { return kernelu(du - ju - 1); };
  t_real const dv = v - std::floor(v - Jv * 0.5);
  const auto convol_kernelv = [=](const t_int &jv) -> t_complex { return kernelu(dv - jv - 1); };
  const t_real dwu = u - std::floor(u - Jw * 0.5);
  const t_real dwv = v - std::floor(v - Jw * 0.5);
  const auto convol_kernelw = [=](const t_int &ju, const t_int &jv) -> t_complex {
    return kernelw(dwu - ju - 1, dwv - jv - 1, w);
  };
  return convol::linear_convol_2d<t_complex>(convol_kernelu, convol_kernelv, convol_kernelw, Ju, Jv,
                                             Jw, Jw);
}

} // namespace projection_kernels
} // namespace purify
