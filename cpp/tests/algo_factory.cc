
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"

#include "purify/algorithm_factory.h"

#include "purify/test_data.h"
using namespace purify;
using namespace purify::notinstalled;


int main(int, char **) {
  sopt::logging::initialize();
  purify::logging::initialize();
  sopt::logging::set_level("debug");
  purify::logging::set_level("debug");
  const t_real snr = 10;
  const std::string test_dir = "expected/kernels/";
  const std::string input_data = notinstalled::data_filename(test_dir + "test_data.vis");
  auto uv_data = utilities::read_visibility(input_data);
  uv_data.units = utilities::vis_units::radians;

  // working out value of signal given SNR of 10
  t_real const sigma = utilities::SNR_to_standard_deviation(uv_data.vis, snr);
  t_uint const imsizey = 256;
  t_uint const imsizex = 256;

  auto const measurements_transform
      = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_data, imsizey, imsizex, 1, 1, 2, 100,
          0.0001, kernels::kernel_from_string.at("kb"), 4, 4, operators::fftw_plan::measure, false);
  sopt::wavelets::SARA const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  auto const wavelets = std::make_shared(sopt::linear_transform<t_complex>(sara, imsizey, imsizex));

  auto const epsilon = utilities::calculate_l2_radius(uv_data.vis, sigma);
  PURIFY_HIGH_LOG("Using epsilon of {}", epsilon);
  auto const padmm
      =   factory::algorithm_factory(factory::algorithm::padmm, factory::algo_distribution::serial,
          measurements_transform, wavelets, uv_data, sigma, imsizey, imsizex);
  auto const diagnostic = padmm();
  assert(diagnostic.x.size() == M31.size());
  Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  Vector<t_complex> residuals = measurements_transform->adjoint()
                                * (uv_data.vis - ((*measurements_transform) * diagnostic.x));
  Image<t_complex> residual_image = Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
  return 0;
}
