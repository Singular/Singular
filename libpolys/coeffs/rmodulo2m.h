#ifndef RMODULO2M_H
#define RMODULO2M_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo 2^m such that 2^m - 1
*           fits in an unsigned long
*/

#include <misc/auxiliary.h>

struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;
struct snumber; typedef struct snumber *   number;

#ifdef HAVE_RINGS

BOOLEAN nr2mInitChar    (coeffs r, void*);

#endif
#endif
