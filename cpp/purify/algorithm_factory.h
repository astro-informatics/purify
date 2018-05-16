
#ifndef ALGORITHM_OPERATOR_FACTORY_H
#define ALGORITHM_FACTORY_H

#include "purify/config.h"

#include "purify/logging.h"
#include "purify/types.h"

namespace purify {
namespace factory {
  enum class algorithm {padmm, primal_dual, sdmm, forward_backward};
  enum class algo_distribution {serial, mpi_serial, mpi_distributed};

//! return padmm algorithm given parameters
template <class Algorithm>
typename std::enable_if<std::is_same<Algorithm, sopt::algorithm::ImagingProximalADMM<t_complex>>::value,std::shared_ptr<Algorithm> >::type
padmm_factory(std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const &measurements,
              std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const &Psi, 
              const utilities::vis_params &uv_data, const t_real sigma, const sopt::mpi::Communicator &comm, const t_uint imsizey, const t_uint imsizex);
template <class Algorithm>
typename std::enable_if<std::is_same<Algorithm, sopt::algorithm::ImagingProximalADMM<t_complex>>::value,std::shared_ptr<Algorithm> >::type
padmm_factory(std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const &measurements,
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
padmm_factory(std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const &measurements,
              std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const &Psi, 
              const utilities::vis_params &uv_data, const t_real sigma, const t_uint imsizey, const t_uint imsizex) {
  typedef typename Algorithm::Scalar t_scalar;
  // shared pointer because the convergence function need access to some data that we would rather
  // not reproduce. E.g. padmm definition is self-referential.
  auto const epsilon = 3 * std::sqrt(2 * uv_data.size()) * sigma;
  auto const gamma = 
      ((measurements->adjoint() * uv_data.vis)).real().maxCoeff() * 1e-3;
  auto padmm = std::make_shared<Algorithm>(uv_data.vis);
  padmm->itermax(50)
      .gamma(gamma)
      .relative_variation(1e-3)
      .l2ball_proximal_epsilon(epsilon)
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
  const std::shared_ptr<t_uint> iter = std::make_shared<t_uint>(0);

  std::weak_ptr<typename decltype(padmm)::element_type> const padmm_weak(padmm);
  const auto algo_update
      = [uv_data, imsizex, imsizey, padmm_weak, iter](const Vector<t_complex> &x) -> bool {
    auto padmm = padmm_weak.lock();
    PURIFY_MEDIUM_LOG("Step size γ {}", padmm->gamma());
    *iter = *iter + 1;
    Vector<t_complex> const alpha = padmm->Psi().adjoint() * x;
    // updating parameter
    const t_real new_gamma = alpha.real().cwiseAbs().maxCoeff() * 1e-3;
    PURIFY_MEDIUM_LOG("Step size γ update {}", new_gamma);
    padmm->gamma(((std::abs(padmm->gamma() - new_gamma) > 0.2) and *iter < 200) ? new_gamma :
                                                                                  padmm->gamma());

    Vector<t_complex> const residual = padmm->Phi().adjoint() * (uv_data.vis - padmm->Phi() * x);

    pfitsio::write2d(x, imsizey, imsizex, "solution_update.fits");
    pfitsio::write2d(residual, imsizey, imsizex, "residual_update.fits");
    return true;
  };
  auto lambda = [=](Vector<t_complex> const &x) {
    return convergence_function(x) and algo_update(x);
  };
  padmm->is_converged(lambda);
  return padmm;
}
template <class Algorithm>
typename std::enable_if<std::is_same<Algorithm, sopt::algorithm::ImagingProximalADMM<t_complex>>::value,std::shared_ptr<Algorithm> >::type
padmm_factory(std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const &measurements,
              std::shared_ptr<sopt::LinearTransform<Vector<typename Algorithm::Scalar>> const> const &Psi, 
              const utilities::vis_params &uv_data, const t_real sigma, const sopt::mpi::Communicator &comm,
              const t_uint imsizey, const t_uint imsizex) {

  typedef typename Algorithm::Scalar t_scalar;


  // shared pointer because the convergence function need access to some data that we would rather
  // not reproduce. E.g. padmm definition is self-referential.
  auto padmm = padmm_factory<Algorithm>(measurements, Psi, uv_data, sigma, imsizey, imsizex);
#if PURIFY_PADMM_ALGORITHM == 2
  auto const epsilon = 3 * std::sqrt(comm.all_sum_all(std::pow(sigma, 2)))
                       * std::sqrt(2 * comm.all_sum_all(uv_data.size()));
#elif PURIFY_PADMM_ALGORITHM == 3 || PURIFY_PADMM_ALGORITHM == 1
  auto const epsilon = 3 * std::sqrt(2 * uv_data.size()) * sigma;
#endif
  auto const gamma = comm.all_reduce<t_real>(padmm->gamma(), MPI_MAX);
  PURIFY_MEDIUM_LOG("Epsilon {}", epsilon);
  PURIFY_MEDIUM_LOG("Gamma {}", gamma);
  padmm->gamma(gamma)
#if PURIFY_PADMM_ALGORITHM == 2
      .l2ball_proximal_epsilon(epsilon)
#endif
#if PURIFY_PADMM_ALGORITHM == 2
      // communicator ensuring l2 norm in l2ball proximal is global
      .l2ball_proximal_communicator(comm)
#endif
      // communicator ensuring l1 norm in l1 proximal is global
      .l1_proximal_adjoint_space_comm(comm);
  sopt::ScalarRelativeVariation<t_scalar> conv(padmm->relative_variation(),
                                                padmm->relative_variation(), "Objective function");
  std::weak_ptr<typename decltype(padmm)::element_type> const padmm_weak(padmm);
  padmm->residual_convergence([padmm_weak, conv,
                               comm](Vector<t_scalar> const &x,
                                     Vector<t_scalar> const &residual) mutable -> bool {
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

  padmm->objective_convergence([padmm_weak, conv, comm](Vector<t_scalar> const &x,
                                                        Vector<t_scalar> const &) mutable -> bool {
    auto const padmm = padmm_weak.lock();
#if PURIFY_PADMM_ALGORITHM == 2
    return conv(sopt::mpi::l1_norm(padmm->Psi().adjoint() * x, padmm->l1_proximal_weights(), comm));
#elif PURIFY_PADMM_ALGORITHM == 3 || PURIFY_PADMM_ALGORITHM == 1
    return comm.all_reduce<uint8_t>(
        conv(sopt::l1_norm(padmm->Psi().adjoint() * x, padmm->l1_proximal_weights())), MPI_LAND);
#endif
  });

  auto convergence_function = [](const Vector<t_scalar> &x) { return true; };
  const std::shared_ptr<t_uint> iter = std::make_shared<t_uint>(0);
  const auto algo_update
      = [uv_data, imsizex, imsizey, padmm_weak, iter, comm](const Vector<t_scalar> &x) -> bool {
    auto padmm = padmm_weak.lock();
    if(comm.is_root())
      PURIFY_MEDIUM_LOG("Step size γ {}", padmm->gamma());
    *iter = *iter + 1;
    Vector<t_complex> const alpha = padmm->Psi().adjoint() * x;
    const t_real new_gamma = comm.all_reduce(alpha.real().cwiseAbs().maxCoeff(), MPI_MAX) * 1e-3;
    if(comm.is_root())
      PURIFY_MEDIUM_LOG("Step size γ update {}", new_gamma);
    padmm->gamma(((std::abs(padmm->gamma() - new_gamma) > 0.2) and *iter < 200) ? new_gamma :
                                                                                  padmm->gamma());
    // updating parameter

    Vector<t_scalar> const residual = padmm->Phi().adjoint() * (uv_data.vis - padmm->Phi() * x);

    if(comm.is_root()) {
      pfitsio::write2d(x, imsizey, imsizex, "mpi_solution_update.fits");
      pfitsio::write2d(residual, imsizey, imsizex, "mpi_residual_update.fits");
    }
    return true;
  };
  auto lambda = [convergence_function, algo_update](Vector<t_scalar> const &x) {
    return convergence_function(x) and algo_update(x);
  };
  padmm->is_converged(lambda);
  return padmm;
}

}
}
#endif
