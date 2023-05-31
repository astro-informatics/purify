#ifndef UPDATE_FACTORY_H
#define UPDATE_FACTORY_H
#include "purify/config.h"

#include "purify/logging.h"

#ifdef PURIFY_MPI
#include <sopt/mpi/communicator.h>
#endif

#include "purify/algorithm_factory.h"
#include "purify/pfitsio.h"

#include "purify/cimg.h"

namespace purify {
namespace factory {

template <class T, class Algo>
void add_updater(std::weak_ptr<Algo> const algo_weak, const t_real step_size_scale,
                 const t_real update_tol, const t_uint update_iters,
                 const pfitsio::header_params &update_solution_header,
                 const pfitsio::header_params &update_residual_header, const t_uint imsizey,
                 const t_uint imsizex, const t_uint sara_size, const bool using_mpi,
                 const t_real beam_units = 1) {
  if (update_tol < 0)
    throw std::runtime_error("Step size update tolerance must be greater than zero.");
  if (step_size_scale < 0)
    throw std::runtime_error("Step size update scale must be greater than zero.");
#ifdef PURIFY_CImg
  auto const canvas =
      std::make_shared<CDisplay>(cimg::make_display(Vector<t_real>::Zero(1024 * 512), 1024, 512));
#endif
  const std::shared_ptr<pfitsio::header_params> update_header_sol =
      std::make_shared<pfitsio::header_params>(update_solution_header);
  const std::shared_ptr<pfitsio::header_params> update_header_res =
      std::make_shared<pfitsio::header_params>(update_residual_header);
  if (using_mpi) {
#ifdef PURIFY_MPI
    const auto comm = sopt::mpi::Communicator::World();
    const std::shared_ptr<t_int> iter = std::make_shared<t_int>(0);
    const auto updater = [update_tol, update_iters, imsizex, imsizey, algo_weak, iter,
                          step_size_scale, update_header_sol, update_header_res, sara_size, comm,
                          beam_units](const Vector<T> &x, const Vector<T> &res) -> bool {
      auto algo = algo_weak.lock();
      if (comm.is_root()) PURIFY_MEDIUM_LOG("Step size γ {}", algo->gamma());
      if (algo->gamma() > 0) {
        Vector<t_complex> const alpha = algo->g_proximal()->Psi().adjoint() * x;
        const t_real new_gamma =
            comm.all_reduce((sara_size > 0) ? alpha.real().cwiseAbs().maxCoeff() : 0., MPI_MAX) *
            step_size_scale;
        if (comm.is_root()) PURIFY_MEDIUM_LOG("Step size γ update {}", new_gamma);
        // updating parameter
        algo->gamma(((std::abs(algo->gamma() - new_gamma) > update_tol) and *iter < update_iters)
                        ? new_gamma
                        : algo->gamma());
      }
      Vector<t_complex> const residual = algo->Phi().adjoint() * (res / beam_units).eval();
      PURIFY_MEDIUM_LOG("RMS of residual map in Jy/beam {}",
                        residual.norm() / std::sqrt(residual.size()));
      if (comm.is_root()) {
        update_header_sol->niters = *iter;
        update_header_res->niters = *iter;
        pfitsio::write2d(Image<T>::Map(x.data(), imsizey, imsizex).real(), *update_header_sol,
                         true);
        pfitsio::write2d(Image<T>::Map(residual.data(), imsizey, imsizex).real(),
                         *update_header_res, true);
      }
      *iter = *iter + 1;
      return true;
    };
    auto algo = algo_weak.lock();
    algo->is_converged(updater);

#else
    throw std::runtime_error(
        "Trying to use algorithm step size update with MPI, but you did not compile with MPI.");
#endif
  } else {
    const std::shared_ptr<t_int> iter = std::make_shared<t_int>(0);
    const auto updater = [update_tol, update_iters, imsizex, imsizey, algo_weak, iter,
                          step_size_scale, update_header_sol, update_header_res, beam_units
#ifdef PURIFY_CImg
                          ,
                          canvas
#endif
    ](const Vector<T> &x, const Vector<T> &res) -> bool {
      auto algo = algo_weak.lock();
      if (algo->gamma() > 0) {
        PURIFY_MEDIUM_LOG("Step size γ {}", algo->gamma());
        Vector<T> const alpha = algo->g_proximal()->Psi().adjoint() * x;
        const t_real new_gamma = alpha.real().cwiseAbs().maxCoeff() * step_size_scale;
        PURIFY_MEDIUM_LOG("Step size γ update {}", new_gamma);
        // updating parameter
        algo->gamma(((std::abs((algo->gamma() - new_gamma) / algo->gamma()) > update_tol) and
                     *iter < update_iters)
                        ? new_gamma
                        : algo->gamma());
      }
      Vector<t_complex> const residual = algo->Phi().adjoint() * (res / beam_units).eval();
      PURIFY_MEDIUM_LOG("RMS of residual map in Jy/beam {}",
                        residual.norm() / std::sqrt(residual.size()));
#ifdef PURIFY_CImg
      const auto img1 = cimg::make_image(x.real().eval(), imsizey, imsizex)
                            .get_normalize(0, 1)
                            .get_resize(512, 512);
      const auto img2 = cimg::make_image(residual.real().eval(), imsizey, imsizex)
                            .get_normalize(0, 1)
                            .get_resize(512, 512);
      const auto results =
          CImageList<t_real>(img1.get_equalize(256, 0.05, 1.), img2.get_equalize(256, 0.1, 1.));
      canvas->display(results);
      canvas->resize(true);
#endif
      update_header_sol->niters = *iter;
      update_header_res->niters = *iter;
      pfitsio::write2d(Image<T>::Map(x.data(), imsizey, imsizex).real(), *update_header_sol, true);
      pfitsio::write2d(Image<T>::Map(residual.data(), imsizey, imsizex).real(), *update_header_res,
                       true);
      *iter = *iter + 1;
      return true;
    };
    auto algo = algo_weak.lock();
    algo->is_converged(updater);
  }
}
}  // namespace factory
}  // namespace purify
#endif
