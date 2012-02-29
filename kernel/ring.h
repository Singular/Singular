#ifndef RING_H
#define RING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - the interpreter related ring operations
*/

/* includes */
#include <kernel/structs.h>
#include <kernel/polys-impl.h>

#define SHORT_REAL_LENGTH 6 // use short reals for real <= 6 digits

#if 0
enum n_coeffType
{
  n_unknown=0,
  n_Zp,
  n_Q,
  n_R,
  n_GF,
  n_long_R,
  n_Zp_a,
  n_Q_a,
  n_long_C
};
#endif


// #ifdef HAVE_PLURAL
#if 0
enum nc_type
{
  nc_error = -1, // Something's gone wrong!
  nc_general = 0, /* yx=q xy+... */
  nc_skew, /*1*/ /* yx=q xy */
  nc_comm, /*2*/ /* yx= xy */
  nc_lie,  /*3*/ /* yx=xy+... */
  nc_undef, /*4*/  /* for internal reasons */

  nc_exterior /*5*/ // Exterior Algebra(SCA): yx= -xy & (!:) x^2 = 0
};
#endif
// #endif

enum tHomog
{
   isNotHomog = FALSE,
   isHomog    = TRUE,
   testHomog
};

extern ring      currRing;
extern ideal     currQuotient;
extern idhdl      currRingHdl;


void   rChangeCurrRing(ring r);
void   rSetHdl(idhdl h);
ring   rInit(sleftv* pn, sleftv* rv, sleftv* ord);
idhdl  rDefault(const char *s);
ring   rDefault(int ch, int N, char **n);
ring rDefault(int ch, int N, char **n,int ord_size, int *ord, int *block0, int *block1, int ** wvhdl=NULL);

#define rIsRingVar(A) r_IsRingVar(A,currRing)
int    r_IsRingVar(const char *n, ring r);
void   rWrite(ring r);
void   rKill(idhdl h);
void   rKill(ring r);
ring   rCopy(ring r);
ring   rCopy0(const ring r, BOOLEAN copy_qideal = TRUE, BOOLEAN copy_ordering = TRUE);
ring   rOpposite(ring r);
ring   rEnvelope(ring r);

/// we must always have this test!
static inline bool rIsPluralRing(const ring r)
{
#ifdef HAVE_PLURAL
  nc_struct *n;
  return (r != NULL) && ((n=r->GetNC()) != NULL) /*&& (n->type != nc_error)*/;
#else
  return false;
#endif
}

static inline bool rIsRatGRing(const ring r)
{
#ifdef HAVE_PLURAL
  /* nc_struct *n; */
  return (r != NULL) /* && ((n=r->GetNC()) != NULL) */
          && (r->real_var_start>1);
#else
  return false;
#endif
}




// The following are for LaScala3 only!
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
idhdl rSimpleFindHdl(ring r, idhdl root, idhdl n);
const char * rSimpleOrdStr(int ord);
int rOrderName(char * ordername);
char * rOrdStr(ring r);
char * rVarStr(ring r);
char * rCharStr(ring r);
char * rString(ring r);
int    rChar(ring r=currRing);
#define rPar(r) (r->P)
#define rVar(r) (r->N)
char * rParStr(ring r);
int    rIsExtension(const ring r=currRing);
int    rSum(ring r1, ring r2, ring &sum);
int rSumInternal(ring r1, ring r2, ring &sum, BOOLEAN vartest, BOOLEAN dp_dp);

BOOLEAN rEqual(ring r1, ring r2, BOOLEAN qr = 1);
BOOLEAN rSamePolyRep(ring r1, ring r2);
void   rUnComplete(ring r);

#define  rInternalChar(r) ((r)->ch)

BOOLEAN rRing_is_Homog(ring r=currRing);
BOOLEAN rRing_has_CompLastBlock(ring r=currRing);

#ifdef HAVE_RINGS
static inline BOOLEAN rField_is_Ring_2toM(ring r=currRing)
{ return (r->ringtype == 1); }

static inline BOOLEAN rField_is_Ring_ModN(ring r=currRing)
{ return (r->ringtype == 2); }

static inline BOOLEAN rField_is_Ring_PtoM(ring r=currRing)
{ return (r->ringtype == 3); }

