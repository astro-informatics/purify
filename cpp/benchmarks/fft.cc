
#include <chrono>
#include <benchmark/benchmark.h>
#include "benchmarks/utilities.h"
#include "purify/operators.h"

using namespace purify;


// ----------------- Application benchmarks -----------------------//


class FFTOperatorFixture : public ::benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State& state) {}

  void TearDown(const ::benchmark::State& state) {}

  // A bunch of useful variables
  t_uint m_counter;
};

BENCHMARK_DEFINE_F(FFTOperatorFixture, Apply)(benchmark::State& state) {
  const t_uint m_imsizex = state.range(0);
  const t_uint m_imsizey = state.range(0);
  const t_uint N = m_imsizex * m_imsizey;
  const auto fftop = purify::operators::init_FFT_2d<Vector<t_complex>>(m_imsizey, m_imsizex, 1.);
  const auto &forward = std::get<0>(fftop);
  
  const Vector<t_complex> input = Vector<t_complex>::Random(N);
  Vector<t_complex> output = Vector<t_complex>::Zero(N);
  forward(output, input);
  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    forward(output, input);
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end));
  }
}


BENCHMARK_REGISTER_F(FFTOperatorFixture, Apply)
    //->Apply(b_utilities::Arguments)
    ->RangeMultiplier(2)
    ->Range(128, 128 << 6)
    ->UseManualTime()
    ->Repetitions(10)
    ->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN();
