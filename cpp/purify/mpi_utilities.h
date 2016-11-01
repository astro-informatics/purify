#ifndef PURIFY_MPI_UTILITIES_H
#define PURIFY_MPI_UTILITIES_H

#include "purify/config.h"
#include <vector>
#include "purify/utilities.h"

namespace purify {
namespace utilities {
//! \brief Regroups visibilities data according to input groups
//! \details All data for group with smallest key comes first, then next smallest key, etc.
void regroup(utilities::vis_params &params, std::vector<t_int> const &groups);
}
} // namespace purify
#endif
