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
#include "purify/wide_field_utilities.h"
#include "purify/yaml-parser.h"
#include <sopt/imaging_padmm.h>
#include <sopt/positive_quadrant.h>
#include <sopt/power_method.h>
#include <sopt/relative_variation.h>
#include <sopt/reweighted.h>
#include "purify/setup_utils.h"

using namespace purify;


int main(int argc, const char **argv) {
  std::srand(static_cast<t_uint>(std::time(0)));
  std::mt19937 mersnne(std::time(0));
  sopt::logging::initialize();
  purify::logging::initialize();

  // Read config file path from command line
  if (argc == 1) {
    PURIFY_HIGH_LOG("Specify the config file full path. Aborting.");
    return 1;
  }

  std::string file_path = argv[1];
  YamlParser params = YamlParser(file_path);
  if (params.version() != purify::version())
    throw std::runtime_error(
        "Using purify version " + purify::version() +
        " but the configuration file expects version " + params.version() +
        ". Please updated the config version manually to be compatable with the new version.");

#ifdef PURIFY_MPI
  auto const session = sopt::mpi::init(argc, argv);
#endif

  const auto [mop_algo, wop_algo, using_mpi] = selectOperators(params);

  sopt::logging::set_level(params.logging());
  purify::logging::set_level(params.logging());

  // Read or generate input data
  auto [uv_data, sigma, measurement_op_eigen_vector, image_index, w_stacks] = getInputData(params,
                                                                                           mop_algo,
                                                                                           wop_algo,
                                                                                           using_mpi);

  // create measurement operator
  auto [measurements_transform, operator_norm] = createMeasurementOperator(params,
                                                                          mop_algo,
                                                                          wop_algo,
                                                                          using_mpi,
                                                                          image_index,
                                                                          w_stacks,
                                                                          uv_data,
                                                                          measurement_op_eigen_vector);

  // create wavelet operator
  const waveletInfo wavelets = createWaveletOperator(params, wop_algo);

  PURIFY_LOW_LOG("Value of operator norm is {}", operator_norm);
  t_real const flux_scale = 1.;
  uv_data.vis = uv_data.vis.array() * uv_data.weights.array() / flux_scale;

  // Save some things before applying the algorithm
  // the config yaml file - this also generates the output directory and the timestamp
  initOutDirectoryWithConfig(params);

  // Creating header for saving output images during iterations
  const auto [update_header_sol, update_header_res, def_header] = genHeaders(params, uv_data);
  
  // the eigenvector
  saveMeasurementEigenVector(params, measurement_op_eigen_vector);
  
  // the psf
  t_real beam_units = 1.0;
  if (params.mpiAlgorithm() != factory::algo_distribution::serial) {
#ifdef PURIFY_MPI
    auto const world = sopt::mpi::Communicator::World();
    beam_units = world.all_sum_all(uv_data.size()) / flux_scale / flux_scale;
#else
    throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
  } else {
    beam_units = uv_data.size() / flux_scale / flux_scale;
  }

  savePSF(params, def_header, measurements_transform, uv_data, flux_scale, sigma, operator_norm, beam_units);

  // the dirty image
  saveDirtyImage(params, def_header, measurements_transform, uv_data, beam_units);


  // Create algorithm
  std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>> padmm;
  std::shared_ptr<sopt::algorithm::ImagingForwardBackward<t_complex>> fb;
  std::shared_ptr<sopt::algorithm::ImagingPrimalDual<t_complex>> primaldual;
  if (params.algorithm() == "padmm")
    padmm = factory::padmm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
        params.mpiAlgorithm(), measurements_transform, wavelets.transform, uv_data,
        sigma * params.epsilonScaling() / flux_scale, params.height(), params.width(), wavelets.sara_size,
        params.iterations(), params.realValueConstraint(), params.positiveValueConstraint(),
        (params.wavelet_basis().size() < 2) and (not params.realValueConstraint()) and
            (not params.positiveValueConstraint()),
        params.relVarianceConvergence(), params.dualFBVarianceConvergence(), 50,
        params.epsilonConvergenceScaling(), operator_norm);
  if (params.algorithm() == "fb")
    fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
        params.mpiAlgorithm(), measurements_transform, wavelets.transform, uv_data,
        sigma * params.epsilonScaling() / flux_scale,
        params.stepsize() * std::pow(sigma * params.epsilonScaling() / flux_scale, 2),
        params.regularisation_parameter(), params.height(), params.width(), wavelets.sara_size,
        params.iterations(), params.realValueConstraint(), params.positiveValueConstraint(),
        (params.wavelet_basis().size() < 2) and (not params.realValueConstraint()) and
            (not params.positiveValueConstraint()),
        params.relVarianceConvergence(), params.dualFBVarianceConvergence(), 50, operator_norm,
        params.model_path(), params.gProximalType());
  if (params.algorithm() == "primaldual")
    primaldual = factory::primaldual_factory<sopt::algorithm::ImagingPrimalDual<t_complex>>(
        params.mpiAlgorithm(), measurements_transform, wavelets.transform, uv_data,
        sigma * params.epsilonScaling() / flux_scale, params.height(), params.width(), wavelets.sara_size,
        params.iterations(), params.realValueConstraint(), params.positiveValueConstraint(),
        params.relVarianceConvergence(), params.epsilonConvergenceScaling(), operator_norm);
  // Add primal dual preconditioning
  if (params.algorithm() == "primaldual" and params.precondition_iters() > 0) {
    PURIFY_HIGH_LOG(
        "Using visibility sampling density to precondtion primal dual with {} "
        "subiterations",
        params.precondition_iters());
    primaldual->precondition_iters(params.precondition_iters());
#ifdef PURIFY_MPI
    if (using_mpi) {
      const auto world = sopt::mpi::Communicator::World();
      primaldual->precondition_weights(widefield::sample_density_weights(
          uv_data.u, uv_data.v, params.cellsizex(), params.cellsizey(), params.width(),
          params.height(), params.oversampling(), 0.5, world));
    } else
#endif
      primaldual->precondition_weights(widefield::sample_density_weights(
          uv_data.u, uv_data.v, params.cellsizex(), params.cellsizey(), params.width(),
          params.height(), params.oversampling(), 0.5));
  }

  if (params.algorithm() == "padmm") {
    const std::weak_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>> algo_weak(padmm);
    // Adding step size update to algorithm
    factory::add_updater<t_complex, sopt::algorithm::ImagingProximalADMM<t_complex>>(
        algo_weak, 1e-3, params.update_tolerance(), params.update_iters(), update_header_sol,
        update_header_res, params.height(), params.width(), wavelets.sara_size, using_mpi, beam_units);
  }
  if (params.algorithm() == "primaldual") {
    const std::weak_ptr<sopt::algorithm::ImagingPrimalDual<t_complex>> algo_weak(primaldual);
    // Adding step size update to algorithm
    factory::add_updater<t_complex, sopt::algorithm::ImagingPrimalDual<t_complex>>(
        algo_weak, 1e-3, params.update_tolerance(), params.update_iters(), update_header_sol,
        update_header_res, params.height(), params.width(), wavelets.sara_size, using_mpi, beam_units);
  }
  if (params.algorithm() == "fb") {
    const std::weak_ptr<sopt::algorithm::ImagingForwardBackward<t_complex>> algo_weak(fb);
    // Adding step size update to algorithm
    factory::add_updater<t_complex, sopt::algorithm::ImagingForwardBackward<t_complex>>(
        algo_weak, 0, params.update_tolerance(), 0, update_header_sol, update_header_res,
        params.height(), params.width(), wavelets.sara_size, using_mpi, beam_units);
  }

  PURIFY_HIGH_LOG("Starting sopt!");
  Image<t_real> image;
  Image<t_real> residual_image;
  pfitsio::header_params purified_header = def_header;
  purified_header.fits_name = params.output_path() + "/purified.fits";
  const Vector<t_complex> estimate_image =
      (params.warm_start() != "")
          ? Vector<t_complex>::Map(pfitsio::read2d(params.warm_start()).data(),
                                   params.height() * params.width())
                .eval()
          : Vector<t_complex>::Zero(params.height() * params.width()).eval();
  const Vector<t_complex> estimate_res =
      (*measurements_transform * estimate_image).eval() - uv_data.vis;
  if (params.algorithm() == "padmm") {
    // Apply algorithm
    auto const diagnostic = (*padmm)(std::make_tuple(estimate_image.eval(), estimate_res.eval()));

    // Save the rest of the output
    image = Image<t_complex>::Map(diagnostic.x.data(), params.height(), params.width()).real();
    const Vector<t_complex> residuals =
        measurements_transform->adjoint() * (diagnostic.residual / beam_units);
    residual_image =
        Image<t_complex>::Map(residuals.data(), params.height(), params.width()).real();
    purified_header.hasconverged = diagnostic.good;
    purified_header.niters = diagnostic.niters;
  }
  if (params.algorithm() == "fb") {
    // Apply algorithm
    auto const diagnostic = (*fb)(std::make_tuple(estimate_image.eval(), estimate_res.eval()));

    // Save the rest of the output
    // the clean image
    image = Image<t_complex>::Map(diagnostic.x.data(), params.height(), params.width()).real();
    const Vector<t_complex> residuals =
        measurements_transform->adjoint() * (diagnostic.residual / beam_units);
    residual_image =
        Image<t_complex>::Map(residuals.data(), params.height(), params.width()).real();
    purified_header.hasconverged = diagnostic.good;
    purified_header.niters = diagnostic.niters;
  }
  if (params.algorithm() == "primaldual") {
    // Apply algorithm
    auto const diagnostic =
        (*primaldual)(std::make_tuple(estimate_image.eval(), estimate_res.eval()));

    // Save the rest of the output
    image = Image<t_complex>::Map(diagnostic.x.data(), params.height(), params.width()).real();
    const Vector<t_complex> residuals =
        measurements_transform->adjoint() * (diagnostic.residual / beam_units);
    residual_image =
        Image<t_complex>::Map(residuals.data(), params.height(), params.width()).real();
    purified_header.hasconverged = diagnostic.good;
    purified_header.niters = diagnostic.niters;
  }
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
  residuals_header.fits_name = params.output_path() + "/residuals.fits";
  residuals_header.pix_units = "Jy/Beam";
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
