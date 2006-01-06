#ifndef STRUCTS_H
#define STRUCTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: structs.h,v 1.17 2006-01-06 11:14:07 Singular Exp $ */
/*
* ABSTRACT
*/

#include <string.h> /* for memset */

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


#if defined(i386) || defined(x86_64_Linux)
// the following settings seems to be better on i386 and x86_64 processors
// define if a*b is with mod instead of tables
#define HAVE_MULT_MOD
// #define HAVE_GENERIC_ADD
// #ifdef HAVE_MULT_MOD
// #define HAVE_DIV_MOD
// #endif
#endif

#ifdef IA64_Linux
// the following settings seems to be better on itanium processors
// #define HAVE_MULT_MOD
#define HAVE_GENERIC_ADD
// #ifdef HAVE_MULT_MOD
// #define HAVE_DIV_MOD
// #endif
#endif

#ifdef SunOS_5
// #define HAVE_GENERIC_ADD
#define HAVE_MULT_MOD
#ifdef HAVE_MULT_MOD
#define HAVE_DIV_MOD
#endif
#endif

#if SIZEOF_LONG == 4
typedef long long int64;
#elif SIZEOF_LONG == 8
typedef long int64;
#else
#error int64 undefined 
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
  nc_comm, /*2*/ /* yx= xy */
  nc_lie,  /*3*/ /* yx=xy+... */
  nc_undef /*4*/  /* for internal reasons */
};
#endif

typedef enum { LT_NONE, LT_NOTFOUND, LT_SINGULAR, LT_ELF, LT_HPUX, LT_MACH_O} lib_types;

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
typedef struct sideal_list *      ideal_list;
typedef ideal *            resolvente;
typedef union uutypes      utypes;
typedef ip_command *       command;
typedef struct s_si_link_extension *si_link_extension;
typedef polyrec *   napoly;

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
   // the union stuff
   // Zp:
   int npPrimeM;
   int npPminus1M;
   #ifdef HAVE_DIV_MOD
   CARDINAL *npInvTable;
   #endif
   #if !defined(HAVE_DIV_MOD) || !defined(HAVE_MULT_MOD)
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
   number  (*n_GetDenom)(number &n, const ring r);
   number  (*nGcd)(number a, number b, const ring r);
   number  (*nLcm)(number a, number b, const ring r);
   void    (*cfDelete)(number * a, const ring r);
   nMapFunc (*cfSetMap)(ring src, ring dst);
   char *  (*nName)(number n);
   void    (*nInpMult)(number &a, number b, ring r);
//extern number  (*nMap)(number from);

   number nNULL; /* the 0 as constant */
   int     char_flag;
   int     ref;
   short   nChar;
   n_coeffType type;
};
/* current ring stuff */

extern ring      currRing;
extern ideal     currQuotient;
extern idhdl      currRingHdl;

#ifndef HAVE_NS
extern idhdl      idroot;
#define IDROOT idroot
#endif /* ! HAVE_NS */

#ifdef HAVE_NS
extern idhdl currPackHdl;
extern idhdl basePackHdl;
extern package currPack;
extern package basePack;
#define IDROOT (currPack->idroot)
#endif /* HAVE_NS */

/* the function pointer types */

typedef long     (*pLDegProc)(poly p, int *length, ring r);
typedef long     (*pFDegProc)(poly p, ring r);
typedef void     (*p_SetmProc)(poly p, ring r);

typedef enum
{
  ro_dp, // ordering is a degree ordering
  ro_wp, // ordering is a weighted degree ordering
  ro_wp64, // ordering is a weighted64 degree ordering
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
     sro_wp64 wp64;
     sro_cp cp;
     sro_syzcomp syzcomp;
     sro_syz syz;
  } data;
};

#ifdef HAVE_PLURAL
struct nc_struct
{
  short ref;
  nc_type type;
  ring basering; // the ring C,D,.. live in
  matrix C;
  matrix D;
  matrix *MT;
  matrix COM;
  int *MTsize;
  int IsSkewConstant; /* indicates whethere coeffs C_ij are all equal */
  /* effective together with nc_type=nc_skew */
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
  ideal      minideal;
  int**      wvhdl;  /* array of weight vectors */
  char **    names;  /* array of variable names */

  unsigned long options; /* ring dependent options */

  // what follows below here should be set by rComplete, _only_
  long      *ordsgn;  /* array of +/- 1 (or 0) for comparing monomials */
                       /*  ExpL_Size entries*/

  // is NULL for lp or N == 1, otherwise non-NULL (with OrdSize > 0 entries) */
  sro_ord*   typ;   /* array of orderings + sizes, OrdSize entries */

