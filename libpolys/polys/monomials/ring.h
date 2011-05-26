#ifndef RING_H
#define RING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - the interpreter related ring operations
*/
/* $Id$ */

/* includes */
#include <omalloc/omalloc.h>
#include <misc/auxiliary.h>
#include <coeffs/coeffs.h>
//#include <polys/monomials/polys-impl.h>
//

/* constants */
#define SHORT_REAL_LENGTH 6 // use short reals for real <= 6 digits

/* forward declaration of types */
class idrec;
typedef idrec *   idhdl;
struct  spolyrec;
typedef struct spolyrec    polyrec;
typedef polyrec *          poly;
struct ip_sring;
typedef struct ip_sring *         ring;
class intvec;
struct p_Procs_s;
typedef struct p_Procs_s p_Procs_s;
class slists;
typedef slists *           lists;
class kBucket;
typedef kBucket*           kBucket_pt;

struct sip_sideal;
typedef struct sip_sideal *       ideal;

struct sip_smap;
typedef struct sip_smap *         map;


#if SIZEOF_LONG == 4
typedef long long int64;
#elif SIZEOF_LONG == 8
typedef long int64;
#else
#error int64 undefined
#endif

/* the function pointer types */

typedef long     (*pLDegProc)(poly p, int *length, ring r);
typedef long     (*pFDegProc)(poly p, ring r);
typedef void     (*p_SetmProc)(poly p, const ring r);



typedef enum
{
  ro_dp, // ordering is a degree ordering
  ro_wp, // ordering is a weighted degree ordering
  ro_wp64, // ordering is a weighted64 degree ordering
  ro_wp_neg, // ordering is a weighted degree ordering
             // with possibly negative weights
  ro_cp,    // ordering duplicates variables
  ro_syzcomp, // ordering indicates "subset" of component number (ringorder_S)
  ro_syz, // ordering  with component number >syzcomp is lower (ringorder_s)
  ro_isTemp, ro_is, // Induced Syzygy (Schreyer) ordering (and prefix data placeholder dummy) (ringorder_IS)
  ro_none
}
ro_typ;

// ordering is a degree ordering
struct sro_dp
{
  short place;  // where degree is stored (in L):
  short start;  // bounds of ordering (in E):
  short end;
};
typedef struct sro_dp sro_dp;

// ordering is a weighted degree ordering
struct sro_wp
{
  short place;  // where weighted degree is stored (in L)
  short start;  // bounds of ordering (in E)
  short end;
  int *weights; // pointers into wvhdl field
};
typedef struct sro_wp sro_wp;

// ordering is a weighted degree ordering
struct sro_wp64
{
    short place;  // where weighted degree is stored (in L)
    short start;  // bounds of ordering (in E)
    short end;
    int64 *weights64; // pointers into wvhdl field
};
typedef struct sro_wp64 sro_wp64;

// ordering duplicates variables
struct sro_cp
{
  short place;  // where start is copied to (in E)
  short start;  // bounds of sources of copied variables (in E)
  short end;
};
typedef struct sro_cp sro_cp;

// ordering indicates "subset" of component number
struct sro_syzcomp
{
  short place;  // where the index is stored (in L)
  long *ShiftedComponents; // pointer into index field
  int* Components;
#ifdef PDEBUG
  long length;
#endif
};
typedef struct sro_syzcomp sro_syzcomp;

// ordering  with component number >syzcomp is lower
struct sro_syz
{
  short place;       // where the index is stored (in L)
  int limit;         // syzcomp
  int* syz_index;    // mapping Component -> SyzIndex for Comp <= limit
  int  curr_index;   // SyzIndex for Component > limit
};

typedef struct sro_syz sro_syz;
// Induced Syzygy (Schreyer) ordering is built inductively as follows:
// we look for changes made by ordering blocks which are between prefix/suffix markers:
// that is: which variables where placed by them and where (judging by v)

// due to prefix/suffix nature we need some placeholder:
// prefix stores here initial state
// suffix cleares this up
struct sro_ISTemp
{
  short start; // 1st member SHOULD be short "place"
  int   suffixpos;
  int*  pVarOffset; // copy!
};

// So this is the actuall thing!
// suffix uses last sro_ISTemp (cleares it up afterwards) and
// creates this block
struct sro_IS
{
  short start, end;  // which part of L we want to want to update...
  int*  pVarOffset; // same as prefix!

  int limit; // first referenced component

  // reference poly set?? // Should it be owned by ring?!!!
  ideal F; // reference leading (module)-monomials set. owned by ring...
  const intvec* componentWeights; // component weights! owned by ring...
};

