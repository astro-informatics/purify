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
#include <ctime>

int main( int nargs, char const** args ) {
  if (nargs != 6 )
  {
    std::cout << " Wrong number of arguments! " << '\n';
    return 1;
  }

  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();

  std::string const fitsfile = image_filename("M31.fits");
  

  std::string const kernel = args[1];
  t_real const over_sample = std::stod(static_cast<std::string>(args[2]));
  t_int const J = static_cast<t_int>(std::stod(static_cast<std::string>(args[3])));
  t_real const m_over_n = std::stod(static_cast<std::string>(args[4]));
  std::string const test_number = static_cast<std::string>(args[5]);
  



  std::string const dirty_image_fits = output_filename("M31_dirty_" + kernel + "_" + test_number + ".fits");
  std::string const results = output_filename("M31_results_" + kernel + "_" + test_number + ".txt");

  auto sky_model = pfitsio::read2d(fitsfile);
  auto sky_model_max = sky_model.array().abs().maxCoeff();
  sky_model = sky_model / sky_model_max;
  t_int const number_of_vis = std::floor(m_over_n * sky_model.size());
  t_real const sigma_m = purify_pi/3;
  
  auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m);
  uv_data.units = "radians";
  std::cout << "Number of measurements: " << uv_data.u.size() << '\n';
  MeasurementOperator simulate_measurements(uv_data, 4, 4, "kb", sky_model.cols(), sky_model.rows(), 5); // Generating simulated high quality visibilites
  uv_data.vis = simulate_measurements.degrid(sky_model);

  MeasurementOperator measurements(uv_data, J, J, kernel, sky_model.cols(), sky_model.rows(), 20, over_sample);

  // putting measurement operator in a form that sopt can use
  auto direct = [&measurements](Vector<t_complex> &out, Vector<t_complex> const &x) {
        assert(x.size() == measurements.imsizex() * measurements.imsizey());
        auto const image = Image<t_complex>::Map(x.data(), measurements.imsizey(), measurements.imsizex());
        out = measurements.degrid(image);
  };
  auto adjoint = [&measurements](Vector<t_complex> &out, Vector<t_complex> const &x) {
        auto image = Image<t_complex>::Map(out.data(), measurements.imsizey(), measurements.imsizex());
        image = measurements.grid(x);
  };
  auto measurements_transform = sopt::linear_transform<Vector<t_complex>>(
    direct, {0, 1, static_cast<t_int>(uv_data.vis.size())},
    adjoint, {0, 1, static_cast<t_int>(measurements.imsizex() * measurements.imsizey())}
  );

  sopt::wavelets::SARA const sara{std::make_tuple("Dirac", 3u)};

  auto const Psi = sopt::linear_transform<t_complex>(sara, measurements.imsizey(), measurements.imsizex());


  //working out value of sigma given SNR of 30
  t_real sigma = utilities::SNR_to_standard_deviation(uv_data.vis, 30.);
  //adding noise to visibilities
  uv_data.vis = utilities::add_noise(uv_data.vis, 0., sigma);
  

  Vector<> dimage = (measurements_transform.adjoint() * uv_data.vis).real();
  t_real const max_val = dimage.array().abs().maxCoeff();
  dimage = dimage / max_val;
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  pfitsio::write2d(Image<t_real>::Map(dimage.data(), measurements.imsizey(), measurements.imsizex()), dirty_image_fits);

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
  //Timing reconstruction
  Vector<t_complex> result;
  std::clock_t c_start = std::clock();
  auto const diagonstic = sdmm(result, initial_estimate);
  std::clock_t c_end = std::clock();
  

  Image<t_complex> image = Image<t_complex>::Map(result.data(), measurements.imsizey(), measurements.imsizex());
  t_real const max_val_final = image.array().abs().maxCoeff();
  image = image / max_val_final;

  Vector<t_complex> original = Vector<t_complex>::Map(sky_model.data(), sky_model.size(), 1);
  Image<t_complex> res = sky_model - image;
  Vector<t_complex> residual = Vector<t_complex>::Map(res.data(), image.size(), 1);

  auto snr = 20. * std::log10(original.norm() / residual.norm()); // SNR of reconstruction
  auto total_time = (c_end-c_start) / CLOCKS_PER_SEC; // total time for solver to run in seconds
  //writing snr and time to a file
  std::ofstream out(results);
  out.precision(13);
  out << snr << " " << total_time;
  out.close();

  return 0;
}
