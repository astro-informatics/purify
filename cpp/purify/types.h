#ifndef PURIFY_TYPES_H
#define PURIFY_TYPES_H

#ifdef PURIFY_CImg
#include <CImg.h>
#ifdef Success
#undef Success
#endif
#ifdef Complex
#undef Complex
#endif
#ifdef Bool
#undef Bool
#endif
#ifdef None
#undef None
#endif
#ifdef Status
#undef Status
#endif
#endif

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
using sopt::Vector;
using sopt::t_complex;
using sopt::t_int;
using sopt::t_real;
using sopt::t_uint;

typedef std::complex<float> t_complexf;
//! Root of the type hierarchy for triplet lists
typedef Eigen::Triplet<t_complex> t_tripletList;

//! \brief A matrix of a given type
//! \details Operates as mathematical sparse matrix.
template <class T = t_real> using Sparse = Eigen::SparseMatrix<T, Eigen::RowMajor>;
template <class T = t_real> using SparseVector = Eigen::SparseVector<T>;

#ifdef PURIFY_CImg
//! Image type of CImg library
template <class T = t_real> using CImage = cimg_library::CImg<T>;
template <class T = t_real> using CImageList = cimg_library::CImgList<T>;
//! Display used to display CImg images
typedef cimg_library::CImgDisplay CDisplay;
#endif

enum class stokes {I, Q, U, V, XX, YY, XY, YX, LL, RR, LR, RL, P};


namespace wproj_utilities {
namespace expansions {
//! Type of series approximation
enum class series { none, taylor, chebyshev };
} // namespace expansions

} // namespace wproj_utilities
namespace constant {
//! mathematical constant
const t_real pi = 3.14159265358979323846;
//! speed of light in vacuum
const t_real c = 299792458.0;
} // namespace constant
} // namespace purify
#endif
