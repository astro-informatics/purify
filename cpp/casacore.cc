#include <sstream>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include "casacore.h"
#include "types.h"

namespace purify {
namespace casa {
std::string const MeasurementSet::default_filter = "WHERE NOT ANY(FLAG)";
MeasurementSet &MeasurementSet::filename(std::string const &filename) {
  clear();
  filename_ = filename;
  return *this;
}

::casacore::Table const &MeasurementSet::table(std::string const &name) const {
  auto const tabname = name == "" ? filename() : filename() + "/" + name;
  auto i_result = tables_->find(tabname);
  if(i_result == tables_->end())
    i_result = tables_->emplace(tabname, ::casacore::Table(tabname)).first;

  return i_result->second;
}

std::size_t MeasurementSet::size() const {
  if(table().nrow() == 0)
    return 0;
  auto const column = array_column<::casacore::Double>("CHAN_FREQ", "SPECTRAL_WINDOW");
  auto const orig = column.shape(0);
  for(t_uint i(1); i < column.nrow(); ++i)
    if(orig != column.shape(i))
      throw std::runtime_error("Can only do rectangular measurement set for now");
  return orig(0);
}

MeasurementSet::const_iterator MeasurementSet::begin(std::string const &filter) const {
  return const_iterator(0, *this, filter);
}
MeasurementSet::const_iterator MeasurementSet::end(std::string const &filter) const {
  return const_iterator(size(), *this, filter);
}
MeasurementSet::ChannelWrapper MeasurementSet::operator[](t_uint i) const {
  return ChannelWrapper(i, *this, "");
}

MeasurementSet::ChannelWrapper MeasurementSet::
operator[](std::tuple<t_uint, std::string> const &i) const {
  if(std::get<0>(i) >= size())
    throw std::out_of_range("Not that many channels");
  return ChannelWrapper(std::get<0>(i), *this, std::get<1>(i));
}

std::string MeasurementSet::ChannelWrapper::filter() const {
  std::ostringstream sstr;
  sstr << "WHERE NOT any(FLAG[" << channel_ << ",])";
  if(not filter_.empty())
    sstr << "AND " << filter_;
  return sstr.str();
}
std::string MeasurementSet::ChannelWrapper::index(std::string const &variable) const {
  std::ostringstream sstr;
  sstr << variable << "[" << channel_ << ",]";
  return sstr.str();
}

Vector<t_real> MeasurementSet::ChannelWrapper::frequencies() const {
  auto const frequencies = raw_frequencies();
  auto const ids = table_column<::casacore::Int>(ms_.table(), "DATA_DESC_ID", filter());
  auto const spids
      = table_column<::casacore::Int>(ms_.table("DATA_DESCRIPTION"), "SPECTRAL_WINDOW_ID");
  Vector<t_real> result(ids.size());
  for(Eigen::DenseIndex i(0); i < ids.size(); ++i) {
    assert(ids(i) < spds.size());
    assert(spids(ids(i)) < frequencies.size());
    result(i) = frequencies(spids(ids(i)));
  }
  return result;
}

bool MeasurementSet::ChannelWrapper::is_valid() const {
  std::ostringstream sstr;
  sstr << "USING STYLE PYTHON SELECT FLAG[" << channel_ << ",] as R FROM $1 WHERE NOT any(FLAG["
       << channel_ << ",])";
  auto const taql_table = ::casacore::tableCommand(sstr.str(), ms_.table());
  return taql_table.table().nrow() > 0;
}

std::string
MeasurementSet::ChannelWrapper::stokes(std::string const &pol, std::string const &column) const {
  std::ostringstream sstr;
  sstr << "mscal.stokes(" << column << ", '" << pol << "')";
  return sstr.str();
}

Vector<t_real> MeasurementSet::ChannelWrapper::raw_frequencies() const {
  std::ostringstream sstr;
  sstr << "CHAN_FREQ[" << channel_ << "]";
  return table_column<t_real>(ms_.table("SPECTRAL_WINDOW"), sstr.str());
}

MeasurementSet::const_iterator &MeasurementSet::const_iterator::operator++() {
  ++channel;
  wrapper = std::make_shared<value_type>(channel, ms, filter);
  return *this;
}

MeasurementSet::const_iterator MeasurementSet::const_iterator::operator++(int) {
  operator++();
  return const_iterator(channel - 1, ms, filter);
}

bool MeasurementSet::const_iterator::operator==(const_iterator const &c) const {
  if(not same_measurement_set(c))
    throw std::runtime_error("Iterators are not over the same measurement set");
  return channel == c.channel;
}

MeasurementSet::const_iterator &MeasurementSet::const_iterator::operator+=(t_int n) {
  channel += n;
  return *this;
}

bool MeasurementSet::const_iterator::operator>(const_iterator const &c) const {
  if(not same_measurement_set(c))
    throw std::runtime_error("Iterators are not over the same measurement set");
  return channel > c.channel;
}

bool MeasurementSet::const_iterator::operator>=(const_iterator const &c) const {
  if(not same_measurement_set(c))
    throw std::runtime_error("Iterators are not over the same measurement set");
  return channel >= c.channel;
}
}
}
