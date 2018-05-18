#include "purify/read_measurements.h"

#include "purify/uvfits.h"
#include "purify/mpi_utilities.h"

#ifdef PURIFY_CASACORE
#include "purify/casacore.h"
#endif

namespace purify {
  namespace read_measurements {
utilities::vis_params read_measurements(const std::vector<std::string> &names) {
  format format_type = format::uvfits;
  for(t_int i = 0; i < names.size(); i++){
  format format_check = format::uvfits;
  // checking if reading measurement set or .vis file
  std::size_t found = names.at(i).find_last_of(".");
  std::string format = "." + names.at(i).substr(found + 1);
  std::transform(format.begin(), format.end(), format.begin(), ::tolower);
  if (format == ".ms")
    format_check = format::ms;
  else if (format == ".vis")
    format_check = format::vis;
  else if (format == ".uvfits")
    format_check = format::uvfits;
  else
    throw std::runtime_error("File extention for "+names.at(i) + " not recognised. Must be .vis, .uvfits, or .ms.");
  if(i == 0)
    format_type = format_check;
  if(i > 0 and (format_check != format_type))
    throw std::runtime_error("File extention is not the same for " + names.at(i) + " " + names.at(i - 1));
  }
  switch(format_type){
    case(format::vis):
      return utilities::read_visibility(names);
      break;
    case(format::uvfits):
      return pfitsio::read_uvfits(names, true, pfitsio::stokes::I);
      break;
    case(format::ms):
#ifdef PURIFY_CASACORE
      return casa::read_measurementset(names, MeasurementSet::ChannelWrapper::polarization::I);
      break;
#else
      throw std::runtime_error("You want to read a measurement set, but you did not compile with casacore.");
#endif
    default:
      throw std::runtime_error("Format not recongised.");
  }
}

utilities::vis_params
read_measurements(const std::vector<std::string> &names, sopt::mpi::Communicator const &comm) {
  if(comm.size() == 1)
    return read_measurements(names);
  if(comm.is_root()) {
    auto result = read_measurements(names);
    auto const order = distribute::distribute_measurements(result, comm, distribute::plan::w_term);
    return utilities::regroup_and_scatter(result, order, comm);
  }
  auto result = utilities::scatter_visibilities(comm);
  return result;
}

}
}
