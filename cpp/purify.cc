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

#ifdef PURIFY_ARRAYFIRE
#include "purify/operators_gpu.h"
#endif

#ifndef PURIFY_PADMM_ALGORITHM
#define PURIFY_PADMM_ALGORITHM 2
#endif

using namespace purify;
using namespace purify::notinstalled;

std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const>
measurement_operator_factory(const Parameters &params, const utilities::vis_params &uv_data) {
#ifdef PURIFY_MPI
#if PURIFY_PADMM_ALGORITHM == 2 || PURIFY_PADMM_ALGORITHM == 3
#ifdef PURIFY_GPU
  af::setDevice(0);
  auto const measurements = gpu::measurementoperator::init_degrid_operator_2d(
      params.comm, uv_data, params.image_height, params.image_width, params.cellsizey,
      params.cellsizex, params.oversample_ratio, params.power_method_iterations,
      params.power_method_tolerance, params.kernel, params.J, params.J, w_term,
      params.energy_chirp_fraction, params.energy_kernel_fraction);

#else
  auto const measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      params.comm, uv_data, params.image_height, params.image_width, params.cellsizey,
      params.cellsizex, params.oversample_ratio, params.power_method_iterations,
      params.power_method_tolerance, params.kernel, params.J, params.J, params.fftw_plan, w_term,
      params.energy_chirp_fraction, params.energy_kernel_fraction);
#endif
#elif PURIFY_PADMM_ALGORITHM == 1
#ifdef PURIFY_GPU
  af::setDevice(0);
  auto const measurements = gpu::measurementoperator::init_degrid_operator_2d_mpi(
      params.comm, uv_data, params.image_height, params.image_width, params.cellsizey,
      params.cellsizex, params.oversample_ratio, params.power_method_iterations,
      params.power_method_tolerance, params.kernel, params.J, params.J, w_term,
      params.energy_chirp_fraction, params.energy_kernel_fraction);
#else
  auto const measurements = measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
      params.comm, uv_data, params.image_height, params.image_width, params.cellsizey,
      params.cellsizex, params.oversample_ratio, params.power_method_iterations,
      params.power_method_tolerance, params.kernel, params.J, params.J, params.fftw_plan, w_term,
      params.energy_chirp_fraction, params.energy_kernel_fraction);
#endif
#endif
#else
#ifdef PURIFY_GPU
  af::setDevice(0);
  auto const measurements = gpu::measurementoperator::init_degrid_operator_2d(
      uv_data, params.image_height, params.image_width, params.cellsizey, params.cellsizex,
      params.oversample_ratio, params.power_method_iterations, params.power_method_tolerance,
      params.kernel, params.J, params.J, w_term, params.energy_chirp_fraction,
      params.energy_kernel_fraction);

#else
  auto const measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uv_data, params.image_height, params.image_width, params.cellsizey, params.cellsizex,
      params.oversample_ratio, params.power_method_iterations, params.power_method_tolerance,
      params.kernel, params.J, params.J, params.fftw_plan, w_term, params.energy_chirp_fraction,
      params.energy_kernel_fraction);
#endif
#endif
  return measurements;
};
std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const>
wavelet_operator_factory(const Parameters &params) {
  auto const sara = sopt::wavelets::distribute_sara(
      sopt::wavelets::SARA{
          std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
          std::make_tuple("DB3", 3u), std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
          std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)},
      world);

  return std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(
      sara, params.image_height, params.image_width, params.comm);
};

std::vector<utilities::vis_params> read_measurements(const Parameters &params) {
  // checking if reading measurement set or .vis file
  std::size_t found = params.visfile.find_last_of(".");
  std::string format = "." + params.visfile.substr(found + 1);
  std::transform(format.begin(), format.end(), format.begin(), ::tolower);
  return (format == ".ms") ?
             purify::casa::read_measurementset_channels(params.visfile, params.stokes_val, 0) :
             std::vector<utilities::vis_params>{
                 utilities::read_visibility(params.visfile, params.use_w_term)};
}

