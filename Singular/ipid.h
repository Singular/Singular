#ifndef IPID_H
#define IPID_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipid.h,v 1.16 1998-09-24 09:59:45 Singular Exp $ */
/*
* ABSTRACT: identfier handling
*/
#include <string.h>
#include "structs.h"
#include "subexpr.h"
//#include "polys-impl.h"

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
  short      N;      /* number of vars */

  short      P;      /* number of pars */
  short      OrdSgn; /* 1 for polynomial rings, -1 otherwise */

  short      ref;

  // what follows below here should be set by rComplete, _only_
  short     VarOffset; /* controls indexing of exponents */
  short     VarCompIndex; /* controls locations of component in exp vector */

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

struct sip_command
{
  sleftv arg1; /*arg1 to build_in, proc to proc_call*/
  sleftv arg2; /*NULL or arg2 to build_in, args to proc_call*/
  sleftv arg3; /*NULL or arg3*/
  short argc; /*0,1,2,3 to build_in, -1 otherwise*/
  short op;   /* build_in or PROC_CMD*/
};

struct sip_package
{
  idhdl      idroot; /* local objects */
};

union uutypes;

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
  BITSET     flag;
  attr       attribute;
  idtyp      typ;

  short      lev;
  short      ref;

#define IDNEXT(a)    ((a)->next)
#define IDTYP(a)     ((a)->typ)
#define IDFLAG(a)    ((a)->flag)
#define IDLEV(a)     ((a)->lev)
#define IDID(a)      ((a)->id)
#define IDATTR(a)    ((a)->attribute)

#define IDINT(a)    ((a)->data.i)
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
//  ~idrec();
};

#ifdef HAVE_NAMESPACES
class namerec {
  public:
  namehdl    next;
  namehdl    root;
  package    pack;
  idhdl      currRingHdl;
  char *     name;
  int        lev;
  BOOLEAN    isroot;
#define NSROOT(a) ((a)->pack->idroot)
#define NSPACK(a) ((a)->pack)


 namerec()  { memset(this,0,sizeof(*this)); }
  //namehdl    Set(idhdl root);
  namehdl    pop();
  namehdl    push(package pack, char *name, BOOLEAN init=FALSE);
  idhdl      get(const char * s, int lev, BOOLEAN root=FALSE);
};

extern namehdl namespaceroot;
#  define IDROOT (NSROOT(namespaceroot))
#else /* HAVE_NAMESPACES */
extern idhdl      idroot;
#  define IDROOT idroot
#endif /* HAVE_NAMESPACES */

extern idhdl      currRingHdl;
/*extern ring     currRing;  in structs.h */
extern ideal      currQuotient;

char *idhdl2id(idhdl pck, idhdl h);
void  iiname2hdl(char *name, idhdl *pck, idhdl *id);
idhdl enterid(char * a, int lev, idtyp t, idhdl* root, BOOLEAN init=TRUE);
idhdl ggetid(const char *n, BOOLEAN local = FALSE);
idhdl ggetid(const char *n, BOOLEAN local, idhdl *packhdl);
void  killid(char * a, idhdl * i);
void  killhdl(idhdl h);
void  killhdl(idhdl h, idhdl * ih);
lists ipNameList(idhdl root);
void  ipMoveId(idhdl h);

#define FLAG_STD   0
#define FLAG_DRING 1
#define FLAG_DOPERATOR 2
#define hasFlag(A,F) Sy_inset((F),(A)->flag)
#define setFlag(A,F) (A)->flag|=Sy_bit(F)
#define resetFlag(A,F) (A)->flag&=~Sy_bit(F)
void ipListFlag(idhdl h);
#endif


