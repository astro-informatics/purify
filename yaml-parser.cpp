
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
  void parseAndSetGeneralConfiguration();
  // Variables
  std::string filename;
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
  // for (std::size_t i=0; i<this->config_file.size(); i++) {
  //   std::cout << "i from iteration " << i << std::endl;
  //   std::cout << this->config_file[i] << std::endl;
  // }

  this->parseAndSetGeneralConfiguration(this->config_file["GeneralConfiguration"])

  
    // std::cout << this->config_file["GeneralConfiguration"][0] << std::endl;


    // for (YAML::const_iterator iter = config_file.begin(); iter != config_file.end(); ++iter) {
    //   // Key k = iter.first;
    //   std::cout << iter->first << std::endl;
    //   std::cout << config_file[iter->first] << std::endl;   
    // }

}

void Yaml::parseAndSetGeneralConfiguration (YAML::node generalConfigurationNode) {
    for (YAML::const_iterator iter = {generalConfigurationNode.begin(); iter != generalConfigurationNode.end(); ++iter) {
      // Key k = iter.first;
      std::cout << iter->first << std::endl;
      std::cout << config_file[iter->first] << std::endl;   
    }
  

}

int main () {

  // A bit of defensive programming
  assert (fexists("config.yaml"));

  // Parsing the YAML file and setting the class variables
  YamlParser yaml_parser = YamlParser("config.yaml");

  return 0;
  
}

