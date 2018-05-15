#include <array>
#include <random>
#include <sopt/imaging_padmm.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include "benchmarks/utilities.h"
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/mpi_utilities.h"
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"

using namespace purify;

class PadmmFixtureMPI : public ::benchmark::Fixture {
public:
  void SetUp(const ::benchmark::State &state) {
    // Reading image from file and update related quantities
    bool newImage = b_utilities::updateImage(state.range(0), m_image, m_imsizex, m_imsizey);

    // Generating random uv(w) coverage and update related quantities
    bool newMeasurements = b_utilities::updateMeasurements(state.range(1), m_uv_data, m_epsilon,
                                                           newImage, m_image, m_world);

    bool newKernel = m_kernel != state.range(2);
    if(newImage || newMeasurements || newKernel) {
      m_kernel = state.range(2);
      // creating the measurement operator
      const t_real FoV = 1; // deg
      const t_real cellsize = FoV / m_imsizex * 60. * 60.;
      const bool w_term = false;
      // algorithm 1
      m_measurements1 = measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
          m_world, m_uv_data, m_image.rows(), m_image.cols(), cellsize, cellsize, 2, 0, 1e-4,
          kernels::kernel::kb, m_kernel, m_kernel, operators::fftw_plan::measure, w_term);
      // algorithm 3
      m_measurements3 = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
         m_world, m_uv_data, m_image.rows(), m_image.cols(), cellsize, cellsize, 2, 0, 1e-4,
	 kernels::kernel::kb, m_kernel, m_kernel, operators::fftw_plan::measure, w_term);
     
    }
  }

  void TearDown(const ::benchmark::State &state) {}

  sopt::mpi::Communicator m_world;

  const sopt::wavelets::SARA m_sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  Image<t_complex> m_image;
  t_uint m_imsizex;
  t_uint m_imsizey;

  utilities::vis_params m_uv_data;
  t_real m_epsilon;

  t_uint m_kernel;
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> m_measurements1;
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> m_measurements3;
};

BENCHMARK_DEFINE_F(PadmmFixtureMPI, ApplyAlgo1)(benchmark::State &state) {
  // Create the algorithm - somehow doesn't work if done in the fixture...
  sopt::wavelets::SARA saraDistr = sopt::wavelets::distribute_sara(m_sara, m_world);
  auto const Psi
      = sopt::linear_transform<t_complex>(saraDistr, m_image.rows(), m_image.cols(), m_world);
   t_real gamma
      = utilities::step_size(uv_data, measurements1, 
          std::make_shared<sopt::LinearTransform<Vector<t_complex>> const>(Psi), sara.size()) * 1e-3;
  gamma = m_world.all_reduce(gamma, MPI_MAX);

  std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>> padmm
      = std::make_shared<sopt::algorithm::ImagingProximalADMM<t_complex>>(m_uv_data.vis);
  padmm->itermax(2)
      .gamma(gamma)
      .relative_variation(1e-3)
      .l2ball_proximal_epsilon(m_epsilon)
      // communicator ensuring l1 norm in l1 proximal is global
      .l1_proximal_adjoint_space_comm(m_world)
      .tight_frame(false)
      .l1_proximal_tolerance(1e-2)
      .l1_proximal_nu(1)
      .l1_proximal_itermax(2)
      .l1_proximal_positivity_constraint(true)
      .l1_proximal_real_constraint(true)
      .residual_tolerance(m_epsilon)
      .lagrange_update_scale(0.9)
      .nu(1e0)
      .Psi(Psi)
      .Phi(*m_measurements1);

  sopt::ScalarRelativeVariation<t_complex> conv(padmm->relative_variation(),
                                                padmm->relative_variation(), "Objective function");
  std::weak_ptr<decltype(padmm)::element_type> const padmm_weak(padmm);
  padmm->residual_convergence([padmm_weak,
                               this](Vector<t_complex> const &,
                                     Vector<t_complex> const &residual) mutable -> bool {
    auto const padmm = padmm_weak.lock();
    auto const residual_norm = sopt::l2_norm(residual, padmm->l2ball_proximal_weights());
    auto const result
        = this->m_world.all_reduce<int8_t>(residual_norm < padmm->residual_tolerance(), MPI_LAND);
    return result;
  });
  padmm->objective_convergence([padmm_weak, conv, this](Vector<t_complex> const &x,
                                                        Vector<t_complex> const &) mutable -> bool {
    auto const padmm = padmm_weak.lock();
    auto const result = this->m_world.all_reduce<uint8_t>(
        conv(sopt::l1_norm(padmm->Psi().adjoint() * x, padmm->l1_proximal_weights())), MPI_LAND);
    return result;
  });
  // Benchmark the application of the algorithm
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = (*padmm)();
    auto end = std::chrono::high_resolution_clock::now();
    // std::cout << "Converged? " << result.good << " , niters = " << result.niters << std::endl;
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }
}

