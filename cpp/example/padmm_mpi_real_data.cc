#include "purify/types.h"
#include <array>
#include <memory>
#include <random>
#include <boost/filesystem.hpp>
#include <boost/math/special_functions/erf.hpp>
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/mpi_utilities.h"
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include "purify/read_measurements.h"
#include "purify/utilities.h"
#include "purify/uvfits.h"
#include <sopt/imaging_padmm.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>

#ifdef PURIFY_GPU
#include "purify/operators_gpu.h"
#endif

#ifndef PURIFY_PADMM_ALGORITHM
#define PURIFY_PADMM_ALGORITHM 2
#endif

using namespace purify;
using namespace purify::notinstalled;

utilities::vis_params dirty_visibilities(const std::vector<std::string> &names) {
  return utilities::read_visibility(names, true);
}

utilities::vis_params dirty_visibilities(const std::vector<std::string> &names,
                                         sopt::mpi::Communicator const &comm) {
  if (comm.size() == 1) return dirty_visibilities(names);
  if (comm.is_root()) {
    auto result = dirty_visibilities(names);
    auto const order = distribute::distribute_measurements(result, comm, distribute::plan::w_term);
    return utilities::regroup_and_scatter(result, order, comm);
  }
  auto result = utilities::scatter_visibilities(comm);
  return result;
}

