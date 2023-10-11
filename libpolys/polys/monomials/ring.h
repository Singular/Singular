#ifndef RING_H
#define RING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - the interpreter related ring operations
*/

/* includes */
#include "misc/auxiliary.h"
#include "coeffs/coeffs.h"
#include "misc/intvec.h"
#include "misc/int64vec.h"
#include "coeffs/coeffs.h" // ring,number
#include "polys/monomials/monomials.h"
//#include "polys/monomials/polys-impl.h"
//

/* forward declaration of types */
class idrec; typedef idrec *   idhdl; // _only_ for idhdl ip_sring::idroot
struct p_Procs_s;
typedef struct p_Procs_s p_Procs_s;
class kBucket;
typedef kBucket*           kBucket_pt;

struct sip_sideal;
typedef struct sip_sideal *       ideal;
typedef struct sip_sideal const * const_ideal;

struct sip_smap;
typedef struct sip_smap *         map;
typedef struct sip_smap const *   const_map;

/* the function pointer types */

typedef long     (*pLDegProc)(poly p, int *length, ring r);
typedef long     (*pFDegProc)(poly p, ring r);
typedef void     (*p_SetmProc)(poly p, const ring r);


/// returns a poly from dest_r which is a ShallowCopy of s_p from source_r
/// assumes that source_r->N == dest_r->N and that orderings are the same
typedef poly (*pShallowCopyDeleteProc)(poly s_p, ring source_r, ring dest_r,
                                       omBin dest_bin);

// ro_typ describes what to store at the corresping "data" place in p->exp
// none of the directly corresponds to a ring ordering (ringorder_*)
// as each ringorder_* blocks corrsponds to 0..2 sro-blocks
typedef enum
{
  ro_dp, // total degree with weights 1
  ro_wp, // total weighted degree with weights>0 in wvhdl
  ro_am, // weights for vars + weights for gen
  ro_wp64, // weighted64 degree weights in wvhdl
  ro_wp_neg, // total weighted degree with weights in Z in wvhdl
             // (with possibly negative weights)
  ro_cp,    // ??ordering duplicates variables
  ro_syzcomp, // ??ordering indicates "subset" of component number (ringorder_S)
  ro_syz, // component number if <=syzcomp else 0 (ringorder_s)
  ro_isTemp, ro_is, // ??Induced Syzygy (Schreyer) ordering (and prefix data placeholder dummy) (ringorder_IS)
  ro_none
}
ro_typ;

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
struct sro_am
{
  short place;  // where weighted degree is stored (in L)
  short start;  // bounds of ordering (in E)
  short end;
  short len_gen; // i>len_gen: weight(gen(i)):=0
  int *weights; // pointers into wvhdl field of length (end-start+1) + len_gen + 1
                // contents w_{start},... w_{end}, len, mod_w_1, .. mod_w_len, 0
  int *weights_m; // pointers into wvhdl field of length len_gen + 1
                // len_gen, mod_w_1, .. mod_w_len, 0

};
typedef struct sro_am sro_am;

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
     sro_am am;
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
class skStrategy;
typedef skStrategy * kStrategy;

typedef poly (*NF_Proc)(ideal, ideal, poly, int, int, const ring _currRing);
typedef ideal (*BBA_Proc) (const ideal, const ideal, const intvec *, const intvec *, kStrategy strat, const ring);


struct ip_sring
{
// each entry must have a description and a procedure defining it,
// general ordering: pointer/structs, long, int, short, BOOLEAN/char/enum
// general defining procedures: rInit, rComplete, interpreter, ??
  idhdl      idroot; /* local objects , interpreter*/
  rRingOrder_t* order;  /* array of orderings, rInit/rSleftvOrdering2Ordering */
  int*       block0; /* starting pos., rInit/rSleftvOrdering2Ordering*/
  int*       block1; /* ending pos., rInit/rSleftvOrdering2Ordering*/
//  char**     parameter; /* names of parameters, rInit */
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

//  ideal      minideal;
//  number     minpoly;  /* replaced by minideal->m[0] */
  ideal      qideal; /**< extension to the ring structure: qring, rInit, OR
                          for Q_a/Zp_a, rInit (replaces minideal!);
                          for a start, we assume that there is either no
                          or exactly one generator in minideal, playing
                          the role of the former minpoly; minideal may
                          also be NULL which coincides with the
                          no-generator-case **/

  int*     firstwv;