BENCHMARK_DEFINE_F(PadmmFixtureMPI, ApplyAlgo3)(benchmark::State &state) {
  // Create the algorithm - somehow doesn't work if done in the fixture...
  sopt::wavelets::SARA saraDistr = sopt::wavelets::distribute_sara(m_sara, m_world);
  auto const Psi
      = sopt::linear_transform<t_complex>(saraDistr, m_image.rows(), m_image.cols(), m_world);
   t_real gamma
      = utilities::step_size(uv_data, measurements3, 
          std::make_shared<sopt::LinearTransform<Vector<t_complex>> const>(Psi), sara.size()) * 1e-3;
  gamma = m_world.all_reduce(gamma, MPI_MAX);

  std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>> padmm
      = std::make_shared<sopt::algorithm::ImagingProximalADMM<t_complex>>(m_uv_data.vis);
  padmm->itermax(2)
      .gamma(gamma)
      .relative_variation(1e-3)
      .l2ball_proximal_epsilon(m_epsilon)
      // communicator ensuring l1 norm in l1 proximal is global
      .l1_proximal_adjoint_space_comm(m_world)
      .tight_frame(false)
      .l1_proximal_tolerance(1e-2)
      .l1_proximal_nu(1)
      .l1_proximal_itermax(2)
      .l1_proximal_positivity_constraint(true)
      .l1_proximal_real_constraint(true)
      .residual_tolerance(m_epsilon)
      .lagrange_update_scale(0.9)
      .nu(1e0)
      .Psi(Psi)
      .Phi(*m_measurements3);

  sopt::ScalarRelativeVariation<t_complex> conv(padmm->relative_variation(),
                                                padmm->relative_variation(), "Objective function");
  std::weak_ptr<decltype(padmm)::element_type> const padmm_weak(padmm);
  padmm->residual_convergence([padmm_weak,
                               this](Vector<t_complex> const &,
                                     Vector<t_complex> const &residual) mutable -> bool {
    auto const padmm = padmm_weak.lock();
    auto const residual_norm = sopt::l2_norm(residual, padmm->l2ball_proximal_weights());
    auto const result
        = this->m_world.all_reduce<int8_t>(residual_norm < padmm->residual_tolerance(), MPI_LAND);
    return result;
  });
  padmm->objective_convergence([padmm_weak, conv, this](Vector<t_complex> const &x,
                                                        Vector<t_complex> const &) mutable -> bool {
    auto const padmm = padmm_weak.lock();
    auto const result = this->m_world.all_reduce<uint8_t>(
        conv(sopt::l1_norm(padmm->Psi().adjoint() * x, padmm->l1_proximal_weights())), MPI_LAND);
    return result;
  });
  // Benchmark the application of the algorithm
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = (*padmm)();
    auto end = std::chrono::high_resolution_clock::now();
    // std::cout << "Converged? " << result.good << " , niters = " << result.niters << std::endl;
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }
}

BENCHMARK_REGISTER_F(PadmmFixtureMPI, ApplyAlgo1)
    //->Apply(b_utilities::Arguments)
    ->Args({1024, 1000000, 4})
    ->Args({1024, 10000000, 4})
    ->UseManualTime()
    ->Repetitions(10) //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(PadmmFixtureMPI, ApplyAlgo3)
    //->Apply(b_utilities::Arguments)
    ->Args({1024, 1000000, 4})
    ->Args({1024, 10000000, 4})
    ->UseManualTime()
    ->Repetitions(10) //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);
