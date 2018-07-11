/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT: flint: fmpq_poly
*/

#ifndef FLINTCF_Q_H
#define FLINTCF_Q_H

#include "misc/auxiliary.h"
#ifdef HAVE_FLINT
BOOLEAN flintQ_InitChar(coeffs cf, void * infoStruct);

coeffs flintQInitCfByName(char *s,n_coeffType n);
#endif

#endif