typedef struct sro_IS sro_IS;
typedef struct sro_ISTemp sro_ISTemp;

struct sro_ord
{
  ro_typ  ord_typ;
  int     order_index; // comes from r->order[order_index]
  union
  {
     sro_dp dp;
     sro_wp wp;
     sro_wp64 wp64;
     sro_cp cp;
     sro_syzcomp syzcomp;
     sro_syz syz;
     sro_IS is;
     sro_ISTemp isTemp;
  } data;
};

#ifdef HAVE_PLURAL
struct nc_struct;
typedef struct nc_struct   nc_struct;
#endif

struct ip_sring
{
// each entry must have a description and a procedure defining it,
// general ordering: pointer/structs, long, int, short, BOOLEAN/char/enum
// general defining procedures: rInit, rComplete, interpreter, ??
  idhdl      idroot; /* local objects , interpreter*/
  int*       order;  /* array of orderings, rInit/rSleftvOrdering2Ordering */
  int*       block0; /* starting pos., rInit/rSleftvOrdering2Ordering*/
  int*       block1; /* ending pos., rInit/rSleftvOrdering2Ordering*/
//  char**     parameter; /* names of parameters, rInit */
//  number     minpoly;  /* replaced by minideal->m[0] */
  ideal      minideal;   /* for Q_a/Zp_a, rInit;
                            for a start, we assume that there is either no
                            or exactly one generator in minideal, playing
                            the role of the former minpoly; minideal may
                            also be NULL which coincides with the
                            no-generator-case */
  int**      wvhdl;  /* array of weight vectors, rInit/rSleftvOrdering2Ordering */
  char **    names;  /* array of variable names, rInit */

  // what follows below here should be set by rComplete, _only_
  long      *ordsgn;  /* array of +/- 1 (or 0) for comparing monomials */
                       /*  ExpL_Size entries*/

  // is NULL for lp or N == 1, otherwise non-NULL (with OrdSize > 0 entries) */
  sro_ord*   typ;   /* array of orderings + sizes, OrdSize entries */
  /* if NegWeightL_Size > 0, then NegWeightL_Offset[0..size_1] is index of longs
  in ExpVector whose values need an offset due to negative weights */
  /* array of NegWeigtL_Size indicies */
  int*      NegWeightL_Offset;

  int*     VarOffset;

  ideal      qideal; /* extension to the ring structure: qring, rInit */

  int*     firstwv;

  omBin    PolyBin; /* Bin from where monoms are allocated */
  intvec * pModW;   /* std: module weights */
  poly      ppNoether; /*  variables, set by procedures from hecke/kstd1:

                            the highest monomial below pHEdge */
// #ifdef HAVE_RINGS
//   unsigned int  cf->ringtype;  /* cring = 0 => coefficient field, cring = 1 => coeffs from Z/2^m */
//   int_number    cf->modBase; /* Z/(ringflag^cf->modExponent)=Z/cf->modNumber*/
//   unsigned long cf->modExponent;
//   unsigned long cf->modNumber;  /* Z/cf->modNumber */
//   int_number    cf->modNumber;
// #endif
  
  unsigned long options; /* ring dependent options */

//  int        ch;  /* characteristic, rInit */
  int        ref; /* reference counter to the ring, interpreter */

  short      float_len; /* additional char-flags, rInit */
  short      float_len2; /* additional char-flags, rInit */

  short      N;      /* number of vars, rInit */

  short      OrdSgn; /* 1 for polynomial rings, -1 otherwise, rInit */

  short     firstBlockEnds;
#ifdef HAVE_PLURAL
  short     real_var_start, real_var_end;
#endif

#ifdef HAVE_SHIFTBBA
  short          isLPring; /* 0 for non-letterplace rings, otherwise the number of LP blocks, at least 1, known also as lV */
#endif

  BOOLEAN   VectorOut;
  BOOLEAN   ShortOut;
  BOOLEAN   CanShortOut;
  BOOLEAN   LexOrder; // TRUE if the monomial ordering has polynomial and power series blocks
  BOOLEAN   MixedOrder; // TRUE for global/local mixed orderings, FALSE otherwise
  BOOLEAN   pLexOrder; /* TRUE if the monomial ordering is not compatible with pFDeg */

  BOOLEAN   ComponentOrder; // ???

