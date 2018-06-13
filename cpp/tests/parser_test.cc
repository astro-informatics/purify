#include <iostream>
#include <cstdio>
#include <string>
#include "catch.hpp"
#include "purify/directories.h"
#include "purify/yaml-parser.h"
#include <yaml-cpp/yaml.h>


TEST_CASE("Yaml parser and setting variables test")
{
  std::string file_path = purify::notinstalled::data_filename("config/config.yaml");
  YamlParser yaml_parser = YamlParser(file_path);
  SECTION("Check the GeneralConfiguration measurement input variables")
    {
      std::string file_path_m = purify::notinstalled::data_filename("config/test_measurements_config.yaml");
      YamlParser yaml_parser_m = YamlParser(file_path_m);
      REQUIRE(yaml_parser_m.source() == purify::utilities::vis_source::measurements);
      std::vector<std::string> expected_measurements = {"/path/to/measurment/set"};
      REQUIRE(yaml_parser_m.measurements_files() == expected_measurements);
      REQUIRE(yaml_parser_m.measurements_polarization() == "I");
      REQUIRE(yaml_parser_m.measurements_units() == purify::utilities::vis_units::pixels);
      REQUIRE(yaml_parser_m.measurements_sigma() == 0.1f);
      REQUIRE(yaml_parser_m.skymodel() == "");
      REQUIRE(yaml_parser_m.signal_to_noise() == 30);
    }
  SECTION("Check the GeneralConfiguration simulation input variables")
    {
      std::string file_path_s = purify::notinstalled::data_filename("config/test_simulation_config.yaml");
      YamlParser yaml_parser_s = YamlParser(file_path_s);
      REQUIRE(yaml_parser_s.source() == purify::utilities::vis_source::simulation);
      REQUIRE(yaml_parser_s.measurements_files() == std::vector<std::string>());
      REQUIRE(yaml_parser_s.measurements_polarization() == "");
      REQUIRE(yaml_parser_s.measurements_units() == purify::utilities::vis_units::radians);
      REQUIRE(yaml_parser_s.measurements_sigma() == 1);
      REQUIRE(yaml_parser_s.skymodel() == "/path/to/sky/image");
      REQUIRE(yaml_parser_s.signal_to_noise() == 10);
   }
  SECTION("Check the rest of the GeneralConfiguration variables")
    {
      REQUIRE(yaml_parser.filepath() == file_path);
      REQUIRE(yaml_parser.logging() == "debug");
      REQUIRE(yaml_parser.iterations() == 100);
      REQUIRE(yaml_parser.epsilonScaling() == 1);
      REQUIRE(yaml_parser.gamma() == "default");
      REQUIRE(yaml_parser.output_prefix() == "purified");
    }
  SECTION("Check the MeasureOperators node variables")
    {
      REQUIRE(yaml_parser.Jweights() == "kb");
      REQUIRE(yaml_parser.wProjection() == false);
      REQUIRE(yaml_parser.oversampling() == 2);
      REQUIRE(yaml_parser.powMethod_iter() == 100);
      REQUIRE(yaml_parser.powMethod_tolerance() == float(1e-4));
      REQUIRE(yaml_parser.Dx() == 1);
      REQUIRE(yaml_parser.Dy() == 1);
      REQUIRE(yaml_parser.x() == 1024);
      REQUIRE(yaml_parser.y() == 1024);
      REQUIRE(yaml_parser.Jx() == 4);
      REQUIRE(yaml_parser.Jy() == 4);
      REQUIRE(yaml_parser.chirp_fraction() == 1);
      REQUIRE(yaml_parser.kernel_fraction() == 1);
    }
  SECTION("Check the SARA node variables")
    {
      std::vector<std::string> expected_wavelets = {"Dirac", "DB1", "DB2", "DB3", "DB4", "DB5", "DB6", "DB7", "DB8"};
      REQUIRE(yaml_parser.wavelet_basis() == expected_wavelets);
      REQUIRE(yaml_parser.wavelet_levels() == 4);
      REQUIRE(yaml_parser.algorithm() == "padmm");
    }
  SECTION("Check the AlgorithmOptions node variables")
    {
      REQUIRE(yaml_parser.epsilonConvergenceScaling() == 1);
      REQUIRE(yaml_parser.realValueConstraint() == true);
      REQUIRE(yaml_parser.positiveValueConstraint() == true);
      REQUIRE(yaml_parser.mpiAlgorithm() == "fully-distributed");
      REQUIRE(yaml_parser.relVarianceConvergence() == 1e-3);
      REQUIRE(yaml_parser.param1() == "none");
      REQUIRE(yaml_parser.param2() == "none");
    }
  SECTION("Check the writeOutput method")
    {
      yaml_parser.writeOutput();
      std::string file_path_save = purify::notinstalled::data_filename("config/config_" + yaml_parser.timestamp() + "_save.yaml");
      YamlParser yaml_parser_check(file_path_save);
      REQUIRE(yaml_parser_check.filepath() == file_path_save);
      REQUIRE(yaml_parser_check.logging() == yaml_parser.logging());
      REQUIRE(yaml_parser_check.iterations() == yaml_parser.iterations());
      REQUIRE(yaml_parser_check.epsilonScaling() == yaml_parser.epsilonScaling());
      REQUIRE(yaml_parser_check.gamma() == yaml_parser.gamma());
      REQUIRE(yaml_parser_check.output_prefix() == yaml_parser.output_prefix());
      REQUIRE(yaml_parser_check.source() == yaml_parser.source());
      REQUIRE(yaml_parser_check.measurements_files() == yaml_parser.measurements_files());
      REQUIRE(yaml_parser_check.measurements_polarization() == yaml_parser.measurements_polarization());
      REQUIRE(yaml_parser_check.measurements_units() == yaml_parser.measurements_units());
      REQUIRE(yaml_parser_check.measurements_sigma() == yaml_parser.measurements_sigma());
      REQUIRE(yaml_parser_check.skymodel() == yaml_parser.skymodel());
      REQUIRE(yaml_parser_check.signal_to_noise() == yaml_parser.signal_to_noise());
      REQUIRE(yaml_parser_check.Jweights() == yaml_parser.Jweights());
      REQUIRE(yaml_parser_check.wProjection() == yaml_parser.wProjection());
      REQUIRE(yaml_parser_check.oversampling() == yaml_parser.oversampling());
      REQUIRE(yaml_parser_check.powMethod_iter() == yaml_parser.powMethod_iter());
      REQUIRE(yaml_parser_check.powMethod_tolerance() == yaml_parser_check.powMethod_tolerance());
      REQUIRE(yaml_parser_check.Dx() == yaml_parser.Dx());
      REQUIRE(yaml_parser_check.Dy() == yaml_parser.Dy());
      REQUIRE(yaml_parser_check.x() == yaml_parser.x());
      REQUIRE(yaml_parser_check.y() == yaml_parser.y());
      REQUIRE(yaml_parser_check.Jx() == yaml_parser.Jx());
      REQUIRE(yaml_parser_check.Jy() == yaml_parser.Jy());
      REQUIRE(yaml_parser_check.chirp_fraction() == yaml_parser.chirp_fraction());
      REQUIRE(yaml_parser_check.kernel_fraction() == yaml_parser.kernel_fraction());
      REQUIRE(yaml_parser.wavelet_basis() == yaml_parser.wavelet_basis());
      REQUIRE(yaml_parser.wavelet_levels() == yaml_parser.wavelet_levels());
      REQUIRE(yaml_parser.algorithm() == yaml_parser.algorithm());
      REQUIRE(yaml_parser.epsilonConvergenceScaling() == yaml_parser.epsilonConvergenceScaling());
      REQUIRE(yaml_parser.realValueConstraint() == yaml_parser.realValueConstraint());
      REQUIRE(yaml_parser.positiveValueConstraint() == yaml_parser.positiveValueConstraint());
      REQUIRE(yaml_parser.mpiAlgorithm() == yaml_parser.mpiAlgorithm());
      REQUIRE(yaml_parser.relVarianceConvergence() == yaml_parser.relVarianceConvergence());
      REQUIRE(yaml_parser.param1() == yaml_parser.param1());
      REQUIRE(yaml_parser.param2() == yaml_parser.param2());
      std::remove(file_path_save.c_str());
    }
}
