#ifndef RING_H
#define RING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - the interpreter related ring operations
*/
/* $Id: ring.h,v 1.19 1998-12-03 17:58:05 Singular Exp $ */

/* includes */
#include "structs.h"
#include "polys-impl.h"

#ifdef DRING
void   rChangeCurrRing(ring r, BOOLEAN complete = TRUE, idhdl h = NULL);
#else
void   rChangeCurrRing(ring r, BOOLEAN complete = TRUE);
#endif
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
idhdl  rFindHdl(ring r, idhdl n, idhdl w);
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
void   rComplete(ring r);
int    rBlocks(ring r);


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

typedef enum rOrderType_t
{
  rOrderType_General = 0, // non-simple ordering as specified by currRing
  rOrderType_CompExp,     // simple ordering, component has priority
  rOrderType_ExpComp,     // simple ordering, exponent vector has priority
                          // component not compatible with exp-vector order
  rOrderType_Exp,         // simple ordering, exponent vector has priority
                          // component is compatible with exp-vector order
  rOrderType_Syz,         // syzygy ordering
  rOrderType_Schreyer     // Schreyer ordering
} rOrderType_t;

BOOLEAN rHasSimpleOrder(ring r);
// returns TRUE, if simple lp or ls ordering
BOOLEAN rHasSimpleLexOrder(ring r);
rOrderType_t    rGetOrderType(ring r);
BOOLEAN rIsPolyVar(int i); /* returns TRUE if var(i) belongs to p-block */

void rOptimizeOrder(ring r);

#ifdef RDEBUG
extern short rNumber; /* current ring id (r->no) */
#define rTest(r)    rDBTest(r, __FILE__, __LINE__)
extern BOOLEAN rDBTest(ring r, char* fn, int l);
#else
#define rTest(r)
#endif

#endif

