
#ifndef PURIFY_RAN
#define PURIFY_RAN
#ifdef __cplusplus
extern "C" {
#endif
#include "purify_config.h"

double purify_ran_gasdev2(int idum);
double purify_ran_ran2(int idum);
int purify_ran_knuthshuffle(int *perm, int nperm, int N, int seed);

#ifdef __cplusplus
}
#endif
#endif
