#ifndef PURIFY_H5READER_H
#define PURIFY_H5READER_H
#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"
#include "purify/uvw_utilities.h"

#ifdef PURIFY_MPI
#include <mpi.h>
#include <sopt/mpi/communicator.h>
#endif

#include "highfive/H5File.hpp"

#include <algorithm>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

#ifndef PURIFY_MPI
namespace sopt::mpi {
class Communicator;
}
#endif

namespace purify::H5 {

#ifdef PURIFY_MPI

HighFive::FileAccessProps MPIFileAccess() {
  HighFive::FileAccessProps fap;
  fap.add(HighFive::MPIOFileAccess{MPI_COMM_WORLD, MPI_INFO_NULL});
  fap.add(HighFive::MPIOCollectiveMetadata{});
  return fap;
}

HighFive::DataTransferProps MPIDataTransfer() {
  HighFive::DataTransferProps dtp;
  dtp.add(HighFive::UseCollectiveIO{});
  return dtp;
}

#endif

/// @brief Purify interface class to handle HDF5 input files
class H5Handler {
  using DatsetMap = std::map<std::string, HighFive::DataSet>;

 public:
  H5Handler() = delete;

  /// @brief Default constructor (serial behaviour)
  H5Handler(const std::string& filename)
      : _comm(nullptr),
        _fap(HighFive::FileAccessProps{}),
        _dtp(HighFive::DataTransferProps{}),
        _file(filename, HighFive::File::ReadOnly) {
    _datalen = _slicepos = _slicelen = _batchpos = 0;
  }

#ifdef PURIFY_MPI
  /// @brief Alternative constructor enabling MPI-collective behaviour
  H5Handler(const std::string& filename, const sopt::mpi::Communicator& comm)
      : _comm(&comm),
        _fap(MPIFileAccess()),
        _dtp(MPIDataTransfer()),
        _file(filename, HighFive::File::ReadOnly, _fap) {
    _datalen = _slicepos = _slicelen = _batchpos = 0;
  }
#endif

  /// Method to read the entire dataset
  template <typename T = double>
  std::vector<T> read(const std::string& label) const {
    auto dataset = _file.getDataSet(label);
    return dataset.read<std::vector<T>>();
  }

  /// Method to read a dataset slice with
  /// slices evenly split across MPI ranks
  template <typename T = double>
  std::vector<T> distread(const std::string& label) {
    if (!_comm) throw std::runtime_error("No MPI-collective reading enabled!");
    _loadDataSet(label);
    std::vector<T> data;
    data.reserve(_slicelen);
    _ds[label].select({_slicepos}, {_slicelen}).read(data, _dtp);
    return data;
  }

  /// Method to stochastically draw a subset
  /// from the distributed dataset slice
  template <typename T = double>
  std::vector<T> stochread(const std::string& label, size_t batchsize, bool shuffle = false) {
    if (!_comm) throw std::runtime_error("No MPI-collective reading enabled!");

    _loadDataSet(label);
    if (shuffle) _shuffle();

    std::vector<T> data;
    data.reserve(batchsize);
    // account for wrap around near
    // the edges of the slice
    size_t pos = _batchpos;
    while (batchsize) {
      std::vector<T> tmp;
      size_t len = std::min(batchsize, _slicepos + _slicelen - pos);
      _ds[label].select({pos}, {len}).read(tmp, _dtp);
      data.insert(data.end(), std::make_move_iterator(std::begin(tmp)),
                  std::make_move_iterator(std::end(tmp)));
      pos = _slicepos;
      batchsize -= len;
    }
    return data;
  }

 private:
  void _loadDataSet(const std::string& label) {
#ifdef PURIFY_MPI
    if (_ds.find(label) != _ds.end()) return;

    _ds[label] = std::move(_file.getDataSet(label));
    const auto& dims = _ds[label].getDimensions();
    size_t len = dims.at(0);
    if (len == 0) throw std::runtime_error("Dataset has zero length!");
    if (len < _comm->size()) throw std::runtime_error("Not enough data for each MPI rank!");
    if (_datalen == 0) {
      _datalen = len;
      // Determine starting position and length of the data slice
      _slicelen = _datalen / _comm->size();
      _slicepos = _comm->rank() * _slicelen;
      if (_comm->rank() == _comm->size() - 1) {
        _slicelen += _datalen % _comm->size();
      }
    } else if (len != _datalen) {
      throw std::runtime_error("Inconsistent dataset length!");
    }
#endif
  }

