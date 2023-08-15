#include <iomanip>
#include "catch.hpp"

#include "purify/directories.h"
#include "purify/kernels.h"
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include "purify/test_data.h"
#include "purify/utilities.h"
#include "purify/wproj_operators.h"
#include <sopt/power_method.h>

using namespace purify::notinstalled;
using namespace purify;

TEST_CASE("regression_degrid") {
  const t_int imsizex = 256;
  const t_int imsizey = 256;
  Vector<t_real> u = Vector<t_real>::Random(10) * imsizex / 2;
  Vector<t_real> v = Vector<t_real>::Random(10) * imsizey / 2;
  Vector<t_complex> input = Vector<t_complex>::Zero(imsizex * imsizey);
  input(static_cast<t_int>(imsizex * 0.5 + imsizey * 0.5 * imsizex)) = 1.;
  const t_uint M = u.size();
  const t_real oversample_ratio = 2;
  const t_int ftsizev = std::floor(imsizey * oversample_ratio);
  const t_int ftsizeu = std::floor(imsizex * oversample_ratio);
  const t_uint Ju = 8;
  const t_uint Jv = 8;

  const Vector<t_complex> y = Vector<t_complex>::Ones(u.size());
  CAPTURE(u);
  CAPTURE(v);

  SECTION("kb") {
    const kernels::kernel kernel = kernels::kernel::kb;
    const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measure_op =
        measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
            u, v, Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M), imsizey, imsizex,
            oversample_ratio, kernel, Ju, Jv);
    const Vector<t_complex> y_test = *measure_op * input;
    CAPTURE(y_test.array() / y.array());
    const t_real max_test = y_test.cwiseAbs().mean();
    CAPTURE(y_test / max_test);
    CAPTURE(y);
    CHECK((y_test / max_test).isApprox((y), 1e-6));
  }
  SECTION("pswf") {
    const kernels::kernel kernel = kernels::kernel::pswf;
    const auto measure_op = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        u, v, Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M), imsizey, imsizex,
        oversample_ratio, kernel, 6, 6);
    const Vector<t_complex> y_test = *measure_op * input;
    CAPTURE(y_test.array() / y.array());
    const t_real max_test = y_test.cwiseAbs().mean();
    CAPTURE(y_test / max_test);
    CAPTURE(y);
    CHECK((y_test / max_test).isApprox((y), 1e-3));
  }
  SECTION("gauss") {
    const kernels::kernel kernel = kernels::kernel::gauss;
    const auto measure_op = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        u, v, Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M), imsizey, imsizex,
        oversample_ratio, kernel, 10, 10);
    const Vector<t_complex> y_test = *measure_op * input;
    CAPTURE(y_test.array() / y.array());
    const t_real max_test = y_test.cwiseAbs().mean();
    CAPTURE(y_test / max_test);
    CAPTURE(y);
    CHECK((y_test / max_test).isApprox((y), 1e-4));
  }
}

