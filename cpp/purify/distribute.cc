#include "purify/distribute.h"

namespace purify {
namespace distribute {
std::vector<t_int> distribute_measurements(Vector<t_real> const &u, Vector<t_real> const &v,
                                           Vector<t_real> const &w, t_int const number_of_nodes,
                                           std::string const &distribution_plan,
                                           t_int const &grid_size) {
  // distrubte visibilities from a measurement
  Vector<t_int> index = Vector<t_int>::LinSpaced(u.size(), 0, u.size());
  t_int const patition_size
      = std::ceil(static_cast<t_real>(u.size()) / static_cast<t_real>(number_of_nodes));
  PURIFY_DEBUG(
      "Using {} to make {} partitions from {} visibilities, with {} visibilities per a node.",
      distribution_plan, number_of_nodes, index.size(), patition_size);
  // return a vector of vectors of indicies for each node
  if(distribution_plan == "equal_distribution")
    index = equal_distribution(u, v, grid_size);
  if(distribution_plan == "distance_distribution")
    index = distance_distribution(u, v);
  std::vector<t_int> patitions(u.size());
  // creating patitions
  for(t_int i = 0; i < index.size(); i++) {
    if(std::floor(static_cast<t_real>(i) / static_cast<t_real>(patition_size))
       > number_of_nodes - 1) {
      PURIFY_ERROR("Error: Probably a bug in distribution plan {}.", distribution_plan);
      throw std::runtime_error("Distributing data into too many nodes");
    }
    patitions[index(i)] = std::floor(static_cast<t_real>(i) / static_cast<t_real>(patition_size));
  }
  return patitions;
}
Vector<t_int> distance_distribution(Vector<t_real> const &u, Vector<t_real> const &v) {
  // sort visibilities by distance from centre
  Vector<t_int> index = Vector<t_int>::LinSpaced(u.size(), 0, u.size());
  std::sort(index.data(), index.data() + index.size(), [&u, &v](int a, int b) {
    return std::sqrt(u(a) * u(a) + v(a) * v(a)) < std::sqrt(u(b) * u(b) + v(b) * v(b));
  });
  return index;
}
Vector<t_int>
equal_distribution(Vector<t_real> const &u, Vector<t_real> const &v, t_int const &grid_size) {
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
  for(t_int i = 0; i < index.size(); i++) {
    histogram(std::floor(scaled_u(i)), std::floor(scaled_v(i))) += 1;
  }
  // sorting indicies
  std::sort(index.data(), index.data() + index.size(),
            [&histogram, &scaled_u, &scaled_v](int a, int b) {
              return (histogram(std::floor(scaled_u(a)), std::floor(scaled_v(a)))
                      < histogram(std::floor(scaled_u(b)), std::floor(scaled_v(b))));
            });
  return index;
}
}
}
