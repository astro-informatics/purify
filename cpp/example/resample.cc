#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/utilities.h"

#include "purify/directories.h"
#include "purify/kernels.h"
#include "purify/pfitsio.h"
#include "purify/spherical_resample.h"
using namespace purify;
using namespace purify::notinstalled;

using namespace purify;

int main(int nargs, char const **args) {
  purify::logging::initialize();
  purify::logging::set_level("debug");

  std::string const fitsfile = image_filename("M31.fits");
  Image<t_complex> const M31 = pfitsio::read2d(fitsfile);
  Vector<t_complex> const image = Vector<t_complex>::Map(M31.data(), M31.size());
  const t_int number_of_samples = 1024 * 1024;
  const t_real theta_0 = constant::pi / 180. * 180.;  // radians
  const t_real phi_0 = constant::pi / 180. * 90.;    // radians
  const t_int imsizey = M31.rows();
  const t_int imsizex = M31.cols();
  const t_real dl = 1. / imsizex;
  const t_real dm = 1. / imsizey;
  const t_int Jl = 4;
  const t_int Jm = 4;
  const t_int num_theta = std::sqrt(number_of_samples);
  const t_int num_phi = std::sqrt(number_of_samples);

  std::function<t_real(t_real)> kernell, kernelm, ftkernell, ftkernelm;
  std::tie(kernell, kernelm, ftkernell, ftkernelm) =
      purify::create_kernels(kernels::kernel_from_string.at("kb"), Jl, Jm, imsizey, imsizex, 1);
  const auto theta = [num_theta](const t_int k) -> t_real {
    return t_real(k % num_theta) / num_theta * 2 * constant::pi;
  };
  const auto phi = [num_theta, num_phi](const t_int k) -> t_real {
    return std::floor(k / num_theta) / num_phi * constant::pi;
  };

  const auto resample_operator =
      spherical_resample::init_resample_operator_2d<Vector<t_complex>,
                                                    std::function<t_real(t_int)>>(
          number_of_samples, theta_0, phi_0, theta, phi, imsizey, imsizex, dl, dm, kernell, kernelm,
          Jl, Jm);
  sopt::LinearTransform<Vector<t_complex>> const resampler =
      sopt::LinearTransform<Vector<t_complex>>(
          std::get<0>(resample_operator), {0, 1, number_of_samples}, std::get<1>(resample_operator),
          {0, 1, imsizex * imsizey});
  const Vector<t_complex> output = resampler.adjoint() * image;

  pfitsio::write2d(Image<t_complex>::Map(output.data(), num_phi, num_theta).real(),
                   "image_on_sphere.fits");
}
