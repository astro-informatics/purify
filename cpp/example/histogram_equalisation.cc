#include "purify/config.h"
#include "purify/types.h"
#include "purify/cimg.h"
#include "purify/directories.h"
#include "purify/pfitsio.h"
using namespace purify;
using namespace purify::notinstalled;

int main(int nargs, char const **args) {
  // up samples M31 example
  auto const input_name =
      (nargs > 1) ? static_cast<std::string>(args[1]) : image_filename("M31.fits");
  auto const output_name = (nargs > 2) ? static_cast<std::string>(args[2]) : "M31_heq.fits";
  Image<t_real> const input = pfitsio::read2d(input_name).real();
#ifdef PURIFY_CImg
  CDisplay display = cimg::make_display<Image<t_real>>(input, "Image");
  const auto img1 = cimg::make_image(input.real().eval()).get_normalize(0, 1);
  const auto results = img1.get_equalize(256, 0.01, 1);
  const Image<t_real> &output = Image<t_real>::Map(reinterpret_cast<const t_real *>(results.data()),
                                                   input.rows(), input.cols());
  pfitsio::write2d(output, output_name);
#else
  throw std::runtime_error("compile with CImg.");
#endif
}
