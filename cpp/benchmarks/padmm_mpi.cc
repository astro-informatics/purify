#include <array>
//#include <memory>
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

std::tuple<utilities::vis_params, t_real>
  dirty_visibilities(Image<t_complex> &ground_truth_image, t_uint number_of_vis, t_real snr,
		     const std::tuple<bool, t_real> &w_term) {
    auto uv_data
      = utilities::random_sample_density(number_of_vis, 0, constant::pi / 3, std::get<0>(w_term));
    uv_data.units = "radians";
    // creating operator to generate measurements
    auto const sky_measurements = std::make_shared<sopt::LinearTransform<Vector<t_complex>> const>(
      measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_data, ground_truth_image.rows(), ground_truth_image.cols(), std::get<1>(w_term),
          std::get<1>(w_term), 2, 100, 1e-4, "kb", 8, 8, "measure", std::get<0>(w_term)));
  // Generates measurements from image
  uv_data.vis = (*sky_measurements)
                * Image<t_complex>::Map(ground_truth_image.data(), ground_truth_image.size(), 1);

  // working out value of signal given SNR of 30
  auto const sigma = utilities::SNR_to_standard_deviation(uv_data.vis, snr);
  // adding noise to visibilities
  uv_data.vis = utilities::add_noise(uv_data.vis, 0., sigma);
  return std::make_tuple(uv_data, sigma);
  }

std::tuple<utilities::vis_params, t_real>
dirty_visibilities(Image<t_complex> &ground_truth_image, t_uint number_of_vis, t_real snr,
                   const std::tuple<bool, t_real> &w_term, sopt::mpi::Communicator const &comm) {
  if(comm.size() == 1)
    //return b_utilities::dirty_visibilities(ground_truth_image, number_of_vis, snr, w_term);
    return dirty_visibilities(ground_truth_image, number_of_vis, snr, w_term);
  if(comm.is_root()) {
    //auto result = b_utilities::dirty_visibilities(ground_truth_image, number_of_vis, snr, w_term);
    auto result = dirty_visibilities(ground_truth_image, number_of_vis, snr, w_term);
    comm.broadcast(std::get<1>(result));
    auto const order
        = distribute::distribute_measurements(std::get<0>(result), comm, "distance_distribution");
    std::get<0>(result) = utilities::regroup_and_scatter(std::get<0>(result), order, comm);
    return result;
  }
  auto const sigma = comm.broadcast<t_real>();
  return std::make_tuple(utilities::scatter_visibilities(comm), sigma);
}


class PadmmFixtureMPI : public ::benchmark::Fixture
{
public:
  void SetUp(const ::benchmark::State& state) {
    // Reading image from file and update related quantities
    bool newImage = b_utilities::updateImage(state.range(0), m_image, m_imsizex, m_imsizey);

   // Generating random uv(w) coverage and update related quantities
    //bool newMeasurements = b_utilities::updateMeasurements(state.range(1), m_uv_data, m_world);

    m_world = sopt::mpi::Communicator::World();
      const t_real FoV = 1;      // deg
      const t_real cellsize = FoV / m_imsizex * 60. * 60.;
      const bool w_term = false;
      std::tuple<utilities::vis_params, t_real> temp = dirty_visibilities(m_image, state.range(1), 30., std::make_tuple(w_term,cellsize), m_world);
   m_uv_data = std::get<0>(temp);
   t_real sigma =  std::get<1>(temp);

       m_kernel = state.range(2);
   // algorithm 1
     m_measurements = std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(
      measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
          m_world, m_uv_data, m_image.rows(), m_image.cols(), cellsize,
          cellsize, 2, 100, 1e-4, "kb", m_kernel, m_kernel, "measure", w_term));

     sopt::wavelets::SARA saraDistr = sopt::wavelets::distribute_sara(m_sara, m_world);
     auto const Psi = sopt::linear_transform<t_complex>(saraDistr, m_image.rows(), m_image.cols(), m_world);
     m_epsilon = utilities::calculate_l2_radius(m_uv_data.vis, sigma);
     m_gamma
       = (Psi.adjoint() * (m_measurements->adjoint() * m_uv_data.vis)).cwiseAbs().maxCoeff() * 1e-3;
     m_gamma = m_world.all_reduce(m_gamma, MPI_MAX);

     m_padmm = std::make_shared<sopt::algorithm::ImagingProximalADMM<t_complex>>(m_uv_data.vis);
     m_padmm->itermax(50)
      .gamma(m_gamma)
      .relative_variation(1e-3)
      .l2ball_proximal_epsilon(m_epsilon)
      // communicator ensuring l1 norm in l1 proximal is global
      .l1_proximal_adjoint_space_comm(m_world)
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

  }
  
  void TearDown(const ::benchmark::State& state) {
  }
  
  const sopt::wavelets::SARA m_sara{std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u), std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  sopt::mpi::Communicator m_world;

 Image<t_complex> m_image;
  t_uint m_imsizex;
  t_uint m_imsizey;
 
  utilities::vis_params m_uv_data;
  t_real m_epsilon;

  t_uint m_kernel;
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> m_measurements;
  t_real m_gamma;

  std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>> m_padmm;
};

BENCHMARK_DEFINE_F(PadmmFixtureMPI, Ctor)(benchmark::State &state) {
  // Benchmark the construction of the algorithm
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto end   = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start,end,m_world));
  }
}
    
