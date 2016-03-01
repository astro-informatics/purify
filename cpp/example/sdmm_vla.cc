#include <array>
#include <memory>
#include <random>
#include "sopt/relative_variation.h"
#include "sopt/sdmm.h"
#include "sopt/utilities.h"
#include "sopt/wavelets.h"
#include "sopt/wavelets/sara.h"
#include "directories.h"
#include "pfitsio.h"
#include "regressions/cwrappers.h"
#include "types.h"
#include "MeasurementOperator.h"

int main(int, char **) {
  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();

  std::string const visfile = vla_filename("at166B.3C129.c0.vis");
  std::string const outfile = output_filename("vla.tiff");
  std::string const outfile_fits = output_filename("vla_solution.fits");
  std::string const dirty_image = output_filename("vla_dirty.tiff");
  std::string const dirty_image_fits = output_filename("vla_dirty.fits");

  t_real const over_sample = 1.375;
  auto uv_data = utilities::read_visibility(visfile);
  t_real cellsize = 0.3;
  t_int width = 512;
  t_int height = 512;
  uv_data = utilities::set_cell_size(uv_data, cellsize); // scale uv coordinates to correct pixel size and to units of 2pi
  uv_data = utilities::uv_scale(uv_data, floor(width * over_sample), floor(height * over_sample)); // scale uv coordinates to units of Fourier grid size
  uv_data = utilities::uv_symmetry(uv_data); // Enforce conjugate symmetry by reflecting measurements in uv coordinates
  MeasurementOperator measurements(uv_data, 4, 4, "kb_interp", width, height, over_sample, "natural");

 
  auto direct = [&measurements, &width, &height](Vector<t_complex> &out, Vector<t_complex> const &x) {
        assert(x.size() == width * height);
        auto const image = Image<t_complex>::Map(x.data(), height, width);
        out = measurements.degrid(image);
  };
  auto adjoint = [&measurements, &width, &height](Vector<t_complex> &out, Vector<t_complex> const &x) {
        auto image = Image<t_complex>::Map(out.data(), height, width);
        image = measurements.grid(x);
  };
  auto measurements_transform = sopt::linear_transform<Vector<t_complex>>(
    direct, {0, 1, static_cast<t_int>(uv_data.vis.size())},
    adjoint, {0, 1, static_cast<t_int>(width * height)}
  );

  sopt::wavelets::SARA const sara{std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u), std::make_tuple("DB3", 3u), std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u), std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const Psi = sopt::linear_transform<t_complex>(sara, height, width);

  Vector<t_complex> & input = uv_data.vis;
  Vector<> dimage = (measurements_transform.adjoint() * input).real();
  t_real max_val = dimage.array().abs().maxCoeff();
  dimage = dimage / max_val;
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());

  sopt::utilities::write_tiff(Image<t_real>::Map(dimage.data(), height, width), dirty_image);
  pfitsio::write2d(Image<t_real>::Map(dimage.data(), height, width), dirty_image_fits);


  t_real noise_variance = utilities::variance(uv_data.vis.array() * measurements.W)/2;
  t_real const noise_rms = std::sqrt(noise_variance);
  std::cout << "Calculated RMS noise of "<< noise_rms * 1e3 << " mJy" << '\n';

  input = uv_data.vis / max_val;
  noise_variance = utilities::variance(input.array() * measurements.W)/2;
  t_real epsilon = std::sqrt(2 * uv_data.vis.size() + 4 * std::sqrt(uv_data.vis.size()) * noise_variance); //Calculation of l_2 bound following SARA paper
  std::cout << "Starting sopt!" << '\n';
  std::cout << "Epsilon = " << epsilon << '\n';
  auto const sdmm
      = sopt::algorithm::SDMM<t_complex>()
            .itermax(11)
            .gamma((measurements_transform.adjoint() * input).real().maxCoeff() * 1e-3) //l_1 bound
            .is_converged(sopt::RelativeVariation<t_complex>(1e-3))
            .conjugate_gradient(100, 1e-3)
            .append(sopt::proximal::translate(sopt::proximal::L2Ball<t_complex>(epsilon), -input),
                    measurements_transform)
            .append(sopt::proximal::l1_norm<t_complex>, Psi.adjoint(), Psi)
            .append(sopt::proximal::positive_quadrant<t_complex>);
  auto const result = sdmm(initial_estimate);
  assert(result.out.size() == width * height);
  Image<t_real> image = Image<t_complex>::Map(result.out.data(), height, width).real();
  t_real max_val_final = image.array().abs().maxCoeff();
  image = image / max_val_final;
  sopt::utilities::write_tiff(image, outfile);
  pfitsio::write2d(image, outfile_fits);
  return 0;
}
