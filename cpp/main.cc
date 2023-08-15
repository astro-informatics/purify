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

  factory::distributed_measurement_operator mop_algo =
      (not params.gpu()) ? factory::distributed_measurement_operator::serial
                         : factory::distributed_measurement_operator::gpu_serial;
  factory::distributed_wavelet_operator wop_algo = factory::distributed_wavelet_operator::serial;
  bool using_mpi = false;
  std::vector<t_int> image_index = std::vector<t_int>();
  std::vector<t_real> w_stacks = std::vector<t_real>();

#ifdef PURIFY_MPI
  auto const session = sopt::mpi::init(argc, argv);
#endif

  if (params.mpiAlgorithm() != factory::algo_distribution::serial) {
#ifdef PURIFY_MPI
    auto const world = sopt::mpi::Communicator::World();
#else
    throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
    mop_algo = (not params.gpu())
                   ? factory::distributed_measurement_operator::mpi_distribute_image
                   : factory::distributed_measurement_operator::gpu_mpi_distribute_image;
    if (params.mpi_all_to_all())
      mop_algo = (not params.gpu())
                     ? factory::distributed_measurement_operator::mpi_distribute_all_to_all
                     : factory::distributed_measurement_operator::gpu_mpi_distribute_all_to_all;
    wop_algo = factory::distributed_wavelet_operator::mpi_sara;
    if (params.mpiAlgorithm() == factory::algo_distribution::mpi_random_updates) {
      mop_algo = (not params.gpu()) ? factory::distributed_measurement_operator::serial
                                    : factory::distributed_measurement_operator::serial;
      wop_algo = factory::distributed_wavelet_operator::serial;
    }
    using_mpi = true;
  }

  sopt::logging::set_level(params.logging());
  purify::logging::set_level(params.logging());

  // Read or generate input data
  utilities::vis_params uv_data;
  t_real sigma;
  Vector<t_complex> measurement_op_eigen_vector =
      Vector<t_complex>::Ones(params.width() * params.height());
  // read eigen vector for power method
  if (params.eigenvector_real() != "" and params.eigenvector_imag() != "") {
    t_int rows;
    t_int cols;
    t_int pols;
    t_int chans;
    Vector<t_real> temp_real;
    Vector<t_real> temp_imag;
    pfitsio::read3d(params.eigenvector_real(), temp_real, rows, cols, chans, pols);
    if (rows != params.height() or cols != params.width() or chans != 1 or pols != 1)
      throw std::runtime_error("Image of measurement operator eigenvector is wrong size.");
    pfitsio::read3d(params.eigenvector_imag(), temp_imag, rows, cols, chans, pols);
    if (rows != params.height() or cols != params.width() or chans != 1 or pols != 1)
      throw std::runtime_error("Image of measurement operator eigenvector is wrong size.");
    measurement_op_eigen_vector.real() = temp_real;
    measurement_op_eigen_vector.imag() = temp_imag;
  }
  if (params.source() == purify::utilities::vis_source::measurements) {
    PURIFY_HIGH_LOG("Input visibilities are from files:");
    for (size_t i = 0; i < params.measurements().size(); i++)
      PURIFY_HIGH_LOG("{}", params.measurements()[i]);
    sigma = params.measurements_sigma();
#ifdef PURIFY_MPI
    if (using_mpi) {
      auto const world = sopt::mpi::Communicator::World();
      uv_data = read_measurements::read_measurements(params.measurements(), world,
                                                     distribute::plan::radial, true, stokes::I,
                                                     params.measurements_units());
      const t_real norm =
          std::sqrt(world.all_sum_all(
                        (uv_data.weights.real().array() * uv_data.weights.real().array()).sum()) /
                    world.all_sum_all(uv_data.size()));
      // normalise weights
      uv_data.weights = uv_data.weights / norm;
      // using no weights for now
      // uv_data.weights = Vector<t_complex>::Ones(uv_data.size());
    } else
#endif
    {
      uv_data = read_measurements::read_measurements(params.measurements(), true, stokes::I,
                                                     params.measurements_units());
      const t_real norm = std::sqrt(
          (uv_data.weights.real().array() * uv_data.weights.real().array()).sum() / uv_data.size());
      // normalising weights
      uv_data.weights = uv_data.weights / norm;
      // using no weights for now
      // uv_data.weights = Vector<t_complex>::Ones(uv_data.size());
    }
    if (params.conjugate_w()) uv_data = utilities::conjugate_w(uv_data);
#ifdef PURIFY_MPI
    if (params.mpi_wstacking() and
        (mop_algo == factory::distributed_measurement_operator::mpi_distribute_all_to_all or
         mop_algo == factory::distributed_measurement_operator::gpu_mpi_distribute_all_to_all)) {
      auto const world = sopt::mpi::Communicator::World();
      const auto cost = [](t_real x) -> t_real { return std::abs(x * x); };
      const t_real du =
          widefield::pixel_to_lambda(params.cellsizex(), params.width(), params.oversampling());
      std::tie(uv_data, image_index, w_stacks) = utilities::w_stacking_with_all_to_all(
          uv_data, du, params.Jx(), params.Jw(), world, params.kmeans_iters(), 0, cost);
    } else if (params.mpi_wstacking()) {
      auto const world = sopt::mpi::Communicator::World();
      const auto cost = [](t_real x) -> t_real { return std::abs(x * x); };
      uv_data = utilities::w_stacking(uv_data, world, params.kmeans_iters(), cost);
    }
#endif
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
    t_int const number_of_vis = params.number_of_measurements();
    t_real const sigma_m = constant::pi / 4;
    const t_real rms_w = params.w_rms();  // lambda
    if (params.measurements().at(0) == "") {
      uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m, rms_w);
      uv_data.units = utilities::vis_units::radians;
      uv_data.weights = Vector<t_complex>::Ones(uv_data.size());
    } else {
#ifdef PURIFY_MPI
      if (using_mpi) {
        auto const world = sopt::mpi::Communicator::World();
        uv_data = read_measurements::read_measurements(params.measurements(), world,
                                                       distribute::plan::radial, true, stokes::I,
                                                       params.measurements_units());
      } else
#endif
        uv_data = read_measurements::read_measurements(params.measurements(), true, stokes::I,
                                                       params.measurements_units());
      uv_data.weights = Vector<t_complex>::Ones(uv_data.weights.size());
    }
    if (params.conjugate_w()) uv_data = utilities::conjugate_w(uv_data);
