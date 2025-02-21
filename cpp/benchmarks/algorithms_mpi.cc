#include "purify/types.h"
#include <array>
#include <random>
#include "benchmarks/utilities.h"
#include "purify/algorithm_factory.h"
#include "purify/convergence_factory.h"
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/measurement_operator_factory.h"
#include "purify/mpi_utilities.h"
#include "purify/operators.h"
#include "purify/utilities.h"
#include "purify/wavelet_operator_factory.h"
#include <sopt/imaging_padmm.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>

using namespace purify;

class AlgoFixtureMPI : public ::benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State &state) {
    // Reading image from file and update related quantities
    bool newImage = b_utilities::updateImage(state.range(0), m_image, m_imsizex, m_imsizey);

    if (state.range(5) == 1) {
      // Generating random uv(w) coverage
      bool newMeasurements = b_utilities::updateMeasurements(state.range(1), m_uv_data, m_epsilon,
							     newImage, m_image, m_world);
    }
    if (state.range(5) == 2) {
      auto hdf5_filename_small = data_filename("ska_mid/uvw_ska1mid197_simulation_12h_dt_60.h5");
      m_uv_data = utilities::read_visibility(hdf5_filename_small, true, true);
    }

    bool newKernel = m_kernel != state.range(2);

    m_kernel = state.range(2);
    // Create the measurement operator for both distributed algorithms
    const t_real FoV = 1;  // deg
    const t_real cellsize = FoV / m_imsizex * 60. * 60.;
    const bool w_term = false;
    if (state.range(4) == 1) {
      PURIFY_INFO("Using distributed image MPI algorithm");
      m_measurements_distribute_image = factory::measurement_operator_factory<Vector<t_complex>>(
          factory::distributed_measurement_operator::mpi_distribute_image, m_uv_data,
          m_image.rows(), m_image.cols(), cellsize, cellsize, 2, kernels::kernel::kb, m_kernel,
          m_kernel, w_term);
    }

    if (state.range(4) == 2) {
      PURIFY_INFO("Using distributed grid MPI algorithm");
      m_measurements_distribute_grid = factory::measurement_operator_factory<Vector<t_complex>>(
          factory::distributed_measurement_operator::mpi_distribute_grid, m_uv_data, m_image.rows(),
          m_image.cols(), cellsize, cellsize, 2, kernels::kernel::kb, m_kernel, m_kernel, w_term);
    }

    m_sigma = 0.016820222945913496 * std::sqrt(2);  // see test_parameters file
  }

  void TearDown(const ::benchmark::State &state) {}

  sopt::mpi::Communicator m_world;

  std::vector<std::tuple<std::string, t_uint>> const m_sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  Image<t_complex> m_image;
  t_uint m_imsizex;
  t_uint m_imsizey;

  utilities::vis_params m_uv_data;
  t_real m_epsilon;
  t_real m_sigma;
  t_uint m_kernel;

  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> m_measurements_distribute_image;
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> m_measurements_distribute_grid;
  std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>> m_padmm;
  std::shared_ptr<sopt::algorithm::ImagingForwardBackward<t_complex>> m_fb;
};

BENCHMARK_DEFINE_F(AlgoFixtureMPI, PadmmDistributeImage)(benchmark::State &state) {
  // Create the algorithm - has to be done there to reset the internal state.
  // If done in the fixture repeats would start at the solution and converge immediately.
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::mpi_sara, m_sara, m_imsizey, m_imsizex);

  m_padmm = factory::padmm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
      factory::algo_distribution::mpi_distributed, m_measurements_distribute_image, wavelets,
      m_uv_data, m_sigma, m_imsizey, m_imsizex, m_sara.size(), state.range(3) + 1, true, true,
      false, 1e-3, 1e-2, 50, 1.0, 1.0);

  // Benchmark the application of the algorithm
  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = (*m_padmm)();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Converged? " << result.good << " , niters = " << result.niters << std::endl;
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }
}

BENCHMARK_DEFINE_F(AlgoFixtureMPI, PadmmDistributeGrid)(benchmark::State &state) {
  // Create the algorithm - has to be done there to reset the internal state.
  // If done in the fixture repeats would start at the solution and converge immediately.
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::mpi_sara, m_sara, m_imsizey, m_imsizex);

  m_padmm = factory::padmm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
      factory::algo_distribution::mpi_distributed, m_measurements_distribute_grid, wavelets,
      m_uv_data, m_sigma, m_imsizey, m_imsizex, m_sara.size(), state.range(3) + 1, true, true,
      false, 1e-3, 1e-2, 50, 1.0, 1.0);

  // Benchmark the application of the algorithm
  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = (*m_padmm)();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Converged? " << result.good << " , niters = " << result.niters << std::endl;
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }
}

