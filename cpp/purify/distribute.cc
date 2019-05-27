#include "purify/distribute.h"
#include "purify/wide_field_utilities.h"

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
std::tuple<std::vector<t_int>, std::vector<t_real>> kmeans_algo(
    const Vector<t_real> &w, const t_int number_of_nodes, const t_int iters,
    const std::function<t_real(t_real)> &cost, const t_real rel_diff) {
  std::vector<t_int> w_node(w.size(), 0);
  std::vector<t_real> w_centre(number_of_nodes, 0);
  std::vector<t_real> w_sum(number_of_nodes, 0);
  std::vector<t_real> w_count(number_of_nodes, 0);
  t_real diff = 1;
  t_real const wmin = w.minCoeff();
  t_real const wmax = w.maxCoeff();
  PURIFY_DEBUG("Min w = {}", wmin);
  PURIFY_DEBUG("Max w = {}", wmax);
  for (int i = 0; i < w_centre.size(); i++)
    w_centre[i] = (i * (wmax - wmin) / number_of_nodes + wmin);
  // lopp through even nodes to reduces w-term
  for (int n = 0; n < iters; n++) {
    PURIFY_DEBUG("clustering iteration {}", n);
    for (int i = 0; i < w.size(); i++) {
      t_real min = 2 * cost(wmax - wmin);
      for (int node = 0; node < number_of_nodes; node++) {
        const t_real cost_val = cost(w(i) - w_centre.at(node));
        if (cost_val < min) {
          min = cost_val;
          w_node[i] = node;
        }
      }
      w_sum[w_node.at(i)] += w(i);
      w_count[w_node.at(i)]++;
    }
    for (int j = 0; j < number_of_nodes; j++) {
      diff += std::abs(((w_count.at(j) > 0) ? w_sum.at(j) / w_count.at(j) : 0) - w_centre.at(j)) /
              std::abs(w_centre.at(j) * number_of_nodes);
      w_centre[j] = (w_count.at(j) > 0) ? w_sum.at(j) / w_count.at(j) : 0;
      PURIFY_DEBUG("Node {} has {} visibilities, using w-stack w = {}.", j, w_count.at(j),
                   w_centre.at(j));
      w_sum[j] = 0;
      w_count[j] = 0;
    }
    if (diff < rel_diff) {
      PURIFY_DEBUG("Converged!");
      break;
    } else {
      PURIFY_DEBUG("relative_diff = {}", diff);
      diff = 0;
    }
  }

  return std::make_tuple(w_node, w_centre);
}
#ifdef PURIFY_MPI
std::tuple<std::vector<t_int>, std::vector<t_real>> kmeans_algo(
    const Vector<t_real> &w, const t_int number_of_nodes, const t_int iters,
    sopt::mpi::Communicator const &comm, const std::function<t_real(t_real)> &cost,
    const t_real rel_diff) {
  std::vector<t_int> w_node(w.size(), 0);
  std::vector<t_real> w_centre(number_of_nodes, 0);
  std::vector<t_real> w_sum(number_of_nodes, 0);
  std::vector<t_real> w_count(number_of_nodes, 0);
  t_real diff = 1;
  t_real const wmin = comm.all_reduce<t_real>(w.minCoeff(), MPI_MIN);
  t_real const wmax = comm.all_reduce<t_real>(w.maxCoeff(), MPI_MAX);
  if (comm.is_root()) PURIFY_DEBUG("Min w = {}", wmin);
  if (comm.is_root()) PURIFY_DEBUG("Max w = {}", wmax);
  for (int i = 0; i < w_centre.size(); i++)
    w_centre[i] =
        (static_cast<t_real>(i) * (wmax - wmin) / static_cast<t_real>(number_of_nodes) + wmin);
  // lopp through even nodes to reduces w-term
  for (int n = 0; n < iters; n++) {
    if (comm.is_root()) PURIFY_DEBUG("clustering iteration {}", n);
    for (int i = 0; i < w.size(); i++) {
      t_real min = 2 * cost(wmax - wmin);
      for (int node = 0; node < number_of_nodes; node++) {
        const t_real cost_val = cost(w(i) - w_centre.at(node));
        if (cost_val < min) {
          min = cost_val;
          w_node[i] = node;
        }
      }
      w_sum[w_node.at(i)] += w(i);
      w_count[w_node.at(i)]++;
    }
    for (int j = 0; j < number_of_nodes; j++) {
      const t_real global_w_sum = comm.all_sum_all<t_real>(w_sum.at(j));
      const t_real global_w_count = comm.all_sum_all<t_real>(w_count.at(j));
      diff +=
          std::abs(((global_w_count > 0) ? global_w_sum / global_w_count : 0) - w_centre.at(j)) /
          std::abs(w_centre.at(j) * number_of_nodes);
      w_centre[j] = (global_w_count > 0) ? global_w_sum / global_w_count : 0;
      if (comm.is_root())
        PURIFY_DEBUG("Node {} has {} visibilities, using w-stack w = {}.", j, global_w_count,
                     w_centre.at(j));
      w_sum[j] = 0;
      w_count[j] = 0;
    }
    if (diff < rel_diff) {
      if (comm.is_root()) PURIFY_DEBUG("Converged!");
      break;
    } else {
      if (comm.is_root()) PURIFY_DEBUG("relative_diff = {}", diff);
      diff = 0;
    }
  }

  return std::make_tuple(w_node, w_centre);
}

