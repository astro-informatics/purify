#include <random>
#include "catch.hpp"
#include "purify/directories.h"
#include "purify/utilities.h"

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("utilities [mod]", "[mod]") {
  Array<t_real> range;
  range.setLinSpaced(201, -100, 100);
  Array<t_real> output(range.size());
  for (t_int i = 0; i < range.size(); ++i) {
    output(i) = utilities::mod(range(i), 20);
  }
  Array<t_real> expected(201);
  expected << 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 1, 2, 3, 4,
      5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
      12, 13, 14, 15, 16, 17, 18, 19, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
      18, 19, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 1, 2, 3, 4,
      5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
      12, 13, 14, 15, 16, 17, 18, 19, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
      18, 19, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 1, 2, 3, 4,
      5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0;
  CHECK(expected.isApprox(output, 1e-13));
}

TEST_CASE("utilities [reshape]", "[reshape]") {
  // Testing if resize works the same as matlab's reshape
  Matrix<t_real> magic(4, 4);
  magic << 16, 2, 3, 13, 5, 11, 10, 8, 9, 7, 6, 12, 4, 14, 15, 1;
  CAPTURE(magic);
  Vector<t_real> magic_vector(16);
  magic_vector << 16, 5, 9, 4, 2, 11, 7, 14, 3, 10, 6, 15, 13, 8, 12, 1;
  Matrix<t_real> magic_matrix(16, 1);
  magic_matrix << 16, 5, 9, 4, 2, 11, 7, 14, 3, 10, 6, 15, 13, 8, 12, 1;
  magic_matrix.resize(4, 4);
  CHECK(magic.isApprox(magic_matrix, 1e-13));
  magic.resize(16, 1);
  CAPTURE(magic);
  CAPTURE(magic_vector);
  CHECK(magic.isApprox(magic_vector, 1e-13));
}
TEST_CASE("utilities [variance]", "[variance]") {
  // tests if mean and variance calculations are the same as done in matlab
  Vector<t_complex> real_data = Vector<t_complex>::Random(1000);
  Vector<t_complex> imag_data = Vector<t_complex>::Random(1000);
  t_complex I(0, 1);
  const Vector<t_complex> data = (real_data + I * imag_data);
  const t_complex mu = utilities::mean(data);
  CAPTURE(mu);
  CHECK(std::abs(mu - data.mean()) < 1e-13);
  const t_real var = utilities::variance(data);
  const t_real expected_var =
      std::abs(((data.array() - mu) * (data.array() - mu).conjugate()).sum()) / (data.size() - 1.);
  CAPTURE(var);
  CAPTURE(expected_var);
  CHECK(std::abs(var - expected_var) < 1e-13);
}
TEST_CASE("utilities [median]", "[median]") {
  Vector<t_real> x(100);
  x << 0.824149356327936, 0.218232263733975, 0.0996423029616137, 0.619505816445823,
      0.103814782651106, 0.799061803589637, 0.902925119282939, 0.312512845986667, 0.281589166159224,
      0.00678194126805909, 0.495871665666786, 0.988482243012287, 0.737940751495519,
      0.310719817391570, 0.600407500351151, 0.781679826661116, 0.111533124684943, 0.579329289388906,
      0.870371220311720, 0.689776249611124, 0.242970250921629, 0.342722251869759, 0.545439787975426,
      0.0675726926893163, 0.410448399698545, 0.237511397229394, 0.488973556424727,
      0.806066588301788, 0.377847951671179, 0.517978144507388, 0.0945978893449516,
      0.909095192849836, 0.207630593997342, 0.382064523540530, 0.660280265178685, 0.758373302277572,
      0.173069412392096, 0.517379761506326, 0.995338112303466, 0.707609406590709,
      0.0805673009239308, 0.0433080963751025, 0.491155562984260, 0.446596440169203,
      0.486798565845346, 0.165891122085342, 0.360656504813112, 0.880721941763713, 0.744352179996200,
      0.416771050919451, 0.907354808475253, 0.0943068737715302, 0.181326261018145,
      0.946587232183202, 0.100849056860381, 0.388038069974207, 0.289224862024986,
      0.0730868607975930, 0.194608005437083, 0.417485231103283, 0.292926559143940,
      0.702138511791249, 0.239713177935322, 0.959482620708005, 0.305462202307119, 0.154915458213084,
      0.555508419323977, 0.790543689112232, 0.443872293587844, 0.995818721727493, 0.436586698162556,
      0.304441172973089, 0.246510340916772, 0.960825251654563, 0.222880958438666, 0.395609181466230,
      0.224524553336017, 0.270024669015495, 0.418441479002589, 0.997735979773324, 0.911030403930781,
      0.550426888716037, 0.596335815776380, 0.0791452393029073, 0.576636037390549,
      0.898172311971638, 0.463310567259345, 0.398396426873099, 0.104461114798391, 0.652236388678710,
      0.991700005353602, 0.678072815973294, 0.428477593633851, 0.654801321575878, 0.588745528605038,
      0.745054910247788, 0.640870288169766, 0.503676186207024, 0.938041475805194, 0.605343580621484;
  t_real const median = 0.475054566552345;
  CHECK(std::abs(median - utilities::median(x)) < 1e-13);
}
TEST_CASE("utilities [read_write_vis]", "[read_write_vis]") {
  // tests the read and write function for a visibility data set
  std::string vis_file = vla_filename("at166B.3C129.c0I.vis");
  std::string out_file = output_filename("test_output.vis");
  std::string out_w_file = output_filename("test_w_output.vis");
  auto uv_data = utilities::read_visibility(vis_file);
  utilities::write_visibility(uv_data, out_file);
  auto new_uv_data = utilities::read_visibility(out_file);
  CHECK(new_uv_data.u.isApprox(uv_data.u, 1e-13));
  CHECK(new_uv_data.v.isApprox(uv_data.v, 1e-13));
  CHECK(new_uv_data.vis.isApprox(uv_data.vis, 1e-13));
  CHECK(new_uv_data.weights.isApprox(uv_data.weights, 1e-13));
  t_int number_of_random_vis = 1e5;
  const bool w_term = true;
  auto random_uv_data = utilities::random_sample_density(number_of_random_vis, 0, constant::pi / 3);
  utilities::write_visibility(random_uv_data, out_w_file, w_term);
  auto new_random_uv_data = utilities::read_visibility(out_w_file, w_term);
  CHECK(new_random_uv_data.u.isApprox(random_uv_data.u, 1e-8));
  CHECK(new_random_uv_data.v.isApprox(random_uv_data.v, 1e-8));
  CHECK(new_random_uv_data.w.isApprox(random_uv_data.w, 1e-8));
  CHECK(new_random_uv_data.vis.isApprox(random_uv_data.vis, 1e-8));
  CHECK(new_random_uv_data.weights.isApprox(random_uv_data.weights, 1e-8));
}
TEST_CASE("read_mutiple_vis") {
  std::string vis_file = vla_filename("at166B.3C129.c0.vis");
  SECTION("one file") {
    const std::vector<std::string> names = {vis_file};
    const auto uv_data = utilities::read_visibility(vis_file);
    const auto uv_multi = utilities::read_visibility(names);
    CAPTURE(names.size());
    CAPTURE(uv_data.size());
    CHECK(uv_data.size() * names.size() == uv_multi.size());
    for (int i = 0; i < names.size(); i++) {
      CHECK(uv_data.u.isApprox(uv_multi.u.segment(i * uv_data.size(), uv_data.size())));
      CHECK(uv_data.v.isApprox(uv_multi.v.segment(i * uv_data.size(), uv_data.size())));
      CHECK(uv_data.w.isApprox(uv_multi.w.segment(i * uv_data.size(), uv_data.size())));
      CHECK(uv_data.vis.isApprox(uv_multi.vis.segment(i * uv_data.size(), uv_data.size())));
      CHECK(uv_data.weights.isApprox(uv_multi.weights.segment(i * uv_data.size(), uv_data.size())));
    }
  }
  SECTION("many files") {
    const std::vector<std::string> names = {vis_file, vis_file, vis_file};
    const auto uv_data = utilities::read_visibility(vis_file);
    const auto uv_multi = utilities::read_visibility(names);
    CAPTURE(names.size());
    CAPTURE(uv_data.size());
    CHECK(uv_data.size() * names.size() == uv_multi.size());
    for (int i = 0; i < names.size(); i++) {
      CHECK(uv_data.u.isApprox(uv_multi.u.segment(i * uv_data.size(), uv_data.size())));
      CHECK(uv_data.v.isApprox(uv_multi.v.segment(i * uv_data.size(), uv_data.size())));
      CHECK(uv_data.w.isApprox(uv_multi.w.segment(i * uv_data.size(), uv_data.size())));
      CHECK(uv_data.vis.isApprox(uv_multi.vis.segment(i * uv_data.size(), uv_data.size())));
      CHECK(uv_data.weights.isApprox(uv_multi.weights.segment(i * uv_data.size(), uv_data.size())));
    }
  }
}
TEST_CASE("utilities [file exists]", "[file exists]") {
  std::string vis_file = vla_filename("at166B.3C129.c0.vis");
  // File should exist
  CHECK(utilities::file_exists(vis_file));
  // File should not exist
  CHECK(not utilities::file_exists("adfadsf"));
}
TEST_CASE("utilities [fit_fwhm]", "[fit_fwhm]") {
  // testing that the gaussian fitting works.
  t_int imsizex = 512;
  t_int imsizey = 512;
  Image<t_real> psf = Image<t_real>::Zero(imsizey, imsizex);
  // choice of parameters
  t_real const fwhm_x = 3;
  t_real const fwhm_y = 6;
  t_real const theta = 0;
  // setting up Gaussian calculation
  t_real const sigma_x = fwhm_x / (2 * std::sqrt(2 * std::log(2)));
  t_real const sigma_y = fwhm_y / (2 * std::sqrt(2 * std::log(2)));
  // calculating Gaussian
  t_real const a = std::pow(std::cos(theta), 2) / (2 * sigma_x * sigma_x) +
                   std::pow(std::sin(theta), 2) / (2 * sigma_y * sigma_y);
  t_real const b = -std::sin(2 * theta) / (4 * sigma_x * sigma_x) +
                   std::sin(2 * theta) / (4 * sigma_y * sigma_y);
  t_real const c = std::pow(std::sin(theta), 2) / (2 * sigma_x * sigma_x) +
                   std::pow(std::cos(theta), 2) / (2 * sigma_y * sigma_y);
  auto x0 = imsizex * 0.5;
  auto y0 = imsizey * 0.5;
  for (t_int i = 0; i < imsizex; ++i) {
    for (t_int j = 0; j < imsizey; ++j) {
      t_real x = i - x0;
      t_real y = j - y0;
      psf(j, i) = std::exp(-a * x * x + 2 * b * x * y - c * y * y);
    }
  }
  auto const fit = utilities::fit_fwhm(psf, 3);
  auto new_fwhm_x = std::get<0>(fit) * 2 * std::sqrt(2 * std::log(2));
  auto new_fwhm_y = std::get<1>(fit) * 2 * std::sqrt(2 * std::log(2));
  auto new_theta = std::get<2>(fit);
  CHECK(std::abs(new_fwhm_x - fwhm_x) < 1e-13);
  CHECK(std::abs(new_fwhm_y - fwhm_y) < 1e-13);
  CHECK(std::abs(new_theta - theta) < 1e-13);
}

