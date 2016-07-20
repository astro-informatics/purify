#ifndef PURIFY_CASACORE_H
#define PURIFY_CASACORE_H

#include <map>
#include <string>
#include <type_traits>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/TaQL/TableParse.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/Table.h>
#include <sopt/utilities.h>
#include "types.h"

namespace purify {
namespace casa {

//! Interface around measurement sets
class MeasurementSet {
public:
  //! Default filter specifying which data to accept
  static std::string const default_filter;

  //! Constructs the interface around a given measurement set
  MeasurementSet(std::string const filename)
      : filename_(filename), tables_(new std::map<std::string, ::casacore::Table>){};
  //! Filename of the measurement set
  std::string const &filename() const { return filename_; }
  //! Set new filename
  MeasurementSet &filename(std::string const &filename);

  //! Frequencies for each data desc id (rows) and channel (columns)
  Image<t_real> frequency_table() const;
  //! Frequencies where rows match `uvw` and friends, and cols are channels.
  Image<t_real> frequencies(std::string const &filter = default_filter) const;

  //! \brief Gets table or subtable
  ::casacore::Table const &table(std::string const &name = "") const;

  //! Gets scalar column from table
  template <class T>
  ::casacore::ScalarColumn<T>
  scalar_column(std::string const &col, std::string const &tabname = "") const {
    return get_column<T, ::casacore::ScalarColumn>(col, tabname);
  }
  //! \brief Gets array column from table
  template <class T>
  ::casacore::ArrayColumn<T>
  array_column(std::string const &col, std::string const &tabname = "") const {
    return get_column<T, ::casacore::ArrayColumn>(col, tabname);
  }

  //! Flags per measurement (rows) and per channel (cols)
  Image<bool> flagged(std::string const &filter) const;

  //! \brief Input name if given, otherwise DATA or DATA_CORRECTED
  //! It is an error to give a column name if a column with a name does not exist
  std::string data_column_name(std::string const &name = "") const;

  //! Clear memory
  void clear() { tables_->clear(); }

  //! U, V, W in meters
  Matrix<t_real> uvw(std::string const &filter = default_filter) const;
  //! data desc ids
  Array<t_int> data_desc_id(std::string const &filter = default_filter) const;
  //! Stokes I component
  Matrix<t_complex>
  I(std::string const &name = "DATA", std::string const &filter = default_filter) const;
  //! Weights for the Stokes I component
  Matrix<t_real>
  wI(std::string const &name = "SIGMA", std::string const &filter = default_filter) const;
  //! Stokes Q component
  Matrix<t_complex>
  Q(std::string const &name = "DATA", std::string const &filter = default_filter) const;
  //! Weights for the Stokes Q component
  Matrix<t_real>
  wQ(std::string const &name = "SIGMA", std::string const &filter = default_filter) const;
  //! Stokes U component
  Matrix<t_complex>
  U(std::string const &name = "DATA", std::string const &filter = default_filter) const;
  //! Weights for the Stokes U component
  Matrix<t_real>
  wU(std::string const &name = "SIGMA", std::string const &filter = default_filter) const;
  //! Stokes V component
  Matrix<t_complex>
  V(std::string const &name = "DATA", std::string const &filter = default_filter) const;
  //! Weights for the Stokes V component
  Matrix<t_real>
  wV(std::string const &name = "SIGMA", std::string const &filter = default_filter) const;

  //! Gets a column using TaQL
  template <class T>
  Matrix<T> column(std::string const &column, std::string const &filter = default_filter) const {
    return this->column<T>(column, filter,
                           std::integral_constant<bool, sopt::is_complex<T>::value>());
  }

private:
  template <class T>
  Matrix<T> column(std::string const &column, std::string const &filter,
                   std::integral_constant<bool, true> const &) const;
  template <class T>
  Matrix<T> column(std::string const &column, std::string const &filter,
                   std::integral_constant<bool, false> const &) const;
  //! Gets stokes of given array/object
  template <class T, template <class> class TYPE>
  TYPE<T> get_column(std::string const &col, std::string const &tabname) const {
    return {table(tabname), col};
  }

  //! Name associated with the measurement set
  std::string filename_;

