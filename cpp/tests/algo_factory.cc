
#include "catch.hpp"

#include "purify/config.h"
#include "purify/logging.h"

#include "purify/utilities.h"
#include "purify/directories.h"
#include "purify/pfitsio.h"
#include "purify/types.h"

#include "purify/algorithm_factory.h"
#include "purify/measurement_operator_factory.h"
#include "purify/wavelet_operator_factory.h"

#include "purify/test_data.h"

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("padmm_factory"){
  const std::string &test_dir = "expected/padmm/";
  const std::string &input_data_path = notinstalled::data_filename(test_dir + "input_data.vis");
  const std::string &expected_solution_path = notinstalled::data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path = notinstalled::data_filename(test_dir + "residual.fits");

  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  CAPTURE(uv_data.vis.head(5));
  REQUIRE(uv_data.size() == 13107);

  t_uint const imsizey = 256;
  t_uint const imsizex = 256;

  auto const measurements_transform
      = factory::measurement_operator_factory<Vector<t_complex>>(
          factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2, 100,
          0.0001, kernels::kernel_from_string.at("kb"), 4, 4);
  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(factory::distributed_wavelet_operator::serial, sara, imsizey, imsizex);
  t_real const sigma = 0.02378738741225; //see test_parameters file
  auto const padmm
      =   factory::algorithm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
          factory::algorithm::padmm, factory::algo_distribution::serial,
          measurements_transform, wavelets, uv_data, sigma, imsizey, imsizex, sara.size(), 500);

  auto const diagnostic = (*padmm)();
  CHECK(diagnostic.niters == 139);
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  CAPTURE(Vector<t_complex>::Map(solution.data(), solution.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map(image.data(), image.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map((image/solution).eval().data(), image.size()).real().head(10));
  CHECK(image.isApprox(solution, 1e-6));

  const Vector<t_complex> residuals = measurements_transform->adjoint()
                                * (uv_data.vis - ((*measurements_transform) * diagnostic.x));
  const Image<t_complex> residual_image = Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
  CAPTURE(Vector<t_complex>::Map(residual.data(), residual.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map(residuals.data(), residuals.size()).real().head(10));
  CHECK(residual_image.real().isApprox(residual.real(), 1e-6));
  
}
