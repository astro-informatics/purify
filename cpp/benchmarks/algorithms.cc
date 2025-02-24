#include "purify/config.h"
#include "purify/types.h"
#include <array>
#include <benchmark/benchmark.h>
#include "benchmarks/utilities.h"
#include "purify/algorithm_factory.h"
#include "purify/directories.h"
#include "purify/measurement_operator_factory.h"
#include "purify/operators.h"
#include "purify/utilities.h"
#include "purify/wavelet_operator_factory.h"
#include <sopt/imaging_padmm.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>

using namespace purify;

class AlgoFixture : public ::benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State &state) {
    // Reading image from file and update related quantities
    bool newImage = b_utilities::updateImage(state.range(0), m_image, m_imsizex, m_imsizey);

    // Generating random uv(w) coverage
    bool newMeasurements =
        b_utilities::updateMeasurements(state.range(1), m_uv_data, m_epsilon, newImage, m_image);

    bool newKernel = m_kernel != state.range(2);

    m_kernel = state.range(2);
    // creating the measurement operator
    const t_real FoV = 1;  // deg
    const t_real cellsize = FoV / m_imsizex * 60. * 60.;
    const bool w_term = false;
    m_measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
        factory::distributed_measurement_operator::serial, m_uv_data, m_imsizey, m_imsizex,
        cellsize, cellsize, 2, kernels::kernel::kb, m_kernel, m_kernel, w_term);

    t_real const m_sigma = 0.016820222945913496 * std::sqrt(2);  // see test_parameters file
  }

  void TearDown(const ::benchmark::State &state) {}

  t_real m_epsilon;
  t_uint m_counter;
  t_real m_sigma;
  std::vector<std::tuple<std::string, t_uint>> const m_sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  Image<t_complex> m_image;
  t_uint m_imsizex;
  t_uint m_imsizey;

  utilities::vis_params m_uv_data;

  t_uint m_kernel;
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> m_measurements_transform;
  std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>> m_padmm;
  std::shared_ptr<sopt::algorithm::ImagingForwardBackward<t_complex>> m_fb;
};

BENCHMARK_DEFINE_F(AlgoFixture, Padmm)(benchmark::State &state) {
  // Benchmark the application of the algorithm
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, m_sara, m_imsizey, m_imsizex);

  m_padmm = factory::padmm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
      factory::algo_distribution::serial, m_measurements_transform, wavelets, m_uv_data, m_sigma,
      m_imsizey, m_imsizex, m_sara.size(), state.range(3) + 1, true, true, false, 1e-3, 1e-2, 50,
      1.0);

  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    (*m_padmm)();
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end));
  }
}

BENCHMARK_DEFINE_F(AlgoFixture, ForwardBackward)(benchmark::State &state) {
  // Benchmark the application of the algorithm
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, m_sara, m_imsizey, m_imsizex);

  t_real const beta = m_sigma * m_sigma;
  t_real const gamma = 0.0001;

  m_fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
      factory::algo_distribution::serial, m_measurements_transform, wavelets, m_uv_data, m_sigma,
      beta, gamma, m_imsizey, m_imsizex, m_sara.size(), state.range(3) + 1, true, true, false, 1e-3,
      1e-2, 50);

  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    (*m_fb)();
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end));
  }
}

#ifdef PURIFY_ONNXRT
BENCHMARK_DEFINE_F(AlgoFixture, ForwardBackwardOnnx)(benchmark::State &state) {
  // Benchmark the application of the algorithm
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, m_sara, m_imsizey, m_imsizex);

  t_real const beta = m_sigma * m_sigma;
  t_real const gamma = 0.0001;
  std::string tf_model_path = purify::models_directory() + "/snr_15_model_dynamic.onnx";

  m_fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
      factory::algo_distribution::serial, m_measurements_transform, wavelets, m_uv_data, m_sigma,
      beta, gamma, m_imsizey, m_imsizex, m_sara.size(), state.range(3) + 1, true, true, false, 1e-3,
      1e-2, 50, 1.0, tf_model_path, nondiff_func_type::Denoiser);

  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    (*m_fb)();
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end));
  }
}

BENCHMARK_REGISTER_F(AlgoFixture, ForwardBackwardOnnx)
    //->Apply(b_utilities::Arguments)
    ->Args({128, 10000, 4, 10})
    ->UseManualTime()
    ->MinTime(10.0)
    ->MinWarmUpTime(5.0)
    ->Repetitions(3)  //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);
#endif

BENCHMARK_REGISTER_F(AlgoFixture, Padmm)
    //->Apply(b_utilities::Arguments)
    ->Args({128, 10000, 4, 10})
    ->UseManualTime()
    ->MinTime(10.0)
    ->MinWarmUpTime(5.0)
    ->Repetitions(3)  //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(AlgoFixture, ForwardBackward)
    //->Apply(b_utilities::Arguments)
    ->Args({128, 10000, 4, 10})
    ->UseManualTime()
    ->MinTime(10.0)
    ->MinWarmUpTime(5.0)
    ->Repetitions(3)  //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
