/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: eigenval.h,v 1.1 2001-03-05 17:31:40 mschulze Exp $ */
/*
* ABSTRACT: eigenvalues of constant square matrices
*/

#ifndef EIGENVAL_H
#define EIGENVAL_H

matrix tridiag(matrix M);
lists eigenval(matrix M);
BOOLEAN tridiag(leftv res,leftv h);
BOOLEAN eigenval(leftv res,leftv h);

#endif
