/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gms.h,v 1.4 2002-02-16 11:00:46 mschulze Exp $ */
/*
* ABSTRACT: Gauss-Manin system normal form
*/

#ifndef GMS_H
#define GMS_H

lists gmsNF(ideal p,ideal g,matrix B,int D,int K);
BOOLEAN gmsNF(leftv res,leftv h);

#endif /* GMS_H */
