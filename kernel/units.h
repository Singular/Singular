/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: procedures to compute with units
*/

#ifndef UNITS_H
#define UNITS_H

ideal redNF(ideal N,ideal M,matrix U=NULL,int d=-1,intvec *w=NULL);
poly redNF(ideal N,poly p,poly u=NULL,int d=-1,intvec *w=NULL);

#endif
