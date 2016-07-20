#include <sstream>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include <casacore/tables/TaQL/TableParse.h>
#include "casacore.h"

namespace purify {
namespace casa {

MeasurementSet &MeasurementSet::filename(std::string const &filename) {
  clear();
  filename_ = filename;
  return *this;
}

// Array<t_real> MeasurementSet::frequencies() const {
//   auto const column = array_column<::casacore::Double>("CHAN_FREQ", "SPECTRAL_WINDOW");
//   auto const array = column.get(spectral_window_id());
//   if(array.ndim() != 1)
//     throw std::runtime_error("CHAN_FREQ is not a 1d array");
//   Array<t_real> result(array.shape()[0]);
//   for(int i(0); i < result.size(); ++i)
//     result(i) = static_cast<t_real>(array(::casacore::IPosition(1, i)));
//   return result;
// }

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

namespace {
// Will cast to double
std::string dtype(::casacore::Float) { return "R8"; }
// No-op
std::string dtype(::casacore::Double) { return ""; }
// Will cast to complex double
std::string dtype(::casacore::Complex) { return "C8"; }
// No-op
std::string dtype(::casacore::DComplex) { return ""; }
}

template <class T> Matrix<T> MeasurementSet::stokes(std::string origin, std::string pol) const {
  // casting won't work if t_complex is not complex<double>
  assert(sizeof(t_complex) == 16 and sizeof(t_real) == 8);
  if(table().nrow() == 0)
    return Matrix<T>::Zero(0, 0);
  auto const taql_table
      = ::casacore::tableCommand("SELECT mscal.stokes(" + origin + ", '" + pol + "') AS result "
                                     + dtype(T(0)) + " " + "FROM $1 WHERE all(SIGMA >  0)",
                                 table());
  auto const vtable = taql_table.table();
  if(not vtable.tableDesc().columnDesc("result").isArray())

    throw std::runtime_error("Expected an array");
  auto const column = ::casacore::ArrayColumn<T>(vtable, "result");
  auto const shape = column.shape(0);
  auto nsize
      = std::accumulate(shape.begin(), shape.end(), 1, [](ssize_t a, ssize_t b) { return a * b; });
  auto const data_column = column.getColumn();
  auto const result = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>::Map(
      data_column.data(), column.nrow(), nsize);
  return result.template cast<T>();
}

#define PURIFY_MACRO(NAME)                                                                         \
  Matrix<t_complex> MeasurementSet::NAME(std::string const &data) const {                          \
    return stokes<t_complex>(data, #NAME);                                                         \
  }                                                                                                \
  Matrix<t_real> MeasurementSet::w##NAME(std::string const &data) const {                          \
    return stokes<t_real>(data, "1.0/" #NAME);                                                     \
  }
PURIFY_MACRO(I);
PURIFY_MACRO(Q);
PURIFY_MACRO(U);
PURIFY_MACRO(V);
#undef PURIFY_MACRO
}
}
