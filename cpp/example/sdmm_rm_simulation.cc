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
#include "RMOperator.h"
#include "utilities.h"
using namespace purify;  

int main( int nargs, char const** args ){
  
  utilities::rm_params rm_vis;
  t_real over_sample;
  t_real cellsize;
  std::string kernel;


  //Gridding example
  cellsize = 0.3; // in rad/m^2 
  over_sample = 2;
  t_int J = 4;
  t_int width = 512;
  kernel = "kb";
  t_int number_of_samples = 1000;
  t_real RM = 0;

  rm_vis.frequency.setLinSpaced(number_of_samples, 100, 200);
  Array<t_real> wavelength_squared = purify_c * purify_c/(rm_vis.frequency.array() * rm_vis.frequency.array()) * 1e-12;
  t_complex I(0, 1);
  rm_vis.linear_polarisation = exp(2. * I * RM * wavelength_squared);
  rm_vis.weights = rm_vis.linear_polarisation.array() * 0. + 1.;
  RMOperator measurements(rm_vis, J, kernel, width, over_sample, cellsize, "none", 0); // Generating gridding matrix

    //working out value of signal given SNR of 30
  t_real sigma = utilities::SNR_to_standard_deviation(rm_vis.linear_polarisation, 30.);
  //adding noise to visibilities
  rm_vis.linear_polarisation = utilities::add_noise(rm_vis.linear_polarisation, 0., sigma);


  auto direct = [&measurements](Vector<t_complex> &out, Vector<t_complex> const &x) {
        assert(x.size() == measurements.imsize);
        out = measurements.degrid(x);
  };
  auto adjoint = [&measurements](Vector<t_complex> &out, Vector<t_complex> const &x) {
        Image<t_complex> image = measurements.grid(x);
  };
  auto measurements_transform = sopt::linear_transform<Vector<t_complex>>(
    direct, {0, 1, static_cast<t_int>(rm_vis.linear_polarisation.size())},
    adjoint, {0, 1, static_cast<t_int>(measurements.imsize)}
  );


  auto dirty = measurements_transform.adjoint() * rm_vis.linear_polarisation;
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dirty.size());

  Image<t_real> dirty_image = dirty.array().abs();
  pfitsio::write2d(dirty_image, "rm_kb_4.fits");

  sopt::wavelets::SARA const sara{std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u), std::make_tuple("DB3", 3u), 
          std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u), std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), 
          std::make_tuple("DB8", 3u)};
  auto const Psi = sopt::linear_transform<t_complex>(sara, measurements.imsize, 1);

  auto const epsilon = utilities::calculate_l2_radius(rm_vis.linear_polarisation, sigma);
  std::printf("Using epsilon of %f \n", epsilon);
  std::cout << "Starting sopt" << '\n';
  auto const sdmm
      = sopt::algorithm::SDMM<t_complex>()
            .itermax(500)
            .gamma((measurements_transform.adjoint() * rm_vis.linear_polarisation).real().maxCoeff() * 1e-3)
            .is_converged(sopt::RelativeVariation<t_complex>(1e-3))
            .conjugate_gradient(100, 1e-3)
            .append(sopt::proximal::translate(sopt::proximal::L2Ball<t_complex>(epsilon), -rm_vis.linear_polarisation),
                    measurements_transform)
            .append(sopt::proximal::l1_norm<t_complex>, Psi.adjoint(), Psi);
            //.append(sopt::proximal::positive_quadrant<t_complex>);
  //auto const result = sdmm(initial_estimate);

}