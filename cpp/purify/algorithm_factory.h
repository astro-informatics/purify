
#ifndef ALGORITHM_OPERATOR_FACTORY_H
#define ALGORITHM_FACTORY_H

#include "purify/config.h"

#include "purify/logging.h"
#include "purify/types.h"

#include "purify/operators.h"

#include <sopt/mpi/communicator.h>

#include <sopt/imaging_padmm.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>

namespace purify {
namespace factory {
  enum class algorithm {padmm, primal_dual, sdmm, forward_backward};
  enum class algo_distribution {serial, mpi_serial, mpi_distributed};

//! return chosen algorithm given parameters
template <class Algorithm, class... ARGS>
    std::shared_ptr<Algorithm>
    algorithm_factory(const factory::algorithm algo, ARGS &&... args);
//! return shared pointer to padmm object
template <class Algorithm>
typename std::enable_if<std::is_same<Algorithm, sopt::algorithm::ImagingProximalADMM<t_complex>>::value,std::shared_ptr<Algorithm> >::type
padmm_factory(const algo_distribution dist, 
              std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const &measurements,
              std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const &wavelets, 
              const utilities::vis_params &uv_data, const t_real sigma, const t_uint imsizey, const t_uint imsizex, 
              const t_uint sara_size,
              const t_uint max_iterations = 500,
              const bool real_constraint = true, 
              const bool positive_constraint = true, 
              const bool tight_frame = false,
              const t_real relative_variation = 1e-3,
              const t_real l1_proximal_tolerance = 1e-2, 
              const t_uint maximum_proximal_iterations = 50) {
  typedef typename Algorithm::Scalar t_scalar;
  if(sara_size > 1 and tight_frame)
    throw std::runtime_error("l1 proximal not consistent: You say you are using a tight frame, but you have more than one wavelet basis.");
  auto epsilon = utilities::calculate_l2_radius(uv_data.vis, sigma);
  auto padmm = std::make_shared<Algorithm>(uv_data.vis);
  padmm->itermax(max_iterations)
      .relative_variation(relative_variation)
      .l2ball_proximal_epsilon(epsilon)
      .tight_frame(tight_frame)
      .l1_proximal_tolerance(l1_proximal_tolerance)
      .l1_proximal_nu(1.)
      .l1_proximal_itermax(maximum_proximal_iterations)
      .l1_proximal_positivity_constraint(positive_constraint)
      .l1_proximal_real_constraint(real_constraint)
      .residual_convergence(epsilon)
      .lagrange_update_scale(0.9)
      .nu(1e0)
      .Psi(*wavelets)
      .Phi(*measurements);
  auto convergence_function = [](const Vector<t_complex> &x) { return true; };
  switch(dist) {
    case(algo_distribution::serial):
      {
      auto gamma =
      (wavelets->adjoint() * (measurements->adjoint() * uv_data.vis)).cwiseAbs().maxCoeff() * 1e-3;
      padmm->gamma(gamma);
      return padmm;
      }
#ifdef PURIFY_MPI
    case(algo_distribution::mpi_serial):
      {
  auto const comm = sopt::mpi::Communicator::World();
  epsilon = 3 * std::sqrt(comm.all_sum_all(std::pow(sigma, 2)))
                       * std::sqrt(2 * comm.all_sum_all(uv_data.size()));
   padmm->l2ball_proximal_communicator(comm);
   break;
      }
    case(algo_distribution::mpi_distributed):
      {
  epsilon = 3 * std::sqrt(2 * uv_data.size()) * sigma;
   break;
      }
#endif
    default:
  throw std::runtime_error("Type of distributed proximal ADMM algorithm not recognised. You might not have compiled with MPI.");
  }
#ifdef PURIFY_MPI
  auto const comm = sopt::mpi::Communicator::World();
  padmm->gamma(utilities::step_size(uv_data.vis, measurements, wavelets, sara_size) * 1e-3)
      // communicator ensuring l1 norm in l1 proximal is global
      .l1_proximal_adjoint_space_comm(comm)
      .l2ball_proximal_epsilon(epsilon);
#endif
  return padmm;
}


template <class Algorithm, class... ARGS>
    std::shared_ptr<Algorithm>
    algorithm_factory(const factory::algorithm algo, ARGS &&... args){
      switch(algo){
        case algorithm::padmm:
          return padmm_factory<Algorithm>(std::forward<ARGS>(args)...);
          break;
          /*
        case algorithm::primal_dual:
          return padmm_factory(std::forward<ARGS>(args)...);
          break;
        case algorithm::sdmm:
          return padmm_factory(std::forward<ARGS>(args)...);
          break;
        case algorithm::forward_backward:
          return padmm_factory(std::forward<ARGS>(args)...);
          break;
          */
        default:
          throw std::runtime_error("Algorithm not implimented.");
      }
    }

}
}
#endif
