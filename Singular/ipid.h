#ifndef IPID_H
#define IPID_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipid.h,v 1.37 2002-12-13 16:20:06 Singular Exp $ */
/*
* ABSTRACT: identfier handling
*/
#include <string.h>
#include "structs.h"
#include "subexpr.h"

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
  idhdl         idroot; /* local objects */
  char          *libname;
  short         ref;
  language_defs language;
  BOOLEAN       loaded;
  void          *handle;
};

inline package paCopy(package pack)
{
  pack->ref++;
  return pack;
}

inline void paKill(package pack)
{
  pack->ref--;
}

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
  char * String();
//  ~idrec();
};

class proclevel {
  public:
  proclevel * next;
  idhdl      cRingHdl;
  ring       cRing;
  #ifdef HAVE_NS
  idhdl      cPackHdl;
  package    cPack;
  #endif
  char      * name;
  proclevel()  { memset(this,0,sizeof(*this)); }
  void    push(char *);
  void    pop();
};
extern proclevel *procstack;

#ifndef HAVE_NS
extern idhdl      idroot;
#define IDROOT idroot
#endif /* HAVE_NS */

#ifdef HAVE_NS
extern idhdl currPackHdl;
extern idhdl basePackHdl;
extern package currPack;
extern package basePack;
#define IDROOT (currPack->idroot)
#endif /* HAVE_NS */

extern idhdl      currRingHdl;
/*extern ring     currRing;  in structs.h */
extern ideal      currQuotient;

char *idhdl2id(idhdl pck, idhdl h);
void  iiname2hdl(const char *name, idhdl *pck, idhdl *id);
idhdl enterid(char * a, int lev, idtyp t, idhdl* root, BOOLEAN init=TRUE);
idhdl ggetid(const char *n, BOOLEAN local = FALSE);
idhdl ggetid(const char *n, BOOLEAN local, idhdl *packhdl);
void  killid(char * a, idhdl * i);
#ifdef HAVE_NS
void killhdl(idhdl h, package prooti=currPack);
#else
void  killhdl(idhdl h);
#endif
void  killhdl2(idhdl h, idhdl * ih, ring r);
lists ipNameList(idhdl root);
void  ipMoveId(idhdl h);
BOOLEAN checkPackage(package pack);
#ifdef HAVE_NS
idhdl packFindHdl(package r);
#endif

#define FLAG_STD   0
#define FLAG_DRING 1
#define FLAG_DOPERATOR 2
#define FLAG_TWOSTD  3
#define hasFlag(A,F) Sy_inset((F),(A)->flag)
#define setFlag(A,F) (A)->flag|=Sy_bit(F)
#define resetFlag(A,F) (A)->flag&=~Sy_bit(F)
void ipListFlag(idhdl h);

#ifndef OM_ALLOC_H
struct omBin_s;
#endif

extern omBin_s* sip_command_bin;
extern omBin_s* ip_command_bin;
extern omBin_s* sip_package_bin;
extern omBin_s* ip_package_bin;
extern omBin_s* idrec_bin;
extern omBin_s* namerec_bin;
#endif


