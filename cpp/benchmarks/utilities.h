#ifndef BENCHMARK_UTILITIES_H
#define BENCHMARK_UTILITIES_H

#include <chrono>
#include "purify/utilities.h"
#include <benchmark/benchmark.h>
#ifdef PURIFY_MPI
#include <sopt/mpi/communicator.h>
#endif

using namespace purify;

namespace b_utilities {

  void Arguments(benchmark::internal::Benchmark* b);

  double duration(std::chrono::high_resolution_clock::time_point start, std::chrono::high_resolution_clock::time_point end);

  bool updateImage(t_uint newSize, Image<t_complex>& image, t_uint& sizex, t_uint& sizey);
  bool updateEmptyImage(t_uint newSize, Vector<t_complex>& image, t_uint& sizex, t_uint& sizey);
  
  bool updateMeasurements(t_uint newSize, utilities::vis_params& data);
  bool updateMeasurements(t_uint newSize, utilities::vis_params& data, t_real& epsilon, bool newImage, Image<t_complex>& image);
  
  std::tuple<utilities::vis_params, t_real>
    dirty_measurements(Image<t_complex> const &ground_truth_image, t_uint number_of_vis, t_real snr,
		       const t_real &cellsize);

  utilities::vis_params random_measurements(t_int size);
#ifdef PURIFY_MPI
  double duration(std::chrono::high_resolution_clock::time_point start, std::chrono::high_resolution_clock::time_point end,
                  sopt::mpi::Communicator const &comm);
  bool updateMeasurements(t_uint newSize, utilities::vis_params& data, sopt::mpi::Communicator& comm);
  bool updateMeasurements(t_uint newSize, utilities::vis_params& data, t_real& epsilon, bool newImage, Image<t_complex>& image,
			  sopt::mpi::Communicator& comm);
  std::tuple<utilities::vis_params, t_real>
    dirty_measurements(Image<t_complex> const &ground_truth_image, t_uint number_of_vis, t_real snr,
		       const t_real &cellsize, sopt::mpi::Communicator const &comm);
  utilities::vis_params random_measurements(t_int size, sopt::mpi::Communicator const &comm);

#endif
}

#endif
