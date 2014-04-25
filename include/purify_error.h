/*! \file purify_error.h
 *  Error functions used in PURIFY package.
 */

#ifndef PURIFY_ERROR
#define PURIFY_ERROR
#include "purify_config.h"

#include <stdio.h>
#include <stdlib.h>

inline void PURIFY_ERROR_GENERIC(char *comment) {
  printf("ERROR: %s.\n", comment);					
  printf("ERROR: %s <%s> %s %s %s %d.\n",				
	 "Occurred in function",					
	   __PRETTY_FUNCTION__,						
	   "of file", __FILE__,						
	   "on line", __LINE__);					
  exit(1);
}

inline void PURIFY_ERROR_MEM_ALLOC_CHECK(void *pointer) {				
  if(pointer == NULL)
    PURIFY_ERROR_GENERIC("Memory allocation failed");
}

#endif
