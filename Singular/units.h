/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: units.h,v 1.1 2001-02-01 13:11:15 mschulze Exp $ */
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

#endif