BENCHMARK_REGISTER_F(PadmmFixtureMPI, Ctor)
//->Apply(b_utilities::Arguments)
//->Args({1024,1000000,4})->Args({1024,10000000,4})
->Args({128,1000,4})
->UseManualTime()
->Repetitions(5)->ReportAggregatesOnly(true)
->Unit(benchmark::kMillisecond);


/*std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>>
padmm_factory(std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> const &measurements,
              const sopt::wavelets::SARA &sara, const Image<t_complex> &ground_truth_image,
              const utilities::vis_params &uv_data, const t_real sigma,
              const sopt::mpi::Communicator &comm) {

  auto const Psi = sopt::linear_transform<t_complex>(sara, ground_truth_image.rows(),
                                                     ground_truth_image.cols(), comm);

  auto const epsilon = utilities::calculate_l2_radius(uv_data.vis, sigma);
  auto const gamma
      = (Psi.adjoint() * (measurements->adjoint() * uv_data.vis)).cwiseAbs().maxCoeff() * 1e-3;

  // shared pointer because the convergence function need access to some data that we would rather
  // not reproduce. E.g. padmm definition is self-referential.
  auto padmm = std::make_shared<sopt::algorithm::ImagingProximalADMM<t_complex>>(uv_data.vis);
  padmm->itermax(50)
      .gamma(comm.all_reduce(gamma, MPI_MAX))
      .relative_variation(1e-3)
      .l2ball_proximal_epsilon(epsilon)
      // communicator ensuring l1 norm in l1 proximal is global
      .l1_proximal_adjoint_space_comm(comm)
      .tight_frame(false)
      .l1_proximal_tolerance(1e-2)
      .l1_proximal_nu(1)
      .l1_proximal_itermax(50)
      .l1_proximal_positivity_constraint(true)
      .l1_proximal_real_constraint(true)
      .residual_tolerance(epsilon)
      .lagrange_update_scale(0.9)
      .nu(1e0)
      .Psi(Psi)
      .Phi(*measurements);
  sopt::ScalarRelativeVariation<t_complex> conv(padmm->relative_variation(),
                                                padmm->relative_variation(), "Objective function");
  std::weak_ptr<decltype(padmm)::element_type> const padmm_weak(padmm);
  padmm->residual_convergence([padmm_weak, conv,
                               comm](Vector<t_complex> const &x,
                                     Vector<t_complex> const &residual) mutable -> bool {
    auto const padmm = padmm_weak.lock();
    auto const residual_norm = sopt::l2_norm(residual, padmm->l2ball_proximal_weights());
    auto const result
        = comm.all_reduce<int8_t>(residual_norm < padmm->residual_tolerance(), MPI_LAND);
    return result;
  });

  padmm->objective_convergence([padmm_weak, conv, comm](Vector<t_complex> const &x,
                                                        Vector<t_complex> const &) mutable -> bool {
    auto const padmm = padmm_weak.lock();
    return comm.all_reduce<uint8_t>(
        conv(sopt::l1_norm(padmm->Psi().adjoint() * x, padmm->l1_proximal_weights())), MPI_LAND);
  });

  return padmm;
}

int main(int nargs, char const **args) {
  
  const t_real FoV = 1;     // deg
  const t_real max_w = 100; // lambda
  const std::string name = "M31";
  const t_real snr = 30;
  auto const kernel = "kb";
  const bool w_term = true;
  // string of fits file of image to reconstruct
  auto ground_truth_image = pfitsio::read2d(image_filename(name + ".fits"));
  ground_truth_image /= ground_truth_image.array().abs().maxCoeff();

  const t_real cellsize = FoV / ground_truth_image.cols() * 60. * 60.;
  // determine amount of visibilities to simulate
  t_int const number_of_pixels = ground_truth_image.size();
  t_int const number_of_vis = std::floor(number_of_pixels * 0.1);

  // Generating random uv(w) coverage
  auto const data = dirty_visibilities(ground_truth_image, number_of_vis, snr,
                                       std::make_tuple(w_term, cellsize), world);
#if PURIFY_PADMM_ALGORITHM == 2 || PURIFY_PADMM_ALGORITHM == 3
  auto const measurements = std::make_shared<sopt::LinearTransform<Vector<t_complex>> const>(
      measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          world, std::get<0>(data), ground_truth_image.rows(), ground_truth_image.cols(), cellsize,
          cellsize, 2, 100, 1e-4, kernel, 4, 4, "measure", w_term));

#elif PURIFY_PADMM_ALGORITHM == 1
  auto const measurements = std::make_shared<sopt::LinearTransform<Vector<t_complex>> const>(
      measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
          world, std::get<0>(data), ground_truth_image.rows(), ground_truth_image.cols(), cellsize,
          cellsize, 2, 100, 1e-4, kernel, 8, 8, "measure", w_term));

#endif
  auto const sara = sopt::wavelets::distribute_sara(
      sopt::wavelets::SARA{
          std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
          std::make_tuple("DB3", 3u), std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
          std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)},
      world);

  // Create the padmm solver
  auto const padmm = padmm_factory(measurements, sara, ground_truth_image, std::get<0>(data),
                                   std::get<1>(data), world);
  // calls padmm
  auto const diagnostic = (*padmm)();

 }
  return 0;
}
*/
