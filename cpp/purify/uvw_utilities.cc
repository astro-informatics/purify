#include "purify/uvw_utilities.h"
#include "purify/config.h"
#include <fstream>
#include <random>
#include <sys/stat.h>
#include "purify/logging.h"
#include "purify/operators.h"

namespace purify {
namespace utilities {
Matrix<t_real> generate_antennas(const t_uint N, const t_real scale) {
  Matrix<t_real> B = Matrix<t_real>::Zero(N, 3);
  const t_real mean = 0;
  const t_real standard_deviation = scale;
  auto sample = [&mean, &standard_deviation]() {
    std::random_device rd;
    std::mt19937_64 rng(rd());
    t_real output = 4 * standard_deviation + mean;
    static std::normal_distribution<> normal_dist(mean, standard_deviation);
    // ensures that all sample points are within bounds
    while (std::abs(output - mean) > 3 * standard_deviation) {
      output = normal_dist(rng);
    }
    if (output != output) PURIFY_DEBUG("New random-sample density: {}", output);
    return output;
  };
  for (t_uint i = 0; i < N; i++) {
    B(i, 0) = sample();
    B(i, 1) = sample();
    B(i, 2) = sample();
  }
  return B * scale;
}

utilities::vis_params antenna_to_coverage(const Matrix<t_real> &B,
                                          const std::vector<t_real> &frequencies) {
  if (B.cols() != 3) throw std::runtime_error("Antennae coordinates are not 3D vectors.");
  const t_uint M = B.rows() * (B.rows() - 1) * 0.5;
  const t_uint chans = frequencies.size();
  Vector<t_real> u = Vector<t_real>::Zero(M * chans);
  Vector<t_real> v = Vector<t_real>::Zero(M * chans);
  Vector<t_real> w = Vector<t_real>::Zero(M * chans);
  Vector<t_complex> weights = Vector<t_complex>::Ones(M * chans);
  Vector<t_complex> vis = Vector<t_complex>::Ones(M * chans);
  t_uint m = 0;
  for (t_uint i = 0; i < B.rows(); i++) {
    for (t_uint j = i + 1; j < B.rows(); j++) {
      const Vector<t_real> r = (B.row(i) - B.row(j));
      for (t_int k = 0; k < frequencies.size(); k++) {
        u(m + k * M) = r(0) * frequencies.at(k) / constant::c;
        v(m + k * M) = r(1) * frequencies.at(k) / constant::c;
        w(m + k * M) = r(2) * frequencies.at(k) / constant::c;
      }
      m++;
    }
  }
  if (M != m)
    throw std::runtime_error(
        "Number of created baselines does not match expected baseline number N * (N - 1) / 2.");
  utilities::vis_params coverage(u, v, w, vis, weights, utilities::vis_units::lambda);
  return coverage;
};
utilities::vis_params antenna_to_coverage(const Matrix<t_real> &B, const t_real frequency) {
  return antenna_to_coverage(B, std::vector<t_real>(1, frequency));
}

Matrix<t_real> read_ant_positions(const std::string &pos_name) {
  /*
    Reads an csv file with x, y, z (meters) and returns the vectors as a matrix (x, y, z) cols.

    vis_name:: name of input text file containing [x, y, z] (separated by ' ').
  */
  std::ifstream pos_file(pos_name);
  pos_file.precision(13);
  t_int row = 0;
  std::string line;
  // counts size of pos file
  while (std::getline(pos_file, line)) ++row;
  if (row < 1) throw std::runtime_error("No positions in the file: " + pos_name);
  Matrix<t_real> B = Matrix<t_real>::Zero(row, 3);

  pos_file.clear();
  pos_file.seekg(0);
  // reads in vis file
  for (row = 0; row < B.rows(); ++row) {
    B(row, 0) = streamtoreal(pos_file);
    B(row, 1) = streamtoreal(pos_file);
    B(row, 2) = streamtoreal(pos_file);
  }

  return B;
}

utilities::vis_params random_sample_density(const t_int vis_num, const t_real mean,
                                            const t_real standard_deviation, const t_real rms_w) {
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
    while (std::abs(output - mean) > 3 * standard_deviation) {
      output = normal_dist(rng);
    }
    if (output != output) PURIFY_DEBUG("New random-sample density: {}", output);
    return output;
  };

