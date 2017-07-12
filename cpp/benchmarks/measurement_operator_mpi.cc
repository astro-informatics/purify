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


// ----------------- Degrid operator benchmark fixture -----------------------//

class DegridOperatorFixtureMPI : public ::benchmark::Fixture
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

    // Data needed for the creation of the measurement operator
    m_rows = state.range(0);
    m_cols = state.range(0);
    m_number_of_vis = state.range(1);
    const t_real FoV = 1;      // deg
    m_cellsize = FoV / m_imsizex * 60. * 60.;
    m_w_term = false;
  }

  void TearDown(const ::benchmark::State& state) {
  }

  Image<t_complex> m_image;
  t_uint m_imsizex;
  t_uint m_imsizey;
  t_int m_rows;
  t_int m_cols;
  t_int m_number_of_vis;
  t_real m_cellsize;
  bool m_w_term;
};


// -------------- Constructor benchmarks -------------------------//

BENCHMARK_DEFINE_F(DegridOperatorFixtureMPI, CtorDistr)(benchmark::State &state) {
  // Generating random uv(w) coverage
  auto const world = sopt::mpi::Communicator::World();
  auto uv_data = random_measurements(m_number_of_vis,world);

  // benchmark the creation of the distributed measurement operator
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto const sky_measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        world, uv_data, m_rows, m_cols, m_cellsize, m_cellsize, 2, 100, 0.0001, "kb", state.range(2), state.range(2),
        "measure", m_w_term);
    auto end = std::chrono::high_resolution_clock::now();

    state.SetIterationTime(b_utilities::duration(start,end,world));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (m_number_of_vis + m_rows * m_cols) * sizeof(t_complex));
}

BENCHMARK_REGISTER_F(DegridOperatorFixtureMPI, CtorDistr)
->Apply(b_utilities::Arguments)
->UseManualTime()
->Unit(benchmark::kMillisecond);

BENCHMARK_DEFINE_F(DegridOperatorFixtureMPI, CtorMPI)(benchmark::State &state) {
  // Generating random uv(w) coverage
  auto const world = sopt::mpi::Communicator::World();
  auto uv_data = random_measurements(m_number_of_vis,world);

  // benchmark the creation of the distributed MPI measurement operator
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto const sky_measurements = measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
        world, uv_data, m_rows, m_cols, m_cellsize, m_cellsize, 2, 100, 0.0001, "kb", state.range(2), state.range(2),
        "measure", m_w_term);
    auto end = std::chrono::high_resolution_clock::now();

    state.SetIterationTime(b_utilities::duration(start,end,world));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (m_number_of_vis + m_rows * m_cols) * sizeof(t_complex));
}

BENCHMARK_REGISTER_F(DegridOperatorFixtureMPI, CtorMPI)
->Apply(b_utilities::Arguments)
->UseManualTime()
->Unit(benchmark::kMillisecond);


// ----------------- Application benchmarks -----------------------//

BENCHMARK_DEFINE_F(DegridOperatorFixtureMPI, DirectDistr)(benchmark::State &state) {
  // Generating random uv(w) coverage
  auto const world = sopt::mpi::Communicator::World();
  auto uv_data = random_measurements(m_number_of_vis,world);
  
  // Create the distributed operator
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>>  degridOperator =
    std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(
      measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      world, uv_data, m_imsizey, m_imsizex, m_cellsize, m_cellsize, 2, 100, 0.0001, "kb", state.range(2), state.range(2),
      "measure", m_w_term));

  // Benchmark the application of the distributed operator
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    uv_data.vis = (*degridOperator) * Image<t_complex>::Map(m_image.data(), m_image.size(), 1);
    auto end = std::chrono::high_resolution_clock::now();
    
    state.SetIterationTime(b_utilities::duration(start,end,world));
  }
  
  state.SetBytesProcessed(int64_t(state.iterations()) * (m_number_of_vis + m_imsizey * m_imsizex) * sizeof(t_complex));
}