std::vector<utilities::vis_params> distribute_measurements(const Parameters &params) {
  if(params.comm.size() == 1)
    return read_measurements(params);
  if(params.comm.is_root()) {
    const auto uv_data = read_measurements(params);
    const t_uint channels = params.comm.broadcast(uv_data.size());
    std::vector<utilities::vis_params> distributed_channels(channels);
    for(t_int i = 0; i < channels; i++) {
      auto const order
          = distribute::distribute_measurements(uv_data[i], params.comm, "distance_distribution");
      distributed_channels[i] = utilities::regroup_and_scatter(uv_data[i], order, params.comm);
    }
    return distributed_channels;
  }
  const t_uint channels = params.comm.broadcast();
  std::vector<utilities::vis_params> distributed_channels(channels);
  for(t_int i = 0; i < channels; i++) {
    distributed_channels[i] = utilities::scatter_visibilities(params.comm);
  }
  return distributed_channels;
}

std::shared_ptr<sopt::algorithm> algorithm_factory(
    const Parameters &params,
    std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> const &measurements,
    std::shared_ptr < sopt::LinearTransform<Vector<t_complex>> const &psi,
    const utilities::vis_params &uv_data) {
  if(params.sopt_algorithm == algorithm::padmm)
    return padmm_factory(measurements, psi, uv_data, params.comm);
  else
    throw std::runtime_error("Algorithm not implimented for purify yet.");
}

std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>>
padmm_factory(std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> const &measurements,
              std::shared_ptr < sopt::LinearTransform<Vector<t_complex>> const &psi,
              const utilities::vis_params &uv_data, const sopt::mpi::Communicator &comm) {

#if PURIFY_PADMM_ALGORITHM == 2
  auto const epsilon = std::sqrt(
      comm.all_sum_all(std::pow(utilities::calculate_l2_radius(uv_data.vis, sigma), 2)));
#elif PURIFY_PADMM_ALGORITHM == 3 || PURIFY_PADMM_ALGORITHM == 1
  auto const epsilon = utilities::calculate_l2_radius(uv_data.vis, sigma);
#endif
  auto const gamma
      = (psi->adjoint() * (measurements->adjoint() * uv_data.vis)).cwiseAbs().maxCoeff() * 1e-3;
  PURIFY_MEDIUM_LOG("Epsilon {}", epsilon);
  PURIFY_MEDIUM_LOG("Gamma {}", gamma);

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
      .Psi(*psi)
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
  const Parameters params = get_params();
  sopt::logging::initialize();
  purify::logging::initialize();
  sopt::logging::set_level(params.sopt_logging_level);
  purify::logging::set_level(params.purify_logging_level);
  auto const session = sopt::mpi::init(nargs, args);

  // Reading and distributing uv(w) coverage
  const std::vector<utilities::vis_params> uv_channels = distribute_measurements(params);
  // Create wavelet operator
  auto const psi = sopt::linear_transform<t_complex>(params.wavelet_dictionary, params.image_height,
                                                     params.image_width);
  auto std::vector<Vector<t_real>> dirty_images;
  auto std::vector<Vector<t_real>> residual_images;
  auto std::vector<Vector<t_real>> solution_images;
  for(t_int i = 0; i < uv_channels.size(); i++) {
    // Select the data
    auto const &uv_data = uv_channels[i];
    // Create measurement operator
    auto const measurement = measurement_operator_factory(params, uv_data);
    // Create the solver
    auto const algorithm = algorithm_factory(params, measurements, psi, uv_data);
    // calls sovler
    auto const diagnostic = (*algorithm)();

    // makes sure we set things up correctly
    assert(diagnostic.x.size() == params.image_width * params.image_height;
    assert(params.comm.broadcast(diagnostic.x).isApprox(diagnostic.x));

    // then writes stuff to files
    auto const residual_image = (measurements->adjoint() * diagnostic.residual).real();
    auto const dirty_image = (measurements->adjoint() * uv_data.vis).real();
    if(params.comm.is_root()) {
      solution_images.push_back(diagnostic.x.real());
      dirty_images.push_back(dirty_image);
      residual_images.push_back(residual_image);
      save_image_cube("dirty", dirty_images, params, uv_data);
      save_image_cube("residual", residual_images, params, uv_data);
      save_image_cube("solution", solution_images, params, uv_data);
    }
  }
  return 0;
}
