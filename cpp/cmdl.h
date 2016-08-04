#ifndef CMLD_H
#define CMLD_H

#include <string>
#include <cstdlib>
#include "types.h"
#include "casacore.h"
#include <getopt.h>

namespace purify {

  struct Params {
  std::string sopt_logging_level = "debug";
  std::string name = "";
  std::string weighting = "whiten";
  std::string stokes = "I";
  const purify::casa::MeasurementSet::ChannelWrapper::polarization stokes_val = purify::casa::MeasurementSet::ChannelWrapper::polarization::I;

  std::string visfile = "";
  std::string noisefile = "";


  t_int niters = 0;
  t_real beta = 1e-3;
  //measurement operator stuff
  t_real over_sample = 2;
  std::string kernel = "kb";
  t_int J = 4;
  t_int width = 512;
  t_int height = 512;
  t_real cellsizex = 0;
  t_real cellsizey = 0;
  t_real n_mu = 1.2;
  t_int iter = 0;
  t_real upsample_ratio = 3./2.;
  t_int power_method_iterations = 20;
  std::string primary_beam = "none";
  bool fft_grid_correction = false;
  //w_term stuff
  t_real energy_fraction = 1;
  bool use_w_term = false;

  bool update_output = false; //save output after each iteration
  bool adapt_gamma = true; //update gamma/stepsize
  bool run_diagnostic = false; //save and output diagnostic information
  bool no_algo_update = false; //if to use lambda function to record/update algorithm variables
  bool no_reweighted = true; //if to use reweighting
  };


  static struct option long_options[] =
        {
          /* These options set a flag. */
          //{"verbose", no_argument,       &verbose_flag, 1},
          /* These options don’t set a flag.
             We distinguish them by their indices. */
          {"help", no_argument, 0, 'z'},
          {"measurement_set",     required_argument,       0, 'a'},
          {"noise",  required_argument,       0, 'b'},
          {"name",  required_argument, 0, 'c'},
          {"niters",  required_argument, 0, 'd'},
          {"size",    required_argument, 0, 'f'},
          {"update",    no_argument, 0, 'e'},
          {"beta",    required_argument, 0, 'g'},
          {"noadapt",    no_argument, 0, 'h'},
          {"n_mean",    required_argument, 0, 'i'},
          {"diagnostic",    no_argument, 0, 'j'},
          {"power_iterations", required_argument, 0, 'k'},
          {"use_w_term", no_argument, 0, 'l'},
          {"energy_fraction", required_argument, 0, 'm'},
          {"primary_beam", required_argument, 0, 'n'},
          {"fft_grid_correction", no_argument, 0, 'o'},
          {"width", required_argument, 0, 'p'},
          {"height", required_argument, 0, 'q'},
          {"kernel", required_argument, 0, 'r'},
          {"kernel_support", required_argument, 0, 's'},
          {"logging_level", required_argument, 0, 't'},
          {0, 0, 0, 0}
        };

  std::string usage();

  Params parse_cmdl(int argc, char **argv);
}
#endif
