#include "purify/config.h"
#include "purify/types.h"
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/pfitsio.h"
#include "purify/utilities.h"
#include "purify/wavelet_operator_factory.h"

using namespace purify;
using namespace purify::notinstalled;

int main(int nargs, char const **args) {
  purify::logging::initialize();
  purify::logging::set_level("debug");
  auto const input_name =
      (nargs > 1) ? static_cast<std::string>(args[1]) : image_filename("M31.fits");
  auto const output_name =
      (nargs > 2) ? static_cast<std::string>(args[2]) : "wavelet_decomposition.fits";
#define ARGS_MACRO(NAME, ARGN, VALUE, TYPE) \
  auto const NAME =                         \
      static_cast<TYPE>((nargs > ARGN) ? std::stod(static_cast<std::string>(args[ARGN])) : VALUE);

  ARGS_MACRO(levels, 4, 3, t_uint)
  Image<t_complex> const ground_truth_image = pfitsio::read2d(input_name).cast<t_complex>();

  const std::vector<std::tuple<std::string, t_uint>> wavelets{
      std::make_tuple("Dirac", levels), std::make_tuple("DB1", levels),
      std::make_tuple("DB2", levels),   std::make_tuple("DB3", levels),
      std::make_tuple("DB4", levels),   std::make_tuple("DB5", levels),
      std::make_tuple("DB6", levels),   std::make_tuple("DB7", levels),
      std::make_tuple("DB8", levels)};

  const auto phi = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, wavelets, ground_truth_image.rows(),
      ground_truth_image.cols());
  const Image<t_complex> decompostion =
      Image<t_complex>::Map((phi->adjoint() * Vector<t_complex>::Map(ground_truth_image.data(),
                                                                     ground_truth_image.size()))
                                .eval()
                                .data(),
                            ground_truth_image.rows(), ground_truth_image.cols() * wavelets.size());
  pfitsio::write2d(decompostion.real(), output_name);
}
