/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: units.h,v 1.4 2001-02-08 13:13:05 Singular Exp $ */
/*
* ABSTRACT: procedures to compute with units
*/

#ifndef UNITS_H
#define UNITS_H

BOOLEAN invunit(leftv res,leftv h);
poly invunit(int n,poly u);
matrix invunit(int n,matrix U);
ideal rednf(ideal N,ideal M,matrix U=NULL);
poly rednf(ideal N,poly p,poly u=NULL);

#endif
