/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: eigenval.h,v 1.4 2002-04-30 13:35:09 levandov Exp $ */
/*
* ABSTRACT: eigenvalues of constant square matrices
*/

#ifndef EIGENVAL_H
#define EIGENVAL_H
#ifdef HAVE_EIGENVAL

matrix evSwap(matrix M,int i,int j);
BOOLEAN evSwap(leftv res,leftv h);
matrix evRowElim(matrix M,int i,int j,int k);
BOOLEAN evRowElim(leftv res,leftv h);
matrix evColElim(matrix M,int i,int j,int k);
BOOLEAN evColElim(leftv res,leftv h);
matrix evHessenberg(matrix M);
BOOLEAN evHessenberg(leftv res,leftv h);
lists evEigenvals(matrix M);
BOOLEAN evEigenvals(leftv res,leftv h);

#endif /* ifdef HAVE_EIGENVAL */
#endif /* EIGENVAL_H */
