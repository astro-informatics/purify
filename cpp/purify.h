#ifndef PURIFY_H
#define PURIFY_H
#include "parameters.h"
using namespace purify;
using namespace purify::notinstalled;

//! read in vis or measurement set
utilities::vis_params read_measurements(const Parameters &params);

#ifdef PURIFY_MPI
//! read and distribute measurements
utilities::vis_params distribute_measurements(const Parameters &params);
#endif
//! Takes in parameters and returns measurement operator
std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const>
measurement_operator_factory(const Parameters &params, const utilities::vis_params &uv_data);

//! Takes in parameters and returns wavelet transform
std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const>
wavelet_operator_factory(const Parameters &params);

//! Takes in parameters and measurement operator, and returns sopt algorithm
std::shared_ptr<sopt::algorithm> algorithm_factory(
    const Parameters &params,
    std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> const &measurements,
    std::shared_ptr < sopt::LinearTransform<Vector<t_complex>> const &psi,
    const utilities::vis_params &uv_data);

//! main method
int main(int nargs, char const **args);

//! save vector to image cube
template <class T>
void save_image(const std::string &name, const std::vector<Vector<T>> &image,
                const Parameters &params, const utilities::vis_params &uv_data) {
  pfitsio::header_params header;
  header.fits_name = params.obs_name + "_" + name + ".fits";
  header.cell_x = uv_data.cell_x;
  header.cell_y = uv_data.cell_y;
  params.ra = uv_data.ra;
  params.dec = uv_data.dec;
  if(image.size() == 0)
    throw std::runtime_error("Attempting to save empty image cube:" + header.fits_name);
  pfitsio::write3d_header(image, params.image_width, params.image_height, header);
};
//! save vector to image cube
template <class T>
void save_image(const std::string &name, const Vector<T> &image, const Parameters &params,
                const utilities::vis_params &uv_data){
    save_image<T>(name, {image}, params, uv_data)};
#endif
