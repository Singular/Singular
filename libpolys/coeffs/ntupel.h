#ifndef NTUPEL_H
#define NTUPEL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: n-tupel of coefficients
*/
#include "misc/auxiliary.h"

struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;
struct snumber; typedef struct snumber *   number;

/// Initialize r
BOOLEAN nnInitChar(coeffs r, void*);

#endif

