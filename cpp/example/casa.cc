#include "purify/config.h"
#include "purify/casacore.h"
#include "purify/directories.h"
#include "purify/logging.h"

#define CHECK(CONDITION, ERROR)                                                                    \
  if(not(CONDITION))                                                                               \
    throw std::runtime_error(ERROR);
#define CHECK_THROWS(STATEMENT, ERROR)                                                             \
  {                                                                                                \
    bool did_throw = false;                                                                        \
    try {                                                                                          \
      STATEMENT;                                                                                   \
    } catch(...) {                                                                                 \
      did_throw = true;                                                                            \
    }                                                                                              \
    if(not did_throw)                                                                              \
      throw std::runtime_error(ERROR);                                                             \
  }

int main(int, char **) {
  purify::logging::initialize();
  purify::logging::set_level(purify::default_logging_level());
  // Loads a measurement set
  auto const ngc3256_filename = purify::notinstalled::ngc3256_ms();
  auto const ngc3256 = purify::casa::MeasurementSet(ngc3256_filename);

  try {
    // Gets the number of channels
    CHECK(ngc3256.size() == 128, "Not the number of channels I expected");
    // some of these channels are invalid
    // in which case, the different vectors are empty
    auto const channel0 = ngc3256[0];
    CHECK(channel0.is_valid() == false, "Expected channel to be invalid");
    CHECK(channel0.raw_u().size() == 0, "Expected raw u to be empty");
    CHECK(channel0.lambda_u().size() == 0, "Expected frequencies to be empty");
    CHECK(channel0.I().size() == 0, "Expected I component to be empty");
    CHECK(channel0.wI().size() == 0, "Expected weights associated with I to be empty");

    // other channels will have data
    auto const channel17 = ngc3256[17];
    CHECK(channel17.is_valid(), "Channel should be valid");
    CHECK(channel17.frequencies().size() == 141059, "Incorrect channel size");
    CHECK(channel17.width().size() == 141059, "Incorrect channel-width size");
    CHECK(channel17.raw_w().size() == 141059, "Incorrect channel size");
    CHECK(channel17.wQ().size() == 141059, "Incorrect channel size");

    // valid channels have access to RA, DEC
    CHECK(std::abs(channel17.right_ascension() - 2.7395560603928995) < 1e-8, "Right Ascension");
    CHECK(std::abs(channel17.declination() + 0.76628680808811045) < 1e-8, "Declination");
    // invalid channels do not
    CHECK_THROWS(channel0.declination(), "Can't figure out direction of empty channel");

    // It is also possible to loop over channels
    for(auto const &channel : ngc3256) {
      if(not channel.is_valid())
        continue;
      // although we will stop at the first one
      CHECK(channel.channel() == 17, "First valid channel is #17");
      break;
    }

    // Finally, it is possible to filter down some components
    // Note that the filter "all(NOT FLAG[i,])" has already been applied, where
    // i the channel index
    auto const filter = "OBSERVATION_ID=1 AND DATA_DESC_ID=0";
    auto const filtered = ngc3256[std::make_pair(17, filter)];
    CHECK(filtered.lambda_w().size() == 6300, "Incorrect size for filtered data");
    // Loops can also be filtered
    for(auto i_first = ngc3256.begin(filter); i_first != ngc3256.end(filter); ++i_first) {
      if(not i_first->is_valid())
        continue;
      CHECK(i_first->channel() == 17, "First valid channel is #17");
      CHECK(i_first->V().size() == 6300, "Incorrect size for the filtered V Stokes component");
      break;
    }
  } catch(std::runtime_error const &e) {
    std::cerr << "Example did not run as expected:\n" << e.what() << "\n";
    return 1;
  }
  return 0;
}
#undef CHECK
#undef CHECK_THROWS