  utilities::vis_params uv_vis;
  uv_vis.u = Vector<t_real>::Zero(vis_num);
  uv_vis.v = Vector<t_real>::Zero(vis_num);
  uv_vis.w = Vector<t_real>::Zero(vis_num);
  //#pragma omp parallel for
  for (t_uint i = 0; i < vis_num; i++) {
    uv_vis.u(i) = sample();
    uv_vis.v(i) = sample();
    uv_vis.w(i) = sample();
  }
  uv_vis.w = uv_vis.w / standard_deviation * rms_w;
  uv_vis.weights = Vector<t_complex>::Constant(vis_num, 1);
  uv_vis.vis = Vector<t_complex>::Constant(vis_num, 1);
  uv_vis.ra = 0;
  uv_vis.dec = 0;
  uv_vis.average_frequency = 0;
  return uv_vis;
}

utilities::vis_params read_visibility(const std::vector<std::string> &names, const bool w_term) {
  utilities::vis_params output = read_visibility(names.at(0), w_term);
  if (names.size() == 1) return output;
  for (int i = 1; i < names.size(); i++) output = read_visibility(names.at(i), output);
  return output;
}
utilities::vis_params read_visibility(const std::string &vis_name2,
                                      const utilities::vis_params &uv1) {
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
t_real streamtoreal(std::ifstream &stream) {
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
  while (std::getline(vis_file, line)) ++row;
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
  for (row = 0; row < vistemp.size(); ++row) {
    utemp(row) = streamtoreal(vis_file);
    vtemp(row) = streamtoreal(vis_file);
    if (w_term) {
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
  uv_vis.v = -vtemp;  // found that a reflection is needed for the orientation of the gridded image
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
  for (t_int i = 0; i < uv_vis.u.size(); ++i) {
    out << uv_vis.u(i) << " " << -uv_vis.v(i) << " ";
    if (w_term) out << uv_vis.w(i) << " ";
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
  if (cell_size_u == 0 and cell_size_v == 0) {
    cell_size_u = (180 * 3600) / max_u / constant::pi / 3;  // Calculate cell size if not given one

    cell_size_v = (180 * 3600) / max_v / constant::pi / 3;  // Calculate cell size if not given one
    // PURIFY_MEDIUM_LOG("PSF has a FWHM of {} by {} arcseconds", cell_size_u * 3, cell_size_v * 3);
  }
  if (cell_size_v == 0) {
    cell_size_v = cell_size_u;
  }

  PURIFY_MEDIUM_LOG("Using a pixel size of {} by {} arcseconds", cell_size_u, cell_size_v);
  t_real scale_factor_u = 1;
  t_real scale_factor_v = 1;
  if (uv_vis.units == utilities::vis_units::lambda) {
    scale_factor_u = 180 * 3600 / cell_size_u / constant::pi;
    scale_factor_v = 180 * 3600 / cell_size_v / constant::pi;
    scaled_vis.w = uv_vis.w;
  }
  if (uv_vis.units == utilities::vis_units::radians) {
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

utilities::vis_params uv_scale(const utilities::vis_params &uv_vis, const t_int &sizex,
                               const t_int &sizey) {
  /*
    scales the uv coordinates from being in units of 2 * pi to units of pixels.
  */
  utilities::vis_params scaled_vis;
  scaled_vis.u = uv_vis.u / (2 * constant::pi) * sizex;
  scaled_vis.v = uv_vis.v / (2 * constant::pi) * sizey;
  scaled_vis.vis = uv_vis.vis;
  scaled_vis.weights = uv_vis.weights;
  for (t_int i = 0; i < uv_vis.u.size(); ++i) {
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
utilities::vis_params convert_to_pixels(const utilities::vis_params &uv_vis, const t_real cell_x,
                                        const t_real cell_y, const t_real imsizex,
                                        const t_real imsizey, const t_real oversample_ratio) {
  t_real du = 1;
  t_real dv = 1;
  if (uv_vis.units == utilities::vis_units::lambda) {
    du = widefield::pixel_to_lambda(cell_x, imsizex, oversample_ratio);
    dv = widefield::pixel_to_lambda(cell_y, imsizey, oversample_ratio);
  }
  if (uv_vis.units == utilities::vis_units::radians) {
    du = constant::pi / std::floor(imsizex * oversample_ratio);
    dv = constant::pi / std::floor(imsizey * oversample_ratio);
  }
  auto out = uv_vis;
  out.u = uv_vis.u / du;
  out.v = uv_vis.v / dv;
  out.units = utilities::vis_units::pixels;
  return out;
}

utilities::vis_params conjugate_w(const utilities::vis_params &uv_vis) {
  utilities::vis_params output = uv_vis;
#pragma omp parallel for
  for (t_uint i = 0; i < output.size(); i++) {
    if (uv_vis.w(i) < 0) {
      output.w(i) = -uv_vis.w(i);
      output.v(i) = -uv_vis.v(i);
      output.u(i) = -uv_vis.u(i);
      output.vis(i) = std::conj(uv_vis.vis(i));
    }
    assert(output.w(i) >= 0);
  }
  return output;
}
}  // namespace utilities
}  // namespace purify
