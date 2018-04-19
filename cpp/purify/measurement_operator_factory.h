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
  //! determine type of distribute for mpi measurement operator
enum class distributed_type {serial, mpi_distribute_image, mpi_distribute_grid};
//!
template <class T, class... ARGS>
std::shared_ptr<sopt::LinearTransform<T> const>
measurement_operator_factory(const distributed_type distribute, const bool arrayfire,
 ARGS &&... args){
  if(distribute != distributed_type::serial)
    throw std::runtime_error("Not using mpi, but asking for an mpi measurement operator!");
  if(!arrayfire)
    return measurementoperator::init_degrid_operator_2d<T>(std::forward<ARGS>(args)...);
  else{
#ifndef PURIFY_PURIFY_ARRAYFIRE
throw std::runtime_error("Tried to use GPU operator but arrayfire was not used when compiling.");
#else
    if(!std::is_same<Vector<t_complex>, T>::value)
      throw std::runtime_error("Arrayfire will only use complex type with Eigen.");
    af::setDevice(0);
    return gpu::measurementoperator::init_degrid_operator_2d(std::forward<ARGS>(args)...);
#endif
  }
}
#ifdef PURIFY_MPI

template <class T, class... ARGS>
std::shared_ptr<sopt::LinearTransform<T> const>
measurement_operator_factory(const sopt::mpi::Communicator &world, const distributed_type distribute, const bool arrayfire,
 ARGS &&... args){
  switch(distribute){
    case(distributed_type::serial):{
      return measurement_operator_factory<T>(distribute, arrayfire, std::forward<ARGS>(args)...);
      break;
    }
    case(distributed_type::mpi_distribute_image):{
      if(!arrayfire){
      return measurementoperator::init_degrid_operator_2d<T>(
        world, std::forward<ARGS>(args)...);
  }else{
#ifndef PURIFY_PURIFY_ARRAYFIRE
throw std::runtime_error("Tried to use GPU operator but arrayfire was not used when compiling.");
#else
    if(!std::is_same<Vector<t_complex>, T>::value)
      throw std::runtime_error("Arrayfire will only use complex type with Eigen.");
  af::setDevice(0);
  return gpu::measurementoperator::init_degrid_operator_2d(
      world, std::forward<ARGS>(args)...);
#endif
  }
      break;
    }
    case(distributed_type::mpi_distribute_grid):{
      if(!arrayfire){
    return measurementoperator::init_degrid_operator_2d_mpi<T>(
      world, std::forward<ARGS>(args)...);
      }else{
#ifndef PURIFY_PURIFY_ARRAYFIRE
throw std::runtime_error("Tried to use GPU operator but arrayfire was not used when compiling.");
#else
    if(!std::is_same<Vector<t_complex>, T>::value)
      throw std::runtime_error("Arrayfire will only use complex type with Eigen.");
    af::setDevice(0);
    return gpu::measurementoperator::init_degrid_operator_2d_mpi(
      world, std::forward<ARGS>(args)...);
#endif
      }
  }
}
#endif
}

}
#endif
