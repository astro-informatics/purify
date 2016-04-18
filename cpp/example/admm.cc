#include <array>
#include <memory>
#include <random>
#include "sopt/relative_variation.h"
#include "sopt/utilities.h"
#include "sopt/wavelets.h"
#include <sopt/l1_padmm.h>
#include "sopt/wavelets/sara.h"
#include "MeasurementOperator.h"
#include "directories.h"
#include "pfitsio.h"
#include "regressions/cwrappers.h"
#include "types.h"

int main(int, char **) {
  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();

  std::string const fitsfile = image_filename("M31.fits");
  std::string const inputfile = output_filename("M31_input.fits");
  std::string const visfile = image_filename("Coverages/cont_sim4.vis");
  std::string const outfile = output_filename("M31.tiff");
  std::string const residual_fits = output_filename("M31_residual.fits");
  std::string const outfile_fits = output_filename("M31_solution.fits");
  std::string const dirty_image = output_filename("M31_dirty.tiff");
  std::string const dirty_image_fits = output_filename("M31_dirty.fits");

  t_real const over_sample = 2;
  auto M31 = pfitsio::read2d(fitsfile);
  t_real const max = M31.array().abs().maxCoeff();
  M31 = M31 * 1. / max;
  auto uv_data = utilities::read_visibilities(visfile, PURIFY_VISIBILITY_FILETYPE_PROFILE_VIS);
  uv_data.units = "radians";
  uv_data = utilities::uv_symmetry(uv_data); //reflect uv measurements
  std::cout << "Number of measurements / number of pixels: " << uv_data.u.size() * 1./M31.size() << '\n';
  MeasurementOperator measurements(uv_data, 4, 4, "kb", M31.cols(), M31.rows(), over_sample);

 
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

  std::mt19937_64 mersenne;
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
  sopt::utilities::write_tiff(Image<t_real>::Map(dimage.data(), M31.rows(), M31.cols()),
                              dirty_image);
  pfitsio::write2d(Image<t_real>::Map(dimage.data(), M31.rows(), M31.cols()), dirty_image_fits);

  auto const epsilon = utilities::calculate_l2_radius(uv_data.vis, sigma);
  std::printf("Using epsilon of %f \n", epsilon);
  std::cout << "Starting sopt!" << '\n';
  auto const padmm = sopt::algorithm::L1ProximalADMM<t_complex>(uv_data.vis)
                         .itermax(500)
                         .gamma((measurements_transform.adjoint() * uv_data.vis).real().maxCoeff() * 1e-3)
                         .relative_variation(1e-3)
                         .l2ball_proximal_epsilon(epsilon)
                         .tight_frame(false)
                         .l1_proximal_tolerance(1e-2)
                         .l1_proximal_nu(1)
                         .l1_proximal_itermax(50)
                         .l1_proximal_positivity_constraint(true)
                         .l1_proximal_real_constraint(true)
                         .residual_convergence(epsilon * 1.001)
                         .lagrange_update_scale(0.9)
                         .nu(1e0)
                         .Psi(Psi)
                         .Phi(measurements_transform);
   auto const result = padmm(initial_estimate);
   assert(result.x.size() == M31.size());
   Image<t_real> final_image = Image<t_complex>::Map(result.x.data(), M31.rows(), M31.cols()).real();
   t_real max_val_final = final_image.array().abs().maxCoeff();
   final_image = final_image / max_val_final;
   sopt::utilities::write_tiff(final_image, outfile);
   pfitsio::write2d(final_image, outfile_fits);
  return 0;
}
