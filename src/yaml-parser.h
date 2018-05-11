/**
   YamlParser header for purify project
   @author Roland Guichard
   @version 1.0
 */


#ifndef YAML_PARSER_H_
#define YAML_PARSER_H_
#include <iostream>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include <assert.h>
#include <boost/program_options.hpp>


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
  // Constructor
  YamlParser(std::string &filepath);
  // Methods
  void readFile();
  void setParserVariablesFromYaml();
  void parseAndSetGeneralConfiguration(YAML::Node node);
  void parseAndSetMeasureOperators(YAML::Node node);
  void parseAndSetSARA(YAML::Node node);
  std::vector<int> getWavelets(std::string values_str);
  void parseAndSetAlgorithmOptions(YAML::Node node);
  
  // Variables
  std::string filepath;
  std::string logging;
  int iterations;
  double epsilonScaling;
  std::string gamma;
  std::string output_prefix;
  std::string skymodel;
  std::string measurements;
  std::string polarization_measurement;
  std::string noise_estimate;
  std::string polarization_noise;
  std::string Jweights;
  bool wProjection;
  float oversampling;
  int powMethod_iter;
  float powMethod_tolerance;
  double Dx;
  double Dy;
  int x;
  int y;
  unsigned int Jx;
  unsigned int Jy;
  float chirp_fraction;
  float kernel_fraction;
  std::vector<int> wavelet_basis;
  int wavelet_levels;
  std::string algorithm;
  int epsilonConvergenceScaling;
  bool realValueConstraint;
  bool positiveValueConstraint;
  std::string mpiAlgorithm;
  double relVarianceConvergence;
  std::string param1;
  std::string param2;
  
  YAML::Node config_file;  
};			       
#endif /* YAML_PARSER_H */
