#include <sstream>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include "casacore.h"

namespace purify {
namespace casa {

MeasurementSet &MeasurementSet::filename(std::string const &filename) {
  clear();
  filename_ = filename;
  return *this;
}

MeasurementSet &MeasurementSet::data_desc_id(int d) {
  data_desc_id_ = d;
  return *this;
}

int MeasurementSet::spectral_window_id() const {
  auto const column = scalar_column<::casacore::Int>("SPECTRAL_WINDOW_ID", "DATA_DESCRIPTION");
  column.checkRowNumber(data_desc_id());
  return column(data_desc_id());
}

Array<t_real> MeasurementSet::frequencies() const {
  auto const column = array_column<::casacore::Double>("CHAN_FREQ", "SPECTRAL_WINDOW");
  auto const array = column.get(spectral_window_id());
  if(array.ndim() != 1)
    throw std::runtime_error("CHAN_FREQ is not a 1d array");
  Array<t_real> result(array.shape()[0]);
  for(int i(0); i < result.size(); ++i)
    result(i) = static_cast<t_real>(array(::casacore::IPosition(1, i)));
  return result;
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
  auto const tabname = name == "" ? filename(): filename() + "/" + name;
  auto i_result = tables_->find(tabname);
  // Main table: select only data corresponding to data_desc_id
  if(i_result == tables_->end())
    i_result = tables_->emplace(tabname, ::casacore::Table(tabname)).first;
  return i_result->second;
}

::casacore::Table const &MeasurementSet::table(std::string const &name, int data_desc_id) const {
  if(name != "")
    return table(name);

  // Main table: select only data corresponding to data_desc_id
  std::ostringstream sstr;
  sstr << name << "[" << data_desc_id << "]";
  auto const tabname = sstr.str();

  auto i_result = tables_->find(tabname);
  if(i_result == tables_->end()) {
    auto const & main = table(name);
    // Select data desc id
    auto const expr = main.col("DATA_DESC_ID") == data_desc_id;
    i_result = tables_->emplace(tabname, ::casacore::Table(main(expr))).first;
  }
  return i_result->second;
}

#define PURIFY_MACRO(NAME, INDEX)                                                                  \
  Vector<t_real> MeasurementSet::NAME() const {                                                    \
    auto const column = array_column<::casacore::Double>("UVW");                                   \
    auto const data_column = column.getColumn();                                                   \
    return Matrix<::casacore::Double>::Map(data_column.data(), 3, column.nrow())                   \
        .row(INDEX)                                                                                \
        .cast<t_real>();                                                                           \
  }
PURIFY_MACRO(u, 0);
PURIFY_MACRO(v, 1);
PURIFY_MACRO(w, 2);
#undef PURIFY_MACRO

// Tensor<t_real, 3> MeasurementSet::stddev(int Nc, int Nf, ) const {
//   auto const has_sigma = table().tableDesc().isColumn("SIGMA");
//   auto const has_sigmas = table().tableDesc().isColumn("SIGMA SPECTRUM");
//   if(not (has_sima or has_sigmas))
//     throw std:runtime_error("Neither SIGMA nor SIGMA SPECTRUM are defined");
//   using namespace ::casacore;
//   auto const column = scalar_column<Double>("SIGMA SPECTRUM");
//   auto const data_column = column.getColumn();
//   return 1e0 / Tensor<Double, 3>::Map(data_column.data(), column.nrow()).cast<t_real>().array();
// }
}
}
