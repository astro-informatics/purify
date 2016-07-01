#include <array>
#include <random>
#include "directories.h"
#include <sopt/imaging_padmm.h>
#include "sopt/utilities.h"
#include "sopt/wavelets.h"
#include "sopt/wavelets/sara.h"
#include "pfitsio.h"
#include "types.h"
#include "MeasurementOperator.h"
#include <getopt.h>
#include <ctime>
//#include "clara.h"

int main(int argc, char **argv) {
  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();


  std::string name = "";
  std::string weighting = "whiten";
  std::string stokes = "I";

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
  t_real cellsize = 0;
  t_real n_mu = 1.2;
  t_int iter = 0;
  t_int power_method_iterations = 20;
  std::string primary_beam = "none";
  bool fft_grid_correction = false;
  //w_term stuff
  t_real energy_fraction = 1;
  bool use_w_term = false;

  bool update_output = false; //save output after each iteration
  bool adapt_gamma = true; //update gamma/stepsize
  bool run_diagnostic = false; //save and output diagnostic information
  bool no_algo_update = false;
  opterr = 0;

  int c;

  while (1)
    {
      static struct option long_options[] =
        {
          /* These options set a flag. */
          //{"verbose", no_argument,       &verbose_flag, 1},
          /* These options don’t set a flag.
             We distinguish them by their indices. */
          {"help", no_argument, 0, 'z'},
          {"vis",     required_argument,       0, 'a'},
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
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long(argc, argv, "a:bc:defghijklmno",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          printf ("option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s", optarg);
          printf ("\n");
          break;
        case 'z':
          printf("Purify: A program will reconstruct images from radio interferometers using PADMM. \n\n");
          printf("--help: Print this information. \n\n");
          printf("--vis: path of file with visibilities. (required) \n\n");
          printf("--noise: path of file with visibilities noise estimate (Stokes V). \n\n");
          printf("--name: path of file output. (required) \n\n");
          printf("--niters: number of iterations. \n\n");
          printf("--size: image size in pixels. \n\n");
          printf("--update: Switch to allow updating input and output information of PADMM while it is running. \n\n");
          printf("--beta: valued used to set the stepsize of PADMM\n\n");
          printf("--noadapt: Choose not to update the stepsize. \n\n");
          printf("--diagnostic: Save diagnostic information to log file.\n\n");
          printf("--n_mean: Factor to multiply the l2 bound by.\n\n");
          printf("--power_iterations: Maximum iterations for the power method.\n\n");
          printf("--use_w_term: Choose to include the w-projection method (only for small data sets).\n\n");
          printf("--energy_fraction: How sparse the chirp matrix (w-projection) should be.\n\n");
          printf("--primary_beam: Choice of primary beam model. (none is the only option).\n\n");
          printf("--fft_grid_correction: Choose calculate the gridding correction using an FFT rather than analytic formula. \n\n");

          return 0;
          break;
        case 'a':
          visfile = optarg;
          break;

        case 'b':
          noisefile = optarg;
          break;

        case 'c':
          name = optarg;
          break;

        case 'd':
          niters = std::stoi(optarg);
          break;

        case 'f':
          width = std::stoi(optarg);
          height = std::stoi(optarg);
          break;

        case 'e':
          update_output = true;
          break;

        case 'g':
          beta = std::stod(optarg);
          break;

        case 'h':
          adapt_gamma = false;
          break;

        case 'i':
          n_mu = std::stod(optarg);
          break;

        case 'j':
          run_diagnostic = true;
          break;

        case 'k':
          power_method_iterations = std::stoi(optarg);
          if (power_method_iterations < 0)
              power_method_iterations = 1;
          break;

        case 'l':
          use_w_term = true;
          break;

        case 'm':
          energy_fraction = std::stod(optarg);
          if (energy_fraction <= 0 or energy_fraction > 1)
          {
            std::printf("Wrong energy fraction! %f", energy_fraction);
            energy_fraction = 1;
          }
          break;

        case 'n':
          primary_beam = optarg;
          break;

        case 'o':
          fft_grid_correction = true;
          break;

        case '?':
          /* getopt_long already printed an error message. */
          break;


        default:
          abort ();
        }
    }



  auto uv_data = utilities::read_visibility(visfile);
  t_real const max_u = std::sqrt((uv_data.u.array() * uv_data.u.array() + uv_data.v.array() * uv_data.v.array()).maxCoeff());
  uv_data.units = "lambda";
  if (cellsize  == 0)
    cellsize = (180 * 3600) / max_u / purify_pi / 3;


  //header information
  pfitsio::header_params header;
  header.mean_frequency = 1381.67703151703;
  header.ra = 0;
  header.dec = 0;
  header.cell_x = cellsize;
  header.cell_y = cellsize;


  std::string const dirty_image_fits = output_filename(name + "_dirty_"+ weighting + ".fits");
  std::string const psf_fits = output_filename(name + "_psf_"+ weighting + ".fits");

  MeasurementOperator measurements(uv_data, J, J, kernel, width, height, power_method_iterations, 
    over_sample, cellsize, cellsize, "none", 0, use_w_term, energy_fraction, 
    primary_beam, fft_grid_correction);
  
  //calculate weights outside of measurement operator
  uv_data.weights = utilities::init_weights(uv_data.u, uv_data.v, 
      uv_data.weights, over_sample, 
      weighting, 0, over_sample * width, over_sample * height);

  //Read in visibilities for noise estimate
    t_real sigma_real = 1/std::sqrt(2);
    t_real sigma_imag = 1/std::sqrt(2);

  if (noisefile != "")
    {  
      auto const noise_uv_data = utilities::read_visibility(noisefile);
      Vector<t_complex> const noise_vis = uv_data.weights.array() * noise_uv_data.vis.array();

      sigma_real = utilities::median(noise_vis.real().cwiseAbs())/0.6745;
      sigma_imag = utilities::median(noise_vis.imag().cwiseAbs())/0.6745;
    }

  std::cout << "RMS noise of " << sigma_real << " Jy (real) and " << sigma_real  << " Jy (imaginary)" << '\n';


  auto direct = [&measurements, &width, &height](Vector<t_complex> &out, Vector<t_complex> const &x) {
        assert(x.size() == width * height);
        auto const image = Image<t_complex>::Map(x.data(), height, width);
        out = measurements.degrid(image);
  };
  auto adjoint = [&measurements, &width, &height](Vector<t_complex> &out, Vector<t_complex> const &x) {
        auto image = Image<t_complex>::Map(out.data(), height, width);
        image = measurements.grid(x);
  };
  auto measurements_transform = sopt::linear_transform<Vector<t_complex>>(
    direct, {0, 1, static_cast<t_int>(uv_data.vis.size())},
    adjoint, {0, 1, static_cast<t_int>(width * height)}
  );

  sopt::wavelets::SARA const sara{
        std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u), 
        std::make_tuple("DB3", 3u),  std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u), 
        std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};


  auto const Psi = sopt::linear_transform<t_complex>(sara, height, width);
  std::printf("Saving dirty map \n");
 

  Vector<> dimage = (measurements_transform.adjoint() * (uv_data.weights.array() * uv_data.vis.array()).matrix()).real();
  t_real max_val = dimage.array().abs().maxCoeff();
  
  header.fits_name = dirty_image_fits;
  pfitsio::write2d_header(Image<t_real>::Map(dimage.data(), height, width), header);

  
  Vector<> psf = (measurements_transform.adjoint() * (uv_data.weights.array()).matrix()).real();
  max_val = psf.array().abs().maxCoeff();
  psf = psf / max_val;
  header.fits_name = psf_fits;

  pfitsio::write2d_header(Image<t_real>::Map(psf.data(), height, width), header);
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  Vector<t_complex> initial_residuals = Vector<t_complex>::Zero(uv_data.vis.size());

  t_real const noise_rms = std::sqrt(sigma_real * sigma_real + sigma_imag * sigma_imag)/std::sqrt(2);
  t_real const W_norm = uv_data.weights.array().abs().maxCoeff();
  t_real epsilon = n_mu * std::sqrt(2 * uv_data.vis.size()) * noise_rms; //Calculation of l_2 bound following SARA paper


  t_real purify_gamma = (Psi.adjoint() * (measurements_transform.adjoint() * (uv_data.weights.array() * uv_data.vis.array()).matrix())).cwiseAbs().maxCoeff() * beta;

  std::ofstream out_diagnostic(name + "_diagnostic");
  out_diagnostic.precision(13);


  std::cout << "Starting sopt!" << '\n';
  std::cout << "Epsilon = " << epsilon << '\n';
  std::cout << "Gamma = " << purify_gamma << '\n';
  auto padmm = sopt::algorithm::ImagingProximalADMM<t_complex>(uv_data.vis)
    .gamma(purify_gamma)
    .relative_variation(1e-3)
    .l2ball_proximal_epsilon(epsilon)
    .l2ball_proximal_weights(uv_data.weights.array().real())
    .tight_frame(false)
    .l1_proximal_tolerance(1e-3)
    .l1_proximal_nu(1)
    .l1_proximal_itermax(100)
    .l1_proximal_positivity_constraint(true)
    .l1_proximal_real_constraint(true)
    .residual_convergence(epsilon)
    .lagrange_update_scale(0.9)
    .nu(1e0)
    .Psi(Psi)
    .Phi(measurements_transform);
  std::tuple<Vector<t_complex>, Vector<t_complex>> const estimates(initial_estimate, initial_residuals); 
  std::clock_t c_start = std::clock();
  auto algorithm_update  = [&padmm, &iter, &c_start, &update_output, &adapt_gamma, &header, &beta, &Psi, &measurements, &name, &weighting, &uv_data, &run_diagnostic, &out_diagnostic](Vector<t_complex> const & x){
    
    //diagnostic variables
    t_real rms = 0;
    t_real dr = 0;
    t_real max = 0;
    t_real min = 0;
    std::clock_t c_end = std::clock();
    auto total_time = (c_end-c_start) / CLOCKS_PER_SEC; // total time for solver to run in seconds
    //Getting things ready for l1 and l2 norm calculation
    Image<t_complex> const image = Image<t_complex>::Map(x.data(), measurements.imsizey, measurements.imsizex);
    Vector<t_complex> const y_residual = ((uv_data.vis - measurements.degrid(image)).array() * uv_data.weights.array().real());
    t_real const l2_norm = y_residual.stableNorm();
    Vector<t_complex> const alpha = Psi.adjoint() * x;
    t_real const l1_norm = alpha.lpNorm<1>();
    if (update_output){
      std::string const outfile_fits = output_filename(name + "_solution_"+ weighting + "_update.fits");
      std::string const residual_fits = output_filename(name + "_residual_"+ weighting + "_update.fits");

      // header information
      header.pix_units = "JY/PIXEL";
      header.fits_name = outfile_fits;
      pfitsio::write2d_header(image.real(), header);
      
      Image<t_complex> residual = measurements.grid(y_residual).array();
      header.pix_units = "JY/BEAM";
      header.fits_name = residual_fits;
      pfitsio::write2d_header(residual.real(), header);
      
      rms = utilities::standard_deviation(Image<t_complex>::Map(residual.data(), residual.size(), 1));
      dr = utilities::dynamic_range(image, residual);
      max = residual.matrix().real().maxCoeff();
      min = residual.matrix().real().minCoeff();
    }
    //setting information for updating parameters

    auto new_purify_gamma = alpha.cwiseAbs().maxCoeff() * beta;
    auto relative_gamma = std::abs(new_purify_gamma - padmm.gamma())/padmm.gamma();
    std::cout << "Relative gamma = " << relative_gamma << '\n';
    std::cout << "Old gamma = " << padmm.gamma() << '\n';
    std::cout << "New gamma = " << new_purify_gamma << '\n';
    if (relative_gamma > 0.01 and new_purify_gamma > 0 and adapt_gamma)
    {
      padmm.gamma(new_purify_gamma);
    }

    if (run_diagnostic)
    {
      if (iter == 0)
        out_diagnostic << "i Gamma RelativeGamma DynamicRange RMS(Res) Max(Res) Min(Res) l1_norm l2_norm Time(sec)" << std::endl;
      out_diagnostic << iter << " ";
      out_diagnostic << new_purify_gamma << " ";
      out_diagnostic << relative_gamma << " ";
      out_diagnostic << dr << " ";
      out_diagnostic << rms << " ";
      out_diagnostic << max << " ";
      out_diagnostic << min << " ";
      out_diagnostic << l1_norm << " ";
      out_diagnostic << l2_norm << " ";
      out_diagnostic << total_time << " ";
      out_diagnostic << std::endl;
      std::cout << "i Gamma RelativeGamma DynamicRange RMS(Res) Max(Res) Min(Res) Time(sec)" << std::endl;
      std::cout << iter << " ";
      std::cout << new_purify_gamma << " ";
      std::cout << relative_gamma << " ";
      std::cout << dr << " ";
      std::cout << rms << " ";
      std::cout << max << " ";
      std::cout << min << " ";
      std::cout << l1_norm << " ";
      std::cout << l2_norm << " ";
      std::cout << total_time << " ";
      std::cout << std::endl;      
    }

    iter++;

    return false;
  };
  if (!no_algo_update)
    padmm.is_converged(algorithm_update);
  if (niters != 0)
    padmm.itermax(niters);
  auto const diagnostic = padmm(estimates);

  std::string const outfile_fits = output_filename(name + "_solution_"+ weighting + "_final.fits");
  std::string const residual_fits = output_filename(name + "_residual_"+ weighting + "_final.fits");
  Image<t_complex> const image = Image<t_complex>::Map(diagnostic.x.data(), measurements.imsizey, measurements.imsizex);
  // header information
  header.pix_units = "JY/PIXEL";
  header.fits_name = outfile_fits;
  pfitsio::write2d_header(image.real(), header);

  Image<t_complex> residual = measurements.grid(((uv_data.vis - measurements.degrid(image)).array() * uv_data.weights.array().real()).matrix() ).array();
  header.pix_units = "JY/BEAM";
  header.fits_name = residual_fits;
  pfitsio::write2d_header(residual.real(), header);

  out_diagnostic.close(); // closing diagnostic file

  return 0;
}
