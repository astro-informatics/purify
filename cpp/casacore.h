#ifndef PURIFY_CASACORE_H
#define PURIFY_CASACORE_H

#include <string>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/Table.h>
#include "types.h"

namespace purify {
namespace casa {

//! Interface around measurement sets
class MeasurementSet {
public:
  //! Constructs the interface around a given measurement set
  MeasurementSet(std::string const filename) : filename_(filename){};
  //! Filename of the measurement set
  std::string const &filename() const { return filename_; }

  int spectral_window_id(int datadescid = 0) const;
  Array<t_real> frequencies(int datadescid = 0) const;

  //! Gets table or subtable
  ::casacore::Table table(std::string const &name = "") const {
    auto const tabname = name == "" ? filename() : filename() + "/" + name;
    return ::casacore::Table(tabname);
  }

  //! Gets scalar column from table
  template <class T>
  ::casacore::ScalarColumn<T>
  scalar_column(std::string const &col, std::string const &tabname = "") const {
    return get_column<T, ::casacore::ScalarColumn>(col, tabname);
  }
  //! Gets array column from table
  template <class T>
  ::casacore::ArrayColumn<T>
  array_column(std::string const &col, std::string const &tabname = "") const {
    return get_column<T, ::casacore::ArrayColumn>(col, tabname);
  }

private:
  template <class T, template<class> class TYPE>
  TYPE<T> get_column(std::string const &col, std::string const &tabname) const {
    return {table(tabname), col};
  }

  //! Name associated with the measurement set
  std::string filename_;
};

}
}

#endif
