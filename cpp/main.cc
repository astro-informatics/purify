#include <array>
#include <cstddef>
#include <ctime>
#include <random>
#include <sopt/imaging_padmm.h>
#include <sopt/positive_quadrant.h>
#include <sopt/relative_variation.h>
#include <sopt/reweighted.h>
#include "purify/yaml-parser.h"
#include "purify/logging.h"
#include "purify/read_measurements.h"
#include "purify/measurement_operator_factory.h"
#include "purify/wavelet_operator_factory.h"
#include "purify/algorithm_factory.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
using namespace purify;

int main(int argc, const char **argv) {
  sopt::logging::initialize();
  purify::logging::initialize();
  

  // Read config file path from command line
  if (argc==1) {
    PURIFY_HIGH_LOG("Specify the config file full path. Aborting.");
    return 1;
  }

  std::string file_path = argv[1];
  YamlParser params = YamlParser(file_path);

  factory::distributed_measurement_operator mop_algo = factory::distributed_measurement_operator::serial;
  factory::distributed_wavelet_operator wop_algo = factory::distributed_wavelet_operator::serial;

if (params.mpiAlgorithm() != factory::algo_distribution::serial)
{
#ifdef PURIFY_MPI
  sopt::mpi::init(argc, argv);
  auto const world = sopt::mpi::Communicator::World();
#else
  throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
  mop_algo = factory::distributed_measurement_operator::mpi_distribute_image;
  wop_algo = factory::distributed_wavelet_operator::mpi_sara;
}

  sopt::logging::set_level(params.logging());
  purify::logging::set_level(params.logging());

  // Read or generate input data
  utilities::vis_params uv_data;
  t_real sigma;
  if (params.source()==purify::utilities::vis_source::measurements) {
    PURIFY_HIGH_LOG("Input visibilities are from files:");
    for (size_t i=0; i<params.measurements().size(); i++)
      PURIFY_HIGH_LOG("{}", params.measurements()[i]);
    sigma = params.measurements_sigma();
    // TODO: use_w_term hardcoded to false for now
    uv_data = read_measurements::read_measurements(params.measurements(), false, stokes::I, params.measurements_units());
  }
  else if (params.source()==purify::utilities::vis_source::simulation) {
    PURIFY_HIGH_LOG("Input visibilities will be generated for random coverage.");
    // TODO: move this to function (in utilities.h?)
    auto image = pfitsio::read2d(params.skymodel());
    if (params.y()!=image.rows() || params.x()!=image.cols())
      throw std::runtime_error("Input image size ("+std::to_string(image.cols())+"x"+std::to_string(image.rows())+
			       ") is not equal to the input one ("+std::to_string(params.x())+"x"+std::to_string(params.y())+")."); 
    t_int const number_of_pixels = image.size();
    t_int const number_of_vis = std::floor(number_of_pixels * 0.2);
    t_real const sigma_m = constant::pi / 3;
    const t_real max_w = 100.; // lambda 
    uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m, max_w);
    uv_data.units = utilities::vis_units::radians;
    auto const sky_measurements = 
    factory::measurement_operator_factory<Vector<t_complex>>(
							     mop_algo,
							     uv_data, params.y(), params.x(), params.Dy(), params.Dx(),
							     params.oversampling(), params.powMethod_iter(), params.powMethod_tolerance(),
							     kernels::kernel_from_string.at(params.Jweights()), 2 * params.Jy(), 2 * params.Jx());
    uv_data.vis = (*sky_measurements) * Image<t_complex>::Map(image.data(), image.size(), 1);
    Vector<t_complex> const y0 = uv_data.vis;
    sigma = utilities::SNR_to_standard_deviation(y0, params.signal_to_noise());
    uv_data.vis = utilities::add_noise(y0, 0., sigma);
  }
  
  // create measurement operator
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> measurements_transform =
    factory::measurement_operator_factory<Vector<t_complex>>(
							     mop_algo,
							     uv_data, params.y(), params.x(), params.Dy(), params.Dx(),
							     params.oversampling(), params.powMethod_iter(), params.powMethod_tolerance(),
							     kernels::kernel_from_string.at(params.Jweights()), params.Jy(), params.Jx());
  // create wavelet operator
  std::vector<std::tuple<std::string, t_uint>> sara;
  for (size_t i=0; i<params.wavelet_basis().size(); i++)
      sara.push_back( std::make_tuple(params.wavelet_basis().at(i), params.wavelet_levels()));

  auto const wavelets_transform =
    factory::wavelet_operator_factory<Vector<t_complex>>(wop_algo, sara, params.y(), params.x());

  // Create algorithm
  auto algo =
    factory::algorithm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
										factory::algorithm::padmm, params.mpiAlgorithm(),
										measurements_transform, wavelets_transform, uv_data, sigma,
										params.y(), params.x(), sara.size(), params.iterations());

  // Save some things before applying the algorithm
  // the config yaml file - this also generates the output directory and the timestamp
  params.writeOutput();
  std::string out_dir = params.output_path()+"/output_"+params.timestamp();
  // the input measurements, if simulated
  if (params.source()==purify::utilities::vis_source::simulation)
    utilities::write_visibility(uv_data, out_dir+"/input.vis");
  const pfitsio::header_params def_header = pfitsio::header_params("", "Jy/Pixel", 
      1, uv_data.ra, uv_data.dec , params.measurements_polarization(),
      params.Dx(), params.Dy(), uv_data.average_frequency, 
      0, 0, false, 0, 0, 0);
  // the dirty image
  pfitsio::header_params dirty_header = def_header;
  dirty_header.fits_name = out_dir+"/dirty.fits";
  dirty_header.pix_units = "Jy/Beam";
  const Vector<t_complex> dimage = measurements_transform->adjoint() * uv_data.vis;
  const Image<t_real> dirty_image = Image<t_complex>::Map(dimage.data(), params.y(), params.x()).real();
