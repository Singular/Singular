#ifndef WEIGHT_H
#define WEIGHT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Id: weight.h,v 1.4 2000-10-23 12:02:23 obachman Exp $ */

#include "structs.h"

extern short * ecartWeights;
extern pFDegProc pFDegOld;
extern pLDegProc pLDegOld;

void kEcartWeights(polyset s, int sl, short *eweight);
BOOLEAN kWeight(leftv res,leftv id);
BOOLEAN kQHWeight(leftv res,leftv v);
int maxdegreeWecart(poly p,int *l, ring r = currRing);
int totaldegreeWecart(poly p, ring r = currRing);

short * iv2array(intvec * iv);

#endif