  omBin    PolyBin; /* Bin from where monoms are allocated */
  intvec * pModW;   /* std: module weights */
  poly     ppNoether; /*  variables, set by procedures from hecke/kstd1:
                            the highest monomial below pHEdge */
  void * ext_ref;   /* libsing GAP object */
// #ifdef HAVE_RINGS
//   unsigned int  cf->ringtype;  /* cring = 0 => coefficient field, cring = 1 => coeffs from Z/2^m */
//   mpz_ptr    cf->modBase; /* Z/(ringflag^cf->modExponent)=Z/cf->modNumber*/
//   unsigned long cf->modExponent;
//   unsigned long cf->modNumber;  /* Z/cf->modNumber */
//   mpz_ptr    cf->modNumber;
// #endif

  unsigned long options; /* ring dependent options */

//  int        ch;  /* characteristic, rInit */
  int        ref; /* reference counter to the ring, interpreter */

  short      N;      /* number of vars, rInit */

  short      OrdSgn; /* 1 for polynomial rings, -1 otherwise, rInit */

  short     firstBlockEnds;
#ifdef HAVE_PLURAL
  short     real_var_start, real_var_end;
#endif

#ifdef HAVE_SHIFTBBA
  short          isLPring; /* 0 for non-letterplace rings, otherwise the number of LP blocks, at least 1, known also as lV */
  short          LPncGenCount;
#endif

  BOOLEAN   VectorOut;
  BOOLEAN   ShortOut;
  BOOLEAN   CanShortOut;
  BOOLEAN   LexOrder; // TRUE if the monomial ordering has polynomial and power series blocks
  BOOLEAN   MixedOrder; // TRUE for global/local mixed orderings, FALSE otherwise
  BOOLEAN   pLexOrder; /* TRUE if the monomial ordering is not compatible with pFDeg */

  BOOLEAN   ComponentOrder; // 1 if ringorder_c, -1 for ringorder_C,ringorder_S,ringorder_s

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

  short     NegWeightL_Size;
  /* array of size VarL_Size,
     VarL_Offset[i] gets i-th long var in exp vector */
  int*      VarL_Offset;