BENCHMARK_REGISTER_F(DegridOperatorFixtureMPI, DirectDistr)->Apply(b_utilities::Arguments)
->UseManualTime()
->Unit(benchmark::kMicrosecond);

BENCHMARK_DEFINE_F(DegridOperatorFixtureMPI, AdjointDistr)(benchmark::State &state) {
  // Generating random uv(w) coverage
  auto const world = sopt::mpi::Communicator::World();
  auto uv_data = random_measurements(m_number_of_vis,world);
  
  // Create the distributed operator
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>>  degridOperator =
    std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(
      measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      world, uv_data, m_imsizey, m_imsizex, m_cellsize, m_cellsize, 2, 100, 0.0001, "kb", state.range(2), state.range(2),
      "measure", m_w_term));
  
  // Benchmark the application of the adjoint distributed operator
  Vector<t_complex> theImage(m_image.size());
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    theImage = degridOperator->adjoint() * uv_data.vis;
    auto end = std::chrono::high_resolution_clock::now();
    
    state.SetIterationTime(b_utilities::duration(start,end,world));
  }
  
  state.SetBytesProcessed(int64_t(state.iterations()) * (m_number_of_vis + m_imsizey * m_imsizex) * sizeof(t_complex));
}

BENCHMARK_REGISTER_F(DegridOperatorFixtureMPI, AdjointDistr)->Apply(b_utilities::Arguments)
->UseManualTime()
->Unit(benchmark::kMicrosecond);

BENCHMARK_DEFINE_F(DegridOperatorFixtureMPI, DirectMPI)(benchmark::State &state) {
  // Generating random uv(w) coverage
  auto const world = sopt::mpi::Communicator::World();
  auto uv_data = random_measurements(m_number_of_vis,world);
  
  // Create the distributed MPI operator
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>>  degridOperator =
    std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(
      measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
      world, uv_data, m_imsizey, m_imsizex, m_cellsize, m_cellsize, 2, 100, 0.0001, "kb", state.range(2), state.range(2),
      "measure", m_w_term));

  // Benchmark the application of the distributed MPI operator
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    uv_data.vis = (*degridOperator) * Image<t_complex>::Map(m_image.data(), m_image.size(), 1);
    auto end = std::chrono::high_resolution_clock::now();
    
    state.SetIterationTime(b_utilities::duration(start,end,world));
  }
  
  state.SetBytesProcessed(int64_t(state.iterations()) * (m_number_of_vis + m_imsizey * m_imsizex) * sizeof(t_complex));
}

BENCHMARK_REGISTER_F(DegridOperatorFixtureMPI, DirectMPI)->Apply(b_utilities::Arguments)
->UseManualTime()
->Unit(benchmark::kMicrosecond);

BENCHMARK_DEFINE_F(DegridOperatorFixtureMPI, AdjointMPI)(benchmark::State &state) {
  // Generating random uv(w) coverage
  auto const world = sopt::mpi::Communicator::World();
  auto uv_data = random_measurements(m_number_of_vis,world);
  
  // Create the distributed MPI operator
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>>  degridOperator =
    std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(
      measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
      world, uv_data, m_imsizey, m_imsizex, m_cellsize, m_cellsize, 2, 100, 0.0001, "kb", state.range(2), state.range(2),
      "measure", m_w_term));
  
  // Benchmark the application of the adjoint distributed MPI operator
  Vector<t_complex> theImage(m_image.size());
  while(state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    theImage = degridOperator->adjoint() * uv_data.vis;
    auto end = std::chrono::high_resolution_clock::now();
    
    state.SetIterationTime(b_utilities::duration(start,end,world));
  }
  
  state.SetBytesProcessed(int64_t(state.iterations()) * (m_number_of_vis + m_imsizey * m_imsizex) * sizeof(t_complex));
}

BENCHMARK_REGISTER_F(DegridOperatorFixtureMPI, AdjointMPI)->Apply(b_utilities::Arguments)
->UseManualTime()
->Unit(benchmark::kMicrosecond);
