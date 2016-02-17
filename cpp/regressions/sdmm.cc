#include <catch.hpp>
#include <complex>
#include <iomanip>
#include <iostream>
#include <random>
#include <type_traits>

#include "sopt/relative_variation.h"
#include "sopt/sdmm.h"
#include "sopt/utility.h"
#include "sopt/wavelets.h"
#include "sopt/wavelets/sara.h"
#include "cdata.h"
#include "cwrappers.h"
#include "types.h"
#include "utilities.h"
#include "sopt_l1.h"

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

TEST_CASE("Compare SDMMS", "") {
  using namespace purify;
  using namespace purify::notinstalled;

  auto const M31 = pfitsio::read2d(image_filename("M31.fits"));
  purify::CMeasurementOperator const measurements(image_filename("Coverages/cont_sim4.vis"),
                                                  M31.rows(), M31.cols());
  sopt::wavelets::SARA const sara{std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u)};
  auto const psi = sopt::linear_transform<t_complex>(sara, M31.rows(), M31.cols());

  std::mt19937_64 mersenne;
  Vector<t_complex> const y0
      = measurements.linear_transform() * Vector<t_complex>::Map(M31.data(), M31.size());
  auto const y = dirty(y0, mersenne, 30e0);
  auto const epsilon
      = std::sqrt(y0.size() + 2 * std::sqrt(y0.size())) * sigma(y0, 30e0)
        / std::sqrt(static_cast<t_real>(y0.size()) / static_cast<t_real>(M31.size()));

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

  // Create c++ SDMM
  auto const wavelet = sopt::wavelets::factory("DB2", 2);

  // Create C bindings for C++ operators
  CData<Scalar> const psi_data{static_cast<std::ptrdiff_t>(M31.size() * sara.size()), M31.size(),
                               psi, 0, 0};

  // Try increasing number of iterations and check output of c and c++ algorithms are the same
  for(t_uint i : {1, 2, 5}) {
    SECTION(fmt::format("With {} iterations", i)) {
      sopt_l1_sdmmparam c_params = params;
      c_params.max_iter = i;
      c_params.cg_tol *= 0.5;
      auto sdmm = ::create_sdmm(measurements, psi, y, c_params);
      t_Vector cpp(M31.size());
      auto const result = sdmm(cpp, t_Vector::Zero(M31.size()));
      CHECK(result.niters == i);

      t_Vector c = t_Vector::Zero(M31.size());
      Vector<Real> l1_weights = Vector<Real>::Ones(M31.size() * sara.size());
      auto forward_data = measurements.forward_data();
      auto inverse_data = measurements.inverse_data();
      sopt_l1_sdmm((void *)c.data(), c.size(), &purify_measurement_cftfwd, forward_data.data(),
                   &purify_measurement_cftadj, inverse_data.data(), &direct_transform<Scalar>,
                   (void **)&psi_data, &adjoint_transform<Scalar>, (void **)&psi_data,
                   c.size() * sara.size(), (void *)y.data(), y.size(), l1_weights.data(), c_params);

      CHECK(cpp.isApprox(c, 1e-8));
    }
  };
}
