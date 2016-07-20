#ifndef PURIFY_CASACORE_H
#define PURIFY_CASACORE_H

#include <map>
#include <string>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/Table.h>
#include "types.h"

namespace purify {
namespace casa {

//! Interface around measurement sets
class MeasurementSet {
public:
  enum class Sigma {
    PREFER_SIGMA,
    PREFER_SIGMA_SPECTRUM,
    ONLY_SIGMA,
    ONLY_SIGMA_SPECTRUM
  };
  //! Constructs the interface around a given measurement set
  MeasurementSet(std::string const filename)
      : filename_(filename),
        tables_(new std::map<std::string, ::casacore::Table>){};
  //! Filename of the measurement set
  std::string const &filename() const { return filename_; }
  //! Set new filename
  MeasurementSet &filename(std::string const &filename);

  // Array<t_real> frequencies() const;

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

  //! \brief Input name if given, otherwise DATA or DATA_CORRECTED
  //! It is an error to give a column name if a column with a name does not exist
  std::string data_column_name(std::string const &name = "") const;

  //! Clear memory
  void clear() { tables_->clear(); }

  Vector<t_real> u() const;
  Vector<t_real> v() const;
  Vector<t_real> w() const;
  //! Stokes I component
  Matrix<t_complex> I(std::string const &name = "DATA") const;
  //! Weights for the Stokes I component
  Matrix<t_real> wI(std::string const &name = "SIGMA") const;
  //! Stokes Q component
  Matrix<t_complex> Q(std::string const &name = "DATA") const;
  //! Weights for the Stokes Q component
  Matrix<t_real> wQ(std::string const &name = "SIGMA") const;
  //! Stokes U component
  Matrix<t_complex> U(std::string const &name = "DATA") const;
  //! Weights for the Stokes U component
  Matrix<t_real> wU(std::string const &name = "SIGMA") const;
  //! Stokes V component
  Matrix<t_complex> V(std::string const &name = "DATA") const;
  //! Weights for the Stokes V component
  Matrix<t_real> wV(std::string const &name = "SIGMA") const;

private:
  //! Gets stokes of given array/object
  template<class T> Matrix<T> stokes(std::string origin, std::string pol) const;

  template <class T, template <class> class TYPE>
  TYPE<T> get_column(std::string const &col, std::string const &tabname) const {
    return {table(tabname), col};
  }

  //! Name associated with the measurement set
  std::string filename_;

  //! Holds tables that have been opened
  std::unique_ptr<std::map<std::string, ::casacore::Table>> tables_;
};
}
}

#endif
