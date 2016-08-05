#ifndef PURIFY_DATA_DIR_H
#define PURIFY_DATA_DIR_H

#include <string>
namespace purify {
namespace notinstalled {

//! Holds data and such
inline std::string data_directory() { return "@PROJECT_SOURCE_DIR@/data"; }
//! Holds data and such
inline std::string data_filename(std::string const &filename) {
  return data_directory() + "/" + filename;
}
//! Image filename
inline std::string image_filename(std::string const &filename) {
  return data_filename("images/" + filename);
}
//! Specific vla data
inline std::string vla_filename(std::string const &filename) {
  return data_filename("vla/" + filename);
}
//! Specific atca data
inline std::string atca_filename(std::string const &filename) {
  return data_filename("atca/" + filename);
}
//! Some gridding regression data
inline std::string gridding_filename(std::string const &filename) {
  return data_filename("expected/gridding/" + filename);
}
//! Some degridding regression data
inline std::string degridding_filename(std::string const &filename) {
  return data_filename("expected/degridding/" + filename);
}

//! Where test outputs go
inline std::string output_directory() { return "@PROJECT_BINARY_DIR@/outputs"; }
//! Test output file
inline std::string output_filename(std::string const &filename) {
  return output_directory() + "/" + filename;
}

inline std::string ngc3256_ms() { return "@NGC3256_MS@"; }
}
} /* sopt::notinstalled */
#endif
