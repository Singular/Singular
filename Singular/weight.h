#ifndef WEIGHT_H
#define WEIGHT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Id: weight.h,v 1.2 1997-03-24 14:26:09 Singular Exp $ */

#include "structs.h"

extern short * ecartWeights;
extern pFDegProc pFDegOld;
extern pLDegProc pLDegOld;

void kEcartWeights(polyset s, int sl, short *eweight);
BOOLEAN kWeight(leftv res,leftv id);
BOOLEAN kQHWeight(leftv res,leftv v);
int maxdegreeWecart(poly p,int *l);
int totaldegreeWecart(poly p);

short * iv2array(intvec * iv);

#endif

