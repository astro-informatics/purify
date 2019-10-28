#include "purify/utilities.h"
#include "purify/config.h"
#include <fstream>
#include <random>
#include <sys/stat.h>
#include "purify/logging.h"
#include "purify/operators.h"

namespace purify {
namespace utilities {

t_int sub2ind(const t_int &row, const t_int &col, const t_int &rows, const t_int &cols) {
  /*
    Converts (row, column) of a matrix to a single index. This does the same as the matlab funciton
    sub2ind, converts subscript to index.
    Though order of cols and rows is probably transposed.

    row:: row of matrix (y)
    col:: column of matrix (x)
    cols:: number of columns for matrix
    rows:: number of rows for matrix
   */
  return row * cols + col;
}

std::tuple<t_int, t_int> ind2sub(const t_int &sub, const t_int &cols, const t_int &rows) {
  /*
    Converts index of a matrix to (row, column). This does the same as the matlab funciton sub2ind,
    converts subscript to index.

    sub:: subscript of entry in matrix
    cols:: number of columns for matrix
    rows:: number of rows for matrix
    row:: output row of matrix
    col:: output column of matrix

   */
  return std::make_tuple<t_int, t_int>(std::floor((sub - (sub % cols)) / cols), sub % cols);
}

t_real mod(const t_real &x, const t_real &y) {
  /*
    returns x % y, and warps circularly around y for negative values.
  */
  t_real r = std::fmod(x, y);
  if (r < 0) r = y + r;
  return r;
}

Image<t_complex> convolution_operator(const Image<t_complex> &a, const Image<t_complex> &b) {
  /*
  returns the convolution of images a with images b
  a:: vector a, which is shifted
  b:: vector b, which is fixed
  */

  // size of a image
  t_int a_y = a.rows();
  t_int a_x = a.cols();
  // size of b image
  t_int b_y = b.rows();
  t_int b_x = b.cols();

  Image<t_complex> output = Image<t_complex>::Zero(a_y + b_y, a_x + b_x);

  for (t_int l = 0; l < b.cols(); ++l) {
    for (t_int k = 0; k < b.rows(); ++k) {
      output(k, l) = (a * b.block(k, l, a_y, a_x)).sum();
    }
  }

  return output;
}

t_real calculate_l2_radius(const t_uint y_size, const t_real &sigma, const t_real &n_sigma,
                           const std::string distirbution) {
  /*
                  Calculates the epsilon, the radius of the l2_ball in sopt
                  y:: vector for the l2 ball
  */
  if (distirbution == "chi^2") {
    if (sigma == 0) {
      return std::sqrt(2 * y_size + n_sigma * std::sqrt(4 * y_size));
    }
    return std::sqrt(2 * y_size + n_sigma * std::sqrt(4 * y_size)) * sigma;
  }
  if (distirbution == "chi") {
    auto alpha =
        1. / (8 * y_size) - 1. / (128 * y_size * y_size);  // series expansion for gamma relation
    auto mean = std::sqrt(2) * std::sqrt(y_size) *
                (1 - alpha);  // using Gamma(k+1/2)/Gamma(k) asymptotic formula
    auto standard_deviation = std::sqrt(2 * y_size * alpha * (2 - alpha));
    if (sigma == 0) {
      return mean + n_sigma * standard_deviation;
    }
    return (mean + n_sigma * standard_deviation) * sigma;
  }

  return 1.;
}
t_real SNR_to_standard_deviation(const Vector<t_complex> &y0, const t_real &SNR) {
  /*
  Returns value of noise rms given a measurement vector and signal to noise ratio
  y0:: complex valued vector before noise added
  SNR:: signal to noise ratio

  This calculation follows Carrillo et al. (2014), PURIFY a new approach to radio interferometric
  imaging but we have assumed that rms noise is for the real and imaginary parts
  */
  return y0.stableNorm() / std::sqrt(2 * y0.size()) * std::pow(10.0, -(SNR / 20.0));
}

Vector<t_complex> add_noise(const Vector<t_complex> &y0, const t_complex &mean,
                            const t_real &standard_deviation) {
  /*
          Adds complex valued Gaussian noise to vector
          y0:: vector before noise
          mean:: complex valued mean of noise
          standard_deviation:: rms of noise
  */
  auto sample = [&mean, &standard_deviation](t_complex x) {
    std::random_device rd_real;
    std::random_device rd_imag;
    std::mt19937_64 rng_real(rd_real());
    std::mt19937_64 rng_imag(rd_imag());
    static std::normal_distribution<t_real> normal_dist_real(std::real(mean), standard_deviation);
    static std::normal_distribution<t_real> normal_dist_imag(std::imag(mean), standard_deviation);
    t_complex I(0, 1.);
    return normal_dist_real(rng_real) + I * normal_dist_imag(rng_imag);
  };

  auto noise = Vector<t_complex>::Zero(y0.size()).unaryExpr(sample);

  return y0 + noise;
}

bool file_exists(const std::string &name) {
  /*
          Checks if a file exists
          name:: path of file checked for existance.

          returns true if exists and false if not exists
  */
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

std::tuple<t_real, t_real, t_real> fit_fwhm(const Image<t_real> &psf, const t_int &size) {
  /*
          Find FWHM of point spread function, using least squares.

          psf:: point spread function, assumed to be normalised.

          The method assumes that you have sampled at least
          3 pixels across the beam.
  */

  t_int x0 = std::floor(psf.cols() * 0.5);
  t_int y0 = std::floor(psf.rows() * 0.5);

  // finding patch
  Image<t_real> patch =
      psf.block(std::floor(y0 - size * 0.5) + 1, std::floor(x0 - size * 0.5) + 1, size, size);
  PURIFY_LOW_LOG("Fitting to Patch");

  // finding values for least squares

  std::vector<t_tripletList> entries;
  t_int total_entries = 0;

  for (t_int i = 0; i < patch.cols(); ++i) {
    for (t_int j = 0; j < patch.rows(); ++j) {
      entries.emplace_back(j, i, std::log(patch(j, i)));
      total_entries++;
    }
  }
  Matrix<t_real> A = Matrix<t_real>::Zero(total_entries, 4);
  Vector<t_real> q = Vector<t_real>::Zero(total_entries);
  // putting values into a vector and matrix for least squares
  for (t_int i = 0; i < total_entries; ++i) {
    t_real x = entries.at(i).col() - size * 0.5 + 0.5;
    t_real y = entries.at(i).row() - size * 0.5 + 0.5;
    A(i, 0) = static_cast<t_real>(x * x);  // x^2
    A(i, 1) = static_cast<t_real>(x * y);  // x * y
    A(i, 2) = static_cast<t_real>(y * y);  // y^2
    q(i) = std::real(entries.at(i).value());
  }
  // least squares fitting
  const auto solution =
      static_cast<Vector<t_real>>(A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(q));
  t_real const a = -solution(0);
  t_real const b = +solution(1) * 0.5;
  t_real const c = -solution(2);
  // parameters of Gaussian
  t_real theta = std::atan2(b, std::sqrt(std::pow(2 * b, 2) + std::pow(c - a, 2)) +
                                   (c - a) * 0.5);  // some relatively complicated trig identity to
                                                    // go from tan(2theta) to tan(theta).
  t_real t = 0.;
  t_real s = 0.;
  if (std::abs(b) < 1e-13) {
    t = a;
    s = c;
    theta = 0;
  } else {
    t = (a + c - 2 * b / std::sin(2 * theta)) * 0.5;
    s = (a + c + 2 * b / std::sin(2 * theta)) * 0.5;
  }

  t_real const sigma_x = std::sqrt(1 / (2 * t));
  t_real const sigma_y = std::sqrt(1 / (2 * s));

  std::tuple<t_real, t_real, t_real> fit_parameters;

  // fit for the beam rms, used for FWHM
  std::get<0>(fit_parameters) = sigma_x;
  std::get<1>(fit_parameters) = sigma_y;
  std::get<2>(fit_parameters) = theta;  // because 2*theta is periodic with pi.
  return fit_parameters;
}

t_real median(const Vector<t_real> &input) {
  // Finds the median of a real vector x
  auto size = input.size();
  Vector<t_real> x(size);
  std::copy(input.data(), input.data() + size, x.data());
  std::sort(x.data(), x.data() + size);
  if (std::floor(size / 2) - std::ceil(size / 2) == 0)
    return (x(std::floor(size / 2) - 1) + x(std::floor(size / 2))) / 2;
  return x(std::ceil(size / 2));
}

t_real dynamic_range(const Image<t_complex> &model, const Image<t_complex> &residuals,
                     const t_real &operator_norm) {
  /*
  Returns value of noise rms given a measurement vector and signal to noise ratio
  y0:: complex valued vector before noise added
  SNR:: signal to noise ratio

  This calculation follows Carrillo et al. (2014), PURIFY a new approach to radio interferometric
  imaging
  */
  return std::sqrt(model.size()) * (operator_norm * operator_norm) / residuals.matrix().norm() *
         model.cwiseAbs().maxCoeff();
}

Array<t_complex> init_weights(const Vector<t_real> &u, const Vector<t_real> &v,
                              const Vector<t_complex> &weights, const t_real &oversample_factor,
                              const std::string &weighting_type, const t_real &R,
                              const t_int &ftsizeu, const t_int &ftsizev) {
  /*
    Calculate the weights to be applied to the visibilities in the measurement operator.
    It does none, whiten, natural, uniform, and robust.
  */
  if (weighting_type == "none") return weights.array() * 0 + 1.;
  if (weighting_type == "natural" or weighting_type == "whiten") return weights;
  Vector<t_complex> out_weights(weights.size());
  if ((weighting_type == "uniform") or (weighting_type == "robust")) {
    t_real scale =
        1. / oversample_factor;  // scale for fov, controlling the region of sidelobe supression
    Matrix<t_complex> gridded_weights = Matrix<t_complex>::Zero(ftsizev, ftsizeu);
    for (t_int i = 0; i < weights.size(); ++i) {
      t_int q = utilities::mod(floor(u(i) * scale), ftsizeu);
      t_int p = utilities::mod(floor(v(i) * scale), ftsizev);
      gridded_weights(p, q) += 1 * 1;  // I get better results assuming all the weights are the
                                       // same. It looks like miriad does this as well.
    }
    t_complex const sum_weights = (weights.array() * weights.array()).sum();
    t_complex const sum_grid_weights2 = (gridded_weights.array() * gridded_weights.array()).sum();
    t_complex const robust_scale =
        sum_weights / sum_grid_weights2 * 25. *
        std::pow(10, -2 * R);  // Following standard formula, a bit different from miriad.
    gridded_weights = gridded_weights.array().sqrt();
    for (t_int i = 0; i < weights.size(); ++i) {
      t_int q = utilities::mod(floor(u(i) * scale), ftsizeu);
      t_int p = utilities::mod(floor(v(i) * scale), ftsizev);
      if (weighting_type == "uniform") out_weights(i) = weights(i) / gridded_weights(p, q);
      if (weighting_type == "robust") {
        out_weights(i) = weights(i) / std::sqrt(1. + robust_scale * gridded_weights(p, q) *
                                                         gridded_weights(p, q));
      }
    }
  } else
    throw std::runtime_error("Wrong weighting type, " + weighting_type + " not recognised.");
  return out_weights;
}

std::tuple<t_int, t_real> checkpoint_log(const std::string &diagnostic) {
  // reads a log file and returns the latest parameters
  if (!utilities::file_exists(diagnostic)) return std::make_tuple(0, 0);

  t_int iters = 0;
  t_real gamma = 0;
  std::ifstream log_file(diagnostic);
  std::string entry;
  std::string s;

  std::getline(log_file, s);

  while (log_file) {
    if (!std::getline(log_file, s)) break;
    std::istringstream ss(s);
    std::getline(ss, entry, ' ');
    iters = std::floor(std::stod(entry));
    std::getline(ss, entry, ' ');
    gamma = std::stod(entry);
  }
  log_file.close();
  return std::make_tuple(iters, gamma);
}

Matrix<t_complex> re_sample_ft_grid(const Matrix<t_complex> &input, const t_real &re_sample_ratio) {
  /*
    up samples image using by zero padding the fft

    input:: fft to be upsampled, with zero frequency at (0,0) of the matrix.

  */
  if (re_sample_ratio == 1) return input;

  // sets up dimensions for old image
  t_int old_x_centre_floor = std::floor(input.cols() * 0.5);
  t_int old_y_centre_floor = std::floor(input.rows() * 0.5);
  // need ceilling in case image is of odd dimension
  t_int old_x_centre_ceil = std::ceil(input.cols() * 0.5);
  t_int old_y_centre_ceil = std::ceil(input.rows() * 0.5);

  // sets up dimensions for new image
  t_int new_x = std::floor(input.cols() * re_sample_ratio);
  t_int new_y = std::floor(input.rows() * re_sample_ratio);

  t_int new_x_centre_floor = std::floor(new_x * 0.5);
  t_int new_y_centre_floor = std::floor(new_y * 0.5);
  // need ceilling in case image is of odd dimension
  t_int new_x_centre_ceil = std::ceil(new_x * 0.5);
  t_int new_y_centre_ceil = std::ceil(new_y * 0.5);

  Matrix<t_complex> output = Matrix<t_complex>::Zero(new_y, new_x);

  t_int box_dim_x;
  t_int box_dim_y;

  // now have to move each quadrant into new grid
  box_dim_x = std::min(old_x_centre_ceil, new_x_centre_ceil);
  box_dim_y = std::min(old_y_centre_ceil, new_y_centre_ceil);
  //(0, 0)
  output.bottomLeftCorner(box_dim_y, box_dim_x) = input.bottomLeftCorner(box_dim_y, box_dim_x);

  box_dim_x = std::min(old_x_centre_ceil, new_x_centre_ceil);
  box_dim_y = std::min(old_y_centre_floor, new_y_centre_floor);
  //(y0, 0)
  output.topLeftCorner(box_dim_y, box_dim_x) = input.topLeftCorner(box_dim_y, box_dim_x);

  box_dim_x = std::min(old_x_centre_floor, new_x_centre_floor);
  box_dim_y = std::min(old_y_centre_ceil, new_y_centre_ceil);
  //(0, x0)
  output.bottomRightCorner(box_dim_y, box_dim_x) = input.bottomRightCorner(box_dim_y, box_dim_x);

  box_dim_x = std::min(old_x_centre_floor, new_x_centre_floor);
  box_dim_y = std::min(old_y_centre_floor, new_y_centre_floor);
  //(y0, x0)
  output.topRightCorner(box_dim_y, box_dim_x) = input.topRightCorner(box_dim_y, box_dim_x);

  return output;
}
Matrix<t_complex> re_sample_image(const Matrix<t_complex> &input, const t_real &re_sample_ratio) {
  const auto ft_plan = operators::fftw_plan::estimate;
  auto fft =
      purify::operators::init_FFT_2d<Vector<t_complex>>(input.rows(), input.cols(), 1., ft_plan);
  Vector<t_complex> ft_grid = Vector<t_complex>::Zero(input.size());
  std::get<0>(fft)(ft_grid, Vector<t_complex>::Map(input.data(), input.size()));
  Matrix<t_complex> const new_ft_grid = utilities::re_sample_ft_grid(
      Matrix<t_complex>::Map(ft_grid.data(), input.rows(), input.cols()), re_sample_ratio);
  Vector<t_complex> output = Vector<t_complex>::Zero(input.size());
  fft = purify::operators::init_FFT_2d<Vector<t_complex>>(new_ft_grid.rows(), new_ft_grid.cols(),
                                                          1., ft_plan);
  std::get<1>(fft)(output, Vector<t_complex>::Map(new_ft_grid.data(), new_ft_grid.size()));
  return Matrix<t_complex>::Map(output.data(), new_ft_grid.rows(), new_ft_grid.cols()) *
         re_sample_ratio;
}
}  // namespace utilities
}  // namespace purify
