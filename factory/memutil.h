/* emacs edit mode for this file is -*- C -*- */
/* $Id: memutil.h,v 1.2 1997-06-19 12:22:16 schmidt Exp $ */

#ifndef INCL_MEMUTIL_H
#define INCL_MEMUTIL_H

#define _POSIX_SOURCE 1

#include <config.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void* getBlock ( size_t size );

void freeBlock ( void * block, size_t size );

void* reallocBlock ( void * block, size_t oldsize, size_t newsize );

#ifdef __cplusplus
}
#endif

#endif /* ! INCL_MEMUTIL_H */
