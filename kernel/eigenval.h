/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: eigenval.h,v 1.2 2004-03-04 10:08:24 Singular Exp $ */
/*
* ABSTRACT: eigenvalues of constant square matrices
*/

#ifndef EIGENVAL_H
#define EIGENVAL_H
#ifdef HAVE_EIGENVAL

matrix evSwap(matrix M,int i,int j);
matrix evRowElim(matrix M,int i,int j,int k);
matrix evColElim(matrix M,int i,int j,int k);
matrix evHessenberg(matrix M);

#endif /* ifdef HAVE_EIGENVAL */
#endif /* EIGENVAL_H */