#ifdef PURIFY_MPI
    if (params.mpi_wstacking() and
        (mop_algo == factory::distributed_measurement_operator::mpi_distribute_all_to_all or
         mop_algo == factory::distributed_measurement_operator::gpu_mpi_distribute_all_to_all)) {
      auto const world = sopt::mpi::Communicator::World();
      const auto cost = [](t_real x) -> t_real { return std::abs(x * x); };
      const t_real du =
          widefield::pixel_to_lambda(params.cellsizex(), params.width(), params.oversampling());
      std::tie(uv_data, image_index, w_stacks) = utilities::w_stacking_with_all_to_all(
          uv_data, du, params.Jx(), params.Jw(), world, params.kmeans_iters(), 0, cost);
    } else if (params.mpi_wstacking()) {
      auto const world = sopt::mpi::Communicator::World();
      const auto cost = [](t_real x) -> t_real { return std::abs(x * x); };
      uv_data = utilities::w_stacking(uv_data, world, params.kmeans_iters(), cost);
    }
#endif
    std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> sky_measurements;
    if (mop_algo != factory::distributed_measurement_operator::mpi_distribute_all_to_all and
        mop_algo != factory::distributed_measurement_operator::gpu_mpi_distribute_all_to_all)
      sky_measurements =
          (not params.wprojection())
              ? factory::measurement_operator_factory<Vector<t_complex>>(
                    mop_algo, uv_data, params.height(), params.width(), params.cellsizey(),
                    params.cellsizex(), params.oversampling(),
                    kernels::kernel_from_string.at(params.kernel()), params.sim_J(), params.sim_J(),
                    params.mpi_wstacking())
              : factory::measurement_operator_factory<Vector<t_complex>>(
                    mop_algo, uv_data, params.height(), params.width(), params.cellsizey(),
                    params.cellsizex(), params.oversampling(),
                    kernels::kernel_from_string.at(params.kernel()), params.sim_J(), params.Jw(),
                    params.mpi_wstacking(), 1e-6, 1e-6, dde_type::wkernel_radial);
    else
      sky_measurements =
          (not params.wprojection())
              ? factory::all_to_all_measurement_operator_factory<Vector<t_complex>>(
                    mop_algo, image_index, w_stacks, uv_data, params.height(), params.width(),
                    params.cellsizey(), params.cellsizex(), params.oversampling(),
                    kernels::kernel_from_string.at(params.kernel()), params.sim_J(), params.sim_J(),
                    params.mpi_wstacking())
              : factory::all_to_all_measurement_operator_factory<Vector<t_complex>>(
                    mop_algo, image_index, w_stacks, uv_data, params.height(), params.width(),
                    params.cellsizey(), params.cellsizex(), params.oversampling(),
                    kernels::kernel_from_string.at(params.kernel()), params.sim_J(), params.Jw(),
                    params.mpi_wstacking(), 1e-6, 1e-6, dde_type::wkernel_radial);
    uv_data.vis =
        ((*sky_measurements) * Vector<t_complex>::Map(image.data(), image.size())).eval().array();
    sigma = utilities::SNR_to_standard_deviation(uv_data.vis, params.signal_to_noise());
    uv_data.vis = utilities::add_noise(uv_data.vis, 0., sigma);
  }
  t_real ideal_cell_x = widefield::estimate_cell_size(uv_data.u.cwiseAbs().maxCoeff(),
                                                      params.width(), params.oversampling());
  t_real ideal_cell_y = widefield::estimate_cell_size(uv_data.v.cwiseAbs().maxCoeff(),
                                                      params.height(), params.oversampling());
