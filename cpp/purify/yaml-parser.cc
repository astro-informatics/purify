/** 
    YamlParser definition for the purify project
    @author Roland Guichard
    @version 1.0
*/

#include <typeinfo>
#include <chrono>
#include <ctime>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <boost/filesystem.hpp>
#include "yaml-parser.h"
#include <assert.h>

YamlParser::YamlParser (const std::string& filepath)
  : filepath_(filepath)
{
  // Reading the file
  this->readFile();
  // Run a batch of methods to parse the YAML and set the
  // class members accordingly
  this->setParserVariablesFromYaml();
}

void YamlParser::readFile ()
{
  YAML::Node config = YAML::LoadFile(this->filepath_);
  // A bit of defensive programming
  assert(config.Type() == YAML::NodeType::Map);
  this->config_file = config;
}

void YamlParser::setParserVariablesFromYaml ()
{
  // Each of these methods correspond to blocks in the YAML
  this->parseAndSetGeneralConfiguration(this->config_file["GeneralConfiguration"]);
  this->parseAndSetMeasureOperators(this->config_file["MeasureOperators"]);
  this->parseAndSetSARA(this->config_file["SARA"]);
  this->parseAndSetAlgorithmOptions(this->config_file["AlgorithmOptions"]); 
}

void YamlParser::parseAndSetGeneralConfiguration (const YAML::Node& generalConfigNode)
{  
  this->logging_ = generalConfigNode["logging"].as<std::string>();
  this->iterations_ = generalConfigNode["iterations"].as<int>();
  this->epsilonScaling_ = generalConfigNode["epsilonScaling"].as<int>();
  this->gamma_ = generalConfigNode["gamma"].as<std::string>();
  
  this->output_path_ = generalConfigNode["InputOutput"]["output_path"].as<std::string>();
  
  std::string source_str = generalConfigNode["InputOutput"]["input"]["source"].as<std::string>();
  if (source_str=="measurements") {
    if (generalConfigNode["InputOutput"]["input"]["simulation"])
      throw std::runtime_error("Expecting only the input measurements block in the configuration file. Please remove simulation block!");
    this->source_ = purify::utilities::vis_source::measurements;
    YAML::Node measurement_seq = generalConfigNode["InputOutput"]["input"]["measurements"]["measurements_files"];
    for (int i=0; i < measurement_seq.size(); i++)
      // TODO: check if files exist, and remove from list if they don't (see read_measurements.cc)
      this->measurements_files_.push_back(measurement_seq[i].as<std::string>());
    // TODO: use the enum instead of string.
    this->measurements_polarization_ = generalConfigNode["InputOutput"]["input"]["measurements"]["measurements_polarization"].as<std::string>();
    std::string units_measurement_str = generalConfigNode["InputOutput"]["input"]["measurements"]["measurements_units"].as<std::string>();
    if (units_measurement_str=="lambda")
      this->measurements_units_ = purify::utilities::vis_units::lambda;
    else if (units_measurement_str=="radians")
      this->measurements_units_ = purify::utilities::vis_units::radians;
    else if (units_measurement_str=="pixels")
      this->measurements_units_ = purify::utilities::vis_units::pixels;
    else
      throw std::runtime_error("Visibility units \""+units_measurement_str+"\" not recognised. Check your config file.");
    this->measurements_sigma_ = generalConfigNode["InputOutput"]["input"]["measurements"]["measurements_sigma"].as<float>();
  }
  else if (source_str=="simulation") {
    if (generalConfigNode["InputOutput"]["input"]["measurements"])
      throw std::runtime_error("Expecting only the input simulation block in the configuration file. Please remove measurements block!");
    this->source_ = purify::utilities::vis_source::simulation;
    this->skymodel_ = generalConfigNode["InputOutput"]["input"]["simulation"]["skymodel"].as<std::string>();
    this->signal_to_noise_ = generalConfigNode["InputOutput"]["input"]["simulation"]["signal_to_noise"].as<float>();
  }
  else
    throw std::runtime_error("Visibility source \""+source_str+"\" not recognised. Check your config file.");
}

void YamlParser::parseAndSetMeasureOperators (const YAML::Node& measureOperatorsNode)
{
  this->Jweights_ = measureOperatorsNode["Jweights"].as<std::string>();
  this->wProjection_ = measureOperatorsNode["wProjection"].as<bool>();
  this->oversampling_ = measureOperatorsNode["oversampling"].as<float>();
  this->powMethod_iter_ = measureOperatorsNode["powMethod_iter"].as<int>();
  this->powMethod_tolerance_ = measureOperatorsNode["powMethod_tolerance"].as<float>();
  this->Dx_ = measureOperatorsNode["pixelSize"]["Dx"].as<double>();
  this->Dy_ = measureOperatorsNode["pixelSize"]["Dy"].as<double>();
  this->x_ = measureOperatorsNode["imageSize"]["x"].as<int>();
  this->y_ = measureOperatorsNode["imageSize"]["y"].as<int>();
  this->Jx_ = measureOperatorsNode["J"]["Jx"].as<unsigned int>();
  this->Jy_ = measureOperatorsNode["J"]["Jy"].as<unsigned int>();
  this->chirp_fraction_ = measureOperatorsNode["wProjection_options"]["chirp_fraction"].as<float>();
  this->kernel_fraction_ = measureOperatorsNode["wProjection_options"]["kernel_fraction"].as<float>();
}