  void _shuffle() {
    std::uniform_int_distribution<size_t> uni(_slicepos, _slicepos + _slicelen - 1);
    _batchpos = uni(_rng);
  }

  const sopt::mpi::Communicator* _comm;

  const HighFive::FileAccessProps _fap;

  const HighFive::DataTransferProps _dtp;

  const HighFive::File _file;

  DatsetMap _ds;

  std::mt19937 _rng;

  size_t _datalen, _slicepos, _slicelen, _batchpos;
};

/// @brief Reads an HDF5 file with u,v visibilities, constructs a vis_params object and returns it.
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

/// @brief Stochastically reads dataset slices from the supplied HDF5-file handler,
/// constructs a vis_params object from them and returns it.
utilities::vis_params stochread_visibility(H5Handler& file, const size_t N, const bool w_term) {
  utilities::vis_params uv_vis;

  std::vector<t_real> utemp =
      file.stochread<t_real>("u", N, true);  //< shuffle batch starting position
  uv_vis.u = Eigen::Map<Vector<t_real>>(utemp.data(), utemp.size(), 1);

  // found that a reflection is needed for the orientation
  // of the gridded image to be correct
  std::vector<t_real> vtemp = file.stochread<t_real>("v", N);
  uv_vis.v = -Eigen::Map<Vector<t_real>>(vtemp.data(), vtemp.size(), 1);

  if (w_term) {
    std::vector<t_real> wtemp = file.stochread<t_real>("w", N);
    uv_vis.w = Eigen::Map<Vector<t_real>>(wtemp.data(), wtemp.size(), 1);
  } else {
    uv_vis.w = Vector<t_real>::Zero(utemp.size());
  }

  std::vector<t_real> retemp = file.stochread<t_real>("re", N);
  std::vector<t_real> imtemp = file.stochread<t_real>("im", N);
  std::vector<t_real> sigma = file.stochread<t_real>("sigma", N);

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


/// @brief Write an HDF5 file with u,v visibilities from a vis_params object.
void write_visibility(const utilities::vis_params& uv_vis, const std::string& h5name,
                      const bool w_term, cons int chunksize = -1) {
  // Set up HDF5 file
  HighFive::File h5file(h5name, HighFive::File::OpenOrCreate | HighFive::File::Truncate);
  // Set up file properies, such as chunking and compression
  // Note: the I/O is minimised if compression is disabeld (obvs)
  // If using decompressed data is not an option, then the
  // I/O performance can be optiomised by chunking the data
  // in such a way that each MPI rank only has to decompressi
  // its allocated segment (or a subset thereof)
  HighFive::DataSetCreateProps props;
  if (uv_vis.u.size()) {
    if (chunkize > 0) {
      props.add(HighFive::Chunking(std::vector<hsize_t>{(size_t)chunksize}));
    }
    else {
      props.add(HighFive::Chunking(std::vector<hsize_t>{uv_vis.u.size()}));
    }
    props.add(HighFive::Deflate(9)); // maximal compression
  }
  // Create the H5 datasets
  h5file.createDataSet("u", std::vector<t_real>(uv_vis.u.data(), uv_vis.u.data() + uv_vis.u.size()), props);
  h5file.createDataSet("v", std::vector<t_real>(uv_vis.v.data(), uv_vis.v.data() + uv_vis.v.size()), props);
  if (w_term) {
    h5file.createDataSet("w", std::vector<t_real>(uv_vis.w.data(), uv_vis.w.data() + uv_vis.w.size()), props);
  }

  vector<t_real> redata, imdata, sigma;
  redata.reserve(uv_vis.vis.size());
  imdata.reserve(uv_vis.vis.size());
  sigma.reserve(uv_vis.weights.size());
  for (size_t i = 0; i < uv_vis.vis.size(); ++i) {
    redata.push_back(uv_vis.vis(i).real());
    imdata.push_back(uv_vis.vis(i).imag());
    sigma.push_back(1 / uv_vis.weights(i));
  }
  h5file.createDataSet("re", std::move(redata), props);
  h5file.createDataSet("im", std::move(imdata), props);
  h5file.createDataSet("sigma", std::move(imdata), props);
}


}  // namespace purify::H5

#endif
