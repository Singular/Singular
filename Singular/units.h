/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: units.h,v 1.2 2001-02-01 15:14:21 mschulze Exp $ */
/*
* ABSTRACT: procedures to compute with units
*/

#ifndef UNITS_H
#define UNITS_H

BOOLEAN invunit(leftv res,leftv h);
poly invunit(poly u,int n);
matrix invunit(matrix U,int n);
BOOLEAN series(leftv res,leftv h);
poly series(poly p,poly u,int n);
ideal series(ideal M,matrix U,int n);
BOOLEAN rednf(leftv res,leftv h);
ideal rednf(ideal N,ideal M,matrix U=NULL);
poly rednf(ideal N,poly p,poly u=NULL);

#endif
