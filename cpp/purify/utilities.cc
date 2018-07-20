#include "purify/utilities.h"
#include "purify/operators.h"
#include "purify/config.h"
#include <fstream>
#include <random>
#include <sys/stat.h>
#include "purify/logging.h"

namespace purify {
namespace utilities {
Matrix<t_real> generate_antennas(const t_uint N) {
  Matrix<t_real> B = Matrix<t_real>::Zero(N, 3);
  const t_real mean = 0;
  const t_real standard_deviation = constant::pi;
  auto sample = [&mean, &standard_deviation]() {
    std::random_device rd;
    std::mt19937_64 rng(rd());
    t_real output = 4 * standard_deviation + mean;
    static std::normal_distribution<> normal_dist(mean, standard_deviation);
    // ensures that all sample points are within bounds
    while(std::abs(output - mean) > 3 * standard_deviation) {
      output = normal_dist(rng);
    }
    if(output != output)
      PURIFY_DEBUG("New random-sample density: {}", output);
    return output;
  };
  for(t_uint i = 0; i < N; i++) {
    B(i, 0) = sample();
    B(i, 1) = sample();
    B(i, 2) = sample();
  }
  return B;
}

utilities::vis_params antenna_to_coverage(const t_uint N) {
  return antenna_to_coverage(generate_antennas(N));
}

utilities::vis_params antenna_to_coverage(const Matrix<t_real> &B) {
  if(B.cols() != 3)
    throw std::runtime_error("Antennae coordinates are not 3D vectors.");
  const t_uint M = B.rows() * (B.rows() - 1) * 0.5;
  Vector<t_real> u = Vector<t_real>::Zero(M);
  Vector<t_real> v = Vector<t_real>::Zero(M);
  Vector<t_real> w = Vector<t_real>::Zero(M);
  Vector<t_complex> weights = Vector<t_complex>::Ones(M);
  Vector<t_complex> vis = Vector<t_complex>::Ones(M);
  t_uint m = 0;
  for(t_uint i = 0; i < B.rows(); i++) {
    for(t_uint j = i + 1; j < B.rows(); j++) {
      const Vector<t_real> r = B.row(i) - B.row(j);
      u(m) = r(0);
      v(m) = r(1);
      w(m) = r(2);
      m++;
    }
  }
  if(M != m)
    throw std::runtime_error(
        "Number of created baselines does not match expected baseline number N * (N - 1) / 2.");
  utilities::vis_params coverage(u, v, w, vis, weights, utilities::vis_units::radians);
  return coverage;
};
utilities::vis_params random_sample_density(const t_int &vis_num, const t_real &mean,
                                            const t_real &standard_deviation, const t_real &max_w) {
  /*
          Generates a random sampling density for visibility coverage
          vis_num:: number of visibilities
          mean:: mean of distribution
          standard_deviation:: standard deviation of distirbution
  */
  auto sample = [&mean, &standard_deviation]() {
    std::random_device rd;
    std::mt19937_64 rng(rd());
    t_real output = 4 * standard_deviation + mean;
    static std::normal_distribution<> normal_dist(mean, standard_deviation);
    // ensures that all sample points are within bounds
    while(std::abs(output - mean) > 3 * standard_deviation) {
      output = normal_dist(rng);
    }
    if(output != output)
      PURIFY_DEBUG("New random-sample density: {}", output);
    return output;
  };

  utilities::vis_params uv_vis;
  uv_vis.u = Vector<t_real>::Zero(vis_num);
  uv_vis.v = Vector<t_real>::Zero(vis_num);
  uv_vis.w = Vector<t_real>::Zero(vis_num);
  //#pragma omp parallel for
  for(t_uint i = 0; i < vis_num; i++) {
    uv_vis.u(i) = sample();
    uv_vis.v(i) = sample();
    uv_vis.w(i) = sample();
  }
  uv_vis.w = uv_vis.w / uv_vis.w.maxCoeff() * max_w;
  uv_vis.weights = Vector<t_complex>::Constant(vis_num, 1);
  uv_vis.vis = Vector<t_complex>::Constant(vis_num, 1);
  uv_vis.ra = 0;
  uv_vis.dec = 0;
  uv_vis.average_frequency = 0;
  return uv_vis;
}

utilities::vis_params read_visibility(const std::vector<std::string> &names, const bool w_term) {
  utilities::vis_params output = read_visibility(names.at(0), w_term);
  if(names.size() == 1)
    return output;
  for(int i = 1; i < names.size(); i++)
    output = read_visibility(names.at(i), output);
  return output;
}
utilities::vis_params
read_visibility(const std::string &vis_name2, const utilities::vis_params &uv1) {
  const bool w_term = not uv1.w.isZero(0);

  const auto uv2 = read_visibility(vis_name2, w_term);
  utilities::vis_params uv;
  uv.u = Vector<t_real>::Zero(uv1.size() + uv2.size());
  uv.v = Vector<t_real>::Zero(uv1.size() + uv2.size());
  uv.w = Vector<t_real>::Zero(uv1.size() + uv2.size());
  uv.vis = Vector<t_complex>::Zero(uv1.size() + uv2.size());
  uv.weights = Vector<t_complex>::Zero(uv1.size() + uv2.size());
  uv.u.segment(0, uv1.size()) = uv1.u;
  uv.v.segment(0, uv1.size()) = uv1.v;
  uv.w.segment(0, uv1.size()) = uv1.w;
  uv.vis.segment(0, uv1.size()) = uv1.vis;
  uv.weights.segment(0, uv1.size()) = uv1.weights;
  uv.u.segment(uv1.size(), uv2.size()) = uv2.u;
  uv.v.segment(uv1.size(), uv2.size()) = uv2.v;
  uv.w.segment(uv1.size(), uv2.size()) = uv2.w;
  uv.vis.segment(uv1.size(), uv2.size()) = uv2.vis;
  uv.weights.segment(uv1.size(), uv2.size()) = uv2.weights;
  return uv;
}
  
//! Reading reals from visibility file (including nan's and inf's)
  t_real streamtoreal(std::ifstream& stream) {
    std::string input;
    stream >> input;
    return std::stod(input);
  }
   
utilities::vis_params read_visibility(const std::string &vis_name, const bool w_term) {
  /*
    Reads an csv file with u, v, visibilities and returns the vectors.

    vis_name:: name of input text file containing [u, v, real(V), imag(V)] (separated by ' ').
  */
 std::ifstream vis_file(vis_name);
  vis_file.precision(13);
  t_int row = 0;
  std::string line;
  // counts size of vis file
  while(std::getline(vis_file, line))
    ++row;
  Vector<t_real> utemp = Vector<t_real>::Zero(row);
  Vector<t_real> vtemp = Vector<t_real>::Zero(row);
  Vector<t_real> wtemp = Vector<t_real>::Zero(row);
  Vector<t_complex> vistemp = Vector<t_complex>::Zero(row);
  Vector<t_complex> weightstemp = Vector<t_complex>::Zero(row);

  vis_file.clear();
  vis_file.seekg(0);
  // reads in vis file
  t_real real;
  t_real imag;
  t_real entry;
  for (row=0; row<vistemp.size(); ++row) {
    utemp(row) = streamtoreal(vis_file);
    vtemp(row) = streamtoreal(vis_file);
    if(w_term) {
      wtemp(row) = streamtoreal(vis_file);
    }
    real = streamtoreal(vis_file);
    imag = streamtoreal(vis_file);
    entry = streamtoreal(vis_file);
    vistemp(row) = t_complex(real, imag);
    weightstemp(row) = 1 / entry;
  }
  utilities::vis_params uv_vis;
  uv_vis.u = utemp;
  uv_vis.v = -vtemp; // found that a reflection is needed for the orientation of the gridded image
                     // to be correct
  uv_vis.w = wtemp;
  uv_vis.vis = vistemp;
  uv_vis.weights = weightstemp;
  uv_vis.ra = 0;
  uv_vis.dec = 0;
  uv_vis.average_frequency = 0;

  return uv_vis;
}

void write_visibility(const utilities::vis_params &uv_vis, const std::string &file_name,
                      const bool w_term) {
  /*
          writes visibilities to output text file (currently ignores w-component)
          uv_vis:: input uv data
          file_name:: name of output text file
  */
  std::ofstream out(file_name);
  out.precision(13);
  for(t_int i = 0; i < uv_vis.u.size(); ++i) {
    out << uv_vis.u(i) << " " << -uv_vis.v(i) << " ";
    if(w_term)
      out << uv_vis.w(i) << " ";
    out << std::real(uv_vis.vis(i)) << " " << std::imag(uv_vis.vis(i)) << " "
        << 1. / std::real(uv_vis.weights(i)) << std::endl;
  }
  out.close();
}

utilities::vis_params set_cell_size(const utilities::vis_params &uv_vis, const t_real &max_u,
                                    const t_real &max_v, const t_real &input_cell_size_u,
                                    const t_real &input_cell_size_v) {
  /*
    Converts the units of visibilities to units of 2 * pi, while scaling for the size of a pixel
    (cell_size)

    uv_vis:: visibilities
    cell_size:: size of a pixel in arcseconds
  */

  utilities::vis_params scaled_vis = uv_vis;
  t_real cell_size_u = input_cell_size_u;
  t_real cell_size_v = input_cell_size_v;
  if(cell_size_u == 0 and cell_size_v == 0) {
    cell_size_u = (180 * 3600) / max_u / constant::pi / 3; // Calculate cell size if not given one

    cell_size_v = (180 * 3600) / max_v / constant::pi / 3; // Calculate cell size if not given one
    // PURIFY_MEDIUM_LOG("PSF has a FWHM of {} by {} arcseconds", cell_size_u * 3, cell_size_v * 3);
  }
  if(cell_size_v == 0) {
    cell_size_v = cell_size_u;
  }

  PURIFY_MEDIUM_LOG("Using a pixel size of {} by {} arcseconds", cell_size_u, cell_size_v);
  t_real scale_factor_u = 1;
  t_real scale_factor_v = 1;
  if(uv_vis.units == utilities::vis_units::lambda) {
    scale_factor_u = 180 * 3600 / cell_size_u / constant::pi;
    scale_factor_v = 180 * 3600 / cell_size_v / constant::pi;
    scaled_vis.w = uv_vis.w;
  }
  if(uv_vis.units == utilities::vis_units::radians) {
    scale_factor_u = 180 * 3600 / constant::pi;
    scale_factor_v = 180 * 3600 / constant::pi;
    scaled_vis.w = uv_vis.w;
  }
  scaled_vis.u = uv_vis.u / scale_factor_u * 2 * constant::pi;
  scaled_vis.v = uv_vis.v / scale_factor_v * 2 * constant::pi;

  scaled_vis.units = utilities::vis_units::radians;
  return scaled_vis;
}
utilities::vis_params set_cell_size(const utilities::vis_params &uv_vis, const t_real &cell_size_u,
                                    const t_real &cell_size_v) {
  const t_real max_u = std::sqrt((uv_vis.u.array() * uv_vis.u.array()).maxCoeff());
  const t_real max_v = std::sqrt((uv_vis.v.array() * uv_vis.v.array()).maxCoeff());
  return set_cell_size(uv_vis, max_u, max_v, cell_size_u, cell_size_v);
}

utilities::vis_params
uv_scale(const utilities::vis_params &uv_vis, const t_int &sizex, const t_int &sizey) {
  /*
    scales the uv coordinates from being in units of 2 * pi to units of pixels.
  */
  utilities::vis_params scaled_vis;
  scaled_vis.u = uv_vis.u / (2 * constant::pi) * sizex;
  scaled_vis.v = uv_vis.v / (2 * constant::pi) * sizey;
  scaled_vis.vis = uv_vis.vis;
  scaled_vis.weights = uv_vis.weights;
  for(t_int i = 0; i < uv_vis.u.size(); ++i) {
    // scaled_vis.u(i) = utilities::mod(scaled_vis.u(i), sizex);
    // scaled_vis.v(i) = utilities::mod(scaled_vis.v(i), sizey);
  }
  scaled_vis.w = uv_vis.w;
  scaled_vis.units = utilities::vis_units::pixels;
  scaled_vis.ra = uv_vis.ra;
  scaled_vis.dec = uv_vis.dec;
  scaled_vis.average_frequency = uv_vis.average_frequency;
  return scaled_vis;
}

utilities::vis_params uv_symmetry(const utilities::vis_params &uv_vis) {
  /*
    Adds in reflection of the fourier plane using the condjugate symmetry for a real image.

    uv_vis:: uv coordinates for the fourier plane
  */
  t_int total = uv_vis.u.size();
  Vector<t_real> utemp(2 * total);
  Vector<t_real> vtemp(2 * total);
  Vector<t_real> wtemp(2 * total);
  Vector<t_complex> vistemp(2 * total);
  Vector<t_complex> weightstemp(2 * total);

  for(t_int i = 0; i < uv_vis.u.size(); ++i) {
    utemp(i) = uv_vis.u(i);
    vtemp(i) = uv_vis.v(i);
    wtemp(i) = uv_vis.w(i);
    vistemp(i) = uv_vis.vis(i);
    weightstemp(i) = uv_vis.weights(i);
  }
  for(t_int i = total; i < 2 * total; ++i) {
    utemp(i) = -uv_vis.u(i - total);
    vtemp(i) = -uv_vis.v(i - total);
    wtemp(i) = uv_vis.w(i - total);
    vistemp(i) = std::conj(uv_vis.vis(i - total));
    weightstemp(i) = uv_vis.weights(i - total);
  }
  utilities::vis_params conj_vis;
  conj_vis.u = utemp;
  conj_vis.v = vtemp;
  conj_vis.w = wtemp;
  conj_vis.vis = vistemp;
  conj_vis.weights = weightstemp;
  conj_vis.units = uv_vis.units;
  conj_vis.ra = uv_vis.ra;
  conj_vis.dec = uv_vis.dec;
  conj_vis.average_frequency = uv_vis.average_frequency;

  return conj_vis;
}

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
  if(r < 0)
    r = y + r;
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

