#ifndef PURIFY_H5READER_H
#define PURIFY_H5READER_H
#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"
//#include "purify/uvw_utilities.h"

#include "highfive/H5File.hpp"

#include <string>
#include <vector>

namespace purify {

/// @brief Purify interface class to handle HDF5 input files
class H5Handler {
  public:

  H5Handler() = delete;

  H5Handler(const std::string& filename)
       : _file(filename) { }

  template <typename T = double>
  std::vector<T> read(const std::string& label) {
    auto dataset = _file.getDataSet(label);
    return dataset.read<std::vector<T>>();
  }

  private:

    /// HDF5 file
    const HighFive::File _file;

};

}  // namespace purify

#endif
