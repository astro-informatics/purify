#ifndef WAVELET_OPERATOR_FACTORY_H
#define WAVELET_OPERATOR_FACTORY_H
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/session.h>
namespace purify {
//! construct distributed sara wavelet operator
#ifdef PURIFY_MPI
std::shared_ptr<sopt::LinearTransform<T>>
wavelet_operator_factory(const sopt::mpi::Communicator &comm, const std::vector<std::tuple<std::string, t_uint>> & wavelets,
    const t_uint imsizey, const t_uint imsizex){
auto sara = sopt::wavelets::distribute_sara(sopt::wavelets::SARA(wavelets), comm);
return  sopt::linear_transform<t_complex>(sara, imsizey, imsizex, comm);
}
#endif
//! construct sara wavelet operator
std::shared_ptr<sopt::LinearTransform<T>>
wavelet_operator_factory(const std::vector<std::tuple<std::string, t_uint>> & wavelets,
    const t_uint imsizey, const t_uint imsizex){
  auto sara = sopt::wavelets::SARA(wavelets);
  return  sopt::linear_transform<t_complex>(sara, imsizey, imsizex);
}
}
#endif
