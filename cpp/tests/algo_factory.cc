
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

#ifdef PURIFY_ONNXRT
#include <sopt/onnx_differentiable_func.h>
#endif

#ifdef PURIFY_H5
#include "purify/h5reader.h"
#endif

#include <sopt/gradient_utils.h>
#include <sopt/power_method.h>

#include "purify/test_data.h"

using namespace purify;

TEST_CASE("padmm_factory") {
  const std::string &test_dir = "expected/padmm/";
  const std::string &input_data_path = data_filename(test_dir + "input_data.vis");
  const std::string &expected_solution_path = data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path = data_filename(test_dir + "residual.fits");

  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  CAPTURE(uv_data.vis.head(5));
  REQUIRE(uv_data.size() == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;
  Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey);
  auto measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2,
      kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(*measurements_transform, 1000, 1e-5, init);
  const t_real op_norm = std::get<0>(power_method_stuff);
  measurements_transform->set_norm(op_norm);

  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, sara, imsizey, imsizex);
  t_real const sigma = 0.016820222945913496 * std::sqrt(2);  // see test_parameters file
  auto const padmm = factory::padmm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
      factory::algo_distribution::serial, measurements_transform, wavelets, uv_data, sigma, imsizey,
      imsizex, sara.size(), 300, true, true, false, 1e-2, 1e-3, 50, 1);

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
  const std::string &input_data_path = data_filename(test_dir + "input_data.vis");
  const std::string &expected_solution_path = data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path = data_filename(test_dir + "residual.fits");

  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  CAPTURE(uv_data.vis.head(5));
  REQUIRE(uv_data.size() == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey);
  auto measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2,
      kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(*measurements_transform, 1000, 1e-5, init);
  const t_real op_norm = std::get<0>(power_method_stuff);
  measurements_transform->set_norm(op_norm);

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
          imsizey, imsizex, sara.size(), 20, true, true, 1e-2, 1);

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

TEST_CASE("fb_factory") {
  const std::string &test_dir = "expected/fb/";
  const std::string &input_data_path = data_filename(test_dir + "input_data.vis");
  const std::string &expected_solution_path = data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path = data_filename(test_dir + "residual.fits");
  const std::string &result_path = data_filename(test_dir + "fb_result.fits");

  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  CAPTURE(uv_data.vis.head(5));
  REQUIRE(uv_data.size() == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey);
  auto measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2,
      kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(*measurements_transform, 1000, 1e-5, init);
  const t_real op_norm = std::get<0>(power_method_stuff);
  measurements_transform->set_norm(op_norm);

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
      gamma, imsizey, imsizex, sara.size(), 1000, true, true, false, 1e-2, 1e-3, 50);

  auto const diagnostic = (*fb)();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  pfitsio::write2d(image.real(), result_path);
  // pfitsio::write2d(residual_image.real(), expected_residual_path);

  double average_intensity = diagnostic.x.real().sum() / diagnostic.x.size();
  SOPT_HIGH_LOG("Average intensity = {}", average_intensity);
  double mse = (Vector<t_complex>::Map(solution.data(), solution.size()) - diagnostic.x)
                   .real()
                   .squaredNorm() /
               solution.size();
  SOPT_HIGH_LOG("MSE = {}", mse);
  CHECK(mse <= average_intensity * 1e-3);
}

