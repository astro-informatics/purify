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

TEST_CASE("wprojection") {
  std::string const fitsfile = image_filename("M31.fits");
  Image<t_complex> const M31 = pfitsio::read2d(fitsfile);
  Vector<t_complex> const input = Vector<t_complex>::Map(M31.data(), M31.size());
  const t_uint M = 10;
  const Vector<t_complex> vis = Vector<t_complex>::Random(M);
  const t_uint imsizex = M31.cols();
  const t_uint imsizey = M31.rows();
  const t_uint Jw = 30;
  const t_real cell_x = 1;
  const t_real cell_y = 1;
  const t_uint power_iters = 1000;
  const t_real power_tol = 1e-4;
  const t_real abs_error = 1e-9;
  const t_real rel_error = 1e-9;
  const bool w_term = false;
  utilities::vis_params uv_data;
  uv_data.u = Vector<t_real>::Random(M);
  uv_data.v = Vector<t_real>::Random(M);
  uv_data.w = Vector<t_real>::Zero(M);
  uv_data.weights = Vector<t_complex>::Ones(M);
  uv_data.vis = Vector<t_complex>::Ones(M);
  SECTION("oversample 2") {
    const kernels::kernel kernel = kernels::kernel::kb;
    const t_real oversample_ratio = 2;
    const t_uint Ju = 4;
    const t_uint Jv = 4;
    auto mop = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
        measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
            uv_data, imsizey, imsizex, cell_x, cell_y, oversample_ratio, kernel, Ju, Jv, w_term),
        power_iters, power_tol, Vector<t_complex>::Ones(imsizex * imsizey).eval()));
    auto mop_wproj = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
        measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
            uv_data, imsizey, imsizex, cell_x, cell_y, oversample_ratio, kernel, Ju, Jw, w_term,
            abs_error, rel_error, dde_type::wkernel_radial),
        power_iters, power_tol, Vector<t_complex>::Ones(imsizex * imsizey).eval()));
    REQUIRE((mop_wproj->adjoint() * vis).size() == imsizex * imsizey);
    REQUIRE((mop->adjoint() * vis).size() == imsizex * imsizey);
    REQUIRE((*mop * input).size() == M);
    REQUIRE((*mop_wproj * input).size() == M);
    const t_real op_norm = std::get<0>(sopt::algorithm::power_method<Vector<t_complex>>(
        {[=](Vector<t_complex>& output, const Vector<t_complex>& inp) {
           output = (*mop * inp).eval() - (*mop_wproj * inp).eval();
         },
         [=](Vector<t_complex>& output, const Vector<t_complex>& inp) {
           output = (mop->adjoint() * inp).eval() - (mop_wproj->adjoint() * inp).eval();
         }},
        power_iters, power_tol, input));
    REQUIRE(op_norm == Approx(0).margin(1e-3));
  }
  SECTION("oversample 1.375") {
    const kernels::kernel kernel = kernels::kernel::kbmin;
    const t_real oversample_ratio = 1.375;
    const t_uint Ju = 5;
    const t_uint Jv = 5;
    auto mop = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
        measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
            uv_data, imsizey, imsizex, cell_x, cell_y, oversample_ratio, kernel, Ju, Jv, w_term),
        power_iters, power_tol, Vector<t_complex>::Ones(imsizex * imsizey).eval()));
    auto mop_wproj = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
        measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
            uv_data, imsizey, imsizex, cell_x, cell_y, oversample_ratio, kernel, Ju, Jw, w_term,
            abs_error, rel_error, dde_type::wkernel_radial),
        power_iters, power_tol, Vector<t_complex>::Ones(imsizex * imsizey).eval()));
    REQUIRE((mop->adjoint() * vis).size() == imsizex * imsizey);
    REQUIRE((mop_wproj->adjoint() * vis).size() == imsizex * imsizey);
    REQUIRE((*mop * input).size() == M);
    REQUIRE((*mop_wproj * input).size() == M);
    const t_real op_norm = std::get<0>(sopt::algorithm::power_method<Vector<t_complex>>(
        {[=](Vector<t_complex>& output, const Vector<t_complex>& inp) {
           output = (*mop * inp).eval() - (*mop_wproj * inp).eval();
         },
         [=](Vector<t_complex>& output, const Vector<t_complex>& inp) {
           output = (mop->adjoint() * inp).eval() - (mop_wproj->adjoint() * inp).eval();
         }},
        power_iters, power_tol, input));
    REQUIRE(op_norm == Approx(0).margin(2e-3));
  }
}
