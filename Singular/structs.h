#ifndef STRUCTS_H
#define STRUCTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: structs.h,v 1.65 2002-06-10 15:25:35 Singular Exp $ */
/*
* ABSTRACT
*/
#define LONGALGNEW
/* standard types */
typedef unsigned char  uchar ;
typedef unsigned short CARDINAL;
typedef short BOOLEAN;

typedef void * Sy_reference;
#define ADDRESS Sy_reference
#define BITSET  unsigned int

/* EXPONENT_TYPE is determined by configure und defined in mod2.h */
/* the following defines should really go into mod2.h,
   but configure dislikes it */


// define if a*b is with mod instead of tables
#if defined(i386)
// seems to be better on i386 processors
#define HAVE_MULT_MOD
#ifdef HAVE_MULT_MOD
/* #define HAVE_DIV_MOD*/
#endif
#endif

typedef long Exponent_t;
typedef long Order_t;

enum tHomog
{
   isNotHomog = FALSE,
   isHomog    = TRUE,
   testHomog
};
enum noeof_t
{
  noeof_brace = 1,
  noeof_asstring,
  noeof_block,
  noeof_bracket,
  noeof_comment,
  noeof_procname,
  noeof_string
};

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

#ifdef HAVE_PLURAL
enum nc_type
{
  nc_general=0, /* yx=q xy+... */
  nc_skew, /*1*/ /* yx=q xy */
  nc_lie,  /*2*/ /* yx=xy+... */
  nc_undef /*3*/  /* for internal reasons */
};
#endif


/* C++-part */
#ifdef __cplusplus
class ip_smatrix;
class idrec;
class intvec;
class sleftv;
class slists;
class sattr;
class skStrategy;
class ssyStrategy;
class procinfo;
class namerec;
class kBucket;
class sBucket;
#endif

struct n_Procs_s;
struct sip_sring;
struct sip_sideal;
struct sip_link;
struct spolynom;
struct _ssubexpr;
struct _sssym;
struct snumber;
struct sip_command;
struct sip_package;
struct s_si_link_extension;
#ifndef LONGALGNEW
struct reca;
#endif /* not LONGALGNEW */

typedef struct  n_Procs_s  n_Procs_s;

#ifdef HAVE_PLURAL
struct nc_struct;
typedef struct nc_struct   nc_struct;
#endif

typedef struct _ssubexpr   sSubexpr;
typedef struct _sssym      ssym;
typedef struct spolyrec    polyrec;
typedef struct sip_sideal  ip_sideal;
typedef struct sip_smap    ip_smap;
typedef struct sip_sring   ip_sring;
typedef struct sip_link    ip_link;
typedef struct sip_command ip_command;
typedef struct sip_package ip_package;

/* the pointer types */
typedef char *             char_ptr;
typedef int  *             int_ptr;
typedef short *            short_ptr;
typedef void *             void_ptr;
typedef ip_sring *         ring;
typedef int                idtyp;
typedef struct snumber *   number;
typedef polyrec *          poly;
typedef poly *             polyset;
typedef ip_sideal *        ideal;
typedef ip_smap *          map;
typedef ideal *            resolvente;
typedef union uutypes      utypes;
typedef ip_command *       command;
typedef struct s_si_link_extension *si_link_extension;
#ifndef LONGALGNEW
typedef struct reca *      napoly;
#else /* LONGALGNEW */
typedef polyrec *   napoly;
#endif /* LONGALGNEW */

#ifdef __cplusplus
typedef idrec *            idhdl;
typedef ip_smatrix *       matrix;
typedef ip_link *          si_link;
typedef sleftv *           leftv;
typedef slists *           lists;
typedef sSubexpr *         Subexpr;
typedef sattr *            attr;
typedef skStrategy *       kStrategy;
typedef ip_package *       package;
typedef ssyStrategy *      syStrategy;
typedef procinfo *         procinfov;
typedef namerec *          namehdl;
typedef kBucket*           kBucket_pt;
typedef sBucket*           sBucket_pt;
typedef struct p_Procs_s p_Procs_s;

// for hdegree.cc
typedef struct sindlist indlist;
typedef indlist * indset;
struct sindlist
{
  indset nx;
  intvec * set;
};

// for longalg.cc
struct snaIdeal
{
  int anz;
  napoly *liste;
};
typedef struct snaIdeal * naIdeal;


// for sparsemat.cc
typedef struct smprec sm_prec;
typedef sm_prec * smpoly;
struct smprec
{
  smpoly n;            // the next element
  int pos;             // position
  int e;               // level
  poly m;              // the element
  float f;             // complexity of the element
};

