#include <chrono>
#include <benchmark/benchmark.h>
#include <sopt/imaging_padmm.h>
#include <sopt/wavelets.h>
#include "purify/operators.h"
#include "purify/directories.h"
#include "purify/pfitsio.h"
#include "benchmarks/utilities.h"

using namespace purify;
using namespace purify::notinstalled;


// -------------- Constructor benchmark -------------------------//

void degrid_operator_ctor(benchmark::State &state) {

  // Generating random uv(w) coverage
  t_int const rows = state.range(0);
  t_int const cols = state.range(0);
  t_int const number_of_vis = state.range(1);
  auto uv_data = b_utilities::random_measurements(number_of_vis);

  const t_real FoV = 1;      // deg
  const t_real cellsize = FoV / cols * 60. * 60.;
  const bool w_term = false;
  // benchmark the creation of measurement operator
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto const sky_measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        uv_data, rows, cols, cellsize, cellsize, 2, 0, 0.0001, "kb", state.range(2), state.range(2),
        "measure", w_term);
    auto end   = std::chrono::high_resolution_clock::now();

    state.SetIterationTime(b_utilities::duration(start,end));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (number_of_vis + rows * cols) * sizeof(t_complex));
}

BENCHMARK(degrid_operator_ctor)
//->Apply(b_utilities::Arguments)
->Args({1024,1000000,4})->Args({1024,10000000,4})
->UseManualTime()
->Repetitions(10)->ReportAggregatesOnly(true)
->Unit(benchmark::kMillisecond);


// ----------------- Application benchmarks -----------------------//

class DegridOperatorFixture : public ::benchmark::Fixture
{
public:
  void SetUp(const ::benchmark::State& state) {
    bool newImage = m_imsizex!=state.range(0);
    bool newMeasurements = m_uv_data.vis.size()!=state.range(1);
    bool newKernel = m_kernel!=state.range(2);
    
    // Reading image from file
    if (newImage) {
      const std::string &name = "M31_"+std::to_string(state.range(0));
      std::string const fitsfile = image_filename(name + ".fits");
      m_image = pfitsio::read2d(fitsfile);
      m_imsizex = m_image.cols();
      m_imsizey = m_image.rows();
      t_real const max = m_image.array().abs().maxCoeff();
      m_image = m_image * 1. / max;
    }
    
    // Generating random uv(w) coverage
    if (newMeasurements) {
      m_uv_data = b_utilities::random_measurements(state.range(1));
    }
    
    // Create measurement operator
    if (newImage || newMeasurements || newKernel) {
      const t_real FoV = 1;      // deg
      const t_real cellsize = FoV / m_imsizex * 60. * 60.;
      const bool w_term = false;
      m_kernel = state.range(2);
      m_degridOperator = std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(
          measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
	  m_uv_data, m_imsizey, m_imsizex, cellsize, cellsize, 2, 0, 0.0001, "kb", m_kernel, m_kernel,
          "measure", w_term));
	  }
  }

  void TearDown(const ::benchmark::State& state) {
  }

  Image<t_complex> m_image;
  t_uint m_imsizex;
  t_uint m_imsizey;
  utilities::vis_params m_uv_data;
  t_uint m_kernel;
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> m_degridOperator;
};


BENCHMARK_DEFINE_F(DegridOperatorFixture, Direct)(benchmark::State &state) {
  // Benchmark the application of the operator
  utilities::vis_params theMeasurement;
  theMeasurement.vis = (*m_degridOperator) *  Image<t_complex>::Map(m_image.data(), m_image.size(), 1);

  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    theMeasurement.vis = (*m_degridOperator) *  Image<t_complex>::Map(m_image.data(), m_image.size(), 1);
    auto end   = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start,end));
  }
  
  state.SetBytesProcessed(int64_t(state.iterations()) * (state.range(1) + m_imsizey * m_imsizex) * sizeof(t_complex));
}

BENCHMARK_DEFINE_F(DegridOperatorFixture, Adjoint)(benchmark::State &state) {
  // Benchmark the application of the adjoint operator
  Vector<t_complex> theImage(m_image.size());
  theImage = m_degridOperator->adjoint() * m_uv_data.vis;
  
   while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    theImage = m_degridOperator->adjoint() * m_uv_data.vis;
    auto end   = std::chrono::high_resolution_clock::now();   
    state.SetIterationTime(b_utilities::duration(start,end));
  }
  
  state.SetBytesProcessed(int64_t(state.iterations()) * (state.range(1) + m_imsizey * m_imsizex) * sizeof(t_complex));
}

BENCHMARK_REGISTER_F(DegridOperatorFixture, Direct)
//->Apply(b_utilities::Arguments)
->Args({1024,1000000,4})->Args({1024,10000000,4})
->UseManualTime()
->Repetitions(10)->ReportAggregatesOnly(true)
->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(DegridOperatorFixture, Adjoint)
//->Apply(b_utilities::Arguments)
->Args({1024,1000000,4})->Args({1024,10000000,4})
->UseManualTime()
->Repetitions(10)->ReportAggregatesOnly(true)
->Unit(benchmark::kMillisecond);



BENCHMARK_MAIN();