TEST_CASE("flux units") {
  const t_real oversample_ratio = 2;
  Vector<t_real> u = Vector<t_real>::Random(10);
  Vector<t_real> v = Vector<t_real>::Random(10);
  const t_uint M = u.size();
  const Vector<t_complex> y = Vector<t_complex>::Ones(u.size());
  SECTION("kb") {
    const kernels::kernel kernel = kernels::kernel::kb;
    for (auto& J : {4, 5, 6, 7, 8}) {
      for (auto& imsize : {128, 256, 512}) {
        const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measure_op =
            measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
                u * imsize / 2, v * imsize / 2, Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M),
                imsize, imsize, oversample_ratio, kernel, J, J);
        Vector<t_complex> input = Vector<t_complex>::Zero(imsize * imsize);
        input(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize)) = 1.;
        const Vector<t_complex> y_test = (*measure_op * input).eval();
        CAPTURE(y_test.cwiseAbs().mean());
        CAPTURE(y);
        CAPTURE(y_test);
        CAPTURE(J);
        CAPTURE(imsize);
        CHECK(y_test.isApprox(y, 1e-3));
        const Vector<t_complex> psf = (measure_op->adjoint() * y) * 1. / M;
        CHECK(std::real(psf(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize))) ==
              Approx(1.).margin(0.001));
      }
    }
  }
  SECTION("pswf") {
    const kernels::kernel kernel = kernels::kernel::pswf;
    for (auto& J : {4, 5, 6, 7, 8}) {
      for (auto& imsize : {128, 256, 512}) {
        if (J != 6)
          CHECK_THROWS(measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
              u * imsize / 2, v * imsize / 2, Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M),
              imsize, imsize, oversample_ratio, kernel, J, J));
        else {
          const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measure_op =
              measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
                  u * imsize / 2, v * imsize / 2, Vector<t_real>::Zero(M),
                  Vector<t_complex>::Ones(M), imsize, imsize, oversample_ratio, kernel, J, J);
          Vector<t_complex> input = Vector<t_complex>::Zero(imsize * imsize);
          input(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize)) = 1.;
          const Vector<t_complex> y_test = (*measure_op * input).eval();
          CAPTURE(y_test.cwiseAbs().mean());
          CAPTURE(y);
          CAPTURE(y_test);
          CAPTURE(J);
          CAPTURE(imsize);
          CHECK(y_test.isApprox(y, 1e-3));
          const Vector<t_complex> psf = (measure_op->adjoint() * y) * 1. / M;
          CHECK(std::real(psf(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize))) ==
                Approx(1.).margin(0.001));
        }
      }
    }
  }
  SECTION("gauss") {
    const kernels::kernel kernel = kernels::kernel::gauss;
    for (auto& J : {4, 5, 6, 7, 8}) {
      for (auto& imsize : {128, 256, 512}) {
        const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measure_op =
            measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
                u * imsize / 2, v * imsize / 2, Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M),
                imsize, imsize, oversample_ratio, kernel, J, J);
        Vector<t_complex> input = Vector<t_complex>::Zero(imsize * imsize);
        input(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize)) = 1.;
        const Vector<t_complex> y_test = (*measure_op * input).eval();
        CAPTURE(y_test.cwiseAbs().mean());
        CAPTURE(y);
        CAPTURE(y_test);
        CAPTURE(J);
        CAPTURE(imsize);
        CHECK(y_test.isApprox(y, 1e-2));
        const Vector<t_complex> psf = (measure_op->adjoint() * y) * 1. / M;
        CHECK(std::real(psf(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize))) ==
              Approx(1.).margin(0.01));
      }
    }
  }
  SECTION("box") {
    const kernels::kernel kernel = kernels::kernel::box;
    for (auto& J : {4, 5, 6, 7, 8}) {
      for (auto& imsize : {128, 256, 512}) {
        const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measure_op =
            measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
                u * imsize / 2, v * imsize / 2, Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M),
                imsize, imsize, oversample_ratio, kernel, J, J);
        Vector<t_complex> input = Vector<t_complex>::Zero(imsize * imsize);
        input(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize)) = 1.;
        const Vector<t_complex> y_test = (*measure_op * input).eval();
        CAPTURE(y_test.cwiseAbs().mean());
        CAPTURE(y);
        CAPTURE(y_test);
        CAPTURE(J);
        CAPTURE(imsize);
        CHECK(y_test.isApprox(y, 1e-3));
        const Vector<t_complex> psf = (measure_op->adjoint() * y) * 1. / M;
        CHECK(std::real(psf(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize))) ==
              Approx(1.).margin(0.001));
      }
    }
  }
  SECTION("wproj kb") {
    const kernels::kernel kernel = kernels::kernel::kb;
    for (auto& J : {4, 5, 6, 7, 8}) {
      for (auto& imsize : {128, 256, 512}) {
        const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measure_op =
            measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
                u * imsize / 2, v * imsize / 2, Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M),
                imsize, imsize, oversample_ratio, kernel, J, J, false, 1e-6, 1e-6);
        Vector<t_complex> input = Vector<t_complex>::Zero(imsize * imsize);
        input(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize)) = 1.;
        const Vector<t_complex> y_test = (*measure_op * input).eval();
        CAPTURE(y_test.cwiseAbs().mean());
        CAPTURE(y);
        CAPTURE(y_test);
        CAPTURE(J);
        CAPTURE(imsize);
        CHECK(y_test.isApprox(y, 1e-3));
        const Vector<t_complex> psf = (measure_op->adjoint() * y) * 1. / M;
        CHECK(std::real(psf(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize))) ==
              Approx(1.).margin(0.001));
      }
    }
  }
  SECTION("wproj pswf") {
    const kernels::kernel kernel = kernels::kernel::pswf;
    for (auto& J : {4, 5, 6, 7, 8}) {
      for (auto& imsize : {128, 256, 512}) {
        if (J != 6)
          CHECK_THROWS(measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
              u * imsize / 2, v * imsize / 2, Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M),
              imsize, imsize, oversample_ratio, kernel, J, J, false, 1e-6, 1e-6));
        else {
          const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measure_op =
              measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
                  u * imsize / 2, v * imsize / 2, Vector<t_real>::Zero(M),
                  Vector<t_complex>::Ones(M), imsize, imsize, oversample_ratio, kernel, J, J, false,
                  1e-6, 1e-6);
          Vector<t_complex> input = Vector<t_complex>::Zero(imsize * imsize);
          input(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize)) = 1.;
          const Vector<t_complex> y_test = (*measure_op * input).eval();
          CAPTURE(y_test.cwiseAbs().mean());
          CAPTURE(y);
          CAPTURE(y_test);
          CAPTURE(J);
          CAPTURE(imsize);
          CHECK(y_test.isApprox(y, 1e-3));
          const Vector<t_complex> psf = (measure_op->adjoint() * y) * 1. / M;
          CHECK(std::real(psf(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize))) ==
                Approx(1.).margin(0.001));
        }
      }
    }
  }
  SECTION("wproj gauss") {
    const kernels::kernel kernel = kernels::kernel::gauss;
    for (auto& J : {4, 5, 6, 7, 8}) {
      for (auto& imsize : {128, 256, 512}) {
        const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measure_op =
            measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
                u * imsize / 2, v * imsize / 2, Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M),
                imsize, imsize, oversample_ratio, kernel, J, J, false, 1e-6, 1e-6);
        Vector<t_complex> input = Vector<t_complex>::Zero(imsize * imsize);
        input(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize)) = 1.;
        const Vector<t_complex> y_test = (*measure_op * input).eval();
        CAPTURE(y_test.cwiseAbs().mean());
        CAPTURE(y);
        CAPTURE(y_test);
        CAPTURE(J);
        CAPTURE(imsize);
        CHECK(y_test.isApprox(y, 1e-2));
        const Vector<t_complex> psf = (measure_op->adjoint() * y) * 1. / M;
        CHECK(std::real(psf(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize))) ==
              Approx(1.).margin(0.01));
      }
    }
  }
  SECTION("wproj box") {
    const kernels::kernel kernel = kernels::kernel::box;
    for (auto& J : {4, 5, 6, 7, 8}) {
      for (auto& imsize : {128, 256, 512}) {
        const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measure_op =
            measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
                u * imsize / 2, v * imsize / 2, Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M),
                imsize, imsize, oversample_ratio, kernel, J, J, false, 1e-6, 1e-6);
        Vector<t_complex> input = Vector<t_complex>::Zero(imsize * imsize);
        input(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize)) = 1.;
        const Vector<t_complex> y_test = (*measure_op * input).eval();
        CAPTURE(y_test.cwiseAbs().mean());
        CAPTURE(y);
        CAPTURE(y_test);
        CAPTURE(J);
        CAPTURE(imsize);
        CHECK(y_test.isApprox(y, 1e-2));
        const Vector<t_complex> psf = (measure_op->adjoint() * y) * 1. / M;
        CHECK(std::real(psf(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize))) ==
              Approx(1.).margin(0.001));
      }
    }
  }
}
TEST_CASE("normed operator") {
  const t_real oversample_ratio = 2;
  const t_int power_iters = 1000;
  const t_real power_tol = 1e-4;
  Vector<t_real> u = Vector<t_real>::Random(10);
  Vector<t_real> v = Vector<t_real>::Random(10);
  const t_uint M = u.size();
  const Vector<t_complex> y = Vector<t_complex>::Ones(u.size());
  SECTION("kb") {
    const kernels::kernel kernel = kernels::kernel::kb;
    for (auto& J : {4, 5, 6, 7, 8}) {
      for (auto& imsize : {128, 256, 512}) {
        const Vector<t_complex> init = Vector<t_complex>::Ones(imsize * imsize);
        auto power_method_result = sopt::algorithm::normalise_operator<Vector<t_complex>>(
            measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
                u * imsize / 2, v * imsize / 2, Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M),
                imsize, imsize, oversample_ratio, kernel, J, J),
            power_iters, power_tol, init);
        const t_real norm = std::get<0>(power_method_result);
        const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measure_op =
            std::get<2>(power_method_result);

        Vector<t_complex> input = Vector<t_complex>::Zero(imsize * imsize);
        input(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize)) = 1.;
        const Vector<t_complex> y_test = (*measure_op * input).eval();
        CAPTURE(y_test.cwiseAbs().mean());
        CAPTURE(y);
        CAPTURE(y_test);
        CAPTURE(J);
        CAPTURE(imsize);
        CHECK((y_test * norm).isApprox(y, 1e-3));
        const Vector<t_complex> psf = (measure_op->adjoint() * y) * 1. / M * norm;
        CHECK(std::real(psf(static_cast<t_int>(imsize * 0.5 + imsize * 0.5 * imsize))) ==
              Approx(1.).margin(0.001));
      }
    }
  }
}
