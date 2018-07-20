#include <array>
#include <memory>
#include <random>
#include <boost/filesystem.hpp>
#include <boost/math/special_functions/erf.hpp>
#include <sopt/imaging_padmm.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/mpi_utilities.h"
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"

#ifdef PURIFY_GPU
#include "purify/operators_gpu.h"
#endif

#ifndef PURIFY_PADMM_ALGORITHM
#define PURIFY_PADMM_ALGORITHM 2
#endif

using namespace purify;
using namespace purify::notinstalled;

std::tuple<utilities::vis_params, t_real>
dirty_visibilities(Image<t_complex> const &ground_truth_image, t_uint number_of_vis, t_real snr,
                   const std::tuple<bool, t_real> &w_term) {
  auto uv_data
      = utilities::random_sample_density(number_of_vis, 0, constant::pi / 3, std::get<0>(w_term));
  uv_data.units = utilities::vis_units::radians;
  PURIFY_HIGH_LOG("Number of measurements / number of pixels: {}",
                  uv_data.u.size() / ground_truth_image.size());
  // creating operator to generate measurements
  auto const sky_measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uv_data, ground_truth_image.rows(), ground_truth_image.cols(), std::get<1>(w_term),
      std::get<1>(w_term), 2, 100, 1e-4, kernels::kernel::kb, 8, 8,
      std::get<0>(w_term));
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
dirty_visibilities(Image<t_complex> const &ground_truth_image, t_uint number_of_vis, t_real snr,
                   const std::tuple<bool, t_real> &w_term, sopt::mpi::Communicator const &comm) {
  if(comm.size() == 1)
    return dirty_visibilities(ground_truth_image, number_of_vis, snr, w_term);
  if(comm.is_root()) {
    auto result = dirty_visibilities(ground_truth_image, number_of_vis, snr, w_term);
    comm.broadcast(std::get<1>(result));
    auto const order
        = distribute::distribute_measurements(std::get<0>(result), comm, distribute::plan::radial);
    std::get<0>(result) = utilities::regroup_and_scatter(std::get<0>(result), order, comm);
    return result;
  }
  auto const sigma = comm.broadcast<t_real>();
  return std::make_tuple(utilities::scatter_visibilities(comm), sigma);
}

std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>>
padmm_factory(std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> const &measurements,
              const sopt::wavelets::SARA &sara, const Image<t_complex> &ground_truth_image,
              const utilities::vis_params &uv_data, const t_real sigma,
              const sopt::mpi::Communicator &comm) {

  auto const Psi = sopt::linear_transform<t_complex>(sara, ground_truth_image.rows(),
                                                     ground_truth_image.cols(), comm);

#if PURIFY_PADMM_ALGORITHM == 2
  auto const epsilon = std::sqrt(
      comm.all_sum_all(std::pow(utilities::calculate_l2_radius(uv_data.vis.size(), sigma), 2)));
#elif PURIFY_PADMM_ALGORITHM == 3 || PURIFY_PADMM_ALGORITHM == 1
  auto const epsilon = utilities::calculate_l2_radius(uv_data.vis.size(), sigma);
#endif
  PURIFY_LOW_LOG("SARA Size = {}, Rank = {}", sara.size(), comm.rank());
  const t_real gamma
      = utilities::step_size(uv_data.vis, measurements, 
          std::make_shared<sopt::LinearTransform<Vector<t_complex>> const>(Psi), sara.size()) * 1e-3;
  PURIFY_LOW_LOG("Epsilon {}, Rank = {}", epsilon, comm.rank());
  PURIFY_LOW_LOG("Gamma {}, SARA Size = {}, Rank = {}", gamma, sara.size(), comm.rank());

  // shared pointer because the convergence function need access to some data that we would rather
  // not reproduce. E.g. padmm definition is self-referential.
  auto padmm = std::make_shared<sopt::algorithm::ImagingProximalADMM<t_complex>>(uv_data.vis);
  padmm->itermax(50)
      .gamma(comm.all_reduce(gamma, MPI_MAX))
      .relative_variation(1e-3)
      .l2ball_proximal_epsilon(epsilon)
#if PURIFY_PADMM_ALGORITHM == 2
      // communicator ensuring l2 norm in l2ball proximal is global
      .l2ball_proximal_communicator(comm)
#endif
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
#if PURIFY_PADMM_ALGORITHM == 2
    auto const residual_norm = sopt::mpi::l2_norm(residual, padmm->l2ball_proximal_weights(), comm);
    auto const result = residual_norm < padmm->residual_tolerance();
#elif PURIFY_PADMM_ALGORITHM == 3 || PURIFY_PADMM_ALGORITHM == 1
    auto const residual_norm = sopt::l2_norm(residual, padmm->l2ball_proximal_weights());
    auto const result
        = comm.all_reduce<int8_t>(residual_norm < padmm->residual_tolerance(), MPI_LAND);
#endif
    SOPT_LOW_LOG("    - [PADMM] Residuals: {} <? {}", residual_norm, padmm->residual_tolerance());
    return result;
  });

  padmm->objective_convergence([padmm_weak, conv, comm](Vector<t_complex> const &x,
                                                        Vector<t_complex> const &) mutable -> bool {
    auto const padmm = padmm_weak.lock();
#if PURIFY_PADMM_ALGORITHM == 2
    return conv(sopt::mpi::l1_norm(padmm->Psi().adjoint() * x, padmm->l1_proximal_weights(), comm));
#elif PURIFY_PADMM_ALGORITHM == 3 || PURIFY_PADMM_ALGORITHM == 1
    return comm.all_reduce<uint8_t>(
        conv(sopt::l1_norm(padmm->Psi().adjoint() * x, padmm->l1_proximal_weights())), MPI_LAND);
#endif
  });

  return padmm;
}

