#ifndef PURIFY_TYPES_H
#define PURIFY_TYPES_H

#include "purify/config.h"
#include <complex>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/SparseCore>

namespace purify {
//! Root of the type hierarchy for signed integers
typedef int t_int;
//! Root of the type hierarchy for unsigned integers
typedef long t_long;
//! Root of the type hierarchy for unsigned integers
typedef size_t t_uint;
//! Root of the type hierarchy for real numbers
typedef double t_real;
//! Root of the type hierarchy for (real) complex numbers
typedef std::complex<t_real> t_complex;
//! Root of the type hierarchy for triplet lists
typedef Eigen::Triplet<t_complex> t_tripletList;

//! \brief A vector of a given type
//! \details Operates as mathematical vector.
template <class T = t_real> using Vector = Eigen::Matrix<T, Eigen::Dynamic, 1>;
//! \brief A matrix of a given type
//! \details Operates as mathematical matrix.
template <class T = t_real> using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;
//! \brief A matrix of a given type
//! \details Operates as mathematical sparse matrix.
template <class T = t_real> using Sparse = Eigen::SparseMatrix<T, Eigen::RowMajor>;
//! \brief A 1-dimensional list of elements of given type
//! \details Operates coefficient-wise, not matrix-vector-wise
template <class T = t_real> using Array = Eigen::Array<T, Eigen::Dynamic, 1>;
//! \brief A 2-dimensional list of elements of given type
//! \details Operates coefficient-wise, not matrix-vector-wise
template <class T = t_real> using Image = Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic>;

namespace constant {
//! mathematical constant
const t_real pi = 3.14159265358979323846;
//! speed of light in vacuum
const t_real c = 299792458.0;
}
}
#endif
