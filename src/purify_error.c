/*! \file purify_error.c
 *  Error functions used in PURIFY package.
 */
#include "purify_config.h"
#include "purify_error.h"

/*!
 * Display error message and halt program execution.
 *
 * \param[in] comment Additional comment to display.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
extern inline void PURIFY_ERROR_GENERIC(char *comment);

/*!
 * Test whether memory allocation was successful (i.e. check allocated
 * pointer not NULL) and throw a generic error is allocation failed.
 *
 * \param[in] pointer Pointer that should point to allocated memory is
 * memory allocation was successful.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
extern inline void PURIFY_ERROR_MEM_ALLOC_CHECK(void *pointer);
