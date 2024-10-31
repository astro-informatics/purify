#ifndef PURIFY_H5READER_H
#define PURIFY_H5READER_H
#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"
#include "purify/uvw_utilities.h"

#ifdef PURIFY_MPI
#include <sopt/mpi/communicator.h>
#endif

#include "highfive/H5File.hpp"

#include <algorithm>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace purify::H5 {

/// @brief Purify interface class to handle HDF5 input files
class H5Handler {
  using DatsetMap = std::map<std::string, HighFive::DataSet>;

 public:
  H5Handler() = delete;

  H5Handler(const std::string& filename) : _file(filename) {}

  /// Method to read the entire dataset
  template <typename T = double>
  std::vector<T> read(const std::string& label) {
    auto dataset = _file.getDataSet(label);
    return dataset.read<std::vector<T>>();
  }

  /// Method to read a dataset slice with
  /// slices evenly split across MPI ranks
  template <typename T = double>
  std::vector<T> distread(const std::string& label, const sopt::mpi::Communicator& comm) {
    if (_ds.find(label) == _ds.end()) {  // load the dataset
      _ds[label] = std::move(_file.getDataSet(label));
    }
    const auto& dims = _ds[label].getDimensions();
    size_t datalen = dims.at(0);
    if (datalen < comm.size()) throw std::runtime_error("Not enough data for each MPI rank!");

    // Read the relevant slice of the dataset
    // @todo Cache the calculation of starting point/slice length?
    size_t len = datalen / comm.size();
    if (comm.rank() == comm.size() - 1) {
      len += datalen % comm.size();
    }
    size_t pos = comm.rank() * len;
    std::vector<T> data;
    data.reserve(len);
    _ds[label].select({pos}, {len}).read(data);
    return data;
  }

  /// Method to stochastically draw a subset
  /// of the distributed dataset slice
  template <typename T = double>
  std::vector<T> stochread(const std::string& label, size_t N,
                           const sopt::mpi::Communicator& comm) {
    std::vector<T> data = distread(label, comm);
    if (N > data.size()) throw std::runtime_error("Not enough data for requested dataset size!");
    // stochastic shuffle
    std::mt19937 rng(rnd_device());
    std::shuffle(std::begin(data), std::end(data), rng);
    data.resize(N);  // clip dataset to first N elements
    return data;
  }

 private:
  /// HDF5 file
  const HighFive::File _file;

  DatsetMap _ds;

  std::random_device rnd_device;
};

/// @brief Reads an HDF5 file with u, v, visibilities and returns the vectors.
///
/// @note vis_name: name of input HDF5 file containing [u, v, real(V), imag(V)].
utilities::vis_params read_visibility(const std::string& vis_name, const bool w_term) {
  H5Handler vis_file(vis_name);
  utilities::vis_params uv_vis;

  std::vector<t_real> utemp = vis_file.read<t_real>("u");
  uv_vis.u = Eigen::Map<Vector<t_real>>(utemp.data(), utemp.size(), 1);

  // found that a reflection is needed for the orientation
  // of the gridded image to be correct
  std::vector<t_real> vtemp = vis_file.read<t_real>("v");
  uv_vis.v = -Eigen::Map<Vector<t_real>>(vtemp.data(), vtemp.size(), 1);

  if (w_term) {
    std::vector<t_real> wtemp = vis_file.read<t_real>("w");
    uv_vis.w = Eigen::Map<Vector<t_real>>(wtemp.data(), wtemp.size(), 1);
  } else {
    uv_vis.w = Vector<t_real>::Zero(utemp.size());
  }

  std::vector<t_real> retemp = vis_file.read<t_real>("re");
  std::vector<t_real> imtemp = vis_file.read<t_real>("im");
  std::vector<t_real> sigma = vis_file.read<t_real>("sigma");
  assert(retemp.size() == imtemp.size());

  uv_vis.vis = Vector<t_complex>::Zero(retemp.size());
  uv_vis.weights = Vector<t_complex>::Zero(retemp.size());
  for (size_t i = 0; i < retemp.size(); ++i) {
    uv_vis.vis(i) = t_complex(retemp[i], imtemp[i]);
    uv_vis.weights(i) = 1 / sigma[i];
  }

  uv_vis.ra = 0;
  uv_vis.dec = 0;
  uv_vis.average_frequency = 0;

  return uv_vis;
}

}  // namespace purify::H5

#endif
