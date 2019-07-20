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
      const t_real l = spherical_resample::calculate_l(theta_0, phi_0);
      const t_real m = spherical_resample::calculate_m(theta_0, phi_0);
      const t_real n = spherical_resample::calculate_n(phi_0);
      CHECK(l == Approx(0.));
      CHECK(m == Approx(0.));
      CHECK(n == Approx(1.));
    }
    SECTION("l = 1, m = 0, n = 0") {
      const t_real theta = 0;
      const t_real phi = constant::pi / 2.;
      const t_real l = spherical_resample::calculate_l(theta, phi);
      const t_real m = spherical_resample::calculate_m(theta, phi);
      const t_real n = spherical_resample::calculate_n(phi);
      CHECK(l == Approx(1.));
      CHECK(m == Approx(0.));
      CHECK(n == Approx(0.).margin(1e-12));
    }
    SECTION("l = 0, m = 1, n = 0") {
      const t_real theta = constant::pi / 2.;
      const t_real phi = constant::pi / 2.;
      const t_real l = spherical_resample::calculate_l(theta, phi);
      const t_real m = spherical_resample::calculate_m(theta, phi);
      const t_real n = spherical_resample::calculate_n(phi);
      CHECK(l == Approx(0.).margin(1e-12));
      CHECK(m == Approx(1.));
      CHECK(n == Approx(0.).margin(1e-12));
    }
  }
  SECTION("rotations") {
    SECTION("l = 0, m = 0, n = 1") {
      const t_real theta_0 = -constant::pi / 2.;
      const t_real phi_0 = constant::pi / 3.;
      CHECK(spherical_resample::calculate_l(theta_0, phi_0) ==
            Approx(spherical_resample::calculate_l(0., 0., theta_0, phi_0, 0.)));
      CHECK(spherical_resample::calculate_m(theta_0, phi_0) ==
            Approx(spherical_resample::calculate_m(0., 0., theta_0, phi_0, 0.)));
      CHECK(spherical_resample::calculate_n(phi_0) ==
            Approx(spherical_resample::calculate_n(0., 0., 0., phi_0, theta_0)));
    }
    SECTION("l = 1, m = 0, n = 0") {
      const t_real theta_0 = 0;
      const t_real phi_0 = constant::pi / 2.;
      CHECK(spherical_resample::calculate_l(theta_0, phi_0) ==
            Approx(spherical_resample::calculate_l(0., 0., theta_0, phi_0, 0.)));
      CHECK(spherical_resample::calculate_m(theta_0, phi_0) ==
            Approx(spherical_resample::calculate_m(0., 0., theta_0, phi_0, 0.)));
      CHECK(spherical_resample::calculate_n(phi_0) ==
            Approx(spherical_resample::calculate_n(0., 0., theta_0, phi_0, 0.)));
    }
    SECTION("l = 0, m = 1, n = 0") {
      const t_real theta_0 = constant::pi / 2.;
      const t_real phi_0 = constant::pi / 2.;
      CHECK(spherical_resample::calculate_l(theta_0, phi_0) ==
            Approx(spherical_resample::calculate_l(0., 0., theta_0, phi_0, 0.)));
      CHECK(spherical_resample::calculate_m(theta_0, phi_0) ==
            Approx(spherical_resample::calculate_m(0., 0., theta_0, phi_0, 0.)));
      CHECK(spherical_resample::calculate_n(phi_0) ==
            Approx(spherical_resample::calculate_n(0., 0., theta_0, phi_0, 0.)));
    }
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