std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>> padmm_factory(
    std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> const &measurements,
    t_real const sigma, const sopt::wavelets::SARA &sara, const utilities::vis_params &uv_data,
    const sopt::mpi::Communicator &comm, const t_uint &imsizex, const t_uint &imsizey) {
  auto const Psi = sopt::linear_transform<t_complex>(sara, imsizey, imsizex, comm);

#if PURIFY_PADMM_ALGORITHM == 2
  auto const epsilon = 3 * std::sqrt(comm.all_sum_all(std::pow(sigma, 2))) *
                       std::sqrt(2 * comm.all_sum_all(uv_data.size()));
#elif PURIFY_PADMM_ALGORITHM == 3 || PURIFY_PADMM_ALGORITHM == 1
  auto const epsilon = 3 * std::sqrt(2 * uv_data.size()) * sigma;
#endif
  const t_real gamma =
      utilities::step_size(uv_data.vis, measurements,
                           std::make_shared<sopt::LinearTransform<Vector<t_complex>> const>(Psi),
                           sara.size()) *
      1e-3;
  PURIFY_MEDIUM_LOG("Epsilon {}", epsilon);
  PURIFY_MEDIUM_LOG("Gamma {}", gamma);

  // shared pointer because the convergence function need access to some data that we would rather
  // not reproduce. E.g. padmm definition is self-referential.
  auto padmm = std::make_shared<sopt::algorithm::ImagingProximalADMM<t_complex>>(uv_data.vis);
  padmm->itermax(50)
      .gamma(comm.all_reduce<t_real>(gamma, MPI_MAX))
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
  padmm->residual_convergence([padmm_weak, conv, comm](
                                  Vector<t_complex> const &x,
                                  Vector<t_complex> const &residual) mutable -> bool {
    auto const padmm = padmm_weak.lock();
#if PURIFY_PADMM_ALGORITHM == 2
    auto const residual_norm = sopt::mpi::l2_norm(residual, padmm->l2ball_proximal_weights(), comm);
    auto const result = residual_norm < padmm->residual_tolerance();
#elif PURIFY_PADMM_ALGORITHM == 3 || PURIFY_PADMM_ALGORITHM == 1
    auto const residual_norm = sopt::l2_norm(residual, padmm->l2ball_proximal_weights());
    auto const result =
        comm.all_reduce<int8_t>(residual_norm < padmm->residual_tolerance(), MPI_LAND);
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

  auto convergence_function = [](const Vector<t_complex> &x) { return true; };
  const std::shared_ptr<t_uint> iter = std::make_shared<t_uint>(0);
  const auto algo_update = [uv_data, imsizex, imsizey, padmm_weak, iter,
                            comm](const Vector<t_complex> &x) -> bool {
    auto padmm = padmm_weak.lock();
    if (comm.is_root()) PURIFY_MEDIUM_LOG("Step size γ {}", padmm->gamma());
    *iter = *iter + 1;
    Vector<t_complex> const alpha = padmm->Psi().adjoint() * x;
    const t_real new_gamma = comm.all_reduce(alpha.real().cwiseAbs().maxCoeff(), MPI_MAX) * 1e-3;
    if (comm.is_root()) PURIFY_MEDIUM_LOG("Step size γ update {}", new_gamma);
    padmm->gamma(((std::abs(padmm->gamma() - new_gamma) > 0.2) and *iter < 200) ? new_gamma
                                                                                : padmm->gamma());
    // updating parameter

    Vector<t_complex> const residual = padmm->Phi().adjoint() * (uv_data.vis - padmm->Phi() * x);

    if (comm.is_root()) {
      pfitsio::write2d(x, imsizey, imsizex, "mpi_solution_update.fits");
      pfitsio::write2d(residual, imsizey, imsizex, "mpi_residual_update.fits");
    }
    return true;
  };
  auto lambda = [convergence_function, algo_update](Vector<t_complex> const &x) {
    return convergence_function(x) and algo_update(x);
  };
  padmm->is_converged(lambda);
  return padmm;
}

int main(int nargs, char const **args) {
  sopt::logging::initialize();
  purify::logging::initialize();
  sopt::logging::set_level("debug");
  purify::logging::set_level("debug");
  auto const session = sopt::mpi::init(nargs, args);
  auto const world = sopt::mpi::Communicator::World();

  const std::string name = "realdata";
  const std::string filename_base = vla_filename("../mwa/uvdump_");
  const std::vector<std::string> filenames = {filename_base +
                                              "01.vis"};  //, filename_base + "02.vis"};
  auto const kernel = kernels::kernel::kb;
  std::string kernel_name = "kb";
  const bool w_term = false;

  const t_real cellsize = 20;  // arcsec
  const t_uint imsizex = 1024;
  const t_uint imsizey = 1024;

  // Generating random uv(w) coverage
  utilities::vis_params data = dirty_visibilities(filenames, world);

  t_real const sigma =
      data.weights.norm() / std::sqrt(world.all_sum_all(data.weights.size())) * 0.5;
  data.vis = (data.vis.array() * data.weights.array()) /
             world.all_reduce(data.weights.array().cwiseAbs().maxCoeff(), MPI_MAX);
#if PURIFY_PADMM_ALGORITHM == 2 || PURIFY_PADMM_ALGORITHM == 3
#ifndef PURIFY_GPU
  auto const measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      world, data, imsizey, imsizex, cellsize, cellsize, 2, 100, 1e-4, kernel, 4, 4, w_term);
#else
  af::setDevice(0);
  auto const measurements = gpu::measurementoperator::init_degrid_operator_2d(
      world, data, imsizey, imsizex, cellsize, cellsize, 2, 100, 1e-4, kernel, 4, 4, w_term);

#endif
#elif PURIFY_PADMM_ALGORITHM == 1
#ifndef PURIFY_GPU
  auto const measurements = measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
      world, data, imsizey, imsizex, cellsize, cellsize, 2, 100, 1e-4, kernel, 4, 4, w_term);

#else
  af::setDevice(0);
  auto const measurements = gpu::measurementoperator::init_degrid_operator_2d_mpi(
      world, data, imsizey, imsizex, cellsize, cellsize, 2, 100, 1e-4, kernel, 4, 4, w_term);

#endif
#endif
  auto const sara = sopt::wavelets::distribute_sara(
      sopt::wavelets::SARA{
          std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
          std::make_tuple("DB3", 3u), std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
          std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)},
      world);

  Vector<t_real> const dirty_image = (measurements->adjoint() * (data.vis)).real();

  if (world.is_root()) {
    // then writes stuff to files
    boost::filesystem::path const path(output_filename(name));
#if PURIFY_PADMM_ALGORITHM == 3
    auto const pb_path = path / kernel_name / "local_epsilon_replicated_grids";
#elif PURIFY_PADMM_ALGORITHM == 2
    auto const pb_path = path / kernel_name / "global_epsilon_replicated_grids";
#elif PURIFY_PADMM_ALGORITHM == 1
    auto const pb_path = path / kernel_name / "local_epsilon_distributed_grids";
#else
#error Unknown or unimplemented algorithm
#endif
    mkdir_recursive(pb_path);

    pfitsio::write2d(dirty_image, imsizey, imsizex, (pb_path / "dirty.fits").native());
  }

  // Create the padmm solver
  auto const padmm = padmm_factory(measurements, sigma, sara, data, world, imsizey, imsizex);
  // calls padmm
  auto const diagnostic = (*padmm)();

  // makes sure we set things up correctly
  assert(world.broadcast(diagnostic.x).isApprox(diagnostic.x));

  Vector<t_real> const residual_image = (measurements->adjoint() * diagnostic.residual).real();
  if (world.is_root()) {
    // then writes stuff to files
    boost::filesystem::path const path(output_filename(name));
#if PURIFY_PADMM_ALGORITHM == 3
    auto const pb_path = path / kernel_name / "local_epsilon_replicated_grids";
#elif PURIFY_PADMM_ALGORITHM == 2
    auto const pb_path = path / kernel_name / "global_epsilon_replicated_grids";
#elif PURIFY_PADMM_ALGORITHM == 1
    auto const pb_path = path / kernel_name / "local_epsilon_distributed_grids";
#else
#error Unknown or unimplemented algorithm
#endif
    mkdir_recursive(pb_path);

    pfitsio::write2d(dirty_image, imsizey, imsizex, (pb_path / "dirty.fits").native());
    pfitsio::write2d(diagnostic.x.real(), imsizey, imsizex, (pb_path / "solution.fits").native());
    pfitsio::write2d(residual_image, imsizey, imsizex, (pb_path / "residual.fits").native());
  }
  return 0;
}
