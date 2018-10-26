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

int itoInsert(poly p, const ring r);

#endif
#endif