std::vector<t_int> w_support(Vector<t_real> const &w, const std::vector<t_int> &image_index,
                             const std::vector<t_real> &w_stacks, const t_real du,
                             const t_int min_support, const t_int max_support,
                             const t_real fill_relaxation, sopt::mpi::Communicator const &comm) {
  t_real coeff_total = 0;
  for (t_int i = 0; i < w.size(); i++)
    coeff_total += widefield::w_support(std::abs(w(i) - w_stacks.at(image_index.at(i))), du,
                                        min_support, max_support) *
                   widefield::w_support(std::abs(w(i) - w_stacks.at(image_index.at(i))), du,
                                        min_support, max_support);
  const t_real coeff_average =
      comm.all_sum_all<t_real>(coeff_total) / static_cast<t_real>(comm.size());
  if (comm.is_root())
    PURIFY_DEBUG("Each node should have on average {} coefficients.", coeff_average);
  t_real coeff_sum = 0;
  t_int group = 0;
  std::vector<t_int> groups(w.size(), comm.rank());
  std::vector<t_int> coeffs(comm.size(), comm.rank());
  t_int total = 0;
  for (t_int rank = 0; rank < comm.size(); rank++) {
    const auto size = comm.broadcast(w.size(), rank);
    for (t_int i = 0; i < size; i++) {
      if (comm.rank() == rank) {
        const t_int cost = widefield::w_support(std::abs(w(i) - w_stacks.at(image_index.at(i))), du,
                                                min_support, max_support) *
                           widefield::w_support(std::abs(w(i) - w_stacks.at(image_index.at(i))), du,
                                                min_support, max_support);
        total += cost;
        if (group < (comm.size() - 1))
          if ((cost + coeff_sum) > coeff_average * (1. + fill_relaxation)) {
            PURIFY_DEBUG("{} node should have {} coefficients.", group, coeff_sum);
            coeff_sum = 0;
            group++;
          }
        coeff_sum += cost;
        groups[i] = group;
      }
    }
    if (group > comm.size() - 1)
      throw std::runtime_error(
          "Error distributing visibilites to even computational load for wide field imaging. Group "
          "number out of bounds.");
    coeff_sum = comm.broadcast(coeff_sum, rank);
    group = comm.broadcast(group, rank);

    if (total != coeff_total and comm.rank() == rank)
      throw std::runtime_error(
          "Total number of coefficients calculated is not the same, loop might be broken.");
  }
  if (comm.is_root()) PURIFY_DEBUG("{} node should have {} coefficients.", group, coeff_sum);
  return groups;
}
#endif

}  // namespace distribute
}  // namespace purify