  ideal      qideal; /* extension to the ring structure: qring */


  int*     VarOffset;
  int*     firstwv;

  struct omBin_s*   PolyBin; /* Bin from where monoms are allocated */
  int        ch;     /* characteristic */
#ifdef HAVE_RING2TOM
  int        cring;  /* cring = 0 => coefficient field, cring = 1 => coeffs from Z/2^m */
#endif
  int        ref; /* reference counter to the ring */

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

struct sideal_list
{
  ideal_list next;
  ideal      d;
#ifndef NDEBUG
  int nr;
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
void  m2_end(short i);
#ifdef __cplusplus
}
#endif

/* 7.2 C++-routines : */

#ifdef __cplusplus
int   inits(void);
int   IsPrime(int i);
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

extern int      yylineno;
extern char     my_yylinebuf[80];

#define loop for(;;)

#ifndef ABS
#define ABS(x) ((x)<0?(-(x)):(x))
#endif

#if defined(__cplusplus)
inline int si_max(const int a, const int b)  { return (a>b) ? a : b; }
inline int si_min(const int a, const int b)  { return (a<b) ? a : b; }
#if defined(DecAlpha_Linux) || defined(IA64_Linux)
inline long si_max(const long a, const long b)  { return (a>b) ? a : b; }
inline long si_min(const long a, const long b)  { return (a<b) ? a : b; }
#endif
#else
#define si_max(A,B) ((A) > (B) ? (A) : (B))
#define si_min(A,B) ((A) < (B) ? (A) : (B))
#endif

/*
**  Set operations (small sets only)
*/

#define Sy_bit(x)     ((unsigned)1<<(x))
#define Sy_inset(x,s) ((Sy_bit(x)&(s))?TRUE:FALSE)
#define BTEST1(a)     Sy_inset((a), test)
#define BVERBOSE(a)   Sy_inset((a), verbose)

/*
** defines for BITSETs
*/

#define V_SHOW_MEM  2
#define V_YACC      3
#define V_REDEFINE  4
#define V_READING   5
#define V_LOAD_LIB  6
#define V_DEBUG_LIB 7
#define V_LOAD_PROC 8
#define V_DEF_RES   9
#define V_DEBUG_MEM 10
#define V_SHOW_USE  11
#define V_IMAP      12
#define V_PROMPT    13
#define V_NSB       14
#define V_CONTENTSB 15
#define V_CANCELUNIT 16
#define V_DEG_STOP  31


#define OPT_PROT           0
#define OPT_REDSB          1
#define OPT_NOT_BUCKETS    2
#define OPT_NOT_SUGAR      3
#define OPT_INTERRUPT      4
#define OPT_SUGARCRIT      5
#define OPT_DEBUG          6
#define OPT_REDTHROUGH     7
#define OPT_RETURN_SB      9
#define OPT_FASTHC        10
#define OPT_OLDSTD        20
#define OPT_KEEPVARS      21
#define OPT_STAIRCASEBOUND 22
#define OPT_MULTBOUND     23
#define OPT_DEGBOUND      24
#define OPT_REDTAIL       25
#define OPT_INTSTRATEGY   26
#define OPT_INFREDTAIL    28
#define OPT_SB_1          29
#define OPT_NOTREGULARITY 30
#define OPT_WEIGHTM       31

/* define ring dependent options */
#define TEST_RINGDEP_OPTS \
 (Sy_bit(OPT_INTSTRATEGY) | Sy_bit(OPT_REDTHROUGH) | Sy_bit(OPT_REDTAIL))

#define TEST_OPT_PROT              BTEST1(OPT_PROT)
#define TEST_OPT_REDSB             BTEST1(OPT_REDSB)
#define TEST_OPT_NOT_BUCKETS       BTEST1(OPT_NOT_BUCKETS)
#define TEST_OPT_NOT_SUGAR         BTEST1(OPT_NOT_SUGAR)
#define TEST_OPT_SUGARCRIT         BTEST1(OPT_SUGARCRIT)
#define TEST_OPT_DEBUG             BTEST1(OPT_DEBUG)
#define TEST_OPT_FASTHC            BTEST1(OPT_FASTHC)
#define TEST_OPT_INTSTRATEGY       BTEST1(OPT_INTSTRATEGY)
#define TEST_OPT_RETURN_SB         BTEST1(OPT_RETURN_SB)
#define TEST_OPT_KEEPVARS          BTEST1(OPT_KEEPVARS)
#define TEST_OPT_DEGBOUND          BTEST1(OPT_DEGBOUND)
#define TEST_OPT_MULTBOUND         BTEST1(OPT_MULTBOUND)
#define TEST_OPT_STAIRCASEBOUND    BTEST1(OPT_STAIRCASEBOUND)
#define TEST_OPT_REDTAIL           BTEST1(OPT_REDTAIL)
#define TEST_OPT_INFREDTAIL        BTEST1(OPT_INFREDTAIL)
#define TEST_OPT_SB_1              BTEST1(OPT_SB_1)
#define TEST_OPT_NOTREGULARITY     BTEST1(OPT_NOTREGULARITY)
#define TEST_OPT_WEIGHTM           BTEST1(OPT_WEIGHTM)
#define TEST_OPT_REDTHROUGH        BTEST1(OPT_REDTHROUGH)
#define TEST_OPT_OLDSTD            BTEST1(OPT_OLDSTD)
#define TEST_OPT_CONTENTSB         BVERBOSE(V_CONTENTSB)
#define TEST_OPT_CANCELUNIT        BVERBOSE(V_CANCELUNIT)

#define TEST_VERB_NSB              BVERBOSE(V_NSB)
#define TEST_V_DEG_STOP            BVERBOSE(V_DEG_STOP)


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

#ifdef __cplusplus
union uutypes
{
  int           i;
  ring          uring;
  poly          p;
  number        n;
  ideal         uideal;
  map           umap;
  matrix        umatrix;
  char *        ustring;
  intvec *      iv;
  lists         l;
  si_link       li;
  package       pack;
  procinfo *    pinf;
};

class idrec
{
  public:
  /* !! do not change the first 6 entries !! (see subexpr.h: sleftv) */
  idhdl      next;
  char *     id;
  utypes     data;
  attr       attribute;
  BITSET     flag;
  idtyp      typ;