void YamlParser::parseAndSetSARA (const YAML::Node& SARANode)
{
  std::string values_str = SARANode["wavelet_dict"].as<std::string>();
  this->wavelet_basis_ = this->getWavelets(values_str);
  this->wavelet_levels_ = SARANode["wavelet_levels"].as<int>();
  this->algorithm_ = SARANode["algorithm"].as<std::string>();
}

void YamlParser::parseAndSetAlgorithmOptions (const YAML::Node& algorithmOptionsNode)
{
  this->epsilonConvergenceScaling_ = algorithmOptionsNode["padmm"]["epsilonConvergenceScaling"].as<int>();
  this->realValueConstraint_ = algorithmOptionsNode["padmm"]["realValueConstraint"].as<bool>();
  this->positiveValueConstraint_ = algorithmOptionsNode["padmm"]["positiveValueConstraint"].as<bool>();
  this->mpiAlgorithm_ = algorithmOptionsNode["padmm"]["mpiAlgorithm"].as<std::string>();
  this->relVarianceConvergence_ = algorithmOptionsNode["padmm"]["relVarianceConvergence"].as<double>();  
  this->param1_ = algorithmOptionsNode["pd"]["param1"].as<std::string>();
  this->param2_ = algorithmOptionsNode["pd"]["param2"].as<std::string>();
}

std::vector<std::string> YamlParser::getWavelets(std::string values_str)
{
  // input - values_str
  // std::string values_str;
  // values_str = "1, 2, 4..6, 11..18, 24, 31..41"; //config["SARA"]["wavelet_dict"].as<std::string>();

  // Logic to extract the values as vectors
  std::vector<std::string> wavelets;
  std::string value2add;
  values_str.erase(std::remove_if(values_str.begin(), values_str.end(),
                                  [](char x){return std::isspace(x);}), values_str.end());
  // NOTE Maybe a while reststring and using find is better?
  for (int i=0; i <= values_str.size(); i++) {
    if (i == values_str.size() || values_str[i] == ','){
      wavelets.push_back( value2add=="0" ? "Dirac" : "DB"+value2add );
      value2add = "";
    } else if (values_str[i] == '.') {
      // TODO throw exception if open ended: 9..
      // TODO throw if at the begining
      // TODO throw if 3 digits on side
      int n = values_str[i+3] == ',' ? 2 : 3;
      std::string final_value = values_str.substr(i+2, n);
      // TODO throw if final_value < start value
      for (int j=std::stoi(value2add); j <= std::stoi(final_value); j++ )
        wavelets.push_back( j==0 ? "Dirac" : "DB"+std::to_string(j) );
      i += (n + 1);
      value2add = "";
    } else {
      value2add = value2add + values_str[i];
    }
  }

  return wavelets;
}

void YamlParser::writeOutput()
{
  // Get base file name (without path or extension)
  std::size_t file_begin = filepath_.find_last_of("/");
  if (file_begin==std::string::npos) file_begin=0;
  std::string file_path = filepath_.substr(0,file_begin);
  std::string extension = ".yaml";
  std::string base_file_name = this->filepath_.erase(this->filepath_.size()-extension.size());
  base_file_name = base_file_name.substr((file_path.size() ? file_path.size()+1 : 0), base_file_name.size());

  // Get timestamp string
  std::time_t t = std::time(0);   // get time now
  std::tm* now = std::localtime(&t);
  // Make the datetime human readable
  std::string datetime = std::to_string(now->tm_year + 1900) + '-' + std::to_string(now->tm_mon + 1) + '-' + std::to_string(now->tm_mday);
  datetime = datetime + '-' + std::to_string(now->tm_hour) + ':' + std::to_string(now->tm_min) + ':' + std::to_string(now->tm_sec);

  this->timestamp_ = datetime;

  // Construct output directory structure and file name
  boost::filesystem::path const path(this->output_path_);
  auto const out_path = path / ("output_"+datetime);
  boost::filesystem::create_directories(out_path);
  std::string out_filename = (out_path / base_file_name).native() + "_save.yaml";

  // Write out the yaml info
  YAML::Emitter out;
  out << YAML::BeginMap;
  out << YAML::Key << "GeneralConfiguration";
  out << this->config_file["GeneralConfiguration"];
  out << YAML::Key << "MeasureOperators";
  out << this->config_file["MeasureOperators"];
  out << YAML::Key << "SARA";
  out << this->config_file["SARA"];
  out << YAML::Key << "AlgorithmOptions";
  out << this->config_file["AlgorithmOptions"];
  out << YAML::EndMap;  

  std::ofstream output_file;
  output_file.open(out_filename);
  output_file << out.c_str();
  output_file.close();

}
