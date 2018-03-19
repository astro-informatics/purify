#include "purify/config.h"
#include <iostream>
#include "catch.hpp"
#include "purify/logging.h"

#include "purify/types.h"

#include "purify/convolution.h"
#include "purify/directories.h"
#include "purify/kernels.h"
#include "purify/projection_kernels.h"
#include "purify/wproj_grid.h"
#include "purify/wproj_utilities.h"

#include "purify/operators.h"

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("kernel") {
  const t_int Ju = 4;
  const t_int Jv = 4;
  const t_int Jw = 6;
  const t_uint imsize = 2048;
  const t_real cell = 30; // arcseconds
  const t_real oversample_ratio = 2;
  const auto uvkernels
      = purify::create_kernels(kernels::kernel::kb, Ju, Jv, imsize, imsize, oversample_ratio);
  const auto kernelu = std::get<0>(uvkernels);
  const auto kernelv = std::get<1>(uvkernels);
  const auto kernelw
      = projection_kernels::w_projection_kernel(cell, cell, imsize, imsize, oversample_ratio);
  const Vector<t_real> u = Vector<t_real>::Random(5);
  const Vector<t_real> v = Vector<t_real>::Random(5);
  const Vector<t_real> w = Vector<t_real>::Random(5) * 100;
  for(t_uint m = 0; m < u.size(); m++) {
    const Matrix<t_complex> output
        = projection_kernels::projection(kernelv, kernelu, kernelw, u(m), v(m), w(m), Ju, Jv, Jw);
    break;
  }
}

TEST_CASE("w_projection") {

  const t_int Ju = 5;
  const t_int Jv = 5;
  const t_uint imsize = 10;
  const t_real cell = 30; // arcseconds
  const t_real oversample_ratio = 2;
  const auto uvkernels
      = purify::create_kernels(kernels::kernel::kb, Ju, Jv, imsize, imsize, oversample_ratio);
  const auto kernelu = std::get<0>(uvkernels);
  const auto kernelv = std::get<1>(uvkernels);
  const t_uint M = 5;
  const Vector<t_real> u = Vector<t_real>::Random(M);
  const Vector<t_real> v = Vector<t_real>::Random(M);
  const Vector<t_real> w = Vector<t_real>::Random(M) * 100;
  const Vector<t_complex> weights = Vector<t_complex>::Ones(M);
  SECTION("small angle approximation") {
    const t_int Jw = 10;
    const auto kernelw
        = projection_kernels::w_projection_kernel(cell, cell, imsize, imsize, oversample_ratio);
    const Sparse<t_complex> G
        = details::init_gridding_matrix_2d(u, v, w, weights, imsize, imsize, oversample_ratio,
                                           kernelu, kernelv, kernelw, Ju, Jv, true, 1, 1, Jw);
    const Vector<t_complex> output
        = G * Vector<t_complex>::Random(imsize * imsize * oversample_ratio * oversample_ratio);
  }
  SECTION("delta") {
    const t_int Jw = 1;
    const auto kernelw = projection_kernels::box_proj();
    const Sparse<t_complex> G
        = details::init_gridding_matrix_2d(u, v, w, weights, imsize, imsize, oversample_ratio,
                                           kernelu, kernelv, kernelw, Ju, Jv, true, 1, 1, Jw);
    const Vector<t_complex> output
        = G * Vector<t_complex>::Random(imsize * imsize * oversample_ratio * oversample_ratio);
    const Sparse<t_complex> G_id = details::init_gridding_matrix_2d(
        u, v, w, weights, imsize, imsize, oversample_ratio, kernelu, kernelv, kernelw, Ju, Jv);
    CHECK(G.nonZeros() == G_id.nonZeros());
    CAPTURE(G.row(0));
    CAPTURE(G_id.row(0));
    CHECK(G.isApprox(G_id));
  }
}

TEST_CASE("Calcuate Chirp Image") {
  const t_int imsizex = 128;
  const t_int imsizey = 128;
  SECTION("w is zero") {
    const t_real w_rate = 0;
    const Image<t_complex> chirp_image
        = wproj_utilities::generate_chirp(w_rate, 1, 1, imsizex, imsizey);
    CHECK((chirp_image.array().cwiseAbs() - 1. / (imsizex * imsizey)).matrix().norm() < 1e-12);
  }
}

