/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: eigenval_ip.h,v 1.1 2004-03-04 10:06:51 Singular Exp $ */
/*
* ABSTRACT: eigenvalues of constant square matrices
*/

#ifndef EIGENVAL_IP_H
#define EIGENVAL_IP_H
#ifdef HAVE_EIGENVAL
#include "eigenval.h"

BOOLEAN evSwap(leftv res,leftv h);
BOOLEAN evRowElim(leftv res,leftv h);
BOOLEAN evColElim(leftv res,leftv h);
BOOLEAN evHessenberg(leftv res,leftv h);
lists evEigenvals(matrix M);
BOOLEAN evEigenvals(leftv res,leftv h);

#endif /* ifdef HAVE_EIGENVAL */
#endif /* EIGENVAL_IP_H */
