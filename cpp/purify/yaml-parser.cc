/**
    YamlParser definition for the purify project
    @author Roland Guichard
    @version 1.0
*/
#include "purify/yaml-parser.h"
#include <algorithm>
#include <assert.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <typeinfo>
#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>
#include "purify/read_measurements.h"

namespace purify {
YamlParser::YamlParser(const std::string& filepath) : filepath_(filepath) {
  // Reading the file
  this->readFile();
  // Run a batch of methods to parse the YAML and set the
  // class members accordingly
  this->setParserVariablesFromYaml();
  // Setting time stamp
  // Get timestamp string
  std::time_t t = std::time(0);  // get time now
  std::tm* now = std::localtime(&t);
  // Make the datetime human readable
  std::string datetime = std::to_string(now->tm_year + 1900) + '-' +
                         std::to_string(now->tm_mon + 1) + '-' + std::to_string(now->tm_mday);
  datetime = datetime + '-' + std::to_string(now->tm_hour) + ':' + std::to_string(now->tm_min) +
             ':' + std::to_string(now->tm_sec);

  this->timestamp_ = datetime;
}

void YamlParser::readFile() {
  try {
    YAML::Node config = YAML::LoadFile(this->filepath());
    // A bit of defensive programming
    assert(config.Type() == YAML::NodeType::Map);
    this->config_file = config;
  } catch (YAML::BadFile& exception) {
    const std::string current_path = boost::filesystem::current_path().native();
    throw(
        std::runtime_error("Runtime error while trying to find config.yaml. The input file path " +
                           this->filepath() + " could not be found from " + current_path));
  }
}

template <typename T>
T YamlParser::get(const YAML::Node& node_map, const std::initializer_list<const char*> indicies) {
  YAML::Node node = YAML::Clone(node_map);
  std::string faulty_variable;
  for (const char* index : indicies) {
    faulty_variable = std::string(index);
    node = node[index];
    if (!node.IsDefined()) {
      throw std::runtime_error("The initialisation of " + faulty_variable + " is wrong in config " +
                               this->filepath());
    }
  }
  try {
    if (node.size() > 1) throw std::runtime_error("The node has more than one element.");
    return node.as<T>();
  } catch (std::exception e) {
    throw std::runtime_error("There is a mismatch in the type conversion of " + faulty_variable +
                             " of " + this->filepath());
  }
}

template <typename T>
T get_vector(const YAML::Node& node_map, const std::initializer_list<const char*> indicies) {
  YAML::Node node = YAML::Clone(node_map);
  std::string faulty_variable;
  for (const char* index : indicies) {
    faulty_variable = std::string(index);
    node = node[index];
    if (!node.IsDefined()) {
      throw std::runtime_error("The initialisation of " + faulty_variable + " is wrong.");
    }
  }
  try {
    T output;
    for (int i = 0; i < node.size(); i++) output.push_back(node[i].as<typename T::value_type>());
    return output;
  } catch (std::exception e) {
    throw std::runtime_error("There is a mismatch in the type conversion of " + faulty_variable);
  }
}

void YamlParser::setParserVariablesFromYaml() {
  // Each of these methods correspond to blocks in the YAML
  this->parseAndSetGeneralConfiguration(this->config_file["GeneralConfiguration"]);
  this->parseAndSetMeasureOperators(this->config_file["MeasureOperators"]);
  this->parseAndSetSARA(this->config_file["SARA"]);
  this->parseAndSetAlgorithmOptions(this->config_file["AlgorithmOptions"]);
}

void YamlParser::parseAndSetGeneralConfiguration(const YAML::Node& generalConfigNode) {
  this->logging_ = get<std::string>(generalConfigNode, {"logging"});
  this->iterations_ = get<int>(generalConfigNode, {"iterations"});
  this->epsilonScaling_ = get<t_real>(generalConfigNode, {"epsilonScaling"});
  this->update_iters_ = get<t_int>(generalConfigNode, {"stepsize", "update_iters"});
  this->update_tolerance_ = get<t_real>(generalConfigNode, {"stepsize", "update_tolerance"});
  this->output_prefix_ = get<std::string>(generalConfigNode, {"InputOutput", "output_prefix"});

  const std::string source_str =
      get<std::string>(generalConfigNode, {"InputOutput", "input", "source"});
  if (source_str == "measurements") {
    this->measurements_polarization_ = stokes_string.at(get<std::string>(
        generalConfigNode, {"InputOutput", "input", "measurements", "measurements_polarization"}));
    if (generalConfigNode["InputOutput"]["input"]["simulation"])
      throw std::runtime_error(
          "Expecting only the input measurements block in the configuration file. Please remove "
          "simulation block!");
    this->source_ = purify::utilities::vis_source::measurements;
    this->measurements_ = get_vector<std::vector<std::string>>(
        generalConfigNode, {"InputOutput", "input", "measurements", "measurements_files"});
    // TODO: use the enum instead of string.
    const std::string units_measurement_str = get<std::string>(
        generalConfigNode, {"InputOutput", "input", "measurements", "measurements_units"});
    if (units_measurement_str == "lambda")
      this->measurements_units_ = purify::utilities::vis_units::lambda;
    else if (units_measurement_str == "radians")
      this->measurements_units_ = purify::utilities::vis_units::radians;
    else if (units_measurement_str == "pixels")
      this->measurements_units_ = purify::utilities::vis_units::pixels;
    else
      throw std::runtime_error("Visibility units \"" + units_measurement_str +
                               "\" not recognised. Check your config file.");
    this->measurements_sigma_ = get<t_real>(
        generalConfigNode, {"InputOutput", "input", "measurements", "measurements_sigma"});
  } else if (source_str == "simulation") {
    if (generalConfigNode["InputOutput"]["input"]["measurements"])
      throw std::runtime_error(
          "Expecting only the input simulation block in the configuration file. Please remove "
          "measurements block!");
    this->source_ = purify::utilities::vis_source::simulation;
    this->skymodel_ =
        get<std::string>(generalConfigNode, {"InputOutput", "input", "simulation", "skymodel"});
    this->signal_to_noise_ =
        get<t_real>(generalConfigNode, {"InputOutput", "input", "simulation", "signal_to_noise"});
  } else
    throw std::runtime_error("Visibility source \"" + source_str +
                             "\" not recognised. Check your config file.");
}

void YamlParser::parseAndSetMeasureOperators(const YAML::Node& measureOperatorsNode) {
  this->kernel_ = get<std::string>(measureOperatorsNode, {"kernel"});
  this->oversampling_ = get<float>(measureOperatorsNode, {"oversampling"});
  this->powMethod_iter_ = get<int>(measureOperatorsNode, {"powMethod_iter"});
  this->powMethod_tolerance_ = get<float>(measureOperatorsNode, {"powMethod_tolerance"});
  this->cellsizex_ = get<double>(measureOperatorsNode, {"pixelSize", "cellsizex"});
  this->cellsizey_ = get<double>(measureOperatorsNode, {"pixelSize", "cellsizey"});
  this->width_ = get<int>(measureOperatorsNode, {"imageSize", "width"});
  this->height_ = get<int>(measureOperatorsNode, {"imageSize", "height"});
  this->Jx_ = get<unsigned int>(measureOperatorsNode, {"J", "Jx"});
  this->Jy_ = get<unsigned int>(measureOperatorsNode, {"J", "Jy"});
  this->Jw_ = get<unsigned int>(measureOperatorsNode, {"J", "Jw"});
  this->wprojection_ = get<bool>(measureOperatorsNode, {"wprojection"});
  this->mpi_wstacking_ = get<bool>(measureOperatorsNode, {"mpi_wstacking"});
}

void YamlParser::parseAndSetSARA(const YAML::Node& SARANode) {
  const std::string values_str = get<std::string>(SARANode, {"wavelet_dict"});
  this->wavelet_basis_ = this->getWavelets(values_str);
  this->wavelet_levels_ = get<t_int>(SARANode, {"wavelet_levels"});
}

void YamlParser::parseAndSetAlgorithmOptions(const YAML::Node& algorithmOptionsNode) {
  this->algorithm_ = get<std::string>(algorithmOptionsNode, {"algorithm"});
  if (this->algorithm_ != "padmm")
    throw std::runtime_error(
        "Only padmm algorithm configured for now. Please fill the appropriate block in the "
        "configuration file.");
  this->epsilonConvergenceScaling_ =
      get<t_int>(algorithmOptionsNode, {"padmm", "epsilonConvergenceScaling"});
  this->realValueConstraint_ = get<bool>(algorithmOptionsNode, {"padmm", "realValueConstraint"});
  this->positiveValueConstraint_ =
      get<bool>(algorithmOptionsNode, {"padmm", "positiveValueConstraint"});
  this->mpiAlgorithm_ = factory::algo_distribution_string.at(
      get<std::string>(algorithmOptionsNode, {"padmm", "mpiAlgorithm"}));
  this->relVarianceConvergence_ =
      get<t_real>(algorithmOptionsNode, {"padmm", "relVarianceConvergence"});
  this->dualFBVarianceConvergence_ =
      get<t_real>(algorithmOptionsNode, {"padmm", "dualFBVarianceConvergence"});
}

std::vector<std::string> YamlParser::getWavelets(const std::string& values_str) {
  // input - values_str
  // std::string values_str;
  // values_str = "1, 2, 4..6, 11..18, 24, 31..41";
  // //config["SARA"]["wavelet_dict"].as<std::string>();

  // Logic to extract the values as vectors
  std::vector<std::string> wavelets;
  std::string value2add;
  std::string input = values_str;
  input.erase(std::remove_if(input.begin(), input.end(), [](char x) { return std::isspace(x); }),
              input.end());
  // NOTE Maybe a while reststring and using find is better?
  for (int i = 0; i <= input.size(); i++) {
    if ((i == input.size()) || (input.at(i) == ',')) {
      wavelets.push_back((value2add == "0") ? "Dirac" : ("DB" + value2add));
      value2add = "";
    } else if (input.at(i) == '.') {
      // TODO throw exception if open ended: 9..
      // TODO throw if at the begining
      // TODO throw if 3 digits on side
      const int n = ((i + 3) >= input.size()) ? 2 : ((input.at(i + 3) == ',') ? 2 : 3);
      const std::string final_value = input.substr(i + 2, n);
      // TODO throw if final_value < start value
      for (int j = std::stoi(value2add); j <= std::stoi(final_value); j++)
        wavelets.push_back((j == 0) ? "Dirac" : ("DB" + std::to_string(j)));
      i += (n + 1);
      value2add = "";
    } else {
      value2add = value2add + input.at(i);
    }
  }

  return wavelets;
}

void YamlParser::writeOutput() {
  // Get base file name (without path or extension)
  std::size_t file_begin = this->filepath_.find_last_of("/");
  if (file_begin == std::string::npos) file_begin = 0;
  std::string file_path = filepath_.substr(0, file_begin);
  std::string extension = ".yaml";
  std::string base_file_name = this->filepath_.erase(this->filepath_.size() - extension.size());
  base_file_name =
      base_file_name.substr((file_path.size() ? file_path.size() + 1 : 0), base_file_name.size());
  // Construct output directory structure and file name
  boost::filesystem::path const path(this->output_prefix_);
  std::string const out_path = output_prefix_ + "/output_" + std::string(this->timestamp());
  mkdir_recursive(out_path);
  std::string out_filename = out_path + "/" + base_file_name + "_save.yaml";

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
}  // namespace purify
