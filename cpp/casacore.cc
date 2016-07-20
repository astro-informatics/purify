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
}
}
