#ifndef STRUCTS_H
#define STRUCTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: structs.h,v 1.22 1999-07-28 17:51:07 Singular Exp $ */
/*
* ABSTRACT
*/

/* standard types */
typedef unsigned char  uchar ;
typedef unsigned short CARDINAL;
typedef short BOOLEAN;

typedef void * Sy_reference;
#define ADDRESS Sy_reference
#define BITSET  unsigned int

/* EXPONENT_TYPE is determined by configure und defined in mod2.h */
typedef EXPONENT_TYPE Exponent_t;

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

struct sip_memHeap;
typedef struct sip_memHeap ip_memHeap;
typedef ip_memHeap *       memHeap;

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
#endif

struct  sip_sring;
struct  sip_sideal;
struct  sip_link;
struct  spolynom;
struct _ssubexpr;
struct _sssym;
struct snumber;
struct sip_command;
struct sip_package;
struct s_si_link_extension;
struct reca;

typedef struct _ssubexpr   sSubexpr;
typedef struct _sssym      ssym;
typedef struct spolyrec    polyrec;
typedef struct snumber     rnumber;
typedef struct sip_sideal  ip_sideal;
typedef struct sip_smap    ip_smap;
typedef struct sip_sring   ip_sring;
typedef struct sip_link    ip_link;
typedef struct sip_command ip_command;
typedef struct sip_package ip_package;


/* the pointer types */
typedef ip_sring *         ring;
typedef int                idtyp;
typedef rnumber *          number;
typedef polyrec *          poly;
typedef poly *             polyset;
typedef ip_sideal *        ideal;
typedef ip_smap *          map;
typedef ideal *            resolvente;
typedef union uutypes      utypes;
typedef ip_command *       command;
typedef struct s_si_link_extension *si_link_extension;
typedef struct reca *      alg;
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

struct _scmdnames
{
  char *name;
  short alias;
  short tokval;
  short toktype;
};
typedef struct _scmdnames cmdnames;

struct sip_sring
{
  idhdl      idroot; /* local objects */
  int*       order;  /* array of orderings */
  int*       block0; /* starting pos.*/
  int*       block1; /* ending pos.*/
  char**     parameter; /* names of parameters */
  number     minpoly;
  short**    wvhdl;  /* array of weight vectors */
  char **    names;  /* array of variable names */
  /* extension to the ring structure: qring */
  ideal      qideal;
#ifdef SDRING
  short      partN;
#endif
  short      ch;     /* characteristic */
  short      ch_flags; /* additional char-flags */
  short      N;      /* number of vars */

  short      P;      /* number of pars */
  short      OrdSgn; /* 1 for polynomial rings, -1 otherwise */

  short      ref;

  // what follows below here should be set by rComplete, _only_
  int       *VarOffset;   /* controls indexing of exponents */
  short     VarCompIndex; /* location of component in exp vector */
  short     VarLowIndex;  /* lowest index of an exponent */
  short     VarHighIndex; /* Highest index of an expoentn */

#ifdef RDEBUG
  short      no; /* unique id for rings */
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
#endif

/* the function pointer types */
typedef number (*numberfunc)(number a,number b);

typedef void    (*pSetmProc)(poly p);
typedef int     (*pLDegProc)(poly p, int *length);
typedef int     (*pFDegProc)(poly p);
typedef int     (*pCompProc)(poly p1, poly p2);

extern ring      currRing;

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

#endif

