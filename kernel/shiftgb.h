#ifndef SHIFTGB_H
#define SHIFTGB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: shiftgb.h,v 1.1 2007-06-02 13:29:07 levandov Exp $ */
/*
* ABSTRACT: kernel: utils for kStd
*/

#include "structs.h"
#include "gring.h"

ideal freegb(ideal I, int uptodeg, int lVblock);

poly pLPshift(poly p, int lV);

int lastVblock(poly p, int lV);

int isInV(poly p, int lV);

void enterOnePairShift(int i, poly p, int ecart, int isFromQ, kStrategy strat, int atR = -1, int uptodeg, int lV);

void enterOnePairManyShifts(int i, poly p, int ecart, int isFromQ, kStrategy strat, int atR = -1, int uptodeg, int lV);

void initenterpairsShift (poly h,int k,int ecart,int isFromQ,kStrategy strat, int atR = -1);

ideal bbaShift(ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat);
#endif
