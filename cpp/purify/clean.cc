#include "purify/config.h"
#include "purify/clean.h"
#include "purify/logging.h"

namespace purify {

namespace clean {
Image<t_complex> clean(MeasurementOperator &op, const utilities::vis_params &uv_vis,
                       const t_int &niters, const t_real &gain, const std::string &mode,
                       const t_real clip) {
  /*
          hogbom and sdi clean algorithm
  */
  PURIFY_HIGH_LOG("Starting Clean...");
  Vector<t_complex> residual = uv_vis.vis.array() * uv_vis.weights.array();
  Image<t_complex> res_image = op.grid(residual);
  Image<t_complex> clean_model = res_image * 0;
  Image<t_complex> temp_model = clean_model * 0;

  // should add a method to calculate clean sdi clip automatically
  PURIFY_MEDIUM_LOG("Will run for %d iterations", niters);
  for(t_int i = 0; i < niters; ++i) {
    // finding peak in residual image
    t_int max_x;
    t_int max_y;
    res_image = op.grid(residual);
    res_image.abs().maxCoeff(&max_y, &max_x);
    if(i % 50 == 0)
      PURIFY_LOW_LOG("Iteration: %d, Max: %f, RMS: %f", i, std::abs(res_image(max_y, max_x)),
                     utilities::standard_deviation(
                         Image<t_complex>::Map(res_image.data(), res_image.size(), 1)));
    // generating clean model
    if(mode == "hogbom")
      temp_model(max_y, max_x) = gain * res_image(max_y, max_x);

    if(mode == "steer") {
      t_real max = std::abs(res_image(max_y, max_x));
      temp_model = res_image;
      // clipping residual map for clean model
      for(t_int i = 0; i < temp_model.size(); ++i) {
        if(std::abs(temp_model(i)) < max * clip)
          temp_model(i) = 0;
      }
      // need to write in correction factor for beam volume, eta
      auto dirty_model = op.grid(op.degrid(temp_model));
      t_complex eta = (res_image * dirty_model.conjugate()).sum()
                      / (dirty_model * dirty_model.conjugate()).sum();
      if(0 < std::abs(eta) < 0.02)
        eta = 0.02 * eta
              / std::abs(eta); // imperical way to stop a semi-infinite loop, following miriad

      temp_model = eta * gain * temp_model;
    }
    // add components to clean model
    clean_model = clean_model + temp_model;
    // subtract model from data
    residual = residual - (op.degrid(temp_model).array() * uv_vis.weights.array()).matrix();
    // clear temp model for next iteration
    temp_model = temp_model * 0;
  }
  return clean_model;
}

Image<t_complex> model_estimate(const Image<t_complex> &dirty_image,
                                const Image<t_complex> &dirty_beam, const t_int &niters,
                                const t_real &gain, const t_real clip) {
  /*
          Fast method to produce a model for Purify to use as an initial estimate.
  */

  Image<t_complex> res_image = dirty_image;
  Image<t_complex> clean_model = res_image * 0;
  Image<t_complex> temp_model = clean_model * 0;

  FFTOperator fft;
  const Image<t_complex> dirty_beam_fft = fft.forward(dirty_beam);
  // should add a method to calculate clean steer clip automatically

  for(t_int i = 0; i < niters; ++i) {
    // finding peak in residual image
    t_int max_x;
    t_int max_y;

    res_image.abs().maxCoeff(&max_y, &max_x);

    // generating clean model

    t_real max = std::abs(res_image(max_y, max_x));
    temp_model = res_image;
    // clipping residual map for clean model
    for(t_int i = 0; i < temp_model.size(); ++i) {
      if(std::abs(temp_model(i)) < max * clip)
        temp_model(i) = 0;
    }
    // convolve beam with temp model to create dirty model
    Image<t_complex> dirty_model = fft.inverse(fft.forward(temp_model).array() * dirty_beam_fft);
    // need to write in correction factor for beam volume, eta
    t_complex eta = (res_image * dirty_model.conjugate()).sum()
                    / (dirty_model * dirty_model.conjugate()).sum();
    if(0 < std::abs(eta) < 0.02)
      eta = 0.02 * eta
            / std::abs(eta); // empirical way to stop a semi-infinite loop, following miriad

    temp_model = eta * gain * temp_model;
    // subtract model from data
    res_image = res_image - eta * gain * dirty_model;
    // add components to clean model
    clean_model = clean_model + temp_model;
    // clear temp model for next iteration
    temp_model = temp_model * 0;
  }
  return clean_model;
}
Image<t_complex>
convolve_model(const Image<t_complex> &clean_model, const Image<t_complex> &gaussian) {
  Fft2d fft;
  const Image<t_complex> fft_point_source = fft.forward(gaussian);
  const Image<t_complex> clean_model_fft = fft.forward(clean_model);
  return fft.inverse(clean_model_fft.array() * fft_point_source.array()).array();
}
Image<t_complex> fit_gaussian(MeasurementOperator &op, const utilities::vis_params &uv_vis) {
  Image<t_real> psf = op.grid(uv_vis.weights).real();
  t_int psf_x;
  t_int psf_y;
  t_real max = psf.maxCoeff(&psf_x, &psf_y);
  PURIFY_LOW_LOG("PSF max pixel at (%d, %d)", psf_y, psf_x);
  psf = psf / max;
  Image<t_complex> gaussian = Image<t_complex>::Zero(op.imsizey(), op.imsizex());
  // choice of parameters
  auto const fit = utilities::fit_fwhm(psf, 3);
  auto fwhm_x = fit(0) * 2 * std::sqrt(2 * std::log(2));
  auto fwhm_y = fit(1) * 2 * std::sqrt(2 * std::log(2));
  auto theta = fit(2);
  PURIFY_MEDIUM_LOG("Fitted a Beam of %f x %f , %f", fwhm_x, fwhm_y, theta / constant::pi * 180);
  // setting up Gaussian calculation
  t_real const sigma_x = fwhm_x / (2 * std::sqrt(2 * std::log(2)));
  t_real const sigma_y = fwhm_y / (2 * std::sqrt(2 * std::log(2)));
  // calculating Gaussian
  t_real const a = std::pow(std::cos(theta), 2) / (2 * sigma_x * sigma_x)
                   + std::pow(std::sin(theta), 2) / (2 * sigma_y * sigma_y);
  t_real const b = -std::sin(2 * theta) / (4 * sigma_x * sigma_x)
                   + std::sin(2 * theta) / (4 * sigma_y * sigma_y);
  t_real const c = std::pow(std::sin(theta), 2) / (2 * sigma_x * sigma_x)
                   + std::pow(std::cos(theta), 2) / (2 * sigma_y * sigma_y);
  auto x0 = op.imsizex() * 0.5;
  auto y0 = op.imsizey() * 0.5;
  for(t_int i = 0; i < op.imsizex(); ++i) {
    for(t_int j = 0; j < op.imsizey(); ++j) {
      t_real x = i - x0;
      t_real y = j - y0;
      gaussian(j, i) = std::exp(-a * x * x + 2 * b * x * y - c * y * y)
                       / (2 * constant::pi * sigma_x * sigma_y);
    }
  }
  t_int gaussian_x;
  t_int gaussian_y;
  PURIFY_DEBUG("{}", gaussian.abs().maxCoeff(&gaussian_x, &gaussian_y));
  PURIFY_LOW_LOG("Gaussian max pixel at (%d, %d)", gaussian_y, gaussian_x);
  return gaussian;
}
Image<t_complex> restore(MeasurementOperator &op, const utilities::vis_params &uv_vis,
                         const Image<t_complex> &clean_model) {
  /*
          Produces the final image given a clean model
  */

  const Image<t_complex> final_model
      = clean::convolve_model(clean_model, clean::fit_gaussian(op, uv_vis));
  // need to workout correction factor to multiply residuals by...
  t_real residual_correction_factor = 1.;
  // add convolved clean model to residual image
  auto residual = uv_vis.vis - op.degrid(clean_model);
  Image<t_complex> restored_image
      = op.grid(residual.array() * uv_vis.weights.array()).array() * residual_correction_factor
        + final_model;
  return restored_image;
}
}
}
