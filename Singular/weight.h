#ifndef WEIGHT_H
#define WEIGHT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Id: weight.h,v 1.6 2001-05-28 12:35:51 Singular Exp $ */

#include "structs.h"

extern short * ecartWeights;
extern pFDegProc pFDegOld;
extern pLDegProc pLDegOld;

void kEcartWeights(polyset s, int sl, short *eweight);
BOOLEAN kWeight(leftv res,leftv id);
BOOLEAN kQHWeight(leftv res,leftv v);
long maxdegreeWecart(poly p,int *l, ring r = currRing);
long totaldegreeWecart(poly p, ring r = currRing);
long totaldegreeWecart_IV(poly p, ring r, const short *w);

short * iv2array(intvec * iv);

#endif

