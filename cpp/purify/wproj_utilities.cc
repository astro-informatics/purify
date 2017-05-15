#include "purify/wproj_utilities.h"

#define EIGEN_DONT_PARALLELIZE
#define EIGEN_NO_AUTOMATIC_RESIZING
namespace purify {
namespace wproj_utilities {

Matrix<t_complex> generate_chirp(const t_real &w_rate, const t_real &cell_x, const t_real &cell_y,
                                 const t_int &x_size, const t_int &y_size) {

  // return chirp image fourier transform for w component
  // w:: w-term in units of lambda
  // celly:: size of y pixel in arcseconds
  // cellx:: size of x pixel in arcseconds
  // x_size:: number of pixels along x-axis
  // y_size:: number of pixels along y-axis

  const t_real pi = constant::pi;
  const t_real theta_FoV_L = cell_x * x_size;
  const t_real theta_FoV_M = cell_y * y_size;

  const t_real L = 2 * std::sin(pi / 180. * theta_FoV_L / (60. * 60.) * 0.5);
  const t_real M = 2 * std::sin(pi / 180. * theta_FoV_M / (60. * 60.) * 0.5);

  const t_real delt_x = 1 * L / x_size;
  const t_real delt_y = 1 * M / y_size;
  Image<t_complex> chirp(y_size, x_size);
  const t_complex I(0, 1);
  const t_real nz = y_size * x_size;

  for(t_int l = 0; l < x_size; ++l) {
    for(t_int m = 0; m < y_size; ++m) {
      const t_real x = (l + 0.5 - x_size * 0.5) * delt_x;
      const t_real y = (m + 0.5 - y_size * 0.5) * delt_y;
      chirp(l, m) = (std::exp(1 * pi * I * w_rate * (x * x + y * y)))
                    * std::exp(-2 * pi * I * (l * 0.5 + m * 0.5)) / nz;
    }
  }

  return chirp;
}

Sparse<t_complex> create_chirp_row(const t_real &w_rate, const t_real &cell_x, const t_real &cell_y,
                                   const t_real &ftsizev, const t_real &ftsizeu,
                                   const t_real &energy_fraction,
                                   const std::shared_ptr<FFTOperator> &fftop_) {

  const t_int Npix = ftsizeu * ftsizev;
  Matrix<t_complex> chirp
      = wproj_utilities::generate_chirp(w_rate, cell_x, cell_y, ftsizeu, ftsizev);
#pragma omp critical(fft)
  chirp = fftop_->forward(chirp);
  chirp.resize(Npix, 1);
  const t_real thres = wproj_utilities::sparsify_row_dense_thres(chirp, energy_fraction);
  t_int sp = 0;
  for(t_int kk; kk < Npix; kk++) {
    if(std::abs(chirp(kk, 0)) > thres) {
      sp++;
      chirp(kk, 0) = 0;
    }
  }
  assert(chirp.norm() > 0);
  const t_real row_max = chirp.cwiseAbs().maxCoeff();
  return chirp.sparseView(row_max, 1e-10);
}

Sparse<t_complex> wprojection_matrix(const Sparse<t_complex> &G, const t_int &Nx, const t_int &Ny,
                                     const Vector<t_real> &w_components, const t_real &cell_x,
                                     const t_real &cell_y, const t_real &energy_fraction_chirp,
                                     const t_real &energy_fraction_wproj) {

  const t_int Npix = Nx * Ny;
  const t_int Nvis = w_components.size();

  PURIFY_HIGH_LOG("Hard-thresholding of the Chirp kernels: energy [{}] ", energy_fraction_chirp);
  PURIFY_HIGH_LOG("Hard-thresholding of the rows of G: energy [{}]  ", energy_fraction_wproj);

  const t_int fft_flag = (FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
  const std::shared_ptr<FFTOperator> fftop_
      = std::make_shared<FFTOperator>(purify::FFTOperator().fftw_flag(fft_flag));
  Sparse<t_complex> GW(Nvis, Npix);
  t_uint counts = 0;
#pragma omp parallel for
  for(t_int m = 0; m < G.outerSize(); m++) {
    // PURIFY_DEBUG("CURRENT WPROJ - Kernel index [{}], w = {}", m, w_components(m));
    const Sparse<t_complex> chirp
        = create_chirp_row(w_components(m), cell_x, cell_y, Nx, Ny, energy_fraction_chirp, fftop_);
    Sparse<t_complex> kernel = row_wise_sparse_convolution(G.row(m), chirp, Nx, Ny);
    assert(kernel.nonZeros() > G.row(m).nonZeros());
    const t_real thres = sparsify_row_thres(kernel, energy_fraction_wproj);
    kernel.prune([&](const t_uint &i, const t_uint &j, const t_complex &value) {
      return std::abs(value) > thres;
    });

    assert(kernel.rows() > 0);
    assert(kernel.cols() == 0);
    assert(kernel.nonZeros() > 0);
#pragma omp critical
    GW.row(m) = kernel.transpose();
#pragma omp critical
    counts++;
    if(counts % 100 == 0) {
      PURIFY_DEBUG("Row {} of {}", counts, GW.rows());
      PURIFY_DEBUG("With {} entries non zero, which is {} entries per a row.", GW.nonZeros(),
                   static_cast<t_real>(GW.nonZeros()) / counts);
    }
  }
  assert(GW.nonZeros() > 0);
  PURIFY_DEBUG("\nBuilding the rows of GW.. DONE!\n");
  PURIFY_DEBUG("DONE - With {} entries non zero, which is {} entries per a row.", GW.nonZeros(),
               static_cast<t_real>(GW.nonZeros()) / GW.rows());
  return GW;
} // namespace wproj_utilities

t_real snr_metric(const Image<t_real> &model, const Image<t_real> &solution) {
  /*
     Returns SNR of the estimated model image
     */
  t_real nm = model.matrix().norm();
  t_real ndiff = (model - solution).matrix().norm();
  t_real val = 20 * std::log10(nm / ndiff);
  return val;
}
t_real mr_metric(const Image<t_real> &model, const Image<t_real> &solution) {
  /*
     Returns SNR of the estimated model image
     */
  t_int Npix = model.rows() * model.cols();
  Image<t_real> model_ = model.array() + 1e-10;
  Image<t_real> solution_ = solution.array() + 1e-10;
  Image<t_real> model_sol = model_.matrix().cwiseQuotient(solution_.matrix());
  Image<t_real> sol_model = solution_.matrix().cwiseQuotient(model_.matrix());
  Image<t_real> min_ratio = sol_model.matrix().cwiseMin(model_sol.matrix());
  t_real val = min_ratio.array().sum() / Npix;
  return val;
}

Sparse<t_complex>
generate_vect(const t_int &x_size, const t_int &y_size, const t_real &sigma, const t_real &mean) {

  // t_real sigma = 3;
  // const t_real pi = constant::pi;
  t_int W = x_size;
  Matrix<t_complex> chirp = Image<t_complex>::Zero(y_size, x_size);
  // t_real mean = 0;
  t_complex I(0, 1);
  t_real step = W / 2;

  // t_complex sum = 0.0; // For accumulating the kernel values
  for(int x = 0; x < W; ++x) {
    for(int y = 0; y < W; ++y) {
      t_int xx = x - step;
      xx = xx * xx;
      t_int yy = y - step;
      yy = yy * yy;
      t_complex val
          = exp(-0.5
                * (xx / (sigma * sigma)
                   + yy / (sigma * sigma))); //* std::exp(- 2 * pi * I * (x * 0.5 + y * 0.5));
      if(std::abs(val) > 1e-5)
        chirp(x, y) = val;
      else
        chirp(x, y) = 0.0;
    }
  }

  chirp.resize(x_size * y_size, 1);
  Sparse<t_complex> chirp_ = chirp.sparseView(1e-5, 1);
  return chirp_;
}

} // namespace wproj_utilities
} // namespace purify
