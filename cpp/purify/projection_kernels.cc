#include "purify/projection_kernels.h"
#include <boost/math/special_functions/sinc.hpp>
#include "purify/convolution.h"

namespace purify {
namespace projection_kernels {

std::function<t_complex(t_real, t_real, t_real)> const box_proj() {
  return [=](const t_real &, const t_real &, const t_real &) -> t_complex { return 1.; };
}
std::function<t_complex(t_real, t_real, t_real)> const gauss_proj(const t_real &sigma) {
  return [=](const t_real &u, const t_real &v, const t_real &w) -> t_complex {
    return std::exp(-(u * u + v * v) / (2 * sigma * sigma));
  };
}
inline t_complex w_proj_approx(const t_real &u, const t_real &v, const t_real &w) {
  const t_complex I(0., 1.);
  // du = 1/dl and dv = 1/dm depend on the cell size in directional consine (which ranges from -1 to
  // 1). We are converting this within the kernel to be consistent with
  // exp(-2pi *I *(lu *du + mv *dv + nw))
  return std::exp(-constant::pi * I * (u * u + v * v) / w) / w / I;
}
inline t_complex w_proj_sphere(const t_real &u, const t_real &v, const t_real &w) {

  return 2 * constant::pi
         * boost::math::sinc_pi(2 * constant::pi * std::sqrt(u * u + v * v + w * w))
         * std::exp(2 * constant::pi * t_complex(0., 1.) * w);
};
t_real pixel_to_lambda(const t_real &cell, const t_uint &imsize, const t_real &oversample_ratio) {
  return 60. * 60. * 180. / cell / std::floor(oversample_ratio * imsize) / constant::pi;
}
std::function<t_complex(t_real, t_real, t_real)> const
w_projection_kernel_approx(const t_real &cellx, const t_real &celly, const t_uint &imsizex,
                           const t_uint &imsizey, const t_real &oversample_ratio) {
  const t_real du = pixel_to_lambda(cellx, imsizex, oversample_ratio);
  const t_real dv = pixel_to_lambda(celly, imsizey, oversample_ratio);
  // return gauss_proj(2);
  return [=](const t_real &u, const t_real &v, const t_real &w) -> t_complex {
    if((std::abs(u * du / w) < 1) and (std::abs(v * dv / w) < 1))
      return w_proj_approx(u * du, v * dv, w);
    else {
      if(std::abs(u) < 0.5)
        return 1.;
      else
        return 1.;
    }
  };
}

std::function<t_complex(t_real, t_real, t_real)> const
w_projection_kernel_sphere(const t_real &cellx, const t_real &celly, const t_uint &imsizex,
                           const t_uint &imsizey, const t_real &oversample_ratio) {
  const t_real du = pixel_to_lambda(cellx, imsizex, oversample_ratio);
  const t_real dv = pixel_to_lambda(celly, imsizey, oversample_ratio);
  // return gauss_proj(2);
  return [=](const t_real &u, const t_real &v, const t_real &w) -> t_complex {
    return w_proj_sphere(u * du, v * dv, w);
  };
}
Matrix<t_complex>
projection(const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv,
           const std::function<t_complex(t_real, t_real, t_real)> kernelw, const t_real u,
           const t_real v, const t_real w, const t_int &Ju, const t_int &Jv, const t_int &Jw) {

  // w_projection convolution setup
  const t_complex I(0., 1.);
  t_real const du = u - std::floor(u - Ju * 0.5);
  const auto convol_kernelu = [=](const t_int &ju) -> t_complex {
    return kernelu(du - ju - 1);
    //   * std::exp(-2 * constant::pi * I * ((std::floor(u - Ju * 0.5) + ju - 1) * 0.5));
  };
  t_real const dv = v - std::floor(v - Jv * 0.5);
  const auto convol_kernelv = [=](const t_int &jv) -> t_complex {
    return kernelu(dv - jv - 1);
    //       * std::exp(-2 * constant::pi * I * ((std::floor(v - Jv * 0.5) + jv - 1) * 0.5));
  };
  const t_real dwu = u - std::floor(u - Jw * 0.5);
  const t_real dwv = v - std::floor(v - Jw * 0.5);
  const auto convol_kernelw = [=](const t_int &jv, const t_int &ju) -> t_complex {
    return kernelw(dwu - ju - 1, dwv - jv - 1, w);
    //      * std::exp(-2 * constant::pi * I * ((std::floor(u - Jw * 0.5) + ju - 1) * 0.5))
    //      * std::exp(-2 * constant::pi * I * ((std::floor(v - Jw * 0.5) + jv - 1) * 0.5));
  };
  return convol::linear_convol_2d<t_complex>(convol_kernelu, convol_kernelv, convol_kernelw, Ju, Jv,
                                             Jw, Jw);
}

} // namespace projection_kernels
} // namespace purify
