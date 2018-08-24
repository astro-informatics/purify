/**
   YamlParser header for purify project
   @author Roland Guichard
   @version 1.0
 */

#ifndef YAML_PARSER_H_
#define YAML_PARSER_H_
#include <assert.h>
#include <fstream>
#include <iostream>
#include "purify/algorithm_factory.h"
#include "yaml-cpp/yaml.h"

/**
   The YamlParser class definition.

   It browses and searches through the config.yaml file in the data folder
   and sets the class variable members accordingly, to be used by the
   purify algorithm.

   @param filepath path to the config file
 */
namespace purify {
class YamlParser {
 public:
  /**
    YamlParser constructor definition

    @param filepath path to config file
  */
  YamlParser(const std::string& filepath);
  /**
    Read configuration file in memory
  */
  void readFile();

  /**
    Set the class members from the YAML inputs
  */
  void setParserVariablesFromYaml();

  /**
    Parse the YAML GeneralConfiguration block
    and set the class members appropriately
  */
  void parseAndSetGeneralConfiguration(const YAML::Node& node);

  /**
    Parse the YAML MeasureOperator block
    and set the class members appropriately
  */
  void parseAndSetMeasureOperators(const YAML::Node& node);

  /**
    Parse the YAML SARA block
    and set the class members appropriately
  */
  void parseAndSetSARA(const YAML::Node& node);

  /**
    Translate the waveket basis string t_into a vector
  */
  std::vector<std::string> getWavelets(const std::string& values_str);

  /**
    Parse the YAML AlgorithmOptions block
    and set the class members appropriately
  */
  void parseAndSetAlgorithmOptions(const YAML::Node& node);

  /**
    Write an output YAML file for the current
    simulation parameters.
  */
  void writeOutput();

  // Variables
#define YAML_MACRO(TYPE, NAME, VALUE) \
 private:                             \
  TYPE NAME##_ = VALUE;               \
                                      \
 public:                              \
  TYPE NAME() { return NAME##_; };

  YAML_MACRO(std::string, filepath, "")
  YAML_MACRO(std::string, timestamp, "")
  YAML_MACRO(std::string, logging, "")
  YAML_MACRO(std::string, algorithm, "")
  YAML_MACRO(factory::algo_distribution, mpiAlgorithm, factory::algo_distribution::serial)
  YAML_MACRO(purify::utilities::vis_source, source, purify::utilities::vis_source::measurements)
  YAML_MACRO(bool, realValueConstraint, true)
  YAML_MACRO(bool, positiveValueConstraint, true)
  YAML_MACRO(t_int, iterations, 0)
  YAML_MACRO(t_int, powMethod_iter, 0)
  YAML_MACRO(t_int, width, 0)
  YAML_MACRO(t_int, height, 0)
  YAML_MACRO(t_int, wavelet_levels, 0)
  YAML_MACRO(t_uint, Jx, 0)
  YAML_MACRO(t_uint, Jy, 0)
  YAML_MACRO(t_real, oversampling, 0)
  YAML_MACRO(t_real, powMethod_tolerance, 0)
  YAML_MACRO(t_real, epsilonScaling, 0)
  YAML_MACRO(t_real, cellsizex, 0)
  YAML_MACRO(t_real, cellsizey, 0)
  YAML_MACRO(t_real, measurements_sigma, 1)
  YAML_MACRO(t_real, signal_to_noise, 30)
  YAML_MACRO(t_real, relVarianceConvergence, 0)
  YAML_MACRO(t_real, epsilonConvergenceScaling, 0)
  YAML_MACRO(std::vector<std::string>, wavelet_basis, {})
  YAML_MACRO(t_int, update_iters, 0)
  YAML_MACRO(t_real, update_tolerance, 0)
  YAML_MACRO(std::string, output_prefix, "")
  YAML_MACRO(std::string, skymodel, "")
  YAML_MACRO(std::vector<std::string>, measurements, {})
  YAML_MACRO(stokes, measurements_polarization, stokes::I)
  YAML_MACRO(utilities::vis_units, measurements_units, utilities::vis_units::radians)
  YAML_MACRO(std::string, kernel, "")

#undef YAML_MACRO
 private:
  YAML::Node config_file;

  template <typename T>
  T get(const YAML::Node& node_map, const std::initializer_list<const char*> indicies);
};
}  // namespace purify
#endif /* YAML_PARSER_H */
