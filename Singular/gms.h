/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gms.h,v 1.2 2002-02-12 17:24:43 mschulze Exp $ */
/*
* ABSTRACT: Gauss-Manin system normal form
*/

#ifndef GMS_H
#define GMS_H

lists gmsnf(ideal p,ideal g,matrix B,int D,int K);
BOOLEAN gmsnf(leftv res,leftv h);

#endif
