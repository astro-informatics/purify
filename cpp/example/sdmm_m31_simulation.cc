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
#include "utilities.h"
#include <boost/math/special_functions/erf.hpp>

int main( int nargs, char const** args ) {
  if (nargs != 5 )
  {
    std::cout << " Wrong number of arguments! " << '\n';
    return 1;
  }

  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();

  std::string const fitsfile = image_filename("M31.fits");
  std::string const inputfile = output_filename("M31_input.fits");
  

  std::string const kernel = args[1];
  t_real const over_sample = std::stod(static_cast<std::string>(args[2]));
  t_int const J = static_cast<t_int>(std::stod(static_cast<std::string>(args[3])));
  std::string const test_number = static_cast<std::string>(args[4]);


  std::string const outfile_fits = output_filename("M31_solution_" + kernel + "_" + test_number + ".fits");
  std::string const residual_fits = output_filename("M31_residual_" + kernel + "_" + test_number + ".fits");
  std::string const dirty_image_fits = output_filename("M31_dirty_" + kernel + "_" + test_number + ".fits");
  std::string const vis_file = output_filename("M31_vis_" + test_number + ".vis");

  auto uv_data = utilities::read_visibility(vis_file);
  uv_data.units = "radians";
  auto M31 = pfitsio::read2d(fitsfile);
  //uv_data = utilities::uv_symmetry(uv_data); //reflect uv measurements
  MeasurementOperator measurements(uv_data, J, J, kernel, M31.cols(), M31.rows(), over_sample);

  
  auto direct = [&measurements](Vector<t_complex> &out, Vector<t_complex> const &x) {
        assert(x.size() == measurements.imsizex * measurements.imsizey);
        auto const image = Image<t_complex>::Map(x.data(), measurements.imsizey, measurements.imsizex);
        out = measurements.degrid(image);
  };
  auto adjoint = [&measurements](Vector<t_complex> &out, Vector<t_complex> const &x) {
        auto image = Image<t_complex>::Map(out.data(), measurements.imsizey, measurements.imsizex);
        image = measurements.grid(x);
  };
  auto measurements_transform = sopt::linear_transform<Vector<t_complex>>(
    direct, {0, 1, static_cast<t_int>(uv_data.vis.size())},
    adjoint, {0, 1, static_cast<t_int>(measurements.imsizex * measurements.imsizey)}
  );

  sopt::wavelets::SARA const sara{std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u), std::make_tuple("DB3", 3u), 
          std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u), std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), 
          std::make_tuple("DB8", 3u)};

  auto const Psi = sopt::linear_transform<t_complex>(sara, measurements.imsizey, measurements.imsizex);

  Vector<t_complex> const y0
      = (measurements_transform * Vector<t_complex>::Map(M31.data(), M31.size()));
  //working out value of signal given SNR of 30
  t_real sigma = utilities::SNR_to_standard_deviation(y0, 30.);
  //adding noise to visibilities
  uv_data.vis = utilities::add_noise(y0, 0., sigma);
  Vector<> dimage = (measurements_transform.adjoint() * uv_data.vis).real();
  t_real const max_val = dimage.array().abs().maxCoeff();
  dimage = dimage / max_val;
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  //pfitsio::write2d(Image<t_real>::Map(dimage.data(), measurements.imsizey, measurements.imsizex), dirty_image_fits);

  auto const epsilon = utilities::calculate_l2_radius(uv_data.vis, sigma);
  std::printf("Using epsilon of %f \n", epsilon);
  std::cout << "Starting sopt" << '\n';
  auto const sdmm
      = sopt::algorithm::SDMM<t_complex>()
            .itermax(500)
            .gamma((measurements_transform.adjoint() * uv_data.vis).real().maxCoeff() * 1e-3)
            .is_converged(sopt::RelativeVariation<t_complex>(1e-3))
            .conjugate_gradient(100, 1e-3)
            .append(sopt::proximal::translate(sopt::proximal::L2Ball<t_complex>(epsilon), -uv_data.vis),
                    measurements_transform)
            .append(sopt::proximal::l1_norm<t_complex>, Psi.adjoint(), Psi)
            .append(sopt::proximal::positive_quadrant<t_complex>);
  auto const result = sdmm(initial_estimate);
  assert(result.out.size() == M31.size());
  Image<t_complex> image = Image<t_complex>::Map(result.out.data(), measurements.imsizey, measurements.imsizex);
  t_real const max_val_final = image.array().abs().maxCoeff();
  image = image / max_val_final;

  Image<t_real> solution = measurements.degrid(image).array().abs();
  pfitsio::write2d(solution, outfile_fits);
  Image<t_real> residual = (y0 - measurements.degrid(image)).array().abs();
  pfitsio::write2d(residual, residual_fits);
  return 0;
}
