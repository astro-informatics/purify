/**
   YamlParser header for purify project
   @author Roland Guichard
   @version 1.0
 */


#ifndef YAML_PARSER_H_
#define YAML_PARSER_H_
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <assert.h>

#include "utilities.h"

/**
   The YamlParser class definition.

   It browses and searches through the config.yaml file in the data folder
   and sets the class variable members accordingly, to be used by the 
   purify algorithm.

   @param filepath path to the config file
 */

class YamlParser
{
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
    Translate the waveket basis string into a vector
  */
  std::vector<std::string> getWavelets(std::string values_str);

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
  private: \
  TYPE NAME##_ =  VALUE; \
  public: \
  TYPE NAME(){ return NAME##_;};

  YAML_MACRO(std::string, filepath, "");
  YAML_MACRO(std::string, timestamp, "");
  YAML_MACRO(std::string, logging, "");
  YAML_MACRO(std::string, algorithm, "");
  YAML_MACRO(std::string, mpiAlgorithm, "");
  YAML_MACRO(std::string, param1, "");
  YAML_MACRO(std::string, param2, "");
  YAML_MACRO(bool, realValueConstraint, true);
  YAML_MACRO(bool, positiveValueConstraint, true);
  YAML_MACRO(int, iterations, 0);
  YAML_MACRO(int, powMethod_iter, 0);
  YAML_MACRO(int, x, 0);
  YAML_MACRO(int, y, 0);
  YAML_MACRO(int, wavelet_levels, 0);
  YAML_MACRO(int, epsilonConvergenceScaling, 0);
  YAML_MACRO(unsigned int, Jx, 0);
  YAML_MACRO(unsigned int, Jy, 0);
  YAML_MACRO(float, oversampling, 0);
  YAML_MACRO(float, powMethod_tolerance, 0);
  YAML_MACRO(float, chirp_fraction, 0);
  YAML_MACRO(float, kernel_fraction, 0);
  YAML_MACRO(double, epsilonScaling, 0);
  YAML_MACRO(double, Dx, 0);
  YAML_MACRO(double, Dy, 0);
  YAML_MACRO(double, relVarianceConvergence, 0);
  YAML_MACRO(std::vector<std::string>, wavelet_basis, {});  
  YAML_MACRO(std::string, gamma, "");
  YAML_MACRO(std::string, output_prefix, "");
  YAML_MACRO(std::string, skymodel, "");
  YAML_MACRO(std::vector<std::string>, measurements, {});
  YAML_MACRO(std::string, polarization_measurement, "");
  YAML_MACRO(purify::utilities::vis_units, units_measurement, purify::utilities::vis_units::radians);
  YAML_MACRO(std::string, noise_estimate, "");
  YAML_MACRO(std::string, polarization_noise, "");
  YAML_MACRO(std::string, Jweights, "");
  YAML_MACRO(bool, wProjection, true);

#undef YAML_MACRO

  YAML::Node config_file;
};			       
#endif /* YAML_PARSER_H */
