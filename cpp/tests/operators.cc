#include "purify/operators.h"
#include "purify/config.h"
#include "purify/types.h"
#include "catch.hpp"
#include "purify/directories.h"
#include "purify/kernels.h"
#include "purify/logging.h"
#include "purify/test_data.h"
#include "purify/wproj_operators.h"
#include <sopt/power_method.h>

using namespace purify;
using namespace purify::notinstalled;

namespace operators_test {
const std::string test_dir = "expected/operators/";
//! data for u coordinate
const std::vector<t_real> u = notinstalled::read_data<t_real>(
    notinstalled::data_filename(test_dir + "u_data"));  //! data for v coordinate
const std::vector<t_real> v =
    notinstalled::read_data<t_real>(notinstalled::data_filename(test_dir + "v_data"));
//! data for degridding input
const std::vector<t_complex> direct_input =
    notinstalled::read_data<t_complex>(notinstalled::data_filename(test_dir + "direct_input_data"));
//! data for degridding output
const std::vector<t_complex> expected_direct = notinstalled::read_data<t_complex>(
    notinstalled::data_filename(test_dir + "expected_direct_data"));
//! data for gridding input
const std::vector<t_complex> indirect_input = notinstalled::read_data<t_complex>(
    notinstalled::data_filename(test_dir + "indirect_input_data"));
//! data for gridding output
const std::vector<t_complex> expected_indirect = notinstalled::read_data<t_complex>(
    notinstalled::data_filename(test_dir + "expected_indirect_data"));

//! data for gridding correction
const std::vector<t_complex> expected_S = notinstalled::read_data(
    notinstalled::read_data<t_real>(notinstalled::data_filename(test_dir + "expected_S_data")));
}  // namespace operators_test

