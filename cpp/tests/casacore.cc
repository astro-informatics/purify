#include <boost/filesystem.hpp>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/Tables/ArrColDesc.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ColumnDesc.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/SetupNewTab.h>
#include <casacore/tables/Tables/TableColumn.h>
#include "casacore.h"

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

TEST_CASE("From constructed") {
  TmpMS ms;
  { // setup spectral windo id
    ScalarColumn<Int> column(ms->dataDescription(), "SPECTRAL_WINDOW_ID");
    ms->dataDescription().addRow();
    ms->dataDescription().addRow();
    column.put(0, 0);
    column.put(1, 1);
  }
  purify::casa::MeasurementSet pms(ms.path().string());

  SECTION("spectral window id") {
    ScalarColumn<Int> column(ms->dataDescription(), "SPECTRAL_WINDOW_ID");
    column.put(1, 5);
    CHECK(pms.spectral_window_id() == 0);
    pms.data_desc_id(1);
    CHECK(pms.spectral_window_id() == 5);
  }
  SECTION("frequencies") {
    ArrayColumn<Double> column(ms->spectralWindow(), "CHAN_FREQ");
    ms->spectralWindow().addRow();
    ms->spectralWindow().addRow();

    Array<Double> single(IPosition(1, 1));
    single(IPosition(1, 0)) = 1.5;
    column.put(0, single);

    Array<Double> triple(IPosition(1, 3));
    triple(IPosition(1, 0)) = 1.5;
    triple(IPosition(1, 1)) = 2.5;
    triple(IPosition(1, 2)) = 3.5;
    column.put(1, triple);

    CHECK(pms.frequencies().size() == 1);
    CHECK(pms.frequencies().isApprox(purify::Array<purify::t_real>::Ones(1) * 1.5));
    pms.data_desc_id(1);
    CHECK(pms.frequencies().size() == 3);
    CHECK(pms.frequencies().isApprox(Eigen::Array3d(1.5, 2.5, 3.5)));
  }
}

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
  CHECK(pms.u().isApprox(purify::Array<purify::t_real>::LinSpaced(3, 0, 6).matrix()));
  CHECK(pms.v().isApprox(purify::Array<purify::t_real>::LinSpaced(3, 1, 7).matrix()));
  CHECK(pms.w().isApprox(purify::Array<purify::t_real>::LinSpaced(3, 2, 8).matrix()));
}

TEST_CASE("SIGMA") {
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
  CHECK(pms.u().isApprox(purify::Array<purify::t_real>::LinSpaced(3, 0, 6).matrix()));
  CHECK(pms.v().isApprox(purify::Array<purify::t_real>::LinSpaced(3, 1, 7).matrix()));
  CHECK(pms.w().isApprox(purify::Array<purify::t_real>::LinSpaced(3, 2, 8).matrix()));
}

TEST_CASE("FSpectral Window ID") {
  auto const filename = "/Users/mdavezac/workspaces/purify/data/at166B.3C129.c0.ms";
  // ::casacore::Table table(filename);
  // REQUIRE(table.tableDesc().isColumn("UVW"));
  purify::casa::MeasurementSet const pms(filename);
  std::string NAME = "DATA";
  REQUIRE(pms.table().tableDesc().isColumn(NAME));
  CAPTURE(pms.table().tableDesc().columnDesc(NAME).trueDataType());
  CAPTURE(pms.table().tableDesc().columnDesc(NAME).isArray());
  CAPTURE(pms.table().tableDesc().columnDesc(NAME).ndim());
  CAPTURE(pms.table().tableDesc().columnDesc(NAME).shape());
  CAPTURE(pms.table().tableDesc().columnDesc(NAME).isFixedShape());
  auto const column = pms.array_column<::casacore::Complex>(NAME);
  CAPTURE(column.nrow());
  CAPTURE(column.shape(0));
  auto const acol = column.getColumn();
  // CHECK(false);
}
