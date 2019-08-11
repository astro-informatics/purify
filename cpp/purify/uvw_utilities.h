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

//! Generates a random visibility coverage
utilities::vis_params random_sample_density(const t_int vis_num, const t_real mean,
                                            const t_real standard_deviation,
                                            const t_real rms_w = 0);
//! Generate guassianly distributed (sigma = scale) antenna positions
Matrix<t_real> generate_antennas(const t_uint N, const t_real scale);
template <class T>
utilities::vis_params antenna_to_coverage(const t_uint N, const t_real scale, const T &frequency) {
  return antenna_to_coverage(generate_antennas(N, scale), frequency);
}
//! Using guassianly distributed (sigma = pi) antenna positions generate a coverage
utilities::vis_params antenna_to_coverage(const t_uint N);
//! Provided antenna positions generate a coverage for a fixed frequency
utilities::vis_params antenna_to_coverage(const Matrix<t_real> &B, const t_real frequency);
//! Provided antenna positions generate a coverage for a fixed frequencies
utilities::vis_params antenna_to_coverage(const Matrix<t_real> &B,
                                          const std::vector<t_real> &frequencies);
//! Provided antenna positions generate a coverage
template <class T>
utilities::vis_params antenna_to_coverage(const Vector<t_real> &x, const Vector<t_real> &y,
                                          const Vector<t_real> &z, const T &frequencies) {
  if (x.size() != y.size()) throw std::runtime_error("x and y positions are not the same amount.");
  if (x.size() != z.size()) throw std::runtime_error("x and z positions are not the same amount.");
  Matrix<t_real> B(x.size(), 3);
  B.col(0) = x;
  B.col(1) = y;
  B.col(2) = z;
  return antenna_to_coverage(B, frequencies);
}
//! Read in a text file of antenna positions into a matrix [x, y ,z]
Matrix<t_real> read_ant_positions(const std::string &pos_name);
//! Read in a text file of antenna positions into a matrix [x, y ,z] and generate coverage for
//! frequencies
template <class T>
utilities::vis_params read_ant_positions_to_coverage(const std::string &pos_name,
                                                     const T &frequencies) {
  return antenna_to_coverage(read_ant_positions(pos_name), frequencies);
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
