#ifndef IPID_H
#define IPID_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT: identfier handling
*/
#include "misc/options.h"
#include "Singular/idrec.h"
#include "Singular/subexpr.h"
#include "Singular/lists.h"

EXTERN_VAR idhdl currPackHdl;
EXTERN_VAR idhdl basePackHdl;
EXTERN_VAR package currPack;
EXTERN_VAR package basePack;
#define IDROOT (currPack->idroot)

struct sip_command;
typedef struct sip_command ip_command;
typedef ip_command *       command;

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

class proclevel
{
  public:
  proclevel * next;
  idhdl      cPackHdl;
  package    cPack;
  char      * name;
  proclevel()  { memset(this,0,sizeof(*this)); }
  void    push(char *);
  void    pop();
};
EXTERN_VAR proclevel *procstack;

typedef struct
{
  int (*iiAddCproc)(const char *libname, const char *procname, BOOLEAN pstatic,
                    BOOLEAN(*func)(leftv res, leftv v));
  int (*iiArithAddCmd)(const char *szName, short nAlias, short nTokval,
                       short nToktype, short nPos);

} SModulFunctions;


EXTERN_VAR idhdl      currRingHdl;
/* ================================================================== */
/* module support */
typedef int (*SModulFunc_t)(SModulFunctions*);
BOOLEAN load_builtin(const char *newlib, BOOLEAN autoexport, SModulFunc_t init);
void module_help_main(const char *newlib,const char *help);
void module_help_proc(const char *newlib,const char *p, const char *help);

/* ================================================================== */

/*extern ring     currRing;  in ring.h */

idhdl enterid(const char * a, int lev, int t, idhdl* root, BOOLEAN init=TRUE, BOOLEAN search=TRUE);
idhdl ggetid(const char *n);
//idhdl ggetid(const char *n, idhdl *packhdl);
void  killid(const char * a, idhdl * i);
void killhdl(idhdl h, package prooti=currPack);
void  killhdl2(idhdl h, idhdl * ih, ring r);
lists ipNameList(idhdl root);
lists ipNameListLev(idhdl root, int lev);
void  ipMoveId(idhdl h);
BOOLEAN checkPackage(package pack);
idhdl packFindHdl(package r);
poly jj_NormalizeQRingP(poly p, const ring r);
void jjNormalizeQRingId(leftv I);
#define jjNormalizeQRingP(p) jj_NormalizeQRingP(p,currRing)
void *idrecDataInit(int t);

#define FLAG_STD        0 // is a SB wrt. currRing ("isSB")
#define FLAG_TWOSTD     3 // is a 2-sided SB wrt. currRing (" 2SB")
#define FLAG_QRING      4 // is normalized wrt. qring ("qringNF")
#define FLAG_QRING_DEF  5 // is a qring to be defined
#define FLAG_OTHER_RING 6 // mark newstruct parts as "write-only"
#define FLAG_RING       7 // mark newstruct parts as "ring dep."
#define hasFlag(A,F) Sy_inset((F),(A)->flag)
#define setFlag(A,F) (A)->flag|=Sy_bit(F)
#define resetFlag(A,F) (A)->flag&=~Sy_bit(F)
void ipListFlag(idhdl h);


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
#define IDBIMAT(a)  ((a)->data.bim)
#define IDPOLY(a)   ((a)->data.p)
#define IDBIGINT(a) ((a)->data.n)
#define IDNUMBER(a) ((a)->data.n)
#define IDIDEAL(a)  ((a)->data.uideal)
#define IDMATRIX(a) ((a)->data.umatrix)
#define IDMAP(a)    ((a)->data.umap)
#define IDSTRING(a) ((a)->data.ustring)
#define IDLIST(a)   ((a)->data.l)
#define IDLINK(a)   ((a)->data.li)
#define IDPACKAGE(a) ((a)->data.pack)
#define IDPROC(a)   ((a)->data.pinf)

EXTERN_VAR omBin sip_command_bin;
EXTERN_VAR omBin sip_package_bin;
EXTERN_VAR omBin idrec_bin;
EXTERN_VAR omBin sleftv_bin;

EXTERN_VAR coeffs coeffs_BIGINT;

/* options */
struct soptionStruct
{
  const char * name;
  unsigned   setval;
  unsigned   resetval;
};

extern const struct soptionStruct optionStruct[];
extern const struct soptionStruct verboseStruct[];

#endif


