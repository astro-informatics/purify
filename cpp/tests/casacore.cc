#include <boost/filesystem.hpp>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/TaQL/TableParse.h>
#include <casacore/tables/Tables/ArrColDesc.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ColumnDesc.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/SetupNewTab.h>
#include <casacore/tables/Tables/TableColumn.h>
#include "casacore.h"
#include "directories.h"

#include "catch.hpp"
using namespace ::casacore;

TEST_CASE("Casacore") {
  // create the table descriptor
  casa::TableDesc simpleDesc = casa::MS::requiredTableDesc();
  // set up a new table
  casa::SetupNewTable newTab("simpleTab", simpleDesc, casa::Table::New);
  // create the MeasurementSet
  casa::MeasurementSet simpleMS(newTab);
  // now we need to define all required subtables
  // the following call does this for us if we don't need to
  // specify details of Storage Managers for columns.
  simpleMS.createDefaultSubtables(casa::Table::New);
  // fill MeasurementSet via its Table interface
  // For example, construct one of the columns
  casa::TableColumn feed(simpleMS, casa::MS::columnName(casa::MS::FEED1));
  casa::uInt rownr = 0;
  // add a row
  simpleMS.addRow();
  // set the values in that row, e.g. the feed column
  feed.putScalar(rownr, 1);
  // Access a subtable
  casa::ArrayColumn<casa::Double> antpos(simpleMS.antenna(),
                                         casa::MSAntenna::columnName(casa::MSAntenna::POSITION));
  simpleMS.antenna().addRow();
  casa::Array<casa::Double> position(casa::IPosition(1, 3));
  position(casa::IPosition(1, 0)) = 1.;
  position(casa::IPosition(1, 1)) = 2.;
  position(casa::IPosition(1, 2)) = 3.;
  antpos.put(0, position);
}

class TmpPath {
public:
  TmpPath()
      : path_(boost::filesystem::unique_path(boost::filesystem::temp_directory_path()
                                             / "%%%%-%%%%-%%%%-%%%%.ms")) {}
  ~TmpPath() {
    if(boost::filesystem::exists(path()))
      boost::filesystem::remove_all(path());
  }
  boost::filesystem::path const &path() const { return path_; }

private:
  boost::filesystem::path path_;
};

class TmpMS : public TmpPath {
public:
  TmpMS() : TmpPath() {
    casa::TableDesc simpleDesc = casa::MS::requiredTableDesc();
    casa::SetupNewTable newTab(path().string(), simpleDesc, casa::Table::New);
    ms_.reset(new casa::MeasurementSet(newTab));
    ms_->createDefaultSubtables(casa::Table::New);
  }
  casa::MeasurementSet const &operator*() const { return *ms_; }
  casa::MeasurementSet &operator*() { return *ms_; }

  casa::MeasurementSet const *operator->() const { return ms_.get(); }
  casa::MeasurementSet *operator->() { return ms_.get(); }

protected:
  std::unique_ptr<casa::MeasurementSet> ms_;
};

TEST_CASE("DATA column") {
  TmpMS ms;
  purify::casa::MeasurementSet pms(ms.path().string());
  CHECK_THROWS_AS(pms.data_column_name(), std::runtime_error);
  CHECK_THROWS_AS(pms.data_column_name("something"), std::runtime_error);

  ms->addColumn(ScalarColumnDesc<float>("DATA"));
  CHECK(pms.data_column_name() == "DATA");
  CHECK_THROWS_AS(pms.data_column_name("something"), std::runtime_error);

  ms->addColumn(ScalarColumnDesc<float>("CORRECTED_DATA"));
  CHECK(pms.data_column_name() == "CORRECTED_DATA");
  CHECK_THROWS_AS(pms.data_column_name("something"), std::runtime_error);

  ms->addColumn(ScalarColumnDesc<float>("something"));
  CHECK(pms.data_column_name("something") == "something");
}

TEST_CASE("UVW") {
  TmpMS ms;
  ms->addRow();
  ms->addRow();
  ms->addRow();
  ArrayColumn<Double> uvw_column(*ms, "UVW");
  std::vector<Double> const uvw_data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  casa::Array<casa::Double> uvw_array(casa::IPosition(1, 3), uvw_data.data());
  uvw_column.put(0, casa::Array<casa::Double>(casa::IPosition(1, 3), uvw_data.data()));
  uvw_column.put(1, casa::Array<casa::Double>(casa::IPosition(1, 3), uvw_data.data() + 3));
  uvw_column.put(2, casa::Array<casa::Double>(casa::IPosition(1, 3), uvw_data.data() + 6));

  purify::casa::MeasurementSet pms(ms.path().string());
  CHECK(pms.uvw().col(0).isApprox(purify::Array<purify::t_real>::LinSpaced(3, 0, 6).matrix()));
  CHECK(pms.uvw().col(1).isApprox(purify::Array<purify::t_real>::LinSpaced(3, 1, 7).matrix()));
  CHECK(pms.uvw().col(2).isApprox(purify::Array<purify::t_real>::LinSpaced(3, 2, 8).matrix()));
}

TEST_CASE("Stokes I") {
  purify::casa::MeasurementSet const pms(purify::notinstalled::ngc3256_ms());
  auto const I = pms.I("DATA", "WHERE ALL(FLAG)");
  CAPTURE(I(0, 0));
  CAPTURE(I(10, 8));
  CHECK(I.rows() == 98557);
  CHECK(I.cols() == 128);
  CHECK(std::abs(I(0, 0) - 0.95408797264099121) < 1e-12);
  CHECK(std::abs(I(10, 8).real() - 9.6010975539684296e-5) < 1e-16);
  CHECK(std::abs(I(10, 8).imag() + 0.00010972682503052056) < 1e-16);
}

TEST_CASE("Frequency table") {
  purify::casa::MeasurementSet const pms(purify::notinstalled::ngc3256_ms());
  auto const f = pms.frequency_table();
  CHECK(f.rows() == 4);
  CHECK(f.cols() == 128);
  CHECK(std::abs(f(0, 0) - 113211987500.0) < 1e-8);
  CHECK(std::abs(f(3, 10) - 102894612500.0) < 1e-8);
}

TEST_CASE("Data description id") {
  purify::casa::MeasurementSet const pms(purify::notinstalled::ngc3256_ms());
  auto const d = pms.data_desc_id("WHERE ALL(FLAG)");
  CHECK(d.size() == 98557);
  CHECK(d(0) == 0);
  CHECK(d(98356) == 0);
  CHECK(d(84) == 1);
  CHECK(d(98399) == 1);
  CHECK(d(168) == 2);
  CHECK(d(98445) == 2);
  CHECK(d(252) == 3);
  CHECK(d(98555) == 3);
}

TEST_CASE("Frequencies") {
  auto const filter = "WHERE ALL(FLAG)";
  auto const freq
      = purify::casa::MeasurementSet(purify::notinstalled::ngc3256_ms()).frequencies(filter);
  auto const tab
      = purify::casa::MeasurementSet(purify::notinstalled::ngc3256_ms()).frequency_table();
  CHECK(freq.rows() == 98557);
  CHECK(freq.cols() == 128);
  CHECK(freq.row(0).isApprox(tab.row(0)));
  CHECK(freq.row(98356).isApprox(tab.row(0)));
  CHECK(freq.row(84).isApprox(tab.row(1)));
  CHECK(freq.row(98399).isApprox(tab.row(1)));
  CHECK(freq.row(168).isApprox(tab.row(2)));
  CHECK(freq.row(252).isApprox(tab.row(3)));
}
