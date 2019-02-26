#include <chrono>
#include <sstream>
#include <benchmark/benchmark.h>
#include "benchmarks/utilities.h"
#include "purify/directories.h"
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include "purify/wide_field_utilities.h"
#include "purify/wproj_operators.h"
#include <sopt/imaging_padmm.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
#include <sopt/wavelets.h>

using namespace purify;
using namespace purify::notinstalled;

// ----------------- Degrid operator constructor fixture -----------------------//

class DegridOperatorCtorFixturePar : public ::benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State &state) {
    // Keep count of the benchmark repetitions
    m_counter++;
    // Data needed for the creation of the measurement operator
    const t_real FoV = 25;  // deg
    m_cellsize = FoV / m_imsizex * 60. * 60.;
    m_w_term = true;

    m_imsizex = state.range(0);
    m_imsizey = state.range(0);

    // Generating random uv(w) coverage
    b_utilities::update_comm(m_world);
    t_real const sigma_m = constant::pi / 3;
    const t_real du = widefield::pixel_to_lambda(m_cellsize, m_imsizex, 2);
    const t_real max_w = 300.;  // lambda
    auto const uv_data = b_utilities::random_measurements(state.range(1), max_w, m_world.rank());
    const auto cost = [](t_real x) -> t_real { return std::abs(x * x); };
    m_uv_data = utilities::w_stacking(uv_data, m_world, 100, cost);
    std::tie(m_uv_data_all_to_all, m_image_index, m_w_stacks) =
        utilities::w_stacking_with_all_to_all(uv_data, du, 4, 100, m_world, 100, cost);
  }

  void TearDown(const ::benchmark::State &state) {}

  t_uint m_counter;
  sopt::mpi::Communicator m_world;
  t_uint m_imsizex;
  t_uint m_imsizey;
  utilities::vis_params m_uv_data;
  utilities::vis_params m_uv_data_all_to_all;
  std::vector<t_real> m_w_stacks;
  std::vector<t_int> m_image_index;
  t_real m_cellsize;
  bool m_w_term;
};

// -------------- Constructor benchmarks -------------------------//

BENCHMARK_DEFINE_F(DegridOperatorCtorFixturePar, Distr)(benchmark::State &state) {
  // benchmark the creation of the distributed measurement operator
  if ((m_counter % 10) == 1) {
    auto sky_measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        m_world, m_uv_data, m_imsizey, m_imsizex, m_cellsize, m_cellsize, 2, kernels::kernel::kb,
        state.range(2), 100, m_w_term, 1e-6, 1e-6, dde_type::wkernel_radial);
  }
  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto sky_measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        m_world, m_uv_data, m_imsizey, m_imsizex, m_cellsize, m_cellsize, 2, kernels::kernel::kb,
        state.range(2), 100, m_w_term, 1e-6, 1e-6, dde_type::wkernel_radial);
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (state.range(1) + m_imsizex * m_imsizey) *
                          sizeof(t_complex));
}

BENCHMARK_DEFINE_F(DegridOperatorCtorFixturePar, MPI)(benchmark::State &state) {
  // benchmark the creation of the distributed MPI measurement operator
  if ((m_counter % 10) == 1) {
    auto sky_measurements =
        measurementoperator::init_degrid_operator_2d_all_to_all<Vector<t_complex>>(
            m_world, m_image_index, m_w_stacks, m_uv_data_all_to_all, m_imsizey, m_imsizex,
            m_cellsize, m_cellsize, 2, kernels::kernel::kb, state.range(2), 100, m_w_term, 1e-6,
            1e-6, dde_type::wkernel_radial);
  }
  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto sky_measurements =
        measurementoperator::init_degrid_operator_2d_all_to_all<Vector<t_complex>>(
            m_world, m_image_index, m_w_stacks, m_uv_data_all_to_all, m_imsizey, m_imsizex,
            m_cellsize, m_cellsize, 2, kernels::kernel::kb, state.range(2), 100, m_w_term, 1e-6,
            1e-6, dde_type::wkernel_radial);
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (state.range(1) + m_imsizex * m_imsizey) *
                          sizeof(t_complex));
}

