#ifndef IPSHELL_H
#define IPSHELL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipshell.h,v 1.4 1997-04-28 09:57:55 Singular Exp $ */
/*
* ABSTRACT
*/
#include <stdio.h>
#include "structs.h"
#include "subexpr.h"

extern int  traceit ;
#define TRACE_SHOW_PROC   1
#define TRACE_SHOW_LINENO 2
#define TRACE_SHOW_LINE   4
#define TRACE_SHOW_RINGS  8
#define TRACE_SHOW_LINE1  16
extern leftv iiCurrArgs;
extern int iiOp; /* the current operation*/
extern int  myynest;
extern char *  currid;
extern int     iiRETURNEXPR_len;
extern sleftv *iiRETURNEXPR;
extern ring   *iiLocalRing;
extern cmdnames cmds[];
extern char *lastreserved;

int     IsCmd(char *n, int & tok);
BOOLEAN iiPStart(idhdl pn, char *filename, sleftv * sl);
BOOLEAN iiEStart(char* example);
void    type_cmd(idhdl h);
void    test_cmd(int i);
void    list_cmd(int typ, const char* what, char * prefix, BOOLEAN iterate);
void    iiWriteMatrix(matrix im, const char *n, int dim, int spaces=0);
char *  iiStringMatrix(matrix im, int dim, char ch=',');
void    killlocals(int v);
int     exprlist_length(leftv v);
char *  Tok2Cmdname(int i);
char *  iiTwoOps(int t);
int     IsPrime(int i);

BOOLEAN iiWRITE(leftv res,leftv exprlist);
BOOLEAN iiExport(leftv v, int toLev);
BOOLEAN iiExport(leftv v, int toLev, idhdl &root);
char *  iiGetLibName(char *procstr);
char *  iiGetLibProcBuffer( char* libname, char* procname, int part=1 );
char *  iiProcName(char *buf, char & ct, char* &e);
char *  iiProcArgs(char *e,BOOLEAN withParenth);
BOOLEAN iiLibCmd( char *newlib );
leftv   iiMap(map theMap, char * what);
void    iiMakeResolv(resolvente r, int length, int rlen, char * name, int typ0,
           intvec ** weights=NULL);
BOOLEAN jjMINRES(leftv res, leftv v);
BOOLEAN jjBETTI(leftv res, leftv v);
int     iiRegularity(lists L);
leftv   singular_system(sleftv h);
BOOLEAN jjSYSTEM(leftv res, leftv v);
void    iiDebug();
BOOLEAN iiCheckRing(int i);
/* ================================================================== */
/* Expressions : */
BOOLEAN iiExprArith1(leftv res, sleftv* a, int op);
BOOLEAN iiExprArith2(leftv res, sleftv* a, int op, sleftv* b,
                     BOOLEAN proccall=FALSE);
BOOLEAN iiExprArith3(leftv res, int op, leftv a, leftv b, leftv c);
BOOLEAN iiExprArithM(leftv res, sleftv* a, int op);

typedef BOOLEAN (*proc1)(leftv,leftv);

#ifdef INIT_BUG
void    jjInitTab1();
#endif
#ifdef GENTABLE
typedef char * (*Proc1)(char *);
struct sValCmd1
{
  proc1 p;
  short cmd;
  short res;
  short arg;
};

typedef BOOLEAN (*proc2)(leftv,leftv,leftv);
struct sValCmd2
{
  proc2 p;
  short cmd;
  short res;
  short arg1;
  short arg2;
};

typedef BOOLEAN (*proc3)(leftv,leftv,leftv,leftv);
struct sValCmd3
{
  proc3 p;
  short cmd;
  short res;
  short arg1;
  short arg2;
  short arg3;
};
struct sValCmdM
{
  proc1 p;
  short cmd;
  short res;
  short number_of_args; /* -1: any, -2: any >0, .. */
};
extern struct sValCmd2 dArith2[];
extern struct sValCmd1 dArith1[];
extern struct sValCmd3 dArith3[];
extern struct sValCmdM dArithM[];
#endif

/* ================================================================== */
/* Assigments : */
BOOLEAN iiAssign(leftv left, leftv right);

typedef BOOLEAN (*proci)(leftv,leftv,Subexpr);
struct sValAssign_sys
{
  proc1 p;
  short res;
  short arg;
};

struct sValAssign
{
  proci p;
  short res;
  short arg;
};

BOOLEAN iiParameter(leftv p);
/* ================================================================== */
int     iiDeclCommand(leftv sy, leftv name, int lev, int t, idhdl* root,
  BOOLEAN init_b=TRUE);
sleftv * iiMake_proc(idhdl pn, sleftv* sl);
// from misc.cc:
void   showOption();
BOOLEAN setOption(leftv res, leftv v);
/* ================================================================== */
// from sing_dld.cc:
#ifdef HAVE_DLD
  sleftv * iiMake_binary(idhdl pn, sleftv* sl);
  BOOLEAN dlLoad(char * fn, char *pn, proc1 &p);
  #ifdef linux
    #ifdef __ELF__
      #define dlInit(A)
    #else
      void    dlInit(const char *thisfile);
    #endif
  #else
    #define dlInit(A)
  #endif
#endif
#endif

