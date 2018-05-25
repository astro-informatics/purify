 #ifndef CONVERGENCE_FACTORY_H
#define CONVERGENCE_FACTORY_H

#include <sopt/imaging_padmm.h>
#include <sopt/mpi/communicator.h>
#include <sopt/relative_variation.h>

namespace purify {
  namespace factory {
    enum class ConvergenceType {mpi_local, mpi_global};

    template <class T, class Algo>
      std::function<bool(Vector<T> const &, Vector<T> const &)>
      l2_convergence_factory(const ConvergenceType algo,	std::weak_ptr<Algo> const padmm_weak) {
	const auto comm = sopt::mpi::Communicator::World();
  switch(algo){
    case(ConvergenceType::mpi_local):
      {
      return [algo, padmm_weak, comm](Vector<T> const &, Vector<T> const & residual) {
	      auto const padmm = padmm_weak.lock();
	      auto const residual_norm = sopt::l2_norm(residual, padmm->l2ball_proximal_weights());
        SOPT_LOW_LOG("    - [PADMM] Residuals: {} <? {}", residual_norm, padmm->residual_tolerance());
	      return static_cast<bool>(comm.all_reduce<int8_t>(
              residual_norm < padmm->residual_tolerance(), MPI_LAND));
      };
      break;
    }
    case (ConvergenceType::mpi_global):
    {
      return [algo, padmm_weak, comm](Vector<T> const &, Vector<T> const & residual) {
	      auto const padmm = padmm_weak.lock();
	      auto const residual_norm = sopt::mpi::l2_norm(residual, padmm->l2ball_proximal_weights(), comm);
        SOPT_LOW_LOG("    - [PADMM] Residuals: {} <? {}", residual_norm, padmm->residual_tolerance());
	      return static_cast<bool>(residual_norm < padmm->residual_tolerance());
	    };
      break;
    }
    default:
	  throw std::runtime_error("Unknown type of distributed MPI convergence algorithm.");
    }
    }

    template <class T, class Algo>
      std::function<bool(Vector<T> const &, Vector<T> const &)>
      l1_convergence_factory(const ConvergenceType algo, std::weak_ptr<Algo> const padmm_weak) {
 	      auto const comm = sopt::mpi::Communicator::World();
	      auto const padmm_temp = padmm_weak.lock();
       const std::shared_ptr<sopt::ScalarRelativeVariation<T>> conv = std::make_shared<sopt::ScalarRelativeVariation<T>>(padmm_temp->relative_variation(),
						      padmm_temp->relative_variation(), "Objective function");
        switch(algo){
          case(ConvergenceType::mpi_local):
            {
            return [padmm_weak, comm, conv](Vector<T> const & x, Vector<T> const &) {
	            auto const padmm = padmm_weak.lock();
	            return comm.all_reduce<uint8_t>(
              (*conv)(sopt::l1_norm(padmm->Psi().adjoint() * x, padmm->l1_proximal_weights())), MPI_LAND);
              };
          break;
            }
          case(ConvergenceType::mpi_global):
            {
            return [padmm_weak, comm, conv](Vector<T> const & x, Vector<T> const &) {
	            auto const padmm = padmm_weak.lock();
	          return
              (*conv)(sopt::mpi::l1_norm(padmm->Psi().adjoint() * x, padmm->l1_proximal_weights(), comm));
              };
          break;
          }
          default:
	  throw std::runtime_error("Unknown type of distributed MPI convergence algorithm.");
        }
    }
  } // namespace factory
} // namespace purify

#endif
