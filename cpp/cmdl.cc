#include "purify/config.h"
#include "cmdl.h"

namespace purify {

std::string usage() {
  return "Purify: A program that will reconstruct images from radio interferometers using PADMM. "
         "\n\n"
         "--help: Print this information. \n\n"
         "--measurement_set: path of measurement set. (required) \n\n"
         "--noise: path of measurement_set with Stokes V noise estimate. (Assumes Stokes V of "
         "measurement set). \n\n"
         "--name: path of file output. (required) \n\n"
         "--niters: number of iterations. \n\n"
         "--stokes: choice of stokes I, Q, U, or V (I is default). \n\n"
         "--cellsize: the dimensions of a pixel in arcseconds. \n\n"
         "--size: image size in pixels. \n\n"
         "--width: image width in pixels. \n\n"
         "--height: image height in pixels. \n\n"
         "--beta: valued used to set the stepsize of PADMM\n\n"
         "--noadapt: Choose not to update the stepsize. \n\n"
         "--diagnostic: Save diagnostic information to log file.\n\n"
         "--l2_bound: Factor to multiply the l2 bound by.\n\n"
         "--relative_variation: The convergence criteria on relative variation (default is 1e-3).\n\n"
         "--residual_convergence: Factor to multiply the l2 bound by for convergence. (default is 1)"
         "--relative_gamma_adapt: Relative difference criteria for adapting the stepsize gamma (default 0.01).\n\n"
         "--power_iterations: Maximum iterations for the power method.\n\n"
         "--primary_beam: Choice of primary beam model. (none is the only option).\n\n"
         "--fft_grid_correction: Choose calculate the gridding correction using an FFT rather than "
         "analytic formula. \n\n"
         "--kernel: Type of gridding kernel to use, kb, gauss, pswf, box. (kb is default) \n\n"
         "--kernel_support: Support of kernel in grid cells. (4 is the default) \n\n"
         "--logging_level: Determines the output logging level for sopt and purify. (\"debug\" is "
         "the default) \n\n";
}

Params parse_cmdl(int argc, char **argv) {
  Params params;
  opterr = 0;

  int c;

  while(1) {
    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long(argc, argv, "a:bc:defghijklmnopqrst", long_options, &option_index);

    /* Detect the end of the options. */
    if(c == -1)
      break;

    switch(c) {
    case 0:
      /* If this option set a flag, do nothing else now. */
      if(long_options[option_index].flag != 0)
        break;
      printf("option %s", long_options[option_index].name);
      if(optarg)
        printf(" with arg %s", optarg);
      printf("\n");
      break;

    case 'z':
      std::cout << usage();
      std::exit(0);
      break;

    case 'a':
      params.visfile = optarg;
      break;

    case 'b':
      params.noisefile = optarg;
      break;

    case 'c':
      params.name = optarg;
      break;

    case 'd':
      params.niters = std::stoi(optarg);
      break;
    
    case 'e':
      params.name = params.stokes;
      break;

    case 'f':
      params.width = std::stoi(optarg);
      params.height = std::stoi(optarg);
      break;

    case 'g':
      params.beta = std::stod(optarg);
      break;

    case 'h':
      params.adapt_gamma = false;
      break;

    case 'i':
      params.n_mu = std::stod(optarg);
      break;

    case 'j':
      params.run_diagnostic = true;
      break;

    case 'k':
      params.power_method_iterations = std::stoi(optarg);
      if(params.power_method_iterations < 0)
        params.power_method_iterations = 1;
      break;

    case 'l':
      params.use_w_term = true;
      break;

    case 'm':
      params.energy_fraction = std::stod(optarg);
      if(params.energy_fraction <= 0 or params.energy_fraction > 1) {
        std::printf("Wrong energy fraction! %f", params.energy_fraction);
        params.energy_fraction = 1;
      }
      break;

    case 'n':
      params.primary_beam = optarg;
      break;

    case 'o':
      params.fft_grid_correction = true;
      break;

    case 'p':
      params.width = std::stoi(optarg);
      break;

    case 'q':
      params.height = std::stoi(optarg);
      break;

    case 'r':
      params.kernel = optarg;
      break;

    case 's':
      params.J = std::stoi(optarg);
      break;

    case 't':
      params.sopt_logging_level = optarg;
break;

    case 'u':
      params.cellsizex = std::stod(optarg);
      params.cellsizey = std::stod(optarg);
      break;

    case 'v':
      params.relative_variation = std::stod(optarg);
      break;

    case 'w':
      params.residual_convergence = std::stod(optarg);
      break;

    case 'x':
      params.relative_gamma_adapt = std::stod(optarg);
      break;

    case 'y':
      params.adapt_iter = std::stod(optarg);
      break;

    case '1':
      params.fftw_plan = optarg;
      break;

    case '?':
      /* getopt_long already printed an error message. */
      break;

    default:
      abort();
    }
  }
  return params;
}
};
