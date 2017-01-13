#ifndef PURIFY_DISTRIBUTE_H
#define PURIFY_DISTRIBUTE_H
#include "purify/config.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <sopt/mpi/communicator.h>
#include "purify/logging.h"
#include "purify/types.h"
#include "purify/utilities.h"

namespace purify {
  namespace distribute {
    //! Distribute visiblities into groups
    std::vector<t_int>
      distribute_measurements(Vector<t_real> const &u, Vector<t_real> const &v, Vector<t_real> const &w,
          t_int const number_of_nodes,
          std::string const &distribution_plan = "equal_distribution",
          t_int const &grid_size = 128);

    inline std::vector<t_int>
      distribute_measurements(utilities::vis_params const &params, sopt::mpi::Communicator const &comm,
          std::string const &distribution_plan = "equal_distribution",
          t_int const &grid_size = 128) {
        return distribute_measurements(params.u, params.v, params.w, comm.size(), distribution_plan,
            grid_size);
      }
    //! Distribute visiblities into nodes in order of distance from the centre
    Vector<t_int> distance_distribution(Vector<t_real> const &u, Vector<t_real> const &v);
    //! Distribute the visiblities into nodes in order of density
    Vector<t_int>
      equal_distribution(Vector<t_real> const &u, Vector<t_real> const &v, t_int const &grid_size);
  }
}
#endif
