#include <chrono>
#include <benchmark/benchmark.h>
#include "benchmarks/utilities.h"
#include "purify/operators.h"

using namespace purify;

// -------------- Constructor benchmark -------------------------//

void degrid_operator_ctor(benchmark::State &state) {

  // Generating random uv(w) coverage
  t_int const rows = state.range(0);
  t_int const cols = state.range(0);
  t_int const number_of_vis = state.range(1);
  auto uv_data = b_utilities::random_measurements(number_of_vis);

  const t_real FoV = 1; // deg
  const t_real cellsize = FoV / cols * 60. * 60.;
  const bool w_term = false;
  // benchmark the creation of measurement operator
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto sky_measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        uv_data, rows, cols, cellsize, cellsize, 2, 0, 0.0001, kernels::kernel::kb, state.range(2),
        state.range(2), w_term);
    auto end = std::chrono::high_resolution_clock::now();

    state.SetIterationTime(b_utilities::duration(start, end));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (number_of_vis + rows * cols)
                          * sizeof(t_complex));
}

BENCHMARK(degrid_operator_ctor)
    //->Apply(b_utilities::Arguments)
    ->Args({1024, 1000000, 4})
    ->Args({1024, 10000000, 4})
    ->UseManualTime()
    ->Repetitions(10)
    ->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

// ----------------- Application benchmarks -----------------------//

class DegridOperatorFixture : public ::benchmark::Fixture {
public:
  void SetUp(const ::benchmark::State &state) {
    // Keep count of the benchmark repetitions
    m_counter++;

    // Reading image from file and create temporary image
    bool newImage = updateImage(state.range(0));

    // Generating random uv(w) coverage
    bool newMeasurements = b_utilities::updateMeasurements(state.range(1), m_uv_data);

    // Create measurement operator
    bool newKernel = m_kernel != state.range(2);
    if(newImage || newMeasurements || newKernel) {
      const t_real FoV = 1; // deg
      const t_real cellsize = FoV / m_imsizex * 60. * 60.;
      const bool w_term = false;
      m_kernel = state.range(2);
      m_degridOperator = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          m_uv_data, m_imsizey, m_imsizex, cellsize, cellsize, 2, 0, 0.0001, kernels::kernel::kb,
          m_kernel, m_kernel, w_term);
    }
  }

  void TearDown(const ::benchmark::State &state) {}

  virtual bool updateImage(t_uint newSize) = 0;

  t_uint m_counter;

  t_uint m_imsizex;
  t_uint m_imsizey;

  utilities::vis_params m_uv_data;

  t_uint m_kernel;
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> m_degridOperator;
};

class DegridOperatorDirectFixture : public DegridOperatorFixture {
public:
  virtual bool updateImage(t_uint newSize) {
    return b_utilities::updateImage(newSize, m_image, m_imsizex, m_imsizey);
  }

  Image<t_complex> m_image;
};

class DegridOperatorAdjointFixture : public DegridOperatorFixture {
public:
  virtual bool updateImage(t_uint newSize) {
    return b_utilities::updateEmptyImage(newSize, m_image, m_imsizex, m_imsizey);
  }

  Vector<t_complex> m_image;
};

BENCHMARK_DEFINE_F(DegridOperatorDirectFixture, Apply)(benchmark::State &state) {
  // Benchmark the application of the operator
  if((m_counter % 10) == 1) {
    m_uv_data.vis = (*m_degridOperator) * Image<t_complex>::Map(m_image.data(), m_image.size(), 1);
  }
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    m_uv_data.vis = (*m_degridOperator) * Image<t_complex>::Map(m_image.data(), m_image.size(), 1);
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (state.range(1) + m_imsizey * m_imsizex)
                          * sizeof(t_complex));
}

BENCHMARK_DEFINE_F(DegridOperatorAdjointFixture, Apply)(benchmark::State &state) {
  // Benchmark the application of the adjoint operator
  if((m_counter % 10) == 1) {
    m_image = m_degridOperator->adjoint() * m_uv_data.vis;
  }
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    m_image = m_degridOperator->adjoint() * m_uv_data.vis;
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (state.range(1) + m_imsizey * m_imsizex)
                          * sizeof(t_complex));
}

BENCHMARK_REGISTER_F(DegridOperatorDirectFixture, Apply)
    //->Apply(b_utilities::Arguments)
    ->Args({1024, 1000000, 4})
    ->Args({1024, 10000000, 4})
    ->UseManualTime()
    ->Repetitions(10)
    ->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(DegridOperatorAdjointFixture, Apply)
    //->Apply(b_utilities::Arguments)
    ->Args({1024, 1000000, 4})
    ->Args({1024, 10000000, 4})
    ->UseManualTime()
    ->Repetitions(10)
    ->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
