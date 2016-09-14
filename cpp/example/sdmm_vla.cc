#include <array>
#include <memory>
#include <random>
#include <sopt/relative_variation.h>
#include <sopt/sdmm.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include "purify/MeasurementOperator.h"
#include "purify/directories.h"
#include "purify/pfitsio.h"
#include "purify/types.h"

int main(int, char **) {
  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();

  std::string const visfile = vla_filename("at166B.3C129.c0.vis");
  std::string const outfile = output_filename("vla.tiff");
  std::string const outfile_fits = output_filename("vla_solution.fits");

  std::string const dirty_image_fits = output_filename("vla_dirty.fits");

  std::string const residual_fits = output_filename("vla_residual.fits");

  t_int const niters = 1e1;
  t_real const beta = 1e-3;
  t_real const over_sample = 1.375;
  auto uv_data = utilities::read_visibility(visfile);
  uv_data.units = "lambda";
  t_real cellsize = 0.3;
  t_int width = 512;
  t_int height = 512;
  uv_data = utilities::uv_symmetry(uv_data);
  MeasurementOperator measurements(uv_data, 4, 4, "kb_interp", width, height, 20, over_sample,
                                   cellsize, cellsize, "whiten");
  // putting measurement operator in a form that sopt can use
  auto measurements_transform = linear_transform(measurements, uv_data.vis.size());

  sopt::wavelets::SARA const sara{std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
                                  std::make_tuple("DB3", 3u), std::make_tuple("DB4", 3u),
                                  std::make_tuple("DB5", 3u), std::make_tuple("DB6", 3u),
                                  std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  auto const Psi = sopt::linear_transform<t_complex>(sara, height, width);
  std::printf("Saving dirty map \n");
  Vector<t_complex> &input = uv_data.vis;
  Vector<> dimage = (measurements_transform.adjoint() * input).real();
  t_real max_val = dimage.array().abs().maxCoeff();
  dimage = dimage / max_val;
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  if(utilities::file_exists(outfile_fits))
    std::printf("Using previous run.");
  // initial_estimate = pfitsio::read2d(outfile_fits);

  pfitsio::write2d(Image<t_real>::Map(dimage.data(), height, width), dirty_image_fits);

  const Vector<t_complex> &weighted_data = (uv_data.vis.array() * measurements.W).matrix();
  t_real noise_variance = utilities::variance(weighted_data) / 2;
  t_real const noise_rms = std::sqrt(noise_variance);
  std::cout << "Calculated RMS noise of " << noise_rms * 1e3 << " mJy" << '\n';

  t_real epsilon = utilities::calculate_l2_radius(
      uv_data.vis, 1.); // Calculation of l_2 bound following SARA paper
  auto purify_gamma
      = (Psi.adjoint() * (measurements_transform.adjoint() * (input.array()).matrix()))
            .real()
            .maxCoeff()
        * beta;

  std::cout << "Starting sopt!" << '\n';
  std::cout << "Epsilon = " << epsilon << '\n';
  auto const sdmm = sopt::algorithm::SDMM<t_complex>()
                        .itermax(niters)
                        .gamma(purify_gamma) // l_1 bound
                        .is_converged(sopt::RelativeVariation<t_complex>(1e-3))
                        .conjugate_gradient(100, 1e-3)
                        .append(sopt::proximal::translate(
                                    sopt::proximal::L2Ball<t_complex>(epsilon), -uv_data.vis),
                                measurements_transform)
                        .append(sopt::proximal::l1_norm<t_complex>, Psi.adjoint(), Psi)
                        .append(sopt::proximal::positive_quadrant<t_complex>);
  Vector<t_complex> result;
  auto const diagonstic = sdmm(result);
  Image<t_complex> image
      = Image<t_complex>::Map(result.data(), measurements.imsizey(), measurements.imsizex());
  t_real const max_val_final = image.array().abs().maxCoeff();
  image = image / max_val_final;
  sopt::utilities::write_tiff(image.real(), outfile);
  pfitsio::write2d(image.real(), outfile_fits);
  Image<t_complex> residual = measurements.grid(input - measurements.degrid(image));
  pfitsio::write2d(residual.real(), residual_fits);
  return 0;
}
