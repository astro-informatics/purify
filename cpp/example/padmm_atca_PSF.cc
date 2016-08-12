#include <array>
#include <memory>
#include <random>
#include "sopt/relative_variation.h"
#include "sopt/utilities.h"
#include "sopt/wavelets.h"
#include <sopt/imaging_padmm.h>
#include "sopt/wavelets/sara.h"
#include "MeasurementOperator.h"
#include "PSFOperator.h"
#include "directories.h"
#include "pfitsio.h"
#include "regressions/cwrappers.h"
#include "types.h"

int main(int, char **) {
  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();

  std::string const visfile = atca_filename("1637-77.vis");
  std::string const outfile_fits = output_filename("atca_solution.fits");

  std::string const dirty_image_fits = output_filename("atca_dirty.fits");

  std::string const residual_fits = output_filename("atca_residual.fits");

  t_int const niters = 500;
  t_real const beta = 1e-3;
  t_real const over_sample = 2;
  auto uv_data = utilities::read_visibility(visfile);
  uv_data.units = "lambda";
  t_real cellsize = 1;
  t_int width = 1024;
  t_int height = 1024;
  // uv_data = utilities::uv_symmetry(uv_data);
  // header information
  pfitsio::header_params header;
  header.mean_frequency = 1381.67703151703;
  header.ra = 251.083333359168;
  header.dec = -77.2583333178662;
  header.cell_x = cellsize;
  header.cell_y = cellsize;

  MeasurementOperator image_generator(uv_data, 4, 4, "kb", width, height, over_sample, cellsize,
                                      cellsize, "uniform");

  MeasurementOperator psf_generator(uv_data, 4, 4, "kb", 2 * width, 2 * height, 20, over_sample,
                                    cellsize, cellsize, "uniform");

  auto const dirty_image = image_generator.grid(uv_data.vis).real();
  Vector<t_complex> const dirty_measurements = image_generator.grid(uv_data.vis);

  Image<t_complex> dirty_psf = psf_generator.grid(uv_data.vis.array() * 0 + 1.);
  const t_real max_val_psf = dirty_psf.array().abs().maxCoeff();
  dirty_psf = dirty_psf / max_val_psf;

  PSFOperator psf_op(dirty_psf);

  auto direct = [&psf_op, &width, &height](Vector<t_complex> &out, Vector<t_complex> const &x) {
    assert(x.size() == width * height);
    auto const image = Image<t_complex>::Map(x.data(), height, width);
    out = psf_op.forward(image);
  };
  auto adjoint = [&psf_op, &width, &height](Vector<t_complex> &out, Vector<t_complex> const &x) {
    auto image = Image<t_complex>::Map(out.data(), height, width);
    image = psf_op.adjoint(x);
  };
  auto measurements_transform = sopt::linear_transform<Vector<t_complex>>(
      direct, {0, 1, static_cast<t_int>(width * height)}, adjoint,
      {0, 1, static_cast<t_int>(width * height)});

  sopt::wavelets::SARA const sara{
      std::make_tuple("Dirac", 4u), std::make_tuple("DB1", 4u), std::make_tuple("DB2", 4u),
      std::make_tuple("DB3", 4u),   std::make_tuple("DB4", 4u), std::make_tuple("DB5", 4u),
      std::make_tuple("DB6", 4u),   std::make_tuple("DB7", 4u), std::make_tuple("DB8", 4u)};

  auto const Psi = sopt::linear_transform<t_complex>(sara, height, width);

  header.fits_name = dirty_image_fits;
  pfitsio::write2d_header(dirty_image, header);

  t_real const noise_rms = 0.5630 * 0.6 / std::sqrt(2);
  std::cout << "Calculated RMS noise of " << noise_rms * 1e3 << " mJy" << '\n';
  // t_real epsilon = utilities::calculate_l2_radius(dirty_measurements, noise_rms); //Calculation
  // of l_2 bound following SARA paper
  t_real epsilon = std::sqrt(uv_data.vis.size()) * noise_rms;
  auto purify_gamma = (Psi.adjoint() * (measurements_transform.adjoint() * dirty_measurements))
                          .real()
                          .cwiseAbs()
                          .maxCoeff()
                      * beta;

  std::cout << "Starting sopt!" << '\n';
  std::cout << "Epsilon = " << epsilon << '\n';
  std::cout << "Gamma = " << purify_gamma << '\n';
  auto const padmm = sopt::algorithm::ImagingProximalADMM<t_complex>(dirty_measurements)
                         .itermax(niters)
                         .gamma(purify_gamma)
                         .relative_variation(1e-3)
                         .l2ball_proximal_epsilon(epsilon)
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
  std::tuple<Vector<t_complex>, Vector<t_complex>> const estimates(
      Vector<t_complex>::Zero(dirty_measurements.size()),
      Vector<t_complex>::Zero(dirty_measurements.size()));
  auto const diagnostic = padmm(estimates);
  assert(diagnostic.x.size() == width * height);
  Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), width, height);
  t_real const max_val_final = image.array().abs().maxCoeff();
  image = image;
  // header information
  header.pix_units = "JY/PIXEL";
  header.fits_name = outfile_fits;

  pfitsio::write2d_header(image.real(), header);

  Image<t_real> residual = dirty_image - image.real();
  header.pix_units = "JY/BEAM";
  header.fits_name = residual_fits;
  pfitsio::write2d_header(residual, header);
  return 0;
}
