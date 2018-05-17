
#include<iostream>
#include<string>
#include "catch.hpp"
#include "../purify/yaml-parser.h"
#include "yaml-cpp/yaml.h"


TEST_CASE("Yaml parser and setting variables test") {
  std::string file_path = "../data/config/config.yaml";
  YamlParser yaml_parser = YamlParser(file_path);
  SECTION("Check the GeneralConfiguration node variables") {
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
  SECTION("Check the MeasureOperators node variables") {
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
  SECTION("Check the SARA node variables") {
    std::vector<int> expected_wavelets = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    REQUIRE(yaml_parser.wavelet_basis() == expected_wavelets);
    REQUIRE(yaml_parser.wavelet_levels() == 4);
    REQUIRE(yaml_parser.algorithm() == "padmm");
  }
  SECTION("Check the AlgorithmOptions node variables") {
    REQUIRE(yaml_parser.epsilonConvergenceScaling() == 1);
    REQUIRE(yaml_parser.realValueConstraint() == true);
    REQUIRE(yaml_parser.positiveValueConstraint() == true);
    REQUIRE(yaml_parser.mpiAlgorithm() == "fully-distributed");
    REQUIRE(yaml_parser.relVarianceConvergence() == 1e-3);
    REQUIRE(yaml_parser.param1() == "none");
    REQUIRE(yaml_parser.param2() == "none");
  }

}