TEST_CASE("utilities [sparse multiply]", "[sparse multiply]") {
  // Checking that the parallel sparse matrix multiplication works against Eigen.
  t_int cols = 1024 * 1024;
  t_int rows = 1e3;
  t_int nz_values = 16 * rows;
  Vector<t_complex> const x = Vector<t_complex>::Random(cols);

  std::vector<t_tripletList> tripletList;
  tripletList.reserve(nz_values);
  Vector<t_complex> M_values = Vector<t_complex>::Random(nz_values);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis_row(0, rows);
  std::uniform_real_distribution<> dis_col(0, cols);
  for (t_int i = 0; i < nz_values; ++i) {
    tripletList.emplace_back(std::floor(dis_row(rd)), std::floor(dis_col(rd)), M_values(i));
  }
  Sparse<t_complex> M(rows, cols);
  M.setFromTriplets(tripletList.begin(), tripletList.end());

  Vector<t_complex> const parallel_output = utilities::sparse_multiply_matrix(M, x);
  Vector<t_complex> const correct_output = M * x;

  for (t_int i = 0; i < rows; ++i) {
    CHECK(std::abs((correct_output(i) - parallel_output(i)) / correct_output(i)) < 1e-13);
  }
}

TEST_CASE("generate_baseline") {
  // testing if randomly generating a uvcoverage from baseline configuration works
  const Matrix<t_real> B = utilities::generate_antennas(4, 1);
  CHECK(B.allFinite());
  CHECK(B.rows() == 4);
  CHECK(B.cols() == 3);
  CAPTURE(B);
  SECTION("one wavelength") {
    const t_real frequency = constant::c;
    const utilities::vis_params test_coverage = utilities::antenna_to_coverage(B, frequency);
    CHECK(test_coverage.units == utilities::vis_units::lambda);
    const utilities::vis_params test_coverage_xyz =
        utilities::antenna_to_coverage(B.col(0), B.col(1), B.col(2), frequency);
    CHECK(test_coverage_xyz.u.isApprox(test_coverage.u));
    CHECK(test_coverage_xyz.v.isApprox(test_coverage.v));
    CHECK(test_coverage_xyz.w.isApprox(test_coverage.w));
    CHECK(test_coverage.u.allFinite());
    CHECK(test_coverage.v.allFinite());
    CHECK(test_coverage.w.allFinite());
    CHECK(test_coverage.size() == 2 * 3);
    const Vector<t_real> R0 = B.row(0) - B.row(1);
    const Vector<t_real> R1 = B.row(0) - B.row(2);
    const Vector<t_real> R2 = B.row(0) - B.row(3);
    const Vector<t_real> R3 = B.row(1) - B.row(2);
    const Vector<t_real> R4 = B.row(1) - B.row(3);
    const Vector<t_real> R5 = B.row(2) - B.row(3);
    CHECK(R0(0) == Approx(test_coverage.u(0)));
    CHECK(R0(1) == Approx(test_coverage.v(0)));
    CHECK(R0(2) == Approx(test_coverage.w(0)));
    CHECK(R1(0) == Approx(test_coverage.u(1)));
    CHECK(R1(1) == Approx(test_coverage.v(1)));
    CHECK(R1(2) == Approx(test_coverage.w(1)));
    CHECK(R2(0) == Approx(test_coverage.u(2)));
    CHECK(R2(1) == Approx(test_coverage.v(2)));
    CHECK(R2(2) == Approx(test_coverage.w(2)));
    CHECK(R3(0) == Approx(test_coverage.u(3)));
    CHECK(R3(1) == Approx(test_coverage.v(3)));
    CHECK(R3(2) == Approx(test_coverage.w(3)));
    CHECK(R4(0) == Approx(test_coverage.u(4)));
    CHECK(R4(1) == Approx(test_coverage.v(4)));
    CHECK(R4(2) == Approx(test_coverage.w(4)));
    CHECK(R5(0) == Approx(test_coverage.u(5)));
    CHECK(R5(1) == Approx(test_coverage.v(5)));
    CHECK(R5(2) == Approx(test_coverage.w(5)));
  }
  SECTION("more wavelengths") {
    const auto frequency = std::vector<t_real>{constant::c, 2 * constant::c, 5 * constant::c};
    const t_int chan_size = B.rows() * (B.rows() - 1.) / 2.;
    const utilities::vis_params test_coverage = utilities::antenna_to_coverage(B, frequency);
    CHECK(test_coverage.u.allFinite());
    CHECK(test_coverage.v.allFinite());
    CHECK(test_coverage.w.allFinite());
    CHECK(test_coverage.size() == chan_size * frequency.size());
    t_int f_index = 0;
    for (auto& f : frequency) {
      CAPTURE(f);
      CAPTURE(f_index);
      CAPTURE(frequency.size());
      const utilities::vis_params test_coverage_f = utilities::antenna_to_coverage(B, f);
      CHECK(test_coverage_f.u.isApprox(test_coverage.u.segment(f_index * chan_size, chan_size)));
      CHECK(test_coverage_f.v.isApprox(test_coverage.v.segment(f_index * chan_size, chan_size)));
      CHECK(test_coverage_f.w.isApprox(test_coverage.w.segment(f_index * chan_size, chan_size)));
      f_index++;
    }
    CHECK(f_index == frequency.size());
  }
}

