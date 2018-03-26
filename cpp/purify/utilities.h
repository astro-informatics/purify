#ifndef PURIFY_UTILITIES_H
#define PURIFY_UTILITIES_H

#include "purify/config.h"
#include <fstream>
#include <string>
#include <type_traits>
#include "purify/FFTOperator.h"
#include "purify/types.h"

namespace purify {

namespace utilities {
enum class vis_units { lambda, radians, pixels };
struct vis_params {
  Vector<t_real> u; // u coordinates
  Vector<t_real> v; // v coordinates
  Vector<t_real> w;
  Vector<t_real> time;
  Vector<t_uint> baseline;
  Vector<t_real> frequencies;
  Vector<t_complex> vis;     // complex visiblities
  Vector<t_complex> weights; // weights for visibilities
  vis_units units = vis_units::lambda;
  t_real ra = 0.;  // decimal degrees
  t_real dec = 0.; // decimal degrees
  t_real average_frequency = 0.;
  t_real phase_centre_x = 0.; // phase centre
  t_real phase_centre_y = 0.; // phase centre
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
      : u(u_), v(v_), w(w_), vis(vis_), weights(weights_), ra(ra_), dec(dec_), units(units_),
        average_frequency(average_frequency_){};
  //! return number of measurements
  t_uint size() const { return this->vis.size(); };
};

//! Generates a random visibility coverage
utilities::vis_params random_sample_density(const t_int &vis_num, const t_real &mean,
                                            const t_real &standard_deviation,
                                            const t_real &max_w = 0);
//! Generate guassianly distributed (sigma = pi) antenna positions
Matrix<t_real> generate_antennas(const t_uint N);
//! Using guassianly distributed (sigma = pi) antenna positions generate a coverage
utilities::vis_params antenna_to_coverage(const t_uint N);
//! Provided antenna positions generate a coverage
utilities::vis_params antenna_to_coverage(const Matrix<t_real> &B);
//! Reads in visibility file
utilities::vis_params read_visibility(const std::string &vis_name, const bool w_term = false);
//! Reads in two visibility files
utilities::vis_params
read_visibility(const std::string &vis_name2, const utilities::vis_params &u1);
//! Read in multiple visiblity files
template <class... ARGS>
utilities::vis_params read_visibility(const std::string &vis_name2, ARGS &&... args) {
  return read_visibility(vis_name2, true, read_visibility(std::forward<ARGS>(args)...));
}
//! Writes visibilities to txt
void write_visibility(const utilities::vis_params &uv_vis, const std::string &file_name,
                      const bool w_term = false);
//! Scales visibilities to a given pixel size in arcseconds
utilities::vis_params set_cell_size(const utilities::vis_params &uv_vis,
                                    const t_real &cell_size_u = 0, const t_real &cell_size_v = 0);
utilities::vis_params set_cell_size(const utilities::vis_params &uv_vis, const t_real &max_u,
                                    const t_real &max_v, const t_real &cell_size_u,
                                    const t_real &cell_size_v);
//! scales the visibilities to units of pixels
utilities::vis_params
uv_scale(const utilities::vis_params &uv_vis, const t_int &ftsizeu, const t_int &ftsizev);
//! Puts in conjugate visibilities
utilities::vis_params uv_symmetry(const utilities::vis_params &uv_vis);
//! Converts from subscript to index for matrix.
t_int sub2ind(const t_int &row, const t_int &col, const t_int &rows, const t_int &cols);
//! Converts from index to subscript for matrix.
std::tuple<t_int, t_int> ind2sub(const t_int &sub, const t_int &cols, const t_int &rows);
//! Mod function modified to wrap circularly for negative numbers
t_real mod(const t_real &x, const t_real &y);
//! Calculate mean of vector
template <class K> typename K::Scalar mean(const K x) {
  // Calculate mean of vector x
  return x.array().mean();
}
//! Calculate variance of vector
template <class K> t_real variance(const K x) {
  // calculate variance of vector x
  auto q = (x.array() - x.array().mean()).matrix();
  t_real var = std::real((q.adjoint() * q)(0) / static_cast<t_real>(q.size() - 1));
  return var;
}
//! Calculates the standard deviation of a vector
template <class K> t_real standard_deviation(const K x) {
  // calculate standard deviation of vector x
  return std::sqrt(variance(x));
}
//! Calculates the convolution between two images
Image<t_complex> convolution_operator(const Image<t_complex> &a, const Image<t_complex> &b);
//! A vector that whiten's the visibilities given the weights.
utilities::vis_params whiten_vis(const utilities::vis_params &uv_vis);
//! A function that calculates the l2 ball radius for sopt
t_real calculate_l2_radius(const Vector<t_complex> &y, const t_real &sigma = 0,
                           const t_real &n_sigma = 2., const std::string distirbution = "chi");
//! Converts SNR to RMS noise
t_real SNR_to_standard_deviation(const Vector<t_complex> &y0, const t_real &SNR = 30.);
//! Add guassian noise to vector
Vector<t_complex>
add_noise(const Vector<t_complex> &y, const t_complex &mean, const t_real &standard_deviation);
//! Test to see if file exists
bool file_exists(const std::string &name);
//! Method to fit Gaussian to PSF
Vector<t_real> fit_fwhm(const Image<t_real> &psf, const t_int &size = 3);
//! Return median of real vector
t_real median(const Vector<t_real> &input);
//! Calculate the dynamic range between the model and residuals
t_real dynamic_range(const Image<t_complex> &model, const Image<t_complex> &residuals,
                     const t_real &operator_norm = 1);
//! Calculate weightings
Array<t_complex> init_weights(const Vector<t_real> &u, const Vector<t_real> &v,
                              const Vector<t_complex> &weights, const t_real &oversample_factor,
                              const std::string &weighting_type, const t_real &R,
                              const t_int &ftsizeu, const t_int &ftsizev);
//! Parallel multiplication with a sparse matrix and vector
template <class T0, class T1>
typename std::enable_if<std::is_same<typename T0::Scalar, typename T1::Scalar>::value
                            and T0::IsRowMajor,
                        Vector<typename T0::Scalar>>::type
sparse_multiply_matrix(const Eigen::SparseMatrixBase<T0> &M, const Eigen::MatrixBase<T1> &x) {
  assert(M.cols() == x.size());
  Vector<typename T0::Scalar> y = Vector<typename T0::Scalar>::Zero(M.rows());
  auto const &derived = M.derived();
// parallel sparse matrix multiplication with vector.
#pragma omp parallel for
  //#pragma omp simd
  for(t_int k = 0; k < M.outerSize(); ++k)
    for(typename Sparse<typename T0::Scalar>::InnerIterator it(derived, k); it; ++it)
      y(k) += it.value() * x(it.index());
  return y;
}
//! Reads a diagnostic file and updates parameters
std::tuple<t_int, t_real> checkpoint_log(const std::string &diagnostic);
//! Multiply images coefficient-wise using openmp
template <class K, class L> Image<t_complex> parallel_multiply_image(const K &A, const L &B) {
  const t_int rows = A.rows();
  const t_int cols = A.cols();
  Image<t_complex> C = Matrix<t_complex>::Zero(rows, cols);

#pragma omp simd collapse(2)
  for(t_int i = 0; i < cols; ++i)
    for(t_int j = 0; j < rows; ++j)
      C(j, i) = A(j, i) * B(j, i);
  return C;
}
//! zero pads ft grid for image up sampling and downsampling
Matrix<t_complex> re_sample_ft_grid(const Matrix<t_complex> &input, const t_real &re_sample_factor);
//! resamples image size
Matrix<t_complex> re_sample_image(const Matrix<t_complex> &input, const t_real &re_sample_ratio);

} // namespace utilities
} // namespace purify

#endif
