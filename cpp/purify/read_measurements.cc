#include "purify/read_measurements.h"

#include "purify/uvfits.h"

#ifdef PURIFY_CASACORE
#include "purify/casacore.h"
#endif
#include <iostream>
#include <boost/filesystem.hpp>

#ifdef PURIFY_MPI
#include "purify/mpi_utilities.h"
#endif
namespace purify {
namespace read_measurements {
utilities::vis_params read_measurements(const std::string &name, const bool w_term,
                                        const stokes pol, const utilities::vis_units units) {
  return read_measurements(std::vector<std::string>{name}, w_term, pol, units);
}
utilities::vis_params read_measurements(const std::vector<std::string> &names, const bool w_term,
                                        const stokes pol, const utilities::vis_units units) {
  std::vector<std::string> found_files;
  format format_type = format::uvfits;
  for (t_int i = 0; i < names.size(); i++) {
    const boost::filesystem::path file_path(names.at(i));
    if (not boost::filesystem::exists(file_path)) {
      PURIFY_HIGH_LOG("Missing file will be removed from list: {}", names.at(i));
    } else {
      found_files.emplace_back(names.at(i));
      format format_check = format::uvfits;
      // checking if reading measurement set or .vis file
      std::size_t found = names.at(i).find_last_of(".");
      std::string format = "." + names.at(i).substr(found + 1);
      std::transform(format.begin(), format.end(), format.begin(), ::tolower);
      if (format == ".ms") {
        format_check = format::ms;
        if (not boost::filesystem::is_directory(file_path))
          throw std::runtime_error(names.at(i) +
                                   " is not a directory, as expected for a measurement set.");
      } else if (format == ".vis") {
        format_check = format::vis;
        if (not boost::filesystem::is_regular_file(file_path))
          throw std::runtime_error(names.at(i) + " is not a regular file.");
      } else if (format == ".uvfits") {
        format_check = format::uvfits;
        if (not boost::filesystem::is_regular_file(file_path))
          throw std::runtime_error(names.at(i) + " is not a regular file.");
      } else
        throw std::runtime_error("File extention for " + names.at(i) +
                                 " not recognised. Must be .vis, .uvfits, or .ms.");
      if (i == 0) format_type = format_check;
      if (i > 0 and (format_check != format_type))
        throw std::runtime_error("File extention is not the same for " + names.at(i) + " " +
                                 names.at(i - 1));
    }
  }

  if (found_files.size() == 0) throw std::runtime_error("No files found, all files are missing!");
  switch (format_type) {
  case (format::vis): {
    if (pol != stokes::I)
      throw std::runtime_error(
          ".vis files are ascii, so it is assumed that it is Stokes I. But, you are trying to "
          "choose a different type!");
    auto measurements = utilities::read_visibility(found_files, w_term);
    measurements.units = units;
    return measurements;
    break;
  }
  case (format::uvfits): {
    return pfitsio::read_uvfits(found_files, true, pol);
    break;
  }
  case (format::ms): {
#ifdef PURIFY_CASACORE
    return casa::read_measurementset(found_files, pol);
    break;
#else
    throw std::runtime_error(
        "You want to read a measurement set, but you did not compile with casacore.");
#endif
  }
  default:
    throw std::runtime_error("Format not recongised.");
  }
}

#ifdef PURIFY_MPI
utilities::vis_params read_measurements(const std::string &name,
                                        sopt::mpi::Communicator const &comm,
                                        const distribute::plan plan, const bool w_term,
                                        const stokes pol, const utilities::vis_units units) {
  return read_measurements(std::vector<std::string>{name}, comm, plan, w_term, pol, units);
}
utilities::vis_params read_measurements(const std::vector<std::string> &names,
                                        sopt::mpi::Communicator const &comm,
                                        const distribute::plan plan, const bool w_term,
                                        const stokes pol, const utilities::vis_units units) {
  if (comm.size() == 1) {
    try {
      return read_measurements(names, w_term, pol, units);
    } catch (const std::runtime_error &e) {
      comm.abort(e.what());
    }
  }
  if (comm.is_root()) {
    utilities::vis_params result;
    try {
      result = read_measurements(names, w_term, pol, units);
    } catch (const std::runtime_error &e) {
      comm.abort(e.what());
    }
    auto const order = distribute::distribute_measurements(result, comm, plan);
    return utilities::regroup_and_scatter(result, order, comm);
  }
  auto result = utilities::scatter_visibilities(comm);
  return result;
}
#endif

}  // namespace read_measurements
}  // namespace purify
