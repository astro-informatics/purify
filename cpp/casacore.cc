#include <sstream>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include "casacore.h"

namespace purify {
namespace casa {
std::string const MeasurementSet::default_filter = "WHERE NOT ANY(FLAG)";

Image<bool> MeasurementSet::flagged(std::string const &filter) const {
  return column<bool>("FLAG", filter).array();
}
MeasurementSet &MeasurementSet::filename(std::string const &filename) {
  clear();
  filename_ = filename;
  return *this;
}

Image<t_real> MeasurementSet::frequency_table() const {
  auto const column = array_column<::casacore::Double>("CHAN_FREQ", "SPECTRAL_WINDOW");
  if(column.nrow() == 0)
    return Image<t_real>::Zero(0, 0);
  if(column.ndim(0) != 1)
    throw std::runtime_error("CHAN_FREQ does not consist of 1d array");
#ifndef _NDEBUG
  for(unsigned int i(1), s(column.ndim(0)), n(column.shape(0)(0)); i < column.nrow(); ++i)
    if(column.ndim(i) != s or column.shape(i)(0) != n)
      throw std::runtime_error("CHAN_FREQ are not homogeneous");
#endif
  auto const data_column = column.getColumn();
  return Eigen::Array<::casacore::Double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>::Map(
             data_column.data(), column.nrow(), column.shape(0)(0))
      .cast<t_real>();
}

Array<t_int> MeasurementSet::data_desc_id(std::string const &filter) const {
  return column<t_int>("DATA_DESC_ID", filter).array();
}

std::string MeasurementSet::data_column_name(std::string const &col) const {
  auto const desc = table().tableDesc();
  if(col != "") {
    if(not desc.isColumn(col))
      throw std::runtime_error("Table does not have a column " + col + ".");
    return col;
  }
  if(desc.isColumn("CORRECTED_DATA"))
    return "CORRECTED_DATA";
  if(not desc.isColumn("DATA"))
    throw std::runtime_error("Could not find column CORRECTED_DATA nor column DATA");
  return "DATA";
}

::casacore::Table const &MeasurementSet::table(std::string const &name) const {
  auto const tabname = name == "" ? filename() : filename() + "/" + name;
  auto i_result = tables_->find(tabname);
  if(i_result == tables_->end())
    i_result = tables_->emplace(tabname, ::casacore::Table(tabname)).first;
  return i_result->second;
}

Matrix<t_real> MeasurementSet::uvw(std::string const &filter) const {
  if(table().nrow() == 0)
    return Matrix<t_real>::Zero(0, 3);
  return column<t_real>("UVW", filter);
}

Image<t_real> MeasurementSet::frequencies(std::string const &filter) const {
  auto desc_id = data_desc_id(filter);
  auto const freqs = frequency_table();
  Image<t_real> result(desc_id.rows(), freqs.cols());
  for(Eigen::DenseIndex i(0); i < result.rows(); ++i)
    result.row(i) = freqs.row(desc_id(i));
  return result;
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

#define PURIFY_MACRO(NAME)                                                                         \
  Matrix<t_complex> MeasurementSet::NAME(std::string const &data, std::string const &filter)       \
      const {                                                                                      \
    auto const col = "mscal.stokes(" + data + ", '" #NAME "')";                                    \
    return column<t_complex>(col, filter);                                                         \
  }                                                                                                \
  Matrix<t_real> MeasurementSet::w##NAME(std::string const &data, std::string const &filter)       \
      const {                                                                                      \
    auto const col = "1.0 / mscal.stokes(" + data + ", '" #NAME "')";                              \
    return column<t_real>(col, filter);                                                            \
  }
PURIFY_MACRO(I);
PURIFY_MACRO(Q);
PURIFY_MACRO(U);
PURIFY_MACRO(V);
#undef PURIFY_MACRO

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
  auto const ids = data_desc_id();
  Vector<t_real> result(ids.size());
  for(Eigen::DenseIndex i(0); i < ids.size(); ++i)
    result(i) = frequencies(ids(i));
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
  if(&ms != &c.ms)
    throw std::runtime_error("Iterators are not over the same measurement set");
  return channel == c.channel;
}
}
}