#ifdef PURIFY_H5
TEST_CASE("fb_factory_stochastic") {
  const std::string &test_dir = "expected/fb/";
  const std::string &input_data_path = data_filename(test_dir + "input_data.vis");
  const std::string &expected_solution_path = data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path = data_filename(test_dir + "residual.fits");
  const std::string &result_path = data_filename(test_dir + "fb_result_stochastic.fits");

  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  CAPTURE(uv_data.vis.head(5));
  REQUIRE(uv_data.size() == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  // This functor would be defined in Purify
  std::mt19937 rng(0);
  const size_t N = 1000;
  std::function<std::shared_ptr<sopt::IterationState<Vector<t_complex>>>()> random_updater =
      [&input_data_path, imsizex, imsizey, &rng, &N]() {
        utilities::vis_params uv_data = utilities::read_visibility(input_data_path, false);
        uv_data.units = utilities::vis_units::radians;

        // Get random subset
        std::vector<size_t> indices(uv_data.size());
        size_t i = 0;
        for (auto &x : indices) {
          x = i++;
        }

        std::shuffle(indices.begin(), indices.end(), rng);
        Vector<t_real> u_fragment(N);
        Vector<t_real> v_fragment(N);
        Vector<t_real> w_fragment(N);
        Vector<t_complex> vis_fragment(N);
        Vector<t_complex> weights_fragment(N);
        for (i = 0; i < N; i++) {
          size_t j = indices[i];
          u_fragment[i] = uv_data.u[j];
          v_fragment[i] = uv_data.v[j];
          w_fragment[i] = uv_data.w[j];
          vis_fragment[i] = uv_data.vis[j];
          weights_fragment[i] = uv_data.weights[j];
        }
        utilities::vis_params uv_data_fragment(u_fragment, v_fragment, w_fragment, vis_fragment,
                                               weights_fragment, uv_data.units, uv_data.ra,
                                               uv_data.dec, uv_data.average_frequency);

        auto phi = factory::measurement_operator_factory<Vector<t_complex>>(
            factory::distributed_measurement_operator::serial, uv_data_fragment, imsizey, imsizex,
            1, 1, 2, kernels::kernel_from_string.at("kb"), 4, 4);

        Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey);
        auto const power_method_stuff =
            sopt::algorithm::power_method<Vector<t_complex>>(Phi, 1000, 1e-5, init);
        const t_real op_norm = std::get<0>(power_method_stuff);
        phi->set_norm(op_norm);

        return std::make_shared<sopt::IterationState<Vector<t_complex>>>(uv_data_fragment.vis, phi);
      };

  const auto solution = pfitsio::read2d(expected_solution_path);

  // wavelets
  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, sara, imsizey, imsizex);

  // algorithm
  t_real const sigma = 0.016820222945913496 * std::sqrt(2);  // see test_parameters file
  t_real const beta = sigma * sigma;
  t_real const gamma = 0.0001;

  sopt::algorithm::ImagingForwardBackward<t_complex> fb(random_updater);
  fb.itermax(1000)
      .step_size(beta * sqrt(2))
      .sigma(sigma * sqrt(2))
      .regulariser_strength(gamma)
      .relative_variation(1e-3)
      .residual_tolerance(0)
      .tight_frame(true);

  auto gp = std::make_shared<sopt::algorithm::L1GProximal<t_complex>>(false);
  gp->l1_proximal_tolerance(1e-4)
      .l1_proximal_nu(1)
      .l1_proximal_itermax(50)
      .l1_proximal_positivity_constraint(true)
      .l1_proximal_real_constraint(true)
      .Psi(*wavelets);
  fb.g_function(gp);

  auto const diagnostic = fb();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  // pfitsio::write2d(image.real(), result_path);
  // pfitsio::write2d(residual_image.real(), expected_residual_path);

  auto soln_flat = Vector<t_complex>::Map(solution.data(), solution.size());
  double average_intensity = soln_flat.real().sum() / soln_flat.size();
  SOPT_HIGH_LOG("Average intensity = {}", average_intensity);
  double mse = (soln_flat - diagnostic.x).real().squaredNorm() / solution.size();
  SOPT_HIGH_LOG("MSE = {}", mse);
  CHECK(mse <= average_intensity * 1e-3);
}
#endif

