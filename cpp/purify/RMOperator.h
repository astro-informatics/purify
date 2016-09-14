#ifndef PURIFY_RM_OPERATOR_H
#define PURIFY_RM_OPERATOR_H

#include "purify/config.h"
#include "purify/FFTOperator.h"
#include "purify/kernels.h"
#include "purify/types.h"
#include "purify/utilities.h"

#include <iostream>
#include <string>
#include <CCfits/CCfits>
#include <unsupported/Eigen/FFT>

namespace purify {

//! This does something
class RMOperator {
public:
  Sparse<t_complex> G;
  Array<t_real> S;
  Array<t_complex> W;
  t_real norm = 1;
  const t_real oversample_factor;
  const t_int imsize;
  const t_int ftsize;

  //! \brief Generates tools/operators needed for gridding and degridding.
  //! \param[in] u: visibilities in units of ftsizeu
  //! \param[in] v: visibilities in units of ftsizev
  //! \param[in] Ju: support size for u axis
  //! \param[in] Jv: support size for v axis
  //! \param[in] kernel_name: flag that determines what kernel to use (gauss, pswf, kb)
  //! \param[in] imsizex: size of image along xaxis
  //! \param[in] imsizey: size of image along yaxis
  //! \param[in] oversample_factor: factor for oversampling the FFT grid
  RMOperator(const utilities::rm_params &rm_vis_input, const t_int &J,
             const std::string &kernel_name, const t_int &imsize, const t_real &oversample_factor,
             const t_real &cell_size = 1, const std::string &weighting_type = "none",
             const t_real &R = 0, bool fft_grid_correction = false);

  //! Degridding operator that degrids image to visibilities
  Vector<t_complex> degrid(const Vector<t_complex> &eigen_image);
  //! Gridding operator that grids image from visibilities
  Vector<t_complex> grid(const Vector<t_complex> &visibilities);

protected:
  Eigen::FFT<t_real> fft;
  //! Match uv coordinates to grid
  Vector<t_real> omega_to_k(const Vector<t_real> &omega);
  //! Generates interpolation matrix from kernels without using w-component
  Sparse<t_complex> init_interpolation_matrix1d(const Vector<t_real> &u, const t_int Ju,
                                                const std::function<t_real(t_real)> kernelu);
  //! Generates scaling factors for gridding correction using an fft
  Array<t_real> init_correction1d_fft(const std::function<t_real(t_real)> kernelu, const t_int Ju);
  //! Generates scaling factors for gridding correction
  Array<t_real> init_correction1d(const std::function<t_real(t_real)> ftkernelu);
  //! Generates and calculates weights
  Array<t_complex> init_weights(const Vector<t_real> &u, const Vector<t_complex> &weights,
                                const t_real &oversample_factor, const std::string &weighting_type,
                                const t_real &R);
  //! Estiamtes norm of operator
  t_real power_method(const t_int niters);
  //! Coveriance matrix column calculation
  Vector<t_complex> covariance_calculation(const Vector<t_complex> &vector);
};
}

#endif
