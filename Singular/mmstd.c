/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmstd.c,v 1.1 2000-11-20 10:18:56 hannes Exp $ */
/*
* ABSTRACT: standard version of C-memory management alloc func
*/

#include "mod2.h"

/* We define those, so that our values of 
   OM_TRACK and OM_CHECK are used  */

// define this so that all addr allocated there are marked 
// as static, i.e. not metioned by omPrintUsedAddr
#define OM_MALLOC_MARK_AS_STATIC
#include <omalloc.c>
