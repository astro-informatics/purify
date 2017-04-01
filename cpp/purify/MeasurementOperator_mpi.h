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
      MeasurementOperator(const purify::MeasurementOperator & measurements,
        const sopt::mpi::Communicator & comm);
      //! Degridding operator that degrids image to visibilities with mpi
      Vector<t_complex> degrid(const Image<t_complex> &eigen_image, const sopt::mpi::Communicator &comm) const;
      //! Gridding operator that grids image from visibilities with mpi
      Image<t_complex> grid(const Vector<t_complex> &visibilities, const sopt::mpi::Communicator &comm) const;
    private:
      //! global mask, containing indices for each b_i element
      Vector<t_int> M;
      //! local ft_grid mask that determines b_i grid indices
      Vector<t_int> M_local;
      //! Used to generate the sizes of b for each node
      std::vector<t_int> sizes;
      //! Used to create and distribute the local mask M_local
      void create_mask(const sopt::mpi::Communicator &comm);
    };

  //! \brief Helper function to wrap a linear transform around a distributed measurement operator
  //! \note The measurement operator must exist during the lifetime of the linear transforms.
  sopt::LinearTransform<sopt::Vector<sopt::t_complex>>
    linear_transform(MeasurementOperator const &measurements, t_uint nvis,
        sopt::mpi::Communicator const &comm);

  }


}


#endif
