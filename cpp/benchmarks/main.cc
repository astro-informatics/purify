//#include <sopt/mpi/session.h>
#include <mpi.h>
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
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
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
    MPI_Allreduce(&elapsed_seconds, &max_elapsed_second, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
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
int main(int argc, char **argv) {

  //auto const session = sopt::mpi::init(nargs, args);
  MPI_Init(&argc, &argv);
  //auto const world = sopt::mpi::Communicator::World();

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  ::benchmark::Initialize(&argc, argv);

  if(rank == 0)
    //if(world.is_root()) {  
    // root process will use a reporter from the usual set provided by
    // ::benchmark
    ::benchmark::RunSpecifiedBenchmarks();
  else {
    // reporting from other processes is disabled by passing a custom reporter
    NullReporter null;
    ::benchmark::RunSpecifiedBenchmarks(&null);
  }

  MPI_Finalize();
  return 0;
}
