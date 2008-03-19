#ifndef IPID_H
#define IPID_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipid.h,v 1.43 2008-03-19 17:44:34 Singular Exp $ */
/*
* ABSTRACT: identfier handling
*/
#include <string.h>
#include "structs.h"
#ifdef TOK_H
#include "subexpr.h"
#endif

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

typedef struct
{
  int (*iiAddCproc)(char *libname, char *procname, BOOLEAN pstatic,
                    BOOLEAN(*func)(leftv res, leftv v));
  int (*iiArithAddCmd)(const char *szName, short nAlias, short nTokval,
                       short nToktype, short nPos);

} SModulFunctions;


/*extern idhdl      currRingHdl; in structs.h */
/*extern ring     currRing;  in structs.h */
/*extern ideal      currQuotient; in structs.h */

char *idhdl2id(idhdl pck, idhdl h);
void  iiname2hdl(const char *name, idhdl *pck, idhdl *id);
idhdl enterid(const char * a, int lev, idtyp t, idhdl* root, BOOLEAN init=TRUE);
idhdl ggetid(const char *n, BOOLEAN local = FALSE);
idhdl ggetid(const char *n, BOOLEAN local, idhdl *packhdl);
void  killid(const char * a, idhdl * i);
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


