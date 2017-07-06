#ifndef PARSE_YAML_H_
#define PARSE_YAML_H_
#include <string>
#include <yaml-cpp/yaml.h>
#include "parameters.h"

class Parse_Yaml{
  //private:
  //std::string input_name;
  //structure to return?

 public:
  // constructors
  Parse_Yaml();
  Parse_Yaml(std::string);

  // destructor
  ~Parse_Yaml();

  // access methods
  purify::Parameters config_parameters;
  purify::Parameters getParameters(); // Why I had that for? (const std::string);
  std::vector<int> getWavelets(std::string);
  // mutator methods

  // where does it go the internal methods? privates?

};
#endif // PARSE_YAML_H
