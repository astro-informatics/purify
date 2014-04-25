
#ifndef PURIFY_UTILS
#define PURIFY_UTILS
#include "purify_config.h"

void purify_utils_fftshift_1d(double *out, double *in, int n);

void purify_utils_ifftshift_1d(double *out, double *in, int n);

double purify_utils_maxarray(double *a, int num_elements);

int purify_utils_absearch(double *x, int nx, double key);

#endif
