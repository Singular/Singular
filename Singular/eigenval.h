/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: eigenval.h,v 1.2 2002-02-16 10:56:19 mschulze Exp $ */
/*
* ABSTRACT: eigenvalues of constant square matrices
*/

#ifndef EIGENVAL_H
#define EIGENVAL_H

matrix evSwap(matrix M,int i,int j);
BOOLEAN evSwap(leftv res,leftv h);
matrix evRowElim(matrix M,int i,int j,int k);
BOOLEAN evRowElim(leftv res,leftv h);
matrix evColElim(matrix M,int i,int j,int k);
BOOLEAN evColElim(leftv res,leftv h);
matrix evHessenberg(matrix M);
BOOLEAN evHessenberg(leftv res,leftv h);
lists evEigenvalue(matrix M);
BOOLEAN evEigenvalue(leftv res,leftv h);

#endif /* EIGENVAL_H */
