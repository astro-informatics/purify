#include <array>
#include <ctime>
#include <random>
#include <cstddef>
#include <sopt/imaging_padmm.h>
#include <sopt/primal_dual.h>
#include <sopt/positive_quadrant.h>
#include <sopt/relative_variation.h>
#include <sopt/reweighted.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include <sopt/power_method.h>
#include "AlgorithmUpdate.h"
#include "cmdl.h"
#include "purify/MeasurementOperator.h"
#include "purify/casacore.h"
#include "purify/logging.h"
#include "purify/pfitsio.h"
#include "purify/types.h"

using namespace purify;
namespace {

void bandwidth_scaling(purify::utilities::vis_params const &uv_data, purify::Params &params) {
  t_real const max_u = std::sqrt((uv_data.u.array() * uv_data.u.array()).maxCoeff());
  t_real const max_v = std::sqrt((uv_data.v.array() * uv_data.v.array()).maxCoeff());
  if(params.cellsizex == 0 and params.cellsizey == 0) {
    t_real const max = std::sqrt(
        (uv_data.u.array() * uv_data.u.array() + uv_data.v.array() * uv_data.v.array()).maxCoeff());
    params.cellsizex = (180 * 3600) / max / constant::pi / 2;
    params.cellsizey = (180 * 3600) / max / constant::pi / 2;
  }
  if(params.cellsizex == 0)
    params.cellsizex = (180 * 3600) / max_u / constant::pi / 2;
  if(params.cellsizey == 0)
    params.cellsizey = (180 * 3600) / max_v / constant::pi / 2;
}

pfitsio::header_params
create_new_header(purify::utilities::vis_params const &uv_data, purify::Params const &params) {
  // header information
  pfitsio::header_params header;
  header.mean_frequency = uv_data.average_frequency;
  header.ra = uv_data.ra;
  header.dec = uv_data.dec;
  header.cell_x = params.cellsizex;
  header.cell_y = params.cellsizey;
  header.residual_convergence = params.residual_convergence;
  header.relative_variation = params.relative_variation;
  return header;
}

t_real estimate_noise(purify::Params const &params) {

  // Read in visibilities for noise estimate
  t_real sigma_real = 1 / std::sqrt(2);
  t_real sigma_imag = 1 / std::sqrt(2);

  if(params.noisefile != "") {
    auto const noise_uv_data = purify::casa::read_measurementset(
        params.noisefile, purify::casa::MeasurementSet::ChannelWrapper::polarization::V);
    Vector<t_complex> const noise_vis = noise_uv_data.weights.array() * noise_uv_data.vis.array();
    sigma_real = utilities::median(noise_vis.real().cwiseAbs()) / 0.6745;
    sigma_imag = utilities::median(noise_vis.imag().cwiseAbs()) / 0.6745;
  }

  PURIFY_MEDIUM_LOG("RMS noise of {}Jy + i{}Jy", sigma_real, sigma_real);
  return std::sqrt(sigma_real * sigma_real + sigma_imag * sigma_imag); //calculation is for combined real and imaginary sigma, factor of 1/sqrt(2) in epsilon calculation
}

purify::casa::MeasurementSet::ChannelWrapper::polarization choose_pol(std::string const & stokes){
  /*
   Chooses the polarisation to read from a measurement set.
   */
  auto stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::I;
  //stokes
  if (stokes == "I" or stokes == "i")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::I;
  if (stokes == "Q" or stokes == "q")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::Q;
  if (stokes == "U" or stokes == "u")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::U;
  if (stokes == "V" or stokes == "v")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::V;
  //linear
  if (stokes == "XX" or stokes == "xx")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::XX;
  if (stokes == "YY" or stokes == "yy")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::YY;
  if (stokes == "XY" or stokes == "xy")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::XY;
  if (stokes == "YX" or stokes == "yx")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::YX;
  //circular
  if (stokes == "LL" or stokes == "ll")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::LL;
  if (stokes == "RR" or stokes == "rr")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::RR;
  if (stokes == "LR" or stokes == "lr")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::LR;
  if (stokes == "RL" or stokes == "rl")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::RL;
  if (stokes == "P" or stokes == "p")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::P;
  return stokes_val;
}
t_real save_psf_and_dirty_image(
    sopt::LinearTransform<sopt::Vector<sopt::t_complex>> const &measurements,
    purify::utilities::vis_params const &uv_data, purify::Params const &params) {
  // returns psf normalisation
  purify::pfitsio::header_params header = create_new_header(uv_data, params);
  std::string const dirty_image_fits = params.name + "_dirty_" + params.weighting;
  std::string const psf_fits = params.name + "_psf_" + params.weighting;
  Vector<t_complex> const psf_image = measurements.adjoint() * (uv_data.weights.array());
  Image<t_real> psf = Image<t_complex>::Map(psf_image.data(), params.height, params.width).real();
  t_real max_val = psf.array().abs().maxCoeff();
  PURIFY_LOW_LOG("PSF peak is {}", max_val);
  psf = psf;//not normalised, so it is easy to compare scales
  header.fits_name = psf_fits + ".fits";
  PURIFY_HIGH_LOG("Saving {}", header.fits_name);
  pfitsio::write2d_header(psf, header);
  Vector<t_complex> const dirty_image
      = measurements.adjoint() * (uv_data.weights.array() * uv_data.vis.array());
  Image<t_complex> dimage
      = Image<t_complex>::Map(dirty_image.data(), params.height, params.width);
  header.fits_name = dirty_image_fits + ".fits";
  PURIFY_HIGH_LOG("Saving {}", header.fits_name);
  pfitsio::write2d_header(dimage.real(), header);
  if(params.stokes_val == purify::casa::MeasurementSet::ChannelWrapper::polarization::P){
    header.fits_name = dirty_image_fits + "_imag.fits";
    PURIFY_HIGH_LOG("Saving {}", header.fits_name);
    pfitsio::write2d_header(dimage.imag(), header);
  }
    return max_val;
}

void save_final_image(std::string const &outfile_fits, std::string const &residual_fits,
                      Vector<t_complex> const &x, utilities::vis_params const &uv_data,
                      Params const &params, MeasurementOperator measurements) {
  //! Save final output image
  purify::pfitsio::header_params header = create_new_header(uv_data, params);
  Image<t_complex> const image
      = Image<t_complex>::Map(x.data(), measurements.imsizey(), measurements.imsizex());
  // header information
  header.pix_units = "JY/PIXEL";
  header.niters = params.iter;
  header.epsilon = params.epsilon;
  header.fits_name = outfile_fits + ".fits";
  pfitsio::write2d_header(image.real(), header);
  if(params.stokes_val == purify::casa::MeasurementSet::ChannelWrapper::polarization::P){
    header.fits_name = outfile_fits + "_imag.fits";
    pfitsio::write2d_header(image.real(), header);
  }
  Image<t_complex> residual = measurements
                                  .grid(((uv_data.vis - measurements.degrid(image)).array()
                                         * uv_data.weights.array().real())
                                            .matrix())
                                  .array();
  header.pix_units = "JY/PIXEL";
  header.fits_name = residual_fits + ".fits";
  pfitsio::write2d_header(residual.real(), header);
  if(params.stokes_val == purify::casa::MeasurementSet::ChannelWrapper::polarization::P){
    header.fits_name = residual_fits + "_imag.fits";
    pfitsio::write2d_header(residual.real(), header);
  }
};

std::tuple<Vector<t_complex>, Vector<t_complex>>
read_estimates(sopt::LinearTransform<sopt::Vector<sopt::t_complex>> const &measurements,
               purify::utilities::vis_params const &uv_data, purify::Params const &params) {
  Vector<t_complex> initial_estimate
      = measurements.adjoint() * (uv_data.weights.array() * uv_data.vis.array());
  Vector<t_complex> initial_residuals = Vector<t_complex>::Zero(uv_data.vis.size());
  // loading data from check point.
  if(utilities::file_exists(params.name + "_diagnostic") and params.warmstart == true) {
    PURIFY_HIGH_LOG("Loading checkpoint for {}", params.name.c_str());
    std::string const outfile_fits = params.name + "_solution_" + params.weighting + "_update.fits";
    if(utilities::file_exists(outfile_fits)) {
      auto const image = pfitsio::read2d(outfile_fits);
      if(params.height != image.rows() or params.width != image.cols()) {
        std::runtime_error("Initial model estimate is the wrong size.");
      }
      initial_estimate = Matrix<t_complex>::Map(image.data(), image.size(), 1);
      Vector<t_complex> const model = measurements * image;
      initial_residuals = (uv_data.vis - model).array() * (uv_data.weights.array().real());
    }
  }
  std::tuple<Vector<t_complex>, Vector<t_complex>> const estimates(initial_estimate,
                                                                   initial_residuals);
  return estimates;
}

MeasurementOperator
construct_measurement_operator(utilities::vis_params const &uv_data, purify::Params const &params) {
  auto measurements = MeasurementOperator()
                          .Ju(params.J)
                          .Jv(params.J)
                          .kernel_name(params.kernel)
                          .imsizex(params.width)
                          .imsizey(params.height)
                          .norm_iterations(params.power_method_iterations)
                          .oversample_factor(params.over_sample)
                          .cell_x(params.cellsizex)
                          .cell_y(params.cellsizey)
                          .weighting_type("none") // weighting is done outside of the operator
                          .R(0)
                          .use_w_term(params.use_w_term)
                          .energy_fraction(params.energy_fraction)
                          .primary_beam(params.primary_beam)
                          .fft_grid_correction(params.fft_grid_correction)
                          .fftw_plan_flag(params.fftw_plan)
                          .gradient(params.gradient);
  measurements.init_operator(uv_data);
  return measurements;
};
}

