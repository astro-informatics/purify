
#include "purify/wproj_grid.h"
#include <set>
#include "purify/wproj_utilities.h"
namespace purify {

namespace wproj_utilities {
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

std::vector<Sparse<t_complex>>
w_expansion(const t_uint &x_size, const t_uint &y_size, const t_real &cell_x, const t_real &cell_y,
            const t_real &energy_fraction_chirp, const std::vector<t_uint> &w_rows,
            const t_real &w_cell, const t_real &w_max,
            const std::vector<std::function<t_complex(t_real, t_real)>> &a_n) {

  // I assume that w_components is sorted and the grid points
  PURIFY_HIGH_LOG("Hard-thresholding of the Chirp kernels: energy [{}] ", energy_fraction_chirp);
  // Need to scale this in terms of directional cosines x, y
  auto const LM = fov_cosines(cell_x, cell_y, x_size, y_size);

  const t_real L = std::get<0>(LM) * 0.5;
  const t_real M = std::get<1>(LM) * 0.5;
  const t_real n_max = (1 - std::sqrt(1 - L * L - M * M));
  Vector<t_real> const w_grid_coords = w_range(w_cell, w_max);
  const auto ft_plan = operators::fftw_plan::measure;
  const auto fftop_ = operators::init_FFT_2d<Vector<t_complex>>(y_size, x_size, 1., ft_plan);
  std::vector<Sparse<t_complex>> W_expansion;
  PURIFY_HIGH_LOG("Performing expansion of order {} on {} planes ", a_n.size() - 1, w_rows.size());
  for(auto a_coeff : a_n) {
    Sparse<t_complex> W(w_grid_coords.size(), x_size * y_size);
    t_uint counts = 0;
    t_uint total_non_zero = 0;
    PURIFY_HIGH_LOG("Creating {} planes", w_rows.size());
#pragma omp parallel for
    for(t_int m = 0; m < w_rows.size(); m++) {
      const t_real w = w_grid_coords(w_rows.at(m));
      const Sparse<t_complex> chirp = create_chirp_row(
          Vector<t_complex>::Map(generate_chirp(a_coeff, w, cell_x, cell_y, x_size, y_size).data(),
                                 x_size * y_size),
          energy_fraction_chirp, std::get<0>(fftop_));
#pragma omp critical
      W.row(w_rows.at(m)) = chirp;
#pragma omp critical
      counts++;
#pragma omp critical
      total_non_zero += chirp.nonZeros();
      if(counts % 100 == 0) {
        PURIFY_DEBUG("Row {} of {}", counts, W.rows());
        PURIFY_DEBUG("With {} entries non zero, which is {} entries per a row.", total_non_zero,
                     static_cast<t_real>(total_non_zero) / counts);
      }
    }
    W.makeCompressed();
    W_expansion.push_back(W);

    assert(W.nonZeros() > 0);
    PURIFY_DEBUG("\nBuilding the rows of W grid.. DONE!\n");
    PURIFY_DEBUG("DONE - With {} entries non zero, which is {} entries per a row.", W.nonZeros(),
                 static_cast<t_real>(W.nonZeros()) / W.rows());
  }
  if(W_expansion.size() != a_n.size())
    throw std::runtime_error("Expansion size has the wrong number of terms. It could be that high "
                             "order expansion coefficients are very small.");
  return W_expansion;
}

Sparse<t_complex> w_projection_expansion(const std::vector<Sparse<t_complex>> &plane_expansion,
                                         const Vector<t_real> &w_components,
                                         const Vector<t_real> &w_grid_coords,
                                         const std::vector<t_uint> &w_rows,
                                         const std::vector<std::function<t_complex(t_real)>> &f_w) {

  if(f_w.size() != plane_expansion.size())
    throw std::runtime_error("Number of planes in expansion not the same as order of expansion.");
  const t_real w_max = w_grid_coords.maxCoeff();
  const t_real w_cell = w_grid_coords(1) - w_grid_coords(0);
  t_uint const rows = w_components.size();
  t_uint const cols = plane_expansion.at(0).cols();
  auto omega_to_k = [](const Vector<t_real> &omega) {
    return omega.unaryExpr(std::ptr_fun<double, double>(std::floor));
  };
  const t_uint grid_size = w_grid_coords.size();
  Vector<t_real> w_mod = w_components / w_max * grid_size * 0.5;
  for(t_uint m = 0; m < w_components.size(); m++)
    w_mod(m) = utilities::mod(w_mod(m), grid_size);
  const Vector<t_real> k_w = omega_to_k(w_mod - Vector<t_real>::Constant(w_mod.size(), 0.5));
  Sparse<t_complex> W(rows, cols);
  t_uint counts = 0;
  t_uint total_non_zero = 0;
#pragma omp parallel for
  for(t_int m = 0; m < w_components.size(); m++) {
    const t_real w_delta = (w_mod(m) - k_w(m)) / grid_size * w_max;
    if(w_cell < std::abs(w_delta)) {
#pragma omp critical
      std::cout << w_delta << " > " << w_cell << std::endl;
      throw std::runtime_error("w_delta size is larger than w cell size!");
    }
    const t_real row_index = w_rows.at(m);
    Sparse<t_complex> chirp = plane_expansion.at(0).row(row_index) * f_w.at(0)(w_delta);
    for(t_uint p = 1; p < f_w.size(); p++)
      chirp += plane_expansion.at(p).row(row_index) * f_w.at(p)(w_delta);
#pragma omp critical
    W.row(m) = chirp;
#pragma omp critical
    counts++;
#pragma omp critical
    total_non_zero += chirp.nonZeros();
    if(counts % 100 == 0) {
      PURIFY_DEBUG("Row {} of {}", counts, W.rows());
      PURIFY_DEBUG("With {} entries non zero, which is {} entries per a row.", total_non_zero,
                   static_cast<t_real>(total_non_zero) / counts);
    }
  }

  assert(W.nonZeros() > 0);
  PURIFY_DEBUG("\nBuilding the rows of W grid.. DONE!\n");
  PURIFY_DEBUG("DONE - With {} entries non zero, which is {} entries per a row.", W.nonZeros(),
               static_cast<t_real>(W.nonZeros()) / W.rows());
  return W;
}

const std::vector<t_uint>
w_rows(const Vector<t_real> &w_components, const Vector<t_real> &w_grid_coords) {

  t_uint const rows = w_components.size();
  auto omega_to_k = [](const Vector<t_real> &omega) -> std::vector<t_uint> {
    std::vector<t_uint> out;
    for(t_uint i = 0; i < omega.size(); i++)
      out.push_back(std::floor(omega(i)));
    return out;
  };
  const t_uint grid_size = w_grid_coords.size();
  Vector<t_real> w_mod = w_components / w_grid_coords.maxCoeff() * grid_size * 0.5;
  for(t_uint m = 0; m < w_components.size(); m++)
    w_mod(m) = utilities::mod(w_mod(m), grid_size);
  return omega_to_k(w_mod - Vector<t_real>::Constant(w_mod.size(), 0.5));
}

namespace expansions {

std::tuple<std::vector<std::function<t_complex(t_real, t_real)>>,
           std::vector<std::function<t_complex(t_real)>>>
taylor(const t_uint order) {
  std::vector<std::function<t_complex(t_real, t_real)>> a_n;
  std::vector<std::function<t_complex(t_real)>> f_w;
  const t_complex I(0, 1);
  for(int i = 0; i < order + 1; i++) {
    a_n.push_back([=](const t_real &y, const t_real &x) -> t_complex {
      // There may be a faster way to calculate the factorials...
      if(i < 1)
        return 1.;
      t_real const n = (std::sqrt(1 - x * x - y * y) - 1);
      t_complex z = -2 * constant::pi * I * n;
      for(t_real p = 2; p < i; p++) {
        z *= z / p;
      }
      return z;
    });
    f_w.push_back([=](const t_real &w) -> t_complex { return std::pow(w, i); });
  }
  return std::make_tuple(a_n, f_w);
}
std::tuple<std::vector<std::function<t_complex(t_real, t_real)>>,
           std::vector<std::function<t_complex(t_real)>>>
chebyshev(const t_uint order) {
  throw std::runtime_error("Need to impliment.");
  std::vector<std::function<t_complex(t_real, t_real)>> a_n;
  std::vector<std::function<t_complex(t_real)>> f_w;
  const t_complex I(0, 1);
  for(int i = 0; i < order + 1; i++) {
    a_n.push_back([=](const t_real &y, const t_real &x) -> t_complex { return 1.; });
    f_w.push_back([=](const t_real &w) -> t_complex { return 1.; });
  }
  return std::make_tuple(a_n, f_w);
}

} // namespace expansions

} // namespace wproj_utilities
} // namespace purify