#ifdef PURIFY_MPI
  if (using_mpi) {
    auto const comm = sopt::mpi::Communicator::World();
    ideal_cell_x = widefield::estimate_cell_size(
        comm.all_reduce<t_real>(uv_data.u.cwiseAbs().maxCoeff(), MPI_MAX), params.width(),
        params.oversampling());
    ideal_cell_y = widefield::estimate_cell_size(
        comm.all_reduce<t_real>(uv_data.v.cwiseAbs().maxCoeff(), MPI_MAX), params.height(),
        params.oversampling());
  }
#endif
  PURIFY_HIGH_LOG(
      "Using cell size {}\" x {}\", recommended from the uv coverage and field of view is "
      "{}\"x{}\".",
      params.cellsizey(), params.cellsizex(), ideal_cell_y, ideal_cell_x);
  PURIFY_HIGH_LOG("The equivalent miriad cell size is: {}\" x {}\"",
                  widefield::equivalent_miriad_cell_size(params.cellsizex(), params.width(),
                                                         params.oversampling()),
                  widefield::equivalent_miriad_cell_size(params.cellsizey(), params.height(),
                                                         params.oversampling()));
  // create measurement operator
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measurements_transform;
  if (mop_algo != factory::distributed_measurement_operator::mpi_distribute_all_to_all and
      mop_algo != factory::distributed_measurement_operator::gpu_mpi_distribute_all_to_all)
    measurements_transform =
        (not params.wprojection())
            ? factory::measurement_operator_factory<Vector<t_complex>>(
                  mop_algo, uv_data, params.height(), params.width(), params.cellsizey(),
                  params.cellsizex(), params.oversampling(),
                  kernels::kernel_from_string.at(params.kernel()), params.Jy(), params.Jx(),
                  params.mpi_wstacking())
            : factory::measurement_operator_factory<Vector<t_complex>>(
                  mop_algo, uv_data, params.height(), params.width(), params.cellsizey(),
                  params.cellsizex(), params.oversampling(),
                  kernels::kernel_from_string.at(params.kernel()), params.Jy(), params.Jw(),
                  params.mpi_wstacking(), 1e-6, 1e-6, dde_type::wkernel_radial);
  else
    measurements_transform =
        (not params.wprojection())
            ? factory::all_to_all_measurement_operator_factory<Vector<t_complex>>(
                  mop_algo, image_index, w_stacks, uv_data, params.height(), params.width(),
                  params.cellsizey(), params.cellsizex(), params.oversampling(),
                  kernels::kernel_from_string.at(params.kernel()), params.Jy(), params.Jx(),
                  params.mpi_wstacking())
            : factory::all_to_all_measurement_operator_factory<Vector<t_complex>>(
                  mop_algo, image_index, w_stacks, uv_data, params.height(), params.width(),
                  params.cellsizey(), params.cellsizex(), params.oversampling(),
                  kernels::kernel_from_string.at(params.kernel()), params.Jy(), params.Jw(),
                  params.mpi_wstacking(), 1e-6, 1e-6, dde_type::wkernel_radial);
  t_real operator_norm = 1.;
