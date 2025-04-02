#include "purify/config.h"
#include "purify/types.h"
#include <array>
#include <random>
#include <benchmark/benchmark.h>
#include "benchmarks/utilities.h"
#include "purify/algorithm_factory.h"
#include "purify/directories.h"
#include "purify/measurement_operator_factory.h"
#include "purify/mpi_utilities.h"
#include "purify/operators.h"
#include "purify/utilities.h"
#include "purify/uvw_utilities.h"
#include "purify/wavelet_operator_factory.h"
#include <sopt/imaging_padmm.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
#include <sopt/power_method.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>

#ifdef PURIFY_H5
#include "purify/h5reader.h"
#endif

using namespace purify;

class StochasticAlgoFixture : public ::benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State &state) {
    m_imsizex = state.range(0);
    m_imsizey = state.range(0);

    m_sigma = 0.016820222945913496 * std::sqrt(2);
    m_beta = m_sigma * m_sigma;
    m_gamma = 0.0001;

    m_N = state.range(1);

    m_input_data_path = data_filename("expected/fb/input_data.h5");

    m_world = sopt::mpi::Communicator::World();
  }

  void TearDown(const ::benchmark::State &state) {}

  sopt::mpi::Communicator m_world;

  std::string m_input_data_path;

  t_uint m_imsizey;
  t_uint m_imsizex;

  t_real m_sigma;
  t_real m_beta;
  t_real m_gamma;

  size_t m_N;

  std::vector<std::tuple<std::string, t_uint>> const m_sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
};

BENCHMARK_DEFINE_F(StochasticAlgoFixture, ForwardBackward)(benchmark::State &state) {
  // This functor would be defined in Purify
  std::function<std::shared_ptr<sopt::IterationState<Vector<t_complex>>>()> random_updater =
      [this]() {
        H5::H5Handler h5file(m_input_data_path, m_world);
        utilities::vis_params uv_data = H5::stochread_visibility(h5file, m_N, false);
        uv_data.units = utilities::vis_units::radians;
        auto phi = factory::measurement_operator_factory<Vector<t_complex>>(
            factory::distributed_measurement_operator::mpi_distribute_image, uv_data, m_imsizex,
            m_imsizey, 1, 1, 2, kernels::kernel_from_string.at("kb"), 4, 4);

        auto const power_method_stuff = sopt::algorithm::power_method<Vector<t_complex>>(
            *phi, 1000, 1e-5,
            m_world.broadcast(Vector<t_complex>::Ones(m_imsizex * m_imsizey).eval()));

        const t_real op_norm = std::get<0>(power_method_stuff);
        phi->set_norm(op_norm);

        return std::make_shared<sopt::IterationState<Vector<t_complex>>>(uv_data.vis, phi);
      };

  // wavelets
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, m_sara, m_imsizey, m_imsizex);

  // algorithm
  sopt::algorithm::ImagingForwardBackward<t_complex> fb(random_updater);
  fb.itermax(state.range(2))
      .step_size(m_beta * sqrt(2))
      .sigma(m_sigma * sqrt(2))
      .regulariser_strength(m_gamma)
      .relative_variation(1e-3)
      .residual_tolerance(0)
      .tight_frame(true)
      .obj_comm(m_world);

  auto gp = std::make_shared<sopt::algorithm::L1GProximal<t_complex>>(false);
  gp->l1_proximal_tolerance(1e-4)
      .l1_proximal_nu(1)
      .l1_proximal_itermax(50)
      .l1_proximal_positivity_constraint(true)
      .l1_proximal_real_constraint(true)
      .Psi(*wavelets);
  fb.g_function(gp);

  PURIFY_INFO("Start iteration loop");

  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    fb();
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }
}

BENCHMARK_DEFINE_F(StochasticAlgoFixture, ForwardBackwardApproxNorm)(benchmark::State &state) {
  // This functor would be defined in Purify
  std::function<std::shared_ptr<sopt::IterationState<Vector<t_complex>>>()> random_updater =
      [this]() {
        H5::H5Handler h5file(m_input_data_path, m_world);
        utilities::vis_params uv_data = H5::stochread_visibility(h5file, m_N, false);
        uv_data.units = utilities::vis_units::radians;
        auto phi = factory::measurement_operator_factory<Vector<t_complex>>(
            factory::distributed_measurement_operator::mpi_distribute_image, uv_data, m_imsizex,
            m_imsizey, 1, 1, 2, kernels::kernel_from_string.at("kb"), 4, 4);

        // declaration of static variables to avoid recalculating the normalisation
        static auto const power_method_stuff = sopt::algorithm::power_method<Vector<t_complex>>(
            *phi, 1000, 1e-5,
            m_world.broadcast(Vector<t_complex>::Ones(m_imsizex * m_imsizey).eval()));

        static const t_real op_norm = std::get<0>(power_method_stuff);
        
        // set the normalisation of the new phi
        phi->set_norm(op_norm);

        return std::make_shared<sopt::IterationState<Vector<t_complex>>>(uv_data.vis, phi);
      };

  // wavelets
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, m_sara, m_imsizey, m_imsizex);

  // algorithm
  sopt::algorithm::ImagingForwardBackward<t_complex> fb(random_updater);
  fb.itermax(state.range(2))
      .step_size(m_beta * sqrt(2))
      .sigma(m_sigma * sqrt(2))
      .regulariser_strength(m_gamma)
      .relative_variation(1e-3)
      .residual_tolerance(0)
      .tight_frame(true)
      .obj_comm(m_world);

  auto gp = std::make_shared<sopt::algorithm::L1GProximal<t_complex>>(false);
  gp->l1_proximal_tolerance(1e-4)
      .l1_proximal_nu(1)
      .l1_proximal_itermax(50)
      .l1_proximal_positivity_constraint(true)
      .l1_proximal_real_constraint(true)
      .Psi(*wavelets);
  fb.g_function(gp);

  PURIFY_INFO("Start iteration loop");

  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    fb();
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }
}

BENCHMARK_REGISTER_F(StochasticAlgoFixture, ForwardBackward)
    ->Args({128, 10000, 10})
    ->UseManualTime()
    ->MinTime(60.0)
    ->MinWarmUpTime(5.0)
    ->Repetitions(3)  //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(StochasticAlgoFixture, ForwardBackwardApproxNorm)
    ->Args({128, 10000, 10})
    ->UseManualTime()
    ->MinTime(60.0)
    ->MinWarmUpTime(5.0)
    ->Repetitions(3)  //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);