  for(t_int l = 0; l < b.cols(); ++l) {
    for(t_int k = 0; k < b.rows(); ++k) {
      output(k, l) = (a * b.block(k, l, a_y, a_x)).sum();
    }
  }

  return output;
}

utilities::vis_params whiten_vis(const utilities::vis_params &uv_vis) {
  /*
                  A function that whitens and returns the visibilities.

                  vis:: input visibilities
                  weights:: this expects weights that are the inverse of the complex variance, they
     are converted th RMS for whitenning.
  */
  auto output_uv_vis = uv_vis;
  output_uv_vis.vis = uv_vis.vis.array() * uv_vis.weights.array().cwiseAbs().sqrt();
  return output_uv_vis;
}
t_real calculate_l2_radius(const t_uint y_size, const t_real &sigma, const t_real &n_sigma,
                           const std::string distirbution) {
  /*
                  Calculates the epsilon, the radius of the l2_ball in sopt
                  y:: vector for the l2 ball
  */
  if(distirbution == "chi^2") {
    if(sigma == 0) {
      return std::sqrt(2 * y_size + n_sigma * std::sqrt(4 * y_size));
    }
    return std::sqrt(2 * y_size + n_sigma * std::sqrt(4 * y_size)) * sigma;
  }
  if(distirbution == "chi") {
    auto alpha = 1. / (8 * y_size)
                 - 1. / (128 * y_size * y_size); // series expansion for gamma relation
    auto mean = std::sqrt(2) * std::sqrt(y_size)
                * (1 - alpha); // using Gamma(k+1/2)/Gamma(k) asymptotic formula
    auto standard_deviation = std::sqrt(2 * y_size * alpha * (2 - alpha));
    if(sigma == 0) {
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
  imaging
  */
  return y0.stableNorm() / std::sqrt(y0.size()) * std::pow(10.0, -(SNR / 20.0));
}

Vector<t_complex>
add_noise(const Vector<t_complex> &y0, const t_complex &mean, const t_real &standard_deviation) {
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
    static std::normal_distribution<t_real> normal_dist_real(std::real(mean),
                                                             standard_deviation / std::sqrt(2));
    static std::normal_distribution<t_real> normal_dist_imag(std::imag(mean),
                                                             standard_deviation / std::sqrt(2));
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

Vector<t_real> fit_fwhm(const Image<t_real> &psf, const t_int &size) {
  /*
          Find FWHM of point spread function, using least squares.

          psf:: point spread function, assumed to be normalised.

          The method assumes that you have sampled at least
          3 pixels across the beam.
  */

  auto x0 = std::floor(psf.cols() * 0.5);
  auto y0 = std::floor(psf.rows() * 0.5);

  // finding patch
  Image<t_real> patch
      = psf.block(std::floor(y0 - size * 0.5) + 1, std::floor(x0 - size * 0.5) + 1, size, size);
  PURIFY_LOW_LOG("Fitting to Patch");

  // finding values for least squares

  std::vector<t_tripletList> entries;
  auto total_entries = 0;

  for(t_int i = 0; i < patch.cols(); ++i) {
    for(t_int j = 0; j < patch.rows(); ++j) {

      entries.emplace_back(j, i, std::log(patch(j, i)));
      total_entries++;
    }
  }
  Matrix<t_real> A = Matrix<t_real>::Zero(total_entries, 4);
  Vector<t_real> q = Vector<t_real>::Zero(total_entries);
  // putting values into a vector and matrix for least squares
  for(t_int i = 0; i < total_entries; ++i) {
    t_real x = entries.at(i).col() - size * 0.5 + 0.5;
    t_real y = entries.at(i).row() - size * 0.5 + 0.5;
    A(i, 0) = static_cast<t_real>(x * x); // x^2
    A(i, 1) = static_cast<t_real>(x * y); // x * y
    A(i, 2) = static_cast<t_real>(y * y); // y^2
    q(i) = std::real(entries.at(i).value());
  }
  // least squares fitting
  const auto solution = static_cast<Vector<t_real>>(
      A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(q));
  t_real const a = -solution(0);
  t_real const b = +solution(1) * 0.5;
  t_real const c = -solution(2);
  // parameters of Gaussian
  t_real theta = std::atan2(b, std::sqrt(std::pow(2 * b, 2) + std::pow(c - a, 2))
                                   + (c - a) * 0.5); // some relatively complicated trig identity to
                                                     // go from tan(2theta) to tan(theta).
  t_real t = 0.;
  t_real s = 0.;
  if(std::abs(b) < 1e-13) {
    t = a;
    s = c;
    theta = 0;
  } else {
    t = (a + c - 2 * b / std::sin(2 * theta)) * 0.5;
    s = (a + c + 2 * b / std::sin(2 * theta)) * 0.5;
  }

  t_real const sigma_x = std::sqrt(1 / (2 * t));
  t_real const sigma_y = std::sqrt(1 / (2 * s));

  Vector<t_real> fit_parameters = Vector<t_real>::Zero(3);

  // fit for the beam rms, used for FWHM
  fit_parameters(0) = sigma_x;
  fit_parameters(1) = sigma_y;
  fit_parameters(2) = theta; // because 2*theta is periodic with pi.
  return fit_parameters;
}

t_real median(const Vector<t_real> &input) {
  // Finds the median of a real vector x
  auto size = input.size();
  Vector<t_real> x(size);
  std::copy(input.data(), input.data() + size, x.data());
  std::sort(x.data(), x.data() + size);
  if(std::floor(size / 2) - std::ceil(size / 2) == 0)
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
  return std::sqrt(model.size()) * (operator_norm * operator_norm) / residuals.matrix().norm()
         * model.cwiseAbs().maxCoeff();
}

Array<t_complex> init_weights(const Vector<t_real> &u, const Vector<t_real> &v,
                              const Vector<t_complex> &weights, const t_real &oversample_factor,
                              const std::string &weighting_type, const t_real &R,
                              const t_int &ftsizeu, const t_int &ftsizev) {
  /*
    Calculate the weights to be applied to the visibilities in the measurement operator.
    It does none, whiten, natural, uniform, and robust.
  */
  if(weighting_type == "none")
    return weights.array() * 0 + 1.;
  if(weighting_type == "natural" or weighting_type == "whiten")
    return weights;
  Vector<t_complex> out_weights(weights.size());
  if((weighting_type == "uniform") or (weighting_type == "robust")) {
    t_real scale
        = 1. / oversample_factor; // scale for fov, controlling the region of sidelobe supression
    Matrix<t_complex> gridded_weights = Matrix<t_complex>::Zero(ftsizev, ftsizeu);
    for(t_int i = 0; i < weights.size(); ++i) {
      t_int q = utilities::mod(floor(u(i) * scale), ftsizeu);
      t_int p = utilities::mod(floor(v(i) * scale), ftsizev);
      gridded_weights(p, q) += 1 * 1; // I get better results assuming all the weights are the same.
                                      // It looks like miriad does this as well.
    }
    t_complex const sum_weights = (weights.array() * weights.array()).sum();
    t_complex const sum_grid_weights2 = (gridded_weights.array() * gridded_weights.array()).sum();
    t_complex const robust_scale
        = sum_weights / sum_grid_weights2 * 25.
          * std::pow(10, -2 * R); // Following standard formula, a bit different from miriad.
    gridded_weights = gridded_weights.array().sqrt();
    for(t_int i = 0; i < weights.size(); ++i) {
      t_int q = utilities::mod(floor(u(i) * scale), ftsizeu);
      t_int p = utilities::mod(floor(v(i) * scale), ftsizev);
      if(weighting_type == "uniform")
        out_weights(i) = weights(i) / gridded_weights(p, q);
      if(weighting_type == "robust") {
        out_weights(i)
            = weights(i)
              / std::sqrt(1. + robust_scale * gridded_weights(p, q) * gridded_weights(p, q));
      }
    }
  } else
    throw std::runtime_error("Wrong weighting type, " + weighting_type + " not recognised.");
  return out_weights;
}

std::tuple<t_int, t_real> checkpoint_log(const std::string &diagnostic) {
  // reads a log file and returns the latest parameters
  if(!utilities::file_exists(diagnostic))
    return std::make_tuple(0, 0);

  t_int iters = 0;
  t_real gamma = 0;
  std::ifstream log_file(diagnostic);
  std::string entry;
  std::string s;

  std::getline(log_file, s);

  while(log_file) {
    if(!std::getline(log_file, s))
      break;
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
  if(re_sample_ratio == 1)
    return input;

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
  auto fft 
      = purify::operators::init_FFT_2d<Vector<t_complex>>(input.rows(), input.cols(), 1., ft_plan);
  Vector<t_complex> ft_grid = Vector<t_complex>::Zero(input.size());
  std::get<0>(fft)(ft_grid, Vector<t_complex>::Map(input.data(), input.size()));
  Matrix<t_complex> const new_ft_grid = utilities::re_sample_ft_grid(Matrix<t_complex>::Map(ft_grid.data(),input.rows(), input.cols()), re_sample_ratio);
  Vector<t_complex> output = Vector<t_complex>::Zero(input.size());
  fft 
      = purify::operators::init_FFT_2d<Vector<t_complex>>(new_ft_grid.rows(), new_ft_grid.cols(), 1., ft_plan);
  std::get<1>(fft)(output, Vector<t_complex>::Map(new_ft_grid.data(), new_ft_grid.size()));
  return Matrix<t_complex>::Map(output.data(), new_ft_grid.rows(), new_ft_grid.cols()) * re_sample_ratio;
}
} // namespace utilities
} // namespace purify
