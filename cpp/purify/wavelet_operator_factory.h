#ifndef WAVELET_OPERATOR_FACTORY_H
#define WAVELET_OPERATOR_FACTORY_H

#include "purify/config.h"

#include "purify/logging.h"
#include "purify/types.h"

#include <vector>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#ifdef PURIFY_MPI
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
#endif
namespace purify {
  namespace factory {
enum class distributed_wavelet_operator {serial, mpi_sara};
//! construct sara wavelet operator
template <class T>
std::shared_ptr<sopt::LinearTransform<T> const>
wavelet_operator_factory(const distributed_wavelet_operator distribute, const std::vector<std::tuple<std::string, t_uint>> & wavelets,
    const t_uint imsizey, const t_uint imsizex){
  const auto sara = sopt::wavelets::SARA(wavelets.begin(), wavelets.end());
  switch(distribute){
    case(distributed_wavelet_operator::serial):{
      PURIFY_LOW_LOG("Using serial wavelet operator.");
      return  std::make_shared<sopt::LinearTransform<T>>(sopt::linear_transform<typename T::Scalar>(sara, imsizey, imsizex));
    }
#ifdef PURIFY_MPI
    case(distributed_wavelet_operator::mpi_sara):{
      auto const comm = sopt::mpi::Communicator::World();
      PURIFY_LOW_LOG("Using distributed image MPI wavelet operator.");
      auto const dsara = sopt::wavelets::distribute_sara(sara, comm);
      return std::make_shared<sopt::LinearTransform<T>>(sopt::linear_transform<typename T::Scalar>(dsara, imsizey, imsizex, comm));
    }
#endif
    default:
      throw std::runtime_error("Distributed method not found for Wavelet Operator. Are you sure you compiled with MPI?");
  }
}
}
}
#endif
