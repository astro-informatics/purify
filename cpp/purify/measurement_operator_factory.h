
#include "purify/config.h"
#include "purify/logging.h"

#include "purify/operators.h"
#include "purify/operators_gpu.h"

#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>

namespace purify {
enum class distributed_type {serial, mpi_distribute_image, mpi_distribute_grid};
//!
template <class T, class... ARGS>
std::shared_ptr<sopt::LinearTransform<T>>
measurement_operator_factory(const distributed_type distribute, const bool arrayfire,
 ARGS &&... args){
  if(distribute != distributed_type::serial)
    throw std::runtime_error("Not using mpi, but asking for an mpi measurement operator!");
  if(!arrayfire)
    return measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
       std::forward<ARGS>(args)...);
  else{
    /*
    af::setDevice(0);
    return gpu::measurementoperator::init_degrid_operator_2d(
       std::forward<ARGS>(args)...);
       */
  }
}
#ifdef PURIFY_MPI

template <class T, class... ARGS>
std::shared_ptr<sopt::LinearTransform<T>>
measurement_operator_factory(const sopt::mpi::Communicator &world, const distributed_type distribute, const bool arrayfire,
 ARGS &&... args){
  switch(distribute){
    case(distributed_type::serial):{
      return measurement_operator_factory<Vector<t_complex>>(arrayfire, std::forward<ARGS>(args)...);
      break;
    }
    case(distributed_type::mpi_distribute_image):{
      if(!arrayfire){
      return measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        world, std::forward<ARGS>(args)...);
  }else{
    /*
  af::setDevice(0);
  return gpu::measurementoperator::init_degrid_operator_2d(
      world, std::forward<ARGS>(args)...);
      */
  }
      break;
    }
    case(distributed_type::mpi_distribute_grid):{
      if(!arrayfire){
    return measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
      world, std::forward<ARGS>(args)...);
      }else{
 /*
      af::setDevice(0);
  return gpu::measurementoperator::init_degrid_operator_2d_mpi(
      world, std::forward<ARGS>(args)...);
      */
      }
  }
}
#endif
}
