#ifndef RMODULON_H
#define RMODULON_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo n
*/
#include <misc/auxiliary.h>

struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;
struct snumber; typedef struct snumber *   number;

#include <coeffs/rintegers.h>

#ifdef HAVE_RINGS

typedef struct { mpz_ptr base;  unsigned long exp; } ZnmInfo;

BOOLEAN nrnInitChar    (coeffs r, void*);

#endif

#endif
