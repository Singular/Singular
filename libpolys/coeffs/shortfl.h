#ifndef SHORTFL_H
#define SHORTFL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
#include "misc/auxiliary.h"
#include "omalloc/omalloc.h"

#if SIZEOF_DOUBLE == SIZEOF_LONG
#define SI_FLOAT double
#else
#define SI_FLOAT float
#endif


struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;
struct snumber; typedef struct snumber *   number;

/// Initialize r
BOOLEAN nrInitChar(coeffs r, void*);

// will be reused by gnumpc.cc and longrat.cc
/// Converts a n_R number into a float. Needed by Maps
SI_FLOAT  nrFloat(number n);

#endif

