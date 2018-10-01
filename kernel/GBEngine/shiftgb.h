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

poly p_LPshift(poly p, int sh, const ring r);
poly p_mLPshift(poly p, int sh, const ring r);
poly p_LPCopyAndShiftLM(poly p, int sh, const ring r);

#define pLPshift(p, sh) p_LPshift(p, sh, currRing)
#define pmLPshift(p, sh) p_mLPshift(p, sh, currRing)
#define pLPCopyAndShiftLM(p, sh) p_LPCopyAndShiftLM(p, sh, currRing)

int p_LastVblock(poly p, const ring r);
int p_mLastVblock(poly p, const ring r);

#define pLastVblock(p) p_LastVblock(p,currRing)
#define pmLastVblock(p) p_mLastVblock(p,currRing)

int p_FirstVblock(poly p, const ring r);
int p_mFirstVblock(poly p, const ring r);

#define pFirstVblock(p) p_FirstVblock(p,currRing)
#define pmFirstVblock(p) p_mFirstVblock(p,currRing)

int isInV(poly p, const ring r);
int poly_isInV(poly p, const ring r);
int ideal_isInV(ideal I, const ring r);

int itoInsert(poly p, const ring r);

void k_SplitFrame(poly &m1, poly &m2, int at, const ring r);
#endif
#endif
