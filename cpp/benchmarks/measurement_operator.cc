#include <sstream>
#include <chrono>
#include <benchmark/benchmark.h>
#include <sopt/imaging_padmm.h>
#include <sopt/wavelets.h>
#include "purify/operators.h"
#include "purify/utilities.h"
#include "purify/directories.h"
#include "purify/pfitsio.h"

using namespace purify;
using namespace purify::notinstalled;


utilities::vis_params random_measurements(t_int size) {

  t_real const sigma_m = constant::pi / 3;
  const t_real max_w = 100.; // lambda
  auto uv_data = utilities::random_sample_density(size, 0, sigma_m, max_w);
  uv_data.units = "radians";

  return uv_data;
}



// -----------------------------------------------------------------------------------------------//

void degrid_operator_ctor(benchmark::State &state) {

  // Generating random uv(w) coverage
  t_int const rows = state.range(0);
  t_int const cols = state.range(0);
  t_int const number_of_vis = state.range(1);
  auto uv_data = random_measurements(number_of_vis);

  const t_real FoV = 1;      // deg
  const t_real cellsize = FoV / cols * 60. * 60.;
  const bool w_term = false;
  // benchmark the creation of measurement operator
  while(state.KeepRunning()) {
    //auto start = std::chrono::high_resolution_clock::now();
    auto const sky_measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        uv_data, rows, cols, cellsize, cellsize, 2, 100, 0.0001, "kb", state.range(2), state.range(2),
        "measure", w_term);
    //auto end   = std::chrono::high_resolution_clock::now();

    //auto elapsed_seconds =
    //std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    //state.SetIterationTime(elapsed_seconds.count());
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (number_of_vis + rows * cols) * sizeof(t_complex));
}


void degrid_operator_apply_dir(benchmark::State &state) {

  // Reading image from file
  const std::string &name = "M31_"+std::to_string(state.range(0));
  std::string const fitsfile = image_filename(name + ".fits");
  auto M31 = pfitsio::read2d(fitsfile);
  t_real const max = M31.array().abs().maxCoeff();
  M31 = M31 * 1. / max;

  // Generating random uv(w) coverage
  t_int const number_of_vis = state.range(1);
  auto uv_data = random_measurements(number_of_vis);

  // Create measurement operator
  const t_real FoV = 1;      // deg
  const t_real cellsize = FoV / M31.cols() * 60. * 60.;
  const bool w_term = false;
  t_uint const imsizey = M31.rows();
  t_uint const imsizex = M31.cols();
  auto const sky_measurements = std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(
      measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_data, imsizey, imsizex, cellsize, cellsize, 2, 100, 0.0001, "kb", state.range(2), state.range(2),
          "measure", w_term));

  // Benchmark the application of the operator
  while(state.KeepRunning()) {
    uv_data.vis = (*sky_measurements) * Image<t_complex>::Map(M31.data(), M31.size(), 1);
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (number_of_vis + imsizey * imsizex) * sizeof(t_complex));
}


static void Arguments(benchmark::internal::Benchmark* b) {
  int uv_size_max = 256; // 4096
  int im_size_max = 10000; // 1M, 10M, 100M
  int kernel_max = 16; // 16
  for (int i=128; i<=uv_size_max; i*=2)
    for (int j=1000; j<=im_size_max; j*=10)
      for (int k=2; k<=kernel_max; k*=2)
        b->Args({i,j,k});
}


//BENCHMARK(degrid_operator_ctor)
//->Apply(Arguments)
//->Unit(benchmark::kMillisecond);

BENCHMARK(degrid_operator_apply_dir)
->Apply(Arguments)
->Unit(benchmark::kMicrosecond);


BENCHMARK_MAIN();
