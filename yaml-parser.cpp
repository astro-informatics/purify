
#include <iostream>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include <assert.h>


bool fexists(const char *filename) {
  std::ifstream ifile(filename);
  return (bool)ifile;
}

class YamlParser {

 public:
  // Constructor
  YamlParser(std::string filename);
  // Methods
  void readFile();
  void setParserVariablesFromYaml();
  void parseAndSetGeneralConfiguration(YAML::Node node);
  // Variables
  std::string filename;
  std::string logging;
  int iterations;
  int epsilonScaling;
  std::string gamma;
  YAML::Node config_file;
};

// YamlParser constructor definition
YamlParser::YamlParser (std::string filename) {
  // Setting the variable
  this->filename = filename;
  // Reading the file
  this->readFile();
  this->setParserVariablesFromYaml();
}

// Method for reading file definition
void YamlParser::readFile () {
  YAML::Node config = YAML::LoadFile(this->filename);
  // A bit of defensive programming
  assert(config.Type() == YAML::NodeType::Map);
  this->config_file = config;
}

// Method for setting the class variables definition
void YamlParser::setParserVariablesFromYaml () {
  
  this->parseAndSetGeneralConfiguration(this->config_file["GeneralConfiguration"]);
  
    //   for (YAML::const_iterator iter = generalConfigNode.begin(); iter != generalConfigNode.end(); ++iter) {
    //   // Key k = iter.first;
    //   std::cout << iter->first << std::endl;
    //   std::cout << generalConfigNode[iter->first] << std::endl;   
    // }

}

void YamlParser::parseAndSetGeneralConfiguration (YAML::Node generalConfigNode) {
  
  // for (YAML::const_iterator iter = generalConfigNode.begin(); iter != generalConfigNode.end(); ++iter) {
  //     std::cout << iter->first << std::endl;
  //     std::cout << generalConfigNode[iter->first] << std::endl;   
  //   }

  this->logging = generalConfigNode["logging"].as<std::string>();
  this->iterations = generalConfigNode["iterations"].as<int>();
  this->epsilonScaling = generalConfigNode["epsilonScaling"].as<int>();
  this->gamma = generalConfigNode["gamma"].as<std::string>();
  
}

int main () {

  // A bit of defensive programming
  assert (fexists("config.yaml"));

  // Parsing the YAML file and setting the class variables
  YamlParser yaml_parser = YamlParser("config.yaml");

  return 0;
  
}

