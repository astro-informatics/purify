#include "purify/config.h"
#include <array>
#include <random>
#include "purify/MeasurementOperator.h"
#include "purify/clean.h"
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/pfitsio.h"
#include "purify/types.h"

int main(int, char **) {
  using namespace purify;
  using namespace purify::notinstalled;
  purify::logging::initialize();
  purify::logging::set_level(purify::default_logging_level());

  std::string const vla_1 = "at166B.3C129.c0";

  std::string const name = vla_1;
  std::string const weighting = "whiten";
  std::string const stokes = "I";

  std::string const visfile = vla_filename(name + stokes + ".vis");
  std::string const noisefile = vla_filename(name + "V.vis");

  t_int const niters = 200;

  t_real const over_sample = 1.375;

  auto uv_data = utilities::read_visibility(visfile);
  t_real const max_u = std::sqrt(
      (uv_data.u.array() * uv_data.u.array() + uv_data.v.array() * uv_data.v.array()).maxCoeff());
  uv_data.units = "lambda";
  t_real cellsize = (180 * 3600) / max_u / constant::pi / 3;
  t_int width = 256;
  t_int height = 256;
  // uv_data = utilities::uv_symmetry(uv_data);
  // header information
  pfitsio::header_params header;
  header.mean_frequency = 1381.67703151703;
  header.ra = 0;
  header.dec = 0;
  header.cell_x = cellsize;
  header.cell_y = cellsize;

  std::string const dirty_image_fits = output_filename(name + "_dirty_" + weighting + ".fits");
  std::string const psf_fits = output_filename(name + "_psf_" + weighting + ".fits");

  MeasurementOperator measurements(uv_data, 4, 4, "kb_min", width, height, 20, over_sample,
                                   cellsize, cellsize, "none");

  uv_data.weights
      = utilities::init_weights(uv_data.u, uv_data.v, uv_data.weights, over_sample, weighting, 0,
                                over_sample * width, over_sample * height);

  auto const noise_uv_data = utilities::read_visibility(noisefile);
  Vector<t_complex> const noise_vis = uv_data.weights.array() * noise_uv_data.vis.array();

  auto sigma_real = utilities::median(noise_vis.real().cwiseAbs()) / 0.6745;

  std::cout << "Stokes V RMS noise of " << sigma_real * 1e3 << " mJy and " << sigma_real * 1e3
            << " mJy" << '\n';

  auto psf_model = clean::fit_gaussian(measurements, uv_data);
  std::string const psf_model_fits
      = output_filename(name + "_psf_model_" + weighting + "_clean.fits");
  header.pix_units = "JY/BEAM";
  header.fits_name = psf_model_fits;
  pfitsio::write2d_header(psf_model.real(), header);

  auto model = clean::clean(measurements, uv_data, niters, 0.1, "hogbom");

  const Image<t_complex> final_model = clean::convolve_model(model, psf_model);

  std::string const model_fits = output_filename(name + "_model_" + weighting + "_clean.fits");
  header.pix_units = "JY/BEAM";
  header.fits_name = model_fits;
  pfitsio::write2d_header(model.real(), header);

  auto restored_image = clean::restore(measurements, uv_data, model);

  std::string const outfile_fits = output_filename(name + "_solution_" + weighting + "_clean.fits");

  std::string const residual_fits
      = output_filename(name + "_residual_" + weighting + "_clean.fits");
  Image<t_complex> const image = Image<t_complex>::Map(
      restored_image.data(), measurements.imsizey(), measurements.imsizex());
  // header information
  header.pix_units = "JY/BEAM";
  header.fits_name = outfile_fits;
  pfitsio::write2d_header(image.real(), header);

  Image<t_complex> residual = measurements
                                  .grid(((uv_data.vis - measurements.degrid(model)).array()
                                         * uv_data.weights.array().real())
                                            .matrix())
                                  .array();
  header.pix_units = "JY/BEAM";
  header.fits_name = residual_fits;
  pfitsio::write2d_header(residual.real(), header);

  return 0;
}
