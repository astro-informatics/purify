#include <fstream>
#include <sstream>
#include <benchmarks/utilities.h>
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/mpi_utilities.h"
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include <sopt/linear_transform.h>

using namespace purify;
using namespace purify::notinstalled;

namespace b_utilities {

void Arguments(benchmark::internal::Benchmark *b) {
  int im_size_max = 4096;      // 4096
  int uv_size_max = 10000000;  // 1M, 10M, 100M
  int kernel_max = 16;         // 16
  for (int i = 128; i <= im_size_max; i *= 2)
    for (int j = 1000000; j <= uv_size_max; j *= 10)
      for (int k = 2; k <= kernel_max; k *= 2)
        if (k * k < i) b->Args({i, j, k});
}

double duration(std::chrono::high_resolution_clock::time_point start,
                std::chrono::high_resolution_clock::time_point end) {
  auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
  return elapsed_seconds.count();
}

bool updateImage(t_uint newSize, Image<t_complex> &image, t_uint &sizex, t_uint &sizey) {
  if (sizex == newSize) {
    return false;
  }
  const std::string &name = "M31_" + std::to_string(newSize);
  std::string const fitsfile = image_filename(name + ".fits");
  image = pfitsio::read2d(fitsfile);
  sizex = image.cols();
  sizey = image.rows();
  t_real const max = image.array().abs().maxCoeff();
  image = image * 1. / max;
  return true;
}

bool updateEmptyImage(t_uint newSize, Vector<t_complex> &image, t_uint &sizex, t_uint &sizey) {
  if (sizex == newSize) {
    return false;
  }
  image.resize(newSize * newSize);
  sizex = newSize;
  sizey = newSize;
  return true;
}

bool updateMeasurements(t_uint newSize, utilities::vis_params &data) {
  if (data.vis.size() == newSize) {
    return false;
  }
  data = b_utilities::random_measurements(newSize);
  return true;
}

bool updateMeasurements(t_uint newSize, utilities::vis_params &data, t_real &epsilon, bool newImage,
                        Image<t_complex> &image) {
  if (data.vis.size() == newSize && !newImage) {
    return false;
  }
  const t_real FoV = 1;  // deg
  const t_real cellsize = FoV / image.size() * 60. * 60.;
  std::tuple<utilities::vis_params, t_real> temp =
      b_utilities::dirty_measurements(image, newSize, 30., cellsize);
  data = std::get<0>(temp);
  epsilon = utilities::calculate_l2_radius(data.vis.size(), std::get<1>(temp));

  return true;
}

std::tuple<utilities::vis_params, t_real> dirty_measurements(
    Image<t_complex> const &ground_truth_image, t_uint number_of_vis, t_real snr,
    const t_real &cellsize) {
  auto uv_data = random_measurements(number_of_vis);
  // creating operator to generate measurements
  auto measurement_op = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uv_data, ground_truth_image.rows(), ground_truth_image.cols(), cellsize, cellsize, 2, 0, 1e-4,
      kernels::kernel::kb, 8, 8, false);
  // Generates measurements from image
  uv_data.vis = (*measurement_op) *
                Image<t_complex>::Map(ground_truth_image.data(), ground_truth_image.size(), 1);

  // working out value of signal given SNR
  auto const sigma = utilities::SNR_to_standard_deviation(uv_data.vis, snr);
  // adding noise to visibilities
  uv_data.vis = utilities::add_noise(uv_data.vis, 0., sigma);
  return std::make_tuple(uv_data, sigma);
}

utilities::vis_params random_measurements(t_int size) {
  std::stringstream filename;
  filename << "random_" << size << ".vis";
  std::string const vis_file = visibility_filename(filename.str());
  std::ifstream vis_file_str(vis_file);

  utilities::vis_params uv_data;
  if (vis_file_str.good()) {
    uv_data = utilities::read_visibility(vis_file, false);
    uv_data.units = utilities::vis_units::radians;
  } else {
    t_real const sigma_m = constant::pi / 3;
    const t_real max_w = 100.;  // lambda
    uv_data = utilities::random_sample_density(size, 0, sigma_m, max_w);
    uv_data.units = utilities::vis_units::radians;
    utilities::write_visibility(uv_data, vis_file);
  }
  return uv_data;
}

#ifdef PURIFY_MPI
utilities::vis_params random_measurements(t_int size, sopt::mpi::Communicator const &comm) {
  if (comm.is_root()) {
    // Generate random measurements
    auto uv_data = random_measurements(size);
    if (comm.size() == 1) return uv_data;

    // Distribute them
    auto const order = distribute::distribute_measurements(uv_data, comm, distribute::plan::radial);
    uv_data = utilities::regroup_and_scatter(uv_data, order, comm);
    return uv_data;
  }
  return utilities::scatter_visibilities(comm);
}

bool updateMeasurements(t_uint newSize, utilities::vis_params &data,
                        sopt::mpi::Communicator &comm) {
  if (data.vis.size() == newSize) {
    return false;
  }
  comm = sopt::mpi::Communicator::World();
  data = b_utilities::random_measurements(newSize, comm);
  return true;
}

bool updateMeasurements(t_uint newSize, utilities::vis_params &data, t_real &epsilon, bool newImage,
                        Image<t_complex> &image, sopt::mpi::Communicator &comm) {
  if (data.vis.size() == newSize && !newImage) {
    return false;
  }
  comm = sopt::mpi::Communicator::World();
  const t_real FoV = 1;  // deg
  const t_real cellsize = FoV / image.size() * 60. * 60.;
  std::tuple<utilities::vis_params, t_real> temp =
      b_utilities::dirty_measurements(image, newSize, 30., cellsize, comm);
  data = std::get<0>(temp);
  epsilon = utilities::calculate_l2_radius(data.vis.size(), std::get<1>(temp));

  return true;
}
double duration(std::chrono::high_resolution_clock::time_point start,
                std::chrono::high_resolution_clock::time_point end,
                sopt::mpi::Communicator const &comm) {
  auto elapsed_seconds = duration(start, end);
  // Now get the max time across all procs: the slowest processor is the one that is
  // holding back the others in the benchmark.
  return comm.all_reduce(elapsed_seconds, MPI_MAX);
}
std::tuple<utilities::vis_params, t_real> dirty_measurements(
    Image<t_complex> const &ground_truth_image, t_uint number_of_vis, t_real snr,
    const t_real &cellsize, sopt::mpi::Communicator const &comm) {
  if (comm.size() == 1) return dirty_measurements(ground_truth_image, number_of_vis, snr, cellsize);
  if (comm.is_root()) {
    auto result = dirty_measurements(ground_truth_image, number_of_vis, snr, cellsize);
    comm.broadcast(std::get<1>(result));
    auto const order =
        distribute::distribute_measurements(std::get<0>(result), comm, distribute::plan::radial);
    std::get<0>(result) = utilities::regroup_and_scatter(std::get<0>(result), order, comm);
    return result;
  }
  auto const sigma = comm.broadcast<t_real>();
  return std::make_tuple(utilities::scatter_visibilities(comm), sigma);
}
void update_comm(sopt::mpi::Communicator &comm) { comm = sopt::mpi::Communicator::World(); }
#endif
}  // namespace b_utilities
