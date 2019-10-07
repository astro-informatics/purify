#ifndef ALGORITHM_FACTORY_H
#define ALGORITHM_FACTORY_H

#include "purify/config.h"

#include "purify/types.h"
#include "purify/convergence_factory.h"
#include "purify/logging.h"
#include "purify/utilities.h"

#ifdef PURIFY_MPI
#include "purify/mpi_utilities.h"
#include <sopt/mpi/communicator.h>
#endif

#include <sopt/imaging_forward_backward.h>
#include <sopt/imaging_padmm.h>
#include <sopt/imaging_primal_dual.h>
#include <sopt/joint_map.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>

namespace purify {
namespace factory {
enum class algorithm { padmm, primal_dual, sdmm, forward_backward };
enum class algo_distribution { serial, mpi_serial, mpi_distributed, mpi_random_updates };
const std::map<std::string, algo_distribution> algo_distribution_string = {
    {"none", algo_distribution::serial},
    {"serial-equivalent", algo_distribution::mpi_serial},
    {"fully-distributed", algo_distribution::mpi_distributed}};

//! return chosen algorithm given parameters
template <class Algorithm, class... ARGS>
std::shared_ptr<Algorithm> algorithm_factory(const factory::algorithm algo, ARGS &&... args);
//! return shared pointer to padmm object
template <class Algorithm>
typename std::enable_if<
    std::is_same<Algorithm, sopt::algorithm::ImagingProximalADMM<t_complex>>::value,
    std::shared_ptr<Algorithm>>::type
padmm_factory(const algo_distribution dist,
              std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const
                  &measurements,
              std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const
                  &wavelets,
              const utilities::vis_params &uv_data, const t_real sigma, const t_uint imsizey,
              const t_uint imsizex, const t_uint sara_size, const t_uint max_iterations = 500,
              const bool real_constraint = true, const bool positive_constraint = true,
              const bool tight_frame = false, const t_real relative_variation = 1e-3,
              const t_real l1_proximal_tolerance = 1e-2,
              const t_uint maximum_proximal_iterations = 50) {
  typedef typename Algorithm::Scalar t_scalar;
  if (sara_size > 1 and tight_frame)
    throw std::runtime_error(
        "l1 proximal not consistent: You say you are using a tight frame, but you have more than "
        "one wavelet basis.");
  auto epsilon =
      std::sqrt(2 * uv_data.size() + 2 * std::sqrt(4 * uv_data.size())) * sigma / std::sqrt(2);
  auto padmm = std::make_shared<Algorithm>(uv_data.vis);
  padmm->itermax(max_iterations)
      .relative_variation(relative_variation)
      .tight_frame(tight_frame)
      .l1_proximal_tolerance(l1_proximal_tolerance)
      .l1_proximal_nu(1.)
      .l1_proximal_itermax(maximum_proximal_iterations)
      .l1_proximal_positivity_constraint(positive_constraint)
      .l1_proximal_real_constraint(real_constraint)
      .lagrange_update_scale(0.9)
      .nu(1e0)
      .Psi(*wavelets)
      .Phi(*measurements);
#ifdef PURIFY_MPI
  ConvergenceType obj_conv = ConvergenceType::mpi_global;
  ConvergenceType rel_conv = ConvergenceType::mpi_global;
#endif
  switch (dist) {
  case (algo_distribution::serial): {
    padmm
        ->gamma((wavelets->adjoint() * (measurements->adjoint() * uv_data.vis).eval())
                    .cwiseAbs()
                    .maxCoeff() *
                1e-3)
        .l2ball_proximal_epsilon(epsilon)
        .residual_tolerance(epsilon);
    return padmm;
  }
#ifdef PURIFY_MPI
  case (algo_distribution::mpi_serial): {
    obj_conv = ConvergenceType::mpi_global;
    rel_conv = ConvergenceType::mpi_global;
    auto const comm = sopt::mpi::Communicator::World();
    epsilon = std::sqrt(2 * comm.all_sum_all(uv_data.size()) +
                        2 * std::sqrt(4 * comm.all_sum_all(uv_data.size()))) *
              sigma / std::sqrt(2);
    // communicator ensuring l2 norm in l2ball proximal is global
    padmm->l2ball_proximal_communicator(comm);
    break;
  }
  case (algo_distribution::mpi_distributed): {
    obj_conv = ConvergenceType::mpi_local;
    rel_conv = ConvergenceType::mpi_local;
    auto const comm = sopt::mpi::Communicator::World();
    epsilon = std::sqrt(2 * uv_data.size() + 2 * std::sqrt(4 * uv_data.size()) *
                                                 std::sqrt(comm.all_sum_all(4 * uv_data.size())) /
                                                 comm.all_sum_all(std::sqrt(4 * uv_data.size()))) *
              sigma / std::sqrt(2);
    break;
  }
#endif
  default:
    throw std::runtime_error(
        "Type of distributed proximal ADMM algorithm not recognised. You might not have compiled "
        "with MPI.");
  }
#ifdef PURIFY_MPI
  auto const comm = sopt::mpi::Communicator::World();
  std::weak_ptr<Algorithm> const padmm_weak(padmm);
  // set epsilon
  padmm->residual_tolerance(epsilon).l2ball_proximal_epsilon(epsilon);
  // set gamma
  padmm->gamma(comm.all_reduce(
      utilities::step_size(uv_data.vis, measurements, wavelets, sara_size) * 1e-3, MPI_MAX));
  // communicator ensuring l1 norm in l1 proximal is global
  padmm->l1_proximal_adjoint_space_comm(comm);
  padmm->residual_convergence(
      purify::factory::l2_convergence_factory<typename Algorithm::Scalar>(rel_conv, padmm_weak));
  padmm->objective_convergence(
      purify::factory::l1_convergence_factory<typename Algorithm::Scalar>(obj_conv, padmm_weak));
#endif
  return padmm;
}

//! return shared pointer to forward backward object
template <class Algorithm>
typename std::enable_if<
    std::is_same<Algorithm, sopt::algorithm::ImagingForwardBackward<t_complex>>::value,
    std::shared_ptr<Algorithm>>::type
fb_factory(const algo_distribution dist,
           std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const
               &measurements,
           std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const
               &wavelets,
           const utilities::vis_params &uv_data, const t_real sigma, const t_real step_size,
           const t_real reg_parameter, const t_uint imsizey, const t_uint imsizex,
           const t_uint sara_size, const t_uint max_iterations = 500,
           const bool real_constraint = true, const bool positive_constraint = true,
           const bool tight_frame = false, const t_real relative_variation = 1e-3,
           const t_real l1_proximal_tolerance = 1e-2,
           const t_uint maximum_proximal_iterations = 50) {
  typedef typename Algorithm::Scalar t_scalar;
  if (sara_size > 1 and tight_frame)
    throw std::runtime_error(
        "l1 proximal not consistent: You say you are using a tight frame, but you have more than "
        "one wavelet basis.");
  auto fb = std::make_shared<Algorithm>(uv_data.vis);
  fb->itermax(max_iterations)
      .gamma(reg_parameter)
      .sigma(sigma)
      .beta(step_size)
      .relative_variation(relative_variation)
      .tight_frame(tight_frame)
      .l1_proximal_tolerance(l1_proximal_tolerance)
      .l1_proximal_nu(1.)
      .l1_proximal_itermax(maximum_proximal_iterations)
      .l1_proximal_positivity_constraint(positive_constraint)
      .l1_proximal_real_constraint(real_constraint)
      .nu(1e0)
      .Psi(*wavelets)
      .Phi(*measurements);
  switch (dist) {
  case (algo_distribution::serial): {
    break;
  }
#ifdef PURIFY_MPI
  case (algo_distribution::mpi_serial): {
    auto const comm = sopt::mpi::Communicator::World();
    fb->l1_proximal_adjoint_space_comm(comm);
    fb->obj_comm(comm);
    break;
  }
#endif
  default:
    throw std::runtime_error(
        "Type of distributed Forward Backward algorithm not recognised. You might not have "
        "compiled "
        "with MPI.");
  }
  return fb;
}

//! return shared pointer to primal dual object
template <class Algorithm>
typename std::enable_if<
    std::is_same<Algorithm, sopt::algorithm::ImagingPrimalDual<t_complex>>::value,
    std::shared_ptr<Algorithm>>::type
primaldual_factory(
    const algo_distribution dist,
    std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const
        &measurements,
    std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const
        &wavelets,
    const utilities::vis_params &uv_data, const t_real sigma, const t_uint imsizey,
    const t_uint imsizex, const t_uint sara_size, const t_uint max_iterations = 500,
    const bool real_constraint = true, const bool positive_constraint = true,
    const t_real relative_variation = 1e-3) {
  typedef typename Algorithm::Scalar t_scalar;
  auto epsilon =
      std::sqrt(2 * uv_data.size() + 2 * std::sqrt(4 * uv_data.size())) * sigma / std::sqrt(2);
  auto primaldual = std::make_shared<Algorithm>(uv_data.vis);
  primaldual->itermax(max_iterations)
      .relative_variation(relative_variation)
      .real_constraint(real_constraint)
      .positivity_constraint(positive_constraint)
      .Psi(*wavelets)
      .Phi(*measurements);
#ifdef PURIFY_MPI
  ConvergenceType obj_conv = ConvergenceType::mpi_global;
  ConvergenceType rel_conv = ConvergenceType::mpi_global;
#endif
  switch (dist) {
  case (algo_distribution::serial): {
    primaldual
        ->gamma((wavelets->adjoint() * (measurements->adjoint() * uv_data.vis).eval())
                    .cwiseAbs()
                    .maxCoeff() *
                1e-3)
        .l2ball_proximal_epsilon(epsilon)
        .residual_tolerance(epsilon);
    return primaldual;
  }
#ifdef PURIFY_MPI
  case (algo_distribution::mpi_serial): {
    obj_conv = ConvergenceType::mpi_global;
    rel_conv = ConvergenceType::mpi_global;
    auto const comm = sopt::mpi::Communicator::World();
    epsilon = std::sqrt(2 * comm.all_sum_all(uv_data.size()) +
                        2 * std::sqrt(4 * comm.all_sum_all(uv_data.size()))) *
              sigma;
    // communicator ensuring l2 norm in l2ball proximal is global
    primaldual->l2ball_proximal_communicator(comm);
    break;
  }
  case (algo_distribution::mpi_distributed): {
    obj_conv = ConvergenceType::mpi_local;
    rel_conv = ConvergenceType::mpi_local;
    auto const comm = sopt::mpi::Communicator::World();
    epsilon = std::sqrt(2 * uv_data.size() + 2 * std::sqrt(4 * uv_data.size()) *
                                                 std::sqrt(comm.all_sum_all(4 * uv_data.size())) /
                                                 comm.all_sum_all(std::sqrt(4 * uv_data.size()))) *
              sigma / std::sqrt(2);
    break;
  }
  case (algo_distribution::mpi_random_updates): {
    auto const comm = sopt::mpi::Communicator::World();
    epsilon = std::sqrt(2 * uv_data.size() + 2 * std::sqrt(4 * uv_data.size()) *
                                                 std::sqrt(comm.all_sum_all(4 * uv_data.size())) /
                                                 comm.all_sum_all(std::sqrt(4 * uv_data.size()))) *
              sigma / std::sqrt(2);
    obj_conv = ConvergenceType::mpi_local;
    rel_conv = ConvergenceType::mpi_local;
    auto const comm = sopt::mpi::Communicator::World();
    const t_int measurement_sampling_size = std::floor(0.5 * comm.size());
    const t_int wavelet_sampling_size = std::floor(0.5 * comm.size());
    std::shared_ptr<std::vector<t_int>> ind;
    for (t_int i = 0; i < comm.size(); i++) ind->push_back(i);
    auto random_measurement_updater = [measurement_sampling_size, ind, comm] -> bool {
      if (comm.is_root()) std::random_shuffle(ind->begin(), ind->end());
      *ind = comm.broadcast(*ind);
      for (t_int i = 0; i < measurement_sampling_size; i++)
        if (comm.rank() == ind->at(i)) return true;
      return false;
    };
    auto random_wavelet_updater = [wavelet_sampling_size, ind, comm] -> bool {
      if (comm.is_root()) std::random_shuffle(ind->begin(), ind->end());
      *ind = comm.broadcast(*ind);
      for (t_int i = 0; i < wavelet_sampling_size; i++)
        if (comm.rank() == ind->at(i)) return true;
      return false;
    };
    primaldual->random_measurement_updater(random_measurement_updater)
        .random_wavelet_updater(random_wavelet_updater)
        .v_all_sum_all_comm(comm)
        .u_all_sum_all_comm(comm);

    break;
  }
#endif
  default:
    throw std::runtime_error(
        "Type of distributed primal dual algorithm not recognised. You might not have compiled "
        "with MPI.");
  }
#ifdef PURIFY_MPI
  auto const comm = sopt::mpi::Communicator::World();
  std::weak_ptr<Algorithm> const primaldual_weak(primaldual);
  // set epsilon
  primaldual->residual_tolerance(epsilon).l2ball_proximal_epsilon(epsilon);
  // set gamma
  primaldual->gamma(comm.all_reduce(
      utilities::step_size(uv_data.vis, measurements, wavelets, sara_size) * 1e-3, MPI_MAX));
  // communicator ensuring l1 norm in l1 proximal is global
  primaldual->residual_convergence(
      purify::factory::l2_convergence_factory<typename Algorithm::Scalar>(rel_conv,
                                                                          primaldual_weak));
  primaldual->objective_convergence(
      purify::factory::l1_convergence_factory<typename Algorithm::Scalar>(obj_conv,
                                                                          primaldual_weak));
#endif
  return primaldual;
}

template <class Algorithm, class... ARGS>
std::shared_ptr<Algorithm> algorithm_factory(const factory::algorithm algo, ARGS &&... args) {
  switch (algo) {
  case algorithm::padmm:
    return padmm_factory<Algorithm>(std::forward<ARGS>(args)...);
    break;
    /*
  case algorithm::primal_dual:
    return pd_factory(std::forward<ARGS>(args)...);
    break;
  case algorithm::sdmm:
    return sdmm_factory(std::forward<ARGS>(args)...);
    break;
  case algorithm::forward_backward:
    return fb_factory(std::forward<ARGS>(args)...);
    break;
    */
  default:
    throw std::runtime_error("Algorithm not implimented.");
  }
}

}  // namespace factory
}  // namespace purify
#endif