#ifdef PURIFY_MPI
  if (using_mpi) {
    auto const comm = sopt::mpi::Communicator::World();
    auto power_method_result =
        (params.mpiAlgorithm() != factory::algo_distribution::mpi_random_updates)
            ? sopt::algorithm::power_method<Vector<t_complex>>(
                  *measurements_transform, params.powMethod_iter(), params.powMethod_tolerance(),
                  comm.broadcast(measurement_op_eigen_vector).eval())
            : sopt::algorithm::all_sum_all_power_method<Vector<t_complex>>(
                  comm, *measurements_transform, params.powMethod_iter(),
                  params.powMethod_tolerance(), comm.broadcast(measurement_op_eigen_vector).eval());
    measurement_op_eigen_vector = std::get<1>(power_method_result);
    operator_norm = std::get<0>(power_method_result);
  } else
#endif
  {
    auto power_method_result = sopt::algorithm::power_method<Vector<t_complex>>(
        *measurements_transform, params.powMethod_iter(), params.powMethod_tolerance(),
        measurement_op_eigen_vector);
    measurement_op_eigen_vector = std::get<1>(power_method_result);
    operator_norm = std::get<0>(power_method_result);
  }
  PURIFY_LOW_LOG("Value of operator norm is {}", operator_norm);
  t_real const flux_scale = 1.;
  uv_data.vis = uv_data.vis.array() * uv_data.weights.array() / flux_scale;

  // Save some things before applying the algorithm
  // the config yaml file - this also generates the output directory and the timestamp
  if (params.mpiAlgorithm() != factory::algo_distribution::serial) {
#ifdef PURIFY_MPI
    auto const world = sopt::mpi::Communicator::World();
    if (world.is_root())
#else
    throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
      params.writeOutput();
  } else {
    params.writeOutput();
  }
  const std::string out_dir = params.output_prefix() + "/output_" + params.timestamp();
  // Creating header for saving output images during iterations
  const pfitsio::header_params update_header_sol =
      pfitsio::header_params(out_dir + "/sol_update.fits", "Jy/Pixel", 1, uv_data.ra, uv_data.dec,
                             params.measurements_polarization(), params.cellsizex(),
                             params.cellsizey(), uv_data.average_frequency, 0, 0, false, 0, 0, 0);
  const pfitsio::header_params update_header_res =
      pfitsio::header_params(out_dir + "/res_update.fits", "Jy/Beam", 1, uv_data.ra, uv_data.dec,
                             params.measurements_polarization(), params.cellsizex(),
                             params.cellsizey(), uv_data.average_frequency, 0, 0, false, 0, 0, 0);
  const pfitsio::header_params def_header = pfitsio::header_params(
      "", "Jy/Pixel", 1, uv_data.ra, uv_data.dec, params.measurements_polarization(),
      params.cellsizex(), params.cellsizey(), uv_data.average_frequency, 0, 0, false, 0, 0, 0);
  // the eigenvector
  if (params.mpiAlgorithm() != factory::algo_distribution::serial) {
#ifdef PURIFY_MPI
    auto const world = sopt::mpi::Communicator::World();
    if (world.is_root())
#else
    throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
    {
      pfitsio::write2d(measurement_op_eigen_vector.real(), params.height(), params.width(),
                       out_dir + "/eigenvector_real.fits", "pix", true);
      pfitsio::write2d(measurement_op_eigen_vector.imag(), params.height(), params.width(),
                       out_dir + "/eigenvector_imag.fits", "pix", true);
    }
  } else {
    pfitsio::write2d(measurement_op_eigen_vector.real(), params.height(), params.width(),
                     out_dir + "/eigenvector_real.fits", "pix", true);
    pfitsio::write2d(measurement_op_eigen_vector.imag(), params.height(), params.width(),
                     out_dir + "/eigenvector_imag.fits", "pix", true);
  }
  // the psf
  pfitsio::header_params psf_header = def_header;
  psf_header.fits_name = out_dir + "/psf.fits";
  psf_header.pix_units = "Jy/Pixel";
  const Vector<t_complex> psf = measurements_transform->adjoint() * (uv_data.weights / flux_scale);
  const Image<t_real> psf_image =
      Image<t_complex>::Map(psf.data(), params.height(), params.width()).real();
  PURIFY_HIGH_LOG(
      "Peak of PSF: {} (used to convert between Jy/Pixel and Jy/BEAM)",
      psf_image(static_cast<t_int>(params.width() * 0.5 + params.height() * 0.5 * params.width())));
  t_real beam_units = 1.;
  if (params.mpiAlgorithm() != factory::algo_distribution::serial) {
#ifdef PURIFY_MPI
    auto const world = sopt::mpi::Communicator::World();
    beam_units = world.all_sum_all(uv_data.size()) / flux_scale / flux_scale;
    PURIFY_LOW_LOG(
        "Expected image domain residual RMS is {} jy/beam",
        sigma * params.epsilonScaling() * operator_norm /
            (std::sqrt(params.width() * params.height()) * world.all_sum_all(uv_data.size())));
    if (world.is_root())
#else
    throw std::runtime_error("Compile with MPI if you want to use MPI algorithm");
#endif
      pfitsio::write2d(psf_image, psf_header, true);
  } else {
    beam_units = uv_data.size() / flux_scale / flux_scale;
    PURIFY_LOW_LOG("Expected image domain residual RMS is {} jy/beam",
                   sigma * params.epsilonScaling() * operator_norm /
                       (std::sqrt(params.width() * params.height()) * uv_data.size()));
    pfitsio::write2d(psf_image, psf_header, true);
  }
  PURIFY_HIGH_LOG(
      "Theoretical calculation for peak PSF: {} (used to convert between Jy/Pixel and Jy/BEAM)",
      beam_units);
  PURIFY_HIGH_LOG("Effective sigma is {} Jy", sigma * params.epsilonScaling());
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
      pfitsio::write2d(dirty_image / beam_units, dirty_header, true);
  } else {
    pfitsio::write2d(dirty_image / beam_units, dirty_header, true);
  }
  // create wavelet operator
  std::vector<std::tuple<std::string, t_uint>> sara;
  for (size_t i = 0; i < params.wavelet_basis().size(); i++)
    sara.push_back(std::make_tuple(params.wavelet_basis().at(i), params.wavelet_levels()));
  t_uint sara_size = 0;
