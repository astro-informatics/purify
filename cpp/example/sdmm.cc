#include "purify_config.h"
#include "purify_types.h"
#include <array>
#include <fftw3.h>
#include <memory>
#include <random>
#include "sopt/relative_variation.h"
#include "sopt/sdmm.h"
#include "sopt/utilities.h"
#include "sopt/wavelets.h"
#include "sopt/wavelets/sara.h"
#include "directories.h"
#include "pfitsio.h"
#include "regressions/cwrappers.h"
#include "types.h"

int main(int, char **) {
  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();

  std::string const fitsfile = image_filename("M31.fits");
  std::string const visfile = image_filename("Coverages/cont_sim4.vis");
  std::string const outfile = output_filename("M31.tiff");

  auto const M31 = pfitsio::read2d(fitsfile);
  purify::CMeasurementOperator const measurements(visfile, M31.rows(), M31.cols());
  sopt::wavelets::SARA const sara{std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u)};
  auto const Psi = sopt::linear_transform<t_complex>(sara, M31.rows(), M31.cols());

  std::mt19937_64 mersenne;
  Vector<t_complex> const y0
      = measurements.linear_transform() * Vector<t_complex>::Map(M31.data(), M31.size());
  auto const input = dirty(y0, mersenne, 30e0);
  auto const epsilon
      = std::sqrt(y0.size() + 2 * std::sqrt(y0.size())) * sigma(y0, 30e0)
        / std::sqrt(static_cast<t_real>(y0.size()) / static_cast<t_real>(M31.size()));

  auto const sdmm
      = sopt::algorithm::SDMM<t_complex>()
            .itermax(500)
            .gamma((measurements.linear_transform().adjoint() * input).real().maxCoeff() * 1e-3)
            .is_converged(sopt::RelativeVariation<t_complex>(1e-3))
            .conjugate_gradient(100, 1e-6)
            .append(sopt::proximal::translate(sopt::proximal::L2Ball<t_complex>(epsilon), -input),
                    measurements.linear_transform())
            .append(sopt::proximal::l1_norm<t_complex>, Psi.adjoint(), Psi)
            .append(sopt::proximal::positive_quadrant<t_complex>);
  auto const result = sdmm(Vector<t_complex>::Zero(M31.size()));
  assert(result.out.size() == M31.size());
  Image<t_real> image = Image<t_complex>::Map(result.out.data(), M31.rows(), M31.cols()).real();
  sopt::utilities::write_tiff(image, outfile);
  return 0;
}
