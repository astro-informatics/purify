#include "purify/wproj_utilities.h"

#define EIGEN_DONT_PARALLELIZE
#define EIGEN_NO_AUTOMATIC_RESIZING
namespace purify {
namespace wproj_utilities {
Vector<t_real> scale_w(Vector<t_real> const &w, const t_real &n_max) {
  /*
   *  Calculate the scale for the w plane grid based on the field of view
   *  Assume that w is in kilo lambda, convert to radians.
   */

  t_real const w_fwhm = 2 * 1.895 / (constant::pi * n_max);
  return 2 * w / w_fwhm * 2 * constant::pi;
}
std::tuple<t_real, t_real> fov_cosines(t_real const &cell_x, t_real const &cell_y,
                                       t_uint const &x_size, t_uint const &y_size) {
  // celly:: size of y pixel in arcseconds
  // cellx:: size of x pixel in arcseconds
  // x_size:: number of pixels along x-axis
  // y_size:: number of pixels along y-axis
  const t_real pi = constant::pi;
  const t_real theta_FoV_L = cell_x * x_size;
  const t_real theta_FoV_M = cell_y * y_size;
  const t_real L = 2 * std::sin(pi / 180. * theta_FoV_L / (60. * 60.) * 0.5);
  const t_real M = 2 * std::sin(pi / 180. * theta_FoV_M / (60. * 60.) * 0.5);
  return std::make_tuple(L, M);
}

Matrix<t_complex> generate_dde(const std::function<t_complex(t_real, t_real)> &dde,
                               const t_real &cell_x, const t_real &cell_y, const t_uint &x_size,
                               const t_uint &y_size) {

  // celly:: size of y pixel in arcseconds
  // cellx:: size of x pixel in arcseconds
  // x_size:: number of pixels along x-axis
  // y_size:: number of pixels along y-axis
  auto const LM = fov_cosines(cell_x, cell_y, x_size, y_size);

  const t_real L = std::get<0>(LM);
  const t_real M = std::get<1>(LM);

  const t_real delt_x = L / x_size;
  const t_real delt_y = M / y_size;
  Image<t_complex> chirp(y_size, x_size);

  for(t_int l = 0; l < x_size; ++l)
    for(t_int m = 0; m < y_size; ++m) {
      const t_real x = (l + 0.5 - x_size * 0.5) * delt_x;
      const t_real y = (m + 0.5 - y_size * 0.5) * delt_y;
      chirp(l, m) = dde(y, x);
    }

  return chirp;
}
Matrix<t_complex> generate_chirp(const std::function<t_complex(t_real, t_real)> &dde,
                                 const t_real &w_rate, const t_real &cell_x, const t_real &cell_y,
                                 const t_uint &x_size, const t_uint &y_size) {

  // return chirp image fourier transform for w component
  // w:: w-term in units of lambda
  // celly:: size of y pixel in arcseconds
  // cellx:: size of x pixel in arcseconds
  // x_size:: number of pixels along x-axis
  // y_size:: number of pixels along y-axis
  const t_real nz = y_size * x_size;
  const t_complex I(0, 1);
  const t_real pi = constant::pi;
  const auto chirp = [=](const t_real &y, const t_real &x) {
    return dde(y, x) * (std::exp(-2 * pi * I * w_rate * (std::sqrt(1 - x * x - y * y) - 1))) / nz;
  };
  const auto chirp_approx = [=](const t_real &y, const t_real &x) {
    return dde(y, x) * (std::exp(2 * pi * I * w_rate * (x * x + y * y))) / nz;
  };
  return generate_dde(chirp, cell_x, cell_y, x_size, y_size);
}
Matrix<t_complex> generate_chirp(const t_real &w_rate, const t_real &cell_x, const t_real &cell_y,
                                 const t_uint &x_size, const t_uint &y_size) {

  // return chirp image fourier transform for w component
  // w:: w-term in units of lambda
  // celly:: size of y pixel in arcseconds
  // cellx:: size of x pixel in arcseconds
  // x_size:: number of pixels along x-axis
  // y_size:: number of pixels along y-axis
  return generate_chirp([](const t_real &, const t_real &) { return 1.; }, w_rate, cell_x, cell_y,
                        x_size, y_size);
}
Sparse<t_complex> create_chirp_row(const t_real &w_rate, const t_real &cell_x, const t_real &cell_y,
                                   const t_uint &ftsizev, const t_uint &ftsizeu,
                                   const t_real &energy_fraction,
                                   const sopt::OperatorFunction<Vector<t_complex>> &fftop) {

  const Matrix<t_complex> chirp
      = wproj_utilities::generate_chirp(w_rate, cell_x, cell_y, ftsizeu, ftsizev);
  return create_chirp_row(Vector<t_complex>::Map(chirp.data(), chirp.size()), energy_fraction,
                          fftop);
}
Sparse<t_complex> create_chirp_row(const Vector<t_complex> &chirp_image,
                                   const t_real &energy_fraction,
                                   const sopt::OperatorFunction<Vector<t_complex>> &fftop) {
  Vector<t_complex> chirp;
#pragma omp critical(fft)
  fftop(chirp, chirp_image);
  chirp *= std::sqrt(chirp.size());
  const t_real thres = wproj_utilities::sparsify_row_dense_thres(chirp, energy_fraction);
  assert(chirp.norm() > 0);
  const t_real row_max = chirp.cwiseAbs().maxCoeff();
  return convert_sparse(chirp, thres).transpose();
}
std::vector<t_uint> w_rows(const Sparse<t_complex> &w_degrider) {

  std::set<t_uint> w_rows_set;
  for(t_int k = 0; k < w_degrider.outerSize(); ++k) {
    t_uint index = 0;
    for(Sparse<t_complex>::InnerIterator it(w_degrider, k); it; ++it) {
      index++;
      if(index > 0) {
        w_rows_set.insert(it.col());
      }
    }
  }
  std::vector<t_uint> w_rows(w_rows_set.begin(), w_rows_set.end());
  std::sort(w_rows.begin(), w_rows.end());
  return w_rows;
}

Vector<t_real> w_range(const t_real &w_cell, const t_real &w_max) {

  if(w_cell > w_max)
    throw std::runtime_error("FoV is smaller than a pixel.");
  // const t_real w_fwhm = 2 * 1.895 / (constant::pi * n_max);
  // const t_real w_max = 2 * 1.895 / (constant::pi * n_fwhm);
  t_int const size = std::floor(w_max / w_cell);
  Vector<t_real> range = Vector<t_real>::Zero(2 * size + 1);
  for(int i = 0; i < size + 1; i++)
    range(i) = i * w_cell;
  for(int i = size + 1; i < 2 * size + 1; i++)
    range(i) = (i - 1 - 2 * size) * w_cell;

  PURIFY_HIGH_LOG("Chirp kernel grid has size {} ", range.size());
  PURIFY_HIGH_LOG("Chirp kernel grid cell size {} ", w_cell);
  PURIFY_HIGH_LOG("Chirp kernel grid has [{},{}] ", range.minCoeff(), range.maxCoeff());
  return range;
}

t_complex ft_kernel(const t_real &y, const t_real &x, const t_real &n_max, const t_uint &J) {
  t_real const u = (1 - std::sqrt(1 - x * x - y * y)) / n_max;
  return 1. / kernels::ft_kaiser_bessel(u, J);
  // return 1.;
}

Sparse<t_complex> w_plane_grid(const t_uint &x_size, const t_uint &y_size, const t_real &cell_x,
                               const t_real &cell_y, const t_real &energy_fraction_chirp,
                               const t_uint &J, const std::vector<t_uint> &w_rows,
                               const t_real &w_cell, const t_real &w_max) {

  // I assume that w_components is sorted and the grid points
  PURIFY_HIGH_LOG("Hard-thresholding of the Chirp kernels: energy [{}] ", energy_fraction_chirp);
  // Need to scale this in terms of directional cosines x, y
  auto const LM = fov_cosines(cell_x, cell_y, x_size, y_size);

  const t_real L = std::get<0>(LM) * 0.5;
  const t_real M = std::get<1>(LM) * 0.5;
  const t_real n_max = (1 - std::sqrt(1 - L * L - M * M));
  const auto grid_correction = [=](const t_real &y, const t_real &x) -> t_complex {
    return 1. / ft_kernel(y, x, n_max, J) / std::sqrt(1 - L * L - M * M);
  };
  Vector<t_real> const w_grid_coords = w_range(w_cell, w_max);
  const auto ft_plan = operators::fftw_plan::measure;
  const auto fftop_ = operators::init_FFT_2d<Vector<t_complex>>(y_size, x_size, 1., ft_plan);
  Sparse<t_complex> W(w_grid_coords.size(), x_size * y_size);
  t_uint counts = 0;
  t_uint total_non_zero = 0;
  PURIFY_HIGH_LOG("Creating {} planes ", w_rows.size());
#pragma omp parallel for
  for(t_int m = 0; m < w_rows.size(); m++) {
    const t_real w = w_grid_coords(w_rows.at(m));
    const Sparse<t_complex> chirp = create_chirp_row(
        Vector<t_complex>::Map(
            generate_chirp(grid_correction, w, cell_x, cell_y, x_size, y_size).data(),
            x_size * y_size),
        energy_fraction_chirp, std::get<0>(fftop_));
#pragma omp critical
    W.row(w_rows.at(m)) = chirp;
#pragma omp critical
    counts++;
#pragma omp critical
    total_non_zero += chirp.nonZeros();
#pragma omp critical
    {
      if(counts % 100 == 0) {
        PURIFY_DEBUG("Row {} of {}", counts, W.rows());
        PURIFY_DEBUG("With {} entries non zero, which is {} entries per a row.", total_non_zero,
                     static_cast<t_real>(total_non_zero) / counts);
      }
    }
  }
  assert(W.nonZeros() > 0);
  PURIFY_DEBUG("\nBuilding the rows of W grid.. DONE!\n");
  PURIFY_DEBUG("DONE - With {} entries non zero, which is {} entries per a row.", W.nonZeros(),
               static_cast<t_real>(W.nonZeros()) / W.rows());
  return W;
} // namespace wproj_utilities

Sparse<t_complex> w_plane_degrid_matrix(const Vector<t_real> &w_range,
                                        const Vector<t_real> &w_components, const t_uint &J) {
  t_uint const rows = w_components.size();
  auto omega_to_k = [](const Vector<t_real> &omega) {
    return omega.unaryExpr(std::ptr_fun<double, double>(std::floor));
  };
  const t_uint grid_size = w_range.size();
  Vector<t_real> w_mod = w_components / w_range.maxCoeff() * grid_size * 0.5;
  for(t_uint m = 0; m < w_components.size(); m++)
    w_mod(m) = utilities::mod(w_mod(m), grid_size);
  const Vector<t_real> k_w = omega_to_k(w_mod - Vector<t_real>::Constant(w_mod.size(), J * 0.5));
  const auto kernel = [=](const t_real &w) { return kernels::kaiser_bessel(w, J); };

  Sparse<t_complex> w_degrid(rows, grid_size);
  w_degrid.reserve(Vector<t_int>::Constant(w_degrid.rows(), J));

  const t_complex I(0, 1);
  const t_int j_max = std::min(J + 1, grid_size + 1);
  //#pragma omp parallel for collapse(2)
  for(t_int m = 0; m < rows; ++m) {
    for(t_int j = 1; j < j_max; ++j) {
      const t_uint q = utilities::mod(k_w(m) + j, grid_size);
      w_degrid.coeffRef(m, q) = kernel(w_mod(m) - (k_w(m) + j));
    }
  }
  return w_degrid;
}

Sparse<t_complex>
wprojection_matrix(const Sparse<t_complex> &G, const t_uint &x_size, const t_uint &y_size,
                   const Vector<t_real> &w_components, const t_real &cell_x, const t_real &cell_y,
                   const t_real &energy_fraction_chirp, const t_real &energy_fraction_wproj) {

  const t_uint Npix = x_size * y_size;
  const t_uint Nvis = w_components.size();

  PURIFY_HIGH_LOG("Hard-thresholding of the Chirp kernels: energy [{}] ", energy_fraction_chirp);
  PURIFY_HIGH_LOG("Hard-thresholding of the rows of G: energy [{}]  ", energy_fraction_wproj);

  const auto ft_plan = operators::fftw_plan::measure;
  const auto fftop_ = operators::init_FFT_2d<Vector<t_complex>>(y_size, x_size, 1., ft_plan);
  Sparse<t_complex> GW(Nvis, Npix);
  t_uint counts = 0;
  t_uint total_non_zero = 0;
#pragma omp parallel for
  for(t_int m = 0; m < G.rows(); m++) {
    Sparse<t_complex> row(1, G.cols());
    row.reserve(G.row(m).nonZeros());
    for(Sparse<t_complex>::InnerIterator it(G, m); it; ++it)
      row.coeffRef(0, it.index()) = it.value();
    const Sparse<t_complex> chirp
        = create_chirp_row(w_components(m), cell_x, cell_y, x_size, y_size, energy_fraction_chirp,
                           std::get<0>(fftop_));
    Sparse<t_complex> kernel = row_wise_sparse_convolution(row, chirp, x_size, y_size);
    //#pragma omp critical
    //    assert(kernel.nonZeros() >= row.nonZeros());
    const t_real thres = sparsify_row_thres(kernel, energy_fraction_wproj);
    kernel.prune([&](const t_uint &i, const t_uint &j, const t_complex &value) {
      return std::abs(value) > thres;
    });
    assert(kernel.cols() > 0);
    assert(kernel.rows() == 1);
    assert(kernel.nonZeros() >= row.nonZeros());
#pragma omp critical
    GW.row(m) = kernel;
#pragma omp critical
    counts++;
#pragma omp critical
    total_non_zero += kernel.nonZeros();
    if(counts % 100 == 0) {
      PURIFY_DEBUG("Row {} of {}", counts, GW.rows());
      PURIFY_DEBUG("With {} entries non zero, which is {} entries per a row.", total_non_zero,
                   static_cast<t_real>(total_non_zero) / counts);
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
generate_vect(const t_uint &x_size, const t_uint &y_size, const t_real &sigma, const t_real &mean) {

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
