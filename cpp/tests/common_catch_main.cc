#define CATCH_CONFIG_RUNNER

#include "purify/config.h"
#include <sopt/config.h>
#include <catch.hpp>
#include <memory>
#include <random>
#include "purify/logging.h"
#include <sopt/logging.h>

std::unique_ptr<std::mt19937_64> mersenne(new std::mt19937_64(0));

int main(int argc, char **argv) {
  Catch::Session session;  // There must be exactly once instance

  int returnCode = session.applyCommandLine(argc, const_cast<char **>(argv));
  if (returnCode != 0)  // Indicates a command line error
    return returnCode;
  mersenne.reset(new std::mt19937_64(session.configData().rngSeed));

  sopt::logging::initialize();
  purify::logging::initialize();

  return session.run();
}
