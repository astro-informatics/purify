#include "leastsquares.h"

namespace purify {
  namespace leastsquares{
    Vector<t_complex> levenberg_marquardt(const std::function<Vector<t_complex>(Vector<t_real>, Vector<t_complex>)> model_function,
      const Vector<t_complex> & y, const Vector<t_complex> & x,
      const Vector<t_real> weights, const Vector<t_complex> & initial_params, total_iterations){
    //Main least squares solving algortihm
     Vector<t_complex> current_params = initial_params;
     t_real lambda = 0.001;
     auto current_model = [&current_params](const Vector<t_real> & x){return model_function(x, current_params);};
     const auto residuals = residual_calculation(y, x, model_function, weights);
     t_real chi2 = chisquared(residuals);
     for (t_int i = 0; i < total_iterations; i++) {
       //Setting up the iteration
        auto new_params = current_params + iteration(current_params, x, model_function, y, weights, lambda);
        auto new_model = [&current_params](const Vector<t_real> & x){return model_function(x, new_params);};
        const chi2_new = chisquared(residual_calculation(y, x, new_model, weights));
        if (chi2_new > chi2) {
          lambda = lambda * 10.;
          i = i - 1;
        }else{
          lambda = lambda * 0.1;
          current_params = new_params;
        }
        if(std::abs(chi2 - chi2_new) < 0.001)
          break;
     }
     return current_params;
    }

    t_real chisquared(const Vector<t_complex> & residuals){
      // Calculate chi squared
      return (residuals.array() * residuals.array()).abs().sum();
    }

    Vector<t_complex> iteration(const Vector<t_complex> & current_params, const Vector<t_real> & x,
        const std::function<Vector<t_complex>(Vector<t_real>, Vector<t_complex>)> model_function,
        const Vector<t_complex> & residuals, const Vector<t_real> weights, const t_real lambda){
       //Calculate parameters from an iteration
       const Matrix<t_complex> J = jacobian_matrix(current_model, x, 0.05);
       const Vector<t_complex> beta = beta_vector(J, weights, residuals);
       const Matrix<t_complex> alpha = alpha_matrix(J, weights, lambda);
       return alpha.ldlt(beta);
    }

    Matrix<t_complex> jacobian_matrix(const std::function<Vector<t_complex>(Vector<t_real>)> model_function,
        const Vector<t_real> x0, const t_real & step_size){
      //Return the Jacobian matrix of a Vector valued function
      const Vector<t_complex> & y = model_function(x0);
      Matrix<t_complex> J = Matrix<t_complex>::Zero(y.size(), x.size());
      auto h = Vector<t_real>::Zero(x0.size());
      for (t_int i = 0; i < x0.size(); i++) {
        h(i) = step_size;
        J.block(0, i, y.size(), 1) = (model_function(x0 + h) - y) / step_size;
        h(i) = 0;
      }
      return J;
    }

    Matrix<t_complex> alpha_matrix(const Matrix<t_complex> & J,
        const Vector<t_real> & weights, const t_real & lambda){
      //
      Matrix<t_complex> alpha = Matrix<t_complex>::Zero(J.rows(), J.rows());
      for (t_int i = 0; i < alpha.rows(); i++) {
        for (t_int j = 0; j < alpha.cols(); j++) {
          for (t_int k = 0; k < J.cols(); k++) {
            alpha(i, j) = alpha(i, j) + 2 * J(i, k) * J(j, k) * weights(k);
          }
          if (i == j)
           alpha(i, j) = alpha(i, j) * (1 + lambda);
        }
      }
      return alpha;
    }

    Vector<t_complex> beta_vector(const Matrix<t_complex> & J, const Vector<t_real> & weights,
          const Vector<t_real> & residuals){
      //
      Vector<t_complex> beta = Vector<t_complex>::Zero(J.rows());
      for (t_int i = 0; i < beta.size(); i++) {
        for (t_int j = 0; j < residuals.size(); j++) {
          beta(i) = beta(i) - 2 * residuals(j) * weights(j) * J(i, j);
        }
      }
      return beta;
    }

    Vector<t_complex> residual_calculation(const Vector<t_complex> & y0, const Vector<t_real> & x,
          const std::function<Vector<t_complex>(Vector<t_complex>)> model_function, const Vector<t_real> & weights){
      //
        Vector<t_complex> y = Vector<t_complex>::Zero(y0.size());
        for (t_int i = 0; i < count; i++) {
          y(i) = model_function(x(i));
        }
        return (y0 - y).array() * weights.array();
    }
  }
}
