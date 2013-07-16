/*! \file purify_error.h
 *  Error functions used in PURIFY package.
 */

#ifndef PURIFY_ERROR
#define PURIFY_ERROR

inline void PURIFY_ERROR_GENERIC(char *comment);
inline void PURIFY_ERROR_MEM_ALLOC_CHECK(void *pointer);

#endif
