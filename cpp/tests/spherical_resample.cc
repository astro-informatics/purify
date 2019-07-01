#include "purify/config.h"
#include "purify/types.h"
#include <iostream>
#include "catch.hpp"
#include "purify/directories.h"
#include "purify/logging.h"

#include "purify/kernels.h"
#include "purify/spherical_resample.h"

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("test basic l m n calculation") {
  SECTION("north pole") {
    const t_real theta_0 = 0;
    const t_real phi_0 = 0;
    SECTION("l = 0, m = 0, n = 1") {
      const t_real l = spherical_resample::calculate_l(theta_0, phi_0, 0, 0);
      const t_real m = spherical_resample::calculate_m(theta_0, phi_0, 0, 0);
      const t_real n = spherical_resample::calculate_n(theta_0, phi_0, 0, 0);
      CHECK(l == Approx(0.));
      CHECK(m == Approx(0.));
      CHECK(n == Approx(1.));
    }
    SECTION("l = 1, m = 0, n = 0") {
      const t_real theta = 0;
      const t_real phi = constant::pi / 2.;
      const t_real l = spherical_resample::calculate_l(theta_0, phi_0, theta, phi);
      const t_real m = spherical_resample::calculate_m(theta_0, phi_0, theta, phi);
      const t_real n = spherical_resample::calculate_n(theta_0, phi_0, theta, phi);
      CHECK(l == Approx(1.));
      CHECK(m == Approx(0.));
      CHECK(n == Approx(0.).margin(1e-12));
    }
    SECTION("l = 0, m = 1, n = 0") {
      const t_real theta = constant::pi / 2.;
      const t_real phi = constant::pi / 2.;
      const t_real l = spherical_resample::calculate_l(theta_0, phi_0, theta, phi);
      const t_real m = spherical_resample::calculate_m(theta_0, phi_0, theta, phi);
      const t_real n = spherical_resample::calculate_n(theta_0, phi_0, theta, phi);
      CHECK(l == Approx(0.).margin(1e-12));
      CHECK(m == Approx(1.));
      CHECK(n == Approx(0.).margin(1e-12));
    }
  }
  SECTION("random pointing") {
    const t_real theta_0 = -constant::pi / 2.;
    const t_real phi_0 = constant::pi / 3.;
    SECTION("l = 0, m = 0, n = 1") {
      const t_real l = spherical_resample::calculate_l(theta_0, phi_0, theta_0, phi_0);
      const t_real m = spherical_resample::calculate_m(theta_0, phi_0, theta_0, phi_0);
      const t_real n = spherical_resample::calculate_n(theta_0, phi_0, theta_0, phi_0);
      CHECK(l == Approx(0.));
      CHECK(m == Approx(0.));
      CHECK(n == Approx(1.));
    }
    SECTION("l = 1, m = 0, n = 0") {
      const t_real theta = 0;
      const t_real phi = constant::pi / 2.;
      const t_real l =
          spherical_resample::calculate_l(theta_0, phi_0, theta + theta_0, phi + phi_0);
      const t_real m =
          spherical_resample::calculate_m(theta_0, phi_0, theta + theta_0, phi + phi_0);
      const t_real n =
          spherical_resample::calculate_n(theta_0, phi_0, theta + theta_0, phi + phi_0);
      CHECK(l == Approx(1.));
      CHECK(m == Approx(0.));
      CHECK(n == Approx(0.).margin(1e-12));
    }
    SECTION("l = 0, m = 1, n = 0") {
      const t_real theta = constant::pi / 2.;
      const t_real phi = constant::pi / 2.;
      const t_real l =
          spherical_resample::calculate_l(theta_0, phi_0, theta + theta_0, phi + phi_0);
      const t_real m =
          spherical_resample::calculate_m(theta_0, phi_0, theta + theta_0, phi + phi_0);
      const t_real n =
          spherical_resample::calculate_n(theta_0, phi_0, theta + theta_0, phi + phi_0);
      CHECK(l == Approx(0.).margin(1e-12));
      CHECK(m == Approx(1.));
      CHECK(n == Approx(0.).margin(1e-12));
    }
  }
  SECTION("wrapping") {
    const t_real theta_0 = 0;
    const t_real phi_0 = 0.;
    const t_real theta = constant::pi / 3.;
    const t_real phi = constant::pi / 6.;
    // wrapping of theta
    CHECK(Approx(spherical_resample::calculate_l(theta_0, phi_0, theta, phi)) ==
          spherical_resample::calculate_l(theta_0, phi_0, theta - 2 * constant::pi, phi));
    CHECK(Approx(spherical_resample::calculate_l(theta_0, phi_0, theta, phi)) ==
          spherical_resample::calculate_l(theta_0, phi_0, theta + 2 * constant::pi, phi));
    CHECK(Approx(spherical_resample::calculate_m(theta_0, phi_0, theta, phi)) ==
          spherical_resample::calculate_m(theta_0, phi_0, theta - 2 * constant::pi, phi));
    CHECK(Approx(spherical_resample::calculate_m(theta_0, phi_0, theta, phi)) ==
          spherical_resample::calculate_m(theta_0, phi_0, theta + 2 * constant::pi, phi));
    CHECK(Approx(spherical_resample::calculate_n(theta_0, phi_0, theta, phi)) ==
          spherical_resample::calculate_n(theta_0, phi_0, theta - 2 * constant::pi, phi));
    CHECK(Approx(spherical_resample::calculate_n(theta_0, phi_0, theta, phi)) ==
          spherical_resample::calculate_n(theta_0, phi_0, theta + 2 * constant::pi, phi));
    // wrapping of phi
    CHECK(Approx(spherical_resample::calculate_l(theta_0, phi_0, theta, phi)) ==
          spherical_resample::calculate_l(theta_0, phi_0, theta, phi - 2 * constant::pi));
    CHECK(Approx(spherical_resample::calculate_l(theta_0, phi_0, theta, phi)) ==
          spherical_resample::calculate_l(theta_0, phi_0, theta, phi + 2 * constant::pi));
    CHECK(Approx(spherical_resample::calculate_m(theta_0, phi_0, theta, phi)) ==
          spherical_resample::calculate_m(theta_0, phi_0, theta, phi - 2 * constant::pi));
    CHECK(Approx(spherical_resample::calculate_m(theta_0, phi_0, theta, phi)) ==
          spherical_resample::calculate_m(theta_0, phi_0, theta, phi + 2 * constant::pi));
    CHECK(Approx(spherical_resample::calculate_n(theta_0, phi_0, theta, phi)) ==
          spherical_resample::calculate_n(theta_0, phi_0, theta, phi - 2 * constant::pi));
    CHECK(Approx(spherical_resample::calculate_n(theta_0, phi_0, theta, phi)) ==
          spherical_resample::calculate_n(theta_0, phi_0, theta, phi + 2 * constant::pi));
  }
}

