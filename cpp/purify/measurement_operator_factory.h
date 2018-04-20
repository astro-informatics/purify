#ifndef MEASUREMENT_OPERATOR_FACTORY_H
#define MEASUREMENT_OPERATOR_FACTORY_H

#include "purify/config.h"

#include "purify/logging.h"
#include "purify/types.h"

#include "purify/operators.h"
#include "purify/operators_gpu.h"

#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>

namespace purify {
  namespace factory {
  //! determine type of distribute for mpi measurement operator
enum class distributed_type {serial, mpi_distribute_image, mpi_distribute_grid};

//! Non-distributed measurement operator factory
template <class T, class... ARGS>
std::shared_ptr<sopt::LinearTransform<T> const>
measurement_operator_factory( const distributed_type distribute, ARGS &&... args){
    PURIFY_LOW_LOG("Using serial measurement operator.");
    if(distribute == distributed_type::serial)
     return measurementoperator::init_degrid_operator_2d<T>(std::forward<ARGS>(args)...);
    else
      throw std::runtime_error("Asking for factory to build distirbuted measurement operator, but not using MPI. Add communicator to factory.");
}


//! Non-distributed measurement operator factory with arrayfire
template <class T, class... ARGS>
std::shared_ptr<sopt::LinearTransform<T> const>
measurement_operator_factory_gpu(const distributed_type distribute, ARGS &&... args){
    PURIFY_LOW_LOG("Using serial measurement operator with Arrayfire.");
#ifndef PURIFY_PURIFY_ARRAYFIRE
  throw std::runtime_error("Tried to use GPU operator but arrayfire is not usable.");
#else
  if(distribute == distributed_type::serial){
    if(!std::is_same<Vector<t_complex>, T>::value)
      throw std::runtime_error("Arrayfire will only use complex type with Eigen.");
    af::setDevice(0);
    return gpu::measurementoperator::init_degrid_operator_2d(std::forward<ARGS>(args)...);
  }
    else
#endif
      throw std::runtime_error("Asking for factory to build distirbuted measurement operator with arrayfire, but not using MPI. Add communicator to factory.");
}

#ifdef PURIFY_MPI

//! distributed measurement operator factory
template <class T, class... ARGS>
std::shared_ptr<sopt::LinearTransform<T> const>
measurement_operator_factory(const sopt::mpi::Communicator &world, const distributed_type distribute, ARGS &&... args){
  switch(distribute){
    case(distributed_type::serial):{
      if(world.size() > 1)
        throw std::runtime_error("Using serial algorithm with MPI, using " + std::to_string(world.size()) + " > 1 nodes.");
      return measurement_operator_factory<T>(distribute, std::forward<ARGS>(args)...);
    }
    case(distributed_type::mpi_distribute_image):{
      PURIFY_LOW_LOG("Using distributed image MPI measurement operator.");
      return measurementoperator::init_degrid_operator_2d<T>(world, std::forward<ARGS>(args)...);
    }
    case(distributed_type::mpi_distribute_grid):{
      PURIFY_LOW_LOG("Using distributed grid MPI measurement operator.");
      return measurementoperator::init_degrid_operator_2d_mpi<T>(world, std::forward<ARGS>(args)...);
  }
    default:
      throw std::runtime_error("Distributed method not found for Measurement Operator.");
}
}

//! distributed measurement operator with arrayfire factory
template <class T, class... ARGS>
std::shared_ptr<sopt::LinearTransform<T> const>
measurement_operator_factory_gpu(const sopt::mpi::Communicator &world, const distributed_type distribute, ARGS &&... args){
#ifndef PURIFY_PURIFY_ARRAYFIRE
throw std::runtime_error("Tried to use GPU operator but arrayfire is not usable.");
#else
  if(!std::is_same<Vector<t_complex>, T>::value)
      throw std::runtime_error("Arrayfire will only use complex type with Eigen.");
  switch(distribute){
    case(distributed_type::serial):{
      if(world.size() > 1)
        throw std::runtime_error("Using serial algorithm with MPI + Arrayfire, using " + std::to_string(world.size()) + " > 1 nodes.");
      return measurement_operator_factory_gpu<T>(distribute, std::forward<ARGS>(args)...);
    }
    case(distributed_type::mpi_distribute_image):{
      PURIFY_LOW_LOG("Using distributed image MPI + Arrayfire measurement operator.");
     af::setDevice(0);
     return gpu::measurementoperator::init_degrid_operator_2d(world, std::forward<ARGS>(args)...);
    }
    case(distributed_type::mpi_distribute_grid):{
      PURIFY_LOW_LOG("Using distributed grid MPI + Arrayfire measurement operator.");
    af::setDevice(0);
    return gpu::measurementoperator::init_degrid_operator_2d_mpi(world, std::forward<ARGS>(args)...);
      }
    default:
      throw std::runtime_error("Distributed method not found for Measurement Operator.");
  }
#endif
}
#endif
}
}
#endif
