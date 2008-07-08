#ifndef SHIFTGB_H
#define SHIFTGB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: shiftgb.h,v 1.5 2008-07-08 13:02:21 Singular Exp $ */
/*
* ABSTRACT: kernel: utils for kStd
*/

#include "structs.h"
#ifdef HAVE_SHIFTBBA
#include "gring.h"

poly p_LPshiftT(poly p, int sh, int uptodeg, int lV, kStrategy strat, const ring r);
int p_LastVblockT(poly p, int lV, kStrategy strat, const ring r);

poly p_ShrinkT(poly p, int lV, kStrategy strat, const ring r);
poly p_Shrink(poly p, int lV, const ring r);
poly p_mShrink(poly p, int lV, const ring r);
//poly p_Shrink(poly p, int uptodeg, int lV, kStrategy strat, const ring r);
//poly p_mShrink(poly p, int uptodeg, int lV, const ring r);

poly p_LPshift(poly p, int sh, int uptodeg, int lV,const ring r);
poly p_mLPshift(poly p, int sh, int uptodeg, int lV,const ring r);

int p_mLastVblock(poly p, int lV,const ring r);
int p_LastVblock(poly p, int lV, const ring r);

poly pLPshift(poly p, int sh, int uptodeg, int lV);
poly pmLPshift(poly p, int sh, int uptodeg, int lV);

int pLastVblock(poly p, int lV);
int pmLastVblock(poly p, int lV);

int pFirstVblock(poly p, int lV);
int pmFirstVblock(poly p, int lV);

int isInV(poly p, int lV);

int itoInsert(poly p, int uptodeg, int lV, const ring r);

#endif
#endif
