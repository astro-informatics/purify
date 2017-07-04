#ifndef PARSE_YAML_H_
#define PARSE_YAML_H_
#include <string>
#include <yaml-cpp/yaml.h>

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
  std::vector<int> getWavelets(std::string);
  // mutator methods

  // where does it go the internal methods? privates?

};
#endif // PARSE_YAML_H
