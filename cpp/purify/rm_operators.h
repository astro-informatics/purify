#ifndef PURIFY_RM_OPERATOR_H
#define PURIFY_RM_OPERATOR_H

#include "purify/config.h"
#include "purify/types.h"

#include "purify/operators.h"

namespace purify {

namespace details {

//! Construct gridding matrix
Sparse<t_complex> init_gridding_matrix_1d(const Vector<t_real> &u, const Vector<t_complex> &weights,
                                          const t_uint imsizex_, const t_real oversample_ratio,
                                          const std::function<t_real(t_real)> kernelu,
                                          const t_uint Ju);

//! Given the Fourier transform of a gridding kernel, creates the scaling image for gridding
//! correction.
Image<t_complex> init_correction1d(const t_real oversample_ratio, const t_uint imsizex_,
                                   const std::function<t_real(t_real)> ftkernelu);

}  // namespace details

namespace operators {

//! constructs lambdas that apply degridding matrix with adjoint
template <class T, class... ARGS>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> init_gridding_matrix_1d(
    ARGS &&... args) {
  const std::shared_ptr<const Sparse<t_complex>> interpolation_matrix =
      std::make_shared<const Sparse<t_complex>>(
          details::init_gridding_matrix_1d(std::forward<ARGS>(args)...));
  const std::shared_ptr<const Sparse<t_complex>> adjoint =
      std::make_shared<const Sparse<t_complex>>(interpolation_matrix->adjoint());

  return std::make_tuple(
      [=](T &output, const T &input) {
        output = utilities::sparse_multiply_matrix(*interpolation_matrix, input);
      },
      [=](T &output, const T &input) {
        output = utilities::sparse_multiply_matrix(*adjoint, input);
      });
}

//! Construsts zero padding operator
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> init_zero_padding_1d(
    const Image<typename T::Scalar> &S, const t_real oversample_ratio) {
  const t_uint imsizex_ = S.cols();
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);
  auto direct = [=](T &output, const T &x) {
    assert(x.size() == imsizex_);
    output = Vector<t_complex>::Zero(ftsizeu_);
#pragma omp parallel for
    for (t_uint j = 0; j < imsizex_; j++) output(x_start + j) = S(j) * x(j);
  };
  auto indirect = [=](T &output, const T &x) {
    assert(x.size() == ftsizeu_);
    output = T::Zero(imsizex_);
#pragma omp parallel for
    for (t_uint j = 0; j < imsizex_; j++) output(j) = std::conj(S(j)) * x(x_start + j);
  };
  return std::make_tuple(direct, indirect);
}  // namespace operators

//! Construsts FFT operator
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> init_FFT_1d(
    const t_uint imsizex_, const t_real oversample_factor_,
    const fftw_plan fftw_plan_flag_ = fftw_plan::measure) {
  t_int const ftsizeu_ = std::floor(imsizex_ * oversample_factor_);
  t_int plan_flag = (FFTW_MEASURE | FFTW_PRESERVE_INPUT);
  switch (fftw_plan_flag_) {
  case (fftw_plan::measure):
    plan_flag = (FFTW_MEASURE | FFTW_PRESERVE_INPUT);
    break;
  case (fftw_plan::estimate):
    plan_flag = (FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
    break;
  }

#ifdef PURIFY_OPENMP_FFTW
  PURIFY_LOW_LOG("Using OpenMP threading with FFTW.");
  fftw_init_threads();
#endif
  Vector<typename T::Scalar> src = Vector<t_complex>::Zero(ftsizeu_);
  Vector<typename T::Scalar> dst = Vector<t_complex>::Zero(ftsizeu_);
  // creating plans
  const auto del = [](fftw_plan_s *plan) { fftw_destroy_plan(plan); };
  // fftw plan with threads needs to be used before each fftw_plan is created
#ifdef PURIFY_OPENMP_FFTW
  fftw_plan_with_nthreads(omp_get_max_threads());
#endif
  const std::shared_ptr<fftw_plan_s> m_plan_forward(
      fftw_plan_dft_1d(ftsizeu_, reinterpret_cast<fftw_complex *>(src.data()),
                       reinterpret_cast<fftw_complex *>(dst.data()), FFTW_FORWARD, plan_flag),
      del);
  // fftw plan with threads needs to be used before each fftw_plan is created
#ifdef PURIFY_OPENMP_FFTW
  fftw_plan_with_nthreads(omp_get_max_threads());
#endif
  const std::shared_ptr<fftw_plan_s> m_plan_inverse(
      fftw_plan_dft_1d(ftsizeu_, reinterpret_cast<fftw_complex *>(src.data()),
                       reinterpret_cast<fftw_complex *>(dst.data()), FFTW_BACKWARD, plan_flag),
      del);
  auto const direct = [m_plan_forward, ftsizeu_](T &output, const T &input) {
    assert(input.size() == ftsizeu_);
    output = Vector<typename T::Scalar>::Zero(input.size());
    fftw_execute_dft(
        m_plan_forward.get(),
        const_cast<fftw_complex *>(reinterpret_cast<const fftw_complex *>(input.data())),
        reinterpret_cast<fftw_complex *>(output.data()));
    output /= std::sqrt(output.size());
  };
  auto const indirect = [m_plan_inverse, ftsizeu_](T &output, const T &input) {
    assert(input.size() == ftsizeu_);
    output = Vector<typename T::Scalar>::Zero(input.size());
    fftw_execute_dft(
        m_plan_inverse.get(),
        const_cast<fftw_complex *>(reinterpret_cast<const fftw_complex *>(input.data())),
        reinterpret_cast<fftw_complex *>(output.data()));
    output /= std::sqrt(output.size());
  };
  return std::make_tuple(direct, indirect);
}

template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> base_padding_and_FFT_1d(
    const std::function<t_real(t_real)> ftkernelu, const t_uint imsizex,
    const t_real oversample_ratio = 2, const fftw_plan &ft_plan = fftw_plan::measure) {
  sopt::OperatorFunction<T> directZ, indirectZ;
  sopt::OperatorFunction<T> directFFT, indirectFFT;

  const Image<t_complex> S =
      purify::details::init_correction1d(oversample_ratio, imsizex, ftkernelu);
  PURIFY_LOW_LOG("Building Measurement Operator: WGFZDB");
  PURIFY_LOW_LOG(
      "Constructing Zero Padding "
      "and Correction Operator: "
      "ZDB");
  PURIFY_MEDIUM_LOG("Faraday Depth size (width): {}", imsizex);
  PURIFY_MEDIUM_LOG("Oversampling Factor: {}", oversample_ratio);
  std::tie(directZ, indirectZ) = purify::operators::init_zero_padding_1d<T>(S, oversample_ratio);
  PURIFY_LOW_LOG("Constructing FFT operator: F");
  switch (ft_plan) {
  case fftw_plan::measure:
    PURIFY_MEDIUM_LOG("Measuring Plans...");
    break;
  case fftw_plan::estimate:
    PURIFY_MEDIUM_LOG("Estimating Plans...");
    break;
  }
  std::tie(directFFT, indirectFFT) =
      purify::operators::init_FFT_1d<T>(imsizex, oversample_ratio, ft_plan);
  auto direct = sopt::chained_operators<T>(directFFT, directZ);
  auto indirect = sopt::chained_operators<T>(indirectZ, indirectFFT);
  return std::make_tuple(direct, indirect);
}

template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> base_degrid_operator_1d(
    const Vector<t_real> &u, const Vector<t_complex> &weights, const t_uint imsizey,
    const t_uint imsizex, const t_real oversample_ratio = 2,
    const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4, const t_uint Jv = 4,
    const fftw_plan ft_plan = fftw_plan::measure) {
  std::function<t_real(t_real)> kernelu, ftkernelu;
  std::tie(kernelu, ftkernelu) = purify::create_radial_ftkernel(kernel, Ju, oversample_ratio);
  sopt::OperatorFunction<T> directFZ, indirectFZ;
  std::tie(directFZ, indirectFZ) =
      base_padding_and_FFT_1d<T>(ftkernelu, imsizex, oversample_ratio, ft_plan);
  sopt::OperatorFunction<T> directG, indirectG;
  PURIFY_LOW_LOG("Constructing Weighting and Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of channels: {}", u.size());
  std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_1d<T>(
      u, weights, imsizex, oversample_ratio, kernelu, Ju);
  auto direct = sopt::chained_operators<T>(directG, directFZ);
  auto indirect = sopt::chained_operators<T>(indirectFZ, indirectG);
  PURIFY_LOW_LOG("Finished consturction of RM Φ.");
  return std::make_tuple(direct, indirect);
}

}  // namespace operators

namespace measurementoperator {

//! Returns linear transform that is the standard degridding operator
template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_1d(
    const Vector<t_real> &u, const Vector<t_complex> &weights, const t_uint imsizex,
    const t_real oversample_ratio = 2, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4) {
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::operators::base_degrid_operator_1d<T>(
      u, weights, imsizex, oversample_ratio, kernel, Ju);
  return std::make_shared<sopt::LinearTransform<T>>(directDegrid, M, indirectDegrid, N);
}

template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_1d(
    const utilities::vis_params &uv_vis_input, const t_uint imsizex,
    const t_real oversample_ratio = 2, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4) {
  auto uv_vis = uv_vis_input;
  //need to find a way to set RM resolution, i.e. Faraday pixel size
  return init_degrid_operator_1d<T>(uv_vis.u, uv_vis.weights, imsizex, oversample_ratio, kernel,
                                    Ju);
}

}  // namespace measurementoperator
};  // namespace purify
#endif