  /* mask for getting single exponents, also maxExp */
  unsigned long bitmask;
  /* wanted maxExp */
  unsigned long wanted_maxExp;
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

////////// DEPRECATED
/////// void   rChangeCurrRing(ring r);

ring   rDefault(int ch, int N, char **n);
ring   rDefault(const coeffs cf, int N, char **n, const rRingOrder_t o=ringorder_lp);
ring   rDefault(int ch, int N, char **n,int ord_size, rRingOrder_t *ord, int *block0, int *block1, int **wvhdl=NULL);
ring   rDefault(const coeffs cf, int N, char **n,int ord_size, rRingOrder_t *ord, int *block0, int *block1, int **wvhdl=NULL, unsigned long bitmask=0);
unsigned long rGetExpSize(unsigned long bitmask, int & bits, int N);

// #define rIsRingVar(A) r_IsRingVar(A,currRing)
int    r_IsRingVar(const char *n, char**names, int N);
void   rWrite(ring r, BOOLEAN details = FALSE);
ring   rCopy(ring r);
ring   rCopy0(const ring r, BOOLEAN copy_qideal = TRUE, BOOLEAN copy_ordering = TRUE);
ring rCopy0AndAddA(ring r, int64vec *wv64, BOOLEAN copy_qideal = TRUE,
                   BOOLEAN copy_ordering = TRUE);
ring   rOpposite(ring r);
ring   rEnvelope(ring r);

/// we must always have this test!
static inline BOOLEAN rIsPluralRing(const ring r)
{
  assume(r != NULL);
#ifdef HAVE_PLURAL
  nc_struct *n;
  return ((n=r->GetNC()) != NULL) /*&& (n->type != nc_error)*/;
#else
  return FALSE;
#endif
}

static inline BOOLEAN rIsLPRing(const ring r)
{
  assume(r != NULL);
#ifdef HAVE_SHIFTBBA
  return (r->isLPring!=0);
#else
  return FALSE;
#endif
}

static inline BOOLEAN rIsNCRing(const ring r)
{
  assume(r != NULL);
  return rIsPluralRing(r) || rIsLPRing(r);
}

static inline BOOLEAN rIsRatGRing(const ring r)
{
  assume(r != NULL);
#ifdef HAVE_PLURAL
  /* nc_struct *n; */
  return (r != NULL) /* && ((n=r->GetNC()) != NULL) */
          && (r->real_var_start>1);
#else
  return FALSE;
#endif
}

// The following are for LaScala3 only!
void rChangeSComps(int* currComponents, long* currShiftedComponents, int length, ring r);
void rGetSComps(int** currComponents, long** currShiftedComponents, int *length, ring r);



const char * rSimpleOrdStr(int ord);
rRingOrder_t rOrderName(char * ordername);
char * rOrdStr(ring r);
char * rVarStr(ring r);
char * rCharStr(ring r);
char * rString(ring r);
int    rChar(ring r);

char * rParStr(ring r);

int    rSum(ring r1, ring r2, ring &sum);
/// returns -1 for not compatible, 1 for compatible (and sum)
/// dp_dp:0: block ordering, 1: dp,dp, 2: aa(...),dp
/// vartest: check for name conflicts
int rSumInternal(ring r1, ring r2, ring &sum, BOOLEAN vartest, BOOLEAN dp_dp);

/// returns TRUE, if r1 equals r2 FALSE, otherwise Equality is
/// determined componentwise, if qr == 1, then qrideal equality is
/// tested, as well
BOOLEAN rEqual(ring r1, ring r2, BOOLEAN qr = TRUE);

/// returns TRUE, if r1 and r2 represents the monomials in the same way
/// FALSE, otherwise
/// this is an analogue to rEqual but not so strict
BOOLEAN rSamePolyRep(ring r1, ring r2);

void   rUnComplete(ring r);

BOOLEAN rRing_has_CompLastBlock(const ring r);
BOOLEAN rRing_ord_pure_dp(const ring r);
BOOLEAN rRing_ord_pure_Dp(const ring r);
BOOLEAN rRing_ord_pure_lp(const ring r);

#ifdef HAVE_RINGS
static inline BOOLEAN rField_is_Ring_2toM(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return ( nCoeff_is_Ring_2toM(r->cf) ); }

static inline BOOLEAN rField_is_Ring_PtoM(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return ( nCoeff_is_Ring_PtoM(r->cf) ); }

#define rField_is_Ring(R) nCoeff_is_Ring((R)->cf)

static inline BOOLEAN rField_is_Domain(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_is_Domain(r->cf); }

static inline BOOLEAN rField_has_Units(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_has_Units(r->cf); }
#else
#define rField_is_Ring(A) (0)
#define rField_is_Ring_2toM(A) (0)
#define rField_is_Ring_PtoM(A) (0)
#define rField_is_Domain(A) (1)
#define rField_has_Units(A) (1)
#endif

static inline BOOLEAN rField_is_Zp(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return (getCoeffType(r->cf) == n_Zp); }

static inline BOOLEAN rField_is_Zp(const ring r, int p)
{ assume(r != NULL); assume(r->cf != NULL); return (getCoeffType(r->cf) == n_Zp) && (r->cf->ch == p); }

static inline BOOLEAN rField_is_Q(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_is_Q(r->cf); }

static inline BOOLEAN rField_is_Z(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_is_Z(r->cf); }

static inline BOOLEAN rField_is_Zn(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_is_Zn(r->cf); }

static inline BOOLEAN rField_is_numeric(const ring r) /* R, long R, long C */
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_is_numeric(r->cf); }

static inline BOOLEAN rField_is_R(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_is_R(r->cf); }

static inline BOOLEAN rField_is_GF(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_is_GF(r->cf); }

static inline BOOLEAN rField_is_GF(const ring r, int q)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_is_GF(r->cf, q); }

/* DO NOT USE; just here for compatibility reasons towards
   the SINGULAR svn trunk */
static inline BOOLEAN rField_is_Zp_a(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_is_Zp_a(r->cf); }

/* DO NOT USE; just here for compatibility reasons towards
   the SINGULAR svn trunk */
static inline BOOLEAN rField_is_Zp_a(const ring r, int p)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_is_Zp_a(r->cf, p); }

/* DO NOT USE; just here for compatibility reasons towards
   the SINGULAR svn trunk */
static inline BOOLEAN rField_is_Q_a(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_is_Q_a(r->cf); }

static inline BOOLEAN rField_is_long_R(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_is_long_R(r->cf); }

static inline BOOLEAN rField_is_long_C(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_is_long_C(r->cf); }

static inline BOOLEAN rField_has_simple_inverse(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_has_simple_inverse(r->cf); }

/// Z/p, GF(p,n), R: nCopy, nNew, nDelete are dummies
static inline BOOLEAN rField_has_simple_Alloc(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return nCoeff_has_simple_Alloc(r->cf); }

/// the type of the coefficient filed of r (n_Zp, n_Q, etc)
static inline n_coeffType rFieldType(const ring r) { return (r->cf->type); }

/// this needs to be called whenever a new ring is created: new fields
/// in ring are created (like VarOffset), unless they already exist
/// with force == 1, new fields are _always_ created (overwritten),
/// even if they exist
BOOLEAN rComplete(ring r, int force = 0);
// use this to free fields created by rComplete //?

