/* emacs edit mode for this file is -*- C -*- */
/* $Id: memutil.h,v 1.0 1996-05-17 10:59:41 stobbe Exp $ */

#ifndef INCL_MEMUTIL_H
#define INCL_MEMUTIL_H

/*
$Log: not supported by cvs2svn $
*/

#define _POSIX_SOURCE 1

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

#endif /* INCL_MEMUTIL_H */
