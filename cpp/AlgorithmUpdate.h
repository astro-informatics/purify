#ifndef ALGORITHMUPDATE_H
#define ALGORITHMUPDATE_H

#include "purify/config.h"
#include <sopt/imaging_padmm.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include "cmdl.h"
#include "purify/MeasurementOperator.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"

namespace purify {
class AlgorithmUpdate {
  // update function that saves information in algorithm
public:
  AlgorithmUpdate(const purify::Params &params, const utilities::vis_params &uv_data,
                  sopt::algorithm::ImagingProximalADMM<t_complex> &padmm, std::ostream &stream,
                  const MeasurementOperator &measurements,
                  const sopt::LinearTransform<sopt::Vector<sopt::t_complex>> &Psi);

  bool operator()(Vector<t_complex> const &x);

private:
  struct Stats {
    t_int iter = 0;
    t_real new_purify_gamma = 0;
    t_real relative_gamma = 0;
    t_real dr = 0;
    t_real rms = 0;
    t_real max = 0;
    t_real min = 0;
    t_real l1_norm = 0;
    t_real l2_norm = 0;
    t_real l1_variation = 0;
    t_real total_time = 0;
  };

  const Params &params;
  Stats stats;
  const utilities::vis_params &uv_data;
  std::ostream &out_diagnostic;
  sopt::algorithm::ImagingProximalADMM<t_complex> &padmm;
  std::clock_t const c_start;
  const sopt::LinearTransform<sopt::Vector<sopt::t_complex>> &Psi;
  const MeasurementOperator &measurements;

private:
  //! Method to modify gamma
  void modify_gamma(Vector<t_complex> const &alpha);
  //! method to print log to stream
  void print_to_stream(std::ostream &stream);
  //! method to save images to fits files
  void save_figure(const Vector<t_complex> &image, std::string const &output_file_name,
                   std::string const &units, t_real const &upsample_ratio);
  //! method to create fits header
  pfitsio::header_params
  create_header(purify::utilities::vis_params const &uv_data, purify::Params const &params);
  //! read params to stats
  Stats read_params_to_stats(const Params &params) {
    Stats stats;
    stats.iter = params.iter;
    return stats;
  }
};
}
#endif