  short      lev;
  short      ref;

#define IDNEXT(a)    ((a)->next)
#define IDTYP(a)     ((a)->typ)
#define IDFLAG(a)    ((a)->flag)
#define IDLEV(a)     ((a)->lev)
#define IDID(a)      ((a)->id)
#define IDATTR(a)    ((a)->attribute)

#define IDINT(a)    ((int)(long)((a)->data.ustring))
#define IDDATA(a)   ((a)->data.ustring)
#define IDRING(a)   ((a)->data.uring)
#define IDINTVEC(a) ((a)->data.iv)
#define IDPOLY(a)   ((a)->data.p)
#define IDNUMBER(a) ((a)->data.n)
#define IDIDEAL(a)  (((a)->data).uideal)
#define IDMATRIX(a) (((a)->data).umatrix)
#define IDMAP(a)    (((a)->data).umap)
#define IDSTRING(a) ((a)->data.ustring)
#define IDLIST(a)   ((a)->data.l)
#define IDLINK(a)   ((a)->data.li)
#define IDPACKAGE(a) ((a)->data.pack)
#define IDPROC(a)   ((a)->data.pinf)

  idrec() { memset(this,0,sizeof(*this)); }
  idhdl get(const char * s, int lev);
  idhdl set(char * s, int lev, idtyp t, BOOLEAN init=TRUE);
  char * String();
//  ~idrec();
};

class proc_singular
{
public:
  long   proc_start;       // position where proc is starting
  long   def_end;          // position where proc header is ending
  long   help_start;       // position where help is starting
  long   help_end;         // position where help is starting
  long   body_start;       // position where proc-body is starting
  long   body_end;         // position where proc-body is ending
  long   example_start;    // position where example is starting
  long   proc_end;         // position where proc is ending
  int    proc_lineno;
  int    body_lineno;
  int    example_lineno;
  char   *body;
  long help_chksum;
};

struct proc_object
{
//public:
  BOOLEAN (*function)(leftv res, leftv v);
};

union uprocinfodata;

union uprocinfodata
{
public:
  proc_singular  s;        // data of Singular-procedure
  struct proc_object    o; // pointer to binary-function
};

typedef union uprocinfodata procinfodata;

typedef enum { LANG_NONE, LANG_TOP, LANG_SINGULAR, LANG_C, LANG_MAX} language_defs;
// LANG_TOP     : Toplevel package only
// LANG_SINGULAR:
// LANG_C       :
class procinfo
{
public:
  char          *libname;
  char          *procname;
#ifdef HAVE_NS
  package       pack;
#endif
  language_defs language;
  short         ref;
  char          is_static;        // if set, proc not accessible for user
  char          trace_flag;
  procinfodata  data;
};

#endif

#endif

