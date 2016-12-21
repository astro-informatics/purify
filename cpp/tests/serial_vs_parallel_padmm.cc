#include <catch.hpp>
#include <numeric>
#include <random>
#include <utility>

#include <sopt/imaging_padmm.h>
#include <sopt/logging.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/utilities.h>
#include <sopt/wavelets.h>
#include "purify/MeasurementOperator.h"
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/mpi_utilities.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"


TEST_CASE("Parallel vs serial inpainting") {
  extern std::unique_ptr<std::mt19937_64> mersenne;
  using namespace sopt;
  auto const world = mpi::Communicator::World();
  // split into serial and parallel
  auto const split_comm = world.split(world.is_root());
  if(world.size() < 2){
    std::cout << "Number of worlds: " << world.size() << std::endl;
    return;
  }

  using namespace purify;
  using namespace purify::notinstalled;
  std::string const kernel = "kb";
  t_real const over_sample = 2;
  t_int const J = 1;
  t_real const m_over_n = 2;
  t_real const ISNR = 30;

  std::string const fitsfile = image_filename("30dor_256.fits");

  PURIFY_MEDIUM_LOG("Starting Purify");
  
  auto sky_model = world.is_root() ? world.broadcast(pfitsio::read2d(fitsfile)) :
                                     world.broadcast<sopt::Image<t_complex>>();
  auto sky_model_max = sky_model.array().abs().maxCoeff();
  sky_model = sky_model / sky_model_max;
  t_int const number_of_vis = std::floor(m_over_n * sky_model.size());
  t_real const sigma_m = constant::pi / 3;

  auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m);
  uv_data.units = "radians";
  PURIFY_MEDIUM_LOG("Number of measurements: {}", uv_data.u.size());

  MeasurementOperator sky_measurements(uv_data, 4, 4, "kb", sky_model.cols(), sky_model.rows(), 100,
                                       over_sample);
  // working out value of sigma given SNR of 30
  auto const sigma = world.broadcast(utilities::SNR_to_standard_deviation(uv_data.vis, ISNR));
  // adding noise to visibilities
  uv_data.vis = sky_measurements.degrid(sky_model);
  uv_data.vis = utilities::add_noise(uv_data.vis, 0., sigma);
  uv_data.u = world.broadcast(uv_data.u);
  uv_data.v = world.broadcast(uv_data.v);
  uv_data.w = world.broadcast(uv_data.w);
  uv_data.weights = world.broadcast(uv_data.weights);
  uv_data.vis = world.broadcast(uv_data.vis);
  if(split_comm.size() > 1 and split_comm.is_root()) {
    auto const order = distribute::distribute_measurements(uv_data, split_comm);
    uv_data = utilities::regroup_and_scatter(uv_data, order, split_comm);
  } else if(split_comm.size() > 1)
    uv_data = utilities::scatter_visibilities(split_comm);

  MeasurementOperator measurements(uv_data, J, J, kernel, sky_model.cols(), sky_model.rows(), 100,
                                   over_sample);
  auto measurements_transform = linear_transform(measurements, uv_data.vis.size());

  auto const wavelets = {std::make_tuple("DB4", 3u)};
  sopt::wavelets::SARA const sara(wavelets.begin(), wavelets.end());
  auto const Psi
      = sopt::linear_transform<t_complex>(sara, measurements.imsizey(), measurements.imsizex());

  sopt::Vector<> dimage = world.broadcast((measurements_transform.adjoint() * uv_data.vis).real());
  t_real const max_val = dimage.array().abs().maxCoeff();
  dimage = dimage / max_val;
  sopt::Vector<t_complex> initial_estimate = sopt::Vector<t_complex>::Zero(dimage.size());
  // pfitsio::write2d(Image<t_real>::Map(dimage.data(), measurements.imsizey(),
  // measurements.imsizex()), dirty_image_fits);

  auto const epsilon = world.broadcast(utilities::calculate_l2_radius(uv_data.vis, sigma));
  auto const purify_gamma = world.broadcast(
      (Psi.adjoint() * (measurements_transform.adjoint() * uv_data.vis)).real().maxCoeff() * 1e-3);
  PURIFY_HIGH_LOG("Starting sopt!");
  PURIFY_MEDIUM_LOG("Epsilon {}", epsilon);
  PURIFY_MEDIUM_LOG("Gamma {}", purify_gamma);
  auto padmm
      = sopt::algorithm::ImagingProximalADMM<t_complex>(uv_data.vis)
            .gamma(purify_gamma)
            .relative_variation(1e-3)
            .l2ball_proximal(
                sopt::proximal::WeightedL2Ball<sopt::t_complex>(epsilon).communicator(split_comm))
            .tight_frame(false)
            .l1_proximal_tolerance(1e-2)
            .l1_proximal_nu(1)
            .l1_proximal_itermax(50)
            .l1_proximal_positivity_constraint(true)
            .l1_proximal_real_constraint(true)
            .lagrange_update_scale(0.9)
            .nu(1e0)
            .Psi(Psi)
            .itermax(5);
  LinearTransform<sopt::Vector<t_complex>> const parallel_phi(
      [&measurements_transform](sopt::Vector<t_complex> &out,
                                sopt::Vector<t_complex> const &input) {
        out = measurements_transform * input;
      },
      measurements_transform.sizes(),
      [&measurements_transform, split_comm](sopt::Vector<t_complex> &out,
                                            sopt::Vector<t_complex> const &input) {
        out = measurements_transform.adjoint() * input;
        split_comm.all_sum_all(out);
      },
      measurements_transform.adjoint().sizes());

  padmm.Phi(parallel_phi);
  padmm.residual_convergence(
      [&padmm, epsilon, split_comm, world](sopt::Vector<t_complex> const &residual) {
        auto const tolerance = epsilon * 1.001;
        auto const residual_norm
            = sopt::mpi::l2_norm(residual, padmm.l2ball_proximal_weights(), split_comm);
        SOPT_LOW_LOG("    - Residuals: epsilon = {}, residual norm = {}", tolerance, residual_norm);
        auto const result = residual_norm < tolerance;
        CHECK(result == (world.broadcast<int>(result, world.root_id()) != 0));
        return result;
      });
  sopt::algorithm::ImagingProximalADMM<t_complex>::ObjectiveConvergence conv;
  padmm.objective_convergence([&padmm, conv, split_comm,
                               world](sopt::Vector<t_complex> const &residual) mutable -> bool {
    auto const result = conv(padmm, sopt::mpi::l1_norm(residual + padmm.target(),
                                                       padmm.l1_proximal_weights(), split_comm));
    CHECK(result == (world.broadcast<int>(result, world.root_id()) != 0));
    return result;
  });
  padmm.is_converged([world](sopt::Vector<t_complex> const &image) -> bool {
    auto const from_root = world.broadcast(image);
    CHECK(from_root.isApprox(image, 1e-12));
    return true;
  });

  auto const diagnostic = padmm();
  CHECK(diagnostic.good == (world.broadcast<int>(diagnostic.good, world.root_id()) != 0));
  CHECK(diagnostic.x.isApprox(world.broadcast(diagnostic.x)));
}
