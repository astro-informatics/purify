#include <cstdio>
#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>
#include "catch.hpp"
#include "purify/directories.h"
#include "purify/yaml-parser.h"

using namespace purify;

TEST_CASE("Yaml parser and setting variables test") {
  std::string file_path = purify::notinstalled::data_filename("config/config.yaml");
  YamlParser yaml_parser = YamlParser(file_path);
  std::string file_path_m =
      purify::notinstalled::data_filename("config/test_measurements_config.yaml");
  YamlParser yaml_parser_m = YamlParser(file_path_m);
  SECTION("Check the GeneralConfiguration measurement input variables") {
    REQUIRE(yaml_parser_m.source() == purify::utilities::vis_source::measurements);
    std::vector<std::string> expected_measurements = {"/path/to/measurment/set"};
    REQUIRE(yaml_parser_m.measurements() == expected_measurements);
    REQUIRE(yaml_parser_m.measurements_polarization() == stokes::I);
    REQUIRE(yaml_parser_m.measurements_units() == purify::utilities::vis_units::pixels);
    REQUIRE(yaml_parser_m.measurements_sigma() == 0.1);
    REQUIRE(yaml_parser_m.skymodel() == "");
    REQUIRE(yaml_parser_m.signal_to_noise() == 30);
    REQUIRE(yaml_parser_m.number_of_measurements() == 100000);
    REQUIRE(yaml_parser_m.w_rms() == 100.);
    REQUIRE(yaml_parser_m.warm_start() == "/path/to/warm/start/image");
  }
  SECTION("Check the GeneralConfiguration simulation input variables") {
    std::string file_path_s =
        purify::notinstalled::data_filename("config/test_simulation_config.yaml");
    YamlParser yaml_parser_s = YamlParser(file_path_s);
    REQUIRE(yaml_parser_s.source() == purify::utilities::vis_source::simulation);
    REQUIRE(yaml_parser_s.measurements() ==
            std::vector<std::string>({"path/to/coverage/measurement/file"}));
    REQUIRE(yaml_parser_s.measurements_polarization() == stokes::I);
    REQUIRE(yaml_parser_s.measurements_units() == purify::utilities::vis_units::lambda);
    REQUIRE(yaml_parser_s.measurements_sigma() == 1);
    REQUIRE(yaml_parser_s.skymodel() == "/path/to/sky/image");
    REQUIRE(yaml_parser_s.signal_to_noise() == 10);
    REQUIRE(yaml_parser_s.sim_J() == 8);
  }
  SECTION("Check the rest of the GeneralConfiguration variables") {
    REQUIRE(yaml_parser.filepath() == file_path);
    REQUIRE(yaml_parser.logging() == "debug");
    REQUIRE(yaml_parser.iterations() == 100);
    REQUIRE(yaml_parser.epsilonScaling() == 1);
    REQUIRE(yaml_parser.update_iters() == 0);
    REQUIRE(yaml_parser.update_tolerance() == 1e-1);
    REQUIRE(yaml_parser.output_prefix() == "/path/to/output/dir");
  }
  SECTION("Check the MeasureOperators node variables") {
    REQUIRE(yaml_parser.kernel() == "kb");
    REQUIRE(yaml_parser.oversampling() == 2);
    REQUIRE(yaml_parser.powMethod_iter() == 100);
    REQUIRE(yaml_parser.powMethod_tolerance() == float(1e-4));
    REQUIRE(yaml_parser.cellsizex() == 1);
    REQUIRE(yaml_parser.cellsizey() == 1);
    REQUIRE(yaml_parser.height() == 1024);
    REQUIRE(yaml_parser.width() == 1024);
    REQUIRE(yaml_parser.Jx() == 4);
    REQUIRE(yaml_parser.Jy() == 4);
    REQUIRE(yaml_parser.Jw() == 30);
    REQUIRE(yaml_parser.wprojection() == false);
    REQUIRE(yaml_parser.mpi_wstacking() == false);
    REQUIRE(yaml_parser.mpi_all_to_all() == false);
    REQUIRE(yaml_parser.kmeans_iters() == 100);
    REQUIRE(yaml_parser.gpu() == false);
  }
  SECTION("Check the SARA node variables") {
    std::vector<std::string> expected_wavelets = {"Dirac", "DB1", "DB2", "DB3", "DB4",
                                                  "DB5",   "DB6", "DB7", "DB8"};
    REQUIRE(yaml_parser.wavelet_basis() == expected_wavelets);
    REQUIRE(yaml_parser.wavelet_levels() == 4);
    REQUIRE(yaml_parser.algorithm() == "padmm");
  }
  SECTION("Check the AlgorithmOptions node variables") {
    REQUIRE(yaml_parser.epsilonConvergenceScaling() == 1);
    REQUIRE(yaml_parser.realValueConstraint() == true);
    REQUIRE(yaml_parser.positiveValueConstraint() == true);
    REQUIRE(yaml_parser.mpiAlgorithm() == factory::algo_distribution::mpi_serial);
    REQUIRE(yaml_parser.relVarianceConvergence() == 1e-3);
    REQUIRE(yaml_parser.dualFBVarianceConvergence() == 1e-3);
  }
  SECTION("Check the writeOutput method") {
    yaml_parser_m.writeOutput();  // This test config file has a relative path for output, for
                                  // testing purposes
    std::string file_path_save = yaml_parser_m.output_prefix() + "/output_" +
                                 yaml_parser_m.timestamp() + "/test_measurements_config_save.yaml";
    YamlParser yaml_parser_check(file_path_save);
    REQUIRE(yaml_parser_check.filepath() == file_path_save);
    REQUIRE(yaml_parser_check.logging() == yaml_parser_m.logging());
    REQUIRE(yaml_parser_check.iterations() == yaml_parser_m.iterations());
    REQUIRE(yaml_parser_check.epsilonScaling() == yaml_parser_m.epsilonScaling());
    REQUIRE(yaml_parser_check.update_tolerance() == yaml_parser_m.update_tolerance());
    REQUIRE(yaml_parser_check.update_iters() == yaml_parser_m.update_iters());
    REQUIRE(yaml_parser_check.output_prefix() == yaml_parser_m.output_prefix());
    REQUIRE(yaml_parser_check.source() == yaml_parser_m.source());
    REQUIRE(yaml_parser_check.measurements() == yaml_parser_m.measurements());
    REQUIRE(yaml_parser_check.measurements_polarization() ==
            yaml_parser_m.measurements_polarization());
    REQUIRE(yaml_parser_check.measurements_units() == yaml_parser_m.measurements_units());
    REQUIRE(yaml_parser_check.measurements_sigma() == yaml_parser_m.measurements_sigma());
    REQUIRE(yaml_parser_check.skymodel() == yaml_parser_m.skymodel());
    REQUIRE(yaml_parser_check.signal_to_noise() == yaml_parser_m.signal_to_noise());
    REQUIRE(yaml_parser_check.number_of_measurements() == yaml_parser_m.number_of_measurements());
    REQUIRE(yaml_parser_check.w_rms() == yaml_parser_m.w_rms());
    REQUIRE(yaml_parser_check.kernel() == yaml_parser_m.kernel());
    REQUIRE(yaml_parser_check.oversampling() == yaml_parser_m.oversampling());
    REQUIRE(yaml_parser_check.powMethod_iter() == yaml_parser_m.powMethod_iter());
    REQUIRE(yaml_parser_check.powMethod_tolerance() == yaml_parser_m.powMethod_tolerance());
    REQUIRE(yaml_parser_check.cellsizex() == yaml_parser_m.cellsizex());
    REQUIRE(yaml_parser_check.cellsizey() == yaml_parser_m.cellsizey());
    REQUIRE(yaml_parser_check.width() == yaml_parser_m.width());
    REQUIRE(yaml_parser_check.mpi_all_to_all() == yaml_parser_m.mpi_all_to_all());
    REQUIRE(yaml_parser_check.height() == yaml_parser_m.height());
    REQUIRE(yaml_parser_check.Jx() == yaml_parser_m.Jx());
    REQUIRE(yaml_parser_check.Jy() == yaml_parser_m.Jy());
    REQUIRE(yaml_parser_check.gpu() == yaml_parser_m.gpu());
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
