
#ifndef ALGORITHM_OPERATOR_FACTORY_H
#define ALGORITHM_FACTORY_H

#include "purify/config.h"

#include "purify/logging.h"
#include "purify/types.h"
#include <sopt/mpi/communicator.h>

namespace purify {
namespace factory {
  enum class algorithm {padmm, primal_dual, sdmm, forward_backward};
  enum class algo_distribution {serial, mpi_serial, mpi_distributed};

template <class Algorithm>
typename std::enable_if<std::is_same<Algorithm, sopt::algorithm::ImagingProximalADMM<t_complex>>::value,std::shared_ptr<Algorithm> >::type
padmm_factory(const algo_distribution dist, std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const &measurements,
              std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const &Psi, 
              const utilities::vis_params &uv_data, const t_real sigma, const t_uint imsizey, const t_uint imsizex);

//! return chosen algorithm given parameters
template <class Algorithm, class... ARGS>
    std::shared_ptr<Algorithm>
    algorithm_factory(const factory::algorithm algo, ARGS &&... args);

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

template <class Algorithm>
typename std::enable_if<std::is_same<Algorithm, sopt::algorithm::ImagingProximalADMM<t_complex>>::value,std::shared_ptr<Algorithm> >::type
padmm_factory(const algo_distribution dist, std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const &measurements,
              std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const &Psi, 
              const utilities::vis_params &uv_data, const t_real sigma, const t_uint imsizey, const t_uint imsizex) {
  typedef typename Algorithm::Scalar t_scalar;
  // shared pointer because the convergence function need access to some data that we would rather
  // not reproduce. E.g. padmm definition is self-referential.
  auto epsilon = 3 * std::sqrt(2 * uv_data.size()) * sigma;
  auto gamma = 
      ((measurements->adjoint() * uv_data.vis)).real().maxCoeff() * 1e-3;
  auto padmm = std::make_shared<Algorithm>(uv_data.vis);
  padmm->itermax(50)
      .gamma(gamma)
      .relative_variation(1e-3)
      .tight_frame(false)
      .l1_proximal_tolerance(1e-2)
      .l1_proximal_nu(1)
      .l1_proximal_itermax(50)
      .l1_proximal_positivity_constraint(true)
      .l1_proximal_real_constraint(true)
      .residual_tolerance(epsilon)
      .lagrange_update_scale(0.9)
      .nu(1e0)
      .Psi(*Psi)
      .Phi(*measurements);
  auto convergence_function = [](const Vector<t_complex> &x) { return true; };
  switch(dist) {
    case(algo_distribution::serial):
      {
      return padmm;
      }
#ifdef PURIFY_MPI
    case(algo_distribution::mpi_serial): 
      {
  auto const comm = sopt::mpi::Communicator::World();
  epsilon = 3 * std::sqrt(comm.all_sum_all(std::pow(sigma, 2)))
                       * std::sqrt(2 * comm.all_sum_all(uv_data.size()));
   padmm->l2ball_proximal_communicator(comm);
      }
    case(algo_distribution::mpi_distributed): 
      {
  epsilon = 3 * std::sqrt(2 * uv_data.size()) * sigma;
      }
  auto const comm = sopt::mpi::Communicator::World();
  PURIFY_MEDIUM_LOG("Epsilon {}", epsilon);
  PURIFY_MEDIUM_LOG("Gamma {}", gamma);
  padmm->gamma(gamma)
      // communicator ensuring l1 norm in l1 proximal is global
      .l1_proximal_adjoint_space_comm(comm)
      .l2ball_proximal_epsilon(epsilon);
#endif
  }
}
}
}
#endif
