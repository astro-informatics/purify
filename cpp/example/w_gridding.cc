
#include "purify/directories.h"
#include "purify/pfitsio.h"
#include "purify/wproj_utilities.h"

using namespace purify;
using namespace purify::notinstalled;

int main(int nargs, char const **args) {
  std::srand((unsigned int)std::time(0));
  purify::logging::initialize();
  purify::logging::set_level("debug");
  const t_real fov = 10; // degrees
  const t_int imsizex = 512;
  const t_int imsizey = imsizex;
  const t_real cell_x = fov / imsizex * 60 * 60;
  const t_real cell_y = fov / imsizey * 60 * 60;
  const t_uint J = 4;
  const t_uint M = 1;
  auto const LM = wproj_utilities::fov_cosines(cell_x, cell_y, imsizex, imsizey);

  const t_real cL = std::get<0>(LM) * 0.5;
  const t_real cM = std::get<1>(LM) * 0.5;
  const t_real n_max = (1 - std::sqrt(1 - cL * cL - cM * cM));
  //  const t_real w_cell = 1.88 / (constant::pi * n_max) / 2;
  const t_real w_cell = 2 * constant::pi / n_max;
  const t_real w_max = w_cell * imsizex * 0.25;
  const Vector<t_real> w_grid_coords = wproj_utilities::w_range(w_cell, w_max);
  const Vector<t_real> w_components = Vector<t_real>::Random(M) * w_max * 0;

  const Sparse<t_complex> w_degrider
      = wproj_utilities::w_plane_degrid_matrix(w_grid_coords, w_components, J);

  const std::vector<t_uint> w_rows = wproj_utilities::w_rows(w_degrider);

  const Sparse<t_complex> w_grid = wproj_utilities::w_plane_grid(imsizex, imsizey, cell_x, cell_y,
                                                                 1., J, w_rows, w_cell, w_max);
  //  const Image<t_complex> grid_image = Matrix<t_complex>(w_grid);
  //  std::string const grid_filename = output_filename("w_grid.fits");
  //  pfitsio::write2d(grid_image.cwiseAbs().transpose(), grid_filename);
  const Sparse<t_complex> W = w_degrider * w_grid;
  const auto ft_plan = operators::fftw_plan::measure;
  const auto fftop_ = operators::init_FFT_2d<Vector<t_complex>>(imsizex, imsizey, 1., ft_plan);

  Vector<t_complex> interpchirp;
  const Vector<t_complex> kernel = Vector<t_complex>(W.row(0).transpose());
  std::get<1>(fftop_)(interpchirp, kernel);
  std::string const interp_image_filename = output_filename("w_interp_image.fits");
  pfitsio::write2d(Matrix<t_complex>::Map(interpchirp.data(), imsizey, imsizex).real().transpose(),
                   interp_image_filename);

  const auto grid_correction = [=](const t_real &y, const t_real &x) -> t_complex {
    return 1. / wproj_utilities::ft_kernel(y, x, n_max, J);
  };
  const Matrix<t_complex> correction
      = wproj_utilities::generate_dde(grid_correction, cell_x, cell_y, imsizex, imsizey);
  std::string const correction_filename = output_filename("w_grid_correction.fits");
  pfitsio::write2d(correction.cwiseAbs().transpose(), correction_filename);
  const Matrix<t_complex> chirp_corrected = wproj_utilities::generate_chirp(
      grid_correction, 0. * w_max, cell_x, cell_y, imsizex, imsizey);
  std::string const chirp_correction_filename = output_filename("w_grid_chrip_corrected.fits");
  pfitsio::write2d(chirp_corrected.real().transpose(), chirp_correction_filename);
  const Matrix<t_complex> chirp
      = wproj_utilities::generate_chirp(0. * w_max, cell_x, cell_y, imsizex, imsizey);
  std::string const chirp_filename = output_filename("w_grid_chrip.fits");
  pfitsio::write2d(chirp.real().transpose(), chirp_filename);
}
