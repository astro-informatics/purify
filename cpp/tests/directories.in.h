#ifndef PURIFY_DATA_DIR_H
#define PURIFY_DATA_DIR_H

#include <string>
#include <sys/stat.h>
namespace purify {
namespace notinstalled {

//! Holds data and such
inline std::string data_directory() { return "@PROJECT_SOURCE_DIR@/data"; }
//! Holds data and such
inline std::string data_filename(std::string const &filename) {
  return data_directory() + "/" + filename;
}
//! Holds data and such in Scratch (useful for legion)
inline std::string scratch_directory() {
  std::string dirName = "$ENV{HOME}/Scratch/purify/data";
  struct stat sb;
  if ( stat("$ENV{HOME}/Scratch",&sb)!=0 || !S_ISDIR(sb.st_mode) )
    mkdir("$ENV{HOME}/Scratch",S_IRWXU);
  if ( stat("$ENV{HOME}/Scratch/purify",&sb)!=0 || !S_ISDIR(sb.st_mode) )
    mkdir("$ENV{HOME}/Scratch/purify",S_IRWXU);
  if ( stat("$ENV{HOME}/Scratch/purify/data",&sb)!=0 || !S_ISDIR(sb.st_mode) )
    mkdir("$ENV{HOME}/Scratch/purify/data",S_IRWXU);
  return dirName;
}
//! Holds data and such in Scratch (useful for legion)
inline std::string scratch_filename(std::string const &filename) {
  return scratch_directory() + "/" + filename;
}
//! Image filename
inline std::string image_filename(std::string const &filename) {
  return data_filename("images/" + filename);
}
//! Visibility filename
inline std::string visibility_filename(std::string const &filename) {
  return scratch_filename("vis_" + filename);
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