static inline BOOLEAN rField_is_Ring_Z(ring r=currRing)
{ return (r->ringtype == 4); }

static inline BOOLEAN rField_is_Ring(ring r=currRing)
{ return (r->ringtype != 0); }

static inline BOOLEAN rField_is_Domain(ring r=currRing)
{ return (r->ringtype == 4 || r->ringtype == 0); }

static inline BOOLEAN rField_has_Units(ring r=currRing)
{ return ((r->ringtype == 1) || (r->ringtype == 2) || (r->ringtype == 3)); }
#else
#define rField_is_Ring(A) (0)
#define rField_is_Ring_2toM(A) (0)
#define rField_is_Ring_ModN(A) (0)
#define rField_is_Ring_PtoM(A) (0)
#define rField_is_Ring_Z(A) (0)
#define rField_is_Domain(A) (1)
#define rField_has_Units(A) (1)
#endif

#ifdef HAVE_RINGS
static inline BOOLEAN rField_is_Zp(ring r=currRing)
{ return (r->ringtype == 0) && (r->ch > 1) && (r->parameter==NULL); }

static inline BOOLEAN rField_is_Zp(ring r, int p)
{ return (r->ringtype == 0) && (r->ch > 1 && r->ch == ABS(p) && r->parameter==NULL); }

static inline BOOLEAN rField_is_Q(ring r=currRing)
{ return (r->ringtype == 0) && (r->ch == 0) && (r->parameter==NULL); }

static inline BOOLEAN rField_is_numeric(ring r=currRing) /* R, long R, long C */
{ return (r->ringtype == 0) && (r->ch ==  -1); }

static inline BOOLEAN rField_is_R(ring r=currRing)
{
  if (rField_is_numeric(r) && (r->float_len <= (short)SHORT_REAL_LENGTH))
    return (r->ringtype == 0) && (r->parameter==NULL);
  return FALSE;
}

static inline BOOLEAN rField_is_GF(ring r=currRing)
{ return (r->ringtype == 0) && (r->ch > 1) && (r->parameter!=NULL); }

static inline BOOLEAN rField_is_GF(ring r, int q)
{ return (r->ringtype == 0) && (r->ch == q); }

static inline BOOLEAN rField_is_Zp_a(ring r=currRing)
{ return (r->ringtype == 0) && (r->ch < -1); }

static inline BOOLEAN rField_is_Zp_a(ring r, int p)
{ return (r->ringtype == 0) && (r->ch < -1 ) && (-(r->ch) == ABS(p)); }

static inline BOOLEAN rField_is_Q_a(ring r=currRing)
{ return (r->ringtype == 0) && (r->ch == 1); }

static inline BOOLEAN rField_is_long_R(ring r=currRing)
{
  if (rField_is_numeric(r) && (r->float_len >(short)SHORT_REAL_LENGTH))
    return (r->ringtype == 0) && (r->parameter==NULL);
  return FALSE;
}

static inline BOOLEAN rField_is_long_C(ring r=currRing)
{
  if (rField_is_numeric(r))
    return (r->ringtype == 0) && (r->parameter!=NULL);
  return FALSE;
}
#else
static inline BOOLEAN rField_is_Zp(ring r=currRing)
{ return (r->ch > 1) && (r->parameter==NULL); }

static inline BOOLEAN rField_is_Zp(ring r, int p)
{ return (r->ch > 1 && r->ch == ABS(p) && r->parameter==NULL); }

static inline BOOLEAN rField_is_Q(ring r=currRing)
{ return (r->ch == 0) && (r->parameter==NULL); }

static inline BOOLEAN rField_is_numeric(ring r=currRing) /* R, long R, long C */
{ return (r->ch ==  -1); }

static inline BOOLEAN rField_is_R(ring r=currRing)
{
  if (rField_is_numeric(r) && (r->float_len <= (short)SHORT_REAL_LENGTH))
    return (r->parameter==NULL);
  return FALSE;
}

static inline BOOLEAN rField_is_GF(ring r=currRing)
{ return (r->ch > 1) && (r->parameter!=NULL); }

static inline BOOLEAN rField_is_GF(ring r, int q)
{ return (r->ch == q); }

static inline BOOLEAN rField_is_Zp_a(ring r=currRing)
{ return (r->ch < -1); }

