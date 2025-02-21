#include "purify/config.h"
#include "purify/types.h"
#include <array>
#include <random>
#include <benchmark/benchmark.h>
#include "benchmarks/utilities.h"
#include "purify/algorithm_factory.h"
#include "purify/directories.h"
#include "purify/measurement_operator_factory.h"
#include "purify/operators.h"
#include "purify/utilities.h"
#include "purify/uvw_utilities.h"
#include "purify/wavelet_operator_factory.h"
#include <sopt/imaging_padmm.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include <sopt/power_method.h>

#include "purify/test_data.h"


using namespace purify;

class StochasticAlgoFixture : public ::benchmark::Fixture {
 public:
  
  void SetUp(const ::benchmark::State &state) {
    
    // m_uv_data = utilities::read_visibility(input_data_path, false);
    // m_uv_data.units = utilities::vis_units::radians;

    m_imsizex = state.range(0);
    m_imsizey = state.range(0);

    m_sigma = 0.016820222945913496 * std::sqrt(2);  // see test_parameters file
    m_beta = m_sigma * m_sigma;
    m_gamma = 0.0001;

    m_N = 1000;
    
  }

  void TearDown(const ::benchmark::State &state) {}

  //const std::string &input_data_path = data_filename("ska_mid/uvw_ska1mid197_simulation_12h_dt_60.h5");
  const std::string &m_input_data_path = data_filename("expected/fb/input_data.vis");

  //utilities::vis_params m_uv_data;
  
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
    std::mt19937 rng(0);
    std::function<std::shared_ptr<sopt::IterationState<Vector<t_complex>>>()> random_updater =
      [this, &rng]() {
        utilities::vis_params uv_data = utilities::read_visibility(m_input_data_path, false);
        uv_data.units = utilities::vis_units::radians;
	
        // Get random subset
        std::vector<size_t> indices(uv_data.size());
        size_t i = 0;
        for (auto &x : indices) {
          x = i++;
        }

        std::shuffle(indices.begin(), indices.end(), rng);
        Vector<t_real> u_fragment(m_N);
        Vector<t_real> v_fragment(m_N);
        Vector<t_real> w_fragment(m_N);
        Vector<t_complex> vis_fragment(m_N);
        Vector<t_complex> weights_fragment(m_N);
        for (i = 0; i < m_N; i++) {
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
            factory::distributed_measurement_operator::serial, uv_data_fragment, m_imsizey, m_imsizex,
            1, 1, 2, kernels::kernel_from_string.at("kb"), 4, 4);

        return std::make_shared<sopt::IterationState<Vector<t_complex>>>(uv_data_fragment.vis, phi);
      };
  
    Vector<t_complex> const init = Vector<t_complex>::Ones(m_imsizex * m_imsizey);
    
    PURIFY_INFO("Call random_updater");
      
    auto IS = random_updater();
    auto Phi = IS->Phi();

    PURIFY_INFO("Call power method");
    
    auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(Phi, 1000, 1e-5, init);
    const t_real op_norm = std::get<0>(power_method_stuff);

    PURIFY_INFO("Construct wavelets");
    
    // wavelets
    auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, m_sara, m_imsizey, m_imsizex);

    PURIFY_INFO("Construct fb algorithm with random updater");
    
    // algorithm
    sopt::algorithm::ImagingForwardBackward<t_complex> fb(random_updater);
    fb.itermax(state.range(1))
      .step_size(m_beta * sqrt(2))
      .sigma(m_sigma * sqrt(2))
      .regulariser_strength(m_gamma)
      .relative_variation(1e-3)
      .residual_tolerance(0)
      .tight_frame(true)
      .sq_op_norm(op_norm * op_norm);

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
      state.SetIterationTime(b_utilities::duration(start, end));
    }
}

BENCHMARK_REGISTER_F(StochasticAlgoFixture, ForwardBackward)
    ->Args({128, 10})
    ->UseManualTime()
    ->MinTime(10.0)
    ->MinWarmUpTime(5.0)
    ->Repetitions(3)  //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