  // what follows below here should be set by rComplete, _only_
  // contains component, but no weight fields in E */
  short      ExpL_Size; // size of exponent vector in long
  short      CmpL_Size; // portions which need to be compared
  /* number of long vars in exp vector:
     long vars are those longs in the exponent vector which are
     occupied by variables, only */
  short      VarL_Size;
  short      BitsPerExp; /* number of bits per exponent */
  short      ExpPerLong; /* maximal number of Exponents per long */
  short      pCompIndex; /* p->exp.e[pCompIndex] is the component */
  short      pOrdIndex; /* p->exp[pOrdIndex] is pGetOrd(p) */
  short      OrdSize; /* size of ord vector (in sro_ord) */

  /* if >= 0, long vars in exp vector are consecutive and start there
     if <  0, long vars in exp vector are not consecutive */
  short     VarL_LowIndex;
  // number of exponents in r->VarL_Offset[0]
  // is minimal number of exponents in a long var
  short     MinExpPerLong;

  short     NegWeightL_Size;
  /* array of size VarL_Size,
     VarL_Offset[i] gets i-th long var in exp vector */
  int*      VarL_Offset;

  /* mask for getting single exponents */
  unsigned long bitmask;
  /* mask used for divisiblity tests */
  unsigned long divmask; // rComplete

  p_Procs_s*    p_Procs; // rComplete/p_ProcsSet

  /* FDeg and LDeg */
  pFDegProc     pFDeg; // rComplete/rSetDegStuff
  pLDegProc     pLDeg; // rComplete/rSetDegStuff

  /* as it was determined by rComplete */
  pFDegProc     pFDegOrig;
  /* and as it was determined before rOptimizeLDeg */
  pLDegProc     pLDegOrig;

  p_SetmProc    p_Setm;
  n_Procs_s*    cf;
#ifdef HAVE_PLURAL
  private:
    nc_struct*    _nc; // private
  public:
    inline const nc_struct* GetNC() const { return _nc; }; // public!!!
    inline nc_struct*& GetNC() { return _nc; }; // public!!!
#endif
 public:
  operator coeffs() const { return cf; }
};

enum tHomog
{
   isNotHomog = FALSE,
   isHomog    = TRUE,
   testHomog
};

////////// DEPRECATED
/////// void   rChangeCurrRing(ring r);
//void   rSetHdl(idhdl h);
//ring   rInit(sleftv* pn, sleftv* rv, sleftv* ord);
idhdl  rDefault(const char *s);
ring   rDefault(int ch, int N, char **n);
ring   rDefault(const coeffs cf, int N, char **n);
ring rDefault(int ch, int N, char **n,int ord_size, int *ord, int *block0, int *block1);
ring rDefault(const coeffs cf, int N, char **n,int ord_size, int *ord, int *block0, int *block1);

// #define rIsRingVar(A) r_IsRingVar(A,currRing)
int    r_IsRingVar(const char *n, ring r);
void   rWrite(ring r);
//void   rKill(idhdl h);
void   rKill(ring r);
ring   rCopy(ring r);
ring   rCopy0(const ring r, BOOLEAN copy_qideal = TRUE, BOOLEAN copy_ordering = TRUE);
ring   rOpposite(ring r);
ring   rEnvelope(ring r);

/// we must always have this test!
static inline bool rIsPluralRing(const ring r)
{
  assume(r != NULL);   
#ifdef HAVE_PLURAL
  nc_struct *n;
  return (r != NULL) && ((n=r->GetNC()) != NULL) /*&& (n->type != nc_error)*/;
#else
  return false;
#endif
}

static inline bool rIsRatGRing(const ring r)
{
  assume(r != NULL);
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
                     ring r);
void rDBGetSComps(int** currComponents,
                  long** currShiftedComponents,
                  int *length,
                  ring r);
#else
#define rChangeSComps(c,s,l) rNChangeSComps(c,s)
#define rGetSComps(c,s,l) rNGetSComps(c,s)
#endif

void rNChangeSComps(int* currComponents, long* currShiftedComponents, ring r);
void rNGetSComps(int** currComponents, long** currShiftedComponents, ring r);

//idhdl  rFindHdl(ring r, idhdl n, idhdl w);
//idhdl rSimpleFindHdl(ring r, idhdl root, idhdl n);
const char * rSimpleOrdStr(int ord);
int rOrderName(char * ordername);
char * rOrdStr(ring r);
char * rVarStr(ring r);
char * rCharStr(ring r);
char * rString(ring r);
int    rChar(ring r);

char * rParStr(ring r);

int    rSum(ring r1, ring r2, ring &sum);
int rSumInternal(ring r1, ring r2, ring &sum, BOOLEAN vartest, BOOLEAN dp_dp);

BOOLEAN rEqual(ring r1, ring r2, BOOLEAN qr = 1);
BOOLEAN rSamePolyRep(ring r1, ring r2);
void   rUnComplete(ring r);

