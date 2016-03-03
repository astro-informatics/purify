
#include "MeasurementOperator.h"
#include "utilities.h"
#include "pfitsio.h"
#include "FFTOperator.h"

#include "directories.h"

using namespace purify;  
using namespace purify::notinstalled;

int main( int nargs, char const** args ){

	const t_real w = 100000;
	const t_real cellx = 5.;
	const t_real celly = 5.;
	const t_int x_size = 1024;
	const t_int y_size = 1024;
	FFTOperator fftop;
	auto chirp = utilities::generate_chirp(w, cellx, celly, x_size, y_size);
	auto chirp_ft = fftop.forward(chirp);
	pfitsio::write2d(chirp.real(), output_filename("chirp_real.fits"));
	pfitsio::write2d(chirp.imag(), output_filename("chirp_imag.fits"));
	pfitsio::write2d(chirp.cwiseAbs(), output_filename("chirp_abs.fits"));
	pfitsio::write2d(chirp_ft.real(), output_filename("chirp_ft_real.fits"));
	pfitsio::write2d(chirp_ft.imag(), output_filename("chirp_ft_imag.fits"));
	pfitsio::write2d(chirp_ft.cwiseAbs(), output_filename("chirp_ft_abs.fits"));
}