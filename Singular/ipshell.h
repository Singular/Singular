#ifndef IPSHELL_H
#define IPSHELL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipshell.h,v 1.31 2003-12-10 17:34:29 Singular Exp $ */
/*
* ABSTRACT
*/
#include <stdio.h>
#include "structs.h"

extern int  traceit ;
#define TRACE_SHOW_PROC   1
#define TRACE_SHOW_LINENO 2
#define TRACE_SHOW_LINE   4
#define TRACE_SHOW_RINGS  8
#define TRACE_SHOW_LINE1  16
#define TRACE_BREAKPOINT  32
#define TRACE_TMP_BREAKPOINT  64
extern leftv iiCurrArgs;
extern int iiOp; /* the current operation*/
extern int  myynest;
extern char *  currid;
extern int     iiRETURNEXPR_len;
extern sleftv *iiRETURNEXPR;
#ifdef USE_IILOCALRING
extern ring   *iiLocalRing;
#endif
extern cmdnames cmds[];
extern char *lastreserved;
extern char *singular_date;

extern BOOLEAN yyInRingConstruction; /* 1: during ring construction */

int     IsCmd(char *n, int & tok);
BOOLEAN iiPStart(idhdl pn, sleftv * sl);
BOOLEAN iiEStart(char* example, procinfo *pi);
void    type_cmd(idhdl h);
void    test_cmd(int i);
void    list_cmd(int typ, const char* what, char * prefix, BOOLEAN iterate,
                 BOOLEAN fullname=FALSE);
void    iiWriteMatrix(matrix im, const char *n, int dim, int spaces=0);
char *  iiStringMatrix(matrix im, int dim, char ch=',');
void    killlocals(int v);
int     exprlist_length(leftv v);
char *  Tok2Cmdname(int i);
char *  iiTwoOps(int t);
int     IsPrime(int i);

BOOLEAN iiWRITE(leftv res,leftv exprlist);
BOOLEAN iiExport(leftv v, int toLev);
#ifdef HAVE_NS
BOOLEAN iiExport(leftv v, int toLev, idhdl roothdl);
BOOLEAN iiInternalExport (leftv v, int toLev, idhdl roothdl);
#endif /* HAVE_NS */
char *  iiGetLibName(procinfov v);
char *  iiGetLibProcBuffer( procinfov pi, int part=1 );
char *  iiProcName(char *buf, char & ct, char* &e);
char *  iiProcArgs(char *e,BOOLEAN withParenth);
BOOLEAN iiLibCmd( char *newlib, BOOLEAN tellerror=TRUE );
/* sees wheter library lib has already been loaded
   if yes, writes filename of lib into where and returns TRUE,
   if  no, returns FALSE
*/
BOOLEAN iiLocateLib(const char* lib, char* where);
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
poly    iiHighCorner(ideal i, int ak);
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
#ifdef HAVE_PLURAL  
  short valid_for_plural;
#endif  
};

typedef BOOLEAN (*proc2)(leftv,leftv,leftv);
struct sValCmd2
{
  proc2 p;
  short cmd;
  short res;
  short arg1;
  short arg2;
#ifdef HAVE_PLURAL  
  short valid_for_plural;
#endif  
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
#ifdef HAVE_PLURAL  
  short valid_for_plural;
#endif  
};
struct sValCmdM
{
  proc1 p;
  short cmd;
  short res;
  short number_of_args; /* -1: any, -2: any >0, .. */
#ifdef HAVE_PLURAL  
  short valid_for_plural;
#endif  
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
  BOOLEAN isring = FALSE, BOOLEAN init_b=TRUE);
#ifdef HAVE_NS
sleftv * iiMake_proc(idhdl pn, package pack, sleftv* sl);
#else /* HAVE_NS */
sleftv * iiMake_proc(idhdl pn, sleftv* sl);
#endif /* HAVE_NS */
// from misc.cc:
char *  showOption();
BOOLEAN setOption(leftv res, leftv v);
/* ================================================================== */
char * versionString();
/* ================================================================== */
void  singular_example(char *str);

#ifdef HAVE_NS
void listall(int showproc=1);
void checkall();
void iiCheckPack(package &p);
#endif
#endif

