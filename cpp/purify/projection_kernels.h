#ifndef PURIFY_PROJECTION_KERNELS_H
#define PURIFY_PROJECTION_KERNELS_H

#include "purify/config.h"
#include <fstream>
#include <iostream>
#include <omp.h>
#include <random>
#include <set>
#include <stdio.h>
#include <string>
#include <time.h>
#include <Eigen/Sparse>
#include <sys/stat.h>
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/types.h"

namespace purify {
namespace projection_kernels {
t_complex w_proj_approx(const t_real &u, const t_real &v const &t_real &w, const t_real &dl,
                        const t_real &dm);
}
} // namespace purify

#endif
