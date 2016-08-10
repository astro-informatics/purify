#include <array>
#include <random>
#include "sopt/relative_variation.h"
#include <sopt/imaging_padmm.h>
#include "sopt/utilities.h"
#include "sopt/wavelets.h"
#include "sopt/wavelets/sara.h"
#include "directories.h"
#include "pfitsio.h"
#include "types.h"
#include "MeasurementOperator.h"

int main(int, char **) {
  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();

  bool update_output = true; //save output after each iteration

  std::string const atca_1 = "1637-77";
  std::string const atca_2 = "0114-476";
  std::string const atca_3 = "0153-410";
  std::string const atca_4 = "psr1118";
  std::string const atca_5a = "0332-391";
  std::string const atca_5b = "0332-391.noselfcal";

  std::string const name = atca_5a;
  std::string const weighting = "whiten";
  std::string const stokes = "I";

  std::string const visfile = atca_filename(name + stokes + ".vis");
  std::string const noisefile = atca_filename(name + "V.vis");


  t_int const niters = 1000;
  t_real const beta = 1e-3;
  t_real const over_sample = 1.375;

  auto uv_data = utilities::read_visibility(visfile);
  t_real const max_u = std::sqrt((uv_data.u.array() * uv_data.u.array() + uv_data.v.array() * uv_data.v.array()).maxCoeff());
  uv_data.units = "lambda";
  t_real cellsize = (180 * 3600) / max_u / constant::pi / 3;
  t_int width = 1024;
  t_int height = 1024;

  //header information
  pfitsio::header_params header;
  header.mean_frequency = 1381.67703151703;
  header.ra = 251.083333359168;
  header.dec = -77.2583333178662;
  header.cell_x = cellsize;
  header.cell_y = cellsize;


  std::string const dirty_image_fits = output_filename(name + "_dirty_"+ weighting + ".fits");
  std::string const psf_fits = output_filename(name + "_psf_"+ weighting + ".fits");

  MeasurementOperator measurements(uv_data, 4, 4, "kb_min", width, height, 20, over_sample, cellsize, cellsize, weighting, 0);
  uv_data.weights = utilities::init_weights(uv_data.u, uv_data.v, 
      uv_data.weights, over_sample, 
      weighting, 0, over_sample * width, over_sample * height);

  auto const noise_uv_data = utilities::read_visibility(noisefile);
  Vector<t_complex> const noise_vis = uv_data.weights.array() * noise_uv_data.vis.array();

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

  Vector<> dimage = (measurements_transform.adjoint() * (uv_data.weights.array() * uv_data.vis.array()).matrix()).real();
  t_real max_val = dimage.array().abs().maxCoeff();
  
  header.fits_name = dirty_image_fits;
  pfitsio::write2d_header(Image<t_real>::Map(dimage.data(), height, width), header);

  Vector<t_complex> const psf_vis = (uv_data.vis.array() * 0. + 1.).matrix();
  Vector<> psf = (measurements_transform.adjoint() * (uv_data.weights.array() * psf_vis.array()).matrix()).real();
  max_val = psf.array().abs().maxCoeff();
  psf = psf / max_val;
  header.fits_name = psf_fits;

  pfitsio::write2d_header(Image<t_real>::Map(psf.data(), height, width), header);
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  Vector<t_complex> initial_residuals = Vector<t_complex>::Zero(uv_data.vis.size());

  t_real const noise_rms = std::sqrt(sigma_real * sigma_real + sigma_imag * sigma_imag)/std::sqrt(2);
  t_real const n_sigma = 5;
  t_real const W_norm = uv_data.weights.array().abs().maxCoeff();
  t_real epsilon = utilities::calculate_l2_radius(uv_data.vis, noise_rms, n_sigma); //Calculation of l_2 bound following SARA paper


  t_real purify_gamma = (Psi.adjoint() * (measurements_transform.adjoint() * (uv_data.weights.array() * uv_data.vis.array()).matrix())).cwiseAbs().maxCoeff() * beta;

  std::cout << "Starting sopt!" << '\n';
  std::cout << "Epsilon = " << epsilon << '\n';
  std::cout << "Gamma = " << purify_gamma << '\n';
  auto padmm = sopt::algorithm::ImagingProximalADMM<t_complex>(uv_data.vis)
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
  
  auto algorithm_update  = [&padmm, &update_output, &header, &beta, &Psi, &measurements, &name, &weighting, &uv_data](Vector<t_complex> const & x){
    if (update_output){
      std::string const outfile_fits = output_filename(name + "_solution_"+ weighting + "_update.fits");
      std::string const residual_fits = output_filename(name + "_residual_"+ weighting + "_update.fits");

      Image<t_complex> const image = Image<t_complex>::Map(x.data(), measurements.imsizey(), measurements.imsizex());
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
    if (relative_gamma > 0.01 and new_purify_gamma > 0)
    {
      padmm.gamma(new_purify_gamma);
    }

    return false;
  };

  padmm.is_converged(algorithm_update);
  auto const diagnostic = padmm(estimates);

  std::string const outfile_fits = output_filename(name + "_solution_"+ weighting + "_final.fits");
  std::string const residual_fits = output_filename(name + "_residual_"+ weighting + "_final.fits");
  Image<t_complex> const image = Image<t_complex>::Map(diagnostic.x.data(), measurements.imsizey(), measurements.imsizex());
  // header information
  header.pix_units = "JY/PIXEL";
  header.fits_name = outfile_fits;
  pfitsio::write2d_header(image.real(), header);

  Image<t_complex> residual = measurements.grid(((uv_data.vis - measurements.degrid(image)).array() * uv_data.weights.array().real()).matrix() ).array();
  header.pix_units = "JY/BEAM";
  header.fits_name = residual_fits;
  pfitsio::write2d_header(residual.real(), header);

  return 0;
}