int main(int nargs, char const **args) {
  sopt::logging::initialize();
  purify::logging::initialize();
  sopt::logging::set_level("debug");
  purify::logging::set_level("debug");
  auto const session = sopt::mpi::init(nargs, args);
  auto const world = sopt::mpi::Communicator::World();

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
#ifndef PURIFY_GPU
  auto const measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      world, std::get<0>(data), ground_truth_image.rows(), ground_truth_image.cols(), cellsize,
      cellsize, 2, 100, 1e-4, kernels::kernel_from_string.at(kernel), 8, 8, w_term);

#else
  af::setDevice(0);
  auto const measurements = gpu::measurementoperator::init_degrid_operator_2d(
      world, std::get<0>(data), ground_truth_image.rows(), ground_truth_image.cols(), cellsize,
      cellsize, 2, 100, 1e-4, kernels::kernel_from_string.at(kernel), 8, 8, w_term);

#endif
#elif PURIFY_PADMM_ALGORITHM == 1
#ifndef PURIFY_GPU
  auto const measurements = measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
      world, std::get<0>(data), ground_truth_image.rows(), ground_truth_image.cols(), cellsize,
      cellsize, 2, 100, 1e-4, kernels::kernel_from_string.at(kernel), 8, 8, w_term);

#else
  af::setDevice(0);
  auto const measurements = gpu::measurementoperator::init_degrid_operator_2d_mpi(
      world, std::get<0>(data), ground_truth_image.rows(), ground_truth_image.cols(), cellsize,
      cellsize, 2, 100, 1e-4, kernels::kernel_from_string.at(kernel), 8, 8, w_term);

#endif
#endif
  auto const sara = sopt::wavelets::distribute_sara(
      sopt::wavelets::SARA{
          std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
          std::make_tuple("DB3", 3u), std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
          std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)
          },
      world);

  // Create the padmm solver
  auto const padmm = padmm_factory(measurements, sara, ground_truth_image, std::get<0>(data),
                                   std::get<1>(data), world);
  // calls padmm
  auto const diagnostic = (*padmm)();

  // makes sure we set things up correctly
  assert(diagnostic.x.size() == ground_truth_image.size());
  assert(world.broadcast(diagnostic.x).isApprox(diagnostic.x));

  // then writes stuff to files
  auto const residual_image = (measurements->adjoint() * diagnostic.residual).real();
  auto const dirty_image = (measurements->adjoint() * std::get<0>(data).vis).real();
  if(world.is_root()) {
    boost::filesystem::path const path(output_filename(name));
#if PURIFY_PADMM_ALGORITHM == 3
    auto const pb_path = path / kernel / "local_epsilon_replicated_grids";
#elif PURIFY_PADMM_ALGORITHM == 2
    auto const pb_path = path / kernel / "global_epsilon_replicated_grids";
#elif PURIFY_PADMM_ALGORITHM == 1
    auto const pb_path = path / kernel / "local_epsilon_distributed_grids";
#else
#error Unknown or unimplemented algorithm
#endif
    boost::filesystem::create_directories(pb_path);

    pfitsio::write2d(ground_truth_image.real(), (path / "input.fits").native());
    pfitsio::write2d(dirty_image, ground_truth_image.rows(), ground_truth_image.cols(),
                     (pb_path / "dirty.fits").native());
    pfitsio::write2d(diagnostic.x.real(), ground_truth_image.rows(), ground_truth_image.cols(),
                     (pb_path / "solution.fits").native());
    pfitsio::write2d(residual_image, ground_truth_image.rows(), ground_truth_image.cols(),
                     (pb_path / "residual.fits").native());
  }
  return 0;
}
