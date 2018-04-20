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
  void parseAndSetInputOutput(YAML::Node node);
  void parseAndSetInput(YAML::Node node);
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
  
  YAML::Node config_file;
};
#endif	/* YAML_PARSER_H */
