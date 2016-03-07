
#include "MeasurementOperator.h"
#include "utilities.h"
#include "pfitsio.h"

#include "directories.h"

using namespace purify;  
using namespace purify::notinstalled;  


int main( int nargs, char const** args ){
  
  utilities::vis_params uv_vis;
  t_real max;
  t_real max_diff;
  t_real over_sample;
  t_real cellsize;
  std::string kernel;


  //Gridding example
  cellsize = 5;//
  over_sample = 2;
  t_int J = 4;

  t_int number_of_vis = 200;
  bool use_w_term = true;
  std::string const fitsfile = image_filename("M31.fits");
  auto M31 = pfitsio::read2d(fitsfile);
  t_int height = M31.rows();
  t_int width = M31.cols();
  
  uv_vis = utilities::random_sample_density(number_of_vis, 0, 4000);
  

  const t_real theta_FoV_L = cellsize * width * over_sample;
  const t_real theta_FoV_M = cellsize * height * over_sample;

  const t_real L = 2 * std::sin(purify_pi / 180.* theta_FoV_L / (60. * 60.) * 0.5);
  const t_real M = 2 * std::sin(purify_pi / 180.* theta_FoV_M / (60. * 60.) * 0.5);
  t_real FoV = std::max(L, M);
  uv_vis.w = Vector<t_real>::Constant(number_of_vis, 1)/FoV * (uv_vis.u.array() * uv_vis.u.array() + uv_vis.v.array() * uv_vis.v.array()).sqrt().maxCoeff();

  kernel = "kb";
  MeasurementOperator op(uv_vis, J, J, kernel, width, height, over_sample, "none", 0, use_w_term, cellsize, cellsize); // Create Measurement Operator
  auto y0 = op.degrid(M31);
  pfitsio::write2d(op.grid(y0).real(), "w_test.fits");
  MeasurementOperator op_norm(uv_vis, J, J, kernel, width, height, over_sample, "none", 0, false, cellsize, cellsize); // Create Measurement Operator
  pfitsio::write2d(op_norm.grid(y0).real(), "w_norm_test.fits");
}