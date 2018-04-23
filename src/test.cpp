// Let Catch provide main():
#define CATCH_CONFIG_MAIN

#include<iostream>
#include<string>
#include "catch.hpp"
#include "../src/yaml-parser.h"
#include "yaml-cpp/yaml.h"


TEST_CASE("Yaml parser and setting variables test") {
  std::string file_path = "../data/config.yaml";
  YamlParser yaml_parser = YamlParser(file_path);
  SECTION("Check the GeneralConfiguration node variables") {
    REQUIRE(yaml_parser.filename == file_path);
    REQUIRE(yaml_parser.logging == "debug");
    REQUIRE(yaml_parser.iterations == 100);
    REQUIRE(yaml_parser.epsilonScaling == 1);
    REQUIRE(yaml_parser.gamma == "default");
    REQUIRE(yaml_parser.output_prefix == "purified");
    REQUIRE(yaml_parser.skymodel == "none");
    REQUIRE(yaml_parser.gamma == "default");
    REQUIRE(yaml_parser.measurements == "/path/to/measurment/set");
    REQUIRE(yaml_parser.polarization_measurement == "I");
    REQUIRE(yaml_parser.noise_estimate == "/path/to/noise/estimate");
    REQUIRE(yaml_parser.polarization_noise == "I");
  }
 
}
