/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmstd.c,v 1.3 2001-01-27 17:04:59 obachman Exp $ */
/*
* ABSTRACT: standard version of C-memory management alloc func 
* i.e. (malloc/realloc/free)
*/

#include "mod2.h"

#define OM_NO_MALLOC_MACROS
#include "omalloc.h"

// we provide these functions, so that the settings of OM_CHECK
// and OM_TRACK are used, but only provide them if omalloc is not based
// on them
#ifndef OMALLOC_USES_MALLOC

// define this so that all addr allocated there are marked 
// as static, i.e. not metioned by omPrintUsedAddr
#define OM_MALLOC_MARK_AS_STATIC
#include <omalloc.c>
#endif
