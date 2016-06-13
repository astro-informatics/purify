#include <array>
#include <memory>
#include <random>
#include "sopt/relative_variation.h"
#include <sopt/imaging_padmm.h>
#include "sopt/utilities.h"
#include "sopt/wavelets.h"
#include "sopt/wavelets/sara.h"
#include "directories.h"
#include "pfitsio.h"
#include "regressions/cwrappers.h"
#include "types.h"
#include "MeasurementOperator.h"

int main(int, char **) {
  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();


  std::string const atca_1 = "1637-77";
  std::string const atca_2 = "0114-476";
  std::string const atca_3 = "0153-410";

  std::string const name = atca_1;
  std::string const weighting = "natural";
  std::string const stokes = "I";

  std::string const visfile = atca_filename(name + stokes + ".vis");
  std::string const noisefile = atca_filename(name + "V.vis");
  std::string const outfile_fits = output_filename(name + "_solution_"+ weighting + ".fits");
  std::string const dirty_image_fits = output_filename(name + "_dirty_"+ weighting + ".fits");
  std::string const psf_fits = output_filename(name + "_psf_"+ weighting + ".fits");
  std::string const residual_fits = output_filename(name + "_residual_"+ weighting + ".fits");

  t_int const niters = 1000;
  t_real const beta = 1e+0;
  t_real const over_sample = 1.375;

  auto uv_data = utilities::read_visibility(visfile);
  uv_data.units = "lambda";
  t_real cellsize = 1;
  t_int width = 2048;
  t_int height = 2048;
  //uv_data = utilities::uv_symmetry(uv_data);
  //header information
  pfitsio::header_params header;
  header.mean_frequency = 1381.67703151703;
  header.ra = 251.083333359168;
  header.dec = -77.2583333178662;
  header.cell_x = cellsize;
  header.cell_y = cellsize;



  MeasurementOperator measurements(uv_data, 4, 4, "kb_min", width, height, over_sample, cellsize, cellsize, weighting, 0);
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
        std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u), 
        std::make_tuple("DB3", 3u),  std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u), 
        std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};


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
  if (utilities::file_exists(outfile_fits))
    std::printf("Using previous run.");
    //initial_estimate = pfitsio::read2d(outfile_fits);


  t_real const noise_rms = std::sqrt(sigma_real * sigma_real + sigma_imag * sigma_imag)/std::sqrt(2);
  t_real const n_sigma = 1;
  t_real const W_norm = measurements.W.abs().maxCoeff();
  t_real epsilon = 3 * utilities::calculate_l2_radius(input, noise_rms, n_sigma); //Calculation of l_2 bound following SARA paper


  auto purify_gamma = (Psi.adjoint() * (measurements_transform.adjoint() * input)).cwiseAbs().maxCoeff() * beta;
  
  uv_data.weights = measurements.W;
  measurements.W = measurements.W.array() * 0 + 1;
  measurements.norm = 1;
  measurements.norm = std::sqrt(measurements.power_method(100)); //renormalise, because weights are not in the operator
  

  std::cout << "Starting sopt!" << '\n';
  std::cout << "Epsilon = " << epsilon << '\n';
  std::cout << "Gamma = " << purify_gamma << '\n';
  auto const padmm = sopt::algorithm::ImagingProximalADMM<t_complex>(input)
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
    .lagrange_update_scale(0.5)
    .nu(1e0)
    .Psi(Psi)
    .Phi(measurements_transform);
  std::tuple<Vector<t_complex>, Vector<t_complex>> const estimates(initial_estimate, Vector<t_complex>::Zero(input.size()));  
  auto const diagnostic = padmm(estimates);
  assert(diagnostic.x.size() == width * height);
  Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), measurements.imsizey, measurements.imsizex);
  t_real const max_val_final = image.array().abs().maxCoeff();
  image = image;
  // header information
  header.pix_units = "JY/PIXEL";
  header.fits_name = outfile_fits;
  pfitsio::write2d_header(image.real(), header);

  image = Image<t_complex>::Map(diagnostic.x.data(), measurements.imsizey, measurements.imsizex);
  Image<t_complex> residual = measurements.grid(((input - measurements.degrid(image)).array() * uv_data.weights.array().real()).matrix() ).array();
  header.pix_units = "JY/BEAM";
  header.fits_name = residual_fits;
  pfitsio::write2d_header(residual.real(), header);
  return 0;
}
