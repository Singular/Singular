/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: units.h,v 1.3 2001-02-01 15:54:46 mschulze Exp $ */
/*
* ABSTRACT: procedures to compute with units
*/

#ifndef UNITS_H
#define UNITS_H

BOOLEAN invunit(leftv res,leftv h);
poly invunit(int n,poly u);
matrix invunit(int n,matrix U);
BOOLEAN series(leftv res,leftv h);
poly series(int n,poly p,poly u=NULL);
ideal series(int n,ideal M,matrix U=NULL);
BOOLEAN rednf(leftv res,leftv h);
ideal rednf(ideal N,ideal M,matrix U=NULL);
poly rednf(ideal N,poly p,poly u=NULL);

#endif
