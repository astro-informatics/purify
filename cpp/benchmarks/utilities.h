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

  bool updateImage(t_uint newSize, Image<t_complex>& image, t_uint& sizex, t_uint& sizey);
  bool updateTempImage(t_uint newSize, Vector<t_complex>& image);
  bool updateMeasurements(t_uint newSize, utilities::vis_params& data);
  bool updateMeasurements(t_uint newSize, utilities::vis_params& data, sopt::mpi::Communicator& comm);
  
  utilities::vis_params random_measurements(t_int size);
  utilities::vis_params random_measurements(t_int size, sopt::mpi::Communicator const &comm);

}

#endif
