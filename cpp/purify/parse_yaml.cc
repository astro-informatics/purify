#include "parse_yaml.h"
#include <yaml-cpp/yaml.h>
#include <string>
#include <algorithm>

using namespace std;

Parse_Yaml::Parse_Yaml()
{
  // string input_file = "config.yaml";
  config_parameters.config_file = "myconfig.yaml";
}

Parse_Yaml::Parse_Yaml(string infile)
{
  // str input_file = infile;
}

Parse_Yaml::~Parse_Yaml()
{

}

std::vector<int> Parse_Yaml::getWavelets(std::string values_str)
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

purify::Parameters Parse_Yaml::getParameters(){
  return config_parameters;
};