// ----------------- Application fixtures -----------------------//

class DegridOperatorFixturePar : public ::benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State &state) {
    // Keep count of the benchmark repetitions
    m_counter++;

    // Reading image from file and create temporary image
    bool newImage = updateImage(state.range(0));

    // Generating random uv(w) coverage
    bool newMeasurements = b_utilities::updateMeasurements(state.range(1), m_uv_data, m_world);

    // Create measurement operators
    bool newKernel = m_kernel != state.range(2);
    if (newImage || newMeasurements || newKernel) {
      const t_real FoV = 1;  // deg
      const t_real cellsize = FoV / m_imsizex * 60. * 60.;
      const bool w_term = false;
      m_kernel = state.range(2);
      m_degridOperator = measurementOperator(cellsize, w_term);
    }
  }

  void TearDown(const ::benchmark::State &state) {}

  virtual bool updateImage(t_uint newSize) = 0;
  virtual std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> measurementOperator(
      t_real cellsize, bool w_term) = 0;

  t_uint m_counter;
  sopt::mpi::Communicator m_world;
  t_uint m_kernel;

  t_uint m_imsizex;
  t_uint m_imsizey;

  utilities::vis_params m_uv_data;

  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> m_degridOperator;
};

class DegridOperatorDirectFixturePar : public DegridOperatorFixturePar {
 public:
  virtual bool updateImage(t_uint newSize) {
    return b_utilities::updateImage(newSize, m_image, m_imsizex, m_imsizey);
  }

  Image<t_complex> m_image;
};

class DegridOperatorAdjointFixturePar : public DegridOperatorFixturePar {
 public:
  virtual bool updateImage(t_uint newSize) {
    return b_utilities::updateEmptyImage(newSize, m_image, m_imsizex, m_imsizey);
  }

  Vector<t_complex> m_image;
};

class DegridOperatorDirectFixtureDistr : public DegridOperatorDirectFixturePar {
 public:
  virtual std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> measurementOperator(
      t_real cellsize, bool w_term) {
    return measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        m_world, m_uv_data, m_imsizey, m_imsizex, cellsize, cellsize, 2, kernels::kernel::kb,
        m_kernel, m_kernel, w_term);
  }
};

class DegridOperatorDirectFixtureMPI : public DegridOperatorDirectFixturePar {
 public:
  virtual std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> measurementOperator(
      t_real cellsize, bool w_term) {
    return measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
        m_world, m_uv_data, m_imsizey, m_imsizex, cellsize, cellsize, 2, kernels::kernel::kb,
        m_kernel, m_kernel, w_term);
  }
};

class DegridOperatorAdjointFixtureDistr : public DegridOperatorAdjointFixturePar {
 public:
  virtual std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> measurementOperator(
      t_real cellsize, bool w_term) {
    return measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        m_world, m_uv_data, m_imsizey, m_imsizex, cellsize, cellsize, 2, kernels::kernel::kb,
        m_kernel, m_kernel, w_term);
  }
};

class DegridOperatorAdjointFixtureMPI : public DegridOperatorAdjointFixturePar {
 public:
  virtual std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> measurementOperator(
      t_real cellsize, bool w_term) {
    return measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
        m_world, m_uv_data, m_imsizey, m_imsizex, cellsize, cellsize, 2, kernels::kernel::kb,
        m_kernel, m_kernel, w_term);
  }
};

// ----------------- Application benchmarks -----------------------//