static inline BOOLEAN rField_is_Zp_a(ring r, int p)
{ return (r->ch < -1 ) && (-(r->ch) == ABS(p)); }

static inline BOOLEAN rField_is_Q_a(ring r=currRing)
{ return (r->ch == 1); }

static inline BOOLEAN rField_is_long_R(ring r=currRing)
{
  if (rField_is_numeric(r) && (r->float_len >(short)SHORT_REAL_LENGTH))
    return (r->parameter==NULL);
  return FALSE;
}

static inline BOOLEAN rField_is_long_C(ring r=currRing)
{
  if (rField_is_numeric(r))
    return (r->parameter!=NULL);
  return FALSE;
}
#endif

static inline BOOLEAN rField_has_simple_inverse(ring r=currRing)
/* { return (r->ch>1) || (r->ch== -1); } *//* Z/p, GF(p,n), R, long_R, long_C*/
#ifdef HAVE_RINGS
{ return (r->ringtype > 0) || (r->ch>1) || ((r->ch== -1) && (r->float_len < 10)); } /* Z/2^n, Z/p, GF(p,n), R, long_R, long_C*/
#else
{ return (r->ch>1) || ((r->ch== -1) && (r->float_len < 10)); } /* Z/p, GF(p,n), R, long_R, long_C*/
#endif

static inline BOOLEAN rField_has_simple_Alloc(ring r=currRing)
{ return (rField_is_Zp(r)
       || rField_is_GF(r)
#ifdef HAVE_RINGS
       || rField_is_Ring_2toM(r)
#endif
       || rField_is_R(r)); }

/* Z/p, GF(p,n), R: nCopy, nNew, nDelete are dummies*/
static inline BOOLEAN rField_is_Extension(ring r=currRing)
{ return (rField_is_Q_a(r)) || (rField_is_Zp_a(r)); } /* Z/p(a) and Q(a)*/

n_coeffType rFieldType(ring r);

/// this needs to be called whenever a new ring is created: new fields
/// in ring are created (like VarOffset), unless they already exist
/// with force == 1, new fields are _always_ created (overwritten),
/// even if they exist
BOOLEAN rComplete(ring r, int force = 0);
// use this to free fields created by rComplete //?

static inline int rBlocks(ring r)
{
  int i=0;
  while (r->order[i]!=0) i++;
  return i+1;
}

// misc things
static inline char* rRingVar(short i)
{
  return currRing->names[i];
}
static inline char* rRingVar(short i, ring r)
{
  return r->names[i];
}
static inline BOOLEAN rShortOut(ring r)
{
  return (r->ShortOut);
}

/// order stuff
typedef enum rRingOrder_t
{
  ringorder_no = 0,
  ringorder_a,
  ringorder_a64, ///< for int64 weights
  ringorder_c,
  ringorder_C,
  ringorder_M,
  ringorder_S, ///< S?
  ringorder_s, ///< s?
  ringorder_lp,
  ringorder_dp,
  ringorder_rp,
  ringorder_Dp,
  ringorder_wp,
  ringorder_Wp,
  ringorder_ls,
  ringorder_ds,
  ringorder_Ds,
  ringorder_ws,
  ringorder_Ws,
  ringorder_am,
  ringorder_L,
  // the following are only used internally
  ringorder_aa, ///< for idElimination, like a, except pFDeg, pWeigths ignore it
  ringorder_rs, ///< opposite of ls
  ringorder_IS, ///< Induced (Schreyer) ordering
  ringorder_unspec
} rRingOrder_t;

typedef enum rOrderType_t
{
  rOrderType_General = 0, ///< non-simple ordering as specified by currRing
  rOrderType_CompExp,     ///< simple ordering, component has priority
  rOrderType_ExpComp,     ///< simple ordering, exponent vector has priority
                          ///< component not compatible with exp-vector order
  rOrderType_Exp,         ///< simple ordering, exponent vector has priority
                          ///< component is compatible with exp-vector order
  rOrderType_Syz,         ///< syzygy ordering
  rOrderType_Schreyer,    ///< Schreyer ordering
  rOrderType_Syz2dpc,     ///< syzcomp2dpc
  rOrderType_ExpNoComp    ///< simple ordering, differences in component are
                          ///< not considered
} rOrderType_t;

