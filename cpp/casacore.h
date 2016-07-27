#ifndef PURIFY_CASACORE_H
#define PURIFY_CASACORE_H

#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/TaQL/TableParse.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/Table.h>
#include <sopt/utilities.h>
#include "utilities.h"
#include "types.h"

namespace purify {
namespace casa {

template <class T>
Matrix<T> table_column(::casacore::Table const &table, std::string const &column,
                       std::string const &filter = "");

//! Interface around measurement sets
class MeasurementSet {
public:
  //! Iterates over channels
  class const_iterator;
  class ChannelWrapper;
  //! Default filter specifying which data to accept
  static std::string const default_filter;

  //! Constructs the interface around a given measurement set
  MeasurementSet(std::string const filename)
      : filename_(filename),
        tables_(std::make_shared<std::map<std::string, ::casacore::Table>>()){};
  //! Shallow measurement set copy
  MeasurementSet(MeasurementSet const &c) : filename_(c.filename_), tables_(c.tables_){};
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
  void clear() { tables_ = std::make_shared<std::map<std::string, ::casacore::Table>>(); }

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
    return table_column<T>(table(), column, filter);
  }

  //! Number of channels in the measurement set
  std::size_t size() const;

  //! Iterates over channels
  const_iterator begin(std::string const &filter = "") const;
  //! Iterates over channels
  const_iterator end(std::string const &filter = "") const;
  //! Returns wrapper over specific channel
  ChannelWrapper operator[](t_uint i) const;
  //! Returns wrapper over specific channel
  ChannelWrapper operator[](std::tuple<t_uint, std::string> const &i) const;

private:
  //! Gets stokes of given array/object
  template <class T, template <class> class TYPE>
  TYPE<T> get_column(std::string const &col, std::string const &tabname) const {
    return {table(tabname), col};
  }

  //! Name associated with the measurement set
  std::string filename_;

  //! Holds tables that have been opened
  std::shared_ptr<std::map<std::string, ::casacore::Table>> tables_;
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

template <class T>
Matrix<T> table_column(::casacore::Table const &table, std::string const &column,
                       std::string const &filter, std::integral_constant<bool, true> const &) {
  if(table.nrow() == 0)
    return Matrix<T>::Zero(0, 1);
  auto const taql_table = ::casacore::tableCommand(
      "USING STYLE PYTHON SELECT " + column + " as R FROM $1 " + filter, table);
  auto const vtable = taql_table.table();
  if(vtable.nrow() == 0)
    return Matrix<T>(0, 1);

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
  return Matrix<T>::Zero(0, 1);
}

template <class T>
Matrix<T> table_column(::casacore::Table const &table, std::string const &column,
                       std::string const &filter, std::integral_constant<bool, false> const &) {
  if(table.nrow() == 0)
    return Matrix<T>::Zero(0, 1);
  auto const taql_table = ::casacore::tableCommand(
      "USING STYLE PYTHON SELECT " + column + " as R FROM $1 " + filter, table);
  auto const vtable = taql_table.table();
  if(vtable.nrow() == 0)
    return Matrix<T>(0, 1);

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
  return Matrix<T>::Zero(0, 1);
}
}

template <class T>
Matrix<T>
table_column(::casacore::Table const &table, std::string const &column, std::string const &filter) {
  return details::table_column<T>(table, column, filter,
                                  std::integral_constant<bool, sopt::is_complex<T>::value>());
}

class MeasurementSet::ChannelWrapper {
public:
  //! Possible locations for SIGMA
  enum class Sigma { OVERALL, SPECTRUM };
  enum class Stokes { I, Q, U, V };
  ChannelWrapper(t_uint channel, MeasurementSet const &ms, std::string const &filter = "")
      : ms_(ms), filter_(filter), channel_(channel) {}

