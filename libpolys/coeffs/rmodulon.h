#ifndef RMODULON_H
#define RMODULON_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo n
*/
#include "misc/auxiliary.h"
#include "coeffs/coeffs.h"

struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;
struct snumber; typedef struct snumber *   number;

#ifdef HAVE_RINGS
#include "coeffs/rintegers.h"

typedef struct { mpz_ptr base;  unsigned long exp; } ZnmInfo;

BOOLEAN nrnInitChar    (coeffs r, void*);
number nrnMapGMP(number from, const coeffs /*src*/, const coeffs dst);/*for SAGE*/
nMapFunc nrnSetMap(const coeffs src, const coeffs dst);/* for SAGE, better: n_setMap*/
coeffs nrnInitCfByName(char *name,n_coeffType n);

#endif

#endif
