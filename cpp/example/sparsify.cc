
#include "MeasurementOperator.h"
#include "utilities.h"
#include "pfitsio.h"

#include "directories.h"

using namespace purify; 
using namespace purify::notinstalled; 

int main( int nargs, char const** args ){
    //sparsifies M31 example
	const t_real w = 100;
	const t_real cellx = 500;
	const t_real celly = 500;
	const t_int x_size = 1024;
	const t_int y_size = 1024;

	auto chirp = utilities::generate_chirp(w, cellx, celly, x_size, y_size);

    FFTOperator fftop;

    auto ft_grid = fftop.inverse(chirp);
    auto new_ft_grid = utilities::sparsify_chirp(ft_grid, 0.99);
    auto chirp_sparse = fftop.forward(new_ft_grid);
    pfitsio::write2d(chirp_sparse.real(), output_filename("chirp_sparse_real.fits"));
    pfitsio::write2d(chirp_sparse.imag(), output_filename("chirp_sparse_imag.fits"));
}