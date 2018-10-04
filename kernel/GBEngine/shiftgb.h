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

poly p_LPCopyAndShiftLM(poly p, int sh, const ring r);
#define pLPCopyAndShiftLM(p, sh) p_LPCopyAndShiftLM(p, sh, currRing)

int isInV(poly p, const ring r);
int poly_isInV(poly p, const ring r);
int ideal_isInV(ideal I, const ring r);

int itoInsert(poly p, const ring r);

void k_SplitFrame(poly &m1, poly &m2, int at, const ring r);
#endif
#endif
