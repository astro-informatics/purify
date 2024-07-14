
#include "catch2/catch_all.hpp"

#include "purify/config.h"
#include "purify/logging.h"

#include "purify/types.h"
#include "purify/directories.h"
#include "purify/pfitsio.h"
#include "purify/utilities.h"

#include "purify/algorithm_factory.h"
#include "purify/measurement_operator_factory.h"
#include "purify/wavelet_operator_factory.h"
#include <sopt/power_method.h>
#include <sopt/onnx_differentiable_func.h>

#include "purify/test_data.h"

using namespace purify;
using namespace purify::notinstalled;
/*
TEST_CASE("padmm_factory") {
  const std::string &test_dir = "expected/padmm/";
  const std::string &input_data_path = notinstalled::data_filename(test_dir + "input_data.vis");
  const std::string &expected_solution_path =
      notinstalled::data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path =
      notinstalled::data_filename(test_dir + "residual.fits");

  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  CAPTURE(uv_data.vis.head(5));
  REQUIRE(uv_data.size() == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;
  Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey);
  auto const measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2,
      kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(*measurements_transform, 1000, 1e-5, init);
  const t_real op_norm = std::get<0>(power_method_stuff);
  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, sara, imsizey, imsizex);
  t_real const sigma = 0.016820222945913496 * std::sqrt(2);  // see test_parameters file
  auto const padmm = factory::padmm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
      factory::algo_distribution::serial, measurements_transform, wavelets, uv_data, sigma, imsizey,
      imsizex, sara.size(), 300, true, true, false, 1e-2, 1e-3, 50, 1, op_norm);

  auto const diagnostic = (*padmm)();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  // pfitsio::write2d(image.real(), expected_solution_path);
  CAPTURE(Vector<t_complex>::Map(solution.data(), solution.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map(image.data(), image.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map((image / solution).eval().data(), image.size()).real().head(10));
  CHECK(image.isApprox(solution, 1e-4));

  const Vector<t_complex> residuals = measurements_transform->adjoint() *
                                      (uv_data.vis - ((*measurements_transform) * diagnostic.x));
  const Image<t_complex> residual_image = Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
  // pfitsio::write2d(residual_image.real(), expected_residual_path);
  CAPTURE(Vector<t_complex>::Map(residual.data(), residual.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map(residuals.data(), residuals.size()).real().head(10));
  CHECK(residual_image.real().isApprox(residual.real(), 1e-4));
}

// This test does not converge and is therefore set to shouldfail.
// See https://github.com/astro-informatics/purify/issues/317 for details.
TEST_CASE("primal_dual_factory", "[!shouldfail]") {
  const std::string &test_dir = "expected/primal_dual/";
  const std::string &input_data_path = notinstalled::data_filename(test_dir + "input_data.vis");
  const std::string &expected_solution_path =
      notinstalled::data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path =
      notinstalled::data_filename(test_dir + "residual.fits");

  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  CAPTURE(uv_data.vis.head(5));
  REQUIRE(uv_data.size() == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey);
  auto const measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2,
      kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(*measurements_transform, 1000, 1e-5, init);
  const t_real op_norm = std::get<0>(power_method_stuff);
  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, sara, imsizey, imsizex);
  t_real const sigma = 0.016820222945913496 * std::sqrt(2);  // see test_parameters file
  auto const primaldual =
      factory::primaldual_factory<sopt::algorithm::ImagingPrimalDual<t_complex>>(
          factory::algo_distribution::serial, measurements_transform, wavelets, uv_data, sigma,
          imsizey, imsizex, sara.size(), 20, true, true, 1e-2, 1, op_norm);

  auto const diagnostic = (*primaldual)();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  // pfitsio::write2d(image.real(), expected_solution_path);
  CAPTURE(Vector<t_complex>::Map(solution.data(), solution.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map(image.data(), image.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map((image / solution).eval().data(), image.size()).real().head(10));
  CHECK(image.isApprox(solution, 1e-4));

  const Vector<t_complex> residuals = measurements_transform->adjoint() *
                                      (uv_data.vis - ((*measurements_transform) * diagnostic.x));
  const Image<t_complex> residual_image = Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
  // pfitsio::write2d(residual_image.real(), expected_residual_path);
  CAPTURE(Vector<t_complex>::Map(residual.data(), residual.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map(residuals.data(), residuals.size()).real().head(10));
  CHECK(residual_image.real().isApprox(residual.real(), 1e-4));
}
*/
TEST_CASE("fb_factory") {
  const std::string &test_dir = "expected/fb/";
  const std::string &input_data_path = notinstalled::data_filename(test_dir + "input_data.vis");
  const std::string &expected_solution_path =
      notinstalled::data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path =
      notinstalled::data_filename(test_dir + "residual.fits");

  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  CAPTURE(uv_data.vis.head(5));
  REQUIRE(uv_data.size() == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey);
  auto const measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2,
      kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(*measurements_transform, 1000, 1e-5, init);
  const t_real op_norm = std::get<0>(power_method_stuff);
  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, sara, imsizey, imsizex);
  t_real const sigma = 0.016820222945913496 * std::sqrt(2);  // see test_parameters file
  t_real const beta = sigma * sigma;
  t_real const gamma = 0.0001;
  auto const fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
      factory::algo_distribution::serial, measurements_transform, wavelets, uv_data, sigma, beta,
      gamma, imsizey, imsizex, sara.size(), 1000, true, true, false, 1e-2, 1e-3, 50, op_norm);

  auto const diagnostic = (*fb)();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  // pfitsio::write2d(image.real(), expected_solution_path);
  CAPTURE(Vector<t_complex>::Map(solution.data(), solution.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map(image.data(), image.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map((image / solution).eval().data(), image.size()).real().head(10));
  CHECK(image.isApprox(solution, 1e-4));

  const Vector<t_complex> residuals = measurements_transform->adjoint() *
                                      (uv_data.vis - ((*measurements_transform) * diagnostic.x));
  const Image<t_complex> residual_image = Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
  // pfitsio::write2d(residual_image.real(), expected_residual_path);
  CAPTURE(Vector<t_complex>::Map(residual.data(), residual.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map(residuals.data(), residuals.size()).real().head(10));
  CHECK(residual_image.real().isApprox(residual.real(), 1e-4));
}

TEST_CASE("onnx_fb_factory") {
  const std::string &test_dir = "expected/fb/";
  const std::string &input_data_path = notinstalled::data_filename(test_dir + "input_data.vis");
  const std::string &expected_solution_path =
      notinstalled::data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path =
      notinstalled::data_filename(test_dir + "residual.fits");

  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  CAPTURE(uv_data.vis.head(5));
  REQUIRE(uv_data.size() == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey);
  auto const measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2,
      kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(*measurements_transform, 1000, 1e-5, init);
  const t_real op_norm = std::get<0>(power_method_stuff);
  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, sara, imsizey, imsizex);
  t_real const sigma = 0.016820222945913496 * std::sqrt(2);  // see test_parameters file
  t_real const beta = sigma * sigma;
  t_real const gamma = 0.0001;
        //TODO: Fix paths? Add directories.h to sopt interface somehow
  std::string model_dir = "/home/mmcleod/lexci/sopt/cpp/tests/test_data";
  std::string const prior_path = model_dir + "/example_cost_CRR_sigma_5_t_5.onnx";
  std::string const prior_gradient_path = model_dir + "/example_grad_CRR_sigma_5_t_5.onnx";   
  std::shared_ptr<sopt::ONNXDifferentiableFunc<t_complex>> diff_function = std::make_shared<sopt::ONNXDifferentiableFunc<t_complex>>(prior_path, prior_gradient_path, sigma, 20, 5e4, *measurements_transform);

  auto const fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
      factory::algo_distribution::serial, measurements_transform, wavelets, uv_data, sigma, beta,
      gamma, imsizey, imsizex, sara.size(), 1000, true, true, false, 1e-2, 1e-3, 50, op_norm, "", factory::g_proximal_type::Indicator, diff_function);


  auto const diagnostic = (*fb)();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  // pfitsio::write2d(image.real(), expected_solution_path);
  CAPTURE(Vector<t_complex>::Map(solution.data(), solution.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map(image.data(), image.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map((image / solution).eval().data(), image.size()).real().head(10));
  CHECK(image.isApprox(solution, 1e-4));

  const Vector<t_complex> residuals = measurements_transform->adjoint() *
                                      (uv_data.vis - ((*measurements_transform) * diagnostic.x));
  const Image<t_complex> residual_image = Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
  // pfitsio::write2d(residual_image.real(), expected_residual_path);
  CAPTURE(Vector<t_complex>::Map(residual.data(), residual.size()).real().head(10));
  CAPTURE(Vector<t_complex>::Map(residuals.data(), residuals.size()).real().head(10));
  CHECK(residual_image.real().isApprox(residual.real(), 1e-4));
}

/*
TEST_CASE("joint_map_factory") {
  const std::string &test_dir = "expected/joint_map/";
  const std::string &input_data_path = notinstalled::data_filename(test_dir + "input_data.vis");
  const std::string &expected_solution_path =
      notinstalled::data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path =
      notinstalled::data_filename(test_dir + "residual.fits");

  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  CAPTURE(uv_data.vis.head(5));
  REQUIRE(uv_data.size() == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey);
  auto const measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2,
      kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(*measurements_transform, 1000, 1e-5, init);
  const t_real op_norm = std::get<0>(power_method_stuff);
  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, sara, imsizey, imsizex);
  t_real const sigma = 0.016820222945913496 * std::sqrt(2);  // see test_parameters file
  t_real const beta = sigma * sigma;
  t_real const gamma = 1;
  auto const fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
      factory::algo_distribution::serial, measurements_transform, wavelets, uv_data, sigma, beta,
      gamma, imsizey, imsizex, sara.size(), 1000, true, true, false, 1e-2, 1e-3, 50, op_norm);
  auto const l1_norm = [wavelets](const Vector<t_complex> &x) {
    auto val = sopt::l1_norm(wavelets->adjoint() * x);
    return val;
  };
  auto const joint_map =
      sopt::algorithm::JointMAP<sopt::algorithm::ImagingForwardBackward<t_complex>>(
          fb, l1_norm, imsizex * imsizey * sara.size())
          .relative_variation(1e-3)
          .objective_variation(1e-3)
          .beta(1.)
          .alpha(1.);
  auto const diagnostic = joint_map();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  //  CAPTURE(Vector<t_complex>::Map(solution.data(), solution.size()).real().head(10));
  //  CAPTURE(Vector<t_complex>::Map(image.data(), image.size()).real().head(10));
  //  CAPTURE(Vector<t_complex>::Map((image / solution).eval().data(),
  //  image.size()).real().head(10));
  // CHECK(image.isApprox(solution, 1e-6));

  const Vector<t_complex> residuals = measurements_transform->adjoint() *
                                      (uv_data.vis - ((*measurements_transform) * diagnostic.x));
  const Image<t_complex> residual_image = Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
  //  CAPTURE(Vector<t_complex>::Map(residual.data(), residual.size()).real().head(10));
  //  CAPTURE(Vector<t_complex>::Map(residuals.data(), residuals.size()).real().head(10));
  // CHECK(residual_image.real().isApprox(residual.real(), 1e-6));
}
*/