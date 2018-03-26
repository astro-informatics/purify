#include "purify/config.h"
#include "catch.hpp"
#include "purify/logging.h"
#include "purify/types.h"

#include <iostream>
#include "purify/directories.h"
#include "purify/uvfits.h"
using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("readfile") {
  purify::logging::set_level("debug");
  const std::string filename = vla_filename("../mwa/uvdump_01.uvfits");
  const auto uv_data = pfitsio::read_uvfits(filename, false, pfitsio::stokes::V);
  auto const lambda = constant::c / uv_data.frequencies(0);
  t_uint baseline_i = 0;
  const t_complex value = t_complex(772.306, 223.715);
  for(t_uint i = 0; i < uv_data.baseline.size(); i++) {
    if(std::abs(uv_data.vis(i).real() - value.real()) < 1e-4) {
      std::cout << "found!" << std::endl;
      baseline_i = i;
      break;
    }
  }
  CAPTURE(uv_data.u(baseline_i) / uv_data.frequencies(0));
  CAPTURE(uv_data.v(baseline_i) / uv_data.frequencies(0));
  CAPTURE(uv_data.w(baseline_i) / uv_data.frequencies(0));
  CAPTURE(uv_data.u(baseline_i) / uv_data.frequencies(0) * constant::c);
  CAPTURE(uv_data.v(baseline_i) / uv_data.frequencies(0) * constant::c);
  CAPTURE(uv_data.w(baseline_i) / uv_data.frequencies(0) * constant::c);
  CAPTURE(uv_data.vis(baseline_i));
  CAPTURE(uv_data.baseline(baseline_i));
  CAPTURE(uv_data.time(baseline_i));
  CHECK(std::abs(uv_data.vis(baseline_i) - t_complex(772.306, 223.715)) < 1e-4);
}
