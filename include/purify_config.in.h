#ifndef PURIFY_CONFIG_IN_H
#define PURIFY_CONFIG_IN_H
#ifdef __cplusplus
extern "C" {
#endif

// Deal with the different ways to include blas
#cmakedefine PURIFY_BLAS_H <@PURIFY_BLAS_H@>

//! Library version
static const char purify_version[] = "@Purify_VERSION@";
//! Hash of the current git reference
static const char purify_gitref[] = "@Purify_GITREF@";

#ifdef __cplusplus
}
#endif
#endif
