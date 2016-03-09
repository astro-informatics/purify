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

using namespace purify;  
using namespace purify::notinstalled;  


int main( int nargs, char const** args ){
  std::string const fitsfile = image_filename("M31.fits");
  //std::string const visfile = image_filename("Coverages/cont_sim4.vis");
  std::string const outfile = output_filename("M31_w_component.tiff");
  std::string const outfile_fits = output_filename("M31_w_component_solution.fits");
  std::string const dirty_image = output_filename("M31_w_component_dirty.tiff");
  std::string const dirty_image_fits = output_filename("M31_w_component_dirty.fits");  


  utilities::vis_params uv_vis;
  t_real over_sample;
  t_real cellsize;
  std::string kernel;


  //Gridding example
  cellsize = 1./6000. * 180./ purify_pi / 3. * (60. * 60.);
  t_real energy_fraction = 0.99;
  over_sample = 2;
  t_int J = 4;

  t_int number_of_vis = 200;
  bool use_w_term = true;
  
  auto M31 = pfitsio::read2d(fitsfile);
  t_int height = M31.rows();
  t_int width = M31.cols();
  
  uv_vis = utilities::random_sample_density(number_of_vis, 0, 6000. / 3);
  uv_vis.units = "lambda";

  const t_real theta_FoV_L = cellsize * width * over_sample;
  const t_real theta_FoV_M = cellsize * height * over_sample;

  const t_real L = 2 * std::sin(purify_pi / 180.* theta_FoV_L / (60. * 60.) * 0.5);
  const t_real M = 2 * std::sin(purify_pi / 180.* theta_FoV_M / (60. * 60.) * 0.5);
  const t_real FoV = std::max(L, M);
  uv_vis.w = Vector<t_real>::Constant(number_of_vis, 1)/FoV * (uv_vis.u.array() * uv_vis.u.array() + uv_vis.v.array() * uv_vis.v.array()).sqrt().maxCoeff();

  kernel = "kb";
  MeasurementOperator measurements(uv_vis, J, J, kernel, width, height, over_sample, "none", 0, use_w_term, cellsize, cellsize, energy_fraction); // Create Measurement Operator
 
   auto direct = [&measurements, &M31](Vector<t_complex> &out, Vector<t_complex> const &x) {
          assert(x.size() == M31.size());
          auto const image = Image<t_complex>::Map(x.data(), M31.rows(), M31.cols());
          out = measurements.degrid(image);
    };
    auto adjoint = [&measurements, &M31](Vector<t_complex> &out, Vector<t_complex> const &x) {
          auto image = Image<t_complex>::Map(out.data(), M31.rows(), M31.cols());
          image = measurements.grid(x);
    };
    auto measurements_transform = sopt::linear_transform<Vector<t_complex>>(
      direct, {0, 1, static_cast<t_int>(uv_vis.vis.size())},
      adjoint, {0, 1, static_cast<t_int>(M31.size())}
    );

    sopt::wavelets::SARA const sara{std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u), std::make_tuple("DB3", 3u), std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u), std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
    auto const Psi = sopt::linear_transform<t_complex>(sara, M31.rows(), M31.cols());

    std::mt19937_64 mersenne;
    Vector<t_complex> const y0
        = (measurements_transform * Vector<t_complex>::Map(M31.data(), M31.size()));
    auto const input = dirty(y0, mersenne, 30e0);
    Vector<> dimage = (measurements_transform.adjoint() * input).real();
    t_real max_val = dimage.array().abs().maxCoeff();
    dimage = dimage / max_val;
    Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
    sopt::utilities::write_tiff(Image<t_real>::Map(dimage.data(), M31.rows(), M31.cols()), dirty_image);
    pfitsio::write2d(Image<t_real>::Map(dimage.data(), M31.rows(), M31.cols()), dirty_image_fits);

    auto const epsilon
        = std::sqrt(y0.size() + 2 * std::sqrt(y0.size())) * sigma(y0, 30e0)
          / std::sqrt(static_cast<t_real>(y0.size()) / static_cast<t_real>(M31.size()));
    std::cout << "Starting sopt!" << '\n';
    auto const sdmm
        = sopt::algorithm::SDMM<t_complex>()
              .itermax(500)
              .gamma((measurements_transform.adjoint() * input).real().maxCoeff() * 1e-3)
              .is_converged(sopt::RelativeVariation<t_complex>(1e-3))
              .conjugate_gradient(100, 1e-3)
              .append(sopt::proximal::translate(sopt::proximal::L2Ball<t_complex>(epsilon), -input),
                      measurements_transform)
              .append(sopt::proximal::l1_norm<t_complex>, Psi.adjoint(), Psi)
              .append(sopt::proximal::positive_quadrant<t_complex>);
    auto const result = sdmm(initial_estimate);
    assert(result.out.size() == M31.size());
    Image<t_real> image = Image<t_complex>::Map(result.out.data(), M31.rows(), M31.cols()).real();
    t_real max_val_final = image.array().abs().maxCoeff();
    image = image / max_val_final;
    sopt::utilities::write_tiff(image, outfile);
    pfitsio::write2d(image, outfile_fits);
    return 0;

}