BENCHMARK_DEFINE_F(AlgoFixtureMPI, FbDistributeImage)(benchmark::State &state) {
  // Create the algorithm - has to be done there to reset the internal state.
  // If done in the fixture repeats would start at the solution and converge immediately.
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::mpi_sara, m_sara, m_imsizey, m_imsizex);

  t_real const beta = m_sigma * m_sigma;
  t_real const gamma = 0.0001;

  m_fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
      factory::algo_distribution::mpi_serial, m_measurements_distribute_image, wavelets, m_uv_data,
      m_sigma, beta, gamma, m_imsizey, m_imsizex, m_sara.size(), state.range(3) + 1, true, true,
      false, 1e-3, 1e-2, 50, 1.0);

  // Benchmark the application of the algorithm
  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = (*m_fb)();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Converged? " << result.good << " , niters = " << result.niters << std::endl;
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }
}

BENCHMARK_DEFINE_F(AlgoFixtureMPI, FbDistributeGrid)(benchmark::State &state) {
  // Create the algorithm - has to be done there to reset the internal state.
  // If done in the fixture repeats would start at the solution and converge immediately.
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::mpi_sara, m_sara, m_imsizey, m_imsizex);

  t_real const beta = m_sigma * m_sigma;
  t_real const gamma = 0.0001;

  m_fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
      factory::algo_distribution::mpi_serial, m_measurements_distribute_grid, wavelets, m_uv_data,
      m_sigma, beta, gamma, m_imsizey, m_imsizex, m_sara.size(), state.range(3) + 1, true, true,
      false, 1e-3, 1e-2, 50, 1.0);

  // Benchmark the application of the algorithm
  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = (*m_fb)();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Converged? " << result.good << " , niters = " << result.niters << std::endl;
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }
}

#ifdef PURIFY_ONNXRT
BENCHMARK_DEFINE_F(AlgoFixtureMPI, FbOnnxDistributeImage)(benchmark::State &state) {
  // Create the algorithm - has to be done there to reset the internal state.
  // If done in the fixture repeats would start at the solution and converge immediately.

  // TODO: Wavelets are constructed but not used in the factory method
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, m_sara, m_imsizey, m_imsizex);

  t_real const beta = m_sigma * m_sigma;
  t_real const gamma = 0.0001;

  std::string tf_model_path = purify::models_directory() + "/snr_15_model_dynamic.onnx";

  m_fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
      factory::algo_distribution::mpi_serial, m_measurements_distribute_image, wavelets, m_uv_data,
      m_sigma, beta, gamma, m_imsizey, m_imsizex, m_sara.size(), state.range(3) + 1, true, true,
      false, 1e-3, 1e-2, 50, 1.0, tf_model_path, nondiff_func_type::Denoiser);

  // Benchmark the application of the algorithm
  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = (*m_fb)();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Converged? " << result.good << " , niters = " << result.niters << std::endl;
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }
}

BENCHMARK_REGISTER_F(AlgoFixtureMPI, FbOnnxDistributeImage)
    //->Apply(b_utilities::Arguments)
    ->Args({128, 10000, 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e6), 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e7), 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e8), 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e9), 4, 10, 1})
    ->UseManualTime()
    ->MinTime(120.0)
    ->MinWarmUpTime(10.0)
    ->Repetitions(3)  //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

#endif

BENCHMARK_REGISTER_F(AlgoFixtureMPI, FbDistributeImage)
    //->Apply(b_utilities::Arguments)
    ->Args({128, 10000, 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e6), 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e7), 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e8), 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e9), 4, 10, 1})
    ->UseManualTime()
    ->MinTime(120.0)
    ->MinWarmUpTime(10.0)
    ->Repetitions(3)  //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(AlgoFixtureMPI, FbDistributeGrid)
    //->Apply(b_utilities::Arguments)
    ->Args({128, 10000, 4, 10, 2})
    ->Args({1024, static_cast<t_int>(1e6), 4, 10, 2})
    ->Args({1024, static_cast<t_int>(1e7), 4, 10, 2})
    ->Args({1024, static_cast<t_int>(1e8), 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e9), 4, 10, 1})
    ->UseManualTime()
    ->MinTime(120.0)
    ->MinWarmUpTime(10.0)
    ->Repetitions(3)  //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(AlgoFixtureMPI, PadmmDistributeImage)
    //->Apply(b_utilities::Arguments)
    ->Args({128, 10000, 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e6), 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e7), 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e8), 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e9), 4, 10, 1})
    ->UseManualTime()
    ->MinTime(120.0)
    ->MinWarmUpTime(10.0)
    ->Repetitions(3)  //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(AlgoFixtureMPI, PadmmDistributeGrid)
    //->Apply(b_utilities::Arguments)
    ->Args({128, 10000, 4, 10, 2})
    ->Args({1024, static_cast<t_int>(1e6), 4, 10, 2})
    ->Args({1024, static_cast<t_int>(1e7), 4, 10, 2})
    ->Args({1024, static_cast<t_int>(1e8), 4, 10, 1})
    ->Args({1024, static_cast<t_int>(1e9), 4, 10, 1})
    ->UseManualTime()
    ->MinTime(120.0)
    ->MinWarmUpTime(10.0)
    ->Repetitions(3)  //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);
