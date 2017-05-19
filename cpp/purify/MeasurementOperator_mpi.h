#ifndef PURIFY_MEASUREMENT_OPERATOR_MPI_H
#define PURIFY_MEASUREMENT_OPERATOR_MPI_H

#include <set>
#include <vector>
#include <sopt/mpi/communicator.h>
#include "purify/DistributeSparseVector.h"
#include "purify/IndexMapping.h"
#include "purify/MeasurementOperator.h"

namespace purify {

namespace mpi {
#ifdef PURIFY_MPI
class MeasurementOperator : public purify::MeasurementOperator {
public:
  //! Constructor
  MeasurementOperator(const sopt::mpi::Communicator _comm,
                      const utilities::vis_params &uv_vis_input, const t_int &Ju, const t_int &Jv,
                      const std::string &kernel_name, const t_int &imsizex, const t_int &imsizey,
                      const t_int &norm_iterations = 20, const t_real &oversample_factor = 2,
                      const t_real &cell_x = 1, const t_real &cell_y = 1,
                      const std::string &weighting_type = "none", const t_real &R = 0,
                      bool use_w_term = false, const t_real &energy_fraction = 1,
                      const std::string &primary_beam = "none", bool fft_grid_correction = false)
      : purify::MeasurementOperator(uv_vis_input, Ju, Jv, kernel_name, imsizex, imsizey, 0,
                                    oversample_factor, cell_x, cell_y, weighting_type, R,
                                    use_w_term, energy_fraction, primary_beam, fft_grid_correction),
        comm(_comm), distributor(G, _comm) {
    G = compress_outer(G);
    norm = std::sqrt(power_method(norm_iterations));
  }

  //! Degridding operator that degrids image to visibilities with mpi
  Vector<t_complex> degrid(const Image<t_complex> &eigen_image) const override;
  //! Gridding operator that grids image from visibilities with mpi
  Image<t_complex> grid(const Vector<t_complex> &visibilities) const override;

private:
  //! communicator for gridding and degridding
  sopt::mpi::Communicator comm;
  //! Gathers and scatters the Fourrier grid
  DistributeSparseVector distributor;
};
#endif
}
} // namespace purify

#endif
