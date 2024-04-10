#ifndef RMODULO2M_H
#define RMODULO2M_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo 2^m such that 2^m - 1
*           fits in an unsigned long
*/

#include "misc/auxiliary.h"

struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;
struct snumber; typedef struct snumber *   number;

BOOLEAN nr2mInitChar    (coeffs r, void*);
number  nr2mMapZp       (number from, const coeffs /*src*/, const coeffs dst);
/*for SAGE, better: use the result of n_SetMap*/

#endif
