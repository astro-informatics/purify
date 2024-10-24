#include "purify/config.h"
#include "purify/logging.h"
#include <benchmark/benchmark.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
#include <sopt/logging.h>

// This reporter does nothing.
// We can use it to disable output from all but the root process
class NullReporter : public ::benchmark::BenchmarkReporter {
 public:
  NullReporter() {}
  virtual bool ReportContext(const Context &) { return true; }
  virtual void ReportRuns(const std::vector<Run> &) {}
  virtual void Finalize() {}
};

// The main is rewritten to allow for MPI initializing and for selecting a
// reporter according to the process rank
int main(int argc, char const **argv) {

    sopt::logging::set_level("debug");
    purify::logging::set_level("debug");
  
#ifdef PURIFY_MPI
  auto const session = sopt::mpi::init(argc, argv);
  auto const world = sopt::mpi::Communicator::World();
  PURIFY_LOW_LOG("MPI initialized");
#endif
  ::benchmark::Initialize(&argc, const_cast<char **>(argv));

#ifdef PURIFY_MPI
  if (world.is_root())
    // root process will use a reporter from the usual set provided by
    // ::benchmark
    ::benchmark::RunSpecifiedBenchmarks();
  else {
    // reporting from other processes is disabled by passing a custom reporter
    NullReporter null;
    ::benchmark::RunSpecifiedBenchmarks(&null);
  }
#endif
  return 0;
}
