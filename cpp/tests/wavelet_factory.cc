#include "catch.hpp"
#include "purify/wavelet_operator_factory.h"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/mpi_utilities.h"
#include "purify/utilities.h"
using namespace purify;

TEST_CASE("Wavelet Factory Operator") {
  purify::logging::set_level("debug");
 const std::vector<std::tuple<std::string, t_uint>> wavelets{std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
          std::make_tuple("DB3", 3u), std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
          std::make_tuple("DB6", 3u), std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const imsizey = 128;
  auto const imsizex = 128;
  const auto sara = sopt::wavelets::SARA(wavelets.begin(), wavelets.end());
  auto phi = sopt::linear_transform<t_complex>(sara, imsizey, imsizex);
  auto op =  std::make_shared<sopt::LinearTransform<Vector<t_complex>> const>(phi);
  auto factory_op = factory::wavelet_operator_factory<Vector<t_complex>>(factory::distributed_wavelet_operator::serial, wavelets, imsizey, imsizex);
  
  CAPTURE(sara.size());
  SECTION("forward"){
  const  Vector<t_complex> input = Vector<t_complex>::Random(sara.size() * imsizex * imsizey);
  REQUIRE((*op * input).isApprox(*factory_op * input));
  }
  SECTION("backward"){
  const  Vector<t_complex> input = Vector<t_complex>::Random( imsizex * imsizey);
  REQUIRE((op->adjoint() * input).isApprox(factory_op->adjoint() * input));
  }
}
