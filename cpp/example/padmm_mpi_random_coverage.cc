#include <array>
#include <memory>
#include <random>
#include <boost/filesystem.hpp>
#include <boost/math/special_functions/erf.hpp>
#include <sopt/imaging_padmm.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include "purify/MeasurementOperator.h"
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/mpi_utilities.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"

using namespace purify;
using namespace purify::notinstalled;

std::tuple<utilities::vis_params, t_real>
dirty_visibilities(Image<t_complex> const &ground_truth_image, t_uint number_of_vis, t_real snr) {
  auto uv_data = utilities::random_sample_density(number_of_vis, 0, constant::pi / 3);
  uv_data.units = "radians";
  PURIFY_HIGH_LOG("Number of measurements / number of pixels: {}",
                  uv_data.u.size() * ground_truth_image.size());
  // creating operator to generate measurements
  MeasurementOperator sky_measurements(uv_data, 8, 8, "kb", ground_truth_image.cols(),
                                       ground_truth_image.rows(), 100);
  // Generates measurements from image
  uv_data.vis = sky_measurements.degrid(ground_truth_image);

  // working out value of signal given SNR of 30
  auto const sigma = utilities::SNR_to_standard_deviation(uv_data.vis, snr);
  // adding noise to visibilities
  uv_data.vis = utilities::add_noise(uv_data.vis, 0., sigma);
  return {uv_data, sigma};
}

std::tuple<utilities::vis_params, t_real>
dirty_visibilities(Image<t_complex> const &ground_truth_image, t_uint number_of_vis, t_real snr,
                   sopt::mpi::Communicator const &comm) {
  if(comm.size() == 1)
    return dirty_visibilities(ground_truth_image, number_of_vis, snr);
  if(comm.is_root()) {
    auto result = dirty_visibilities(ground_truth_image, number_of_vis, snr);
    comm.broadcast(std::get<1>(result));
    auto const order
        = distribute::distribute_measurements(std::get<0>(result), comm, "distance_distribution");
    std::get<0>(result) = utilities::regroup_and_scatter(std::get<0>(result), order, comm);
    return result;
  }
  auto const sigma = comm.broadcast<t_real>();
  return {utilities::scatter_visibilities(comm), sigma};
}

std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>>
padmm_factory(MeasurementOperator const &measurements, sopt::wavelets::SARA const &sara,
              const Image<t_complex> &ground_truth_image, const utilities::vis_params &uv_data,
              const t_real sigma, const sopt::mpi::Communicator &world) {

  auto measurements_transform = linear_transform(measurements, uv_data.vis.size(), world);
  auto const Psi = sopt::linear_transform<t_complex>(sara, measurements.imsizey(),
                                                     measurements.imsizex(), world);

  auto const epsilon = world.broadcast(utilities::calculate_l2_radius(uv_data.vis, sigma));
  auto const purify_gamma
      = (Psi.adjoint() * (measurements_transform.adjoint() * uv_data.vis)).cwiseAbs().maxCoeff()
        * 1e-3;
  PURIFY_MEDIUM_LOG("Epsilon {}", epsilon);
  PURIFY_MEDIUM_LOG("Gamma {}", purify_gamma);

  // shared pointer because the convergence function need access to some data that we would rather
  // not reproduce. E.g. padmm definition is self-referential.
  auto padmm = std::make_shared<sopt::algorithm::ImagingProximalADMM<t_complex>>(uv_data.vis);
  padmm->itermax(2)
      .gamma(purify_gamma)
      .relative_variation(1e-3)
      .l2ball_proximal_epsilon(epsilon)
      .l2ball_proximal(sopt::proximal::WeightedL2Ball<t_complex>(epsilon).communicator(world))
      .tight_frame(false)
      .l1_proximal_tolerance(1e-2)
      .l1_proximal_nu(1)
      .l1_proximal_itermax(50)
      .l1_proximal_positivity_constraint(true)
      .l1_proximal_real_constraint(true)
      .residual_convergence(epsilon)
      .lagrange_update_scale(0.9)
      .nu(1e0)
      .Psi(Psi)
      .Phi(measurements_transform);
  sopt::ScalarRelativeVariation<t_complex> conv(padmm->relative_variation(),
                                                padmm->relative_variation(), "Objective function");
  padmm->objective_convergence([padmm, conv,
                                world](Vector<t_complex> const &,
                                       Vector<t_complex> const &residual) mutable -> bool {
    auto const result
        = conv(sopt::mpi::l1_norm(residual + padmm->target(), padmm->l1_proximal_weights(), world));
    return result;
  });

  return padmm;
};

int main(int nargs, char const **args) {
  sopt::logging::initialize();
  purify::logging::initialize();
  sopt::logging::set_level("debug");
  purify::logging::set_level("debug");
  auto const session = sopt::mpi::init(nargs, args);
  auto const world = sopt::mpi::Communicator::World();

  const std::string name = "M31";
  const t_real snr = 30;
  auto const kernel = "kb";

  // string of fits file of image to reconstruct
  auto ground_truth_image = pfitsio::read2d(image_filename(name + ".fits"));
  ground_truth_image /= ground_truth_image.array().abs().maxCoeff();

  // determine amount of visibilities to simulate
  t_int const number_of_pixels = ground_truth_image.size();
  t_int const number_of_vis = std::floor(number_of_pixels * 0.5);

  // Generating random uv(w) coverage
  auto const data = dirty_visibilities(ground_truth_image, number_of_vis, snr, world);
  MeasurementOperator const measurements(std::get<0>(data), 4, 4, kernel, ground_truth_image.cols(),
                                         ground_truth_image.rows(), 100, 2);
  auto const sara = sopt::wavelets::distribute_sara(
      sopt::wavelets::SARA{
          std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
          std::make_tuple("DB3", 3u), std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
          std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)},
      world);
  PURIFY_HIGH_LOG("HERE");

  // Create the padmm solver
  auto const padmm = padmm_factory(measurements, sara, ground_truth_image, std::get<0>(data),
                                   std::get<1>(data), world);

  PURIFY_HIGH_LOG("THERE");
  // calls padmm
  auto const diagnostic = (*padmm)();

  // makes sure we set things up correctly
  assert(diagnostic.x.size() == ground_truth_image.size());
  assert(world.broadcast(diagnostic.x).isApprox(diagnostic.x));

  // then writes stuff to files
  auto const residual_image
      = world.all_sum_all<Vector<t_real>>(measurements.grid(diagnostic.residual).real());
  auto const dirty_image
      = world.all_sum_all<Vector<t_real>>(measurements.grid(std::get<0>(data).vis).real());
  if(world.is_root()) {
    boost::filesystem::path const path(name);
    boost::filesystem::create_directories(path / kernel);
    pfitsio::write2d(ground_truth_image.real(), output_filename((path / "input.fits").native()));
    pfitsio::write2d(dirty_image, ground_truth_image.rows(), ground_truth_image.cols(),
                     output_filename((path / "dirty.fits").native()));
    pfitsio::write2d(diagnostic.x.real(), ground_truth_image.rows(), ground_truth_image.cols(),
                     output_filename((path / kernel / "solution.fits").native()));
    pfitsio::write2d(residual_image, ground_truth_image.rows(), ground_truth_image.cols(),
                     output_filename((path / kernel / "residual.fits").native()));
  }
  return 0;
}