#ifdef PURIFY_ONNXRT
TEST_CASE("tf_fb_factory") {
  const std::string &test_dir = "expected/fb/";
  const std::string &input_data_path = data_filename(test_dir + "input_data.vis");
  const std::string &expected_solution_path = data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path = data_filename(test_dir + "residual.fits");
  const std::string &result_path = data_filename(test_dir + "tf_result.fits");

  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  CAPTURE(uv_data.vis.head(5));
  REQUIRE(uv_data.size() == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey);
  auto measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2,
      kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(*measurements_transform, 1000, 1e-5, init);
  const t_real op_norm = std::get<0>(power_method_stuff);
  measurements_transform->set_norm(op_norm);

  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, sara, imsizey, imsizex);
  t_real const sigma = 0.016820222945913496 * std::sqrt(2);  // see test_parameters file
  t_real const beta = sigma * sigma;
  t_real const gamma = 0.0001;

  std::string tf_model_path = purify::models_directory() + "/snr_15_model_dynamic.onnx";

  auto const fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
      factory::algo_distribution::serial, measurements_transform, wavelets, uv_data, sigma, beta,
      gamma, imsizey, imsizex, sara.size(), 1000, true, true, false, 1e-2, 1e-3, 50,
      tf_model_path, nondiff_func_type::Denoiser);

  auto const diagnostic = (*fb)();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  // pfitsio::write2d(image.real(), result_path);
  // pfitsio::write2d(residual_image.real(), expected_residual_path);

  double average_intensity = diagnostic.x.real().sum() / diagnostic.x.size();
  SOPT_HIGH_LOG("Average intensity = {}", average_intensity);
  double mse = (Vector<t_complex>::Map(solution.data(), solution.size()) - diagnostic.x)
                   .real()
                   .squaredNorm() /
               solution.size();
  SOPT_HIGH_LOG("MSE = {}", mse);
  CHECK(mse <= average_intensity * 1e-3);
}

TEST_CASE("onnx_fb_factory") {
  const std::string &test_dir = "expected/fb/";
  const std::string &input_data_path = data_filename(test_dir + "input_data.vis");
  const std::string &expected_solution_path = data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path = data_filename(test_dir + "residual.fits");
  const std::string &result_path = data_filename(test_dir + "onnx_result.fits");
  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  CAPTURE(uv_data.vis.head(5));
  REQUIRE(uv_data.size() == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey);
  auto measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2,
      kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(*measurements_transform, 1000, 1e-5, init);
  const t_real op_norm = std::get<0>(power_method_stuff);
  measurements_transform->set_norm(op_norm);

  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, sara, imsizey, imsizex);
  t_real const sigma = 0.016820222945913496 * std::sqrt(2);  // see test_parameters file
  t_real const beta = sigma * sigma;
  t_real const gamma = 0.0001;

  std::string const prior_path =
      purify::models_directory() + "/example_cost_dynamic_CRR_sigma_5_t_5.onnx";
  std::string const prior_gradient_path =
      purify::models_directory() + "/example_grad_dynamic_CRR_sigma_5_t_5.onnx";
  std::shared_ptr<sopt::ONNXDifferentiableFunc<t_complex>> diff_function =
      std::make_shared<sopt::ONNXDifferentiableFunc<t_complex>>(
          prior_path, prior_gradient_path, sigma, 20, 5e4, *measurements_transform);

  auto const fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
      factory::algo_distribution::serial, measurements_transform, wavelets, uv_data, sigma, beta,
      gamma, imsizey, imsizex, sara.size(), 1000, true, true, false, 1e-2, 1e-3, 50, "",
      nondiff_func_type::RealIndicator, diff_function);

  auto const diagnostic = (*fb)();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  // pfitsio::write2d(image.real(), result_path);
  // pfitsio::write2d(residual_image.real(), expected_residual_path);

  double average_intensity = diagnostic.x.real().sum() / diagnostic.x.size();
  SOPT_HIGH_LOG("Average intensity = {}", average_intensity);
  double mse = (Vector<t_complex>::Map(solution.data(), solution.size()) - diagnostic.x)
                   .real()
                   .squaredNorm() /
               solution.size();
  SOPT_HIGH_LOG("MSE = {}", mse);
  CHECK(mse <= average_intensity * 1e-3);
}
#endif

TEST_CASE("joint_map_factory") {
  const std::string &test_dir = "expected/joint_map/";
  const std::string &input_data_path = data_filename(test_dir + "input_data.vis");
  const std::string &expected_solution_path = data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path = data_filename(test_dir + "residual.fits");

  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  CAPTURE(uv_data.vis.head(5));
  REQUIRE(uv_data.size() == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey);
  auto measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2,
      kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(*measurements_transform, 1000, 1e-5, init);
  const t_real op_norm = std::get<0>(power_method_stuff);
  measurements_transform->set_norm(op_norm);

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
      gamma, imsizey, imsizex, sara.size(), 1000, true, true, false, 1e-2, 1e-3, 50);
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
