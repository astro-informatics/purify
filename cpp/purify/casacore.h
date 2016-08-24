#ifndef PURIFY_CASACORE_H
#define PURIFY_CASACORE_H

#include "purify/config.h"
#include <exception>
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
#include "purify/types.h"
#include "purify/utilities.h"

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
  //! Type for (RA, DEC) direction
  typedef Eigen::Array<t_real, 2, 1> Direction;

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
  //! Clear memory
  void clear() { tables_ = std::make_shared<std::map<std::string, ::casacore::Table>>(); }

  //! Data from a column
  template <class T>
  Matrix<T> column(std::string const &column, std::string const &filter = "") const {
    return table_column<T>(this->table(), column, filter);
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
  //! Direction (RA, DEC) in radians
  Direction direction(t_real tolerance = 1e-8, std::string const &filter = "") const;
  //! Right ascention in radians
  Direction::Scalar right_ascension(t_real tolerance = 1e-8, std::string const &filter = "") const {
    return direction(tolerance, filter)(0);
  }
  //! Declination in radians
  Direction::Scalar declination(t_real tolerance = 1e-8, std::string const &filter = "") const {
    return direction(tolerance, filter)(1);
  }

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
  enum class polarization { I, Q, U, V, LL, RR, RL, LR, XX, YY, XY, YX };
  ChannelWrapper(t_uint channel, MeasurementSet const &ms, std::string const &filter = "")
      : ms_(ms), filter_(filter), channel_(channel) {}

  //! Channel this object is associated with
  t_uint channel() const { return channel_; }
#define PURIFY_MACRO(NAME, INDEX)                                                                  \
  /** Stokes component in meters **/                                                               \
  Vector<t_real> raw_##NAME() const {                                                              \
    return table_column<t_real>(ms_.table(), "UVW[" #INDEX "]", filter());                         \
  }                                                                                                \
  /** \brief U scaled to purify values of wavelengths **/                                          \
  Vector<t_real> lambda_##NAME() const {                                                           \
    return (raw_##NAME().array() * frequencies().array()).matrix() / constant::c;                  \
  }
  PURIFY_MACRO(u, 0);
  PURIFY_MACRO(v, 1);
  PURIFY_MACRO(w, 2);
#undef PURIFY_MACRO
  //! Number of rows in a channel
  t_uint size() const;

  //! FIELD_ID from table MAIN
  Vector<t_int> field_ids() const { return ms_.column<t_int>("FIELD_ID", filter()); }
  //! DATA_DESC_ID from table MAIN
  Vector<t_int> data_desc_id() const { return ms_.column<t_int>("DATA_DESC_ID", filter()); }

  //! Direction (RA, DEC) in radian
  Direction direction(t_real tolerance = 1e-8) const { return ms_.direction(tolerance, filter()); }
  Direction::Scalar right_ascension(t_real tolerance = 1e-8) const {
    return ms_.right_ascension(tolerance, filter());
  }
  Direction::Scalar declination(t_real tolerance = 1e-8) const {
    return ms_.declination(tolerance, filter());
  }

  //! Frequencies from SPECTRAL_WINDOW for a this channel
  Vector<t_real> raw_frequencies() const { return raw_spectral_window("CHAN_FREQ"); }

#define PURIFY_MACRO(NAME)                                                                         \
  /** Stokes component */                                                                          \
  Vector<t_complex> NAME(std::string const &col = "DATA") const {                                  \
    return ms_.column<t_complex>(stokes(#NAME, index(col)), filter());                             \
  }                                                                                                \
  /** Standard deviation for the Stokes component */                                               \
  Vector<t_real> w##NAME(Sigma const &col = Sigma::OVERALL) const {                                \
    return table_column<t_real>(                                                                   \
        ms_.table(), stokes(#NAME, col == Sigma::OVERALL ? "SIGMA" : index("SIGMA_SPECTRUM")),     \
        filter());                                                                                 \
  }
  // Stokes parameters
  PURIFY_MACRO(I);
  PURIFY_MACRO(Q);
  PURIFY_MACRO(U);
  PURIFY_MACRO(V);
  // Circular correlations
  PURIFY_MACRO(LL);
  PURIFY_MACRO(RR);
  PURIFY_MACRO(LR);
  PURIFY_MACRO(RL);
  // Linear correlations
  PURIFY_MACRO(XX);
  PURIFY_MACRO(YY);
  PURIFY_MACRO(XY);
  PURIFY_MACRO(YX);

#undef PURIFY_MACRO

  //! Frequencies for each valid measurement
  Vector<t_real> frequencies() const { return joined_spectral_window("CHAN_FREQ"); }
  //! Channel width for each valid measurement
  Vector<t_real> width() const { return joined_spectral_window("CHAN_WIDTH"); }
  //! Effective noise band-width width for each valid measurement
  Vector<t_real> effective_noise_bandwidth() const {
    return joined_spectral_window("EFFECTIVE_BW");
  }
  //! Effective spectral resolution for each valid measurement
  Vector<t_real> resolution() const { return joined_spectral_window("RESOLUTION"); }

  //! Check if channel has any data
  bool is_valid() const;

protected:
  //! Composes filter for current channel
  std::string filter() const;
  //! Composes "<variable>[<channel>,]"
  std::string index(std::string const &variable = "") const;
  //! Computes given stokes polarization
  std::string stokes(std::string const &pol, std::string const &column = "DATA") const;

  //! column[<channel>] from SPECTRAL_WINDOW
  Vector<t_real> raw_spectral_window(std::string const &column) const;
  //! column[<channel>] from SPECTRAL_WINDOW joined to each measurement
  Vector<t_real> joined_spectral_window(std::string const &column) const;

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
  typedef t_int difference_type;

  const_iterator(t_int channel, MeasurementSet const &ms, std::string const &filter = "")
      : channel_(channel), ms_(ms), filter_(filter),
        wrapper_(new value_type(channel_, ms_, filter_)){};

  pointer operator->() const { return wrapper_; }
  reference operator*() const { return *wrapper_; }
  const_iterator &operator++();
  const_iterator operator++(int);
  const_iterator &operator+=(difference_type n);
  const_iterator &operator-=(difference_type n) { return operator+=(-n); }
  const_iterator operator+(difference_type n) const {
    return const_iterator(channel_ + n, ms_, filter_);
  }
  const_iterator operator-(difference_type n) const {
    return const_iterator(channel_ - n, ms_, filter_);
  }
  bool operator>(const_iterator const &c) const;
  bool operator>=(const_iterator const &c) const;
  bool operator<(const_iterator const &c) const { return not operator>=(c); }
  bool operator<=(const_iterator const &c) const { return not operator>(c); }
  bool operator==(const_iterator const &c) const;
  bool operator!=(const_iterator const &c) const { return not operator==(c); }

  //! True if iterating over the same measurement set
  bool same_measurement_set(const_iterator const &c) const {
    return &ms_.table() == &c.ms_.table();
  }

protected:
  difference_type channel_;
  MeasurementSet ms_;
  std::string const filter_;
  std::shared_ptr<value_type> wrapper_;
};
//! Read measurement set into vis_params structure
utilities::vis_params read_measurementset(std::string const &filename,
                                          const MeasurementSet::ChannelWrapper::polarization pol
                                          = MeasurementSet::ChannelWrapper::polarization::I,
                                          const std::vector<t_int> &channels = std::vector<t_int>(),
                                          std::string const &filter = "");
//! Return average frequency over channels
t_real average_frequency(const purify::casa::MeasurementSet &ms_file, std::string const &filter,
                         const std::vector<t_int> &channels);

inline MeasurementSet::const_iterator operator+(MeasurementSet::const_iterator::difference_type n,
                                                MeasurementSet::const_iterator const &c) {
  return c.operator+(n);
}
inline MeasurementSet::const_iterator operator-(MeasurementSet::const_iterator::difference_type n,
                                                MeasurementSet::const_iterator const &c) {
  return c.operator-(n);
}
}
}

#endif
