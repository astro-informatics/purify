
#include <iostream>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include "yaml-parser.h"
#include <assert.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

bool fexists(const char *filename) {
  std::ifstream ifile(filename);
  return (bool)ifile;
}


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
  
}

void YamlParser::parseAndSetGeneralConfiguration (YAML::Node generalConfigNode) {
  
  this->logging = generalConfigNode["logging"].as<std::string>();
  this->iterations = generalConfigNode["iterations"].as<int>();
  this->epsilonScaling = generalConfigNode["epsilonScaling"].as<int>();
  this->gamma = generalConfigNode["gamma"].as<std::string>();
  // Go one node deeper
  this->parseAndSetInputOutput(generalConfigNode["InputOutput"]);
  
}

void YamlParser::parseAndSetInputOutput (YAML::Node inputOutputNode) {
  
   this->output_prefix = inputOutputNode["output_prefix"].as<std::string>();
   this->skymodel = inputOutputNode["skymodel"].as<std::string>();
   // Go one node deeper
   this->parseAndSetInput(inputOutputNode["input"]);
   
} 

void YamlParser::parseAndSetInput (YAML::Node inputNode) {

   this->measurements = inputNode["measurements"].as<std::string>();
   this->polarization_measurement = inputNode["polarization_measurement"].as<std::string>();
   this->noise_estimate = inputNode["noise_estimate"].as<std::string>();
   this->polarization_noise = inputNode["polarization_noise"].as<std::string>();
   
}


// int main (int argc, char *argv[]) {

//   // Declare the supported options.
//   po::options_description desc(std::string (argv[0]).append("options"));
//   desc.add_options()
//     ("help", "produce help message");
//   po::variables_map args;
//   po::store (po::command_line_parser (argc, argv).options (desc)
//                         .style (po::command_line_style::default_style |
//                                 po::command_line_style::allow_long_disguise)
//                         .run (), args);
//   po::notify (args);
//   if (args.count ("h")) {
//     std::cout << desc << std::endl;
//             return 0;
//         }
//   // po::variables_map vm;
//   // po::store(po::parse_command_line(ac, av, desc), vm);
//   // po::notify(vm);    

//   // if (vm.count("help")) {
//   //   std::cout << desc << "\n";
//   //   return 1;
//   // }

//   // if (vm.count("compression")) {
//   //   std::cout << "Compression level was set to " << vm["compression"].as<int>() << ".\n";
//   // } else {
//   //   std::cout << "Compression level was not set.\n";
//   // }




  

//   // A bit of defensive programming
//   assert (fexists("../data/config.yaml"));

//   // Parsing the YAML file and setting the class variables
//   YamlParser yaml_parser = YamlParser("../data/config.yaml");

//   return 0;
  
// }