  //! Channel this object is associated with
  t_uint channel() const { return channel_; }
#define PURIFY_MACRO(NAME, INDEX)                                                                  \
  /** Stokes component in meters **/                                                               \
  Vector<t_real> raw_##NAME() const {                                                              \
    return table_column<t_real>(ms_.table(), "UVW[" #INDEX "]", filter());                         \
  }                                                                                                \
  /** \brief U scaled to purify values of wavelengths **/                                                        \
  Vector<t_real> lambda_##NAME() const {                                          \
    return (raw_##NAME().array() * frequencies().array()).matrix() / constant::c;                   \
  }
  PURIFY_MACRO(u, 0);
  PURIFY_MACRO(v, 1);
  PURIFY_MACRO(w, 2);
# undef PURIFY_MACRO
  //! Number of rows in a channel
  t_uint size() const;

  //! Frequencies for each DATA_DESC_ID
  Vector<t_real> raw_frequencies() const;

  Vector<t_real> data_desc_id() const {
    return table_column<t_real>(ms_.table(), "DATA_DESC_ID", filter());
  }

#define PURIFY_MACRO(NAME)                                                                         \
  /** Stokes component */                                                                          \
  Vector<t_complex> NAME(std::string const &col = "DATA") const {                                  \
    return table_column<t_complex>(ms_.table(), stokes(#NAME, index(col)), filter());              \
  }                                                                                                \
  /** Standard deviation for the Stokes component */                                               \
  Vector<t_real> w##NAME(Sigma const &col = Sigma::OVERALL) const {                                \
    return table_column<t_real>(                                                                   \
        ms_.table(),                                                                               \
        "1.0/" + stokes(#NAME, col == Sigma::OVERALL ? "SIGMA" : index("SIGMA_SPECTRUM")),         \
        filter());                                                                                 \
  }
  PURIFY_MACRO(I);
  PURIFY_MACRO(Q);
  PURIFY_MACRO(U);
  PURIFY_MACRO(V);
#undef PURIFY_MACRO

  //! Frequencies for each valid measurement
  Vector<t_real> frequencies() const;

  //! Check if channel has any data
  bool is_valid() const;

protected:
  //! Composes filter for current channel
  std::string filter() const;
  //! Composes "<variable>[<channel>,]"
  std::string index(std::string const &variable = "") const;
  //! Computes given stokes polarization
  std::string stokes(std::string const &pol, std::string const &column = "DATA") const;

  //! Table over which to operate
  MeasurementSet ms_;
  //! Filter to apply to searches
  std::string const filter_;
  //! Channel index
  t_uint channel_;
};

class MeasurementSet::const_iterator {
public:
  //! Convenience wrapper to access values associated with a single channel
  typedef MeasurementSet::ChannelWrapper value_type;
  typedef std::shared_ptr<value_type const> pointer;
  typedef value_type const &reference;

  const_iterator(t_int channel, MeasurementSet const &ms, std::string const &filter = "")
      : channel(channel), ms(ms), filter(filter), wrapper(new value_type(channel, ms, filter)){};

  pointer operator->() const { return wrapper; }
  reference operator*() const { return *wrapper; }
  const_iterator &operator++();
  const_iterator operator++(int);
  bool operator==(const_iterator const &c) const;
  bool operator!=(const_iterator const &c) const { return not operator==(c); }

protected:
  t_int channel;
  MeasurementSet ms;
  std::string const filter;
  std::shared_ptr<value_type> wrapper;
};
//! Read measurement set into vis_params structure
utilities::vis_params read_measurementset(std::string const &filename, const std::vector<t_int> & channels = std::vector<t_int>(), 
  const MeasurementSet::ChannelWrapper::Stokes stokes = MeasurementSet::ChannelWrapper::Stokes::I, std::string const &filter = "");
}
//! Return average frequency over channels
t_real average_frequency(const purify::casa::MeasurementSet & ms_file, std::string const &filter, const std::vector<t_int> & channels);
}

#endif
