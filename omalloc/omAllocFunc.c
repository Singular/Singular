/*******************************************************************
 *  File:    omAllocFunc.c
 *  Purpose: implementation of ANSI-C conforming malloc functions
 *           which are sure to be functions, which start with the om prefix,
 *           and end with the Func suffix.
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/
#include "omalloc/omConfig.h"

#ifdef HAVE_OMALLOC
#define malloc      omMallocFunc
#define calloc      omCallocFunc
#define free        omFreeFunc
#define freeSize    omFreeSizeFunc
#define realloc     omReallocFunc
#define reallocSize omReallocSizeFunc
#define strdup      omStrdupFunc
#define valloc      omVallocFunc

#define OMALLOC_FUNC
#include <omalloc/omalloc.c>
#endif
