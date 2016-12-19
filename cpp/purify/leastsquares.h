#ifndef PURIFY_LEASTSQUARES_H
#define PURIFY_LEASTSQUARES_H

#include "purify/config.h"
#include "purify/types.h"
#include "purify/utilities.h"
#include <iostream>
#include <string>

namespace purify {

  namespace leastsquares {
      //! Perform non-linear least squares using Levenberg Marquardt algorithm
      Vector<t_complex> levenberg_marquardt(const std::function<Vector<t_complex>(Vector<t_real>, Vector<t_complex>)> model_function,
        const Vector<t_complex> & y, const Vector<t_complex> & x,
        const Vector<t_real> weights, const Vector<t_complex> & initial_params);
      //! Calculate chi squared from residuals
      t_real chisquared(const Vector<t_complex> & residuals);
      //! Performs an iteration of returning new parameter step
      Vector<t_complex> iteration(const Vector<t_complex> & current_params, const Vector<t_real> & x,
        const std::function<Vector<t_complex>(Vector<t_real>, Vector<t_complex>)> model_function,
        const Vector<t_complex> & residuals, const Vector<t_real> weights, const t_real lambda);
      //! Calculate Jacobian matrix
      Matrix<t_complex> jacobian_matrix(const
          std::function<Vector<t_complex>(Vector<t_complex>)> model_function);
      //! Calculate alpha matrix
      Matrix<t_complex> alpha_matrix(const Matrix<t_complex> & J, const Vector<t_real> & weights,
          const std::function<Vector<t_complex>(Vector<t_complex>)> model_function);
      //! Calculate beta vector
      Vector<t_complex> beta_vector(const Matrix<t_complex> & J, const Vector<t_real> & weights,
          const Vector<t_real> & residuals);
      //! Calculate residuals for chi squared
      Vector<t_complex> residual_calculation(const Vector<t_complex> & y0, const Vector<t_real> & x,
          const std::function<Vector<t_complex>(Vector<t_complex>)> model_function, const Vector<t_real> & weights);

  }
}

#endif
