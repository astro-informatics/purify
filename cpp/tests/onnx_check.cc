
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

#include "purify/test_data.h"

using namespace purify;
using namespace purify::notinstalled;

template <typename T>
double get_sigma(sopt::Image<T> image, double SNR) {
  double A = std::pow(10, -SNR / 20);
  double norm = sopt::Vector<T>::Map(image.data(), image.rows * image.cols).norm();
  return (A * norm) / sqrt(2 * image.size);
}

TEST_CASE("onnx_Tobias_copy") {
  // Set parameters and options
  double tol = 1e-5;
  size_t iter = 15000;
  size_t update_iter = 100;
  bool record_iters = false;

  // CRR params
  double lambda = 5e4;
  double mu = 20;

  double SNR = 30;

  // get test image
  std::string name = "M31.fits";
  std::string directory = "QAI_imgs/";
  auto path = notinstalled::data_filename(directory + name);
  sopt::Image<t_complex> ground_truth = pfitsio::read2d(path);
  std::cout << "Image size = " << ground_truth.size() << std::endl;

  // get visibility info
  const std::string &vis_data_path =
      notinstalled::data_filename("expected/fb/QAI_visibilities.vis");
  auto uv_data = utilities::read_visibility(vis_data_path, false);

  int imsizex = ground_truth.cols();
  int imsizey = ground_truth.rows();

  // Construct the measurement operator
  // I am not sure that Purify is capable of recreating exactly those used in QantifAI
  // I do not know how purify handles Fourier masking
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

  // Construct synthetic measurements y
  uv_data.vis = (*measurements_transform) * ground_truth;

  // sigma = 10^(SNR/20) * |image| / (sqrt(2) * n_data)
  double sigma;  // = get_sigma(..., SNR);

  // Load CRR (prior + gradient)
  std::string const prior_path =
      purify::notinstalled::data_directory() + "/models/example_cost_dynamic_CRR_sigma_5_t_5.onnx";
  std::string const prior_gradient_path =
      purify::notinstalled::data_directory() + "/models/example_grad_dynamic_CRR_sigma_5_t_5.onnx";
  // std::shared_ptr<sopt::ONNXDifferentiableFunc<t_complex>> diff_function =
  //     std::make_shared<sopt::ONNXDifferentiableFunc<t_complex>>(
  //         prior_path, prior_gradient_path, sigma, 20, 5e4, *measurements_transform);
}