/// set all properties of a new ring - also called by rComplete
void p_SetGlobals(const ring r, BOOLEAN complete = TRUE);

static inline int rBlocks(const ring r)
{
  assume(r != NULL);
  int i=0;
  while (r->order[i]!=0) i++;
  return i+1;
}

// misc things
static inline char* rRingVar(short i, const ring r)
{
  assume(r != NULL); assume(r->cf != NULL); return r->names[i];
}
static inline BOOLEAN rShortOut(const ring r)
{
  assume(r != NULL); return (r->ShortOut);
}

static inline BOOLEAN rCanShortOut(const ring r)
{
  assume(r != NULL); return (r->CanShortOut);
}

/// #define rVar(r) (r->N)
static inline short rVar(const ring r)
{
  assume(r != NULL);
  return r->N;
}

/// (r->cf->P)
static inline int rPar(const ring r)
{
  assume(r != NULL);
  const coeffs C = r->cf;
  assume(C != NULL);

  return n_NumberOfParameters(C);
//   if( nCoeff_is_Extension(C) )
//   {
//     const ring R = C->extRing;
//     assume( R != NULL );
//     return rVar( R );
//   }
//   else if (nCoeff_is_GF(C))
//   {
//     return 1;
//   }
//   else if (nCoeff_is_long_C(C))
//   {
//     return 1;
//   }
//   return 0;
}


/// (r->cf->parameter)
static inline char const ** rParameter(const ring r)
{
  assume(r != NULL);
  const coeffs C = r->cf;
  assume(C != NULL);

  return n_ParameterNames(C);
//   if( nCoeff_is_Extension(C) ) // only alg / trans. exts...
//   {
//     const ring R = C->extRing;
//     assume( R != NULL );
//     return R->names;
//   }
//   else if (nCoeff_is_GF(C))
//   {
//     return &(C->m_nfParameter);
//   }
//   else if (nCoeff_is_long_C(C))
//   {
//     return &(C->complex_parameter);
//   }
//   return NULL;
}

/// return the specified parameter as a (new!) number in the given
/// polynomial ring, or NULL if invalid
/// parameters (as variables) begin with 1!
static inline number n_Param(const short iParameter, const ring r)
{
  assume(r != NULL);
  const coeffs C = r->cf;
  assume(C != NULL);
  return n_Param(iParameter, C);
//   const n_coeffType _filed_type = getCoeffType(C);
//
//   if ( iParameter <= 0 || iParameter > rPar(r) )
//     // Wrong parameter
//     return NULL;
//
//   if( _filed_type == n_algExt )
//     return naParameter(iParameter, C);
//
//   if( _filed_type == n_transExt )
//     return ntParameter(iParameter, C);
//
//   if (_filed_type == n_GF)// if (nCoeff_is_GF(C))
//   {
//     number nfPar (int i, const coeffs);
//     return nfPar(iParameter, C);
//   }
//
//   if (_filed_type == n_long_C) // if (nCoeff_is_long_C(C))
//   {
//     number   ngcPar(int i, const coeffs r);
//     return ngcPar(iParameter, C);
//   }
//
//   return NULL;
}

/// if m == var(i)/1 => return i,
int n_IsParam(number m, const ring r);

//#define  rInternalChar(r) ((r)->cf->ch)
static inline int rInternalChar(const ring r)
{
  assume(r != NULL);
  const coeffs C = r->cf;
  assume(C != NULL);
  return C->ch;
}


/// Tests whether '(r->cf->minpoly) == NULL'
static inline BOOLEAN rMinpolyIsNULL(const ring r)
{
  assume(r != NULL);
  const coeffs C = r->cf;
  assume(C != NULL);

  const BOOLEAN ret = nCoeff_is_algExt(C); //  || nCoeff_is_GF(C) || nCoeff_is_long_C(C);

  if( ret )
  {
    assume( (C->extRing) != NULL );
    BOOLEAN idIs0 (ideal h);
    assume((!((C->extRing)->qideal==NULL)) && (!idIs0((C->extRing)->qideal)));
  }

  // TODO: this leads to test fails (due to rDecompose?)
  return !ret;
}



static inline BOOLEAN rIsSyzIndexRing(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return r->order[0] == ringorder_s;}

static inline int rGetCurrSyzLimit(const ring r)
{ assume(r != NULL); assume(r->cf != NULL); return (rIsSyzIndexRing(r)? r->typ[0].data.syz.limit : 0);}

void   rSetSyzComp(int k, const ring r);

