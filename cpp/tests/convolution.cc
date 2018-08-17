#include "purify/config.h"
#include "purify/types.h"
#include <iostream>
#include "catch.hpp"
#include "purify/directories.h"
#include "purify/logging.h"

#include "purify/convolution.h"

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("1d_zeropad") {
  const Vector<t_int> signal = Vector<t_int>::Random(3);
  for (int i = 1; i < 5; i++) {
    const Vector<t_int> output = convol::zero_pad(signal, i);
    CHECK(output.size() == signal.size() + 2 * i);
    for (int j = 0; j < output.size(); j++) {
      if (j < i) CHECK(output(j) == 0);
      if (j > (i + signal.size())) CHECK(output(j) == 0);
    }
  }
}
TEST_CASE("1d_convolution") {
  Vector<t_int> tri = Vector<t_int>::Zero(3);
  tri << 1, 2, 1;
  Vector<t_int> signal = convol::zero_pad<t_int>(Vector<t_int>::Random(4), tri.size());
  const Vector<t_int> output = convol::linear_convol_1d(tri, signal);
  CHECK(output.size() == signal.size() - tri.size() + 1);
  for (int i = 0; i < signal.size() - tri.size() - 1; i++)
    CHECK(output(i) == tri(0) * signal(output.size() - i - 1) +
                           tri(1) * signal(output.size() - i - 1 + 1) +
                           tri(2) * signal(output.size() - i - 1 + 2));
}
TEST_CASE("2d_convolution") {
  const Vector<t_real> kernelu = Vector<t_real>::Random(3);
  const Vector<t_real> kernelv = Vector<t_real>::Random(2);
  const Matrix<t_real> signal = Matrix<t_real>::Random(2, 2);
  const Matrix<t_real> output = convol::linear_convol_2d(kernelu, kernelv, signal);
  CHECK(output.cols() == signal.cols() + kernelu.size() - 1);
  CHECK(output.rows() == signal.rows() + kernelv.size() - 1);
  const Matrix<t_real> test_buff =
      convol::zero_pad<t_real>(signal, kernelv.size() - 1, kernelu.size() - 1);
  // checking for signs that there is a boundary of zero, suggesting an offset
  CHECK(output(0, 0) != output(1, 0));
  CHECK(output(output.rows() - 1, output.cols() - 1) !=
        output(output.rows() - 1, output.cols() - 2));
  for (int i = 0; i < output.cols(); i++) {
    for (int j = 0; j < output.rows(); j++) {
      CHECK(std::abs(
                output(j, i) -
                kernelv(0) *
                    (kernelu(0) * test_buff(output.rows() - j - 1, output.cols() - i - 1) +
                     kernelu(1) * test_buff(output.rows() - j - 1, output.cols() - i + 1 - 1) +
                     kernelu(2) * test_buff(output.rows() - j - 1, output.cols() - i - 1 + 2)) -
                kernelv(1) *
                    (kernelu(0) * test_buff(output.rows() - 1 - j + 1, output.cols() - i - 1) +
                     kernelu(1) * test_buff(output.rows() - 1 - j + 1, output.cols() - i - 1 + 1) +
                     kernelu(2) *
                         test_buff(output.rows() - 1 - j + 1, output.cols() - i - 1 + 2))) < 1e-7);
    }
  }
}
TEST_CASE("2d_convolution_functions") {
  const t_int Ju = 3;
  const t_int Jv = 4;
  SECTION("box and delta") {
    const t_int Jw = 5;
    auto convol_kernelu = [=](const t_int ju) -> t_complex { return 1.; };
    auto convol_kernelv = [=](const t_int jv) -> t_complex { return 1.; };
    auto convol_kernelw = [=](const t_int jv, const t_int ju) -> t_complex {
      return ((std::floor(ju - (Jw - 1) * 0.5) == 0) and (std::floor(jv - (Jw - 1) * 0.5) == 0))
                 ? 1.
                 : 0.;
    };
    const Matrix<t_complex> projection_kernel = convol::linear_convol_2d<t_complex>(
        convol_kernelu, convol_kernelv, convol_kernelw, static_cast<t_int>(Ju),
        static_cast<t_int>(Jv), static_cast<t_int>(Jw), static_cast<t_int>(Jw));
    CAPTURE(projection_kernel);
    CHECK(projection_kernel.cols() == (Ju + Jw - 1));
    CHECK(projection_kernel.rows() == (Jv + Jw - 1));
    CHECK(projection_kernel.block(2, 2, Jv, Ju)
              .isApprox(Matrix<t_complex>::Constant(Jv, Ju, 1.), 1e-12));
  }
  SECTION("delta and delta") {
    const t_int Jw = 5;
    auto convol_kernelu = [=](const t_int ju) -> t_complex {
      return (std::floor(ju - (Ju - 1) * 0.5) == 0) ? 1. : 0.;
    };
    auto convol_kernelv = [=](const t_int jv) -> t_complex {
      return (std::floor(jv - (Jv - 1) * 0.5) == 0) ? 1. : 0.;
    };
    auto convol_kernelw = [=](const t_int jv, const t_int ju) -> t_complex {
      return ((std::floor(ju - (Jw - 1) * 0.5) == 0) and (std::floor(jv - (Jw - 1) * 0.5) == 0))
                 ? 1.
                 : 0.;
    };
    const Matrix<t_complex> projection_kernel = convol::linear_convol_2d<t_complex>(
        convol_kernelu, convol_kernelv, convol_kernelw, static_cast<t_int>(Ju),
        static_cast<t_int>(Jv), static_cast<t_int>(Jw), static_cast<t_int>(Jw));
    CAPTURE(projection_kernel);
    CHECK(projection_kernel.cols() == (Ju + Jw - 1));
    CHECK(projection_kernel.rows() == (Jv + Jw - 1));
    CHECK(projection_kernel(4, 3) == 1.);
  }
  SECTION("delta_size_1") {
    const t_int Jw = 1;
    auto convol_kernelu = [=](const t_int ju) -> t_complex {
      return (std::floor(ju - (Ju - 1) * 0.5) == 0) ? 1. : 0.;
    };
    auto convol_kernelv = [=](const t_int jv) -> t_complex {
      return (std::floor(jv - (Jv - 1) * 0.5) == 0) ? 1. : 0.;
    };
    auto convol_kernelw = [=](const t_int jv, const t_int ju) -> t_complex {
      return ((std::floor(ju - (Jw - 1) * 0.5) == 0) and (std::floor(jv - (Jw - 1) * 0.5) == 0))
                 ? 1.
                 : 0.;
    };
    const Matrix<t_complex> projection_kernel = convol::linear_convol_2d<t_complex>(
        convol_kernelu, convol_kernelv, convol_kernelw, static_cast<t_int>(Ju),
        static_cast<t_int>(Jv), static_cast<t_int>(Jw), static_cast<t_int>(Jw));
    CAPTURE(projection_kernel);
    CHECK(projection_kernel.cols() == (Ju + Jw - 1));
    CHECK(projection_kernel.rows() == (Jv + Jw - 1));
    CHECK(projection_kernel(2, 1) == 1.);
  }
  SECTION("box_and_delta_size_1") {
    const t_int Jw = 1;
    auto convol_kernelu = [=](const t_int ju) -> t_complex { return ju * 1.; };
    auto convol_kernelv = [=](const t_int jv) -> t_complex { return jv * 1.; };
    auto convol_kernelw = [=](const t_int jv, const t_int ju) -> t_complex {
      return ((std::floor(ju - (Jw - 1) * 0.5) == 0) and (std::floor(jv - (Jw - 1) * 0.5) == 0))
                 ? 1.
                 : 0.;
    };
    const Matrix<t_complex> projection_kernel = convol::linear_convol_2d<t_complex>(
        convol_kernelu, convol_kernelv, convol_kernelw, static_cast<t_int>(Ju),
        static_cast<t_int>(Jv), static_cast<t_int>(Jw), static_cast<t_int>(Jw));
    CAPTURE(projection_kernel);
    CHECK(projection_kernel.cols() == (Ju + Jw - 1));
    CHECK(projection_kernel.rows() == (Jv + Jw - 1));
    for (int i = 0; i < projection_kernel.cols(); i++) {
      for (int j = 0; j < projection_kernel.rows(); j++) {
        CHECK(std::abs(static_cast<t_real>(i * j) - projection_kernel(j, i)) < 1e-12);
      }
    }
  }
}
