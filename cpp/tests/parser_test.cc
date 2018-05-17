
#include<iostream>
#include<string>
#include "catch.hpp"
#include "../purify/yaml-parser.h"
#include "yaml-cpp/yaml.h"


TEST_CASE("Yaml parser and setting variables test")
{
  std::string file_path = "../data/config/config.yaml";
  YamlParser yaml_parser = YamlParser(file_path);
  SECTION("Check the GeneralConfiguration node variables")
    {
      REQUIRE(yaml_parser.filepath() == file_path);
      REQUIRE(yaml_parser.logging() == "debug");
      REQUIRE(yaml_parser.iterations() == 100);
      REQUIRE(yaml_parser.epsilonScaling() == 1);
      REQUIRE(yaml_parser.gamma() == "default");
      REQUIRE(yaml_parser.output_prefix() == "purified");
      REQUIRE(yaml_parser.skymodel() == "none");
      REQUIRE(yaml_parser.measurements() == "/path/to/measurment/set");
      REQUIRE(yaml_parser.polarization_measurement() == "I");
      REQUIRE(yaml_parser.noise_estimate() == "/path/to/noise/estimate");
      REQUIRE(yaml_parser.polarization_noise() == "I");
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
      std::vector<int> expected_wavelets = {0, 1, 2, 3, 4, 5, 6, 7, 8};
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
      std::string file_path_save = "config_save.yaml";
      yaml_parser.writeOutput(file_path_save);
      YamlParser yaml_parser_check(file_path_save);
      REQUIRE(yaml_parser_check.filepath() == file_path_save);
      REQUIRE(yaml_parser_check.logging() == yaml_parser.logging());
      REQUIRE(yaml_parser_check.iterations() == yaml_parser.iterations());
      REQUIRE(yaml_parser_check.epsilonScaling() == yaml_parser.epsilonScaling());
      REQUIRE(yaml_parser_check.gamma() == yaml_parser.gamma());
      REQUIRE(yaml_parser_check.output_prefix() == yaml_parser.output_prefix());
      REQUIRE(yaml_parser_check.skymodel() == yaml_parser.skymodel());
      REQUIRE(yaml_parser_check.measurements() == yaml_parser.measurements());
      REQUIRE(yaml_parser_check.polarization_measurement() == yaml_parser.polarization_measurement());
      REQUIRE(yaml_parser_check.noise_estimate() == yaml_parser.noise_estimate());
      REQUIRE(yaml_parser_check.polarization_noise() == yaml_parser.polarization_noise());
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
    }
}
