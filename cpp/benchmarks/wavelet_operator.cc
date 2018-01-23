#include <chrono>
#include <benchmark/benchmark.h>
#include "purify/operators.h"
#include "benchmarks/utilities.h"
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>

using namespace purify;


// -------------- Constructor benchmark -------------------------//

void wavelet_operator_constructor(benchmark::State &state) {

  // Image size
  t_uint m_imsizex = 1024;
  t_uint m_imsizey = 1024;

  // benchmark the creation of measurement operator
  while(state.KeepRunning()) {

    auto start = std::chrono::high_resolution_clock::now();

    const  sopt::wavelets::SARA m_sara{std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

    sopt::LinearTransform<Vector<t_complex>> Psi = sopt::linear_transform<t_complex>(m_sara, m_imsizey, m_imsizex);

    auto end   = std::chrono::high_resolution_clock::now();

    state.SetIterationTime(b_utilities::duration(start,end));
  }
}

BENCHMARK(wavelet_operator_constructor)
//->Apply(b_utilities::Arguments)
->Args({1024,1000,4})//->Args({1024,1000,4})
->UseManualTime()
->Repetitions(1)->ReportAggregatesOnly(true)
->Unit(benchmark::kMillisecond);

// ----------------- Application benchmarks -----------------------//

class WaveletOperatorFixture : public ::benchmark::Fixture {

public:
  void SetUp(const ::benchmark::State& state) {
  }

  void TearDown(const ::benchmark::State& state) {
  }

  // A bunch of useful variables
  t_uint m_counter;
  t_uint m_imsizex = 1024;
  t_uint m_imsizey = 1024;

};


BENCHMARK_DEFINE_F(WaveletOperatorFixture, Apply)(benchmark::State &state) {

  sopt::wavelets::SARA m_sara{std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  sopt::LinearTransform<Vector<t_complex>> m_Psi = sopt::linear_transform<t_complex>(m_sara, m_imsizey, m_imsizex);

  // Benchmark the application of the operator
  Vector<t_complex> temp_adjoint;

  //adjoint application to calculate the dimension of the wavelet basis
  
    temp_adjoint = m_Psi.adjoint() *  Vector<t_complex>::Random(m_imsizey * m_imsizex);
  

  // Get the number of wavelet coefs
  t_uint const n_wave_coeff = temp_adjoint.size();

  // Apply Psi to a temporary vector
  Vector<t_complex> temp;
  Vector<t_complex> const x = Vector<t_complex>::Random(n_wave_coeff);

  while(state.KeepRunning()) {

    auto start = std::chrono::high_resolution_clock::now();
    temp = m_Psi * x;
    auto end   = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start,end));

  }
}

BENCHMARK_REGISTER_F(WaveletOperatorFixture, Apply)
//->Apply(b_utilities::Arguments)
->Args({1024,1000,4})//->Args({1024,1000,4})
->UseManualTime()
->Repetitions(1)->ReportAggregatesOnly(true)
->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN();
