
#include "MeasurementOperator.h"
#include "pfitsio.h"
#include "utilities.h"

using namespace purify;

int main(int nargs, char const **args) {
  utilities::vis_params uv_vis;
  t_real max;
  t_real max_diff;
  t_real over_sample;
  t_real cellsize;
  std::string kernel;
  std::string vis_file = "../data/vla/at166B.3C129.c0.vis";
  // std::string vis_file = "../data/atca/1637-77V.vis";

  // Gridding example
  cellsize = 0.3;
  over_sample = 2;
  t_int J = 4;
  uv_vis = utilities::read_visibility(vis_file); // visibility data being read in
  t_int width = 1024;
  t_int height = 1024;
  // uv_vis = utilities::uv_symmetry(uv_vis); // Enforce condjugate symmetry by reflecting
  // measurements in uv coordinates

  kernel = "kb";
  MeasurementOperator op(uv_vis, J, J, kernel, width, height, 20, over_sample, cellsize, cellsize,
                         "none", 0); // Generating gridding matrix

  Image<t_real> kb_img = op.grid(uv_vis.vis).real();
  pfitsio::write2d(kb_img.real(), "grid_image.fits");
  // pfitsio::write2d(op.S.real(), "scale_kb_4.fits");
}