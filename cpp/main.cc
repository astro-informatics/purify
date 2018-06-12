#include <array>
#include <cstddef>
#include <ctime>
#include <random>
#include <sopt/imaging_padmm.h>
#include <sopt/positive_quadrant.h>
#include <sopt/relative_variation.h>
#include <sopt/reweighted.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include "purify/yaml-parser.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/measurement_operator_factory.h"
#include "purify/wavelet_operator_factory.h"
#include "purify/algorithm_factory.h"
#ifdef PURIFY_ARRAYFIRE
#include "purify/operators_gpu.h"
#endif
#include "purify/pfitsio.h"
#include "purify/types.h"
using namespace purify;
namespace {

  /*purify::casa::MeasurementSet::ChannelWrapper::polarization choose_pol(std::string const &stokes) {
  /* TODO: move this inside the parser, using the new enum
   Chooses the polarisation to read from a measurement set.
   */
  /* auto stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::I;
  // stokes
  if(stokes == "I" or stokes == "i")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::I;
  if(stokes == "Q" or stokes == "q")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::Q;
  if(stokes == "U" or stokes == "u")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::U;
  if(stokes == "V" or stokes == "v")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::V;
  // linear
  if(stokes == "XX" or stokes == "xx")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::XX;
  if(stokes == "YY" or stokes == "yy")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::YY;
  if(stokes == "XY" or stokes == "xy")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::XY;
  if(stokes == "YX" or stokes == "yx")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::YX;
  // circular
  if(stokes == "LL" or stokes == "ll")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::LL;
  if(stokes == "RR" or stokes == "rr")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::RR;
  if(stokes == "LR" or stokes == "lr")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::LR;
  if(stokes == "RL" or stokes == "rl")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::RL;
  if(stokes == "P" or stokes == "p")
    stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::P;
  return stokes_val;
}*/
  /*t_real save_psf_and_dirty_image(
    std::shared_ptr<sopt::LinearTransform<sopt::Vector<sopt::t_complex>> const> const &measurements,
    purify::utilities::vis_params const &uv_data, purify::Params const &params) {
  // returns psf normalisation
  purify::pfitsio::header_params header = create_new_header(uv_data, params);
  std::string const dirty_image_fits = params.name + "_dirty_" + params.weighting;
  std::string const psf_fits = params.name + "_psf_" + params.weighting;
  Vector<t_complex> const psf_image = measurements->adjoint() * (uv_data.weights.array());
  Image<t_real> psf = Image<t_complex>::Map(psf_image.data(), params.height, params.width).real();
  t_real max_val = psf.array().abs().maxCoeff();
  PURIFY_LOW_LOG("PSF peak is {}", max_val);
  psf = psf; // not normalised, so it is easy to compare scales
  header.fits_name = psf_fits + ".fits";
  PURIFY_HIGH_LOG("Saving {}", header.fits_name);
  pfitsio::write2d_header(psf, header);
  Vector<t_complex> const dirty_image = measurements->adjoint() * uv_data.vis;
  Image<t_complex> dimage = Image<t_complex>::Map(dirty_image.data(), params.height, params.width);
  header.fits_name = dirty_image_fits + ".fits";
  PURIFY_HIGH_LOG("Saving {}", header.fits_name);
  pfitsio::write2d_header(dimage.real(), header);
  if(params.stokes_val == purify::casa::MeasurementSet::ChannelWrapper::polarization::P) {
    header.fits_name = dirty_image_fits + "_imag.fits";
    PURIFY_HIGH_LOG("Saving {}", header.fits_name);
    pfitsio::write2d_header(dimage.imag(), header);
  }
  return max_val;
  }*/

} // namespace

