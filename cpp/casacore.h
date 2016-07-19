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
  MeasurementSet(std::string const filename, int data_desc_id = 0)
      : filename_(filename), data_desc_id_(data_desc_id),
        tables_(new std::map<std::string, ::casacore::Table>){};
  //! Filename of the measurement set
  std::string const &filename() const { return filename_; }
  //! Set new filename
  MeasurementSet &filename(std::string const &filename);

  //! This is the data we are looking for?
  int data_desc_id() const { return data_desc_id_; }
  MeasurementSet &data_desc_id(int d);

  int spectral_window_id() const;
  Array<t_real> frequencies() const;

  //! \brief Gets table or subtable
  //! \details If this is a column in the main table, then the current data_desc_id is selected.
  ::casacore::Table const &table(std::string const &name = "") const;

  //! Gets scalar column from table
  template <class T>
  ::casacore::ScalarColumn<T>
  scalar_column(std::string const &col, std::string const &tabname = "") const {
    return get_column<T, ::casacore::ScalarColumn>(col, tabname);
  }
  //! \brief Gets array column from table
  //! \details If this is a column in the main table, then the current data_desc_id is selected.
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
  // Vector<t_real> sigma(int Nc, int Nf, Sigma const &option = PREFER_SIGMA_SPECTRUM) const;

private:
  template <class T, template <class> class TYPE>
  TYPE<T> get_column(std::string const &col, std::string const &tabname) const {
    return {table(tabname, data_desc_id()), col};
  }

  //! \brief Gets table or subtable
  //! \details If this is a column in the main table, then the current data_desc_id is selected.
  ::casacore::Table const &table(std::string const &name, int data_desc_id) const;



  //! Name associated with the measurement set
  std::string filename_;

  int data_desc_id_;

  //! Holds tables that have been opened
  std::unique_ptr<std::map<std::string, ::casacore::Table>> tables_;
};
}
}

#endif