struct _scmdnames
{
  char *name;
  short alias;
  short tokval;
  short toktype;
};
typedef struct _scmdnames cmdnames;

typedef number (*numberfunc)(number a,number b);
typedef number (*nMapFunc)(number a);
struct n_Procs_s
{
   n_Procs_s* next;
   int     nChar;
   n_coeffType type;
   int     char_flag;
   int     ref;
   // the union stuff
   // Zp:
   int npPrimeM;
   int npPminus1M;
   #ifdef HAVE_DIV_MOD
   CARDINAL *npInvTable;
   #else
   CARDINAL *npExpTable;
   CARDINAL *npLogTable;
   #endif
   // Zp_a, Q_a

   // general stuff
   numberfunc nMult, nSub ,nAdd ,nDiv, nIntDiv, nIntMod, nExactDiv;
   void    (*nNew)(number * a);
   number  (*nInit)(int i);
   number  (*nPar)(int i);
   int     (*nParDeg)(number n);
   int     (*nSize)(number n);
   int     (*nInt)(number &n);
   number  (*nNeg)(number a);
   number  (*nInvers)(number a);
   number  (*nCopy)(number a);
   number  (*cfCopy)(number a, const ring r);
   number  (*nRePart)(number a);
   number  (*nImPart)(number a);
   void    (*nWrite)(number &a);
   char *  (*nRead)(char * s, number * a);
   void    (*nNormalize)(number &a);
   BOOLEAN (*nGreater)(number a,number b),
           (*nEqual)(number a,number b),
           (*nIsZero)(number a),
           (*nIsOne)(number a),
           (*nIsMOne)(number a),
           (*nGreaterZero)(number a);
   void    (*nPower)(number a, int i, number * result);
   number  (*nGetDenom)(number &n);
   number  (*nGcd)(number a, number b, const ring r);
   number  (*nLcm)(number a, number b, const ring r);
   void    (*cfDelete)(number * a, const ring r);
   nMapFunc (*cfSetMap)(ring src, ring dst);
   char *  (*nName)(number n);
//extern number  (*nMap)(number from);

   number nNULL; /* the 0 as constant */
};

/* the function pointer types */

extern ring      currRing;
typedef long     (*pLDegProc)(poly p, int *length, ring r= currRing);
typedef long     (*pFDegProc)(poly p, ring r = currRing);
typedef void     (*p_SetmProc)(poly p, ring r = currRing);

