#include "purify/MeasurementOperator.h"
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/mpi_utilities.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"
#include <array>
#include <boost/math/special_functions/erf.hpp>
#include <memory>
#include <random>
#include <sopt/imaging_padmm.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>

using namespace purify;
using namespace purify::notinstalled;

void padmm(const std::string &name, const Image<t_complex> &M31,
           const std::string &kernel, const t_int J,
           const utilities::vis_params &uv_data, const t_real sigma,
           const sopt::mpi::Communicator &world) {
  std::string const outfile = output_filename(name + "_" + kernel + ".tiff");
  std::string const outfile_fits =
      output_filename(name + "_" + kernel + "_solution.fits");
  std::string const residual_fits =
      output_filename(name + "_" + kernel + "_residual.fits");
  std::string const dirty_image =
      output_filename(name + "_" + kernel + "_dirty.tiff");
  std::string const dirty_image_fits =
      output_filename(name + "_" + kernel + "_dirty.fits");

  t_real const over_sample = 2;
  MeasurementOperator measurements(uv_data, J, J, kernel, M31.cols(),
                                   M31.rows(), 100, over_sample);
  auto measurements_transform =
      linear_transform(measurements, uv_data.vis.size(), world);

  sopt::wavelets::SARA const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u),
      std::make_tuple("DB2", 3u),   std::make_tuple("DB3", 3u),
      std::make_tuple("DB4", 3u),   std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u),
      std::make_tuple("DB8", 3u)};
  auto const start = [](t_uint size, t_uint ncomms, t_uint rank) {
    return std::min(size,
                    rank * (size / ncomms) + std::min(rank, size % ncomms));
  };
  auto const startw = start(sara.size(), world.size(), world.rank());
  auto const endw = start(sara.size(), world.size(), world.rank() + 1);
  auto const split_sara =
      sopt::wavelets::SARA(sara.begin() + startw, sara.begin() + endw);

  auto const Psi = sopt::linear_transform<t_complex>(
      split_sara, measurements.imsizey(), measurements.imsizex(), world);

  Vector<> dimage = (measurements_transform.adjoint() * uv_data.vis).real();
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  if (world.is_root())
    pfitsio::write2d(Image<t_real>::Map(dimage.data(), measurements.imsizey(),
                                        measurements.imsizex()),
                     dirty_image_fits);

  auto const epsilon =
      world.broadcast(utilities::calculate_l2_radius(uv_data.vis, sigma));
  auto const purify_gamma =
      world.is_root()
          ? world.broadcast((Psi.adjoint() * (measurements_transform.adjoint() * uv_data.vis))
                                .cwiseAbs()
                                .maxCoeff() *
                            1e-3)
          : world.broadcast<t_real>();
  PURIFY_MEDIUM_LOG("Epsilon {}", epsilon);
  PURIFY_MEDIUM_LOG("Gamma {}", purify_gamma);

  auto padmm =
      sopt::algorithm::ImagingProximalADMM<t_complex>(uv_data.vis)
          .itermax(100)
          .gamma(purify_gamma)
          .relative_variation(1e-3)
          .l2ball_proximal_epsilon(epsilon)
          .l2ball_proximal(
              sopt::proximal::WeightedL2Ball<t_complex>(epsilon).communicator(
                  world))
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
  sopt::ScalarRelativeVariation<t_complex> conv(padmm.relative_variation(),
                                                padmm.relative_variation(),
                                                "Objective function");
  padmm.objective_convergence(
      [&padmm, conv, world](Vector<t_complex> const &,
              Vector<t_complex> const &residual) mutable -> bool {
        auto const result = conv(sopt::mpi::l1_norm(
            residual + padmm.target(), padmm.l1_proximal_weights(), world));
        return result;
      });

  auto const diagnostic = padmm();
  assert(diagnostic.x.size() == M31.size());
  assert(world.broadcast(diagnostic.x).isApprox(diagnostic.x));
  Image<t_complex> image = Image<t_complex>::Map(
      diagnostic.x.data(), measurements.imsizey(), measurements.imsizex());
  // Image<t_complex> residual = measurements_transform.adjoint() *              // FIXME
  //                             (uv_data.vis - measurements_transform * image); // fails with to do with dimensions?
  if (world.is_root()) {
    pfitsio::write2d(image.real(), outfile_fits);
    // pfitsio::write2d(residual.real(), residual_fits);
  }
};

int main(int nargs, char const **args) {
  sopt::logging::initialize();
  purify::logging::initialize();
  sopt::logging::set_level("debug");
  purify::logging::set_level("debug");
  //sopt::mpi::init(nargs, args); // FIXME - What should go here?

  auto const world = sopt::mpi::Communicator::World();

  const std::string &name = "M31"; // image to simulate
  const t_real snr = 30;           // snr of reconstruction
  std::string const fitsfile = image_filename(
      name + ".fits"); // string of fits file of image to reconstruct
  auto M31 = pfitsio::read2d(fitsfile); // data of ground truth, to reconstruct
  std::string const inputfile = output_filename(
      name + "_" + "input.fits"); // name to save ground truth image as

  t_real const max = M31.array().abs().maxCoeff();
  M31 = M31 * 1. / max; // normalise the ground truth
  pfitsio::write2d(
      M31.real(),
      inputfile); // save ground truth, to check it is the right image

  // determine amount of visibilities to simulate
  t_int const number_of_pixels = M31.size();
  t_int const number_of_vis = std::floor(number_of_pixels * 2.);
  // Generating random uv(w) coverage
  t_real const sigma_m = constant::pi / 3; // width of sampling pattern
  utilities::vis_params uv_data; // FIXME it was uv_params, changed to vis_params
  t_real sigma = sigma_m; // FIXME was not defined and padmm cannnot be called
  if (world.is_root()) {
    // generate random sampling pattern
    uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m);
    uv_data.units = "radians";
    std::cout << "Number of measurements / number of pixels: "
              << uv_data.u.size() * number_of_pixels << '\n';
    MeasurementOperator sky_measurements(
        uv_data, 8, 8, "kb", M31.cols(), M31.rows(),
        100); // creating operator to generate measurements
    uv_data.vis =
        sky_measurements.degrid(M31); // Generates measurements from image

    Vector<t_complex> const y0 = uv_data.vis;
    // working out value of signal given SNR of 30
    sigma = utilities::SNR_to_standard_deviation(y0, snr);
    // adding noise to visibilities
    uv_data.vis = utilities::add_noise(y0, 0., sigma);
  };
  padmm(name + "30", M31, "kb", 4, distribute_params(uv_data, world), sigma,
        world);
  return 0;
}