  //! Holds tables that have been opened
  std::unique_ptr<std::map<std::string, ::casacore::Table>> tables_;
};

namespace details {
template <class C> Matrix<C> get_taql_array(::casacore::TaQLResult const &taql) {
  auto const col = ::casacore::ArrayColumn<C>(taql, "R");
  auto const data_column = col.getColumn();
  auto const shape = col.shape(0);
  auto nsize
      = std::accumulate(shape.begin(), shape.end(), 1, [](ssize_t a, ssize_t b) { return a * b; });
  typedef Eigen::Matrix<C, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Matrix;
  return Matrix::Map(data_column.data(), col.nrow(), nsize);
}
template <class C> Matrix<C> get_taql_scalar(::casacore::TaQLResult const &taql) {
  auto const col = ::casacore::ScalarColumn<C>(taql, "R");
  auto const data_column = col.getColumn();
  typedef Eigen::Matrix<C, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Matrix;
  return Matrix::Map(data_column.data(), col.nrow(), 1);
}
}

template <class T>
Matrix<T> MeasurementSet::column(std::string const &column, std::string const &filter,
                                 std::integral_constant<bool, true> const &) const {
  if(table().nrow() == 0)
    return Matrix<T>::Zero(0, 0);
  auto const taql_table
      = ::casacore::tableCommand("SELECT " + column + " as R FROM $1 " + filter, table());
  auto const vtable = taql_table.table();

  switch(vtable.tableDesc().columnDesc("R").trueDataType()) {
#define PURIFY_MACRO(NAME, TYPE)                                                                   \
  case ::casacore::Tp##NAME:                                                                       \
    return details::get_taql_scalar<::casacore::TYPE>(vtable).template cast<T>();
    PURIFY_MACRO(Complex, Complex);
    PURIFY_MACRO(DComplex, DComplex);
#undef PURIFY_MACRO
#define PURIFY_MACRO(NAME, TYPE)                                                                   \
  case ::casacore::TpArray##NAME:                                                                  \
    return details::get_taql_array<::casacore::TYPE>(vtable).template cast<T>();
    PURIFY_MACRO(Complex, Complex);
    PURIFY_MACRO(DComplex, DComplex);
#undef PURIFY_MACRO
  default:
    break;
  }

  throw std::runtime_error("Array type is not handled");
  return Matrix<T>::Zero(0, 0);
}

template <class T>
Matrix<T> MeasurementSet::column(std::string const &column, std::string const &filter,
                                 std::integral_constant<bool, false> const &) const {
  if(table().nrow() == 0)
    return Matrix<T>::Zero(0, 0);
  auto const taql_table
      = ::casacore::tableCommand("SELECT " + column + " as R FROM $1 " + filter, table());
  auto const vtable = taql_table.table();

  switch(vtable.tableDesc().columnDesc("R").trueDataType()) {
#define PURIFY_MACRO(NAME, TYPE)                                                                   \
  case ::casacore::Tp##NAME:                                                                       \
    return details::get_taql_scalar<::casacore::TYPE>(vtable).template cast<T>();
    PURIFY_MACRO(Bool, Bool);
    PURIFY_MACRO(Char, Char);
    PURIFY_MACRO(UChar, uChar);
    PURIFY_MACRO(Short, Short);
    PURIFY_MACRO(UShort, uShort);
    PURIFY_MACRO(Int, Int);
    PURIFY_MACRO(UInt, uInt);
    PURIFY_MACRO(Float, Float);
    PURIFY_MACRO(Double, Double);
#undef PURIFY_MACRO
#define PURIFY_MACRO(NAME, TYPE)                                                                   \
  case ::casacore::TpArray##NAME:                                                                  \
    return details::get_taql_array<::casacore::TYPE>(vtable).template cast<T>();
    PURIFY_MACRO(Bool, Bool);
    PURIFY_MACRO(Char, Char);
    PURIFY_MACRO(UChar, uChar);
    PURIFY_MACRO(Short, Short);
    PURIFY_MACRO(UShort, uShort);
    PURIFY_MACRO(Int, Int);
    PURIFY_MACRO(UInt, uInt);
    PURIFY_MACRO(Float, Float);
    PURIFY_MACRO(Double, Double);
#undef PURIFY_MACRO
  default:
    break;
  }

  throw std::runtime_error("Array type is not handled");
  return Matrix<T>::Zero(0, 0);
}
}
}

#endif
