#ifndef RING_H
#define RING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - the interpreter related ring operations
*/
/* $Id: ring.h,v 1.32 1999-09-27 11:38:32 obachman Exp $ */

/* includes */
#include "structs.h"
#include "polys-impl.h"

#define SHORT_REAL_LENGTH 6 // use short reals for real <= 6 digits


#ifdef DRING
void   rChangeCurrRing(ring r, BOOLEAN complete = TRUE, idhdl h = NULL);
#else
void   rChangeCurrRing(ring r, BOOLEAN complete = TRUE);
#endif
void   rSetHdl(idhdl h, BOOLEAN complete = TRUE);
idhdl  rInit(char *s, sleftv* pn, sleftv* rv, sleftv* ord);
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
BOOLEAN rEqual(ring r1, ring r2, BOOLEAN qr = 1);
BOOLEAN   rComplete(ring r, int force = 0);
void   rUnComplete(ring r);
int    rBlocks(ring r);

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
inline BOOLEAN rField_is_Extension(ring r=currRing)
{ return (rField_is_Q_a(r)) || (rField_is_Zp_a(r)); } /* Z/p(a) and Q(a)*/

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
  rOrderType_Schreyer,     // Schreyer ordering
  rOrderType_Syz2dpc,     // syzcomp2dpc
  rOrderType_ExpNoComp    // simple ordering, differences in component are
                          // not considered
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

