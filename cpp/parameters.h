#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "purify/config.h"
#include <string>
#include <vector>
#include "purify/casacore.h"
#include "purify/types.h"
namespace purify {

enum algorithm { padmm, prima_dual, sdmm };
struct Parameters {
  // logging levels
  std::string sopt_logging_level = "debug";
  std::string purify_logging_level = "debug";
  // reading data
  std::string obs_name = "";
  purify::casa::MeasurementSet::ChannelWrapper::polarization stokes_val
      = purify::casa::MeasurementSet::ChannelWrapper::polarization::I;

  std::string visfile = "";

  t_uint channel_averaging = 0;
  // measurement operator stuff
  t_real oversample_ratio = 2;
  std::string kernel = "kb";
  t_uint J = 4;
  t_uint image_width = 512;
  t_uint image_height = 512;
  t_real cellsizex = 0;
  t_real cellsizey = 0;
  std::string fftw_plan = "measure";
  // w_term stuff
  t_real energy_kernel_fraction = 1;
  t_real energy_chirp_fraction = 1;
  bool use_w_term = false;
  // SARA stuff
  std::shared_ptr<sopt::wavelets::SARA> wavelet_dictionary = std::make_shared<sopt::wavelets::SARA>(
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u), std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u));
  // power method stuff
  t_real power_method_tolerance = 1e-4;
  t_int power_method_iterations = 100;
  // algorithm
  purify::algorithm sopt_algorithm = algorithm::padmm;
  // convergence information
  t_real n_mu = 1.4;                // Factor to multiply scale the l2 bound by
  t_int iter = 0;                   // number of iterations, 0 means unlimited
  t_real relative_variation = 5e-3; // relative difference in model for convergence
  t_real residual_convergence = 1;  // max l2 norm reisudals can have for convergence, -1 means it
                                    // will choose epsilon by default
  t_int niters = 0;
  t_real beta = 1e-3;
  t_real epsilon = 0;
  // constraints
  bool positive_valued = true;
  bool real_valued = true;
// mpi
#ifdef PURIFY_MPI
  sopt::mpi::Communicator comm;
#endif
};

Parameters get_parameters() {
  Parameters params;

#ifdef PURIFY_MPI
  params.comm = sopt::mpi::Communicator::World();
#endif
  return params;
}

#endif
