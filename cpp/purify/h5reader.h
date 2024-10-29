#ifndef PURIFY_H5READER_H
#define PURIFY_H5READER_H
#include "purify/config.h"
#include "purify/logging.h"
#include "purify/types.h"
#include "purify/uvw_utilities.h"

#include "highfive/H5File.hpp"

#include <string>
#include <vector>

namespace purify::H5 {

/// @brief Purify interface class to handle HDF5 input files
class H5Handler {
 public:
  H5Handler() = delete;

  H5Handler(const std::string& filename) : _file(filename) {}

  template <typename T = double>
  std::vector<T> read(const std::string& label) {
    auto dataset = _file.getDataSet(label);
    return dataset.read<std::vector<T>>();
  }

 private:
  /// HDF5 file
  const HighFive::File _file;
};

/// @brief Reads an HDF5 file with u, v, visibilities and returns the vectors.
///
/// @note vis_name: name of input HDF5 file containing [u, v, real(V), imag(V)].
utilities::vis_params read_visibility(const std::string &vis_name, const bool w_term) {

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
