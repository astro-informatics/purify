#include "purify/MeasurementOperator.h"
#include "purify/directories.h"
#include "purify/pfitsio.h"

using namespace purify;
using namespace purify::notinstalled;

int main(int nargs, char const **args) {
  // up samples M31 example
  std::string const fitsfile = image_filename("M31.fits");
  auto const M31 = pfitsio::read2d(fitsfile);

  t_int const fft_flag = (FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
  auto fftop = purify::FFTOperator().fftw_flag(fft_flag);
  t_real const resample_ratio = 2.;
  auto const ft_grid = fftop.forward(M31);
  auto const new_ft_grid = utilities::re_sample_ft_grid(ft_grid, resample_ratio);
  auto const M31_resample = fftop.inverse(new_ft_grid).real() * resample_ratio * resample_ratio;
  pfitsio::write2d(M31_resample, output_filename("M31_resample.fits"));
  auto const M31_resample_alt = utilities::re_sample_image(M31, resample_ratio).real();
  pfitsio::write2d(M31_resample_alt, output_filename("M31_resample_alt.fits"));
}