BENCHMARK_DEFINE_F(DegridOperatorDirectFixtureDistr, Apply)(benchmark::State &state) {
  // Benchmark the application of the distributed operator
  if ((m_counter % 10) == 1) {
    m_uv_data.vis = (*m_degridOperator) * Image<t_complex>::Map(m_image.data(), m_image.size(), 1);
  }
  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    m_uv_data.vis = (*m_degridOperator) * Image<t_complex>::Map(m_image.data(), m_image.size(), 1);
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (state.range(1) + m_imsizey * m_imsizex) *
                          sizeof(t_complex));
}

BENCHMARK_DEFINE_F(DegridOperatorAdjointFixtureDistr, Apply)(benchmark::State &state) {
  // Benchmark the application of the adjoint distributed operator
  if ((m_counter % 10) == 1) {
    m_image = m_degridOperator->adjoint() * m_uv_data.vis;
  }
  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    m_image = m_degridOperator->adjoint() * m_uv_data.vis;
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (state.range(1) + m_imsizey * m_imsizex) *
                          sizeof(t_complex));
}

BENCHMARK_DEFINE_F(DegridOperatorDirectFixtureMPI, Apply)(benchmark::State &state) {
  // Benchmark the application of the distributed MPI operator
  if ((m_counter % 10) == 1) {
    m_uv_data.vis = (*m_degridOperator) * Image<t_complex>::Map(m_image.data(), m_image.size(), 1);
  }
  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    m_uv_data.vis = (*m_degridOperator) * Image<t_complex>::Map(m_image.data(), m_image.size(), 1);
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (state.range(1) + m_imsizey * m_imsizex) *
                          sizeof(t_complex));
}

BENCHMARK_DEFINE_F(DegridOperatorAdjointFixtureMPI, Apply)(benchmark::State &state) {
  // Benchmark the application of the adjoint distributed MPI operator
  if ((m_counter % 10) == 1) {
    m_image = m_degridOperator->adjoint() * m_uv_data.vis;
  }
  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    m_image = m_degridOperator->adjoint() * m_uv_data.vis;
    auto end = std::chrono::high_resolution_clock::now();
    state.SetIterationTime(b_utilities::duration(start, end, m_world));
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * (state.range(1) + m_imsizey * m_imsizex) *
                          sizeof(t_complex));
}

// -------------- Register benchmarks -------------------------//

BENCHMARK_REGISTER_F(DegridOperatorCtorFixturePar, Distr)
    //->Apply(b_utilities::Arguments)
    ->Args({64, 1000, 4})
    //   ->Args({1024, 10000, 4})
    ->UseManualTime()
    ->Repetitions(10)
    //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(DegridOperatorCtorFixturePar, MPI)
    //->Apply(b_utilities::Arguments)
    ->Args({64, 1000, 4})
    //   ->Args({1024, 10000, 4})
    ->UseManualTime()
    ->Repetitions(10)
    //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);
/*
BENCHMARK_REGISTER_F(DegridOperatorDirectFixtureDistr, Apply)
    //->Apply(b_utilities::Arguments)
    ->Args({1024, 1000000, 4})
    ->Args({1024, 10000000, 4})
    ->UseManualTime()
    ->Repetitions(10)
    //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(DegridOperatorAdjointFixtureDistr, Apply)
    //->Apply(b_utilities::Arguments)
    ->Args({1024, 1000000, 4})
    ->Args({1024, 10000000, 4})
    ->UseManualTime()
    ->Repetitions(10)
    //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);


BENCHMARK_REGISTER_F(DegridOperatorDirectFixtureMPI, Apply)
    //->Apply(b_utilities::Arguments)
    ->Args({1024, 1000000, 4})
    ->Args({1024, 10000000, 4})
    ->UseManualTime()
    ->Repetitions(10)
    //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(DegridOperatorAdjointFixtureMPI, Apply)
    //->Apply(b_utilities::Arguments)
    ->Args({1024, 1000000, 4})
    ->Args({1024, 10000000, 4})
    ->UseManualTime()
    ->Repetitions(10)
    //->ReportAggregatesOnly(true)
    ->Unit(benchmark::kMillisecond);
    */
