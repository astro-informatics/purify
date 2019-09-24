#include "purify/types.h"
#include <array>
#include <memory>
#include <random>
#include <boost/math/special_functions/erf.hpp>
#include "purify/cimg.h"
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/pfitsio.h"
#include "purify/spherical_resample.h"
#include "purify/utilities.h"
#include <sopt/credible_region.h>
#include <sopt/imaging_padmm.h>
#include <sopt/power_method.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>

#include "purify/algorithm_factory.h"
#include "purify/wavelet_operator_factory.h"
using namespace purify;
using namespace purify::notinstalled;

utilities::vis_params dirty_visibilities(const utilities::vis_params &uv_data) {
  const std::string &pos_filename = mwa_filename("../chime/chime_config.txt");
  const auto times = std::vector<t_real>{0};
  auto uv_data_root =
      utilities::read_ant_positions_to_coverage(pos_filename, 400e6, times, 0., 0., 0.);
  uv_data_root.units = utilities::vis_units::lambda;
  return uv_data_root;
}
utilities::vis_params dirty_visibilities(const utilities::vis_params &uv_data,
                                         sopt::mpi::Communicator const &comm) {
  if (comm.size() == 1) return dirty_visibilities(uv_data);
  if (comm.is_root()) {
    auto result = dirty_visibilities(uv_data);
    //  auto const order = distribute::distribute_measurements(result, comm,
    //  distribute::plan::uv_stack);
    auto const order = distribute::uv_distribution(result.u, result.v, comm.size());
    // should use conjugate symmetry to make v >= 0 when stacking.
    return utilities::regroup_and_scatter(result, order, comm);
  }
  auto result = utilities::scatter_visibilities(comm);
  return result;
}

int main(int nargs, char const **args) {
  sopt::logging::initialize();
  purify::logging::initialize();
  sopt::logging::set_level("debug");
  purify::logging::set_level("debug");
  auto const session = sopt::mpi::init(nargs, args);
  auto const comm = sopt::mpi::Communicator::World();

  const std::string &name = "allsky400MHz";
  const t_real L = 1.9;
  const t_real max_w = 0.;  // lambda
  const t_real snr = 30;
  std::string const fitsfile = image_filename(name + ".fits");
  const auto all_sky_image = pfitsio::read2d(fitsfile);
  std::string const inputfile = output_filename(name + "_" + "input.fits");
  std::string const dirtyfile = output_filename(name + "_" + "dirty.fits");
  std::string const outfile_fits = output_filename(name + "_" + "solution.fits");
  std::string const residual_fits = output_filename(name + "_" + "residual.fits");
  if (comm.is_root()) pfitsio::write2d(all_sky_image.real(), inputfile);
  const t_real theta_0 =
      0. * constant::pi / 180. + comm.rank() * constant::pi / 180. * comm.size() * 2;
  const t_real phi_0 = 90. * constant::pi / 180.;

  t_int const number_of_pxiels = all_sky_image.size();
  // Generating random uv(w) coverage
  t_real const sigma_m = 500. / 4. / 3. / L;
  t_uint const imsizey = all_sky_image.rows();
  t_uint const imsizex = all_sky_image.cols();
  const t_int num_theta = all_sky_image.rows();
  const t_int num_phi = all_sky_image.cols();

  const t_int number_of_samples = num_theta * num_phi;
  const t_int Jl = 4;
  const t_int Jm = 4;
  const t_int Ju = 4;
  const t_int Jv = 4;
  const t_int Jw = 256;
  const t_real oversample_ratio_image_domain = 1;
  const t_real oversample_ratio = 1.5;
  const bool uvw_stacking = true;
  const kernels::kernel kernel = kernels::kernel::kb;
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;

  const auto phi = [num_phi, num_theta](const t_int k) -> t_real {
    return utilities::ind2row(k, num_phi, num_theta) * constant::pi / num_phi;
  };
  const auto theta = [num_theta, num_phi](const t_int k) -> t_real {
    return utilities::ind2col(k, num_phi, num_theta) * 2 * constant::pi / num_theta;
  };

  t_real sigma = 0.;
  utilities::vis_params uv_data;
  // if (comm.is_root()) {
  {
    uv_data = dirty_visibilities(uv_data);
    std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> const sky_measurements =
        std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
            spherical_resample::measurement_operator::planar_degrid_operator<
                Vector<t_complex>, std::function<t_real(t_int)>>(
                comm, number_of_samples, theta_0, phi_0, theta, phi, uv_data, oversample_ratio,
                oversample_ratio_image_domain, kernel, Ju, Jv, Jl, Jm, ft_plan, uvw_stacking, L, L,
                0., 0.),
            1000, 1e-3, Vector<t_complex>::Random(imsizex * imsizey).eval()));
    uv_data.vis =
        (*sky_measurements) * Image<t_complex>::Map(all_sky_image.data(), all_sky_image.size(), 1);
    // adding noise to visibilities
    sigma = utilities::SNR_to_standard_deviation(uv_data.vis, snr);
    uv_data.vis = utilities::add_noise(uv_data.vis, 0., sigma);
  }
  // uv_data = dirty_visibilities(uv_data, comm);

  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> const measurements_transform =
      std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
          spherical_resample::measurement_operator::planar_degrid_operator<
              Vector<t_complex>, std::function<t_real(t_int)>>(
              comm, number_of_samples, theta_0, phi_0, theta, phi, uv_data, oversample_ratio,
              oversample_ratio_image_domain, kernel, Ju, Jv, Jl, Jm, ft_plan, uvw_stacking, L, L,
              0., 0.),
          1000, 1e-3,
          comm.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(imsizex * imsizey).eval())));
  Vector<t_complex> dmap = measurements_transform->adjoint() * uv_data.vis;
  Image<t_complex> dmap_image = Image<t_complex>::Map(dmap.data(), imsizey, imsizex);
  if (comm.is_root()) pfitsio::write2d(dmap_image.real(), dirtyfile);
  // wavelet transform
  t_uint sara_size = 0.;
  std::vector<std::tuple<std::string, t_uint>> const sara{std::make_tuple("dirac", 1u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::mpi_sara, sara, imsizey, imsizex, sara_size);

  auto const primaldual =
      factory::primaldual_factory<sopt::algorithm::ImagingPrimalDual<t_complex>>(
          factory::algo_distribution::mpi_serial, measurements_transform, wavelets, uv_data, sigma,
          imsizey, imsizex, sara_size, 500);
  auto const diagnostic = (*primaldual)();

  assert(diagnostic.x.size() == all_sky_image.size());
  Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  if (comm.is_root()) pfitsio::write2d(image.real(), outfile_fits);
  Vector<t_complex> residuals = measurements_transform->adjoint() *
                                (uv_data.vis - ((*measurements_transform) * diagnostic.x));
  Image<t_complex> residual_image = Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
  if (comm.is_root()) pfitsio::write2d(residual_image.real(), residual_fits);

  return 0;
}
