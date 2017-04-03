#ifndef PURIFY_MEASUREMENT_OPERATOR_MPI_H
#define PURIFY_MEASUREMENT_OPERATOR_MPI_H

#include "purify/MeasurementOperator.h"
#include <sopt/mpi/communicator.h>
#include <set>
#include <vector>

namespace purify {

  namespace mpi {

    class MeasurementOperator : public purify::MeasurementOperator {
      public:
        //! Constructor
        MeasurementOperator(const sopt::mpi::Communicator comm, const utilities::vis_params &uv_vis_input, const t_int &Ju, const t_int &Jv,
            const std::string &kernel_name, const t_int &imsizex, const t_int &imsizey,
            const t_int &norm_iterations = 20, const t_real &oversample_factor = 2,
            const t_real &cell_x = 1, const t_real &cell_y = 1,
            const std::string &weighting_type = "none", const t_real &R = 0,
            bool use_w_term = false, const t_real &energy_fraction = 1,
            const std::string &primary_beam = "none", bool fft_grid_correction = false)
          : purify::MeasurementOperator(uv_vis_input, Ju, Jv, kernel_name, imsizex,
              imsizey, 0, oversample_factor, cell_x, cell_y,
              weighting_type, R, use_w_term, energy_fraction, primary_beam,
              fft_grid_correction), comm(comm) {
            create_mask();
            norm = std::sqrt(power_method(norm_iterations));
          }

        //! Degridding operator that degrids image to visibilities with mpi
        Vector<t_complex> degrid(const Image<t_complex> &eigen_image) const override;
        //! Gridding operator that grids image from visibilities with mpi
        Image<t_complex> grid(const Vector<t_complex> &visibilities) const override;
      private:
        //! global mask, containing indices for each b_i element
        Vector<t_int> M;
        //! local ft_grid mask that determines b_i grid indices
        Vector<t_int> M_local;
        //! Used to generate the sizes of b for each node
        std::vector<t_int> sizes;
        //! communicator for gridding and degridding
        sopt::mpi::Communicator comm;
        //! Used to create and distribute the local mask M_local
        void create_mask();
    };

    //! \brief Helper function to wrap a linear transform around a distributed measurement operator
    //! \note The measurement operator must exist during the lifetime of the linear transforms.
    sopt::LinearTransform<sopt::Vector<sopt::t_complex>>
      linear_transform(MeasurementOperator const &measurements, t_uint nvis);

  }


}


#endif
