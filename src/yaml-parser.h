#ifndef YAML_PARSER_H_
#define YAML_PARSER_H_
#include <iostream>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include <assert.h>
#include <boost/program_options.hpp>

class YamlParser {

 public:
  // Constructor
  YamlParser(std::string filename);
  // Methods
  void readFile();
  void setParserVariablesFromYaml();
  void parseAndSetGeneralConfiguration(YAML::Node node);
  void parseAndSetMeasureOperators(YAML::Node node);
  void parseAndSetInputOutput(YAML::Node node);
  void parseAndSetInput(YAML::Node node);
  void parseAndSetPixelSize(YAML::Node node);
  void parseAndSetImageSize(YAML::Node node);
  void parseAndSetJ(YAML::Node node);
  void parseAndSetWProjectionOptions(YAML::Node node);
  void parseAndSetSARA(YAML::Node node);
  std::vector<int> getWavelets(std::string values_str);
  // Variables
  std::string filename;
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
  
  YAML::Node config_file;
};			       
#endif /* YAML_PARSER_H */