// Ring Manipulations
ring   rAssure_HasComp(const ring r);
ring   rAssure_SyzOrder(const ring r, BOOLEAN complete);
ring   rAssure_SyzComp(const ring r, BOOLEAN complete = TRUE);
ring   rAssure_InducedSchreyerOrdering(const ring r, BOOLEAN complete = TRUE, int sgn = 1);

ring   rAssure_dp_S(const ring r);
ring   rAssure_dp_C(const ring r);
ring   rAssure_Dp_C(const ring r);
ring   rAssure_Wp_C(const ring r, intvec *w);
ring   rAssure_C_dp(const ring r);
ring   rAssure_c_dp(const ring r);

/// makes sure that c/C ordering is last ordering
ring   rAssure_CompLastBlock(const ring r, BOOLEAN complete = TRUE);

/// makes sure that c/C ordering is last ordering and SyzIndex is first
ring   rAssure_SyzComp_CompLastBlock(const ring r);
ring   rAssure_TDeg(const ring r, int &pos);

/// return the max-comonent wchich has syzIndex i
/// Assume: i<= syzIndex_limit
int rGetMaxSyzComp(int i, const ring r);

BOOLEAN rHasSimpleOrder(const ring r);
BOOLEAN rHas_c_Ordering(const ring r);

/// returns TRUE, if simple lp or ls ordering
BOOLEAN rHasSimpleLexOrder(const ring r);

//???? return TRUE if p->exp[r->pOrdIndex] holds total degree of p ???


inline BOOLEAN rHasGlobalOrdering(const ring r){ return (r->OrdSgn==1); }
inline BOOLEAN rHasLocalOrMixedOrdering(const ring r){ return (r->OrdSgn==-1); }
inline BOOLEAN rHasMixedOrdering(const ring r) { return (r->MixedOrder); }

// #define rHasGlobalOrdering(R) ((R)->OrdSgn==1)
// #define rHasLocalOrMixedOrdering(R) ((R)->OrdSgn==-1)

BOOLEAN rOrd_is_Totaldegree_Ordering(const ring r);
BOOLEAN rOrd_is_dp(const ring r);
BOOLEAN rOrd_is_ds(const ring r);

/// return TRUE if p_SetComp requires p_Setm
BOOLEAN rOrd_SetCompRequiresSetm(const ring r);
rOrderType_t    rGetOrderType(ring r);

static inline BOOLEAN rOrd_is_Comp_dp(const ring r)
{
  assume(r != NULL);
  assume(r->cf != NULL);
  return ((r->order[0] == ringorder_c || r->order[0] == ringorder_C) &&
          r->order[1] == ringorder_dp &&
          r->order[2] == 0);
}

#ifdef RDEBUG
#define rTest(r)  rDBTest(r, __FILE__, __LINE__)
extern BOOLEAN rDBTest(ring r, const char* fn, const int l);
#else
#define rTest(r) (TRUE)
#endif

ring rModifyRing(ring r, BOOLEAN omit_degree,
                         BOOLEAN omit_comp,
                         unsigned long exp_limit);

/// construct Wp, C ring
ring rModifyRing_Wp(ring r, int* weights);

void rKillModifiedRing(ring r);
// also frees weights
void rKillModified_Wp_Ring(ring r);

ring rModifyRing_Simple(ring r, BOOLEAN omit_degree, BOOLEAN omit_comp, unsigned long exp_limit, BOOLEAN &simple);

#ifdef RDEBUG
void rDebugPrint(const ring r);
// void pDebugPrint(poly p);
void p_DebugPrint(poly p, const ring r);
#endif

int64 * rGetWeightVec(const ring r);
void rSetWeightVec(ring r, int64 *wv);

/////////////////////////////
// Auxillary functions
//

/* return the varIndex-th ring variable as a poly;
   varIndex starts at index 1 */
poly rGetVar(const int varIndex, const ring r);

BOOLEAN rSetISReference(const ring r, const ideal F, const int i = 0, const int p = 0);

/// return the position of the p^th IS block order block in r->typ[]...
int rGetISPos(const int p, const ring r);

BOOLEAN rCheckIV(const intvec *iv);
int rTypeOfMatrixOrder(const intvec *order);

void rDelete(ring r); // To be used instead of rKill!

EXTERN_VAR omBin sip_sring_bin;

// ring manipulation
/// K[x],"y" -> K[x,y] resp. K[y,x]
ring rPlusVar(const ring r, char *v,int left);

/// undo rPlusVar
ring rMinusVar(const ring r, char *v);

static inline ring rIncRefCnt(ring r) { r->ref++; return r; }
static inline void rDecRefCnt(ring r) { r->ref--; }
#endif