#ifdef PURIFY_MPI
  {
    auto const world = sopt::mpi::Communicator::World();
    if (params.mpiAlgorithm() == factory::algo_distribution::mpi_random_updates)
      sara = sopt::wavelets::distribute_sara(sara, world);
  }
#endif
  auto const wavelets_transform = factory::wavelet_operator_factory<Vector<t_complex>>(
      wop_algo, sara, params.height(), params.width(), sara_size);

  // Create algorithm
  std::shared_ptr<sopt::algorithm::ImagingProximalADMM<t_complex>> padmm;
  std::shared_ptr<sopt::algorithm::ImagingForwardBackward<t_complex>> fb;
  std::shared_ptr<sopt::algorithm::ImagingPrimalDual<t_complex>> primaldual;
  if (params.algorithm() == "padmm")
    padmm = factory::padmm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
        params.mpiAlgorithm(), measurements_transform, wavelets_transform, uv_data,
        sigma * params.epsilonScaling() / flux_scale, params.height(), params.width(), sara_size,
        params.iterations(), params.realValueConstraint(), params.positiveValueConstraint(),
        (params.wavelet_basis().size() < 2) and (not params.realValueConstraint()) and
            (not params.positiveValueConstraint()),
        params.relVarianceConvergence(), params.dualFBVarianceConvergence(), 50,
        params.epsilonConvergenceScaling(), operator_norm);
  if (params.algorithm() == "fb")
    fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
        params.mpiAlgorithm(), measurements_transform, wavelets_transform, uv_data,
        sigma * params.epsilonScaling() / flux_scale,
        params.stepsize() * std::pow(sigma * params.epsilonScaling() / flux_scale, 2),
        params.regularisation_parameter(), params.height(), params.width(), sara_size,
        params.iterations(), params.realValueConstraint(), params.positiveValueConstraint(),
        (params.wavelet_basis().size() < 2) and (not params.realValueConstraint()) and
            (not params.positiveValueConstraint()),
        params.relVarianceConvergence(), params.dualFBVarianceConvergence(), 50, operator_norm,
        params.model_path(), params.gProximalType());
  if (params.algorithm() == "primaldual")
    primaldual = factory::primaldual_factory<sopt::algorithm::ImagingPrimalDual<t_complex>>(
        params.mpiAlgorithm(), measurements_transform, wavelets_transform, uv_data,
        sigma * params.epsilonScaling() / flux_scale, params.height(), params.width(), sara_size,
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
        update_header_res, params.height(), params.width(), sara_size, using_mpi, beam_units);
  }
  if (params.algorithm() == "primaldual") {
    const std::weak_ptr<sopt::algorithm::ImagingPrimalDual<t_complex>> algo_weak(primaldual);
    // Adding step size update to algorithm
    factory::add_updater<t_complex, sopt::algorithm::ImagingPrimalDual<t_complex>>(
        algo_weak, 1e-3, params.update_tolerance(), params.update_iters(), update_header_sol,
        update_header_res, params.height(), params.width(), sara_size, using_mpi, beam_units);
  }
  if (params.algorithm() == "fb") {
    const std::weak_ptr<sopt::algorithm::ImagingForwardBackward<t_complex>> algo_weak(fb);
    // Adding step size update to algorithm
    factory::add_updater<t_complex, sopt::algorithm::ImagingForwardBackward<t_complex>>(
        algo_weak, 0, params.update_tolerance(), 0, update_header_sol, update_header_res,
        params.height(), params.width(), sara_size, using_mpi, beam_units);
  }

  PURIFY_HIGH_LOG("Starting sopt!");
  Image<t_real> image;
  Image<t_real> residual_image;
  pfitsio::header_params purified_header = def_header;
  purified_header.fits_name = out_dir + "/purified.fits";
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
  residuals_header.fits_name = out_dir + "/residuals.fits";
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
