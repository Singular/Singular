#ifndef SHORTFL_H
#define SHORTFL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
#include <misc/auxiliary.h>

struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;
struct snumber; typedef struct snumber *   number;

/// Initialize r
BOOLEAN nrInitChar(coeffs r, void*);

// will be reused by gnumpc.cc and longrat.cc
/// Converts a n_R number into a float. Needed by Maps
float   nrFloat(number n);

#endif

