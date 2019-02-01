#ifndef MEASUREMENT_OPERATOR_FACTORY_H
#define MEASUREMENT_OPERATOR_FACTORY_H

#include "purify/config.h"

#include "purify/types.h"
#include "purify/logging.h"

#include "purify/operators.h"
#include "purify/operators_gpu.h"
#include "purify/wproj_operators.h"
#include "purify/wproj_operators_gpu.h"

#ifdef PURIFY_MPI
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
#endif

namespace purify {
namespace factory {
//! determine type of distribute for mpi measurement operator
enum class distributed_measurement_operator {
  serial,
  mpi_distribute_image,
  mpi_distribute_grid,
  gpu_serial,
  gpu_mpi_distribute_image,
  gpu_mpi_distribute_grid
};

namespace {
template <class T>
void check_complex_for_gpu() {
  if (!std::is_same<Vector<t_complex>, T>::value)
    throw std::runtime_error("Arrayfire will only use complex type with Eigen.");
}
}  // namespace

//! distributed measurement operator factory
template <class T, class... ARGS>
std::shared_ptr<sopt::LinearTransform<T>> measurement_operator_factory(
    const distributed_measurement_operator distribute, ARGS &&... args) {
  switch (distribute) {
  case (distributed_measurement_operator::serial): {
    PURIFY_LOW_LOG("Using serial measurement operator.");
    return measurementoperator::init_degrid_operator_2d<T>(std::forward<ARGS>(args)...);
  }
  case (distributed_measurement_operator::gpu_serial): {
#ifndef PURIFY_ARRAYFIRE
    throw std::runtime_error("Tried to use GPU operator but you did not build with ArrayFire.");
#else
    check_complex_for_gpu<T>();
    PURIFY_LOW_LOG("Using serial measurement operator with Arrayfire.");
    af::setDevice(0);
    return gpu::measurementoperator::init_degrid_operator_2d(std::forward<ARGS>(args)...);
#endif
  }
#ifdef PURIFY_MPI
  case (distributed_measurement_operator::mpi_distribute_image): {
    auto const world = sopt::mpi::Communicator::World();
    PURIFY_LOW_LOG("Using distributed image MPI measurement operator.");
    return measurementoperator::init_degrid_operator_2d<T>(world, std::forward<ARGS>(args)...);
  }
  case (distributed_measurement_operator::mpi_distribute_grid): {
    auto const world = sopt::mpi::Communicator::World();
    PURIFY_LOW_LOG("Using distributed grid MPI measurement operator.");
    return measurementoperator::init_degrid_operator_2d_mpi<T>(world, std::forward<ARGS>(args)...);
  }
  case (distributed_measurement_operator::gpu_mpi_distribute_image): {
#ifndef PURIFY_ARRAYFIRE
    throw std::runtime_error("Tried to use GPU operator but you did not build with ArrayFire.");
#else
    check_complex_for_gpu<T>();
    auto const world = sopt::mpi::Communicator::World();
    PURIFY_LOW_LOG("Using distributed image MPI + Arrayfire measurement operator.");
    af::setDevice(0);
    return gpu::measurementoperator::init_degrid_operator_2d(world, std::forward<ARGS>(args)...);
#endif
  }
  case (distributed_measurement_operator::gpu_mpi_distribute_grid): {
#ifndef PURIFY_ARRAYFIRE
    throw std::runtime_error("Tried to use GPU operator but you did not build with ArrayFire.");
#else
    check_complex_for_gpu<T>();
    auto const world = sopt::mpi::Communicator::World();
    PURIFY_LOW_LOG("Using distributed grid MPI + Arrayfire measurement operator.");
    af::setDevice(0);
    return gpu::measurementoperator::init_degrid_operator_2d_mpi(world,
                                                                 std::forward<ARGS>(args)...);
#endif
  }
#endif
  default:
    throw std::runtime_error(
        "Distributed method not found for Measurement Operator. Are you sure you compiled with "
        "MPI?");
  }
};

}  // namespace factory
}  // namespace purify
#endif
