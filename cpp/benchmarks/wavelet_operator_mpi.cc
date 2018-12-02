#include <chrono>
#include <benchmark/benchmark.h>
#include "benchmarks/utilities.h"
#include "purify/wavelet_operator_factory.h"
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>

using namespace purify;

// -------------- Constructor benchmark -------------------------//

// void wavelet_operator_constructor_mpi(benchmark::State &state) {

//   // Image size
//   t_uint m_imsizex = state.range(0);
//   t_uint m_imsizey = state.range(0);
//   // MPI communicator
//   sopt::mpi::Communicator m_world = sopt::mpi::Communicator::World();

//   // benchmark the creation of measurement operator
//   while(state.KeepRunning()) {

//     auto start = std::chrono::high_resolution_clock::now();

//     const  sopt::wavelets::SARA m_sara{std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u),
//     std::make_tuple("DB2", 3u),
//       std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
//       std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

//     sopt::wavelets::SARA saraDistr = sopt::wavelets::distribute_sara(m_sara, m_world);

//     sopt::LinearTransform<Vector<t_complex>> Psi = sopt::linear_transform<t_complex>(saraDistr,
//     m_imsizey, m_imsizex, m_world);

//     auto end   = std::chrono::high_resolution_clock::now();

//     state.SetIterationTime(b_utilities::duration(start,end));
//   }
// }

// BENCHMARK(wavelet_operator_constructor_mpi)
// //->Apply(b_utilities::Arguments)
// //->Args({1024})
// ->RangeMultiplier(2)->Range(1024, 1024<<4)
// ->UseManualTime()
// ->Repetitions(1)->ReportAggregatesOnly(true)
// ->Unit(benchmark::kMillisecond);

// ----------------- Application benchmarks -----------------------//

class WaveletOperatorMPIFixture : public ::benchmark::Fixture {
 public:
  WaveletOperatorMPIFixture(){};
  void SetUp(const ::benchmark::State& state) {
    m_imsizex = state.range(0);
    m_imsizey = state.range(0);
    b_utilities::update_comm(m_world);
    sopt::wavelets::SARA m_sara{
        std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
        std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
        std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

    sopt::wavelets::SARA const saraDistr = sopt::wavelets::distribute_sara(m_sara, m_world);

    // Get the number of wavelet coefs
    n_wave_coeff = saraDistr.size() * m_imsizey * m_imsizex;
    m_Psi = sopt::linear_transform<t_complex>(saraDistr, m_imsizey, m_imsizex, m_world);
  }

  void TearDown(const ::benchmark::State& state) {}

  // A bunch of useful variables
  t_uint m_counter;
  // MPI communicator
  sopt::mpi::Communicator m_world;
  sopt::LinearTransform<Vector<t_complex>> m_Psi = sopt::linear_transform_identity<t_complex>();
  t_uint m_imsizex;
  t_uint m_imsizey;
  // Get the number of wavelet coefs
  t_uint n_wave_coeff;
};

BENCHMARK_DEFINE_F(WaveletOperatorMPIFixture, Forward)(benchmark::State& state) {
  // Benchmark the application of the operator

  // Apply Psi to a temporary vector
  Vector<t_complex> image = Vector<t_complex>::Random(m_imsizey * m_imsizex);
  Vector<t_complex> const wavelet_coeff = Vector<t_complex>::Ones(n_wave_coeff);

  while (m_world.broadcast<int>(state.KeepRunning())) {
    auto start = std::chrono::high_resolution_clock::now();
    image = m_Psi * wavelet_coeff;
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }
}

BENCHMARK_DEFINE_F(WaveletOperatorMPIFixture, Adjoint)(benchmark::State& state) {


  // Apply Psi to a temporary vector
  Vector<t_complex> const image = Vector<t_complex>::Ones(m_imsizey * m_imsizex);
  Vector<t_complex> wavelet_coeff = Vector<t_complex>::Zero(n_wave_coeff);

  while (m_world.broadcast<int>(state.KeepRunning())) {
    auto start = std::chrono::high_resolution_clock::now();
    wavelet_coeff = m_Psi.adjoint() * image;
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }
}

BENCHMARK_REGISTER_F(WaveletOperatorMPIFixture, Forward)
    // //->Apply(b_utilities::Arguments)
    ->RangeMultiplier(2)
    ->Range(128, 128 << 3)
    ->UseManualTime()
    ->Repetitions(5)
    ->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(WaveletOperatorMPIFixture, Adjoint)
    // //->Apply(b_utilities::Arguments)
    ->RangeMultiplier(2)
    ->Range(128, 128 << 3)
    ->UseManualTime()
    ->Repetitions(5)
    ->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

// BENCHMARK_MAIN();
