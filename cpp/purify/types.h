#ifndef PURIFY_TYPES_H
#define PURIFY_TYPES_H

#include "purify/config.h"
#include <complex>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <sopt/types.h>

namespace purify {
using sopt::t_int;
using sopt::t_uint;
using sopt::t_real;
using sopt::t_complex;
using sopt::Vector;
using sopt::Matrix;
using sopt::Array;
using sopt::Image;

//! Root of the type hierarchy for triplet lists
typedef Eigen::Triplet<t_complex> t_tripletList;

//! \brief A matrix of a given type
//! \details Operates as mathematical sparse matrix.
template <class T = t_real> using Sparse = Eigen::SparseMatrix<T, Eigen::RowMajor>;

namespace constant {
//! mathematical constant
const t_real pi = 3.14159265358979323846;
//! speed of light in vacuum
const t_real c = 299792458.0;
}
}
#endif
