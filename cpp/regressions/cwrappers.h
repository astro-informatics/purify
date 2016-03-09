#ifndef PURIFY_REGRESSION_CWRAPPERS_H
#define PURIFY_REGRESSION_CWRAPPERS_H

#include "purify_config.h"
#include "purify_types.h"
#include <array>
#include <fftw3.h>
#include <memory>
#include <random>
#include "sopt/linear_transform.h"
#include "sopt/real_type.h"
#include "sopt/types.h"
#include "directories.h"
#include "pfitsio.h"
#include "types.h"
#include "utilities.h"
#include "purify_measurement.h"
#include "purify_sparsemat.h"
#include "purify_visibility.h"

namespace purify {
namespace utilities {
vis_params read_visibilities(std::string const &filename, purify_visibility_filetype const &type) {
  purify_visibility vis_test;
  purify_visibility_readfile(&vis_test, filename.c_str(), type);
  vis_params result;
  result.u = Vector<t_real>::Map(vis_test.u, vis_test.nmeas);
  result.v = Vector<t_real>::Map(vis_test.v, vis_test.nmeas);
  result.vis = Vector<t_complex>::Map(vis_test.y, vis_test.nmeas);
  result.weights = Vector<t_complex>::Map(vis_test.noise_std, vis_test.nmeas);
  result.w = result.u * 0;
  return result;
}
}

//! Bundles C measurement operator in a C++ object
struct CMeasurementOperator {
public:
  CMeasurementOperator(utilities::vis_params const &visibilities, t_uint rows, t_uint cols)
      : visibilities(visibilities), params{static_cast<int>(visibilities.vis.size()),
                                           static_cast<int>(rows),
                                           static_cast<int>(cols),
                                           2,
                                           2,
                                           24,
                                           24},
        deconv(rows * cols), shifts(visibilities.vis.size()),
        fft_forward(rows * cols * params.ofx * params.ofy),
        fft_inverse(rows * cols * params.ofx * params.ofy) {
    forward_plan = fftw_plan_dft_2d(
        rows * params.ofx, cols * params.ofy, reinterpret_cast<fftw_complex *>(fft_forward.data()),
        reinterpret_cast<fftw_complex *>(fft_forward.data()), FFTW_FORWARD, FFTW_MEASURE);
    inverse_plan = fftw_plan_dft_2d(
        rows * params.ofx, cols * params.ofy, reinterpret_cast<fftw_complex *>(fft_inverse.data()),
        reinterpret_cast<fftw_complex *>(fft_inverse.data()), FFTW_BACKWARD, FFTW_MEASURE);
    purify_measurement_init_cft(&gmat, deconv.data(), shifts.data(), visibilities.u.data(),
                                visibilities.v.data(), &params);
  }
  CMeasurementOperator(std::string const &filename, t_uint rows, t_uint cols)
      : CMeasurementOperator(
            utilities::read_visibilities(filename, PURIFY_VISIBILITY_FILETYPE_PROFILE_VIS), rows,
            cols) {}

  virtual ~CMeasurementOperator() {
    fftw_destroy_plan(forward_plan);
    fftw_destroy_plan(inverse_plan);
  }

  //! State array to pass on to C function
  std::array<void *, 6> forward_data() const {
    return {{(void *)&params, (void *)deconv.data(), (void *)&gmat, (void *)&forward_plan,
             (void *)fft_forward.data(), (void *)shifts.data()}};
  }
  //! State array to pass on to C function
  std::array<void *, 6> inverse_data() const {
    return {{(void *)&params, (void *)deconv.data(), (void *)&gmat, (void *)&inverse_plan,
             (void *)fft_inverse.data(), (void *)shifts.data()}};
  }

  //! Transforms to a linear-transform operator
  sopt::LinearTransform<Vector<t_complex>> linear_transform() const {
    auto direct = [this](Vector<t_complex> &out, Vector<t_complex> const &input) {
      auto data = forward_data();
      purify_measurement_cftfwd(out.data(),
                                const_cast<void *>(reinterpret_cast<void const *>(input.data())),
                                data.data());
    };
    std::array<int, 3> const direct_size{{0, 1, static_cast<int>(visibilities.vis.size())}};
    auto adjoint = [this](Vector<t_complex> &out, Vector<t_complex> const &input) {
      auto data = inverse_data();
      purify_measurement_cftadj(out.data(),
                                const_cast<void *>(reinterpret_cast<void const *>(input.data())),
                                data.data());
    };
    std::array<int, 3> const adjoint_size{{0, 1, static_cast<int>(params.nx1 * params.ny1)}};
    return sopt::linear_transform<Vector<t_complex>>(direct, direct_size, adjoint, adjoint_size);
  };

  //! Size of input vector
  std::ptrdiff_t cols() const { return params.nx1 * params.ny1; }
  //! Size of output vector
  std::ptrdiff_t rows() const { return visibilities.vis.size(); }

protected:
  utilities::vis_params visibilities;
  purify_measurement_cparam params;
  Vector<t_real> deconv;
  Vector<t_complex> shifts;
  Vector<t_complex> fft_forward;
  Vector<t_complex> fft_inverse;
  fftw_plan forward_plan;
  fftw_plan inverse_plan;
  purify_sparsemat_row gmat;
};

template <class T>
typename sopt::real_type<T>::type
sigma(sopt::Vector<T> const &y, typename sopt::real_type<T>::type snr = 30e0) {
  return y.stableNorm() / std::sqrt(y.size()) * std::pow(10.0, -(snr / 20.0));
}

template <class T, class RANDOM>
Vector<T> dirty(sopt::Vector<T> const &image, RANDOM &mersenne,
                typename sopt::real_type<T>::type snr = 30e0) {

  // values near the mean are the most likely
  // standard deviation affects the dispersion of generated values from the mean
  std::normal_distribution<> gaussian_dist(0, sigma(image, snr));
  Vector<T> y(image.size());
  for(t_int i = 0; i < image.size(); i++)
    y(i) = image(i) + gaussian_dist(mersenne);

  return y;
}
}
#endif
