#include <sstream>
#include <chrono>
#include <benchmark/benchmark.h>
#include "purify/operators.h"
#include "purify/utilities.h"
#include "purify/directories.h"
#include "purify/pfitsio.h"

using namespace purify;
using namespace purify::notinstalled;


void degrid_operator(benchmark::State &state) {

  // Generating random uv(w) coverage
  t_int const rows = state.range_x();
  t_int const cols = state.range_x();
  t_int const number_of_vis = std::floor(rows * cols * 0.1);
  t_real const sigma_m = constant::pi / 3;
  const t_real max_w = 100.; // lambda
  auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m, max_w);
  uv_data.units = "radians";

  const t_real FoV = 1;      // deg
  const t_real cellsize = FoV / cols * 60. * 60.;
  const bool w_term = true;
  // benchmark the creation of measurement operator
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto const sky_measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_data, rows, cols, cellsize, cellsize, 2, 100, 0.0001, "kb", 8, 8,
          "measure", w_term);
    auto end   = std::chrono::high_resolution_clock::now();

    auto elapsed_seconds =
      std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    state.SetIterationTime(elapsed_seconds.count());
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * number_of_vis * rows * cols * sizeof(t_complex));
}

BENCHMARK(degrid_operator)->Arg(128)->Unit(benchmark::kMillisecond);
//BENCHMARK(degrid_operator)->Arg(128)->UseRealTime()->Unit(benchmark::kMillisecond);
//BENCHMARK(degrid_operator)->Arg(128)->UseManualTime()->Unit(benchmark::kMillisecond);
BENCHMARK(degrid_operator)->Arg(128)->Threads(1)->Unit(benchmark::kMillisecond);
BENCHMARK(degrid_operator)->Arg(128)->Threads(2)->Unit(benchmark::kMillisecond);
BENCHMARK(degrid_operator)->Arg(128)->Threads(3)->Unit(benchmark::kMillisecond);
BENCHMARK(degrid_operator)->Arg(128)->Threads(4)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
