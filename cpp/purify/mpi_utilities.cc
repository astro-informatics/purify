#include <iostream>
#include "purify/config.h"
#include "purify/mpi_utilities.h"

namespace purify {
void regroup(utilities::vis_params &uv_params, std::vector<t_int> const &groups_) {
  std::vector<t_int> groups = groups_;
  // Figure out size of each group
  std::map<t_int, t_int> sizes;
  for(auto const &group : groups)
    if(sizes.count(group) == 0)
      sizes[group] = 1;
    else
      ++sizes[group];

  std::map<t_int, t_int> indices, ends;
  auto i = 0;
  for(auto const &item : sizes) {
    indices[item.first] = i;
    i += item.second;
    ends[item.first] = i;
  }

  auto const expected = [&ends](t_int i) {
    t_int j = 0;
    for(auto const end : ends) {
      if(i < end.second)
        return j;
      ++j;
    }
    return j;
  };

  i = 0;
  while(i < uv_params.u.size()) {
    auto const expected_proc = expected(i);
    if(groups[i] == expected_proc) {
      ++i;
      continue;
    }
    auto &swapper = indices[groups[i]];
    if(groups[swapper] == expected(swapper)) {
      ++swapper;
      continue;
    }
    std::swap(groups[i], groups[swapper]);
    std::swap(uv_params.u(i), uv_params.u(swapper));
    std::swap(uv_params.v(i), uv_params.v(swapper));
    std::swap(uv_params.w(i), uv_params.w(swapper));
    std::swap(uv_params.vis(i), uv_params.vis(swapper));
    std::swap(uv_params.weights(i), uv_params.weights(swapper));

    ++swapper;
    
  }
}
} // namespace purify
