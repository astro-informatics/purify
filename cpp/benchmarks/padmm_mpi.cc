#include <array>
#include <random>
#include <sopt/imaging_padmm.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/mpi_utilities.h"
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"
#include "benchmarks/utilities.h"

using namespace purify;

class PadmmFixtureMPI : public ::benchmark::Fixture
{
public:
  void SetUp(const ::benchmark::State& state) {
    // Reading image from file and update related quantities
    bool newImage = b_utilities::updateImage(state.range(0), m_image, m_imsizex, m_imsizey);
    
    // Generating random uv(w) coverage and update related quantities
    sopt::mpi::Communicator world;
    bool newMeasurements = b_utilities::updateMeasurements(state.range(1), m_uv_data, m_epsilon, newImage, m_image, world);
    
    bool newKernel = m_kernel!=state.range(2);
    if (newImage || newMeasurements || newKernel) {
      m_kernel = state.range(2);
      // creating the measurement operator
      const t_real FoV = 1;      // deg
      const t_real cellsize = FoV / m_imsizex * 60. * 60.;
      const bool w_term = false;
      // algorithm 1
      m_measurements = measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
          world, m_uv_data, m_image.rows(), m_image.cols(), cellsize,
          cellsize, 2, 100, 1e-4, "kb", m_kernel, m_kernel, "measure", w_term);

      sopt::wavelets::SARA saraDistr = sopt::wavelets::distribute_sara(m_sara, world);
      auto const Psi = sopt::linear_transform<t_complex>(saraDistr, m_image.rows(), m_image.cols(), world);
      m_gamma = (Psi.adjoint() * (m_measurements->adjoint() * m_uv_data.vis)).cwiseAbs().maxCoeff() * 1e-3;
      m_gamma = world.all_reduce(m_gamma, MPI_MAX);
      
      // create the padmm algorithm
      m_padmm = std::make_shared<sopt::algorithm::ImagingProximalADMM<t_complex>>(m_uv_data.vis);
      m_padmm->itermax(50)
	.gamma(m_gamma)
	.relative_variation(1e-3)
	.l2ball_proximal_epsilon(m_epsilon)
	// communicator ensuring l1 norm in l1 proximal is global
	.l1_proximal_adjoint_space_comm(world)
	.tight_frame(false)
	.l1_proximal_tolerance(1e-2)
	.l1_proximal_nu(1)
	.l1_proximal_itermax(50)
	.l1_proximal_positivity_constraint(true)
	.l1_proximal_real_constraint(true)
	.residual_tolerance(m_epsilon)
	.lagrange_update_scale(0.9)
	.nu(1e0)
	.Psi(Psi)
	.Phi(*m_measurements);
      sopt::ScalarRelativeVariation<t_complex> conv(
	m_padmm->relative_variation(), m_padmm->relative_variation(), "Objective function");
      std::weak_ptr<decltype(m_padmm)::element_type> const padmm_weak(m_padmm);
      m_padmm->residual_convergence([padmm_weak, conv, world]
				    (Vector<t_complex> const &, Vector<t_complex> const &residual) mutable -> bool {
				      auto const padmm = padmm_weak.lock();
				      auto const residual_norm = sopt::l2_norm(residual, padmm->l2ball_proximal_weights());
				      auto const result
					= world.all_reduce<int8_t>(residual_norm < padmm->residual_tolerance(), MPI_LAND);
				      return result;
				    });
      m_padmm->objective_convergence([padmm_weak, conv, world]
				     (Vector<t_complex> const &x, Vector<t_complex> const &) mutable -> bool {
				       auto const padmm = padmm_weak.lock();
				       return world.all_reduce<uint8_t>(
                                         conv(sopt::l1_norm(padmm->Psi().adjoint() * x, padmm->l1_proximal_weights())), MPI_LAND);
				     });
    }
  }
  
  void TearDown(const ::benchmark::State& state) {
  }
  
  const sopt::wavelets::SARA m_sara{std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u), std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  
  Image<t_complex> m_image;
  t_uint m_imsizex;
  t_uint m_imsizey;
 
  utilities::vis_params m_uv_data;
  t_real m_epsilon;
  
  t_uint m_kernel;
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> m_measurements;
  t_real m_gamma;

  std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>> m_padmm;
};

BENCHMARK_DEFINE_F(PadmmFixtureMPI, Apply)(benchmark::State &state) {
  // Benchmark the application of the algorithm
  sopt::mpi::Communicator world = sopt::mpi::Communicator::World();
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    (*m_padmm)();
    auto end   = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start,end,world));
  }
}

BENCHMARK_REGISTER_F(PadmmFixtureMPI, Apply)
//->Apply(b_utilities::Arguments)
//->Args({1024,1000000,4})->Args({1024,10000000,4})
->Args({128,1000,4})
->UseManualTime()
->Repetitions(5)->ReportAggregatesOnly(true)
->Unit(benchmark::kMillisecond);
