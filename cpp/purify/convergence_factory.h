#ifndef CONVERGENCE_FACTORY_H
#define CONVERGENCE_FACTORY_H

#include <sopt/imaging_padmm.h>
#ifdef PURIFY_MPI
#include <sopt/mpi/communicator.h>
#endif
#include <sopt/relative_variation.h>

namespace purify {
namespace factory {
enum class ConvergenceType { mpi_local, mpi_global };

#ifdef PURIFY_MPI

template <class T, class Algo>
std::function<bool(Vector<T> const &, Vector<T> const &)> l2_convergence_factory(
    const ConvergenceType algo, std::weak_ptr<Algo> const algo_weak) {
  const auto comm = sopt::mpi::Communicator::World();
  switch (algo) {
  case (ConvergenceType::mpi_local): {
    return [algo, algo_weak, comm](Vector<T> const &, Vector<T> const &residual) {
      auto const algo = algo_weak.lock();
      auto const residual_norm = sopt::l2_norm(residual, algo->l2ball_proximal_weights());
      SOPT_LOW_LOG("    - [Algorithm] Residuals: {} <? {}", residual_norm,
                   algo->residual_tolerance());
      return static_cast<bool>(
          comm.all_reduce<int8_t>(residual_norm < algo->residual_tolerance(), MPI_LAND));
    };
    break;
  }
  case (ConvergenceType::mpi_global): {
    return [algo, algo_weak, comm](Vector<T> const &, Vector<T> const &residual) {
      auto const algo = algo_weak.lock();
      auto const residual_norm =
          sopt::mpi::l2_norm(residual, algo->l2ball_proximal_weights(), comm);
      SOPT_LOW_LOG("    - [Algorithm] Residuals: {} <? {}", residual_norm,
                   algo->residual_tolerance());
      return static_cast<bool>(residual_norm < algo->residual_tolerance());
    };
    break;
  }
  default:
    throw std::runtime_error("Unknown type of distributed MPI convergence algorithm.");
  }
}

template <class T, class Algo>
std::function<bool(Vector<T> const &, Vector<T> const &)> l1_convergence_factory(
    const ConvergenceType algo, std::weak_ptr<Algo> const algo_weak) {
  auto const comm = sopt::mpi::Communicator::World();
  auto const algo_temp = algo_weak.lock();
  const std::shared_ptr<sopt::ScalarRelativeVariation<T>> conv =
      std::make_shared<sopt::ScalarRelativeVariation<T>>(algo_temp->relative_variation(), 0,
                                                         "Objective function");
  switch (algo) {
  case (ConvergenceType::mpi_local): {
    return [algo_weak, comm, conv](Vector<T> const &x, Vector<T> const &) {
      auto const algo = algo_weak.lock();
      return comm.all_reduce<uint8_t>((*conv)(sopt::l1_norm(algo->Psi().adjoint() * x)), MPI_LAND);
    };
    break;
  }
  case (ConvergenceType::mpi_global): {
    return [algo_weak, comm, conv](Vector<T> const &x, Vector<T> const &) {
      auto const algo = algo_weak.lock();
      return (*conv)(sopt::mpi::l1_norm(algo->Psi().adjoint() * x, comm));
    };
    break;
  }
  default:
    throw std::runtime_error("Unknown type of distributed MPI convergence algorithm.");
  }
}
#endif
}  // namespace factory
}  // namespace purify
#endif