TEST_CASE("generate coverage from antenna positions") {
  const std::string pos_filename = mwa_filename("Phase1_config.txt");

  auto const B = utilities::read_ant_positions(pos_filename);
  CHECK(B.rows() == 128);
  CHECK(B.cols() == 3);
  CHECK(B.allFinite());
  SECTION("generate coverage") {
    SECTION("one frequency") {
      const t_real f = constant::c;
      auto const coverage_from_file = utilities::read_ant_positions_to_coverage(pos_filename, f);
      auto const coverage_from_B = utilities::antenna_to_coverage(B, f);
      CHECK(coverage_from_file.u.isApprox(coverage_from_B.u));
      CHECK(coverage_from_file.v.isApprox(coverage_from_B.v));
      CHECK(coverage_from_file.w.isApprox(coverage_from_B.w));
    }
    SECTION("multi frequency") {
      const std::vector<t_real> f = {constant::c, constant::c * 4, constant::c * 2};
      auto const coverage_from_file = utilities::read_ant_positions_to_coverage(pos_filename, f);
      auto const coverage_from_B = utilities::antenna_to_coverage(B, f);
      CHECK(coverage_from_file.u.isApprox(coverage_from_B.u));
      CHECK(coverage_from_file.v.isApprox(coverage_from_B.v));
      CHECK(coverage_from_file.w.isApprox(coverage_from_B.w));
    }
  }
}

