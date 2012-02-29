#ifndef STRUCTS_H
#define STRUCTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/

/* for memset: */
#include <string.h>
/* for omBin */
#include <omalloc/omalloc.h>
#ifdef HAVE_RINGS
#include <kernel/si_gmp.h>
#endif


/* standard types */
#ifdef HAVE_RINGS
typedef unsigned long NATNUMBER;
typedef mpz_ptr int_number;
#endif
#if (SIZEOF_LONG == 8)
typedef int BOOLEAN;
/* testet on x86_64, gcc 3.4.6: 2 % */
/* testet on IA64, gcc 3.4.6: 1 % */
#else
/* testet on athlon, gcc 2.95.4: 1 % */
typedef short BOOLEAN;
#endif

typedef void * ADDRESS;
#define BITSET  unsigned int

#if defined(SI_CPU_I386) || defined(SI_CPU_X86_64)
  // the following settings seems to be better on i386 and x86_64 processors
  // define if a*b is with mod instead of tables
  #define HAVE_MULT_MOD
  // #define HAVE_GENERIC_ADD
  // #ifdef HAVE_MULT_MOD
  // #define HAVE_DIV_MOD
  // #endif
#elif defined(SI_CPU_IA64)
  // the following settings seems to be better on itanium processors
  // #define HAVE_MULT_MOD
  #define HAVE_GENERIC_ADD
  // #ifdef HAVE_MULT_MOD
  // #define HAVE_DIV_MOD
  // #endif
#elif defined(SI_CPU_SPARC)
  // #define HAVE_GENERIC_ADD
  #define HAVE_MULT_MOD
  #ifdef HAVE_MULT_MOD
  #define HAVE_DIV_MOD
  #endif
#elif defined(SI_CPU_PPC)
  // the following settings seems to be better on ppc processors
  // testet on: ppc_Linux, 740/750 PowerMac G3, 512k L2 cache
  #define HAVE_MULT_MOD
  // #ifdef HAVE_MULT_MOD
  // #define HAVE_DIV_MOD
  // #endif
#endif

#if SIZEOF_LONG == 4
typedef long long int64;
#elif SIZEOF_LONG == 8
typedef long int64;
#else
#error int64 undefined
#endif


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
#ifdef HAVE_RINGS
  ,n_Z,
  n_Zm,
  n_Zpn,
  n_Z2n
#endif
};

// #ifdef HAVE_PLURAL
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
// #endif

struct snumber;
typedef struct snumber *   number;
typedef number (*numberfunc)(number a,number b);
typedef number (*nMapFunc)(number a);

/* C++-part */
#ifdef __cplusplus
class ip_smatrix;
class intvec;
class sleftv;
class slists;
class sattr;
class skStrategy;
class ssyStrategy;
class procinfo;
class kBucket;
class sBucket;
class CPolynomialSummator;
class CGlobalMultiplier;
class CFormulaPowerMultiplier;
#endif

struct n_Procs_s;
struct sip_sring;
struct sip_link;
struct spolynom;

struct sip_package;
typedef struct sip_package ip_package;
typedef ip_package *       package;

// forward for ideals.h:
struct sip_sideal;
struct sip_smap;
typedef struct sip_smap *         map;
typedef struct sip_sideal *       ideal;


typedef struct  n_Procs_s  n_Procs_s;

// #ifdef HAVE_PLURAL
struct nc_struct;
typedef struct nc_struct   nc_struct;
// #endif

typedef struct spolyrec    polyrec;
typedef struct sip_sring   ip_sring;
typedef struct sip_link    ip_link;

/* the pointer types */
typedef char *             char_ptr;
typedef int  *             int_ptr;
typedef ip_sring *         ring;
typedef polyrec *          poly;
typedef poly *             polyset;

#ifdef __cplusplus
typedef ip_smatrix *       matrix;
typedef ip_link *          si_link;
typedef sleftv *           leftv;
typedef slists *           lists;
typedef sattr *            attr;
typedef skStrategy *       kStrategy;
typedef ssyStrategy *      syStrategy;
typedef procinfo *         procinfov;
typedef kBucket*           kBucket_pt;
typedef sBucket*           sBucket_pt;
typedef struct p_Procs_s p_Procs_s;

struct n_Procs_s
{
   n_Procs_s* next;
   // the union stuff
   // Zp:
   int npPrimeM;
   int npPminus1M;
   #ifdef HAVE_DIV_MOD
   unsigned short *npInvTable;
   #endif
   #if !defined(HAVE_DIV_MOD) || !defined(HAVE_MULT_MOD)
   unsigned short *npExpTable;
   unsigned short *npLogTable;
   #endif
   // Zp_a, Q_a

