#include "purify/mpi_utilities.h"
#include "purify/config.h"
#include <iostream>
#include <type_traits>
#include "purify/distribute.h"

namespace purify {
namespace utilities {

void regroup(vis_params &uv_params, std::vector<t_int> const &groups_, const t_int max_groups) {
  std::vector<t_int> image_index(uv_params.size(), 0);
  regroup(uv_params, image_index, groups_, max_groups);
}

void regroup(vis_params &uv_params, std::vector<t_int> &image_index,
             std::vector<t_int> const &groups_, const t_int max_groups) {
  std::vector<t_int> groups = groups_;
  // Figure out size of each group
  std::map<t_int, t_int> sizes;
  for (auto g = 0; g < max_groups; g++) sizes[g] = 0;
  for (auto const &group : groups) ++sizes[group];

  std::map<t_int, t_int> indices, ends;
  auto i = 0;
  for (auto const &item : sizes) {
    indices[item.first] = i;
    i += item.second;
    ends[item.first] = i;
  }
  const auto minmax = std::minmax_element(ends.begin(), ends.end());
  if (std::get<1>(*std::get<0>(minmax)) < 0)
    throw std::runtime_error("segment end " + std::to_string(std::get<1>(*std::get<0>(minmax))) +
                             " less than 0. Not a valid end.");
  if (std::get<1>(*std::get<1>(minmax)) > uv_params.size())
    throw std::runtime_error("segment end " + std::to_string(std::get<1>(*std::get<1>(minmax))) +
                             " larger than data vector " +
                             std::to_string(static_cast<t_int>(uv_params.size())) +
                             ". End not valid.");

  auto const expected = [&ends](t_int i) {
    t_int j = 0;
    for (auto const end : ends) {
      if (i < end.second) return j;
      ++j;
    }
    return j;
  };

  i = 0;
  while (i < uv_params.u.size()) {
    auto const expected_proc = expected(i);
    if (groups[i] == expected_proc) {
      ++i;
      continue;
    }
    auto &swapper = indices[groups[i]];
    if (groups[swapper] == expected(swapper)) {
      ++swapper;
      continue;
    }
    if (swapper >= uv_params.u.size())
      throw std::runtime_error("regroup (groups, " + std::to_string(groups[swapper]) + ", " +
                               std::to_string(groups[i]) + ") index out of bounds " +
                               std::to_string(i) + " " + std::to_string(swapper) +
                               " >= " + std::to_string(uv_params.u.size()));
    std::swap(groups[i], groups[swapper]);
    std::swap(uv_params.u(i), uv_params.u(swapper));
    std::swap(uv_params.v(i), uv_params.v(swapper));
    std::swap(uv_params.w(i), uv_params.w(swapper));
    std::swap(uv_params.vis(i), uv_params.vis(swapper));
    std::swap(uv_params.weights(i), uv_params.weights(swapper));
    std::swap(image_index[i], image_index[swapper]);

    ++swapper;
  }
}

vis_params regroup_and_scatter(vis_params const &params, std::vector<t_int> const &groups,
                               sopt::mpi::Communicator const &comm) {
  if (comm.size() == 1) return params;
  if (comm.rank() != comm.root_id()) return scatter_visibilities(comm);

  std::vector<t_int> sizes(comm.size());
  std::fill(sizes.begin(), sizes.end(), 0);
  for (auto const &group : groups) {
    if (group > static_cast<t_int>(comm.size()))
      throw std::out_of_range("groups should go from 0 to comm.size()");
    ++sizes[group];
  }

  vis_params copy = params;
  regroup(copy, groups, comm.size());
  return scatter_visibilities(copy, sizes, comm);
}
std::tuple<vis_params, std::vector<t_int>> regroup_and_all_to_all(
    vis_params const &params, const std::vector<t_int> &image_index,
    std::vector<t_int> const &groups, sopt::mpi::Communicator const &comm) {
  if (comm.size() == 1) return std::make_tuple(params, image_index);
  vis_params copy = params;
  std::vector<t_int> index_copy = image_index;
  regroup(copy, index_copy, groups, comm.size());

  std::vector<t_int> sizes(comm.size());
  std::fill(sizes.begin(), sizes.end(), 0);
  for (auto const &group : groups) {
    if (group > static_cast<t_int>(comm.size()))
      throw std::out_of_range("groups should go from 0 to comm.size()");
    ++sizes[group];
  }

  return std::make_tuple(all_to_all_visibilities(copy, sizes, comm),
                         comm.all_to_allv(index_copy, sizes));
}

vis_params regroup_and_all_to_all(vis_params const &params, std::vector<t_int> const &groups,
                                  sopt::mpi::Communicator const &comm) {
  return std::get<0>(
      regroup_and_all_to_all(params, std::vector<t_int>(params.size(), 0), groups, comm));
}

vis_params all_to_all_visibilities(vis_params const &params, std::vector<t_int> const &sizes,
                                   sopt::mpi::Communicator const &comm) {
  if (comm.size() == 1) return params;
  vis_params result;
  result.u = comm.all_to_allv(params.u, sizes);
  result.v = comm.all_to_allv(params.v, sizes);
  result.w = comm.all_to_allv(params.w, sizes);
  result.vis = comm.all_to_allv(params.vis, sizes);
  result.weights = comm.all_to_allv(params.weights, sizes);
  result.units = static_cast<utilities::vis_units>(comm.broadcast(static_cast<int>(params.units)));
  result.ra = comm.broadcast(params.ra);
  result.dec = comm.broadcast(params.dec);
  result.average_frequency = comm.broadcast(params.average_frequency);
  return result;
}
vis_params scatter_visibilities(vis_params const &params, std::vector<t_int> const &sizes,
                                sopt::mpi::Communicator const &comm) {
  if (comm.size() == 1) return params;
  if (not comm.is_root()) return scatter_visibilities(comm);

  comm.scatter_one(sizes);
  vis_params result;
  result.u = comm.scatterv(params.u, sizes);
  result.v = comm.scatterv(params.v, sizes);
  result.w = comm.scatterv(params.w, sizes);
  result.vis = comm.scatterv(params.vis, sizes);
  result.weights = comm.scatterv(params.weights, sizes);
  result.units = static_cast<utilities::vis_units>(comm.broadcast(static_cast<int>(params.units)));
  result.ra = comm.broadcast(params.ra);
  result.dec = comm.broadcast(params.dec);
  result.average_frequency = comm.broadcast(params.average_frequency);
  return result;
}

vis_params scatter_visibilities(sopt::mpi::Communicator const &comm) {
  if (comm.is_root())
    throw std::runtime_error("The root node should call the *other* scatter_visibilities function");

  auto const local_size = comm.scatter_one<t_int>();
  vis_params result;
  result.u = comm.scatterv<decltype(result.u)::Scalar>(local_size);
  result.v = comm.scatterv<decltype(result.v)::Scalar>(local_size);
  result.w = comm.scatterv<decltype(result.w)::Scalar>(local_size);
  result.vis = comm.scatterv<decltype(result.vis)::Scalar>(local_size);
  result.weights = comm.scatterv<decltype(result.weights)::Scalar>(local_size);
  result.units = static_cast<utilities::vis_units>(
      comm.broadcast<std::remove_const<decltype(static_cast<int>(result.units))>::type>());
  result.ra = comm.broadcast<std::remove_const<decltype(result.ra)>::type>();
  result.dec = comm.broadcast<std::remove_const<decltype(result.dec)>::type>();
  result.average_frequency =
      comm.broadcast<std::remove_const<decltype(result.average_frequency)>::type>();
  return result;
}

utilities::vis_params distribute_params(utilities::vis_params const &params,
                                        sopt::mpi::Communicator const &comm) {
  if (comm.is_root() and comm.size() > 1) {
    auto const order = distribute::distribute_measurements(params, comm, distribute::plan::radial);
    return utilities::regroup_and_scatter(params, order, comm);
  } else if (comm.size() > 1)
    return utilities::scatter_visibilities(comm);
  return params;
}

utilities::vis_params set_cell_size(const sopt::mpi::Communicator &comm,
                                    utilities::vis_params const &uv_vis, const t_real &cell_x,
                                    const t_real &cell_y) {
  if (comm.size() == 1) return utilities::set_cell_size(uv_vis, cell_x, cell_y);

  const t_real max_u = comm.all_reduce<t_real>(uv_vis.u.array().cwiseAbs().maxCoeff(), MPI_MAX);
  const t_real max_v = comm.all_reduce<t_real>(uv_vis.v.array().cwiseAbs().maxCoeff(), MPI_MAX);
  return utilities::set_cell_size(uv_vis, max_u, max_v, cell_x, cell_y);
}
utilities::vis_params w_stacking(utilities::vis_params const &params,
                                 sopt::mpi::Communicator const &comm, const t_int iters,
                                 const std::function<t_real(t_real)> &cost) {
  const std::vector<t_int> image_index =
      std::get<0>(distribute::kmeans_algo(params.w, comm.size(), iters, comm, cost));
  return utilities::regroup_and_all_to_all(params, image_index, comm);
}
std::tuple<utilities::vis_params, std::vector<t_int>, std::vector<t_real>>
w_stacking_with_all_to_all(utilities::vis_params const &params, const t_real du,
                           const t_int min_support, const t_int max_support,
                           sopt::mpi::Communicator const &comm, const t_int iters,
                           const std::function<t_real(t_real)> &cost) {
  const auto kmeans = distribute::kmeans_algo(params.w, comm.size(), iters, comm, cost);
  std::vector<t_int> image_index = std::get<0>(kmeans);
  utilities::vis_params outdata;
  const std::vector<t_real> w_stacks = std::get<1>(kmeans);
  const std::vector<t_int> groups =
      distribute::w_support(params.w, image_index, w_stacks, du, min_support, max_support, comm);
  std::tie(outdata, image_index) =
      utilities::regroup_and_all_to_all(params, image_index, groups, comm);
  return std::tuple<utilities::vis_params, std::vector<t_int>, std::vector<t_real>>(
      outdata, image_index, w_stacks);
}
}  // namespace utilities
}  // namespace purify
