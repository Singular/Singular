#ifndef RING_H
#define RING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - the interpreter related ring operations
*/
/* $Id: ring.h,v 1.6 1997-12-15 22:46:39 obachman Exp $ */

/* includes */
#include "structs.h"
#include "polys-impl.h"

void   rSetHdl(idhdl h, BOOLEAN complete);
idhdl  rInit(char *s, sleftv* pn, sleftv* rv, sleftv* ord,
  BOOLEAN isDRing);
idhdl  rDefault(char *s);
int    rIsRingVar(char *n);
char * RingVar(short);
void   rWrite(ring r);
void   rKill(idhdl h);
void   rKill(ring r);
ring   rCopy(ring r);
idhdl  rFindHdl(ring r, idhdl n);
#ifdef DRING
void rDSet();
#endif
void   rDInit();
int rOrderName(char * ordername);
char * rOrdStr(ring r);
char * rVarStr(ring r);
char * rCharStr(ring r);
char * rString(ring r);
int    rChar(ring r=currRing);
#define rPar(r) (r->P)
char * rParStr(ring r);
int    rIsExtension(ring r);
int    rIsExtension();
int    rSum(ring r1, ring r2, ring &sum);
#ifdef COMP_FAST
void   rComplete(ring r);
#else
#define rComplete(r)    ((void*)0)
#endif


enum
{
  ringorder_no = 0,
  ringorder_a,
  ringorder_c,
  ringorder_C,
  ringorder_M,
  ringorder_lp,
  ringorder_dp,
  ringorder_Dp,
  ringorder_wp,
  ringorder_Wp,
  ringorder_ls,
  ringorder_ds,
  ringorder_Ds,
  ringorder_ws,
  ringorder_Ws,
  ringorder_unspec
};

#ifdef RDEBUG
extern short rNumber;
#endif

#endif

