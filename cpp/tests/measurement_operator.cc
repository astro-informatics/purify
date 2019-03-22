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
  const t_int power_iters = 0;
  const t_real power_tol = 1e-9;

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

