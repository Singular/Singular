/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: units.h,v 1.1.1.1 2003-10-06 12:16:04 Singular Exp $ */
/*
* ABSTRACT: procedures to compute with units
*/

#ifndef UNITS_H
#define UNITS_H

ideal redNF(ideal N,ideal M,matrix U=NULL,int d=-1,intvec *w=NULL);
poly redNF(ideal N,poly p,poly u=NULL,int d=-1,intvec *w=NULL);

#endif