BOOLEAN rRing_is_Homog(ring r);
BOOLEAN rRing_has_CompLastBlock(ring r);

#ifdef HAVE_RINGS
static inline BOOLEAN rField_is_Ring_2toM(const ring r)
{ assume(r != NULL); return ( getCoeffType(r->cf) == n_Z2m && nCoeff_is_Ring_2toM(r->cf) ); }

static inline BOOLEAN rField_is_Ring_ModN(const ring r)
{ assume(r != NULL); return ( getCoeffType(r->cf) == n_Zn && nCoeff_is_Ring_ModN(r->cf) ); }

static inline BOOLEAN rField_is_Ring_PtoM(const ring r)
{ assume(r != NULL); return (getCoeffType(r->cf) == n_Zpn && nCoeff_is_Ring_PtoM(r->cf) ); }

static inline BOOLEAN rField_is_Ring_Z(const ring r)
{ assume(r != NULL); return (getCoeffType(r->cf) == n_Z && nCoeff_is_Ring_Z(r->cf) ); }

static inline BOOLEAN rField_is_Ring(const ring r)
{ assume(r != NULL); return nCoeff_is_Ring(r->cf); }

static inline BOOLEAN rField_is_Domain(const ring r)
{ assume(r != NULL); return nCoeff_is_Domain(r->cf); }

static inline BOOLEAN rField_has_Units(const ring r)
{ assume(r != NULL); return nCoeff_has_Units(r->cf); }
#else
#define rField_is_Ring(A) (0)
#define rField_is_Ring_2toM(A) (0)
#define rField_is_Ring_ModN(A) (0)
#define rField_is_Ring_PtoM(A) (0)
#define rField_is_Ring_Z(A) (0)
#define rField_is_Domain(A) (1)
#define rField_has_Units(A) (1)
#endif

static inline BOOLEAN rField_is_Zp(const ring r)
{ assume(r != NULL); return (getCoeffType(r->cf) == n_Zp); }

static inline BOOLEAN rField_is_Zp(const ring r, int p)
{ assume(r != NULL); return (getCoeffType(r->cf) == n_Zp) && (r->cf->ch == p); }

static inline BOOLEAN rField_is_Q(const ring r)
{ assume(r != NULL); return nCoeff_is_Q(r->cf); }

static inline BOOLEAN rField_is_numeric(const ring r) /* R, long R, long C */
{ assume(r != NULL); return nCoeff_is_numeric(r->cf); }

static inline BOOLEAN rField_is_R(const ring r)
{ assume(r != NULL); return nCoeff_is_R(r->cf); }

static inline BOOLEAN rField_is_GF(const ring r)
{ assume(r != NULL); return nCoeff_is_GF(r->cf); }

static inline BOOLEAN rField_is_GF(const ring r, int q)
{ assume(r != NULL); return nCoeff_is_GF(r->cf, q); }

/* DO NOT USE; just here for compatibility reasons towards
   the SINGULAR svn trunk */
static inline BOOLEAN rField_is_Zp_a(const ring r)
{ assume(r != NULL); return nCoeff_is_Zp_a(r->cf); }

/* DO NOT USE; just here for compatibility reasons towards
   the SINGULAR svn trunk */
static inline BOOLEAN rField_is_Zp_a(const ring r, int p)
{ assume(r != NULL); return nCoeff_is_Zp_a(r->cf, p); }

/* DO NOT USE; just here for compatibility reasons towards
   the SINGULAR svn trunk */
static inline BOOLEAN rField_is_Q_a(const ring r)
{ assume(r != NULL); return nCoeff_is_Q_a(r->cf); }
   
static inline BOOLEAN rField_is_long_R(const ring r)
{ assume(r != NULL); return nCoeff_is_long_R(r->cf); }

static inline BOOLEAN rField_is_long_C(const ring r)
{ assume(r != NULL); return nCoeff_is_long_C(r->cf); }

static inline BOOLEAN rField_has_simple_inverse(const ring r)
{ assume(r != NULL); return nCoeff_has_simple_inverse(r->cf); }

static inline BOOLEAN rField_has_simple_Alloc(const ring r)
{ assume(r != NULL); return nCoeff_has_simple_Alloc(r->cf); }

/* Z/p, GF(p,n), R: nCopy, nNew, nDelete are dummies*/
static inline BOOLEAN rField_is_Extension(const ring r)
{ assume(r != NULL); return nCoeff_is_Extension(r->cf); } /* Z/p(a) and Q(a)*/

n_coeffType rFieldType(const ring r);

