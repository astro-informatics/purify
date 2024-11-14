
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

#include <sopt/power_method.h>

#include <random>
#include "purify/test_data.h"

using namespace purify;
using namespace purify::notinstalled;

double get_sigma(const purify::utilities::vis_params &uv_data, double SNR) {
  double A = std::pow(10, -SNR / 20);
  double norm = uv_data.vis.norm();
  return (A * norm) / sqrt(2 * uv_data.vis.size());
}

TEST_CASE("onnx_Tobias_copy") {
  // Set parameters and options
  double tol = 1e-5;
  size_t iter = 15000;
  size_t update_iter = 100;
  bool record_iters = false;

  // CRR params
  double lambda = 1e4;
  double mu = 20;

  double SNR = 30;

  // get test image
  std::string name = "M31.fits";
  std::string directory = "QAI_imgs/";
  auto ground_truth_path = notinstalled::data_filename(directory + name);
  auto result_path = notinstalled::data_filename(directory + "M31_recon.fits");
  auto residual_path = notinstalled::data_filename(directory + "M31_res.fits");
  auto dirty_path = notinstalled::data_filename(directory + "M31_dirty.fits");
  sopt::Image<t_complex> ground_truth = pfitsio::read2d(ground_truth_path);
  std::cout << "Image size = " << ground_truth.size() << std::endl;

  // get visibility info
  const std::string &vis_data_path =
      notinstalled::data_filename("expected/fb/QAI_visibilities.vis");
  auto uv_data = utilities::read_visibility(vis_data_path, false);
  uv_data.units = utilities::vis_units::radians;

  int imsizex = ground_truth.cols();
  int imsizey = ground_truth.rows();

  // Construct the measurement operator
  // I am not sure that Purify is capable of recreating exactly those used in QantifAI
  // I do not know how purify handles Fourier masking
  Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey);
  auto const Phi = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2,
      kernels::kernel_from_string.at("kb"), 6, 6);
  auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(*Phi, 1000, 1e-5, init);
  const t_real op_norm = std::get<0>(power_method_stuff);

  // Construct synthetic measurements y
  Vector<t_complex> ground_truth_vec = Vector<t_complex>::Map(ground_truth.data(), imsizex*imsizey);
  uv_data.vis = (*Phi) * ground_truth_vec;

  Vector<t_real> uvr = uv_data.vis.cwiseAbs();
  auto max_it = std::max_element(uvr.begin(), uvr.end());
  std::cout << "Max = " << *max_it << std::endl;

  // sigma = 10^(SNR/20) * |image| / (sqrt(2) * n_data)
  double effective_sigma = get_sigma(uv_data, SNR);
  double sigma = effective_sigma * sqrt(2);

  std::mt19937_64 rng;
  std::normal_distribution<double> noise(0, effective_sigma);
  for (size_t i = 0; i < uv_data.vis.size(); i++) {
    uv_data.vis[i] += std::complex<double>(noise(rng), noise(rng));
  }

  auto dirty_vec = (((*Phi).adjoint() * uv_data.vis) / (op_norm*op_norm)).eval();
  auto dirty_image = Image<t_complex>::Map(dirty_vec.data(), imsizey, imsizex).eval();
  pfitsio::write2d(dirty_image.real(), dirty_path);

  // Load CRR (prior + gradient)
  std::string const prior_path =
      purify::notinstalled::data_directory() + "/models/example_cost_dynamic_CRR_sigma_5_t_5.onnx";
  std::string const prior_gradient_path =
      purify::notinstalled::data_directory() + "/models/example_grad_dynamic_CRR_sigma_5_t_5.onnx";
  
  std::shared_ptr<sopt::ONNXDifferentiableFunc<t_complex>> diff_function =
      std::make_shared<sopt::ONNXDifferentiableFunc<t_complex>>(
          prior_path, prior_gradient_path, sigma, mu, lambda, *Phi);

  // Dummy wavelet pointer that we should get rid of
  //std::shared_ptr<const sopt::LinearTransform<Vector<t_complex>>> Psi;
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> Psi;
  double beta = 1;  // also dummied and should be ignored by the algorithm
  double gamma = 1e-4;  // This should be ignored
  auto const fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
      factory::algo_distribution::serial, Phi, Psi, uv_data, sigma, beta,
      gamma, imsizey, imsizex, 1, 1000, true, true, false, 1e-5, 1e-3, 50, op_norm, "",
      factory::g_proximal_type::Indicator, diff_function);

  auto const diagnostic = (*fb)();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);

  // Gen some outputs
  pfitsio::write2d(image.real(), result_path);
}