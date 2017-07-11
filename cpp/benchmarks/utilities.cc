#include <benchmarks/utilities.h>

namespace b_utilities {

  void Arguments(benchmark::internal::Benchmark* b) {
    int uv_size_max = 256; // 4096
    int im_size_max = 1000; // 1M, 10M, 100M
    int kernel_max = 4; // 16
    for (int i=128; i<=uv_size_max; i*=2)
      for (int j=1000; j<=im_size_max; j*=10)
        for (int k=2; k<=kernel_max; k*=2)
          b->Args({i,j,k});
  }

  double duration(std::chrono::high_resolution_clock::time_point start, std::chrono::high_resolution_clock::time_point end){
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    return elapsed_seconds.count();
  }

  double duration(std::chrono::high_resolution_clock::time_point start, std::chrono::high_resolution_clock::time_point end,
                  sopt::mpi::Communicator const &comm) {
    auto elapsed_seconds = duration(start,end);
    // Now get the max time across all procs: the slowest processor is the one that is
    // holding back the others in the benchmark.
    return comm.all_reduce(elapsed_seconds, MPI_MAX);
  }
  
}
