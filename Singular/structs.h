#ifndef STRUCTS_H
#define STRUCTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: structs.h,v 1.12 1998-06-02 15:30:05 Singular Exp $ */
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
typedef idrec *            idhdl;
typedef rnumber *          number;
typedef polyrec *          poly;
typedef poly *             polyset;
typedef ip_sideal *        ideal;
typedef ip_smap *          map;
typedef ip_link *          si_link;
typedef ideal *            resolvente;
typedef ip_smatrix *       matrix;
typedef sleftv *           leftv;
typedef slists *           lists;
typedef sSubexpr *         Subexpr;
typedef union uutypes      utypes;
typedef sattr *            attr;
typedef skStrategy *       kStrategy;
typedef ip_command *       command;
typedef ip_package *       package;
typedef struct s_si_link_extension *si_link_extension;
typedef ssyStrategy *      syStrategy;
typedef struct reca *      alg;
typedef procinfo *         procinfov;

struct _scmdnames
{
  char *name;
  short alias;
  short tokval;
  short toktype;
};
typedef struct _scmdnames cmdnames;

/* the function pointer types */
//typedef void (*numberproc)(number a,number b,number * c);
typedef number (*numberfunc)(number a,number b);

typedef void    (*pSetmProc)(poly p);
typedef int     (*pLDegProc)(poly p, int *length);
typedef int     (*pFDegProc)(poly p);
typedef int     (*pCompProc)(poly p1, poly p2);

extern ring      currRing;
#endif

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
void  singular_help(char *str, BOOLEAN example);
#ifdef buildin_help
int   singular_manual(char *str);
#endif
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