int main(int argc, char **argv) {
  sopt::logging::initialize();
  purify::logging::initialize();

  // Read config file path from command line
  if (argc==1) {
    PURIFY_HIGH_LOG("Specify the config file full path. Aborting.");
    return 1;
  }
  std::string file_path = argv[1];
  YamlParser params = YamlParser(file_path);

  sopt::logging::set_level(params.logging());
  purify::logging::set_level(params.logging());
  PURIFY_HIGH_LOG("Input visibilities are from {}", params.source()==purify::utilities::vis_source::measurements ? "measurements" : "simulation");
  PURIFY_HIGH_LOG("Stokes input {}", params.measurements_polarization());
  auto uv_data = utilities::read_visibility(params.measurements_files()[0], false); // TODO: use_w_term hardcoded to false for now
  uv_data.units = params.measurements_units();

  // create measurement operator
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> measurements_transform =
    factory::measurement_operator_factory<Vector<t_complex>>(
							     factory::distributed_measurement_operator::serial,
							     uv_data, params.y(), params.x(), params.Dy(), params.Dx(),
							     params.oversampling(), params.powMethod_iter(), params.powMethod_tolerance(),
							     kernels::kernel_from_string.at(params.Jweights()), params.Jy(), params.Jx());
  // create wavelet operator
  std::vector<std::tuple<std::string, t_uint>> sara;
  for (size_t i=0; i<params.wavelet_basis().size(); i++)
    {
      sara.push_back( std::make_tuple(params.wavelet_basis()[i], params.wavelet_levels()) );
    }
  auto const Psi =
    factory::wavelet_operator_factory<Vector<t_complex>>(
							 factory::distributed_wavelet_operator::serial, sara, params.y(), params.x());
  
  //PURIFY_LOW_LOG("Saving dirty map");
  //params.psf_norm = save_psf_and_dirty_image(measurements_transform, uv_data, params);

  //auto const estimates = read_estimates(measurements_transform, uv_data, params);

  //std::ofstream out_diagnostic;
  // out_diagnostic.precision(13);
  // out_diagnostic.open(params.filepath() + "_diagnostic", std::ios_base::app);

  PURIFY_HIGH_LOG("Starting sopt!");
  //PURIFY_MEDIUM_LOG("Epsilon = {}", epsilon);
  //PURIFY_MEDIUM_LOG("Convergence criteria: Relative variation is less than {}.",
  //                   params.relative_variation);
  //if(params.residual_convergence > 0)
  //   PURIFY_MEDIUM_LOG("Convergence criteria: Residual norm is less than {}.",
  //                    params.residual_convergence);
  //PURIFY_MEDIUM_LOG("Gamma = {}", purify_gamma);
  // TODO: sigma: Real data: input it in yaml file, default to 1. Simulated data: input snr in yaml file defaulted to ?, calculate sigma.
  auto const padmm =
    factory::algorithm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
										factory::algorithm::padmm, factory::algo_distribution::serial,
										measurements_transform, Psi, uv_data, params.measurements_sigma(),
										params.y(), params.x(), sara.size(), params.iterations());


  /*Vector<t_complex> final_model = Vector<t_complex>::Zero(params.width * params.height);
  std::string outfile_fits = "";
  std::string residual_fits = "";
  if(params.stokes_val != purify::casa::MeasurementSet::ChannelWrapper::polarization::I
     or params.gradient == "x" or params.gradient == "y")
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
    auto const min_delta
        = noise_rms * std::sqrt(uv_data.vis.size()) / std::sqrt(9 * params.height * params.width);
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
  save_final_image(outfile_fits, residual_fits, final_model, uv_data, params,
                   measurements_transform);
		   out_diagnostic.close();*/

  auto const diagnostic = (*padmm)();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), params.y(), params.x());
  std::size_t file_begin = params.measurements_files()[0].find_last_of("/");
  std::size_t file_end = params.measurements_files()[0].find_last_of(".");
  std::string outfile_fits = params.measurements_files()[0].substr(0,file_begin) + "/" + params.output_prefix() + "_" + params.measurements_files()[0].substr(file_begin+1,file_end-file_begin) + "fits";
  pfitsio::write2d(image.real(), outfile_fits);
  const Vector<t_complex> residuals = measurements_transform->adjoint()
    * (uv_data.vis - ((*measurements_transform) * diagnostic.x));
    const Image<t_complex> residual_image = Image<t_complex>::Map(residuals.data(), params.y(), params.x());

  return 0;
}
