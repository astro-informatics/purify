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
#include "purify/casacore.h"
#include "purify/directories.h"

#include "purify/types.h"
#include "purify/utilities.h"

#include "catch.hpp"
using namespace ::casacore;

using namespace purify::notinstalled;
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

const std::string test_file = atca_filename("0332-391.ms");

TEST_CASE("Size/Number of channels") {
  CHECK(purify::casa::MeasurementSet(test_file).size() == 13);
}

TEST_CASE("Single channel") {
  namespace pc = purify::casa;
  auto const ms = pc::MeasurementSet(test_file);
  SECTION("Check channel validity") {
    CHECK(pc::MeasurementSet::const_iterator::value_type(0, ms).is_valid());
    CHECK(pc::MeasurementSet::const_iterator::value_type(4, ms).is_valid());
  }
  // Raw data from the measurement set "0332-391.ms" was read out using CASA's casabrowser executable. 
  // Then it was copied into this test.
  SECTION("Raw UVW") {
    auto const channel = pc::MeasurementSet::const_iterator::value_type(11, ms);
    REQUIRE(channel.size() == 20541);
    auto const u = channel.raw_u();
    REQUIRE(u.size() == 20541);
    CHECK(std::abs(u[0] - 3889.46519177759410013095475733280181884765625) < 1e-8);
    CHECK(std::abs(u[3360] - 683.2842475491) < 1e-8);
    auto const v = channel.raw_v();
    REQUIRE(v.size() == 20541);
    CHECK(std::abs(v[0] - 1346.20383349449502929928712546825408935546875) < 1e-8);
    CHECK(std::abs(v[3360] + 785.8117311632) < 1e-8);
    auto const w = channel.raw_w();
    REQUIRE(w.size() == 20541);
    CHECK(std::abs(w[0] - 1663.30025165469896819558925926685333251953125) < 1e-8);
    CHECK(std::abs(w[3360] + 970.3123979733) < 1e-8);
  }

  SECTION("Raw frequencies") {
    auto const f0 = pc::MeasurementSet::const_iterator::value_type(0, ms).raw_frequencies();
    CHECK(f0.size() == 1);
    CHECK(std::abs(f0(0) - 1431999959.500274181365966796875) < 1e-1);

    auto const f11 = pc::MeasurementSet::const_iterator::value_type(11, ms).raw_frequencies();
    CHECK(f11.size() == 1);
    CHECK(std::abs(f11(0) - 1343999961.9890842437744140625) < 1e-1);
  }

  SECTION("data desc id") {
    REQUIRE(pc::MeasurementSet::const_iterator::value_type(0, ms).data_desc_id().size() == 20490);
    REQUIRE(pc::MeasurementSet::const_iterator::value_type(4, ms).data_desc_id().size() == 20606);
  }

  SECTION("I") {
    using namespace purify;
    auto const I = pc::MeasurementSet::const_iterator::value_type(11, ms).I();
    REQUIRE(I.size() == 20541);
    CHECK(std::abs(I(0) - t_complex(0.1666463911533,-0.05232101678848)) < 1e-12);
    CHECK(std::abs(I(10) - t_complex(0.1421023607254,-0.04858251661062)) < 1e-12);

    REQUIRE(pc::MeasurementSet::const_iterator::value_type(0, ms).I().size() == 20490);
  }

  SECTION("wI") {
    using namespace purify;
    auto const wI = pc::MeasurementSet::const_iterator::value_type(11, ms).wI();
    REQUIRE(wI.size() == 20541);
    CAPTURE(wI.head(5).transpose());
    CHECK(wI.isApprox(0.5 * purify::Vector<t_real>::Ones(wI.size())));
  }

  SECTION("frequencies") {
    using namespace purify;
    auto const f = pc::MeasurementSet::const_iterator::value_type(11, ms).frequencies();
    REQUIRE(f.size() == 20541);
    CHECK(std::abs(f(0) - 1343999961.989) < 1e-0);
    CHECK(std::abs(f(1680) - 1343999961.989) < 1e-0);
    CHECK(std::abs(f(3360) - 1343999961.989) < 1e-0);
    CHECK(std::abs(f(5040) - 1343999961.989) < 1e-0);
  }
}

TEST_CASE("Measurement channel") {
  using namespace purify;
  auto const channel = purify::casa::MeasurementSet(test_file)[0];
  REQUIRE(channel.is_valid());
  auto const I = channel.I();
  REQUIRE(I.size() == 20490);
  CHECK(std::abs(I(0) - t_complex(-0.01469034608454,-0.00434834882617)) < 1e-12);
  CHECK(std::abs(I(10) - t_complex(-0.09461227059364,-0.01139064785093)) < 1e-12);
}

TEST_CASE("Channel iteration") {
  std::vector<int> const valids{0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12};
  auto const ms = purify::casa::MeasurementSet(test_file);
  auto i_channel = ms.begin();
  auto const i_end = ms.end();
  for(; i_channel < i_end; i_channel += 5) {
    CHECK(i_channel->channel() < 13);
    bool const is_valid
        = std::find(valids.begin(), valids.end(), i_channel->channel()) != valids.end();
    CHECK(is_valid == i_channel->is_valid());
  }
}


TEST_CASE("Direction") {
  auto const ms = purify::casa::MeasurementSet(test_file);
  auto const direction = ms.direction();
  auto const right_ascension = ms.right_ascension();
  auto const declination = ms.declination();
  CHECK(std::abs(right_ascension - 0.934273294000000031900299291010014712810516357421875) < 1e-8);
  CHECK(std::abs(declination + 0.68069387400000003207622967238421551883220672607421875) < 1e-8);
  CHECK(std::abs(direction[0] - 0.934273294000000031900299291010014712810516357421875) < 1e-8);
  CHECK(std::abs(direction[1] + 0.68069387400000003207622967238421551883220672607421875) < 1e-8);
}
