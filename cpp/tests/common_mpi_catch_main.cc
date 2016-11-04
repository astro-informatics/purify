#define CATCH_CONFIG_RUNNER

#include <purify/config.h>
#include <purify/logging.h>
#include <sopt/logging.h>
#include <catch.hpp>
#include <mpi.h>
#include <memory>
#include <random>

std::unique_ptr<std::mt19937_64> mersenne(new std::mt19937_64(0));

int main(int argc, const char **argv) {
  Catch::Session session; // There must be exactly once instance

  int returnCode = session.applyCommandLine(argc, argv);
  if(returnCode != 0) // Indicates a command line error
    return returnCode;
  mersenne.reset(new std::mt19937_64(session.configData().rngSeed));

  sopt::logging::initialize();
  purify::logging::initialize();

  MPI_Init(&argc, const_cast<char ***>(&argv));

  auto const result = session.run();

  int finalized;
  MPI_Finalized(&finalized);

  return result;
}
