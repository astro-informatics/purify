#include <sopt/mpi/session.h>
#include <sopt/mpi/communicator.h>
#include <benchmark/benchmark.h>
#include <chrono>
#include <thread>

namespace {
// The unit of code we want to benchmark
void i_am_sleepy(int macsleepface) {
  // Pretend to work ...
  std::this_thread::sleep_for(std::chrono::milliseconds(macsleepface));
  // ... as a team
  MPI_Barrier(MPI_COMM_WORLD);
}

void mpi_benchmark(benchmark::State &state) {
  double max_elapsed_second;
  int rank;
  auto const world = sopt::mpi::Communicator::World();
  rank = world.rank();
  while(state.KeepRunning()) {
    // Do the work and time it on each proc
    auto start = std::chrono::high_resolution_clock::now();
    i_am_sleepy(rank % 5);
    auto end = std::chrono::high_resolution_clock::now();
    // Now get the max time across all procs:
    // for better or for worse, the slowest processor is the one that is
    // holding back the others in the benchmark.
    auto const duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    auto elapsed_seconds = duration.count();
    max_elapsed_second = world.all_reduce(elapsed_seconds, MPI_MAX);
    state.SetIterationTime(max_elapsed_second);
  }
}
}

BENCHMARK(mpi_benchmark)->UseManualTime();

// This reporter does nothing.
// We can use it to disable output from all but the root process
class NullReporter : public ::benchmark::BenchmarkReporter {
public:
  NullReporter() {}
  virtual bool ReportContext(const Context &) {return true;}
  virtual void ReportRuns(const std::vector<Run> &) {}
  virtual void Finalize() {}
};

// The main is rewritten to allow for MPI initializing and for selecting a
// reporter according to the process rank
int main(int argc, char const **argv) {

  auto const session = sopt::mpi::init(argc, argv);
  auto const world = sopt::mpi::Communicator::World();

  ::benchmark::Initialize(&argc, const_cast<char**>(argv));

  if(world.is_root())
    // root process will use a reporter from the usual set provided by
    // ::benchmark
    ::benchmark::RunSpecifiedBenchmarks();
  else {
    // reporting from other processes is disabled by passing a custom reporter
    NullReporter null;
    ::benchmark::RunSpecifiedBenchmarks(&null);
  }

  return 0;
}