   // general stuff
   numberfunc nMult, nSub ,nAdd ,nDiv, nIntDiv, nIntMod, nExactDiv;
   number  (*cfInit)(int i,const ring r);
   number  (*nPar)(int i);
   int     (*nParDeg)(number n);
   int     (*nSize)(number n);
   int     (*n_Int)(number &n, const ring r);
#ifdef HAVE_RINGS
   int     (*nDivComp)(number a,number b);
   BOOLEAN (*nIsUnit)(number a);
   number  (*nGetUnit)(number a);
   number  (*nExtGcd)(number a, number b, number *s, number *t);
#endif
   number  (*nNeg)(number a);
   number  (*nInvers)(number a);
   number  (*nCopy)(number a);
   number  (*cfCopy)(number a, const ring r);
   number  (*nRePart)(number a);
   number  (*nImPart)(number a);
   void    (*cfWrite)(number &a, const ring r);
   const char *  (*nRead)(const char * s, number * a);
   void    (*nNormalize)(number &a);
   BOOLEAN (*nGreater)(number a,number b),
#ifdef HAVE_RINGS
           (*nDivBy)(number a, number b),
#endif
           (*nEqual)(number a,number b),
           (*nIsZero)(number a),
           (*nIsOne)(number a),
           (*nIsMOne)(number a),
           (*nGreaterZero)(number a);
   void    (*nPower)(number a, int i, number * result);
   number  (*cfGetDenom)(number &n, const ring r);
   number  (*cfGetNumerator)(number &n, const ring r);
   number  (*nGcd)(number a, number b, const ring r);
   number  (*nLcm)(number a, number b, const ring r);
   number  (*cfFarey)(number p, number n, const ring r);
   number  (*cfChineseRemainder)(number *x, number *q,int rl, const ring);
   void    (*cfDelete)(number * a, const ring r);
   nMapFunc (*cfSetMap)(const ring src, const ring dst);
   char *  (*nName)(number n);
   void    (*nInpMult)(number &a, number b, ring r);
#ifdef LDEBUG
   BOOLEAN (*nDBTest)(number a, const char *f,const int l);
#endif

   number nNULL; /* the 0 as constant */
   int     char_flag;
   int     ref;
   n_coeffType type;
   short   nChar;
};

/* the function pointer types */

typedef long     (*pLDegProc)(poly p, int *length, ring r);
typedef long     (*pFDegProc)(poly p, ring r);
typedef void     (*p_SetmProc)(poly p, const ring r);

