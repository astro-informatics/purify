#include <iomanip>
#include "catch.hpp"

#include "purify/directories.h"
#include "purify/kernels.h"
#include "purify/pfitsio.h"
#include "purify/rm_operators.h"
#include "purify/test_data.h"
#include "purify/utilities.h"
#include <sopt/power_method.h>

using namespace purify::notinstalled;
using namespace purify;

TEST_CASE("1d zeropad") {
  const t_int imsize = 8;
  const t_real oversample_ratio = 2;
  Image<t_complex> S = Vector<t_complex>::Constant(imsize, 2);
  auto pad = purify::operators::init_zero_padding_1d<Vector<t_complex>>(S, oversample_ratio);
  const t_int boundary_size = static_cast<t_int>(imsize * oversample_ratio * 0.5 - imsize * 0.5);
  Vector<t_complex> output;
  const Vector<t_complex> input = Vector<t_complex>::Constant(imsize, 5);
  std::get<0>(pad)(output, input);
  CHECK(output.size() == static_cast<t_int>(imsize * oversample_ratio));
  CAPTURE(boundary_size);
  CAPTURE(input);
  CAPTURE(output);
  CHECK(output.head(boundary_size).isApprox(Vector<t_complex>::Zero(boundary_size)));
  CHECK(output.tail(boundary_size).isApprox(Vector<t_complex>::Zero(boundary_size)));
  CHECK(output.segment(boundary_size, imsize).isApprox(Vector<t_complex>::Constant(imsize, 10)));
  // apply adjoint
  Vector<t_complex> b_output;
  std::get<1>(pad)(b_output, output);
  CHECK(b_output.size() == static_cast<t_int>(imsize));
  CHECK(b_output.isApprox(Vector<t_complex>::Constant(imsize, 20)));
}

TEST_CASE("1d FFT") {
  const t_int imsize = 128;
  const t_real oversample_factor = 2;
  const operators::fftw_plan fftw_plan_flag_ = operators::fftw_plan::measure;
  auto fft = operators::init_FFT_1d<Vector<t_complex>>(imsize, oversample_factor, fftw_plan_flag_);
  SECTION("forward") {
    Vector<t_complex> input = Vector<t_complex>::Zero(imsize * oversample_factor);
    input(static_cast<t_int>(0)) = 1.;
    Vector<t_complex> output;
    std::get<0>(fft)(output, input);
    const Vector<t_complex> expected =
        Vector<t_complex>::Ones(imsize * oversample_factor) / std::sqrt(imsize * oversample_factor);
    CAPTURE(output.head(10));
    CAPTURE(expected.head(10));
    CHECK(output.size() == input.size());
    CHECK(output.isApprox(expected, 1e-6));
  }
  SECTION("backward") {
    Vector<t_complex> input = Vector<t_complex>::Zero(imsize * oversample_factor);
    input(static_cast<t_int>(0)) = 1.;
    Vector<t_complex> output;
    std::get<1>(fft)(output, input);
    const Vector<t_complex> expected =
        Vector<t_complex>::Ones(imsize * oversample_factor) / std::sqrt(imsize * oversample_factor);
    CAPTURE(output.head(10));
    CAPTURE(expected.head(10));
    CHECK(output.size() == input.size());
    CHECK(output.isApprox(expected, 1e-6));
  }
}

TEST_CASE("regression_degrid") {
  const t_int imsizex = 256;
  const t_real cell = constant::pi / imsizex;
  const Vector<t_real> u = Vector<t_real>::Random(10) * imsizex / 2;
  const Vector<t_real> widths = Vector<t_real>::Zero(u.size());
  Vector<t_complex> input = Vector<t_complex>::Zero(imsizex);
  input(static_cast<t_int>(imsizex * 0.5)) = 1.;
  const t_uint M = u.size();
  const t_real oversample_ratio = 2;
  const t_int ftsizeu = std::floor(imsizex * oversample_ratio);
  const t_uint Ju = 8;

  const Vector<t_complex> y = Vector<t_complex>::Ones(u.size());
  CAPTURE(u);

  SECTION("kb") {
    const kernels::kernel kernel = kernels::kernel::kb;
    const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measure_op =
        measurementoperator::init_degrid_operator_1d<Vector<t_complex>>(
            u, widths, Vector<t_complex>::Ones(M), imsizex, cell, oversample_ratio, kernel, Ju, 30,
            1e-6, 1e-6);
    const Vector<t_complex> y_test = *measure_op * input;
    CAPTURE(y_test.array() / y.array());
    const t_real max_test = y_test.cwiseAbs().mean();
    CAPTURE(y_test / max_test);
    CAPTURE(y);
    CHECK((y_test / max_test).isApprox((y), 1e-6));
  }
  SECTION("pswf") {
    const kernels::kernel kernel = kernels::kernel::pswf;
    const auto measure_op = measurementoperator::init_degrid_operator_1d<Vector<t_complex>>(
        u, widths, Vector<t_complex>::Ones(M), imsizex, cell, oversample_ratio, kernel, 6, 30, 1e-6,
        1e-6);
    const Vector<t_complex> y_test = *measure_op * input;
    CAPTURE(y_test.array() / y.array());
    const t_real max_test = y_test.cwiseAbs().mean();
    CAPTURE(y_test / max_test);
    CAPTURE(y);
    CHECK((y_test / max_test).isApprox((y), 1e-3));
  }
}

