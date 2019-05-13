#ifndef PURIFY_TYPES_H
#define PURIFY_TYPES_H

#include "purify/config.h"
#include <complex>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <sopt/types.h>

namespace purify {
using sopt::Array;
using sopt::Image;
using sopt::Matrix;
using sopt::t_complex;
using sopt::t_int;
using sopt::t_real;
using sopt::t_uint;
using sopt::Vector;

typedef std::complex<float> t_complexf;
//! Root of the type hierarchy for triplet lists
typedef Eigen::Triplet<t_complex> t_tripletList;

//! \brief A matrix of a given type
//! \details Operates as mathematical sparse matrix.
template <class T = t_real, class I = t_int>
using Sparse = Eigen::SparseMatrix<T, Eigen::RowMajor, I>;
template <class T = t_real, class I = t_int>
using SparseVector = Eigen::SparseVector<T, Eigen::RowMajor, I>;

enum class stokes { I, Q, U, V, XX, YY, XY, YX, LL, RR, LR, RL, P };
const std::map<stokes, t_int> stokes_int = {{stokes::I, 1},   {stokes::Q, 2},   {stokes::U, 3},
                                            {stokes::V, 4},   {stokes::RR, -1}, {stokes::LL, -2},
                                            {stokes::RL, -3}, {stokes::LR, -4}, {stokes::XX, -5},
                                            {stokes::YY, -6}, {stokes::XY, -7}, {stokes::YX, -8}};
const std::map<std::string, stokes> stokes_string = {
    {"I", stokes::I},   {"i", stokes::I},   {"Q", stokes::Q},   {"q", stokes::Q},
    {"U", stokes::U},   {"u", stokes::U},   {"V", stokes::V},   {"v", stokes::V},
    {"XX", stokes::XX}, {"xx", stokes::XX}, {"YY", stokes::YY}, {"yy", stokes::YY},
    {"XY", stokes::XY}, {"xy", stokes::XY}, {"YX", stokes::YX}, {"yx", stokes::YX},
    {"LL", stokes::LL}, {"ll", stokes::LL}, {"RR", stokes::RR}, {"rr", stokes::RR},
    {"LR", stokes::LR}, {"lr", stokes::LR}, {"RL", stokes::RL}, {"rl", stokes::RL},
    {"P", stokes::P},   {"p", stokes::P}};

//! Types of DDEs in purify
enum class dde_type { wkernel_radial, wkernel_2d };

namespace wproj_utilities {
namespace expansions {
//! Type of series approximation
enum class series { none, taylor, chebyshev };
}  // namespace expansions

}  // namespace wproj_utilities
namespace constant {
//! mathematical constant
const t_real pi = 3.14159265358979323846;
//! speed of light in vacuum
const t_real c = 299792458.0;
}  // namespace constant
}  // namespace purify
#endif
