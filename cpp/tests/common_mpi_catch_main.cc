#define CATCH_CONFIG_RUNNER

#include <purify/config.h>
#include <catch.hpp>
#include <memory>
#include <mpi.h>
#include <random>
#include <regex>
#include <vector>
#include <purify/logging.h>
#include <sopt/logging.h>
#include <sopt/mpi/session.h>

std::unique_ptr<std::mt19937_64> mersenne(new std::mt19937_64(0));

int main(int argc, const char **argv) {
  Catch::Session session; // There must be exactly once instance

  auto const mpi_session = sopt::mpi::init(argc, argv);

  // The following mess transforms the input arguments so that output files have different names
  // on different processors
  std::vector<std::string> arguments(argv, argv + argc);
  auto output_opt = std::find_if(arguments.begin(), arguments.end(), [](std::string const &arg) {
    if(arg == "-o" or arg == "--out")
      return true;
    auto const N = std::string("--out=").size();
    return arg.size() > N and arg.substr(0, N) == "--out=";
  });
  if(output_opt != arguments.end()) {
    if(*output_opt == "-o" or *output_opt == "--out")
      output_opt += 1;
    if(output_opt != arguments.end()) {
      int rank;
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      if(rank > 0)
        *output_opt
            = std::regex_replace(*output_opt, std::regex("\\.xml"), std::to_string(rank) + ".xml");
    }
  }

  // transforms the modified arguments to a C-style array of pointers.
  std::vector<char const *> cargs(arguments.size());
  std::transform(arguments.begin(), arguments.end(), cargs.begin(),
                 [](std::string const &c) { return c.c_str(); });

  int returnCode = session.applyCommandLine(cargs.size(), const_cast<char**>(cargs.data()));
  if(returnCode != 0) // Indicates a command line error
    return returnCode;
  mersenne.reset(new std::mt19937_64(session.configData().rngSeed));

  sopt::logging::initialize();
  purify::logging::initialize();

  auto const result = session.run();

  return result;
}