/// this needs to be called whenever a new ring is created: new fields
/// in ring are created (like VarOffset), unless they already exist
/// with force == 1, new fields are _always_ created (overwritten),
/// even if they exist
BOOLEAN rComplete(ring r, int force = 0);
// use this to free fields created by rComplete //?

static inline int rBlocks(ring r)
{
  assume(r != NULL);
  int i=0;
  while (r->order[i]!=0) i++;
  return i+1;
}

// misc things
static inline char* rRingVar(short i, const ring r)
{
  assume(r != NULL); return r->names[i];
}
static inline BOOLEAN rShortOut(const ring r)
{
  assume(r != NULL); return (r->ShortOut);
}

/// #define rVar(r) (r->N)
static inline short rVar(const ring r)
{
  assume(r != NULL);
  return r->N;
}

/// (r->cf->P)
static inline short rPar(const ring r)
{
  assume(r != NULL);
  const coeffs C = r->cf;
  assume(C != NULL);

  if( rField_is_Extension(r) )
  {
    const ring R = C->extRing;
    assume( R != NULL );
    return rVar( R );
  }
  return 0;
}


/// (r->cf->parameter)
static inline char** rParameter(const ring r)
{
  assume(r != NULL);
  const coeffs C = r->cf;
  assume(C != NULL);

  if( rField_is_Extension(r) )
  {
    const ring R = C->extRing;
    assume( R != NULL );
    return R->names;
  }
  return NULL;
}
   
/* R, Q, Fp: FALSE */
static inline BOOLEAN rIsExtension(const ring r)
{
  assume( (rParameter(r)!=NULL) == rField_is_Extension(r) ); // ?
  return rField_is_Extension(r);
}

/// Tests whether '(r->cf->minpoly) == NULL'
BOOLEAN rMinpolyIsNULL(const ring r);


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
  ringorder_L,
  // the following are only used internally
  ringorder_aa, ///< for idElimination, like a, except pFDeg, pWeigths ignore it
  ringorder_rs, ///< ???
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
{ assume(r != NULL); return r->order[0] == ringorder_s;}

static inline int rGetCurrSyzLimit(const ring r)
{ assume(r != NULL); return (rIsSyzIndexRing(r)? r->typ[0].data.syz.limit : 0);}

// Ring Manipulations
ring   rAssure_HasComp(const ring r);
ring   rAssure_SyzComp(const ring r, BOOLEAN complete = TRUE);
void   rSetSyzComp(int k, const ring r);
ring   rAssure_dp_S(const ring r);
ring   rAssure_dp_C(const ring r);
ring   rAssure_C_dp(const ring r);
/// makes sure that c/C ordering is last ordering
ring rAssure_CompLastBlock(const ring r);

/// makes sure that c/C ordering is last ordering and SyzIndex is first
ring   rAssure_SyzComp_CompLastBlock(const ring r);
ring rAssure_TDeg(ring r, int start_var, int end_var, int &pos);

/// return the max-comonent wchich has syzIndex i
/// Assume: i<= syzIndex_limit
int rGetMaxSyzComp(int i, const ring r);

BOOLEAN rHasSimpleOrder(const ring r);

/// returns TRUE, if simple lp or ls ordering
BOOLEAN rHasSimpleLexOrder(const ring r);

// return TRUE if p->exp[r->pOrdIndex] holds total degree of p */
//inline BOOLEAN rHasGlobalOrdering(const ring r)
//{ return (r->OrdSgn==1); }
#define rHasGlobalOrdering(R) ((R)->OrdSgn==1)
// #define rHasGlobalOrdering_currRing() (pOrdSgn==1)
//inline BOOLEAN rHasLocalOrMixedOrdering(const ring r)
//{ return (r->OrdSgn==-1); }
#define rHasLocalOrMixedOrdering(R) ((R)->OrdSgn==-1)
// #define rHasLocalOrMixedOrdering_currRing() (pOrdSgn==-1)
BOOLEAN rOrd_is_Totaldegree_Ordering(ring r );

/// return TRUE if p_SetComp requires p_Setm
BOOLEAN rOrd_SetCompRequiresSetm(ring r);
rOrderType_t    rGetOrderType(ring r);

/// returns TRUE if var(i) belongs to p-block
BOOLEAN rIsPolyVar(int i, ring r);

static inline BOOLEAN rOrd_is_Comp_dp(ring r)
{
  assume(r != NULL);
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

BOOLEAN rSetISReference(const ring r, const ideal F, const int i = 0, const int p = 0, const intvec * componentWeights = NULL);

BOOLEAN rCheckIV(intvec *iv);
int rTypeOfMatrixOrder(intvec * order);
void rDelete(ring r);

extern omBin sip_sring_bin;
#endif
