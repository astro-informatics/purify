#ifndef PURIFY_CLEAN_H
#define PURIFY_CLEAN_H

#include "types.h"

#include "MeasurementOperator.h"
#include "FFTOperator.h"

#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>


namespace purify {

  namespace clean {
    //basic clean algorithms, returns a clean model
    Image<t_complex> clean(MeasurementOperator & op, const utilities::vis_params & uv_vis, 
      const t_int & niters, const t_real& gain = 0.1, const std::string & mode = "hogbom", const t_real clip = 0.9);
    //uses computationally cheap version of steer clean to generate initial model for purify.
    Image<t_complex> model_estimate(const Image<t_complex> & dirty_image, const Image<t_complex> dirty_beam_fft,
				const t_int & niters, const t_real& gain = 0.1, const t_real clip = 0.9);
    //restores an image from a given clean model
    Image<t_complex> restore(MeasurementOperator & op, const utilities::vis_params & uv_vis, const Image<t_complex> & clean_model);
  }
}

#endif