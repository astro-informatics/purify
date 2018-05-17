 #ifndef CONVERGENCE_FACTORY_H
#define CONVERGENCE_FACTORY_H

#include <sopt/imaging_padmm.h>
#include <sopt/mpi/communicator.h>

namespace purify {
  namespace factory {

    template <class T>
      std::function<bool(Vector<T> const &, Vector<T> const &)>
      residual_convergence_factory(std::weak_ptr<sopt::algorithm::ImagingProximalADMM<T>> const padmm_weak) {
      
      return [padmm_weak](Vector<T> const &, Vector<T> const & residual) {
	auto const padmm = padmm_weak.lock();
	auto const residual_norm = sopt::l2_norm(residual, padmm->l2ball_proximal_weights());
	sopt::mpi::Communicator comm = sopt::mpi::Communicator::World();
	auto const result = comm.all_reduce<int8_t>(residual_norm < padmm->residual_tolerance(), MPI_LAND);
	return result;
      };
    }

    template <class T>
      std::function<bool(Vector<T> const &, Vector<T> const &)>
      objective_convergence_factory(std::weak_ptr<sopt::algorithm::ImagingProximalADMM<T>> const padmm_weak) {
      
      return [padmm_weak](Vector<T> const & x, Vector<T> const &) {
	auto const padmm = padmm_weak.lock();
 	sopt::mpi::Communicator comm = sopt::mpi::Communicator::World();
	sopt::ScalarRelativeVariation<t_complex> conv(padmm->relative_variation(),
						      padmm->relative_variation(), "Objective function");
	auto const result = comm.all_reduce<uint8_t>(
        conv(sopt::l1_norm(padmm->Psi().adjoint() * x, padmm->l1_proximal_weights())), MPI_LAND);
    return result;
      };
    }
    
  } // namespace factory
} // namespace purify

#endif