typedef enum
{
  ro_dp, // ordering is a degree ordering
  ro_wp, // ordering is a weighted degree ordering
  ro_am, // ordering is am: weights for vars + weights for gen
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
struct sro_am
{
  short place;  // where weighted degree is stored (in L)
  short start;  // bounds of ordering (in E)
  short end;
  short len_gen; // i>len_gen: weight(gen(i)):=0
  int *weights; // pointers into wvhdl field of length (end-start+1) + len_gen
                // contents w_1,... w_n, len, mod_w_1, .. mod_w_len, 0
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
// NC pProcs:
typedef poly (*mm_Mult_p_Proc_Ptr)(const poly m, poly p, const ring r);
typedef poly (*mm_Mult_pp_Proc_Ptr)(const poly m, const poly p, const ring r);

typedef ideal (*GB_Proc_Ptr)(const ideal F, const ideal Q, const intvec *w, const intvec *hilb, kStrategy strat);

typedef poly (*SPoly_Proc_Ptr)(const poly p1, const poly p2, const ring r);
typedef poly (*SPolyReduce_Proc_Ptr)(const poly p1, poly p2, const ring r);

typedef void (*bucket_Proc_Ptr)(kBucket_pt b, poly p, number *c);

struct nc_pProcs
{
public:
  mm_Mult_p_Proc_Ptr                    mm_Mult_p;
  mm_Mult_pp_Proc_Ptr                   mm_Mult_pp;

  bucket_Proc_Ptr                       BucketPolyRed;
  bucket_Proc_Ptr                       BucketPolyRed_Z;

  SPoly_Proc_Ptr                        SPoly;
  SPolyReduce_Proc_Ptr                  ReduceSPoly;

  GB_Proc_Ptr                           GB;
//                                         GlobalGB, // BBA
//                                         LocalGB;  // MORA
};


struct nc_struct
{
  nc_type type;
  //ring basering; // the ring C,D,.. live in (commutative ring with this NC structure!)

  // initial data: square matrices rVar() x rVar()
  // logically: upper triangular!!!
  // TODO: eliminate this waste of memory!!!!
  matrix C;
  matrix D;

  // computed data:
  matrix *MT; // size 0.. (rVar()*rVar()-1)/2
  matrix COM;
  int *MTsize; // size 0.. (rVar()*rVar()-1)/2

  // IsSkewConstant indicates whethere coeffs C_ij are all equal,
  // effective together with nc_type=nc_skew
  int IsSkewConstant;

  private:
    // internal data for different implementations
    // if dynamic => must be deallocated in destructor (nc_rKill!)
    union
    {
      struct
      {
        // treat variables from iAltVarsStart till iAltVarsEnd as alternating vars.
        // these variables should have odd degree, though that will not be checked
        // iAltVarsStart, iAltVarsEnd are only used together with nc_type=nc_exterior
        // 1 <= iAltVarsStart <= iAltVarsEnd <= r->N
        unsigned int iFirstAltVar, iLastAltVar; // = 0 by default

        // for factors of super-commutative algebras we need
        // the part of general quotient ideal modulo squares!
        ideal idSCAQuotient; // = NULL by default. // must be deleted in Kill!
      } sca;
    } data;

    CGlobalMultiplier* m_Multiplier;
    CFormulaPowerMultiplier* m_PowerMultiplier;

  public:

    inline nc_type& ncRingType() { return (type); };
    inline nc_type ncRingType() const { return (type); };

    inline unsigned int& FirstAltVar()
        { assume(ncRingType() == nc_exterior); return (data.sca.iFirstAltVar); };
    inline unsigned int& LastAltVar ()
        { assume(ncRingType() == nc_exterior); return (data.sca.iLastAltVar ); };

    inline unsigned int FirstAltVar() const
        { assume(ncRingType() == nc_exterior); return (data.sca.iFirstAltVar); };
    inline unsigned int LastAltVar () const
        { assume(ncRingType() == nc_exterior); return (data.sca.iLastAltVar ); };

    inline ideal& SCAQuotient()
        { assume(ncRingType() == nc_exterior); return (data.sca.idSCAQuotient); };

    inline CGlobalMultiplier* GetGlobalMultiplier() const
        { return (m_Multiplier); };

    inline CGlobalMultiplier*& GetGlobalMultiplier()
        { return (m_Multiplier); };


    inline CFormulaPowerMultiplier* GetFormulaPowerMultiplier() const
        { return (m_PowerMultiplier); };

    inline CFormulaPowerMultiplier*& GetFormulaPowerMultiplier()
        { return (m_PowerMultiplier); };

  public:
    nc_pProcs p_Procs; // NC procedures.

};
#endif

class idrec;
typedef idrec *   idhdl;

struct sip_sring
{
// each entry must have a description and a procedure defining it,
// general ordering: pointer/structs, long, int, short, BOOLEAN/char/enum
// general defining procedures: rInit, rComplete, interpreter, ??
  idhdl      idroot; /* local objects , interpreter*/
  int*       order;  /* array of orderings, rInit/rSleftvOrdering2Ordering */
  int*       block0; /* starting pos., rInit/rSleftvOrdering2Ordering*/
  int*       block1; /* ending pos., rInit/rSleftvOrdering2Ordering*/
  char**     parameter; /* names of parameters, rInit */
  number     minpoly;  /* for Q_a/Zp_a, rInit */
  ideal      minideal;
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
#ifdef HAVE_RINGS
  unsigned int  ringtype;  /* cring = 0 => coefficient field, cring = 1 => coeffs from Z/2^m */
  int_number    ringflaga; /* Z/(ringflag^ringflagb)=Z/nrnModul*/
  unsigned long ringflagb;
  unsigned long nr2mModul;  /* Z/nr2mModul */
  int_number    nrnModul;
#endif
  unsigned long options; /* ring dependent options */

  int        ch;  /* characteristic, rInit */
  int        ref; /* reference counter to the ring, interpreter */

  short      float_len; /* additional char-flags, rInit */
  short      float_len2; /* additional char-flags, rInit */

  short      N;      /* number of vars, rInit */

  short      P;      /* number of pars, rInit */
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
  ring          algring;
#ifdef HAVE_PLURAL
  private:
    nc_struct*    _nc; // private
  public:
    inline const nc_struct* GetNC() const { return _nc; }; // public!!!
    inline nc_struct*& GetNC() { return _nc; }; // public!!!
#endif
};

#endif /* __cplusplus */



/*
**  7. runtime procedures/global data
*/

/* 7.1 C-routines : */


#ifdef __cplusplus
extern "C" {
#endif
void  m2_end(int i) __attribute__((noreturn));
#ifdef __cplusplus
}
#endif

/* 7.2 C++-routines : */

#ifdef __cplusplus
int   inits(void);
int   IsPrime(int i);
extern int siSeed;
int siRand();
#endif

#define loop for(;;)

#ifndef ABS
#define ABS(x) ((x)<0?(-(x)):(x))
#endif

#if defined(__cplusplus)
static inline int si_max(const int a, const int b)  { return (a>b) ? a : b; }
static inline int si_min(const int a, const int b)  { return (a<b) ? a : b; }
static inline long si_max(const long a, const long b)  { return (a>b) ? a : b; }
static inline unsigned long si_max(const unsigned long a, const unsigned long b)  { return (a>b) ? a : b; }
static inline long si_min(const long a, const long b)  { return (a<b) ? a : b; }
static inline unsigned long si_min(const unsigned long a, const unsigned long b)  { return (a<b) ? a : b; }
#else
#define si_max(A,B) ((A) > (B) ? (A) : (B))
#define si_min(A,B) ((A) < (B) ? (A) : (B))
#endif

extern omBin char_ptr_bin;
extern omBin sleftv_bin;

#endif