typedef enum
{
  ro_dp, // ordering is a degree ordering
  ro_wp, // ordering is a weighted degree ordering
  ro_wp_neg, // ordering is a weighted degree ordering
             // with possibly negative weights
  ro_cp,    // ordering duplicates variables
  ro_syzcomp, // ordering indicates "subset" of component number
  ro_syz, // ordering  with component number >syzcomp is lower
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

#ifndef OM_ALLOC_H
struct omBin_s;
#endif

struct sro_ord
{
  ro_typ  ord_typ;
  int     order_index; // comes from r->order[order_index]
  union
  {
     sro_dp dp;
     sro_wp wp;
     sro_cp cp;
     sro_syzcomp syzcomp;
     sro_syz syz;
  } data;
};

#ifdef HAVE_PLURAL
struct nc_struct
{
  nc_type type;
  matrix C;
  matrix D;
  matrix *MT;
  matrix COM;
  int *MTsize;
};
#endif

struct sip_sring
{
  idhdl      idroot; /* local objects */
  int*       order;  /* array of orderings */
  int*       block0; /* starting pos.*/
  int*       block1; /* ending pos.*/
  char**     parameter; /* names of parameters */
  number     minpoly;
  int**      wvhdl;  /* array of weight vectors */
  char **    names;  /* array of variable names */

  unsigned long options; /* ring dependent options */

  // what follows below here should be set by rComplete, _only_
  long      *ordsgn;  /* array of +/- 1 (or 0) for comparing monomials */
                       /*  ExpL_Size entries*/

  // is NULL for lp or N == 1, otherwise non-NULL (with OrdSize > 0 entries) */
  sro_ord*   typ;   /* array of orderings + sizes, OrdSize entries */

  ideal      qideal; /* extension to the ring structure: qring */


  int      *VarOffset;
  int*     firstwv;

  struct omBin_s*   PolyBin; /* Bin from where monoms are allocated */
  short      ch;     /* characteristic */
  short      float_len; /* additional char-flags */
  short      float_len2; /* additional char-flags */

  short      N;      /* number of vars */

  short      P;      /* number of pars */
  short      OrdSgn; /* 1 for polynomial rings, -1 otherwise */

  short     firstBlockEnds;


  BOOLEAN   VectorOut;
  BOOLEAN   ShortOut;
  BOOLEAN   CanShortOut;
  BOOLEAN   LexOrder;
  // TRUE if the monomial ordering has polynomial and power series blocks
  BOOLEAN   MixedOrder;
  // 1 for lex ordering (except ls), -1 otherwise
  BOOLEAN   ComponentOrder;


  // what follows below here should be set by rComplete, _only_
  // contains component, but no weight fields in E */
  short      ExpL_Size; // size of exponent vector in long
  short      CmpL_Size; // portions which need to be compared
  /* number of long vars in exp vector:
     long vars are those longs in the exponent vector which are
     occupied by variables, only */
  short     VarL_Size;

  short      BitsPerExp; /* number of bits per exponent */
  short      ExpPerLong; /* maximal number of Exponents per long */

  short      pCompIndex; /* p->exp.e[pCompIndex] is the component */
  short      pOrdIndex; /* p->exp[pOrdIndex] is pGetOrd(p) */

  short      OrdSize; /* size of ord vector (in sro_ord) */


  short      ref; /* reference counter to the ring */

  /* if >= 0, long vars in exp vector are consecutive and start there
     if <  0, long vars in exp vector are not consecutive */
  short     VarL_LowIndex;
  // number of exponents in r->VarL_Offset[0]
  // is minimal number of exponents in a long var
  short     MinExpPerLong;

  /* if this is > 0, then NegWeightL_Offset[0..size_1] is index of longs in
   ExpVector whose values need an offset due to negative weights */
  short     NegWeightL_Size;
  /* array of NegWeigtL_Size indicies */
  int*      NegWeightL_Offset;

  /* array of size VarL_Size,
     VarL_Offset[i] gets i-th long var in exp vector */
  int*      VarL_Offset;

  /* mask for getting single exponents */
  unsigned long bitmask;
  /* mask used for divisiblity tests */
  unsigned long divmask;

  p_Procs_s*    p_Procs;

  /* FDeg and LDeg */
  pFDegProc     pFDeg;
  pLDegProc     pLDeg;

  /* as it was determined by rComplete */
  pFDegProc     pFDegOrig;
  /* and as it was determined before rOptimizeLDeg */
  pLDegProc     pLDegOrig;

  p_SetmProc    p_Setm;
  n_Procs_s*    cf;
  ring          algring;
#ifdef HAVE_PLURAL
  nc_struct     *nc;
#endif
};

struct sip_sideal
{
  poly*  m;
  long rank;
  int nrows;
  int ncols;
  #define IDELEMS(i) ((i)->ncols)
  inline int& idelems(void) { return ncols; }
};

struct sip_smap
{
  poly *m;
  char *preimage;
  int nrows;
  int ncols;
};
#endif /* __cplusplus */



/*
**  7. runtime procedures/global data
*/

/* 7.1 C-routines : */


#ifdef __cplusplus
extern "C" {
#endif
void  m2_end(short i);
#ifdef __cplusplus
}
#endif

/* 7.2 C++-routines : */

#ifdef __cplusplus
int   inits(void);
#ifdef buildin_rand
extern int siSeed;
int siRand();
#endif
#endif

/*the general set of std-options --> kutil.cc */
extern BITSET test;
/*the general set of verbose-options --> febase.cc */
#ifdef __cplusplus
extern "C" BITSET verbose;
#else
extern BITSET verbose;
#endif
/*debugging the bison grammar --> grammar.cc*/
#ifdef YYDEBUG
#if YYDEBUG
extern int    yydebug;
#endif
#endif

#ifdef HAVE_LIBPARSER
#ifdef __cplusplus
class libstack;
typedef libstack *  libstackv;
#endif
#endif /* HAVE_LIBPARSER */

extern struct omBin_s* MP_INT_bin;
extern struct omBin_s* char_ptr_bin;
extern struct omBin_s* ideal_bin;
extern struct omBin_s* int_bin;
extern struct omBin_s* poly_bin;
extern struct omBin_s* void_ptr_bin;
extern struct omBin_s* indlist_bin;
extern struct omBin_s* naIdeal_bin;
extern struct omBin_s* snaIdeal_bin;
extern struct omBin_s* sm_prec_bin;
extern struct omBin_s* smprec_bin;
extern struct omBin_s* sip_sideal_bin;
extern struct omBin_s* sip_smap_bin;
extern struct omBin_s* sip_sring_bin;
extern struct omBin_s* ip_sideal_bin;
extern struct omBin_s* ip_smap_bin;
extern struct omBin_s* ip_sring_bin;
extern struct omBin_s* sleftv_bin;

#endif

