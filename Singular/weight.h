#ifndef WEIGHT_H
#define WEIGHT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Id: weight.h,v 1.5 2000-10-30 13:40:29 obachman Exp $ */

#include "structs.h"

extern short * ecartWeights;
extern pFDegProc pFDegOld;
extern pLDegProc pLDegOld;

void kEcartWeights(polyset s, int sl, short *eweight);
BOOLEAN kWeight(leftv res,leftv id);
BOOLEAN kQHWeight(leftv res,leftv v);
long maxdegreeWecart(poly p,int *l, ring r = currRing);
long totaldegreeWecart(poly p, ring r = currRing);

short * iv2array(intvec * iv);

#endif

