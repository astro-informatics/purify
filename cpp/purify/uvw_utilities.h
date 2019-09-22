#ifndef PURIFY_UVW_UTILITIES_H
#define PURIFY_UVW_UTILITIES_H

#include "purify/config.h"
#include "purify/types.h"
#include <fstream>
#include <string>
#include <type_traits>

namespace purify {

namespace utilities {
enum class vis_source { measurements, simulation };
enum class vis_units { lambda, radians, pixels };
struct vis_params {
  Vector<t_real> u;  // u coordinates
  Vector<t_real> v;  // v coordinates
  Vector<t_real> w;
  Vector<t_real> time;
  Vector<t_uint> baseline;
  Vector<t_real> frequencies;
  Vector<t_complex> vis;      // complex visiblities
  Vector<t_complex> weights;  // weights for visibilities
  vis_units units = vis_units::lambda;
  t_real ra = 0.;   // decimal degrees
  t_real dec = 0.;  // decimal degrees
  t_real average_frequency = 0.;
  t_real phase_centre_x = 0.;  // phase centre
  t_real phase_centre_y = 0.;  // phase centre
  //! return subset of measurements
  vis_params segment(const t_uint pos, const t_uint length) const {
    return vis_params(this->u.segment(pos, length), this->v.segment(pos, length),
                      this->w.segment(pos, length), this->vis.segment(pos, length),
                      this->weights.segment(pos, length), this->units, this->ra, this->dec,
                      this->average_frequency);
  };
  //! default constructor
  vis_params(){};
  //! constructor
  vis_params(const Vector<t_real> &u_, const Vector<t_real> &v_, const Vector<t_real> &w_,
             const Vector<t_complex> &vis_, const Vector<t_complex> &weights_,
             const vis_units units_ = vis_units::lambda, const t_real &ra_ = 0,
             const t_real &dec_ = 0, const t_real &average_frequency_ = 0)
      : u(u_),
        v(v_),
        w(w_),
        vis(vis_),
        weights(weights_),
        ra(ra_),
        dec(dec_),
        units(units_),
        average_frequency(average_frequency_){};
  //! return number of measurements
  t_uint size() const { return this->vis.size(); };
};

//! calculate the rotated u from euler angles in zyz and starting coordinates (u, v, w)
template <class T>
T calculate_rotated_u(const T &u, const T &v, const T &w, const t_real alpha, const t_real beta,
                      const t_real gamma) {
  return u * (std::cos(alpha) * std::cos(beta) * std::cos(gamma) -
              std::sin(alpha) * std::sin(gamma)) +
         v * (-std::cos(alpha) * std::cos(beta) * std::sin(gamma) -
              std::sin(alpha) * std::cos(gamma)) +
         w * std::cos(alpha) * std::sin(beta);
}
//! calculate the rotated v from euler angles in zyz and starting coordinates (u, v, w)
template <class T>
T calculate_rotated_v(const T &u, const T &v, const T &w, const t_real alpha, const t_real beta,
                      const t_real gamma) {
  return u * (std::sin(alpha) * std::cos(beta) * std::cos(gamma) +
              std::cos(alpha) * std::sin(gamma)) +
         v * (-std::sin(alpha) * std::cos(beta) * std::sin(gamma) +
              std::cos(alpha) * std::cos(gamma)) +
         w * std::sin(alpha) * std::sin(beta);
}
//! calculate the rotated w from euler angles in zyz and starting coordinates (u, v, w)
template <class T>
T calculate_rotated_w(const T &u, const T &v, const T &w, const t_real alpha, const t_real beta,
                      const t_real gamma) {
  return u * (-std::sin(beta) * std::cos(gamma)) + v * (std::sin(beta) * std::sin(gamma)) +
         w * std::cos(beta);
}

//! Generates a random visibility coverage
utilities::vis_params random_sample_density(const t_int vis_num, const t_real mean,
                                            const t_real standard_deviation,
                                            const t_real rms_w = 0);
//! Remove redundent baselines
utilities::vis_params remove_redundent(const utilities::vis_params &uv_data);
//! Generate guassianly distributed (sigma = scale) antenna positions
Matrix<t_real> generate_antennas(const t_uint N, const t_real scale);
template <class T>
utilities::vis_params antenna_to_coverage(const t_uint N, const t_real scale, const T &frequency) {
  return antenna_to_coverage(generate_antennas(N, scale), frequency, 0., 0., 0., 0.);
}
//! Using guassianly distributed (sigma = pi) antenna positions generate a coverage
utilities::vis_params antenna_to_coverage(const t_uint N);
//! Provided antenna positions generate a coverage for a fixed frequency for mulitple times
utilities::vis_params antenna_to_coverage(const Matrix<t_real> &B, const t_real frequency,
                                          const std::vector<t_real> &times, const t_real theta_ra,
                                          const t_real phi_dec, const t_real latitude);
//! Provided antenna positions generate a coverage for a fixed frequency a fixed time
utilities::vis_params antenna_to_coverage(const Matrix<t_real> &B, const t_real frequency,
                                          const t_real times, const t_real theta_ra,
                                          const t_real phi_dec, const t_real latitude);
//! Provided antenna positions generate a coverage for multiple frequency a fixed time
utilities::vis_params antenna_to_coverage(const Matrix<t_real> &B,
                                          const std::vector<t_real> &frequencies,
                                          const t_real times, const t_real theta_ra,
                                          const t_real phi_dec, const t_real latitude);
//! Provided antenna positions generate a coverage for multiple frequencies for multiple times for
//! earth rotation synthesis
utilities::vis_params antenna_to_coverage(const Matrix<t_real> &B,
                                          const std::vector<t_real> &frequencies,
                                          const std::vector<t_real> times, const t_real theta_ra,
                                          const t_real phi_dec, const t_real latitude);
//! Provided antenna positions generate a coverage for multiple frequencies for multiple times at
//! different pointings
template <class F>
utilities::vis_params antenna_to_coverage_general(const Matrix<t_real> &B,
                                                  const std::vector<t_real> &frequencies,
                                                  const std::vector<t_real> &times,
                                                  const F &position_angle_RA_function,
                                                  const F &position_angle_DEC_function,
                                                  const t_real latitude) {
  if (B.cols() != 3) throw std::runtime_error("Antennae coordinates are not 3D vectors.");
  const t_uint M = B.rows() * (B.rows() - 1) / 2;
  const t_uint chans = frequencies.size();
  const t_uint time_samples = times.size();
  Vector<t_real> u = Vector<t_real>::Zero(M * chans * time_samples);
  Vector<t_real> v = Vector<t_real>::Zero(M * chans * time_samples);
  Vector<t_real> w = Vector<t_real>::Zero(M * chans * time_samples);
  Vector<t_complex> weights = Vector<t_complex>::Ones(M * chans * time_samples);
  Vector<t_complex> vis = Vector<t_complex>::Ones(M * chans * time_samples);
  t_uint m = 0;
  for (t_uint i = 0; i < B.rows(); i++) {
    for (t_uint j = i + 1; j < B.rows(); j++) {
      const Vector<t_real> r = (B.row(i) - B.row(j));
      for (t_int k = 0; k < chans; k++) {
        for (t_int t = 0; t < time_samples; t++) {
          const t_int index = m + M * (k + chans * t);
          u(index) = utilities::calculate_rotated_u(
                         r(0), r(1), r(2), position_angle_RA_function(times.at(t)),
                         position_angle_DEC_function(times.at(t)) - latitude, 0.) *
                     frequencies.at(k) / constant::c;
          v(index) = utilities::calculate_rotated_v(
                         r(0), r(1), r(2), position_angle_RA_function(times.at(t)),
                         position_angle_DEC_function(times.at(t)) - latitude, 0.) *
                     frequencies.at(k) / constant::c;
          w(index) = utilities::calculate_rotated_w(
                         r(0), r(1), r(2), position_angle_RA_function(times.at(t)),
                         position_angle_DEC_function(times.at(t)) - latitude, 0.) *
                     frequencies.at(k) / constant::c;
        }
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
//! Provided antenna positions generate a coverage
template <class T, class K>
utilities::vis_params antenna_to_coverage(const Vector<t_real> &x, const Vector<t_real> &y,
                                          const Vector<t_real> &z, const T &frequencies,
                                          const K &times, const t_real theta_ra,
                                          const t_real phi_dec, const t_real latitude) {
  if (x.size() != y.size()) throw std::runtime_error("x and y positions are not the same amount.");
  if (x.size() != z.size()) throw std::runtime_error("x and z positions are not the same amount.");
  Matrix<t_real> B(x.size(), 3);
  B.col(0) = x;
  B.col(1) = y;
  B.col(2) = z;
  return antenna_to_coverage(B, frequencies, times, theta_ra, phi_dec, latitude);
}
//! Read in a text file of antenna positions into a matrix [x, y ,z]
Matrix<t_real> read_ant_positions(const std::string &pos_name);
//! Read in a text file of antenna positions into a matrix [x, y ,z] and generate coverage for
//! frequencies
template <class T, class K>
utilities::vis_params read_ant_positions_to_coverage(const std::string &pos_name,
                                                     const T &frequencies, const K &times,
                                                     const t_real theta_ra, const t_real phi_dec,
                                                     const t_real latitude) {
  return antenna_to_coverage(read_ant_positions(pos_name), frequencies, times, theta_ra, phi_dec,
                             latitude);
}
//! Reading reals from visibility file (including nan's and inf's)
t_real streamtoreal(std::ifstream &stream);
//! Reads in visibility file
utilities::vis_params read_visibility(const std::string &vis_name, const bool w_term = false);
//! Read visibility files from name of vector
utilities::vis_params read_visibility(const std::vector<std::string> &names,
                                      const bool w_term = false);
//! Reads in two visibility files
utilities::vis_params read_visibility(const std::string &vis_name2,
                                      const utilities::vis_params &u1);
//! Writes visibilities to txt
void write_visibility(const utilities::vis_params &uv_vis, const std::string &file_name,
                      const bool w_term = false);
//! Scales visibilities to a given pixel size in arcseconds
utilities::vis_params set_cell_size(const utilities::vis_params &uv_vis,
                                    const t_real &cell_size_u = 0, const t_real &cell_size_v = 0);
utilities::vis_params set_cell_size(const utilities::vis_params &uv_vis, const t_real &max_u,
                                    const t_real &max_v, const t_real &cell_size_u,
                                    const t_real &cell_size_v);
//! Converts u and v coordaintes to units of pixels
utilities::vis_params convert_to_pixels(const utilities::vis_params &uv_vis, const t_real cell_x,
                                        const t_real cell_y, const t_real imsizex,
                                        const t_real imsizey, const t_real oversample_ratio);
//! scales the visibilities to units of pixels
utilities::vis_params uv_scale(const utilities::vis_params &uv_vis, const t_int &ftsizeu,
                               const t_int &ftsizev);
//! reflects visibilities into the w >= 0 domain
utilities::vis_params conjugate_w(const utilities::vis_params &uv_vis);
}  // namespace utilities
}  // namespace purify

#endif
