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

TEST_CASE("Size/Number of channels") {
  CHECK(purify::casa::MeasurementSet(purify::notinstalled::ngc3256_ms()).size() == 128);
}

TEST_CASE("Single channel") {
  namespace pc = purify::casa;
  auto const ms = pc::MeasurementSet(purify::notinstalled::ngc3256_ms());
  SECTION("Check channel validity") {
    CHECK(not pc::MeasurementSet::const_iterator::value_type(0, ms).is_valid());
    CHECK(pc::MeasurementSet::const_iterator::value_type(17, ms).is_valid());
  }
  SECTION("Raw UVW") {
    auto const channel = pc::MeasurementSet::const_iterator::value_type(17, ms);
    REQUIRE(channel.size() == 141059);
    auto const u = channel.raw_u();
    REQUIRE(u.size() == 141059);
    CHECK(std::abs(u[0] + 48.184256396979784) < 1e-8);
    CHECK(std::abs(u[20000] + 34.078338234565763) < 1e-8);
    auto const v = channel.raw_v();
    REQUIRE(v.size() == 141059);
    CHECK(std::abs(v[0] + 2.8979287890493985) < 1e-8);
    CHECK(std::abs(v[20000] - 45.856539171696184) < 1e-8);
  }

  SECTION("Raw frequencies") {
    auto const f0 = pc::MeasurementSet::const_iterator::value_type(0, ms).raw_frequencies();
    CHECK(f0.size() == 4);
    CHECK(std::abs(f0(0) - 113211987500.0) < 1e-1);
    CHECK(std::abs(f0(1) - 111450812500.10001) < 1e-4);

    auto const f17 = pc::MeasurementSet::const_iterator::value_type(17, ms).raw_frequencies();
    CHECK(f17.size() == 4);
    CHECK(std::abs(f17(0) - 113477612500.0) < 1e-1);
    CHECK(std::abs(f17(1) - 111716437500.10001) < 1e-4);
  }

  SECTION("data desc id") {
    REQUIRE(pc::MeasurementSet::const_iterator::value_type(0, ms).data_desc_id().size() == 0);
    REQUIRE(pc::MeasurementSet::const_iterator::value_type(17, ms).data_desc_id().size() == 141059);
  }

  SECTION("I") {
    using namespace purify;
    auto const I = pc::MeasurementSet::const_iterator::value_type(17, ms).I();
    REQUIRE(I.size() == 141059);
    CHECK(std::abs(I(0) - t_complex(-0.30947005748748779, -0.10632525384426117)) < 1e-12);
    CHECK(std::abs(I(10) - t_complex(0.31782057881355286, 0.42863702774047852)) < 1e-12);

    REQUIRE(pc::MeasurementSet::const_iterator::value_type(0, ms).I().size() == 0);
  }

  SECTION("wI") {
    using namespace purify;
    auto const wI = pc::MeasurementSet::const_iterator::value_type(17, ms).wI();
    REQUIRE(wI.size() == 141059);
    CAPTURE(wI.head(5).transpose());
    CHECK(wI.isApprox(0.5 * purify::Vector<t_real>::Ones(wI.size())));
  }

  SECTION("frequencies") {
    using namespace purify;
    auto const f = pc::MeasurementSet::const_iterator::value_type(17, ms).frequencies();
    REQUIRE(f.size() == 141059);
    CHECK(std::abs(f(0) - 113477612500.0) < 1e-0);
    CHECK(std::abs(f(1680) - 111716437500.10001) < 1e-0);
    CHECK(std::abs(f(3360) - 101240562499.89999) < 1e-0);
    CHECK(std::abs(f(5040) - 102785237500.0) < 1e-0);
  }
}

TEST_CASE("Measurement channel") {
  using namespace purify;
  REQUIRE(not purify::casa::MeasurementSet(purify::notinstalled::ngc3256_ms())[0].is_valid());
  auto const channel = purify::casa::MeasurementSet(purify::notinstalled::ngc3256_ms())[17];
  REQUIRE(channel.is_valid());
  auto const I = channel.I();
  REQUIRE(I.size() == 141059);
  CHECK(std::abs(I(0) - t_complex(-0.30947005748748779, -0.10632525384426117)) < 1e-12);
  CHECK(std::abs(I(10) - t_complex(0.31782057881355286, 0.42863702774047852)) < 1e-12);
}

TEST_CASE("Channel iteration") {
  std::vector<int> const valids{
      17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,
      35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,
      53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,
      71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,
      89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106,
      107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124};
  auto const ms = purify::casa::MeasurementSet(purify::notinstalled::ngc3256_ms());
  auto i_channel = ms.begin();
  auto const i_end = ms.end();
  for(; i_channel < i_end; i_channel += 10) {
    CHECK(i_channel->channel() < 128);
    bool const is_valid
        = std::find(valids.begin(), valids.end(), i_channel->channel()) != valids.end();
    CHECK(is_valid == i_channel->is_valid());
  }
}

// TEST_CASE("Read Measurement") {
//   purify::utilities::vis_params const vis_file =
//   purify::utilities::read_visibility(vla_filename("at166B.3C129.c0I.vis"));
//   purify::utilities::vis_params const ms_file =
//   purify::casa::read_measurementset(vla_filename("at166B.3C129.c0.ms"),
//   purify::casa::MeasurementSet::ChannelWrapper::polarization::I);
//
//   REQUIRE(vis_file.u.size() == ms_file.u.size());
//
//   CHECK(vis_file.u.isApprox(ms_file.u, 1e-6));
//   CHECK(vis_file.v.isApprox(ms_file.v, 1e-6));
//   CHECK(vis_file.vis.isApprox(ms_file.vis, 1e-6));
//   purify::utilities::vis_params const ms_fileLL =
//   purify::casa::read_measurementset(vla_filename("at166B.3C129.c0.ms"),
//   purify::casa::MeasurementSet::ChannelWrapper::polarization::LL);
//   purify::utilities::vis_params const ms_fileRR =
//   purify::casa::read_measurementset(vla_filename("at166B.3C129.c0.ms"),
//   purify::casa::MeasurementSet::ChannelWrapper::polarization::RR);
//   purify::Vector<purify::t_complex> const weights = (1./(1./ms_fileLL.weights.array() +
//   1./ms_fileRR.weights.array())).matrix();
//   CHECK(vis_file.weights.real().isApprox(weights.real(), 1e-6));
// }

TEST_CASE("Direction") {
  auto const ms = purify::casa::MeasurementSet(purify::notinstalled::ngc3256_ms());
  auto const direction = ms.direction();
  auto const right_ascension = ms.right_ascension();
  auto const declination = ms.declination();
  CHECK(std::abs(right_ascension - 2.7395560603928995) < 1e-8);
  CHECK(std::abs(declination + 0.76628680808811045) < 1e-8);
  CHECK(std::abs(direction[0] - 2.7395560603928995) < 1e-8);
  CHECK(std::abs(direction[1] + 0.76628680808811045) < 1e-8);
}
