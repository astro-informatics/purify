#include "purify/distribute.h"

namespace purify {
namespace distribute {

std::vector<t_int> distribute_measurements(Vector<t_real> const &u, Vector<t_real> const &v,
                                           Vector<t_real> const &w, t_int const number_of_nodes,
                                           distribute::plan const distribution_plan,
                                           t_int const &grid_size) {
  // distrubte visibilities from a measurement
  Vector<t_int> index = Vector<t_int>::LinSpaced(u.size(), 0, u.size());
  t_int const patition_size =
      std::ceil(static_cast<t_real>(u.size()) / static_cast<t_real>(number_of_nodes));
  // return a vector of vectors of indicies for each node
  std::string plan_name = "";
  switch (distribution_plan) {
  case plan::none: {
    plan_name = "none";
    break;
  }
  case plan::equal: {
    index = equal_distribution(u, v, grid_size);
    plan_name = "equal";
    break;
  }
  case plan::radial: {
    index = distance_distribution(u, v);
    plan_name = "radial";
    break;
  }
  case plan::w_term: {
    index = w_distribution(w);
    plan_name = "w_term";
    break;
  }
  default: {
    throw std::runtime_error("Distribution plan not recognised or implimented.");
    break;
  }
  }
  PURIFY_DEBUG(
      "Using {} to make {} partitions from {} visibilities, with {} visibilities per a node.",
      plan_name, number_of_nodes, index.size(), patition_size);
  std::vector<t_int> patitions(u.size());
  // creating patitions
  for (t_int i = 0; i < index.size(); i++) {
    if (std::floor(static_cast<t_real>(i) / static_cast<t_real>(patition_size)) >
        number_of_nodes - 1) {
      PURIFY_ERROR("Error: Probably a bug in distribution plan.");
      throw std::runtime_error("Distributing data into too many nodes");
    }
    patitions[index(i)] = std::floor(static_cast<t_real>(i) / static_cast<t_real>(patition_size));
  }
  return patitions;
}
Vector<t_int> w_distribution(const Vector<t_real> &u, const Vector<t_real> &v,
                             Vector<t_real> const &w) {
  return w_distribution(w);
}
Vector<t_int> w_distribution(Vector<t_real> const &w) {
  // sort visibilities by w from  w_max to w_min
  Vector<t_int> index = Vector<t_int>::LinSpaced(w.size(), 0, w.size());
  std::sort(index.data(), index.data() + index.size(), [&w](int a, int b) { return w(a) < w(b); });
  return index;
}

Vector<t_int> distance_distribution(Vector<t_real> const &u, Vector<t_real> const &v) {
  // sort visibilities by distance from centre
  Vector<t_int> index = Vector<t_int>::LinSpaced(u.size(), 0, u.size());
  std::sort(index.data(), index.data() + index.size(), [&u, &v](int a, int b) {
    return std::sqrt(u(a) * u(a) + v(a) * v(a)) < std::sqrt(u(b) * u(b) + v(b) * v(b));
  });
  return index;
}

Vector<t_int> equal_distribution(Vector<t_real> const &u, Vector<t_real> const &v,
                                 t_int const &grid_size) {
  // distribute visibilities by density calculated from histogram
  t_real const max_u = u.array().maxCoeff();
  t_real const max_v = v.array().maxCoeff();
  t_real const min_u = u.array().minCoeff();
  t_real const min_v = v.array().minCoeff();
  // scaling for histogram grid
  Vector<t_real> const scaled_u = (u.array() - min_u) * (grid_size - 1) / (max_u - min_u);
  Vector<t_real> const scaled_v = (v.array() - min_v) * (grid_size - 1) / (max_v - min_v);
  Image<t_int> histogram = Image<t_int>::Zero(grid_size, grid_size);
  Vector<t_int> index = Vector<t_int>::LinSpaced(u.size(), 0, u.size());
  // creating histogram grid
  for (t_int i = 0; i < index.size(); i++) {
    histogram(std::floor(scaled_u(i)), std::floor(scaled_v(i))) += 1;
  }
  // sorting indicies
  std::sort(index.data(), index.data() + index.size(),
            [&histogram, &scaled_u, &scaled_v](int a, int b) {
              return (histogram(std::floor(scaled_u(a)), std::floor(scaled_v(a))) <
                      histogram(std::floor(scaled_u(b)), std::floor(scaled_v(b))));
            });
  return index;
}
}  // namespace distribute
}  // namespace purify