TEST_CASE("select resamples") {
  const t_int imsizex = 1024;
  const t_int imsizey = imsizex;
  const t_real dl = 2. / imsizex;
  const t_real dm = 2. / imsizey;
  const t_int N = 1000;
  const Vector<t_real> l = Vector<t_real>::Random(N);
  const Vector<t_real> m = Vector<t_real>::Random(N);
  const Vector<t_real> n = Vector<t_real>::Random(N);
  const std::vector<t_int> indicies =
      spherical_resample::generate_indicies(l, m, n, imsizey, imsizex, dl, dm);
  CAPTURE(l.head(20).transpose());
  CAPTURE(m.head(20).transpose());
  CAPTURE(n.head(20).transpose());
  REQUIRE(indicies.size() > 0);
  for (t_int k = 0; k < indicies.size(); k++) {
    CHECK(std::abs(l(indicies.at(k))) < dl * imsizex * 0.5);
    CHECK(std::abs(m(indicies.at(k))) < dm * imsizex * 0.5);
    CHECK(n(indicies.at(k)) > 0);
  }
}
TEST_CASE("resample operator construction") {
  const t_int number_of_samples = 128;
  const t_real theta_0 = 0;  // radians
  const t_real phi_0 = 0;    // radians
  const t_int imsizey = 256;
  const t_int imsizex = 256;
  const t_real dl = 0.5 / imsizex;
  const t_real dm = 0.5 / imsizey;
  const t_int Jl = 4;
  const t_int Jm = 4;
  const t_int num_theta = std::sqrt(number_of_samples);
  const t_int num_phi = std::sqrt(number_of_samples);

  std::function<t_real(t_real)> kernell, kernelm, ftkernell, ftkernelm;
  std::tie(kernell, kernelm, ftkernell, ftkernelm) =
      purify::create_kernels(kernels::kernel_from_string.at("kb"), Jl, Jm, imsizey, imsizex, 1);
  const auto theta = [num_theta](const t_int k) -> t_real {
    return (k % num_theta) / num_theta * 2 * constant::pi;
  };
  const auto phi = [num_theta, num_phi](const t_int k) -> t_real {
    return std::floor(k / num_theta) / num_phi * constant::pi;
  };

  const auto resample_operator =
      spherical_resample::init_resample_operator_2d<Vector<t_complex>,
                                                    std::function<t_real(t_int)>>(
          number_of_samples, theta_0, phi_0, theta, phi, imsizey, imsizex, dl, dm, kernell, kernelm,
          Jl, Jm);
}
