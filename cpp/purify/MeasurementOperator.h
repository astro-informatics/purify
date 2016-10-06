#ifndef PURIFY_MEASUREMENT_OPERATOR_H
#define PURIFY_MEASUREMENT_OPERATOR_H

#include "purify/config.h"
#include <sopt/linear_transform.h>
#include "purify/FFTOperator.h"
#include "purify/kernels.h"
#include "purify/types.h"
#include "purify/utilities.h"

#include <iostream>
#include <string>

namespace purify {

//! This does something
class MeasurementOperator {
public:
  Sparse<t_complex> G;
  Image<t_real> S;
  Array<t_complex> W;
  Image<t_complex> C;
  t_real norm = 1;
  t_real resample_factor = 1;

  MeasurementOperator();
  MeasurementOperator(const utilities::vis_params &uv_vis_input, const t_int &Ju, const t_int &Jv,
                      const std::string &kernel_name, const t_int &imsizex, const t_int &imsizey,
                      const t_int &norm_iterations = 20, const t_real &oversample_factor = 2,
                      const t_real &cell_x = 1, const t_real &cell_y = 1,
                      const std::string &weighting_type = "none", const t_real &R = 0,
                      bool use_w_term = false, const t_real &energy_fraction = 1,
                      const std::string &primary_beam = "none", bool fft_grid_correction = false);

#define PURIFY_MACRO(NAME, TYPE, VALUE)                                                            \
protected:                                                                                         \
  TYPE NAME##_ = VALUE;                                                                            \
                                                                                                   \
public:                                                                                            \
  TYPE const &NAME() const { return NAME##_; };                                                    \
  MeasurementOperator &NAME(TYPE const &NAME) {                                                    \
    NAME##_ = NAME;                                                                                \
    return *this;                                                                                  \
  };

  PURIFY_MACRO(Ju, t_int, 4);
  PURIFY_MACRO(Jv, t_int, 4);
  PURIFY_MACRO(kernel_name, std::string, "kb");
  PURIFY_MACRO(imsizex, t_int, 512);
  PURIFY_MACRO(imsizey, t_int, 512);
  PURIFY_MACRO(norm_iterations, t_int, 20);
  PURIFY_MACRO(oversample_factor, t_real, 2);
  PURIFY_MACRO(cell_x, t_real, 1);
  PURIFY_MACRO(cell_y, t_real, 1);
  PURIFY_MACRO(weighting_type, std::string, "none");
  PURIFY_MACRO(R, t_real, 0);
  PURIFY_MACRO(use_w_term, bool, false);
  PURIFY_MACRO(energy_fraction, t_real, 1.);
  PURIFY_MACRO(fft_grid_correction, bool, false);
  PURIFY_MACRO(primary_beam, std::string, "none");
  PURIFY_MACRO(fftw_plan_flag, std::string, "estimate");
  //! Reads in visiblities and uses them to construct the operator for use
  MeasurementOperator &construct_operator(const utilities::vis_params &uv_vis_input) {
    MeasurementOperator::init_operator(uv_vis_input);
    return *this;
  };

  // writing definiton of fftoperator so that it is mutable.
protected:
  mutable FFTOperator fftoperator_
      = purify::FFTOperator();

public:
  FFTOperator &fftoperator() { return fftoperator_; };
  MeasurementOperator &fftoperator(FFTOperator const &fftoperator) {
    fftoperator_ = fftoperator;
    return *this;
  };
#undef PURIFY_MACRO
  // Default values
protected:
  t_int ftsizeu_;
  t_int ftsizev_;

public:
  //! Degridding operator that degrids image to visibilities
  Vector<t_complex> degrid(const Image<t_complex> &eigen_image) const;
  //! Gridding operator that grids image from visibilities
  Image<t_complex> grid(const Vector<t_complex> &visibilities) const;

protected:
  //! Match uv coordinates to grid
  Vector<t_real> omega_to_k(const Vector<t_real> &omega);
  //! Generates interpolation matrix
  Sparse<t_complex> init_interpolation_matrix2d(const Vector<t_real> &u, const Vector<t_real> &v,
                                                const t_int Ju, const t_int Jv,
                                                const std::function<t_real(t_real)> kernelu,
                                                const std::function<t_real(t_real)> kernelv);
  //! Generates scaling factors for gridding correction using an fft
  Image<t_real> init_correction2d_fft(const std::function<t_real(t_real)> kernelu,
                                      const std::function<t_real(t_real)> kernelv, const t_int Ju,
                                      const t_int Jv);
  //! Generates scaling factors for gridding correction
  Image<t_real> init_correction2d(const std::function<t_real(t_real)> ftkernelu,
                                  const std::function<t_real(t_real)> ftkernelv);
  //! Generates and calculates weights
  Array<t_complex> init_weights(const Vector<t_real> &u, const Vector<t_real> &v,
                                const Vector<t_complex> &weights, const t_real &oversample_factor,
                                const std::string &weighting_type, const t_real &R);
  //! Calculate Primary Beam
  Image<t_real>
  init_primary_beam(const std::string &primary_beam, const t_real &cell_x, const t_real &cell_y);

public:
  //! Construct operator
  void init_operator(const utilities::vis_params &uv_vis_input);

public:
  //! Estiamtes norm of operator
  t_real power_method(const t_int &niters, const t_real &relative_difference = 1e-9);
};

//! Helper function to create a linear transform from a measurement operator
sopt::LinearTransform<sopt::Vector<sopt::t_complex>>
linear_transform(MeasurementOperator const &measurements, t_uint nvis);
}
#endif