int main(int argc, char **argv) {
  sopt::logging::initialize();
  purify::logging::initialize();

  Params params = parse_cmdl(argc, argv);
  sopt::logging::set_level(params.sopt_logging_level);
  purify::logging::set_level(params.sopt_logging_level);
  params.stokes_val = choose_pol(params.stokes);
  PURIFY_HIGH_LOG("Stokes input {}", params.stokes);
  //checking if reading measurement set or .vis file
  std::size_t found = params.visfile.find_last_of(".");
  std::string format =  "." + params.visfile.substr(found+1);
  std::transform(format.begin(), format.end(), format.begin(), ::tolower);
  auto uv_data = (format == ".ms") ? purify::casa::read_measurementset(params.visfile, params.stokes_val) : utilities::read_visibility(params.visfile, params.use_w_term);
  bandwidth_scaling(uv_data, params);

  string algorithm = "ImagingPADMM";
  
  // calculate weights outside of measurement operator
  uv_data.weights = utilities::init_weights(
      uv_data.u, uv_data.v, uv_data.weights, params.over_sample, params.weighting, 0,
      params.over_sample * params.width, params.over_sample * params.height);
  auto const noise_rms = estimate_noise(params);
  auto const measurements = construct_measurement_operator(uv_data, params);
  params.norm = measurements.norm;
  auto const measurements_transform = linear_transform(measurements, uv_data.vis.size());

  sopt::wavelets::SARA const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  auto const Psi = sopt::linear_transform<t_complex>(sara, params.height, params.width);

  PURIFY_LOW_LOG("Saving dirty map");
  params.psf_norm = save_psf_and_dirty_image(measurements_transform, uv_data, params);

  auto const estimates = read_estimates(measurements_transform, uv_data, params);
  t_real const epsilon = params.n_mu * std::sqrt(2 * uv_data.vis.size()) * noise_rms / std::sqrt(2); // Calculation of l_2 bound following SARA paper
  
  params.epsilon = epsilon;
  params.residual_convergence
      = (params.residual_convergence < 0) ? 0. : params.residual_convergence * epsilon;

  std::ofstream out_diagnostic;
  out_diagnostic.precision(13);
  out_diagnostic.open(params.name + "_diagnostic", std::ios_base::app);

  Vector<t_complex> final_model = Vector<t_complex>::Zero(params.width * params.height);
  std::string outfile_fits = "";
  std::string residual_fits = "";
  
  if(algorithm == "ImagingPADMM"){
    t_real purify_gamma = 0;
    std::tie(params.iter, purify_gamma) = utilities::checkpoint_log(params.name + "_diagnostic");
    if(params.iter == 0)
      purify_gamma = (Psi.adjoint() * (measurements_transform.adjoint()
				       * (uv_data.weights.array() * uv_data.vis.array()).matrix()))
	.cwiseAbs()
	.maxCoeff()
	* params.beta;
    
    
    PURIFY_HIGH_LOG("Starting sopt!");
    PURIFY_MEDIUM_LOG("Epsilon = {}", epsilon);
    PURIFY_MEDIUM_LOG("Convergence criteria: Relative variation is less than {}.",
		      params.relative_variation);
    if(params.residual_convergence > 0)
      PURIFY_MEDIUM_LOG("Convergence criteria: Residual norm is less than {}.",
			params.residual_convergence);
    PURIFY_MEDIUM_LOG("Gamma = {}", purify_gamma);
    auto padmm = sopt::algorithm::ImagingProximalADMM<t_complex>(uv_data.vis)
      .gamma(purify_gamma)
      .relative_variation(params.relative_variation)
      .l2ball_proximal_epsilon(epsilon)
      .l2ball_proximal_weights(uv_data.weights.array().real())
      .tight_frame(false)
      .l1_proximal_tolerance(1e-3)
      .l1_proximal_nu(1)
      .l1_proximal_itermax(100)
      .l1_proximal_positivity_constraint(params.positive)
      .l1_proximal_real_constraint(true)
      .residual_convergence(params.residual_convergence)
      .lagrange_update_scale(0.9)
      .nu(1e0)
      .Psi(Psi)
      .Phi(measurements_transform);
    
    auto convergence_function = [](const Vector<t_complex> &x) { return true; };
    AlgorithmUpdate algo_update(params, uv_data, padmm, out_diagnostic, measurements, Psi);
    auto lambda = [&convergence_function, &algo_update](Vector<t_complex> const &x) {
      return convergence_function(x) and algo_update(x);
    };
    if(params.stokes_val != purify::casa::MeasurementSet::ChannelWrapper::polarization::I or params.gradient == "x" or params.gradient == "y")
      padmm.l1_proximal_positivity_constraint(false);
    if(params.stokes_val == purify::casa::MeasurementSet::ChannelWrapper::polarization::P)
      padmm.l1_proximal_real_constraint(false);
    if(params.algo_update)
      padmm.is_converged(lambda);
    if(params.niters != 0)
      padmm.itermax(params.niters);
    if(params.no_reweighted) {
      auto const diagnostic = padmm(estimates);
      outfile_fits = params.name + "_solution_" + params.weighting + "_final";
      residual_fits = params.name + "_residual_" + params.weighting + "_final";
      final_model = diagnostic.x;
    } else {
      auto const posq = sopt::algorithm::positive_quadrant(padmm);
      auto const min_delta = noise_rms * std::sqrt(uv_data.vis.size())
	/ std::sqrt(9 * measurements.imsizey() * measurements.imsizex());
      // Sets weight after each padmm iteration.
      // In practice, this means replacing the proximal of the l1 objective function.
      auto const reweighted
        = sopt::algorithm::reweighted(padmm).itermax(10).min_delta(min_delta).is_converged(
											   sopt::RelativeVariation<std::complex<t_real>>(1e-3));
      auto const diagnostic = reweighted();
      outfile_fits = params.name + "_solution_" + params.weighting + "_final_reweighted";
      residual_fits = params.name + "_residual_" + params.weighting + "_final_reweighted";
      final_model = diagnostic.algo.x;
    }
  }else if(algorithm == "PrimalDual"){

  auto const nlevels = sara.size();
    
  auto const tau = 0.49;
  PURIFY_MEDIUM_LOG("tau is {} ", tau);

  sopt::Vector<t_complex> rand = sopt::Vector<t_complex>::Random(measurements.imsizey() * measurements.imsizex() * nlevels);
  PURIFY_HIGH_LOG("Setting up power method to calculate sigma values for primal dual");
  auto const pm = sopt::algorithm::PowerMethod<sopt::t_complex>().tolerance(1e-6);
  
  PURIFY_HIGH_LOG("Calculating sigma1");
  auto const nu1data = pm.AtA(Psi, rand);
  auto const nu1 = nu1data.magnitude.real();
  auto sigma1 = 1e0 / nu1;
  PURIFY_MEDIUM_LOG("sigma1 is {} ", sigma1);
  
  rand = sopt::Vector<t_complex>::Random(measurements.imsizey() * measurements.imsizex() * (params.over_sample/2));
  
  PURIFY_HIGH_LOG("Calculating sigma2");  
  auto const nu2data = pm.AtA(measurements_transform, rand);
  auto const nu2 = nu2data.magnitude.real();
  auto sigma2 = 1e0 / nu2;
  PURIFY_MEDIUM_LOG("sigma2 is {} ", sigma2);
  
  PURIFY_HIGH_LOG("Calculating kappa");  
  auto const kappa = ((measurements_transform.adjoint() * uv_data.vis).real().maxCoeff() * 1e-3) / nu2;  
  PURIFY_MEDIUM_LOG("kappa is {} ", kappa);

  auto convergence_function = [](const Vector<t_complex> &x) { return true; };

  t_uint iters = 300;
  if(params.niters != 0)
    iters = params.niters;

  
  PURIFY_HIGH_LOG("Creating primal-dual Functor");
  auto const pd = sopt::algorithm::PrimalDual<t_complex>(uv_data.vis)
    .itermax(iters)
    .tau(tau)
    .kappa(kappa)
    .sigma1(sigma1)
    .sigma2(sigma2)
    .levels(nlevels)
    .l2ball_epsilon(epsilon)
    .nu(nu2)
    .relative_variation(1e-5)
    .positivity_constraint(true)
    .residual_convergence(epsilon * 1.001)
    .Psi(Psi)
    .Phi(measurements_transform)
    .is_converged(convergence_function);
  

    Vector<t_complex> final_model = Vector<t_complex>::Zero(params.width * params.height);
    std::string outfile_fits = "";
    std::string residual_fits = "";
    if(params.no_reweighted) {
      auto const diagnostic = pd(estimates);
      outfile_fits = params.name + "_solution_" + params.weighting + "_final";
      residual_fits = params.name + "_residual_" + params.weighting + "_final";
      final_model = diagnostic.x;
    } else {
      auto const posq = sopt::algorithm::positive_quadrant(pd);
      auto const min_delta = noise_rms * std::sqrt(uv_data.vis.size())
	/ std::sqrt(9 * measurements.imsizey() * measurements.imsizex());
      // Sets weight after each pd iteration.
      // In practice, this means replacing the proximal of the l1 objective function.
      auto const reweighted
        = sopt::algorithm::reweighted(pd).itermax(10).min_delta(min_delta).is_converged(
											   sopt::RelativeVariation<std::complex<t_real>>(1e-3));
      auto const diagnostic = reweighted();
      outfile_fits = params.name + "_solution_" + params.weighting + "_final_reweighted";
      residual_fits = params.name + "_residual_" + params.weighting + "_final_reweighted";
      final_model = diagnostic.algo.x;
  
    }
  }else{
    PURIFY_ERROR("Incorrect algorithm chosen: {}",algorithm);
  }
  save_final_image(outfile_fits, residual_fits, final_model, uv_data, params, measurements);
  out_diagnostic.close();

  return 0;
}
