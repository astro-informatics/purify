#ifndef CMLD_H
#define CMLD_H

#include "purify/config.h"
#include <cstdlib>
#include <getopt.h>
#include <string>
#include "purify/casacore.h"
#include "purify/types.h"

namespace purify {

struct Params {
  std::string sopt_logging_level = "debug";
  std::string name = "";
  std::string weighting = "natural";
  std::string stokes = "I";
  purify::casa::MeasurementSet::ChannelWrapper::polarization stokes_val
      = purify::casa::MeasurementSet::ChannelWrapper::polarization::I;

  std::string visfile = "";
  std::string noisefile = "";

  t_int niters = 0;
  t_real beta = 1e-3;
  // measurement operator stuff
  t_real over_sample = 2;
  std::string kernel = "kb";
  t_int J = 4;
  t_int width = 512;
  t_int height = 512;
  t_real cellsizex = 0;
  t_real cellsizey = 0;
  t_real upsample_ratio = 1;
  std::string primary_beam = "none";
  bool fft_grid_correction = false;
  std::string fftw_plan = "measure";
  // w_term stuff
  t_real energy_fraction = 1;
  bool use_w_term = false;
  //adapting the algorithm
  bool update_output = false;  // save output after each iteration
  bool adapt_gamma = true;     // update gamma/stepsize
  bool run_diagnostic = false; // save and output diagnostic information
  bool algo_update = true;     // if to use lambda function to record/update algorithm variables
  bool no_reweighted = true;   // if to use reweighting
  t_real relative_gamma_adapt = 0.01;
  t_int adapt_iter = 100;
  // flux scaling
  t_real norm = 1; // norm of the measurement operator
  t_real psf_norm = 1; // the peak value of the PSF
  t_int power_method_iterations = 100; // number of power method iterations for setting the flux scale

  //convergence information
  t_real n_mu = 1.4; //Factor to multiply scale the l2 bound by
  t_int iter = 0; // number of iterations, 0 means unlimited
  t_real relative_variation = 5e-3; // relative difference in model for convergence
  t_real residual_convergence = 1; // max l2 norm reisudals can have for convergence, -1 means it will choose epsilon by default
  t_real epsilon = 0;
};

static struct option long_options[] = {
    /* These options set a flag. */
    //{"verbose", no_argument,       &verbose_flag, 1},
    /* These options don’t set a flag.
       We distinguish them by their indices. */
    {"help", no_argument, 0, 'z'},
    {"measurement_set", required_argument, 0, 'a'},
    {"noise", required_argument, 0, 'b'},
    {"name", required_argument, 0, 'c'},
    {"niters", required_argument, 0, 'd'},
    {"stokes", required_argument, 0, 'e'},
    {"size", required_argument, 0, 'f'},
    {"beta", required_argument, 0, 'g'},
    {"noadapt", no_argument, 0, 'h'},
    {"l2_bound", required_argument, 0, 'i'},
    {"diagnostic", no_argument, 0, 'j'},
    {"power_iterations", required_argument, 0, 'k'},
    {"primary_beam", required_argument, 0, 'n'},
    {"fft_grid_correction", no_argument, 0, 'o'},
    {"width", required_argument, 0, 'p'},
    {"height", required_argument, 0, 'q'},
    {"kernel", required_argument, 0, 'r'},
    {"kernel_support", required_argument, 0, 's'},
    {"logging_level", required_argument, 0, 't'},
    {"cellsize", required_argument, 0, 'u'},
    {"relative_variation", required_argument, 0, 'v'},
    {"residual_convergence", required_argument, 0, 'w'},
    {"relative_gamma_adapt", required_argument, 0, 'x'},
    {"adapt_iter", required_argument, 0, 'y'},
    {"fftw_plan", required_argument, 0, '1'},
    {0, 0, 0, 0}};

std::string usage();

Params parse_cmdl(int argc, char **argv);
}
#endif
