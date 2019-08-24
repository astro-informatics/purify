#include <chrono>
#include <benchmark/benchmark.h>
#include "benchmarks/utilities.h"
#include "purify/operators.h"

using namespace purify;

// ----------------- Application benchmarks -----------------------//

class GridOperatorFixture : public ::benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State& state) {
    M = state.range(0);
    const t_uint Ju = 4;
    const t_uint Jv = 4;
    const t_real oversample_ratio = 2;
    std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
    std::tie(kernelu, kernelv, ftkernelu, ftkernelv) =
        purify::create_kernels(kernels::kernel::kb, Ju, Jv, m_imsizey, m_imsizey, oversample_ratio);
    const auto uv_vis = b_utilities::random_measurements(M);
    Gop = purify::operators::init_on_the_fly_gridding_matrix_2d<Vector<t_complex>>(
        uv_vis.u, uv_vis.v, uv_vis.weights, m_imsizey, m_imsizex, oversample_ratio, kernelv,
        kernelu, Ju, Jv);
  }

  void TearDown(const ::benchmark::State& state) {}

  // A bunch of useful variables
  t_uint m_counter;
  t_uint M;
  t_uint m_imsizey = 1024;
  t_uint m_imsizex = 1024;
  std::tuple<sopt::OperatorFunction<Vector<t_complex>>, sopt::OperatorFunction<Vector<t_complex>>>
      Gop;
};

BENCHMARK_DEFINE_F(GridOperatorFixture, Apply)(benchmark::State& state) {
  const t_real oversample_ratio = 2;
  const t_uint N = m_imsizex * m_imsizey * oversample_ratio * oversample_ratio;
  const auto& forward = std::get<0>(Gop);

  const Vector<t_complex> input = Vector<t_complex>::Random(N);
  Vector<t_complex> output = Vector<t_complex>::Zero(M);
  forward(output, input);
  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    forward(output, input);
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end));
  }
}

BENCHMARK_REGISTER_F(GridOperatorFixture, Apply)
    //->Apply(b_utilities::Arguments)
    ->RangeMultiplier(2)
    ->Range(100000, 100000 << 11)
    ->UseManualTime()
    ->Repetitions(10)
    ->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