TEST_CASE("Calculate Threshold") {
  SECTION("None") {
    const t_real expected_sparse_total_energy = 1;
    const Sparse<t_real> row = Vector<t_real>::Random(100000).sparseView();
    const t_real threshold = wproj_utilities::sparsify_row_thres(row, expected_sparse_total_energy);
    t_real total_energy = 0;
    t_real sparse_total_energy = 0;
    for(t_uint i = 0; i < row.rows(); i++) {
      total_energy += std::abs(row.coeff(i, 0) * row.coeff(i, 0));
      if(std::abs(row.coeff(i, 0)) > threshold) {
        sparse_total_energy += std::abs(row.coeff(i, 0) * row.coeff(i, 0));
      }
    }
    sparse_total_energy = sparse_total_energy / total_energy;
    CHECK(std::abs(expected_sparse_total_energy - sparse_total_energy)
              / expected_sparse_total_energy
          < 1e-1);
  }
  SECTION("Half Energy") {
    const t_real expected_sparse_total_energy = 0.4;
    const Sparse<t_real> row = Vector<t_real>::Random(100000).sparseView();
    const t_real threshold = wproj_utilities::sparsify_row_thres(row, expected_sparse_total_energy);
    t_real total_energy = 0;
    t_real sparse_total_energy = 0;
    for(t_uint i = 0; i < row.rows(); i++) {
      total_energy += std::abs(row.coeff(i, 0) * row.coeff(i, 0));
      if(std::abs(row.coeff(i, 0)) > threshold) {
        sparse_total_energy += std::abs(row.coeff(i, 0) * row.coeff(i, 0));
      }
    }
    sparse_total_energy = sparse_total_energy / total_energy;
    CHECK(std::abs(expected_sparse_total_energy - sparse_total_energy)
              / expected_sparse_total_energy
          < 1e-1);
  }
}

TEST_CASE("simple convolution") {
  const t_int Nx = 12;
  const t_int Ny = 12;
  const Sparse<t_complex> G_row = Matrix<t_complex>::Random(1, Nx * Ny).sparseView(1e-2);
  const Sparse<t_complex> C_row = Matrix<t_complex>::Random(1, Nx * Ny).sparseView(1e-2);
  const Sparse<t_complex> kernel = wproj_utilities::row_wise_convolution(G_row, C_row, Nx, Ny);
  const Sparse<t_complex> test_kernel
      = wproj_utilities::row_wise_sparse_convolution(G_row, C_row, Nx, Ny);
  CHECK(test_kernel.nonZeros() == kernel.nonZeros());
  REQUIRE(test_kernel.isApprox(kernel, 1e-12));
}

TEST_CASE("wprojection_matrix") {
  //! test if convolution is identity
  // purify::logging::set_level("debug");
  const Vector<t_real> w_components = Vector<t_real>::Random(12);
  const Vector<t_real> w_components_zero = w_components * 0;
  const t_int Nx = 32;
  const t_int Ny = 32;
  const t_real cellx = 60;
  const t_real celly = 60;
  const t_int rows = w_components.size();
  const t_int cols = Nx * Ny;

  Sparse<t_complex> I(rows, cols);
  I.reserve(rows);
  for(t_int i = 0; i < std::min(rows, cols); i++)
    I.coeffRef(i, i) = 1.;
  Sparse<t_complex> const G_id
      = wproj_utilities::wprojection_matrix(I, Nx, Ny, w_components_zero, cellx, celly, 1, 1);
  Sparse<t_complex> const G
      = wproj_utilities::wprojection_matrix(I, Nx, Ny, w_components, cellx, celly, 1, 1);
  // Testing if G_id == I
  CHECK(G_id.nonZeros() == I.nonZeros());
  CHECK(G.nonZeros() >= I.nonZeros());
  for(t_int k = 0; k < G_id.outerSize(); ++k)
    for(Sparse<t_complex>::InnerIterator it(G_id, k); it; ++it) {
      CAPTURE(it.value() * static_cast<t_real>(Nx));
      CHECK(std::abs(it.value() - 1.) < 1e-12);
      CHECK(it.row() == k); // row index
      CHECK(it.col() == k); // col index
    }
}
TEST_CASE("convolution even") {
  const t_int Nx = 4;
  const t_int Ny = 4;
  Matrix<t_complex> G_data(1, Nx * Ny);
  Matrix<t_complex> C_data(1, Nx * Ny);
  Matrix<t_complex> K_data(1, Nx * Ny);
  SECTION("dirac") {
    G_data = Matrix<t_complex>::Zero(1, Nx * Ny);
    C_data = Matrix<t_complex>::Zero(1, Nx * Ny);
    G_data(0, 0) = 1;
    C_data(0, 5) = 1;
    K_data = C_data;
    const Sparse<t_complex> G_row = G_data.sparseView(1e-10);
    const Sparse<t_complex> C_row = C_data.sparseView(1e-10);
    const Sparse<t_complex> K_row = K_data.sparseView(1e-10);
    const Sparse<t_complex> kernel = wproj_utilities::row_wise_convolution(G_row, C_row, Nx, Ny);
    CHECK(kernel.isApprox(K_row, 1e-7));
  }
  SECTION("data 1") {
    G_data << 0.7634463, 0.8537454, 0.89558853, 0.71197476, 0.07342149, 0.19600616, 0.21977598,
        0.3802055, 0.07577487, 0.94388022, 0.5813741, 0.51562299, 0.433173, 0.34491308, 0.55125663,
        0.9279308;
    C_data << 0.13239976, 0.01737199, 0.44752763, 0.33047119, 0.71925933, 0.40904651, 0.80406293,
        0.95091037, 0.61481599, 0.39252926, 0.22208961, 0.36976165, 0.65017947, 0.22863747,
        0.87700107, 0.06965994;
    K_data << 1.56301893, 0.69579421, 2.55646966, 2.80081109, 1.73328232, 0.95357603, 2.12264685,
        2.45700481, 1.91327398, 1.27415219, 2.06766882, 2.84902461, 2.33590802, 1.64204955,
        3.13634125, 3.72991149;
    const Sparse<t_complex> G_row = G_data.sparseView(1e-10);
    const Sparse<t_complex> C_row = C_data.sparseView(1e-10);
    const Sparse<t_complex> K_row = K_data.sparseView(1e-10);
    const Sparse<t_complex> kernel = wproj_utilities::row_wise_convolution(G_row, C_row, Nx, Ny);
    CHECK(kernel.isApprox(K_row, 1e-7));
  }
}

