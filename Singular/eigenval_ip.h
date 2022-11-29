/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: eigenvalues of constant square matrices
*/

#ifndef EIGENVAL_IP_H
#define EIGENVAL_IP_H
#ifdef HAVE_EIGENVAL
#include "kernel/linear_algebra/eigenval.h"

BOOLEAN evSwap(leftv res,leftv h);
BOOLEAN evRowElim(leftv res,leftv h);
//BOOLEAN evColElim(leftv res,leftv h);
BOOLEAN evHessenberg(leftv res,leftv h);
lists evEigenvals(matrix M);
BOOLEAN evEigenvals(leftv res,leftv h);

#endif /* ifdef HAVE_EIGENVAL */
#endif /* EIGENVAL_IP_H */
