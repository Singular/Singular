#ifndef RING_H
#define RING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - the interpreter related ring operations
*/
/* $Id: ring.h,v 1.40 1999-11-15 17:20:45 obachman Exp $ */

/* includes */
#include "structs.h"
#include "polys-impl.h"

#define SHORT_REAL_LENGTH 6 // use short reals for real <= 6 digits


void   rChangeCurrRing(ring r, BOOLEAN complete = TRUE);
void   rSetHdl(idhdl h, BOOLEAN complete = TRUE);
idhdl  rInit(char *s, sleftv* pn, sleftv* rv, sleftv* ord);
idhdl  rDefault(char *s);
int    rIsRingVar(char *n);
char * RingVar(short);
void   rWrite(ring r);
void   rKill(idhdl h);
void   rKill(ring r);
ring   rCopy(ring r);



#ifdef PDEBUG
#define rChangeSComps(c,s,l) rDBChangeSComps(c,s,l)
#define rGetSComps(c,s,l) rDBGetSComps(c,s,l)
void rDBChangeSComps(int* currComponents,
                     long* currShiftedComponents,
                     int length,
                     ring r = currRing);
void rDBGetSComps(int** currComponents,
                  long** currShiftedComponents,
                  int *length,
                  ring r = currRing);
#else
#define rChangeSComps(c,s,l) rNChangeSComps(c,s)
#define rGetSComps(c,s,l) rNGetSComps(c,s)
#endif

void rNChangeSComps(int* currComponents, long* currShiftedComponents, ring r = currRing);
void rNGetSComps(int** currComponents, long** currShiftedComponents, ring r = currRing);

idhdl  rFindHdl(ring r, idhdl n, idhdl w);
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

BOOLEAN rEqual(ring r1, ring r2, BOOLEAN qr = 1);
void   rUnComplete(ring r);

#define  rInternalChar(r) ((r)->ch)
#ifndef ABS
#define ABS(x) ((x) < 0 ? (-(x)) : (x))
#endif

inline BOOLEAN rField_is_Zp(ring r=currRing)
{ return (r->ch > 1) && (r->parameter==NULL); }

inline BOOLEAN rField_is_Zp(ring r, int p)
{ return (r->ch > 1 && r->ch == ABS(p) && r->parameter==NULL); }

inline BOOLEAN rField_is_Q(ring r=currRing)
{ return (r->ch == 0) && (r->parameter==NULL); }

inline BOOLEAN rField_is_numeric(ring r=currRing) /* R, long R, long C */
{ return (r->ch ==  -1); }

inline BOOLEAN rField_is_R(ring r=currRing)
{
  if (rField_is_numeric(r) && (r->ch_flags <= (short)SHORT_REAL_LENGTH))
    return (r->parameter==NULL);
  return FALSE;
}

inline BOOLEAN rField_is_GF(ring r=currRing)
{ return (r->ch > 1) && (r->parameter!=NULL); }

inline BOOLEAN rField_is_GF(ring r, int q)
{ return (r->ch == q); }

inline BOOLEAN rField_is_Zp_a(ring r=currRing)
{ return (r->ch < -1); }

inline BOOLEAN rField_is_Zp_a(ring r, int p)
{ return (r->ch < -1 ) && (-(r->ch) == ABS(p)); }

inline BOOLEAN rField_is_Q_a(ring r=currRing)
{ return (r->ch == 1); }

inline BOOLEAN rField_is_long_R(ring r=currRing)
{
  if (rField_is_numeric(r) && (r->ch_flags >(short)SHORT_REAL_LENGTH))
    return (r->parameter==NULL);
  return FALSE;
}

inline BOOLEAN rField_is_long_C(ring r=currRing)
{
  if (rField_is_numeric(r))
    return (r->parameter!=NULL);
  return FALSE;
}

inline BOOLEAN rField_has_simple_inverse(ring r=currRing)
{ return (r->ch>1) || (r->ch== -1); } /* Z/p, GF(p,n), R, long_R, long_C*/

inline BOOLEAN rField_has_simple_Alloc(ring r=currRing)
{ return (rField_is_Zp(r) || rField_is_GF(r) || rField_is_R(r)); }

/* Z/p, GF(p,n), R: nCopy, nNew, nDelete are dummies*/

inline BOOLEAN rField_is_Extension(ring r=currRing)
{ return (rField_is_Q_a(r)) || (rField_is_Zp_a(r)); } /* Z/p(a) and Q(a)*/

// this needs to be called whenever a new ring is created: new fields
// in ring are created (like VarOffset), unless they already exist
// with force == 1, new fields are _always_ created (overwritten),
// even if they exist
BOOLEAN rComplete(ring r, int force = 0);
// use this to free fields created by rComplete
void rUnComplete(ring r);
inline int rBlocks(ring r)
{
  int i=0;
  while (r->order[i]!=0) i++;
  return i+1;
}

typedef enum rRingOrder_t 
{
  ringorder_no = 0,
  ringorder_a,
  ringorder_c,
  ringorder_C,
  ringorder_M,
  ringorder_S,
  ringorder_s,
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
  #ifdef HAVE_SHIFTED_EXPONENTS
  ringorder_L,
  #endif
  ringorder_unspec
} rRingOrder_t;

typedef enum rOrderType_t
{
  rOrderType_General = 0, // non-simple ordering as specified by currRing
  rOrderType_CompExp,     // simple ordering, component has priority
  rOrderType_ExpComp,     // simple ordering, exponent vector has priority
                          // component not compatible with exp-vector order
  rOrderType_Exp,         // simple ordering, exponent vector has priority
                          // component is compatible with exp-vector order
  rOrderType_Syz,         // syzygy ordering
  rOrderType_Schreyer,    // Schreyer ordering
  rOrderType_Syz2dpc,     // syzcomp2dpc
  rOrderType_ExpNoComp    // simple ordering, differences in component are
                          // not considered
} rOrderType_t;

inline BOOLEAN rIsSyzIndexRing(ring r)
{ return r->order[0] == ringorder_s;}

inline int rGetCurrSyzLimit()
{ return (currRing->order[0] == ringorder_s ? 
          currRing->typ[0].data.syz.limit : 0);}

// Ring Manipulations
ring   rCurrRingAssureSyzComp();
void   rSetSyzComp(int k);
ring   rCurrRingAssure_dp_S();
ring   rCurrRingAssure_dp_C();
// return the max-comonent wchich has syzIndex i
// Assume: i<= syzIndex_limit
int rGetMaxSyzComp(int i);
  
BOOLEAN rHasSimpleOrder(ring r);
// returns TRUE, if simple lp or ls ordering
BOOLEAN rHasSimpleLexOrder(ring r);
rOrderType_t    rGetOrderType(ring r);
BOOLEAN rIsPolyVar(int i); /* returns TRUE if var(i) belongs to p-block */

void rOptimizeOrder(ring r);

#ifdef RDEBUG
#define rTest(r)    rDBTest(r, __FILE__, __LINE__)
extern BOOLEAN rDBTest(ring r, char* fn, int l);
#else
#define rTest(r)
#endif

void rDebugPrint(ring r);
void pDebugPrint(poly p);

#endif