static inline BOOLEAN rIsSyzIndexRing(const ring r)
{ return r->order[0] == ringorder_s;}

static inline int rGetCurrSyzLimit(const ring r = currRing)
{ return (rIsSyzIndexRing(r)? r->typ[0].data.syz.limit : 0);}

// Ring Manipulations
ring   rAssure_HasComp(ring r);
ring   rCurrRingAssure_SyzComp();
void   rSetSyzComp(int k);
ring   rCurrRingAssure_dp_S();
ring   rCurrRingAssure_dp_C();
ring   rCurrRingAssure_C_dp();

/// makes sure that c/C ordering is last ordering
ring   rCurrRingAssure_CompLastBlock();

/// makes sure that c/C ordering is last ordering and SyzIndex is first
ring   rCurrRingAssure_SyzComp_CompLastBlock();
ring rAssure_TDeg(ring r, int start_var, int end_var, int &pos);

/// return the max-comonent wchich has syzIndex i
/// Assume: i<= syzIndex_limit
int rGetMaxSyzComp(int i);

BOOLEAN rHasSimpleOrder(const ring r);

/// returns TRUE, if simple lp or ls ordering
BOOLEAN rHasSimpleLexOrder(const ring r);

// return TRUE if p->exp[r->pOrdIndex] holds total degree of p */
//inline BOOLEAN rHasGlobalOrdering(const ring r=currRing)
//{ return (r->OrdSgn==1); }
#define rHasGlobalOrdering(R) ((R)->OrdSgn==1)
#define rHasGlobalOrdering_currRing() (pOrdSgn==1)
//inline BOOLEAN rHasLocalOrMixedOrdering(const ring r=currRing)
//{ return (r->OrdSgn==-1); }
#define rHasLocalOrMixedOrdering(R) ((R)->OrdSgn==-1)
#define rHasLocalOrMixedOrdering_currRing() (pOrdSgn==-1)
BOOLEAN rOrd_is_Totaldegree_Ordering(ring r =currRing);

/// return TRUE if p_SetComp requires p_Setm
BOOLEAN rOrd_SetCompRequiresSetm(ring r);
rOrderType_t    rGetOrderType(ring r);

/// returns TRUE if var(i) belongs to p-block
BOOLEAN rIsPolyVar(int i, ring r = currRing);

static inline BOOLEAN rOrd_is_Comp_dp(ring r)
{
  return ((r->order[0] == ringorder_c || r->order[0] == ringorder_C) &&
          r->order[1] == ringorder_dp &&
          r->order[2] == 0);
}

#ifdef RDEBUG
#define rTest(r)    rDBTest(r, __FILE__, __LINE__)
extern BOOLEAN rDBTest(ring r, const char* fn, const int l);
#else
#define rTest(r)
#endif

ring rModifyRing(ring r, BOOLEAN omit_degree,
                         BOOLEAN omit_comp,
                         unsigned long exp_limit);

/// construct Wp, C ring
ring rModifyRing_Wp(ring r, int* weights);
void rModify_a_to_A(ring r);

void rKillModifiedRing(ring r);
// also frees weights
void rKillModified_Wp_Ring(ring r);

ring rModifyRing_Simple(ring r, BOOLEAN omit_degree, BOOLEAN omit_comp, unsigned long exp_limit, BOOLEAN &simple);
void rKillModifiedRing_Simple(ring r);

#ifdef RDEBUG
void rDebugPrint(ring r);
void pDebugPrint(poly p);
void p_DebugPrint(poly p, const ring r);
#endif

#ifndef NDEBUG
/// debug-print at most nTerms (2 by default) terms from poly/vector p,
/// assuming that lt(p) lives in lmRing and tail(p) lives in tailRing.
void p_DebugPrint(const poly p, const ring lmRing, const ring tailRing, const int nTerms = 2);
#endif

int64 * rGetWeightVec(ring r);
void rSetWeightVec(ring r, int64 *wv);

lists rDecompose(const ring r);
ring rCompose(const lists  L);
/////////////////////////////
// Auxillary functions
//

BOOLEAN rCheckIV(intvec *iv);
int rTypeOfMatrixOrder(intvec * order);
void rDelete(ring r);

extern omBin sip_sring_bin;
#endif
