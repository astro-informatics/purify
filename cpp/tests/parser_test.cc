#include <iostream>
#include <cstdio>
#include <string>
#include "catch.hpp"
#include "purify/directories.h"
#include "purify/yaml-parser.h"
#include <yaml-cpp/yaml.h>

using namespace purify;

TEST_CASE("Yaml parser and setting variables test")
{
  std::string file_path = purify::notinstalled::data_filename("config/config.yaml");
  YamlParser yaml_parser = YamlParser(file_path);
  std::string file_path_m = purify::notinstalled::data_filename("config/test_measurements_config.yaml");
  YamlParser yaml_parser_m = YamlParser(file_path_m);
  SECTION("Check the GeneralConfiguration measurement input variables")
    {
      REQUIRE(yaml_parser_m.source() == purify::utilities::vis_source::measurements);
      std::vector<std::string> expected_measurements = {"/path/to/measurment/set"};
      REQUIRE(yaml_parser_m.measurements() == expected_measurements);
      REQUIRE(yaml_parser_m.measurements_polarization() == stokes::I);
      REQUIRE(yaml_parser_m.measurements_units() == purify::utilities::vis_units::pixels);
      REQUIRE(yaml_parser_m.measurements_sigma() == 0.1);
      REQUIRE(yaml_parser_m.skymodel() == "");
      REQUIRE(yaml_parser_m.signal_to_noise() == 30);
    }
  SECTION("Check the GeneralConfiguration simulation input variables")
    {
      std::string file_path_s = purify::notinstalled::data_filename("config/test_simulation_config.yaml");
      YamlParser yaml_parser_s = YamlParser(file_path_s);
      REQUIRE(yaml_parser_s.source() == purify::utilities::vis_source::simulation);
      REQUIRE(yaml_parser_s.measurements() == std::vector<std::string>());
      REQUIRE(yaml_parser_s.measurements_polarization() == stokes::I);
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
      REQUIRE(yaml_parser.output_prefix() == "/path/to/output/dir");
    }
  SECTION("Check the MeasureOperators node variables")
    {
      REQUIRE(yaml_parser.Jweights() == "kb");
      REQUIRE(yaml_parser.oversampling() == 2);
      REQUIRE(yaml_parser.powMethod_iter() == 100);
      REQUIRE(yaml_parser.powMethod_tolerance() == float(1e-4));
      REQUIRE(yaml_parser.Dx() == 1);
      REQUIRE(yaml_parser.Dy() == 1);
      REQUIRE(yaml_parser.x() == 1024);
      REQUIRE(yaml_parser.y() == 1024);
      REQUIRE(yaml_parser.Jx() == 4);
      REQUIRE(yaml_parser.Jy() == 4);
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
      REQUIRE(yaml_parser.mpiAlgorithm() == factory::algo_distribution::mpi_distributed);
      REQUIRE(yaml_parser.relVarianceConvergence() == 1e-3);
    }
  SECTION("Check the writeOutput method")
    {
      yaml_parser_m.writeOutput(); // This test config file has a relative path for output, for testing purposes
      std::string file_path_save = yaml_parser_m.output_prefix() + "/output_" + yaml_parser_m.timestamp() + "/test_measurements_config_save.yaml";
      YamlParser yaml_parser_check(file_path_save);
      REQUIRE(yaml_parser_check.filepath() == file_path_save);
      REQUIRE(yaml_parser_check.logging() == yaml_parser_m.logging());
      REQUIRE(yaml_parser_check.iterations() == yaml_parser_m.iterations());
      REQUIRE(yaml_parser_check.epsilonScaling() == yaml_parser_m.epsilonScaling());
      REQUIRE(yaml_parser_check.gamma() == yaml_parser_m.gamma());
      REQUIRE(yaml_parser_check.output_prefix() == yaml_parser_m.output_prefix());
      REQUIRE(yaml_parser_check.source() == yaml_parser_m.source());
      REQUIRE(yaml_parser_check.measurements() == yaml_parser_m.measurements());
      REQUIRE(yaml_parser_check.measurements_polarization() == yaml_parser_m.measurements_polarization());
      REQUIRE(yaml_parser_check.measurements_units() == yaml_parser_m.measurements_units());
      REQUIRE(yaml_parser_check.measurements_sigma() == yaml_parser_m.measurements_sigma());
      REQUIRE(yaml_parser_check.skymodel() == yaml_parser_m.skymodel());
      REQUIRE(yaml_parser_check.signal_to_noise() == yaml_parser_m.signal_to_noise());
      REQUIRE(yaml_parser_check.Jweights() == yaml_parser_m.Jweights());
      REQUIRE(yaml_parser_check.oversampling() == yaml_parser_m.oversampling());
      REQUIRE(yaml_parser_check.powMethod_iter() == yaml_parser_m.powMethod_iter());
      REQUIRE(yaml_parser_check.powMethod_tolerance() == yaml_parser_m.powMethod_tolerance());
      REQUIRE(yaml_parser_check.Dx() == yaml_parser_m.Dx());
      REQUIRE(yaml_parser_check.Dy() == yaml_parser_m.Dy());
      REQUIRE(yaml_parser_check.x() == yaml_parser_m.x());
      REQUIRE(yaml_parser_check.y() == yaml_parser_m.y());
      REQUIRE(yaml_parser_check.Jx() == yaml_parser_m.Jx());
      REQUIRE(yaml_parser_check.Jy() == yaml_parser_m.Jy());
      REQUIRE(yaml_parser.wavelet_basis() == yaml_parser_m.wavelet_basis());
      REQUIRE(yaml_parser.wavelet_levels() == yaml_parser_m.wavelet_levels());
      REQUIRE(yaml_parser.algorithm() == yaml_parser_m.algorithm());
      REQUIRE(yaml_parser.epsilonConvergenceScaling() == yaml_parser_m.epsilonConvergenceScaling());
      REQUIRE(yaml_parser.realValueConstraint() == yaml_parser_m.realValueConstraint());
      REQUIRE(yaml_parser.positiveValueConstraint() == yaml_parser_m.positiveValueConstraint());
      REQUIRE(yaml_parser.mpiAlgorithm() == yaml_parser_m.mpiAlgorithm());
      REQUIRE(yaml_parser.relVarianceConvergence() == yaml_parser_m.relVarianceConvergence());
     std::remove(file_path_save.c_str());
    }
}
