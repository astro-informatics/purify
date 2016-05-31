#include <array>
#include <memory>
#include <random>
#include "sopt/relative_variation.h"
#include <sopt/l1_padmm.h>
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

  std::string const visfile = atca_filename("1637-77.vis");
  std::string const outfile = output_filename("atca.tiff");
  std::string const outfile_fits = output_filename("atca_solution.fits");

  std::string const dirty_image_fits = output_filename("atca_dirty.fits");

  std::string const residual_fits = output_filename("atca_residual.fits");

  std::string const restore_fits = output_filename("atca_restore.fits");

  t_int const niters = 500;
  t_real const beta = 1e-2;
  t_real const over_sample = 2;
  auto uv_data = utilities::read_visibility(visfile);
  uv_data.units = "lambda";
  t_real cellsize = 2;
  t_int width = 1024;
  t_int height = 1024;
  uv_data = utilities::uv_symmetry(uv_data);
  //header information
  pfitsio::header_params header;
  header.mean_frequency = 1381.67703151703;
  header.ra = 251.083333359168;
  header.dec = -77.2583333178662;
  header.cell_x = cellsize;
  header.cell_y = cellsize;



  MeasurementOperator measurements(uv_data, 4, 4, "kb", width, height, over_sample, cellsize, cellsize, "none");

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

  sopt::wavelets::SARA const sara{std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u), 
        std::make_tuple("DB3", 3u),  std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u), 
        std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};


  auto const Psi = sopt::linear_transform<t_complex>(sara, height, width);
  std::printf("Saving dirty map \n");
  //const Vector<t_complex> weighted_data = (uv_data.vis.array() * measurements.W).matrix(); //whitening data
  const Vector<t_complex> & input = uv_data.vis.array();
  Vector<> dimage = (measurements_transform.adjoint() * input).real();
  t_real max_val = dimage.array().abs().maxCoeff();
  dimage = dimage / max_val;
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  if (utilities::file_exists(outfile_fits))
    std::printf("Using previous run.");
    //initial_estimate = pfitsio::read2d(outfile_fits);
  

  pfitsio::write2d_header(Image<t_real>::Map(dimage.data(), height, width), header);


  t_real const noise_rms = 0.5630 * 0.8/std::sqrt(2);
  std::cout << "Calculated RMS noise of " << noise_rms * 1e3 << " mJy" << '\n';
  t_real epsilon = utilities::calculate_l2_radius(input, noise_rms); //Calculation of l_2 bound following SARA paper
  t_real epsilon_alt = std::sqrt(uv_data.vis.size()) * noise_rms;
  auto purify_gamma = (Psi.adjoint() * (measurements_transform.adjoint() * (uv_data.weights.array().real() * input.array()).matrix())).real().cwiseAbs().maxCoeff() * beta;

  std::cout << "Starting sopt!" << '\n';
  std::cout << "Epsilon = " << epsilon << '\n';
  std::cout << "Gamma = " << purify_gamma << '\n';
  auto const padmm = sopt::algorithm::L1ProximalADMM<t_complex>(input)
    .itermax(niters)
    .gamma(purify_gamma)
    .relative_variation(1e-3)
    .l2ball_proximal_epsilon(epsilon)
    .l2ball_proximal_weights(uv_data.weights.array().real())
    .tight_frame(false)
    .l1_proximal_tolerance(1e-4)
    .l1_proximal_nu(1)
    .l1_proximal_itermax(100)
    .l1_proximal_positivity_constraint(true)
    .l1_proximal_real_constraint(true)
    .residual_convergence(epsilon)
    .lagrange_update_scale(0.5)
    .nu(1e0)
    .Psi(Psi)
    .Phi(measurements_transform);
  auto const result = padmm(initial_estimate);
  assert(result.x.size() == width * height);
  Image<t_complex> image = Image<t_complex>::Map(result.x.data(), measurements.imsizey, measurements.imsizex);
  t_real const max_val_final = image.array().abs().maxCoeff();
  image = image;
  // header information
  header.pix_units = "JY/PIXEL";
  header.fits_name = outfile_fits;


  pfitsio::write2d_header(image.real(), header);
  image = Image<t_complex>::Map(result.x.data(), measurements.imsizey, measurements.imsizex);
  Image<t_complex> residual = measurements.grid(uv_data.weights.array().real().sqrt() * (input - measurements.degrid(image)).array());
  header.pix_units = "JY/BEAM";
  header.fits_name = residual_fits;
  pfitsio::write2d_header(residual.real(), header);
  return 0;
}