if (params.mpiAlgorithm() != factory::algo_distribution::serial)
{
#ifdef PURIFY_MPI
  auto const world = sopt::mpi::Communicator::World();
  if(world.is_root())
#else
  throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
  pfitsio::write2d(dirty_image/dirty_image.maxCoeff(), dirty_header, true);
} else {
  pfitsio::write2d(dirty_image/dirty_image.maxCoeff(), dirty_header, true);
}
  // the psf
  pfitsio::header_params psf_header = def_header;
  psf_header.fits_name = out_dir+"/psf.fits";
  psf_header.pix_units = "Jy/Beam";
  const Vector<t_complex> psf = measurements_transform->adjoint() * (uv_data.weights.array());
  const Image<t_real> psf_image = Image<t_complex>::Map(psf.data(), params.y(), params.x()).real();
if (params.mpiAlgorithm() != factory::algo_distribution::serial)
{
#ifdef PURIFY_MPI
  auto const world = sopt::mpi::Communicator::World();
  if(world.is_root())
#else
  throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
  pfitsio::write2d(psf_image/psf_image.maxCoeff(), psf_header, true);
} else {
  pfitsio::write2d(psf_image/psf_image.maxCoeff(), psf_header, true);
}

  PURIFY_HIGH_LOG("Starting sopt!");
  // Apply algorithm
  auto const diagnostic = (*algo)();

  // Save the rest of the output
  // the clean image
  const Image<t_real> image = Image<t_complex>::Map(diagnostic.x.data(), params.y(), params.x()).real();
  pfitsio::header_params purified_header = def_header;
  purified_header.fits_name = out_dir+"/purified.fits";
  purified_header.hasconverged = diagnostic.good;
  purified_header.niters = diagnostic.niters;
if (params.mpiAlgorithm() != factory::algo_distribution::serial)
{
#ifdef PURIFY_MPI
  auto const world = sopt::mpi::Communicator::World();
  if(world.is_root())
#else
  throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
  pfitsio::write2d(image, purified_header, true);
} else {
  pfitsio::write2d(image, purified_header, true);
}
  // the residuals
  pfitsio::header_params residuals_header = purified_header;
  residuals_header.fits_name = out_dir+"/residuals.fits";
  const Vector<t_complex> residuals = measurements_transform->adjoint()
    * (uv_data.vis - ((*measurements_transform) * diagnostic.x));
  const Image<t_real> residual_image = Image<t_complex>::Map(residuals.data(), params.y(), params.x()).real();
if (params.mpiAlgorithm() != factory::algo_distribution::serial)
{
#ifdef PURIFY_MPI
  auto const world = sopt::mpi::Communicator::World();
  if(world.is_root())
#else
  throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
  pfitsio::write2d(image, residuals_header, true);
} else {
  pfitsio::write2d(image, residuals_header, true);
}

  return 0;
}
