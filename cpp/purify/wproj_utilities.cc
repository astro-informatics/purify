#include "purify/wproj_utilities.h"

namespace purify {
namespace utilities {
vis_params sort_by_w(const vis_params &uv_data) {

  vis_params output = uv_data;
  std::vector<t_uint> ind;
  for(t_uint i = 0; i < uv_data.size(); i++)
    ind.push_back(i);
  std::sort(ind.begin(), ind.end(),
            [&](const t_uint a, const t_uint b) { return uv_data.w(a) < uv_data.w(b); });
  for(t_uint i = 0; i < uv_data.size(); i++) {
    output.u(i) = uv_data.u(ind.at(i));
    output.v(i) = uv_data.v(ind.at(i));
    output.w(i) = uv_data.w(ind.at(i));
    output.weights(i) = uv_data.weights(ind.at(i));
    output.vis(i) = uv_data.vis(ind.at(i));
  }
  return output;
}
} // namespace utilities
namespace wproj_utilities {
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
  auto primary_beam
      = [=](const t_real &x, const t_real &y) { return 1. / std::sqrt(1 - x * x - y * y); };
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

Sparse<t_complex>
wprojection_matrix(const Sparse<t_complex> &G, const t_uint &x_size, const t_uint &y_size,
                   const Vector<t_real> &w_components, const t_real &cell_x, const t_real &cell_y,
                   const t_real &energy_fraction_chirp, const t_real &energy_fraction_wproj,
                   const expansions::series series, const t_uint order,
                   const t_real &interpolation_error) {

  const t_uint Npix = x_size * y_size;
  const t_uint Nvis = w_components.size();

  PURIFY_HIGH_LOG("Spread of w components {} ",
                  std::sqrt(std::pow(w_components.norm(), 2) / Nvis
                            - std::pow(w_components.array().mean(), 2)));
  PURIFY_HIGH_LOG("Mean of w components {} ", w_components.array().mean());
  PURIFY_HIGH_LOG("Hard-thresholding of the chirp kernels: energy [{}] ", energy_fraction_chirp);
  PURIFY_HIGH_LOG("Hard-thresholding of the rows of G: energy [{}]  ", energy_fraction_wproj);

  const auto ft_plan = operators::fftw_plan::measure;
  const auto fftop_ = operators::init_FFT_2d<Vector<t_complex>>(y_size, x_size, 1., ft_plan);
  Sparse<t_complex> GW(Nvis, Npix);

  t_uint counts = 0;
  t_uint total_non_zero = 0;
  GW.reserve(G.nonZeros() * 1e3);
#pragma omp parallel for
  for(t_int m = 0; m < G.rows(); m++) {
    const Sparse<t_complex> chirp
        = create_chirp_row(w_components(m), cell_x, cell_y, x_size, y_size, energy_fraction_chirp,
                           std::get<0>(fftop_));

    Sparse<t_complex> kernel = row_wise_sparse_convolution(G.row(m), chirp, x_size, y_size);
    const t_real thres = sparsify_row_thres(kernel, energy_fraction_wproj);
    kernel.prune([&](const t_uint &i, const t_uint &j, const t_complex &value) {
      return std::abs(value) > thres;
    });
    assert(kernel.cols() > 0);
    assert(kernel.rows() == 1);
#pragma omp critical
    GW.row(m) = kernel;
#pragma omp critical
    counts++;
#pragma omp critical
    total_non_zero += kernel.nonZeros();
#pragma omp critical
    // if(counts % 100 == 0) {
    PURIFY_DEBUG("Row {} of {}", counts, GW.rows());
    PURIFY_DEBUG("With {} entries non zero, which is {} entries per a row.", total_non_zero,
                 static_cast<t_real>(total_non_zero) / counts);
    //}
  }
  assert(GW.nonZeros() > 0);
  PURIFY_DEBUG("\nBuilding the rows of GW.. DONE!\n");
  PURIFY_DEBUG("DONE - With {} entries non zero, which is {} entries per a row.", GW.nonZeros(),
               static_cast<t_real>(GW.nonZeros()) / GW.rows());
  GW.makeCompressed();
  return GW;
}

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
