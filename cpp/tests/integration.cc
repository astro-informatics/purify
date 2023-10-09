#include "purify/config.h"
#include "purify/types.h"
#include <iostream>
#include "catch2/catch_all.hpp"
#include "purify/directories.h"
#include "purify/logging.h"

#include "purify/integration.h"
#include "purify/kernels.h"
using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("integration") {
  const t_uint max_evaluations = 1000;
  std::function<t_real(Vector<t_real>)> func;
  std::function<t_complex(Vector<t_real>)> cfunc;
  for (auto int_method : {integration::method::p, integration::method::h}) {
    SECTION("1d") {
      const t_uint ndim = 1;
      const Vector<t_real> xmin = -Vector<t_real>::Ones(ndim);
      const Vector<t_real> xmax = Vector<t_real>::Ones(ndim);
      func = [=](const Vector<t_real> &x) {
        assert(x.size() == ndim);
        return (x.array() * x.array()).sum();
      };
      cfunc = [=](const Vector<t_real> &x) {
        assert(x.size() == ndim);
        return (x.array() * x.array()).sum();
      };
      const t_real result = integration::integrate(xmin, xmax, func, integration::norm_type::l2,
                                                   1e-4, 1e-4, max_evaluations, int_method);
      const t_complex cresult =
          integration::integrate(xmin, xmax, cfunc, integration::norm_type::paired, 1e-4, 1e-4,
                                 max_evaluations, integration::method::p);
      CAPTURE(result);
      CAPTURE(cresult);
      CHECK(std::abs(result - ndim * std::pow(2, ndim - 1) * 2. / 3) < 1e-4);
      CHECK(std::abs(cresult - ndim * std::pow(2, ndim - 1) * 2. / 3) < 1e-4);
    }
    SECTION("2d") {
      const t_uint ndim = 2;
      const Vector<t_real> xmin = -Vector<t_real>::Ones(ndim);
      const Vector<t_real> xmax = Vector<t_real>::Ones(ndim);
      func = [=](const Vector<t_real> &x) {
        assert(x.size() == ndim);
        return (x.array() * x.array()).sum();
      };
      cfunc = [=](const Vector<t_real> &x) {
        assert(x.size() == ndim);
        return (x.array() * x.array()).sum();
      };
      const t_real result = integration::integrate(xmin, xmax, func, integration::norm_type::l2,
                                                   1e-4, 1e-4, max_evaluations, int_method);
      const t_complex cresult =
          integration::integrate(xmin, xmax, cfunc, integration::norm_type::paired, 1e-4, 1e-4,
                                 max_evaluations, int_method);
      CAPTURE(result);
      CAPTURE(cresult);
      CHECK(std::abs(result - ndim * std::pow(2, ndim - 1) * 2. / 3) < 1e-4);
      CHECK(std::abs(cresult - ndim * std::pow(2, ndim - 1) * 2. / 3) < 1e-4);
    }
    SECTION("3d") {
      const t_uint ndim = 3;
      const Vector<t_real> xmin = -Vector<t_real>::Ones(ndim);
      const Vector<t_real> xmax = Vector<t_real>::Ones(ndim);
      func = [=](const Vector<t_real> &x) {
        assert(x.size() == ndim);
        return (x.array() * x.array()).sum();
      };
      cfunc = [=](const Vector<t_real> &x) {
        assert(x.size() == ndim);
        return (x.array() * x.array()).sum();
      };
      const t_real result = integration::integrate(xmin, xmax, func, integration::norm_type::l2,
                                                   1e-4, 1e-4, max_evaluations, int_method);
      const t_complex cresult =
          integration::integrate(xmin, xmax, cfunc, integration::norm_type::paired, 1e-4, 1e-4,
                                 max_evaluations, integration::method::p);
      CAPTURE(result);
      CAPTURE(cresult);
      CHECK(std::abs(result - ndim * std::pow(2, ndim - 1) * 2. / 3) < 1e-4);
      CHECK(std::abs(cresult - ndim * std::pow(2, ndim - 1) * 2. / 3) < 1e-4);
    }
  }
}

