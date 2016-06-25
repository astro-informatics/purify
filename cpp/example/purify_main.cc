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

int main(int argc, char **argv) {
  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();


  std::string name = "";
  std::string weighting = "whiten";
  std::string stokes = "I";

  std::string visfile = "";
  std::string noisefile = "";


  t_int niters = 1000;
  t_real beta = 1e-3;
  t_real over_sample = 2;
  std::string kernel = "kb";
  t_int J = 4;
  t_int width = 512;
  t_int height = 512;
  t_real cellsize = 0;

  bool update_output = false; //save output after each iteration
  bool adapt_gamma = true;
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
          {"vis",     required_argument,       0, 'a'},
          {"noise",  required_argument,       0, 'b'},
          {"name",  required_argument, 0, 'c'},
          {"niters",  required_argument, 0, 'd'},
          {"size",    required_argument, 0, 'f'},
          {"update",    no_argument, 0, 'e'},
          {"beta",    required_argument, 0, 'g'},
          {"noadapt",    no_argument, 0, 'h'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "abcdefg",
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

  MeasurementOperator measurements(uv_data, J, J, kernel, width, height, over_sample, cellsize, cellsize, weighting, 0);
  auto const noise_uv_data = utilities::read_visibility(noisefile);
  Vector<t_complex> const noise_vis = measurements.W.array() * noise_uv_data.vis.array();

  auto sigma_real = utilities::median(noise_vis.real().cwiseAbs())/0.6745;
  auto sigma_imag = utilities::median(noise_vis.imag().cwiseAbs())/0.6745;

  std::cout << "Stokes V RMS noise of " << sigma_real * 1e3 << " mJy and " << sigma_real * 1e3 << " mJy" << '\n';


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
        std::make_tuple("Dirac", 4u), std::make_tuple("DB1", 4u), std::make_tuple("DB2", 4u), 
        std::make_tuple("DB3", 4u),  std::make_tuple("DB4", 4u), std::make_tuple("DB5", 4u), 
        std::make_tuple("DB6", 4u), std::make_tuple("DB7", 4u), std::make_tuple("DB8", 4u)};


  auto const Psi = sopt::linear_transform<t_complex>(sara, height, width);
  std::printf("Saving dirty map \n");
  //const Vector<t_complex> weighted_data = (uv_data.vis.array() * measurements.W).matrix(); //whitening data
  const Vector<t_complex> & input = uv_data.vis.array();

  Vector<> dimage = (measurements_transform.adjoint() * input).real();
  t_real max_val = dimage.array().abs().maxCoeff();
  
  header.fits_name = dirty_image_fits;
  pfitsio::write2d_header(Image<t_real>::Map(dimage.data(), height, width), header);

  Vector<t_complex> const psf_vis = (input.array() * 0. + 1.).matrix();
  Vector<> psf = (measurements_transform.adjoint() * psf_vis).real();
  max_val = psf.array().abs().maxCoeff();
  psf = psf / max_val;
  header.fits_name = psf_fits;

  pfitsio::write2d_header(Image<t_real>::Map(psf.data(), height, width), header);
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  Vector<t_complex> initial_residuals = Vector<t_complex>::Zero(input.size());

  t_real const noise_rms = std::sqrt(sigma_real * sigma_real + sigma_imag * sigma_imag)/std::sqrt(2);
  t_real const n_sigma = 5;
  t_real const W_norm = measurements.W.abs().maxCoeff();
  t_real epsilon = utilities::calculate_l2_radius(input, noise_rms, n_sigma); //Calculation of l_2 bound following SARA paper


  t_real purify_gamma = (Psi.adjoint() * (measurements_transform.adjoint() * input)).cwiseAbs().maxCoeff() * beta;



  uv_data.weights = measurements.W;
  measurements.W = measurements.W.array() * 0 + 1;
  measurements.norm = 1;
  measurements.norm = std::sqrt(measurements.power_method(100)); //renormalise, because weights are not in the operator

  std::cout << "Starting sopt!" << '\n';
  std::cout << "Epsilon = " << epsilon << '\n';
  std::cout << "Gamma = " << purify_gamma << '\n';
  auto padmm = sopt::algorithm::ImagingProximalADMM<t_complex>(input)
    .itermax(niters)
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
    //
    .Phi(measurements_transform);
  std::tuple<Vector<t_complex>, Vector<t_complex>> const estimates(initial_estimate, initial_residuals); 
  
  auto algorithm_update  = [&padmm, &update_output, &adapt_gamma, &header, &beta, &Psi, &measurements, &name, &weighting, &uv_data](Vector<t_complex> const & x){
    if (update_output){
      std::string const outfile_fits = output_filename(name + "_solution_"+ weighting + "_update.fits");
      std::string const residual_fits = output_filename(name + "_residual_"+ weighting + "_update.fits");

      Image<t_complex> const image = Image<t_complex>::Map(x.data(), measurements.imsizey, measurements.imsizex);
      // header information
      header.pix_units = "JY/PIXEL";
      header.fits_name = outfile_fits;
      pfitsio::write2d_header(image.real(), header);

      Image<t_complex> residual = measurements.grid(((uv_data.vis - measurements.degrid(image)).array() * uv_data.weights.array().real()).matrix() ).array();
      header.pix_units = "JY/BEAM";
      header.fits_name = residual_fits;
      pfitsio::write2d_header(residual.real(), header);
    }
    //setting information for updating parameters

    auto new_purify_gamma = (Psi.adjoint() * x).cwiseAbs().maxCoeff() * beta;
    auto relative_gamma = std::abs(new_purify_gamma - padmm.gamma())/padmm.gamma();
    std::cout << "Relative gamma = " << relative_gamma << '\n';
    std::cout << "Old gamma = " << padmm.gamma() << '\n';
    std::cout << "New gamma = " << new_purify_gamma << '\n';
    if (relative_gamma > 0.01 and new_purify_gamma > 0 and adapt_gamma)
    {
      padmm.gamma(new_purify_gamma);
    }

    return false;
  };

  padmm.is_converged(algorithm_update);
  auto const diagnostic = padmm(estimates);

  std::string const outfile_fits = output_filename(name + "_solution_"+ weighting + "_final.fits");
  std::string const residual_fits = output_filename(name + "_residual_"+ weighting + "_final.fits");
  Image<t_complex> const image = Image<t_complex>::Map(diagnostic.x.data(), measurements.imsizey, measurements.imsizex);
  // header information
  header.pix_units = "JY/PIXEL";
  header.fits_name = outfile_fits;
  pfitsio::write2d_header(image.real(), header);

  Image<t_complex> residual = measurements.grid(((input - measurements.degrid(image)).array() * uv_data.weights.array().real()).matrix() ).array();
  header.pix_units = "JY/BEAM";
  header.fits_name = residual_fits;
  pfitsio::write2d_header(residual.real(), header);

  return 0;
}
