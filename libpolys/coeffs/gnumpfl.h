#ifndef GMPFLOAT_H
#define GMPFLOAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: computations with GMP floating-point numbers
*/

#include <misc/auxiliary.h>

struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;
struct snumber; typedef struct snumber *   number;

/// Initialize r
BOOLEAN ngfInitChar(coeffs r, void *);

// will be reused by gnumpc.cc
const char *   ngfRead (const char *s, number *a, const coeffs r);

#endif
