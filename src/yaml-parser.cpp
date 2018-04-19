
#include <iostream>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include <assert.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

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
  int epsilonScaling;
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
  this->parseAndSetMeasureOperators(this->config_file["MeasureOperators"]);
  this->parseAndSetSARA(this->config_file["SARA"]);
  
}


void YamlParser::parseAndSetSARA(YAML::Node SARANode) {

  // Reading the wavelet basis as a string
  std::string values_str = SARANode["wavelet_dict"].as<std::string>();
  // Convert the wavelet basis in a vector<int>
  this->wavelet_basis = this->getWavelets(values_str);
  this->wavelet_levels = SARANode["wavelet_levels"].as<int>();
  this->algorithm = SARANode["algorithm"].as<std::string>();

}

std::vector<int> YamlParser::getWavelets(std::string values_str)
{
  // input - values_str
  // std::string values_str;
  // values_str = "1, 2, 4..6, 11..18, 24, 31..41"; //config["SARA"]["wavelet_dict"].as<std::string>();

  // Logic to extract the values as vectors
  std::vector<int> wavelets;
  std::string value2add;
  values_str.erase(std::remove_if(values_str.begin(), values_str.end(),
                                  [](char x){return std::isspace(x);}), values_str.end());
  int final_value;
  // NOTE Maybe a while reststring and using find is better?
  for (int i=0; i <= values_str.size(); i++) {
    if (i == values_str.size() || values_str[i] == ','){
      wavelets.push_back(std::stoi(value2add));
      value2add = "";
    } else if (values_str[i] == '.') {
      // TODO throw exception if open ended: 9..
      // TODO throw if at the begining
      // TODO throw if 3 digits on side
      int n = values_str[i+3] == ',' ? 2 : 3;
      std::string final_value = values_str.substr(i+2, n);
      // TODO throw if final_value < start value
      for (int j=std::stoi(value2add); j <= std::stoi(final_value); j++ )
        wavelets.push_back(j);
      i += (n + 1);
      value2add = "";
    } else {
      value2add = value2add + values_str[i];
    }
  }

  return wavelets;
}



void YamlParser::parseAndSetMeasureOperators (YAML::Node measureOperatorsNode) {

  this->Jweights = measureOperatorsNode["Jweights"].as<std::string>();
  this->wProjection = measureOperatorsNode["wProjection"].as<bool>();
  this->oversampling = measureOperatorsNode["oversampling"].as<float>();
  this->powMethod_iter = measureOperatorsNode["powMethod_iter"].as<int>();
  this->powMethod_tolerance = measureOperatorsNode["powMethod_tolerance"].as<float>();

  this->parseAndSetPixelSize(measureOperatorsNode["pixelSize"]);
  this->parseAndSetImageSize(measureOperatorsNode["imageSize"]);
  this->parseAndSetJ(measureOperatorsNode["J"]);
  this->parseAndSetWProjectionOptions(measureOperatorsNode["wProjection_options"]);

  
}


void YamlParser::parseAndSetWProjectionOptions(YAML::Node wProjectionOptionsNode) {

  this->chirp_fraction = wProjectionOptionsNode["chirp_fraction"].as<float>();
  this->kernel_fraction = wProjectionOptionsNode["kernel_fraction"].as<float>();
  
}

void YamlParser::parseAndSetJ(YAML::Node Jnode) {

  this->Jx = Jnode["Jx"].as<unsigned int>();
  this->Jy = Jnode["Jy"].as<unsigned int>();

}

void YamlParser::parseAndSetPixelSize(YAML::Node pixelSizeNode) {

  this->Dx = pixelSizeNode["Dx"].as<double>();
  this->Dy = pixelSizeNode["Dy"].as<double>();

}

void YamlParser::parseAndSetImageSize(YAML::Node imageSizeNode) {

  this->x = imageSizeNode["x"].as<int>();
  this->y = imageSizeNode["y"].as<int>();

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


int main (int argc, char *argv[]) {

  // Declare the supported options.
  po::options_description desc(std::string (argv[0]).append("options"));
  desc.add_options()
    ("help", "produce help message");
  po::variables_map args;
  po::store (po::command_line_parser (argc, argv).options (desc)
                        .style (po::command_line_style::default_style |
                                po::command_line_style::allow_long_disguise)
                        .run (), args);
  po::notify (args);
  if (args.count ("h")) {
    std::cout << desc << std::endl;
            return 0;
        }
  // po::variables_map vm;
  // po::store(po::parse_command_line(ac, av, desc), vm);
  // po::notify(vm);    

  // if (vm.count("help")) {
  //   std::cout << desc << "\n";
  //   return 1;
  // }

  // if (vm.count("compression")) {
  //   std::cout << "Compression level was set to " << vm["compression"].as<int>() << ".\n";
  // } else {
  //   std::cout << "Compression level was not set.\n";
  // }




  

  // A bit of defensive programming
  assert (fexists("../data/config.yaml"));

  // Parsing the YAML file and setting the class variables
  YamlParser yaml_parser = YamlParser("../data/config.yaml");

  return 0;
  
}

