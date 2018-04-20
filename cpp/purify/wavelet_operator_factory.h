#ifndef WAVELET_OPERATOR_FACTORY_H
#define WAVELET_OPERATOR_FACTORY_H

#include <vector>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
namespace purify {
  namespace factory {
//! construct distributed sara wavelet operator
#ifdef PURIFY_MPI
template <class T>
std::shared_ptr<sopt::LinearTransform<T>>
wavelet_operator_factory(const sopt::mpi::Communicator &comm, const std::vector<std::tuple<std::string, t_uint>> & wavelets,
    const t_uint imsizey, const t_uint imsizex){
  const auto sara = sopt::wavelets::SARA(wavelets.begin(), wavelets.end());
const auto distribute_sara = sopt::wavelets::distribute_sara(sara, comm);
return  std::make_shared<sopt::LinearTransform<T>>(sopt::linear_transform<typename T::Scalar>(distribute_sara, imsizey, imsizex, comm));
}
#endif
//! construct sara wavelet operator
template <class T>
std::shared_ptr<sopt::LinearTransform<T>>
wavelet_operator_factory(const std::vector<std::tuple<std::string, t_uint>> & wavelets,
    const t_uint imsizey, const t_uint imsizex){
  const auto sara = sopt::wavelets::SARA(wavelets.begin(), wavelets.end());
  return  std::make_shared<sopt::LinearTransform<T>>(sopt::linear_transform<typename T::Scalar>(sara, imsizey, imsizex));
}
}
}
#endif
