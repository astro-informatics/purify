#ifndef BENCHMARK_UTILITIES_H
#define BENCHMARK_UTILITIES_H

#include <chrono>
#include "purify/utilities.h"
#include <benchmark/benchmark.h>
#include <sopt/mpi/communicator.h>

using namespace purify;
using namespace purify::notinstalled;

namespace b_utilities {

  void Arguments(benchmark::internal::Benchmark* b);

  double duration(std::chrono::high_resolution_clock::time_point start, std::chrono::high_resolution_clock::time_point end);
  double duration(std::chrono::high_resolution_clock::time_point start, std::chrono::high_resolution_clock::time_point end,
                  sopt::mpi::Communicator const &comm);

  utilities::vis_params random_measurements(t_int size);
  utilities::vis_params random_measurements(t_int size, sopt::mpi::Communicator const &comm);

}

#endif