TEST_CASE("flux units") {
  const t_real oversample_ratio = 2;
  Vector<t_real> u = Vector<t_real>::Random(10);
  const Vector<t_real> widths = Vector<t_real>::Zero(u.size());
  const t_uint M = u.size();
  const Vector<t_complex> y = Vector<t_complex>::Ones(u.size());
  SECTION("kb") {
    const kernels::kernel kernel = kernels::kernel::kb;
    for (auto& J : {4, 5, 6, 7, 8}) {
      for (auto& imsize : {128, 256, 512}) {
        const t_real cell = constant::pi / imsize;
        const auto measure_op = measurementoperator::init_degrid_operator_1d<Vector<t_complex>>(
            u * imsize / 2, widths, Vector<t_complex>::Ones(M), imsize, cell, oversample_ratio,
            kernel, J, 30, 1e-6, 1e-6);
        Vector<t_complex> input = Vector<t_complex>::Zero(imsize);
        input(static_cast<t_int>(imsize * 0.5)) = 1.;
        const Vector<t_complex> y_test =
            (*measure_op * input).eval() *
            std::sqrt(static_cast<t_int>(input.size()) * oversample_ratio);
        CAPTURE(y_test.cwiseAbs().mean());
        CAPTURE(y);
        CAPTURE(y_test);
        CAPTURE(J);
        CAPTURE(imsize)
        CHECK(y_test.isApprox(y, 1e-3));
        const Vector<t_complex> psf =
            (measure_op->adjoint() * y) * std::sqrt(input.size() * oversample_ratio) / M;
        CHECK(std::real(psf(static_cast<t_int>(imsize * 0.5))) == Approx(1.).margin(0.001));
      }
    }
  }
  SECTION("pswf") {
    const kernels::kernel kernel = kernels::kernel::pswf;
    for (auto& J : {4, 5, 6, 7, 8}) {
      for (auto& imsize : {128, 256, 512}) {
        const t_real cell = constant::pi / imsize;
        if (J != 6)
          CHECK_THROWS(measurementoperator::init_degrid_operator_1d<Vector<t_complex>>(
              u * imsize / 2, widths, Vector<t_complex>::Ones(M), imsize, cell, oversample_ratio,
              kernel, J, 30, 1e-6, 1e-6));
        else {
          const auto measure_op = measurementoperator::init_degrid_operator_1d<Vector<t_complex>>(
              u * imsize / 2, widths, Vector<t_complex>::Ones(M), imsize, cell, oversample_ratio,
              kernel, J);
          Vector<t_complex> input = Vector<t_complex>::Zero(imsize);
          input(static_cast<t_int>(imsize * 0.5)) = 1.;
          const Vector<t_complex> y_test =
              (*measure_op * input).eval() *
              std::sqrt(static_cast<t_int>(input.size()) * oversample_ratio);
          CAPTURE(y_test.cwiseAbs().mean());
          CAPTURE(y);
          CAPTURE(y_test);
          CAPTURE(J);
          CAPTURE(imsize)
          CHECK(y_test.isApprox(y, 1e-3));
          const Vector<t_complex> psf =
              (measure_op->adjoint() * y) * std::sqrt(input.size() * oversample_ratio) / M;
          CHECK(std::real(psf(static_cast<t_int>(imsize * 0.5))) == Approx(1.).margin(0.001));
        }
      }
    }
  }
}

TEST_CASE("normed operator") {
  const t_real oversample_ratio = 2;
  const t_int power_iters = 1000;
  const t_real power_tol = 1e-4;
  Vector<t_real> u = Vector<t_real>::Random(10);
  const Vector<t_real> widths = Vector<t_real>::Zero(u.size());
  const t_uint M = u.size();
  const Vector<t_complex> y = Vector<t_complex>::Ones(u.size());
  SECTION("kb") {
    const kernels::kernel kernel = kernels::kernel::kb;
    for (auto& J : {4, 5, 6, 7, 8}) {
      for (auto& imsize : {128, 256, 512}) {
        const t_real cell = constant::pi / imsize;
        const Vector<t_complex> init = Vector<t_complex>::Ones(imsize);
        auto power_method_result = sopt::algorithm::normalise_operator<Vector<t_complex>>(
            measurementoperator::init_degrid_operator_1d<Vector<t_complex>>(
                u * imsize / 2, widths, Vector<t_complex>::Ones(M), imsize, cell, oversample_ratio,
                kernel, J, 30, 1e-6, 1e-6),
            power_iters, power_tol, init);
        const t_real norm = std::get<0>(power_method_result);
        const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measure_op =
            std::get<2>(power_method_result);

        Vector<t_complex> input = Vector<t_complex>::Zero(imsize);
        input(static_cast<t_int>(imsize * 0.5)) = 1.;
        const Vector<t_complex> y_test =
            (*measure_op * input).eval() *
            std::sqrt(static_cast<t_int>(input.size()) * oversample_ratio);
        CAPTURE(y_test.cwiseAbs().mean());
        CAPTURE(y);
        CAPTURE(y_test);
        CAPTURE(J);
        CAPTURE(imsize)
        CHECK((y_test * norm).isApprox(y, 1e-3));
        const Vector<t_complex> psf =
            (measure_op->adjoint() * y) * std::sqrt(input.size() * oversample_ratio) / M * norm;
        CHECK(std::real(psf(static_cast<t_int>(imsize * 0.5))) == Approx(1.).margin(0.001));
      }
    }
  }
}