TEST_CASE("conjugate symmetry") {
  t_uint const number_of_vis = 100;
  t_uint const max_w = 100;
  t_real const sigma_m = 1000;
  const auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m, max_w);
  const auto reflected_data = utilities::conjugate_w(uv_data);
  REQUIRE(uv_data.size() == reflected_data.size());
  for (t_uint i = 0; i < uv_data.size(); i++) {
    if (uv_data.w(i) < 0) {
      REQUIRE(uv_data.u(i) == Approx(-reflected_data.u(i)).epsilon(1e-12));
      REQUIRE(uv_data.v(i) == Approx(-reflected_data.v(i)).epsilon(1e-12));
      REQUIRE(uv_data.w(i) == Approx(-reflected_data.w(i)).epsilon(1e-12));
      REQUIRE(uv_data.vis(i).real() ==
              Approx(std::conj(reflected_data.vis(i)).real()).epsilon(1e-12));
      REQUIRE(uv_data.vis(i).imag() ==
              Approx(std::conj(reflected_data.vis(i)).imag()).epsilon(1e-12));
    } else {
      REQUIRE(uv_data.u(i) == Approx(reflected_data.u(i)).epsilon(1e-12));
      REQUIRE(uv_data.v(i) == Approx(reflected_data.v(i)).epsilon(1e-12));
      REQUIRE(uv_data.w(i) == Approx(reflected_data.w(i)).epsilon(1e-12));
      REQUIRE(uv_data.vis(i).real() == Approx(reflected_data.vis(i).real()).epsilon(1e-12));
      REQUIRE(uv_data.vis(i).imag() == Approx(reflected_data.vis(i).imag()).epsilon(1e-12));
    }
  }
}
