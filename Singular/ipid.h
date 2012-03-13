#ifndef IPID_H
#define IPID_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: identfier handling
*/
#include <string.h>
#include <kernel/structs.h>
#include <kernel/idrec.h>
#include <Singular/subexpr.h>

extern idhdl currPackHdl;
extern idhdl basePackHdl;
extern package currPack;
extern package basePack;
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
  idhdl      cRingHdl;
  ring       cRing;
  idhdl      cPackHdl;
  package    cPack;
  char      * name;
  proclevel()  { memset(this,0,sizeof(*this)); }
  void    push(char *);
  void    pop();
};
extern proclevel *procstack;

typedef struct
{
  int (*iiAddCproc)(const char *libname, const char *procname, BOOLEAN pstatic,
                    BOOLEAN(*func)(leftv res, leftv v));
  int (*iiArithAddCmd)(const char *szName, short nAlias, short nTokval,
                       short nToktype, short nPos);

} SModulFunctions;


/* ================================================================== */
/* module support */
typedef int (*SModulFunc_t)(SModulFunctions*);
BOOLEAN load_builtin(char *newlib, BOOLEAN autoexport, SModulFunc_t init);
void module_help_main(char *newlib,const char *help);
void module_help_proc(char *newlib,const char *p, const char *help);

/* ================================================================== */

/*extern idhdl      currRingHdl; in ring.h */
/*extern ring     currRing;  in ring.h */
/*extern ideal      currQuotient; in structs.h */

idhdl enterid(const char * a, int lev, int t, idhdl* root, BOOLEAN init=TRUE, BOOLEAN serach=TRUE);
idhdl ggetid(const char *n);
idhdl ggetid(const char *n, BOOLEAN local, idhdl *packhdl);
void  killid(const char * a, idhdl * i);
void killhdl(idhdl h, package prooti=currPack);
void  killhdl2(idhdl h, idhdl * ih, ring r);
lists ipNameList(idhdl root);
lists ipNameListLev(idhdl root, int lev);
void  ipMoveId(idhdl h);
BOOLEAN checkPackage(package pack);
idhdl packFindHdl(package r);
void jjNormalizeQRingId(leftv I);
void jjNormalizeQRingP(leftv I);
void *idrecDataInit(int t);

#define FLAG_STD   0
#define FLAG_TWOSTD  3
#define FLAG_QRING   4
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

extern omBin sip_command_bin;
extern omBin sip_package_bin;
extern omBin idrec_bin;
#endif


