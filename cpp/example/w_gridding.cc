
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/pfitsio.h"
#include "purify/wproj_utilities.h"

using namespace purify;
using namespace purify::notinstalled;

int main(int nargs, char const **args) {
  purify::logging::initialize();
  purify::logging::set_level("debug");
  std::srand((unsigned int)std::time(0));
  const t_real fov = 15;  // degrees
  const t_int imsizex = 512;
  const t_int imsizey = imsizex;
  const t_real cell_x = fov / imsizex * 60 * 60;
  const t_real cell_y = fov / imsizey * 60 * 60;
  const t_uint p = 1;
  const t_uint M = 1e3;
  auto const LM = wproj_utilities::fov_cosines(cell_x, cell_y, imsizex, imsizey);

  const t_real cL = std::get<0>(LM) * 0.5;
  const t_real cM = std::get<1>(LM) * 0.5;
  const t_real n_max = (1 - std::sqrt(1 - cL * cL - cM * cM));
  const t_real xi = 1e-2;
  const t_real w_cell = xi / (2 * constant::pi * n_max);
  const t_real w_max = w_cell * 20;
  const Vector<t_real> w_grid_coords = wproj_utilities::w_range(w_cell, w_max);
  // const Vector<t_real> w_components = Vector<t_real>::Random(M) * w_max;
  const Vector<t_real> w_components = wproj_utilities::w_range(0.005, w_max);
  const std::vector<t_uint> w_rows = wproj_utilities::w_rows(w_components, w_grid_coords);
  const std::tuple<std::vector<std::function<t_complex(t_real, t_real)>>,
                   std::vector<std::function<t_complex(t_real)>>>
      series = wproj_utilities::expansions::taylor(p, cell_x, cell_y, imsizex, imsizey);
  const std::vector<Sparse<t_complex>> w_expan = wproj_utilities::w_expansion(
      imsizex, imsizey, cell_x, cell_y, 1., w_rows, w_grid_coords, std::get<0>(series));
  const Sparse<t_complex> W = wproj_utilities::w_projection_expansion(
      w_expan, w_components, w_grid_coords, w_rows, std::get<1>(series));
  const auto ft_plan = operators::fftw_plan::measure;
  const auto fftop_ = operators::init_FFT_2d<Vector<t_complex>>(imsizex, imsizey, 1., ft_plan);

  Vector<t_complex> interpchirp;
  const Vector<t_complex> kernel = Vector<t_complex>(W.row(0).transpose());
  std::get<1>(fftop_)(interpchirp, kernel);
  std::string const interp_image_filename = output_filename("w_interp_image.fits");
  pfitsio::write2d(Matrix<t_complex>::Map(interpchirp.data(), imsizey, imsizex).real().transpose(),
                   interp_image_filename);

  const Matrix<t_complex> chirp =
      wproj_utilities::generate_chirp(w_components(0), cell_x, cell_y, imsizex, imsizey);
  std::string const chirp_filename = output_filename("w_grid_chrip.fits");
  pfitsio::write2d(chirp.real().transpose(), chirp_filename);
}
