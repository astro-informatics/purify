/*! \file purify_types.h
 *  Types and constants used in PURIFY package.
 */

#ifndef PURIFY_TYPES
#define PURIFY_TYPES

#ifdef __cplusplus
#include <complex>
#else
#include <complex.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "purify_config.h"

#define PURIFY_STRLEN 256

#define PURIFY_PI    3.141592653589793238462643383279502884197
#define PURIFY_PION2 1.570796326794896619231321691639751442099

#define PURIFY_SQRT2 1.41421356237309504880168872420969807856967

#define purify_min(a,b) (a<b?a:b)
#define purify_max(a,b) (a<b?b:a) 

#ifdef __cplusplus
typedef std::complex<double> purify_complex_double;
#else
typedef complex double purify_complex_double;
#endif

#ifdef __cplusplus
}
#endif
#endif