TEST_CASE("Operators") {
  const t_uint M = 10;
  const t_real oversample_ratio = 2;
  const t_uint imsizex = 16;
  const t_uint imsizey = 16;
  const t_uint ftsizev = std::floor(imsizey * oversample_ratio);
  const t_uint ftsizeu = std::floor(imsizex * oversample_ratio);
  const t_uint Ju = 4;
  const t_uint Jv = 4;
  const t_uint power_iters = 1e4;
  const t_real power_tol = 1e-9;
  const kernels::kernel kernel = kernels::kernel::kb;
  const std::string &weighting_type = "natural";
  utilities::vis_params uv_vis;
  uv_vis.u = Vector<t_real>::Map(operators_test::u.data(), operators_test::u.size());
  uv_vis.v = Vector<t_real>::Map(operators_test::v.data(), operators_test::v.size());
  uv_vis.w = uv_vis.u * 0.;
  uv_vis.weights = Vector<t_complex>::Random(M);
  uv_vis.vis = Vector<t_complex>::Random(M);
  uv_vis.units = utilities::vis_units::pixels;
  std::function<t_real(t_real)> kbu, kbv, ftkbu, ftkbv;
  std::tie(kbu, kbv, ftkbu, ftkbv) =
      create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  SECTION("Gridding on the fly") {
    sopt::OperatorFunction<Vector<t_complex>> directG, indirectG;
    std::tie(directG, indirectG) = operators::init_on_the_fly_gridding_matrix_2d<Vector<t_complex>>(
        uv_vis.u, uv_vis.v, Vector<t_complex>::Constant(M, 1.), imsizey, imsizex, oversample_ratio,
        kbv, kbu, Ju, Jv);
    Vector<t_complex> direct_output;
    directG(direct_output,
            Vector<t_complex>::Map(operators_test::direct_input.data(), ftsizeu * ftsizev));
    CHECK(direct_output.size() == M);
    CHECK(direct_output.size() == operators_test::expected_direct.size());
    REQUIRE(direct_output.isApprox(Vector<t_complex>::Map(operators_test::expected_direct.data(),
                                                          operators_test::expected_direct.size()),
                                   1e-5));
    Vector<t_complex> indirect_output;
    indirectG(indirect_output, Vector<t_complex>::Map(operators_test::indirect_input.data(),
                                                      operators_test::indirect_input.size()));
    CHECK(indirect_output.size() == ftsizev * ftsizeu);
    CAPTURE(Vector<t_complex>::Map(operators_test::expected_indirect.data(),
                                   operators_test::expected_indirect.size())
                .head(5));
    CAPTURE((indirect_output - Vector<t_complex>::Map(operators_test::expected_indirect.data(),
                                                      operators_test::expected_indirect.size()))
                .head(5));
    REQUIRE(
        indirect_output.isApprox(Vector<t_complex>::Map(operators_test::expected_indirect.data(),
                                                        operators_test::expected_indirect.size()),
                                 1e-5));
  }
  SECTION("Gridding") {
    sopt::OperatorFunction<Vector<t_complex>> directG, indirectG;
    std::tie(directG, indirectG) = operators::init_gridding_matrix_2d<Vector<t_complex>>(
        uv_vis.u, uv_vis.v, Vector<t_complex>::Constant(M, 1.), imsizey, imsizex, oversample_ratio,
        kbv, kbu, Ju, Jv);
    Vector<t_complex> direct_output;
    directG(direct_output,
            Vector<t_complex>::Map(operators_test::direct_input.data(), ftsizeu * ftsizev));
    CHECK(direct_output.size() == M);
    CHECK(direct_output.size() == operators_test::expected_direct.size());
    REQUIRE(direct_output.isApprox(Vector<t_complex>::Map(operators_test::expected_direct.data(),
                                                          operators_test::expected_direct.size()),
                                   1e-5));
    Vector<t_complex> indirect_output;
    indirectG(indirect_output, Vector<t_complex>::Map(operators_test::indirect_input.data(),
                                                      operators_test::indirect_input.size()));
    CHECK(indirect_output.size() == ftsizev * ftsizeu);
    CAPTURE(Vector<t_complex>::Map(operators_test::expected_indirect.data(),
                                   operators_test::expected_indirect.size())
                .head(5));
    CAPTURE((indirect_output - Vector<t_complex>::Map(operators_test::expected_indirect.data(),
                                                      operators_test::expected_indirect.size()))
                .head(5));
    REQUIRE(
        indirect_output.isApprox(Vector<t_complex>::Map(operators_test::expected_indirect.data(),
                                                        operators_test::expected_indirect.size()),
                                 1e-5));
  }
  SECTION("Zero Padding") {
    const Image<t_complex> S =
        details::init_correction2d(oversample_ratio, imsizey, imsizex, ftkbu, ftkbv, 0, 0, 0);
    CHECK(imsizex == S.cols());
    CHECK(imsizey == S.rows());
    INFO(S(0) / operators_test::expected_S.at(0));
    INFO(S(5) / operators_test::expected_S.at(5));
    REQUIRE(S.isApprox(Image<t_complex>::Map(operators_test::expected_S.data(), imsizey, imsizex),
                       1e-6));
    sopt::OperatorFunction<Vector<t_complex>> directZ, indirectZ;
    std::tie(directZ, indirectZ) =
        operators::init_zero_padding_2d<Vector<t_complex>>(S, oversample_ratio);
    const Vector<t_complex> direct_input = Vector<t_complex>::Random(imsizex * imsizey);
    Vector<t_complex> direct_output;
    directZ(direct_output, direct_input);
    CHECK(direct_output.size() == ftsizeu * ftsizev);
    const Vector<t_complex> indirect_input = Vector<t_complex>::Random(ftsizeu * ftsizev);
    Vector<t_complex> indirect_output;
    indirectZ(indirect_output, indirect_input);
    CHECK(indirect_output.size() == imsizex * imsizey);
  }
  SECTION("FFT") {
    sopt::OperatorFunction<Vector<t_complex>> directFFT, indirectFFT;
    std::tie(directFFT, indirectFFT) =
        operators::init_FFT_2d<Vector<t_complex>>(imsizey, imsizex, oversample_ratio);
    const Vector<t_complex> direct_input = Vector<t_complex>::Random(ftsizev * ftsizeu);
    Vector<t_complex> direct_output;
    directFFT(direct_output, direct_input);
    CHECK(direct_output.size() == ftsizeu * ftsizev);
    const Vector<t_complex> indirect_input = Vector<t_complex>::Random(ftsizev * ftsizeu);
    Vector<t_complex> indirect_output;
    indirectFFT(indirect_output, indirect_input);
    CHECK(indirect_output.size() == ftsizev * ftsizeu);
    Vector<t_complex> inverse_check;
    Vector<t_complex> buff;
    directFFT(buff, direct_input);
    indirectFFT(inverse_check, buff);
    CHECK(inverse_check.isApprox(direct_input, 1e-4));
  }
  SECTION("Create Weighted Measurement Operator") {
    const t_uint M_measures = 1e4;
    const Vector<t_real> u = Vector<t_real>::Random(M_measures);
    const Vector<t_real> v = Vector<t_real>::Random(M_measures);
    const Vector<t_real> w = Vector<t_real>::Random(M_measures);
    const Vector<t_complex> weights = Vector<t_complex>::Random(M_measures);
    const auto measure_op = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv);
    const Vector<t_complex> direct_input = Vector<t_complex>::Random(imsizex * imsizey);
    const Vector<t_complex> direct_output = *measure_op * direct_input;
    CHECK(direct_output.size() == M_measures);
    const Vector<t_complex> indirect_input = Vector<t_complex>::Random(M_measures).eval();
    const Vector<t_complex> indirect_output = measure_op->adjoint() * indirect_input;
    CHECK(indirect_output.size() == imsizex * imsizey);
  }
}
TEST_CASE("Degridding from multiple Images") {
  const t_int imsizex = 32;
  const t_int imsizey = 32;
  const t_int M = 5;
  const t_real oversample_ratio = 2;
  const t_int ftsizeu = std::floor(imsizex * oversample_ratio);
  const t_int ftsizev = std::floor(imsizey * oversample_ratio);
  auto const kernelu = [](t_real) -> t_real { return 1.; };
  auto const kernelv = [](t_real) -> t_real { return 1.; };
  const t_int Ju = 1;
  const t_int Jv = 1;
  for (t_int images : {1, 2, 3, 5, 10}) {
    Vector<t_real> u = Vector<t_real>::Zero(M * images);
    Vector<t_real> v = Vector<t_real>::Zero(M * images);
    Vector<t_complex> weights = Vector<t_complex>::Zero(M * images);
    Vector<t_complex> vis = Vector<t_complex>::Zero(M * images);
    Vector<t_complex> image = Vector<t_complex>::Zero(ftsizeu * ftsizev * images);
    std::vector<t_int> image_index(M * images, 0);
    // double up vector
    u.segment(0, M) = Vector<t_real>::Random(M);
    v.segment(0, M) = Vector<t_real>::Random(M);
    weights.segment(0, M) = Vector<t_complex>::Random(M);
    vis.segment(0, M) = Vector<t_complex>::Random(M);
    image.segment(0, ftsizeu * ftsizev) = Vector<t_complex>::Random(ftsizeu * ftsizev);
    for (t_int i = 1; i < images; i++) {
      image.segment(ftsizeu * ftsizev * i, ftsizeu * ftsizev) =
          image.segment(0, ftsizeu * ftsizev) * i;
      u.segment(M * i, M) = u.segment(0, M);
      v.segment(M * i, M) = v.segment(0, M);
      weights.segment(M * i, M) = weights.segment(0, M);
      vis.segment(M * i, M) = vis.segment(0, M) * i;
      for (t_int c = 0; c < M; c++) image_index[M * i + c] = i;
    }
    const auto G_tuple = operators::init_gridding_matrix_2d<Vector<t_complex>>(
        images, image_index, u, v, weights, imsizey, imsizex, oversample_ratio, kernelu, kernelv,
        Ju, Jv);
    sopt::LinearTransform<Vector<t_complex>> const G = {
        std::get<0>(G_tuple), {0, 1, M}, std::get<1>(G_tuple), {0, 1, ftsizeu * ftsizev * images}};
    const Vector<t_complex> forward = G * image;
    for (t_int i = 1; i < images; i++)
      REQUIRE((i * forward.segment(0, M)).isApprox(forward.segment(M * i, M), 1e-7));
    const Vector<t_complex> backward = G.adjoint() * vis;
    for (t_int i = 1; i < images; i++)
      REQUIRE((i * backward.segment(0, ftsizeu * ftsizev))
                  .isApprox(backward.segment(ftsizeu * ftsizev * i, ftsizeu * ftsizev), 1e-7));
  }
}
TEST_CASE("Degridding from multiple Images wproj") {
  const t_int imsizex = 32;
  const t_int imsizey = 32;
  const t_int M = 5;
  const t_real oversample_ratio = 2;
  const t_int ftsizeu = std::floor(imsizex * oversample_ratio);
  const t_int ftsizev = std::floor(imsizey * oversample_ratio);
  auto const kernelu = [](t_real) -> t_real { return 1.; };
  auto const kernelv = [](t_real) -> t_real { return 1.; };
  const t_int Ju = 1;
  const t_int Jv = 1;
  for (t_int images : {1, 2, 3, 5, 10}) {
    Vector<t_real> u = Vector<t_real>::Zero(M * images);
    Vector<t_real> v = Vector<t_real>::Zero(M * images);
    Vector<t_complex> weights = Vector<t_complex>::Zero(M * images);
    Vector<t_complex> vis = Vector<t_complex>::Zero(M * images);
    Vector<t_complex> image = Vector<t_complex>::Zero(ftsizeu * ftsizev * images);
    std::vector<t_int> image_index(M * images, 0);
    // double up vector
    u.segment(0, M) = Vector<t_real>::Random(M);
    v.segment(0, M) = Vector<t_real>::Random(M);
    weights.segment(0, M) = Vector<t_complex>::Random(M);
    vis.segment(0, M) = Vector<t_complex>::Random(M);
    image.segment(0, ftsizeu * ftsizev) = Vector<t_complex>::Random(ftsizeu * ftsizev);
    for (t_int i = 1; i < images; i++) {
      image.segment(ftsizeu * ftsizev * i, ftsizeu * ftsizev) =
          image.segment(0, ftsizeu * ftsizev) * i;
      u.segment(M * i, M) = u.segment(0, M);
      v.segment(M * i, M) = v.segment(0, M);
      weights.segment(M * i, M) = weights.segment(0, M);
      vis.segment(M * i, M) = vis.segment(0, M) * i;
      for (t_int c = 0; c < M; c++) image_index[M * i + c] = i;
    }
    const auto G_tuple = operators::init_gridding_matrix_2d<Vector<t_complex>>(
        images, image_index, std::vector<t_real>(images, 0), u, v, u * 0, weights, imsizey, imsizex,
        oversample_ratio, kernelu, kernelv, Ju, 10, 1, 1, 1e-6, 1e-6, dde_type::wkernel_radial);
    sopt::LinearTransform<Vector<t_complex>> const G = {
        std::get<0>(G_tuple), {0, 1, M}, std::get<1>(G_tuple), {0, 1, ftsizeu * ftsizev * images}};
    const Vector<t_complex> forward = G * image;
    for (t_int i = 1; i < images; i++)
      REQUIRE((i * forward.segment(0, M)).isApprox(forward.segment(M * i, M), 1e-7));
    const Vector<t_complex> backward = G.adjoint() * vis;
    for (t_int i = 1; i < images; i++)
      REQUIRE((i * backward.segment(0, ftsizeu * ftsizev))
                  .isApprox(backward.segment(ftsizeu * ftsizev * i, ftsizeu * ftsizev), 1e-7));
  }
}
