#include "purify/config.h"

#include "purify/types.h"
#include <array>
#include <cstddef>
#include <ctime>
#include <random>
#include "purify/algorithm_factory.h"
#include "purify/cimg.h"
#include "purify/logging.h"
#include "purify/measurement_operator_factory.h"
#include "purify/pfitsio.h"
#include "purify/read_measurements.h"
#include "purify/update_factory.h"
#include "purify/wavelet_operator_factory.h"
#include "purify/yaml-parser.h"
#include <sopt/imaging_padmm.h>
#include <sopt/positive_quadrant.h>
#include <sopt/relative_variation.h>
#include <sopt/reweighted.h>
using namespace purify;

int main(int argc, const char **argv) {
  sopt::logging::initialize();
  purify::logging::initialize();

  // Read config file path from command line
  if (argc == 1) {
    PURIFY_HIGH_LOG("Specify the config file full path. Aborting.");
    return 1;
  }

  std::string file_path = argv[1];
  YamlParser params = YamlParser(file_path);

  factory::distributed_measurement_operator mop_algo =
      factory::distributed_measurement_operator::serial;
  factory::distributed_wavelet_operator wop_algo = factory::distributed_wavelet_operator::serial;
  bool using_mpi = false;

#ifdef PURIFY_MPI
  auto const session = sopt::mpi::init(argc, argv);
#endif

  if (params.mpiAlgorithm() != factory::algo_distribution::serial) {
#ifdef PURIFY_MPI
    auto const world = sopt::mpi::Communicator::World();
#else
    throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
    mop_algo = factory::distributed_measurement_operator::mpi_distribute_image;
    wop_algo = factory::distributed_wavelet_operator::mpi_sara;
    using_mpi = true;
  }

  sopt::logging::set_level(params.logging());
  purify::logging::set_level(params.logging());

  // Read or generate input data
  utilities::vis_params uv_data;
  t_real sigma;
  if (params.source() == purify::utilities::vis_source::measurements) {
    PURIFY_HIGH_LOG("Input visibilities are from files:");
    for (size_t i = 0; i < params.measurements().size(); i++)
      PURIFY_HIGH_LOG("{}", params.measurements()[i]);
    sigma = params.measurements_sigma();
    // TODO: use_w_term hardcoded to false for now
    uv_data = read_measurements::read_measurements(params.measurements(), false, stokes::I,
                                                   params.measurements_units());
  } else if (params.source() == purify::utilities::vis_source::simulation) {
    PURIFY_HIGH_LOG("Input visibilities will be generated for random coverage.");
    // TODO: move this to function (in utilities.h?)
    auto image = pfitsio::read2d(params.skymodel());
    if (params.height() != image.rows() || params.width() != image.cols())
      throw std::runtime_error("Input image size (" + std::to_string(image.cols()) + "x" +
                               std::to_string(image.rows()) + ") is not equal to the input one (" +
                               std::to_string(params.width()) + "x" +
                               std::to_string(params.height()) + ").");
    t_int const number_of_pixels = image.size();
    t_int const number_of_vis = std::floor(number_of_pixels * 0.2);
    t_real const sigma_m = constant::pi / 3;
    const t_real max_w = 100.;  // lambda
    uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m, max_w);
    uv_data.units = utilities::vis_units::radians;
    auto const sky_measurements = factory::measurement_operator_factory<Vector<t_complex>>(
        mop_algo, uv_data, params.height(), params.width(), params.cellsizey(), params.cellsizex(),
        params.oversampling(), params.powMethod_iter(), params.powMethod_tolerance(),
        kernels::kernel_from_string.at(params.kernel()), 2 * params.Jy(), 2 * params.Jx());
    uv_data.vis = (*sky_measurements) * Image<t_complex>::Map(image.data(), image.size(), 1);
    Vector<t_complex> const y0 = uv_data.vis;
    sigma = utilities::SNR_to_standard_deviation(y0, params.signal_to_noise());
    uv_data.vis = utilities::add_noise(y0, 0., sigma);
  }
  // need to scale data (uv_data.vis) by u and v cell size
  auto pixel_to_lambda = [](const t_real cell, const t_uint imsize, const t_real oversample_ratio) {
    return 1. / (2 * oversample_ratio *
                 std::sin(0.5 * cell * std::floor(imsize) * constant::pi / (60. * 60. * 180.)));
  };
  const t_real flux_scale =
      (pixel_to_lambda(params.cellsizex(), params.width(), params.oversampling()) *
       pixel_to_lambda(params.cellsizey(), params.height(), params.oversampling()));
  uv_data.vis = uv_data.vis.array() * uv_data.weights.array() / flux_scale /
                uv_data.weights.norm() * uv_data.weights.size();

  // create measurement operator
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> measurements_transform = (not params.wprojection())?
      factory::measurement_operator_factory<Vector<t_complex>>(
          mop_algo, uv_data, params.height(), params.width(), params.cellsizey(),
          params.cellsizex(), params.oversampling(), params.powMethod_iter(),
          params.powMethod_tolerance(), kernels::kernel_from_string.at(params.kernel()),
          params.Jy(), params.Jx(), params.mpi_wstacking() ):
      factory::measurement_operator_factory<Vector<t_complex>>(
          mop_algo, uv_data, params.height(), params.width(), params.cellsizey(),
          params.cellsizex(), params.oversampling(), params.powMethod_iter(),
          params.powMethod_tolerance(), kernels::kernel_from_string.at(params.kernel()),
          params.Jy(), params.Jw(), 1e-6, 1e-6);
  // create wavelet operator
  std::vector<std::tuple<std::string, t_uint>> sara;
  for (size_t i = 0; i < params.wavelet_basis().size(); i++)
    sara.push_back(std::make_tuple(params.wavelet_basis().at(i), params.wavelet_levels()));

  auto const wavelets_transform = factory::wavelet_operator_factory<Vector<t_complex>>(
      wop_algo, sara, params.height(), params.width());

  // Create algorithm
  auto algo = factory::algorithm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
      factory::algorithm::padmm, params.mpiAlgorithm(), measurements_transform, wavelets_transform,
      uv_data, sigma * params.epsilonScaling() / flux_scale, params.height(), params.width(),
      sara.size(), params.iterations(), params.realValueConstraint(),
      params.positiveValueConstraint(), (sara.size() < 2), params.relVarianceConvergence(), 1e-3,
      50);

  // Save some things before applying the algorithm
  // the config yaml file - this also generates the output directory and the timestamp
  params.writeOutput();
  const std::string out_dir = params.output_prefix() + "/output_" + params.timestamp();
  // Creating header for saving output images during iterations
  const pfitsio::header_params update_header_sol =
      pfitsio::header_params(out_dir + "/sol_update.fits", "Jy/Pixel", 1, uv_data.ra, uv_data.dec,
                             params.measurements_polarization(), params.cellsizex(),
                             params.cellsizey(), uv_data.average_frequency, 0, 0, false, 0, 0, 0);
  const pfitsio::header_params update_header_res =
      pfitsio::header_params(out_dir + "/res_update.fits", "Jy/Pixel", 1, uv_data.ra, uv_data.dec,
                             params.measurements_polarization(), params.cellsizex(),
                             params.cellsizey(), uv_data.average_frequency, 0, 0, false, 0, 0, 0);
  // TODO: allow for other algorithm types
  const std::weak_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>> algo_weak(algo);
  // Adding step size update to algorithm
  factory::add_updater<t_complex, sopt::algorithm::ImagingProximalADMM<t_complex>>(
      algo_weak, 1e-3, params.update_tolerance(), params.update_iters(), update_header_sol,
      update_header_res, params.height(), params.width(), using_mpi);
  // the input measurements, if simulated
  if (params.source() == purify::utilities::vis_source::simulation)
    utilities::write_visibility(uv_data, out_dir + "/input.vis");
  const pfitsio::header_params def_header = pfitsio::header_params(
      "", "Jy/Pixel", 1, uv_data.ra, uv_data.dec, params.measurements_polarization(),
      params.cellsizex(), params.cellsizey(), uv_data.average_frequency, 0, 0, false, 0, 0, 0);
  // the psf
  pfitsio::header_params psf_header = def_header;
  psf_header.fits_name = out_dir + "/psf.fits";
  psf_header.pix_units = "Jy/Beam";
  const Vector<t_complex> psf = measurements_transform->adjoint() * (uv_data.weights.array());
  const Image<t_real> psf_image =
      Image<t_complex>::Map(psf.data(), params.height(), params.width()).real();
  if (params.mpiAlgorithm() != factory::algo_distribution::serial) {
#ifdef PURIFY_MPI
    auto const world = sopt::mpi::Communicator::World();
    if (world.is_root())
#else
    throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
      pfitsio::write2d(psf_image / psf_image.maxCoeff(), psf_header, true);
  } else {
    pfitsio::write2d(psf_image / psf_image.maxCoeff(), psf_header, true);
  }
  // the dirty image
  pfitsio::header_params dirty_header = def_header;
  dirty_header.fits_name = out_dir + "/dirty.fits";
  dirty_header.pix_units = "Jy/Beam";
  const Vector<t_complex> dimage = measurements_transform->adjoint() * uv_data.vis;
  const Image<t_real> dirty_image =
      Image<t_complex>::Map(dimage.data(), params.height(), params.width()).real();
  if (params.mpiAlgorithm() != factory::algo_distribution::serial) {
#ifdef PURIFY_MPI
    auto const world = sopt::mpi::Communicator::World();
    if (world.is_root())
#else
    throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
      pfitsio::write2d(dirty_image / psf_image.maxCoeff(), dirty_header, true);
  } else {
    pfitsio::write2d(dirty_image / psf_image.maxCoeff(), dirty_header, true);
  }

  PURIFY_HIGH_LOG("Starting sopt!");
  // Apply algorithm
  auto const diagnostic = (*algo)();

  // Save the rest of the output
  // the clean image
  const Image<t_real> image =
      Image<t_complex>::Map(diagnostic.x.data(), params.height(), params.width()).real();
  pfitsio::header_params purified_header = def_header;
  purified_header.fits_name = out_dir + "/purified.fits";
  purified_header.hasconverged = diagnostic.good;
  purified_header.niters = diagnostic.niters;
  if (params.mpiAlgorithm() != factory::algo_distribution::serial) {
#ifdef PURIFY_MPI
    auto const world = sopt::mpi::Communicator::World();
    if (world.is_root())
#else
    throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
      pfitsio::write2d(image, purified_header, true);
  } else {
    pfitsio::write2d(image, purified_header, true);
  }
  // the residuals
  pfitsio::header_params residuals_header = purified_header;
  residuals_header.fits_name = out_dir + "/residuals.fits";
  const Vector<t_complex> residuals = measurements_transform->adjoint() * diagnostic.residual;
  const Image<t_real> residual_image =
      Image<t_complex>::Map(residuals.data(), params.height(), params.width()).real();
  if (params.mpiAlgorithm() != factory::algo_distribution::serial) {
#ifdef PURIFY_MPI
    auto const world = sopt::mpi::Communicator::World();
    if (world.is_root())
#else
    throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
      pfitsio::write2d(residual_image, residuals_header, true);
  } else {
    pfitsio::write2d(residual_image, residuals_header, true);
  }

  return 0;
}
