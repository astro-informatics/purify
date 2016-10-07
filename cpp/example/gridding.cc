
#include "purify/MeasurementOperator.h"
#include "purify/directories.h"
#include "purify/pfitsio.h"
#include "purify/utilities.h"

using namespace purify;

int main(int nargs, char const **args) {
  auto const vis_file = notinstalled::vla_filename("at166B.3C129.c0.vis");

  // Gridding example
  auto const cellsize = 0.3;
  auto const over_sample = 2;
  auto const J = 4;
  auto const uv_vis = utilities::read_visibility(vis_file); // visibility data being read in
  auto const width = 1024;
  auto const height = 1024;

  auto const kernel = "kb";
  MeasurementOperator op(uv_vis, J, J, kernel, width, height, 20, over_sample, cellsize, cellsize,
                         "none", 0); // Generating gridding matrix

  Image<t_real> kb_img = op.grid(uv_vis.vis).real();
  pfitsio::write2d(kb_img.real(), "grid_image.fits");
}
