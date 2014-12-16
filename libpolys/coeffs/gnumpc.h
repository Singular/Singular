#ifndef GMPCOMPLEX_H
#define GMPCOMPLEX_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: computations with GMP floating-point numbers
*/
#include <misc/auxiliary.h>

struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;

/// Initialize r (n_long_C)
BOOLEAN ngcInitChar(coeffs r, void*);

#endif
/* GMPCOMPLEX_H */
