#include <array>
#include <memory>
#include <random>
#include "sopt/relative_variation.h"
#include <sopt/imaging_padmm.h>
#include "sopt/utilities.h"
#include "sopt/wavelets.h"
#include "sopt/wavelets/sara.h"
#include <sopt/positive_quadrant.h>
#include <sopt/relative_variation.h>
#include <sopt/reweighted.h>
#include "directories.h"
#include "pfitsio.h"
#include "regressions/cwrappers.h"
#include "types.h"
#include "MeasurementOperator.h"
#include "utilities.h"
#include <boost/math/special_functions/erf.hpp>

int main(int, char **) {
  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();

  std::string const fitsfile = image_filename("M31.fits");
  std::string const inputfile = output_filename("M31_input.fits");
  std::string const outfile = output_filename("M31.tiff");
  std::string const outfile_fits = output_filename("M31_solution.fits");
  std::string const residual_fits = output_filename("M31_residual.fits");
  std::string const dirty_image = output_filename("M31_dirty.tiff");
  std::string const dirty_image_fits = output_filename("M31_dirty.fits");
  std::string const output_vis_file = output_filename("M31_Random_coverage.vis");

  
  t_real const over_sample = 2;
  auto M31 = pfitsio::read2d(fitsfile);
  t_real const max = M31.array().abs().maxCoeff();
  M31 = M31 * 1. / max;
  pfitsio::write2d(M31.real(), inputfile);
  //Following same formula in matlab example
  t_real const p = 0.15;
  t_real const sigma_m = purify_pi/3;
  t_real const rho = 2 - (boost::math::erf(purify_pi/(sigma_m * std::sqrt(2)))) * (boost::math::erf(purify_pi/(sigma_m * std::sqrt(2))));
  //t_int const number_of_vis = std::floor(p * rho * M31.size());
  t_int const number_of_vis = 1e4;
  //Generating random uv(w) coverage
  auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m);
  uv_data.units = "radians";
  utilities::write_visibility(uv_data, output_vis_file);
  std::cout << "Number of measurements / number of pixels: " << uv_data.u.size() * 1./M31.size() << '\n';
  //uv_data = utilities::uv_symmetry(uv_data); //reflect uv measurements
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

  sopt::wavelets::SARA const sara{std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u), std::make_tuple("DB3", 3u), 
          std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u), std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), 
          std::make_tuple("DB8", 3u)};

  auto const Psi = sopt::linear_transform<t_complex>(sara, measurements.imsizey, measurements.imsizex);

  std::mt19937_64 mersenne;
  Vector<t_complex> const y0
      = (measurements_transform * Vector<t_complex>::Map(M31.data(), M31.size()));
  //working out value of signal given SNR of 30
  t_real sigma = utilities::SNR_to_standard_deviation(y0, 10.);
  //adding noise to visibilities
  uv_data.vis = utilities::add_noise(y0, 0., sigma);
  Vector<> dimage = (measurements_transform.adjoint() * uv_data.vis).real();
  t_real const max_val = dimage.array().abs().maxCoeff();
  dimage = dimage / max_val;
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  sopt::utilities::write_tiff(Image<t_real>::Map(dimage.data(), measurements.imsizey, measurements.imsizex), dirty_image);
  pfitsio::write2d(Image<t_real>::Map(dimage.data(), measurements.imsizey, measurements.imsizex), dirty_image_fits);

  auto const epsilon = utilities::calculate_l2_radius(uv_data.vis, sigma);
  std::printf("Using epsilon of %f \n", epsilon);
  std::cout << "Starting sopt" << '\n';
  auto const padmm = sopt::algorithm::ImagingProximalADMM<t_complex>(uv_data.vis)
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
  
  auto const posq = positive_quadrant(padmm);
  typedef std::remove_const<decltype(posq)>::type t_PosQuadPADMM;
  auto const min_delta = sigma * std::sqrt(y.size()) / std::sqrt(8 * image.size());
  // Sets weight after each padmm iteration.
  // In practice, this means replacing the proximal of the l1 objective function.
  auto set_weights = [](t_PosQuadPADMM &padmm, Vector<t_complex> const &weights) {
    padmm.algorithm().proximals(0) = [weights](Vector<t_complex> &out, t_real gamma, Vector<t_complex> const &x) {
      out = sopt::soft_threshhold(x, gamma * weights);
    };
  };
  auto call_PsiT
      = [&Psi](t_PosQuadPADMM const &, Vector<t_complex> const &x) -> Vector<t_complex> { return Psi.adjoint() * x; };
  auto const reweighted = sopt::algorithm::reweighted(posq, set_weights, call_PsiT)
                              .itermax(5)
                              .min_delta(min_delta)
                              .is_converged(sopt::RelativeVariation<t_complex>(1e-3));  
  auto const diagnostic = reweighted();
  assert(diagnostic.algo.x.data().size() == M31.size());
  Image<t_complex> image = Image<t_complex>::Map(diagnostic.algo.x.data(), measurements.imsizey, measurements.imsizex);
  t_real const max_val_final = image.array().abs().maxCoeff();
  image = image / max_val_final;
  sopt::utilities::write_tiff(image.real(), outfile);
  pfitsio::write2d(image.real(), outfile_fits);
  Image<t_complex> residual = measurements.grid(y0 - measurements.degrid(image));
  pfitsio::write2d(residual.real(), residual_fits);
  return 0;
}
