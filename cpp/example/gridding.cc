
#include "MeasurementOperator.h"
#include "utilities.h"
#include "pfitsio.h"

using namespace purify;  

int main( int nargs, char const** args ){
  
  utilities::vis_params uv_vis;
  t_real max;
  t_real max_diff;
  t_real over_sample;
  t_real cellsize;
  std::string kernel;
  std::string vis_file = "../data/vla/at166B.3C129.c0.vis";
  //std::string vis_file = "../data/ATCA/1637-77.vis";


  //Gridding example
  cellsize = 0.3;
  over_sample = 2;
  t_int J = 4;
  uv_vis = utilities::read_visibility(vis_file); // visibility data being read in
  t_int width = 1024;
  t_int height = 1024;
  uv_vis = utilities::set_cell_size(uv_vis, cellsize); // scale uv coordinates to correct pixel size and to units of 2pi
  uv_vis = utilities::uv_scale(uv_vis, floor(width * over_sample), floor(height * over_sample)); // scale uv coordinates to units of Fourier grid size
  uv_vis = utilities::uv_symmetry(uv_vis); // Enforce condjugate symmetry by reflecting measurements in uv coordinates

  kernel = "kb";
  MeasurementOperator op(uv_vis, J, J, kernel, width, height, over_sample, "uniform", 0); // Generating gridding matrix
  //Vector<t_complex> point_source = uv_vis.vis * 0; point_source.setOnes();  // Creating model visibilities for point source
  //Image<t_complex> psf;
  //psf = op.grid(point_source);
  //max = psf.real().maxCoeff();
  //psf = psf / max;
  //pfitsio::write2d(psf.real(), "kb_psf.fits", true, false);

  Image<t_real> kb_img = op.grid(uv_vis.vis).real();
  max = kb_img.maxCoeff();
  kb_img = kb_img / max;
  pfitsio::write2d(kb_img.real(), "grid_image_real_kb_4.fits", true, false);
  //pfitsio::write2d(op.S.real(), "scale_kb_4.fits", true, false);
}