TEST_CASE("convolution odd") {
  const t_int Nx = 5;
  const t_int Ny = 5;
  Matrix<t_complex> G_data(1, Nx * Ny);
  Matrix<t_complex> C_data(1, Nx * Ny);
  Matrix<t_complex> K_data(1, Nx * Ny);
  SECTION("dirac") {
    G_data = Matrix<t_complex>::Random(1, Nx * Ny);
    C_data = Matrix<t_complex>::Zero(1, Nx * Ny);
    C_data(0) = 1;
    K_data = G_data;
    const Sparse<t_complex> G_row = G_data.sparseView(1e-10);
    const Sparse<t_complex> C_row = C_data.sparseView(1e-10);
    const Sparse<t_complex> K_row = K_data.sparseView(1e-10);
    const Sparse<t_complex> kernel = wproj_utilities::row_wise_convolution(G_row, C_row, Nx, Ny);
    CHECK(kernel.isApprox(K_row, 1e-7));
  }
  SECTION("data 1") {
    G_data << 0.84416411, 0.95108536, 0.66690087, 0.92687553, 0.82640953, 0.05214219, 0.23629002,
        0.0456125, 0.31058604, 0.20523312, 0.33501332, 0.2103377, 0.67046083, 0.68790078,
        0.84890013, 0.62349394, 0.18208434, 0.91033138, 0.34436924, 0.11369944, 0.31609394,
        0.45869556, 0.63513753, 0.73314836, 0.51449701;
    C_data << 0.30409785, 0.20084303, 0.32436196, 0.49613954, 0.29283696, 0.54984908, 0.60762237,
        0.74812515, 0.96318413, 0.80115342, 0.85182094, 0.33545398, 0.1658094, 0.31987092,
        0.41907388, 0.22498119, 0.60000475, 0.67336284, 0.45901998, 0.550584, 0.90333773,
        0.80207468, 0.20964248, 0.84893657, 0.6063695;
    K_data << 6.2710243, 4.53980133, 3.21624657, 3.82809291, 4.6633187, 6.09692671, 4.24215225,
        3.24627495, 4.52701773, 5.53094158, 3.27058866, 2.46446407, 1.58955594, 2.41529522,
        2.84256957, 4.63801315, 3.28906792, 2.81268103, 2.74098252, 3.65394543, 5.83657047,
        4.21082195, 3.19483206, 3.67326191, 4.7494392;
    const Sparse<t_complex> G_row = G_data.sparseView(1e-10);
    const Sparse<t_complex> C_row = C_data.sparseView(1e-10);
    const Sparse<t_complex> K_row = K_data.sparseView(1e-10);
    const Sparse<t_complex> kernel = wproj_utilities::row_wise_convolution(G_row, C_row, Nx, Ny);
    CHECK(kernel.isApprox(K_row, 1e-7));
  }
}
