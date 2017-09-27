#include <array>
#include <benchmark/benchmark.h>
#include <sopt/imaging_padmm.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include "purify/operators.h"
#include "purify/types.h"
#include "purify/utilities.h"
#include "benchmarks/utilities.h"

using namespace purify;

class PadmmFixture : public ::benchmark::Fixture
{
public:
  void SetUp(const ::benchmark::State& state) {
    // Reading image from file and update related quantities
    bool newImage = b_utilities::updateImage(state.range(0), m_image, m_imsizex, m_imsizey);
    
    // Generating random uv(w) coverage and update related quantities
    bool newMeasurements = b_utilities::updateMeasurements(state.range(1), m_uv_data, m_epsilon, newImage, m_image);
    
    bool newKernel = m_kernel!=state.range(2);
    if (newImage || newMeasurements || newKernel) {
      m_kernel = state.range(2);
      // creating the measurement operator
      const t_real FoV = 1;      // deg
      const t_real cellsize = FoV / m_imsizex * 60. * 60.;
      const bool w_term = false;
      m_measurements_transform = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        m_uv_data, m_imsizey, m_imsizex, cellsize, cellsize, 2, 100, 0.0001, "kb",  m_kernel, m_kernel,
	"measure", w_term);
      m_gamma = (m_measurements_transform->adjoint() * m_uv_data.vis).real().maxCoeff() * 1e-3;

      // create the padmm algorithm
      sopt::LinearTransform<Vector<t_complex>> Psi = sopt::linear_transform<t_complex>(m_sara, m_imsizey, m_imsizex);
      m_padmm = std::make_shared<sopt::algorithm::ImagingProximalADMM<t_complex>>(m_uv_data.vis);
      m_padmm->itermax(100)
	.gamma(m_gamma)
	.relative_variation(1e-3)
	.l2ball_proximal_epsilon(m_epsilon)
	.tight_frame(false)
	.l1_proximal_tolerance(1e-2)
	.l1_proximal_nu(1)
	.l1_proximal_itermax(50)
	.l1_proximal_positivity_constraint(true)
	.l1_proximal_real_constraint(true)
	.residual_convergence(m_epsilon * 1.001)
	.lagrange_update_scale(0.9)
	.nu(1e0)
	.Psi(Psi)
	.Phi(*m_measurements_transform);
    }
  }
  
  void TearDown(const ::benchmark::State& state) {
  }

  t_uint m_counter;  
  const  sopt::wavelets::SARA m_sara{std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  Image<t_complex> m_image;
  t_uint m_imsizex;
  t_uint m_imsizey;
 
  utilities::vis_params m_uv_data;
  t_real m_epsilon;

  t_uint m_kernel;
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> m_measurements_transform;
  t_real m_gamma;
  std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>> m_padmm;
};


BENCHMARK_DEFINE_F(PadmmFixture, Apply)(benchmark::State &state) {
  // Benchmark the application of the algorithm
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    (*m_padmm)();
    auto end   = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start,end));
    }
  
  //  state.SetBytesProcessed(int64_t(state.iterations()) * (state.range(1) + m_imsizey * m_imsizex) * sizeof(t_complex));
  }

BENCHMARK_REGISTER_F(PadmmFixture, Apply)
//->Apply(b_utilities::Arguments)
//->Args({1024,1000000,4})->Args({1024,10000000,4})
->Args({128,1000,4})
->UseManualTime()
->Repetitions(5)->ReportAggregatesOnly(true)
->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
