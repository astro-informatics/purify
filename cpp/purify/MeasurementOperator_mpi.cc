#include "purify/MeasurementOperator_mpi.h"

namespace purify {
namespace mpi {

Vector<t_complex> MeasurementOperator::degrid(const Image<t_complex> &eigen_image) const {
  Vector<t_complex> local_vector;
  if(comm.is_root()) {
    Matrix<t_complex> ft_grid(ftsizev_, ftsizeu_);
    Matrix<t_complex> padded_image = Matrix<t_complex>::Zero(floor(imsizey_ * oversample_factor_),
                                                             floor(imsizex_ * oversample_factor_));
    t_int const x_start
        = std::max(0, static_cast<t_int>(std::floor(imsizex_ * (oversample_factor_ - 1) * 0.5)));
    t_int const y_start
        = std::max(0, static_cast<t_int>(std::floor(imsizey_ * (oversample_factor_ - 1) * 0.5)));

    // zero padding and gridding correction
    assert(y_start + imsizey_ <= padded_image.rows());
    assert(x_start + imsizex_ <= padded_image.cols());
    assert(eigen_image.rows() == imsizey_);
    assert(eigen_image.cols() == imsizex_);
    padded_image.block(y_start, x_start, imsizey_, imsizex_)
        = utilities::parallel_multiply_image(S, eigen_image);
    ft_grid = utilities::re_sample_ft_grid(fftoperator_.forward(padded_image), resample_factor);
    ft_grid.resize(ftsizeu_ * ftsizev_, 1); // using conservativeResize does not work, it garbles
    distributor.scatter(ft_grid, local_vector);
  } else
    distributor.scatter(local_vector);

  auto const result = utilities::sparse_multiply_matrix(G, local_vector).array() * W / norm;
  return result;
}

Image<t_complex> MeasurementOperator::grid(const Vector<t_complex> &visibilities) const {
  auto const local_vector = G.adjoint() * (visibilities.array() * W.conjugate()).matrix() / norm;

  Image<t_complex> image_out;
  if(not comm.is_root())
    distributor.gather(local_vector);
  else {
    Matrix<t_complex> ft_grid;
    distributor.gather(local_vector, ft_grid);
    ft_grid.resize(ftsizev_, ftsizeu_);
    ft_grid = utilities::re_sample_ft_grid(ft_grid, 1. / resample_factor);
    // the fftshift is not needed because of the phase shift in the gridding kernel
    Image<t_complex> padded_image = fftoperator_.inverse(ft_grid);
    t_int const x_start
        = std::max(0, static_cast<t_int>(std::floor(imsizex_ * (oversample_factor_ - 1) * 0.5)));
    t_int const y_start
        = std::max(0, static_cast<t_int>(std::floor(imsizey_ * (oversample_factor_ - 1) * 0.5)));
    auto const block = padded_image.block(y_start, x_start, imsizey_, imsizex_);
    image_out = utilities::parallel_multiply_image(S, block);
  }
  return comm.broadcast(image_out);
}
}
}
