/** 
    YamlParser definition for the purify project
    @author Roland Guichard
    @version 1.0
*/
#include <boost/filesystem.hpp>
#include <typeinfo>
#include <chrono>
#include <ctime>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <fstream>
#include <yaml-cpp/yaml.h>
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
  try
    {
      YAML::Node config = YAML::LoadFile(this->filepath_);
      // A bit of defensive programming
      assert(config.Type() == YAML::NodeType::Map);
      this->config_file = config;
    }
  catch(YAML::BadFile& exception)
    {
      const std::string current_path = boost::filesystem::current_path().native();
      throw (std::runtime_error("Runtime error while trying to find config.yaml. The input file path " + this->filepath_ + " could not be found from " + current_path));
    }
}

template <typename T>
T get(const YAML::Node& node_map, const std::initializer_list<const char*> indicies)
{
  YAML::Node node = YAML::Clone (node_map);
  std::string faulty_variable;
  for (const char* index : indicies)
    {
      faulty_variable = std::string(index);	  
      node = node[index];
      if (!node.IsDefined())
	{
	  throw std::runtime_error("The initialisation of " + faulty_variable + " is wrong.");
	}
    }
  try
    {
      if(node.size() > 1)
	throw std::runtime_error("The node has more than one element.");
      return node.as<T>();
    }
  catch (std::exception e)
    {
      throw std::runtime_error("There is a mismatch in the type conversion of " + faulty_variable);
    }
}


template <typename T>
T get_vector(const YAML::Node& node_map, const std::initializer_list<const char*> indicies)
{
  YAML::Node node = YAML::Clone (node_map);
  std::string faulty_variable;
  for (const char* index : indicies)
    {
      faulty_variable = std::string(index);	  
      node = node[index];
      if (!node.IsDefined())
	{
	  throw std::runtime_error("The initialisation of " + faulty_variable + " is wrong.");
	}
    }
  try
    {
      T output;
      for (int i=0; i < node.size(); i++)
	output.push_back(node[i].as<typename T::value_type>());
      return output;
    }
  catch (std::exception e)
    {
      throw std::runtime_error("There is a mismatch in the type conversion of " + faulty_variable);
    }
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
  this->logging_ = get<std::string>(generalConfigNode, {"logging"});
  this->iterations_ = get<int>(generalConfigNode, {"iterations"});
  this->epsilonScaling_ = get<int>(generalConfigNode, {"epsilonScaling"});
  this->gamma_ = get<std::string>(generalConfigNode, {"gamma"});
  this->output_prefix_ = get<std::string>(generalConfigNode, {"InputOutput", "output_prefix"});
  this->skymodel_ = get<std::string>(generalConfigNode,{"InputOutput", "skymodel"});
  // YAML::Node measurement_seq = generalConfigNode["InputOutput"]["input"]["measurements"];
  // for (int i=0; i < measurement_seq.size(); i++)
  this->measurements_ = get_vector<std::vector<std::string>>(generalConfigNode, {"InputOutput", "input", "measurements"});

  this->polarization_measurement_ = get<std::string>(generalConfigNode,{"InputOutput", "input", "polarization_measurement"});
  this->noise_estimate_ = get<std::string>(generalConfigNode, {"InputOutput", "input", "noise_estimate"});
  this->polarization_noise_ = get<std::string>(generalConfigNode, {"InputOutput", "input", "polarization_noise"});
  
}

void YamlParser::parseAndSetMeasureOperators (const YAML::Node& measureOperatorsNode)
{
  this->Jweights_ = get<std::string>(measureOperatorsNode, {"Jweights"});
  this->wProjection_ = get<bool>(measureOperatorsNode, {"wProjection"});
  this->oversampling_ = get<float>(measureOperatorsNode, {"oversampling"});
  this->powMethod_iter_ = get<int>(measureOperatorsNode, {"powMethod_iter"});
  this->powMethod_tolerance_ =get<float>(measureOperatorsNode, {"powMethod_tolerance"});
  this->Dx_ = get<double>(measureOperatorsNode, {"pixelSize", "Dx"});
  this->Dy_ = get<double>(measureOperatorsNode, {"pixelSize", "Dy"});
  this->x_ = get<int>(measureOperatorsNode, {"imageSize", "x"});
  this->y_ = get<int>(measureOperatorsNode, {"imageSize", "y"});
  this->Jx_ = get<unsigned int>(measureOperatorsNode, {"J", "Jx"});
  this->Jy_ = get<unsigned int>(measureOperatorsNode, {"J", "Jy"});
  this->chirp_fraction_ = get<float>(measureOperatorsNode, {"wProjection_options", "chirp_fraction"});
  this->kernel_fraction_ = get<float>(measureOperatorsNode, {"wProjection_options", "kernel_fraction"});
}

void YamlParser::parseAndSetSARA (const YAML::Node& SARANode)
{
  std::string values_str = SARANode["wavelet_dict"].as<std::string>();
  this->wavelet_basis_ = this->getWavelets(values_str);
  this->wavelet_levels_ = get<int>(SARANode, {"wavelet_levels"});
  this->algorithm_ = get<std::string>(SARANode, {"algorithm"});
}

void YamlParser::parseAndSetAlgorithmOptions (const YAML::Node& algorithmOptionsNode)
{
  this->epsilonConvergenceScaling_ = get<int>(algorithmOptionsNode, {"padmm", "epsilonConvergenceScaling"});
  this->realValueConstraint_ = get<bool>(algorithmOptionsNode, {"padmm", "realValueConstraint"});
  this->positiveValueConstraint_ = get<bool>(algorithmOptionsNode, {"padmm", "positiveValueConstraint"});
  this->mpiAlgorithm_ = get<std::string>(algorithmOptionsNode, {"padmm", "mpiAlgorithm"});
  this->relVarianceConvergence_ = get<double>(algorithmOptionsNode, {"padmm", "relVarianceConvergence"});  
  this->param1_ = get<std::string>(algorithmOptionsNode, {"pd", "param1"});
  this->param2_ = get<std::string>(algorithmOptionsNode, {"pd", "param2"});
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

void YamlParser::writeOutput(const std::string& folder_path)
{

  std::time_t t = std::time(0);   // get time now
  std::tm* now = std::localtime(&t);
  // Make the datetime human readable
  std::string datetime = std::to_string(now->tm_year + 1900) + '-' + std::to_string(now->tm_mon + 1) + '-' + std::to_string(now->tm_mday);
  datetime = datetime + '-' + std::to_string(now->tm_hour) + ':' + std::to_string(now->tm_min) + ':' + std::to_string(now->tm_sec);

  this->timestamp_ = datetime;

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
  output_file.open(folder_path + "/config_" + datetime + "_save.yaml");
  output_file << out.c_str();
  output_file.close();

}