TEST_CASE("complex") {
  const t_uint max_evaluations = 100000;
  std::function<t_complex(Vector<t_real>)> cfunc1;
  for (auto int_method : {integration::method::p, integration::method::h}) {
    SECTION("Fourier Series") {
      const t_uint ndim = 1;
      const Vector<t_real> xmin = -Vector<t_real>::Ones(ndim) * constant::pi;
      const Vector<t_real> xmax = Vector<t_real>::Ones(ndim) * constant::pi;
      const t_real n = 1.;
      const t_complex I(0., 1.);
      cfunc1 = [=](const Vector<t_real> &x) {
        assert(x.size() == ndim);
        return (x(0) >= 0) ? std::exp(I * n * x(0)) * 1. / 2. : -std::exp(I * n * x(0)) * 1. / 2.;
      };
      const t_complex cresult = integration::integrate(
          xmin, xmax, cfunc1, integration::norm_type::l2, 1e-6, 1e-6, max_evaluations, int_method);
      CAPTURE(cresult);
      CHECK(std::abs(cresult - t_complex(0., 2.)) / 2. < 1e-4);
    }
    SECTION("Fourier Series omp") {
      const t_uint ndim = 1;
      const Vector<t_real> xmin = -Vector<t_real>::Ones(ndim) * constant::pi;
      const Vector<t_real> xmax = Vector<t_real>::Ones(ndim) * constant::pi;
      const t_real n = 1.;
      const t_complex I(0., 1.);
      std::vector<std::function<t_complex(Vector<t_real>)>> funcs;
      for (int i = 0; i < 4; i++) {
        funcs.push_back([=](const Vector<t_real> &x) -> t_complex {
          assert(x.size() == ndim);
          return ((x(0) >= 0) ? std::exp(I * n * x(0)) * 1. / 2.
                              : -std::exp(I * n * x(0)) * 1. / 2.) *
                 (i + 1.);
        });
      }
#pragma omp parallel for
      for (int i = 0; i < funcs.size(); i++) {
        const t_complex cresult =
            integration::integrate(xmin, xmax, funcs.at(i), integration::norm_type::l2, 1e-6, 1e-6,
                                   max_evaluations, int_method);
        CAPTURE(cresult);
        CHECK(std::abs(cresult - t_complex(0., 2. * static_cast<t_real>(i + 1))) /
                  (2. * static_cast<t_real>(i + 1)) <
              1e-4);
      }
    }
  }
}

TEST_CASE("Numerical_Fourier_transform") {
  const t_uint max_evaluations = 1e7;
  const t_uint J = 4;
  const t_real w = 10;
  const t_real u = 10;
  const t_real v = 10;
  std::function<t_complex(Vector<t_real>)> cfunc;
  const t_uint ndim = 2;
  const Vector<t_real> xmin = Vector<t_real>::Zero(ndim);
  Vector<t_real> xmax = Vector<t_real>::Ones(ndim);
  xmax(0) = 0.99;
  xmax(1) = 2 * constant::pi;
  t_real width = 1.;
  const t_complex I(0., 1.);
  t_uint evals = 0;
  cfunc = [=, &evals](const Vector<t_real> &x) -> t_complex {
    assert(x.size() == ndim);
    evals++;
    return std::exp(-2 * constant::pi * I *
                    (x(0) * std::cos(x(1)) * u + x(0) * std::sin(x(1)) * v +
                     w * (std::sqrt(1 - std::pow(x(0), 2)) - 1))) *
           x(0) / std::sqrt(1 - std::pow(x(0), 2)) / (2 * constant::pi);
  };
  const t_complex cresult =
      integration::integrate(xmin, xmax, cfunc, integration::norm_type::paired, 1e-5, 1e-5,
                             max_evaluations, integration::method::p);
  CAPTURE(cresult);
  CAPTURE(evals);
  CHECK(std::abs(cresult - t_complex(0.007601618963237, -0.003167113583181)) / std::abs(cresult) <
        1e-6);
}
