#ifndef PURIFY_DATA_H
#define PURIFY_DATA_H

#include <string>
#include <sys/stat.h>
#include <vector>
#include <fstream>

#include "purify/types.h"
#include "purify/directories.h"
namespace purify {
namespace notinstalled {
  //! read real values from data file
  template <class T>
typename std::enable_if<std::is_scalar<T>::value, std::vector<T>>::type
   read_data(const std::string & filename){
  std::ifstream data_file(filename);
  if (!data_file)
      throw std::runtime_error("Test data is missing: "+filename);
  std::string s;
  //read data
  std::vector<T> data;
  for(std::string s; std::getline(data_file, s, ',');){
    data.push_back(std::stod(s));
  }
  return data;
  }
  //! read complex values from data file
  template <class T>
    typename std::enable_if<std::is_same<t_complex, T>::value, std::vector<T>>::type
    read_data(const std::string & filename){
  std::ifstream data_file(filename);
  if (!data_file)
      throw std::runtime_error("Test data is missing: "+filename);
  std::string real = "";
  std::string imag = "";
  //read data
  std::vector<T> data;
  for(std::string real; std::getline(data_file, real, ',');){
    real.erase(0, 1);
    if(!std::getline(data_file, imag, ')'))
      break;
    data.push_back(T(std::stod(real), std::stod(imag)));
    real="";
    imag="";
  }
  return data;
  }

  std::vector<t_complex> read_data(const std::vector<t_real> & input){
    std::vector<t_complex> output;
  for(auto a = input.begin(); a != input.end(); a++ )
    output.push_back(t_complex(*a, 0.));
  return output;
  }
  
}
} /* sopt::notinstalled */
#endif
