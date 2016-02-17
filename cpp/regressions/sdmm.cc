#include <catch.hpp>
#include <complex>
#include <iomanip>
#include <iostream>
#include <random>
#include <tuple>
#include <type_traits>
#include <vector>

#include "sopt/relative_variation.h"
#include "sopt/sdmm.h"
#include "sopt/wavelets.h"
#include "sopt/wavelets/sara.h"
#include "cdata.h"
#include "cwrappers.h"
#include "types.h"
#include "utilities.h"
#include "sopt_l1.h"
#include "sopt_sara.h"

typedef std::complex<double> Scalar;
typedef sopt::real_type<Scalar>::type Real;
typedef purify::Vector<Scalar> t_Vector;
typedef purify::Matrix<Scalar> t_Matrix;

sopt::algorithm::SDMM<Scalar> create_sdmm(purify::CMeasurementOperator const &measurements,
                                          sopt::LinearTransform<t_Vector> const &psi,
                                          t_Vector const &y, sopt_l1_sdmmparam const &params) {

  using namespace sopt;
  return algorithm::SDMM<Scalar>()
      .itermax(params.max_iter)
      .gamma(params.gamma)
      .conjugate_gradient(params.cg_max_iter, params.cg_tol)
      .is_converged(RelativeVariation<Scalar>(params.rel_obj))
      .append(sopt::proximal::translate(sopt::proximal::L2Ball<t_complex>(params.epsilon), -y),
              measurements.linear_transform())
      .append(proximal::l1_norm<Scalar>, psi.adjoint(), psi)
      .append(sopt::proximal::positive_quadrant<t_complex>);
}

sopt::wavelets::SARA create_sara(sopt_sara_param const &params) {
  if(params.ndict == 0)
    throw std::runtime_error("Empty dictionary");

  std::vector<std::tuple<std::string, sopt::t_uint>> result;
  for(int i(0); i < params.ndict; ++i)
    switch(params.wav_params[i].type) {
#define PURIFY_MACRO(TYPE)                                                                         \
  case SOPT_WAVELET_##TYPE:                                                                        \
    result.emplace_back(#TYPE, params.wav_params[i].nb_levels);                                    \
    break;
      PURIFY_MACRO(DB1);
      PURIFY_MACRO(DB2);
      PURIFY_MACRO(DB3);
      PURIFY_MACRO(DB4);
      PURIFY_MACRO(DB5);
      PURIFY_MACRO(DB6);
      PURIFY_MACRO(DB7);
      PURIFY_MACRO(DB8);
      PURIFY_MACRO(DB9);
      PURIFY_MACRO(DB10);
#undef PURIFY_MACRO
    default:
      throw std::runtime_error("Wavelet type not implemented");
      break;
    }
  return {result.begin(), result.end()};
}

TEST_CASE("Compare SDMMS", "") {
  using namespace purify;
  using namespace purify::notinstalled;

  // Read perfect image first
  auto const M31 = pfitsio::read2d(image_filename("M31.fits"));
  purify::CMeasurementOperator const measurements(image_filename("Coverages/cont_sim4.vis"),
                                                  M31.rows(), M31.cols());

  // Creates dirty data from image, measurement operator, and random distribution
  std::mt19937_64 mersenne;
  Vector<t_complex> const y0
      = measurements.linear_transform() * Vector<t_complex>::Map(M31.data(), M31.size());
  auto const y = dirty(y0, mersenne, 30e0);
  auto const epsilon
      = std::sqrt(y0.size() + 2 * std::sqrt(y0.size())) * sigma(y0, 30e0)
        / std::sqrt(static_cast<t_real>(y0.size()) / static_cast<t_real>(M31.size()));

  // Create SARA operators
  std::array<sopt_wavelet_type, 3> wavelet_types{SOPT_WAVELET_DB1, SOPT_WAVELET_DB2,
                                                 SOPT_WAVELET_DB3};
  sopt_sara_param sara_params{wavelet_types.size(), std::is_same<Scalar, Real>::value ? 1 : 0,
                              nullptr};
  sopt_sara_initop(&sara_params, M31.rows(), M31.cols(), 4, wavelet_types.data());
  auto const sara = ::create_sara(sara_params);
  auto const psi = sopt::linear_transform<t_complex>(sara, M31.rows(), M31.cols());

  // Create SDMM
  sopt_l1_sdmmparam params = {
      0,                                                                        // verbosity
      50,                                                                       // max iter
      (measurements.linear_transform().adjoint() * y).real().maxCoeff() * 1e-3, // gamma
      0.01,                                                                     // relative change
      epsilon, // radius of the l2ball
      1e-3,    // Relative tolerance on epsilon
      std::is_same<Scalar, sopt::real_type<Scalar>::type>::value ? 1 : 0, // real data
      200,                                                                // cg max iter
      1e-18,                                                              // cg tol
  };
  auto sdmm = ::create_sdmm(measurements, psi, y, params);

  // Extra variables for calls to C
  Vector<Real> l1_weights = Vector<Real>::Ones(M31.size() * sara.size());
  auto forward_data = measurements.forward_data();
  auto inverse_data = measurements.inverse_data();

  auto check = [params, sdmm, sara, forward_data, inverse_data, y, l1_weights, M31,
                sara_params](t_int niters) mutable {
    auto const cpp = sdmm.itermax(niters)(t_Vector::Zero(M31.size()));
    CHECK(cpp.niters == niters);

    params.max_iter = niters;
    t_Vector c = t_Vector::Zero(M31.size());
    void *data[] = {(void *)&sara_params};
    sopt_l1_sdmm((void *)c.data(), c.size(), &purify_measurement_cftfwd, forward_data.data(),
                 &purify_measurement_cftadj, inverse_data.data(), &sopt_sara_synthesisop, data,
                 &sopt_sara_analysisop, data, c.size() * sara.size(), (void *)y.data(), y.size(),
                 l1_weights.data(), params);

    CHECK(cpp.out.isApprox(c, 1e-8));
  };

  SECTION("With 1 iteration") { check(1); }
  SECTION("With 2 iteration") { check(2); }
  SECTION("With 5 iteration") { check(5); }

  sopt_sara_free(&sara_params);
}
