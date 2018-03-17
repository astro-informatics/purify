#include "purify/config.h"
#include <iostream>
#include "catch.hpp"
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/types.h"

#include "purify/convolution.h"

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("1d_zeropad") {
  const Vector<t_int> signal = Vector<t_int>::Random(3);
  for(int i = 1; i < 5; i++) {
    const Vector<t_int> output = convol::zero_pad(signal, i);
    CHECK(output.size() == signal.size() + 2 * i - 1);
    for(int j = 0; j < output.size(); j++) {
      if(j < i)
        CHECK(output(j) == 0);
      if(j > (i + signal.size()))
        CHECK(output(j) == 0);
    }
  }
}
TEST_CASE("1d_convolution") {
  Vector<t_int> tri = Vector<t_int>::Zero(3);
  tri << 1, 2, 1;
  Vector<t_int> signal = convol::zero_pad<t_int>(Vector<t_int>::Random(4), tri.size());
  const Vector<t_int> output = convol::linear_convol_1d(tri, signal);
  CHECK(output.size() == signal.size() - tri.size());
  for(int i = 0; i < signal.size() - tri.size() - 1; i++)
    CHECK(output(i) == tri(0) * signal(i) + tri(1) * signal(i + 1) + tri(2) * signal(i + 2));
}
TEST_CASE("2d_convolution") {
  const Vector<t_real> kernelu = Vector<t_real>::Random(3);
  const Vector<t_real> kernelv = Vector<t_real>::Random(2);
  const Matrix<t_real> signal = Matrix<t_real>::Random(2, 2);
  const Matrix<t_real> output = convol::linear_convol_2d(kernelu, kernelv, signal);
  CHECK(output.cols() == signal.cols() + kernelu.size() - 1);
  CHECK(output.rows() == signal.rows() + kernelv.size() - 1);
  const Matrix<t_real> test_buff = convol::zero_pad<t_real>(signal, kernelv.size(), kernelu.size());
  // checking for signs that there is a boundary of zero, suggesting an offset
  CHECK(output(0, 0) != output(1, 0));
  CHECK(output(output.rows() - 1, output.cols() - 1)
        != output(output.rows() - 1, output.cols() - 2));
  for(int i = 0; i < output.cols(); i++) {
    for(int j = 0; j < output.rows(); j++) {
      CHECK(
          std::abs(output(j, i)
                   - kernelv(0)
                         * (kernelu(0) * test_buff(j, i) + kernelu(1) * test_buff(j, i + 1)
                            + kernelu(2) * test_buff(j, i + 2))
                   - kernelv(1)
                         * (kernelu(0) * test_buff(j + 1, i) + kernelu(1) * test_buff(j + 1, i + 1)
                            + kernelu(2) * test_buff(j + 1, i + 2)))
          < 1e-7);
    }
  }
}
