#include <sstream>
#include <chrono>
#include <benchmark/benchmark.h>
#include <sopt/mpi/session.h>
#include <sopt/mpi/communicator.h>
#include <sopt/imaging_padmm.h>
#include <sopt/wavelets.h>
#include "purify/operators.h"
#include "purify/distribute.h"
#include "purify/mpi_utilities.h"
#include "purify/directories.h"
#include "purify/pfitsio.h"
#include "benchmarks/utilities.h"

using namespace purify;
using namespace purify::notinstalled;


// -------------- Helper functions ----------------------------//

utilities::vis_params random_measurements(t_int size, sopt::mpi::Communicator const &comm) {
  if(comm.is_root()) {
    // Generate random measurements
    t_real const sigma_m = constant::pi / 3;
    const t_real max_w = 100.; // lambda
    auto uv_data = utilities::random_sample_density(size, 0, sigma_m, max_w);
    uv_data.units = "radians";
    if(comm.size() == 1)
      return uv_data;
    
    // Distribute them
    auto const order
      = distribute::distribute_measurements(uv_data, comm, "distance_distribution");
    uv_data = utilities::regroup_and_scatter(uv_data, order, comm);
    return uv_data;
  }
  return utilities::scatter_visibilities(comm);
}


// -------------- Constructor benchmark -------------------------//

void degrid_operator_ctor_distr(benchmark::State &state) {

  // Generating random uv(w) coverage
  t_int const rows = state.range(0);
  t_int const cols = state.range(0);
  t_int const number_of_vis = state.range(1);
  auto const world = sopt::mpi::Communicator::World();
  auto uv_data = random_measurements(number_of_vis,world);

  const t_real FoV = 1;      // deg
  const t_real cellsize = FoV / cols * 60. * 60.;
  const bool w_term = false;
  // benchmark the creation of the distributed measurement operator
  double max_elapsed_second;
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto const sky_measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        world, uv_data, rows, cols, cellsize, cellsize, 2, 100, 0.0001, "kb", state.range(2), state.range(2),
        "measure", w_term);
    auto end = std::chrono::high_resolution_clock::now();

    state.SetIterationTime(b_utilities::duration(start,end,world));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (number_of_vis + rows * cols) * sizeof(t_complex));
}

BENCHMARK(degrid_operator_ctor_distr)
->Apply(b_utilities::Arguments)
->UseManualTime()
->Unit(benchmark::kMillisecond);

// TODO: this is copy-pasting code. Fixtire didn't work with communicator, have to fix this!
void degrid_operator_ctor_mpi(benchmark::State &state) {

  // Generating random uv(w) coverage
  t_int const rows = state.range(0);
  t_int const cols = state.range(0);
  t_int const number_of_vis = state.range(1);
  auto const world = sopt::mpi::Communicator::World();
  auto uv_data = random_measurements(number_of_vis,world);

  const t_real FoV = 1;      // deg
  const t_real cellsize = FoV / cols * 60. * 60.;
  const bool w_term = false;
  // benchmark the creation of the distributed measurement operator
  double max_elapsed_second;
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto const sky_measurements = measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
        world, uv_data, rows, cols, cellsize, cellsize, 2, 100, 0.0001, "kb", state.range(2), state.range(2),
        "measure", w_term);
    auto end = std::chrono::high_resolution_clock::now();

    state.SetIterationTime(b_utilities::duration(start,end,world));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (number_of_vis + rows * cols) * sizeof(t_complex));
}

BENCHMARK(degrid_operator_ctor_mpi)
->Apply(b_utilities::Arguments)
->UseManualTime()
->Unit(benchmark::kMillisecond);

/*
// ----------------- Application benchmark - with fixture -----------------------//

class DegridOperatorFixture : public ::benchmark::Fixture
{
public:
  void SetUp(const ::benchmark::State& state) {
    // Reading image from file
    const std::string &name = "M31_"+std::to_string(state.range(0));
    std::string const fitsfile = image_filename(name + ".fits");
    m_image = pfitsio::read2d(fitsfile);
    m_imsizex = m_image.cols();
    m_imsizey = m_image.rows();
    t_real const max = m_image.array().abs().maxCoeff();
    m_image = m_image * 1. / max;
    
    // Generating random uv(w) coverage
    t_int const number_of_vis = state.range(1);
    m_uv_data = random_measurements(number_of_vis);
    
    // Create measurement operator
    const t_real FoV = 1;      // deg
    const t_real cellsize = FoV / m_imsizex * 60. * 60.;
    const bool w_term = false;
    m_degridOperator = std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(
      measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          m_uv_data, m_imsizey, m_imsizex, cellsize, cellsize, 2, 100, 0.0001, "kb", state.range(2), state.range(2),
          "measure", w_term));
  }

  void TearDown(const ::benchmark::State& state) {
  }

  Image<t_complex> m_image;
  t_uint m_imsizex;
  t_uint m_imsizey;
  utilities::vis_params m_uv_data;
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> m_degridOperator;
};


  BENCHMARK_DEFINE_F(DegridOperatorFixture, Direct)(benchmark::State &state) {
    // Benchmark the application of the operator
    while(state.KeepRunning()) {
      m_uv_data.vis = (*m_degridOperator) * Image<t_complex>::Map(m_image.data(), m_image.size(), 1);
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * (state.range(1) + m_imsizey * m_imsizex) * sizeof(t_complex));
  }

  BENCHMARK_DEFINE_F(DegridOperatorFixture, Adjoint)(benchmark::State &state) {
    // Benchmark the application of the adjoint operator
    Vector<t_complex> theImage(m_image.size());
    while(state.KeepRunning()) {
      theImage = m_degridOperator->adjoint() * m_uv_data.vis;
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * (state.range(1) + m_imsizey * m_imsizex) * sizeof(t_complex));
  }

BENCHMARK_REGISTER_F(DegridOperatorFixture, Direct)->Apply(Arguments)
->Unit(benchmark::kMicrosecond);

BENCHMARK_REGISTER_F(DegridOperatorFixture, Adjoint)->Apply(Arguments)
->Unit(benchmark::kMicrosecond);



BENCHMARK_MAIN();*/
