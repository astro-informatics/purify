#include "purify/distribute.h"
#include "purify/types.h"
#include "catch.hpp"
#include "purify/directories.h"
#include "purify/utilities.h"

using namespace purify;
using namespace purify::notinstalled;
TEST_CASE("Distribute") {
  // Test splitting up of data for separate nodes
  auto const uv_data = utilities::read_visibility(vla_filename("at166B.3C129.c0.vis"));
  std::printf("Distributing groups\n");
  t_int number_of_groups = 4;
  t_int number_of_vis = uv_data.u.size();
  std::vector<t_int> groups_equal = distribute::distribute_measurements(
      uv_data.u.segment(0, number_of_vis), uv_data.v.segment(0, number_of_vis),
      uv_data.w.segment(0, number_of_vis), number_of_groups, distribute::plan::equal, 1024);
  // Testing number of visiblities in groups adds to total
  CHECK(number_of_vis == groups_equal.size());
  for (t_int i = 0; i < groups_equal.size(); i++) {
    // checking nodes are in allowable values
    CHECK(groups_equal[i] >= 0);
    CHECK(groups_equal[i] < number_of_groups);
  }
  std::vector<t_int> groups_distance = distribute::distribute_measurements(
      uv_data.u.segment(0, number_of_vis), uv_data.v.segment(0, number_of_vis),
      uv_data.w.segment(0, number_of_vis), number_of_groups, distribute::plan::radial);
  // Testing number of visiblities in groups adds to total
  CHECK(number_of_vis == groups_distance.size());
  for (t_int i = 0; i < groups_distance.size(); i++) {
    // checking nodes are in allowable values
    CHECK(groups_distance[i] >= 0);
    CHECK(groups_distance[i] < number_of_groups);
  }
  std::vector<t_int> groups_noorder = distribute::distribute_measurements(
      uv_data.u.segment(0, number_of_vis), uv_data.v.segment(0, number_of_vis),
      uv_data.w.segment(0, number_of_vis), number_of_groups, distribute::plan::none);
  // Testing number of visiblities in groups adds to total
  CHECK(number_of_vis == groups_noorder.size());
  for (t_int i = 0; i < groups_noorder.size(); i++) {
    // checking nodes are in allowable values
    CHECK(groups_noorder[i] >= 0);
    CHECK(groups_noorder[i] < number_of_groups);
  }
  std::vector<t_int> groups_w_term = distribute::distribute_measurements(
      uv_data.u.segment(0, number_of_vis), uv_data.v.segment(0, number_of_vis),
      uv_data.w.segment(0, number_of_vis), number_of_groups, distribute::plan::w_term);
  // Testing number of visiblities in groups adds to total
  CHECK(number_of_vis == groups_distance.size());
  for (t_int i = 0; i < groups_distance.size(); i++) {
    // checking nodes are in allowable values
    CHECK(groups_distance[i] >= 0);
    CHECK(groups_distance[i] < number_of_groups);
  }
}
