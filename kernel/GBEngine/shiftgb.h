#ifndef SHIFTGB_H
#define SHIFTGB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: kernel: utils for kStd
*/

#include "kernel/structs.h"
#ifdef HAVE_SHIFTBBA
#include "polys/nc/nc.h"

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

//poly pLPshift(poly p, int sh, int uptodeg, int lV);
#define pLPshift(p,sh,uptodeg,lV) p_LPshift(p,sh,uptodeg,lV,currRing)

//int pLastVblock(poly p, int lV);
#define pLastVblock(p,lV) p_LastVblock(p,lV,currRing)
//int pmLastVblock(poly p, int lV);
#define pmLastVblock(p,lV) p_mLastVblock(p,lV,currRing)

int pFirstVblock(poly p, int lV);
int pmFirstVblock(poly p, int lV);

int isInV(poly p, int lV);
int poly_isInV(poly p, int lV);
int ideal_isInV(ideal I, int lV);

int itoInsert(poly p, int uptodeg, int lV, const ring r);

#endif
#endif
