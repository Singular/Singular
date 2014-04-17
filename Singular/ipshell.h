#ifndef IPSHELL_H
#define IPSHELL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
#include <stdio.h>
//#include <kernel/structs.h>
#include <kernel/ideals.h>
#include <Singular/lists.h>

struct _ssubexpr;
typedef struct _ssubexpr *Subexpr;

BOOLEAN    spectrumProc ( leftv,leftv );
BOOLEAN    spectrumfProc( leftv,leftv );
BOOLEAN    spaddProc    ( leftv,leftv,leftv );
BOOLEAN    spmulProc    ( leftv,leftv,leftv );
BOOLEAN    semicProc   ( leftv,leftv,leftv );
BOOLEAN    semicProc3   ( leftv,leftv,leftv,leftv );

extern leftv iiCurrArgs;
extern idhdl iiCurrProc;
extern int iiOp; /* the current operation*/
extern const char *  currid;
extern int     iiRETURNEXPR_len;
extern sleftv  iiRETURNEXPR;
#ifdef USE_IILOCALRING
extern ring   *iiLocalRing;
#endif
//extern cmdnames cmds[];
extern const char *lastreserved;
extern const char *singular_date; /* tesths.cc, set by final compile */

extern BOOLEAN yyInRingConstruction; /* 1: during ring construction */

int     IsCmd(const char *n, int & tok);

BOOLEAN iiPStart(idhdl pn, sleftv * sl);
BOOLEAN iiEStart(char* example, procinfo *pi);
BOOLEAN iiAllStart(procinfov pi, char *p,feBufferTypes t, int l);
void    type_cmd(leftv v);
void    test_cmd(int i);
void    list_cmd(int typ, const char* what, const char * prefix,
                 BOOLEAN iterate, BOOLEAN fullname=FALSE);
//char *  iiStringMatrix(matrix im, int dim, char ch=',');
void    killlocals(int v);
int     exprlist_length(leftv v);
const char *  Tok2Cmdname(int i);
const char *  iiTwoOps(int t);
int           iiOpsTwoChar(const char *s);

int     IsPrime(int i);

BOOLEAN iiWRITE(leftv res,leftv exprlist);
BOOLEAN iiExport(leftv v, int toLev);
BOOLEAN iiExport(leftv v, int toLev, package pack);
BOOLEAN iiInternalExport (leftv v, int toLev, package pack);
char *  iiGetLibName(procinfov v);
char *  iiGetLibProcBuffer( procinfov pi, int part=1 );
char *  iiProcName(char *buf, char & ct, char* &e);
char *  iiProcArgs(char *e,BOOLEAN withParenth);
BOOLEAN iiLibCmd( char *newlib, BOOLEAN autoexport, BOOLEAN tellerror, BOOLEAN force );
/* sees wheter library lib has already been loaded
   if yes, writes filename of lib into where and returns TRUE,
   if  no, returns FALSE
*/
/// load lib/module given in v
BOOLEAN jjLOAD(const char *s, BOOLEAN autoexport = FALSE);
BOOLEAN iiLocateLib(const char* lib, char* where);
leftv   iiMap(map theMap, const char * what);
void    iiMakeResolv(resolvente r, int length, int rlen, char * name, int typ0,
           intvec ** weights=NULL);
BOOLEAN jjMINRES(leftv res, leftv v);
BOOLEAN jjBETTI(leftv res, leftv v);
BOOLEAN jjBETTI2(leftv res, leftv u, leftv v);
BOOLEAN jjBETTI2_ID(leftv res, leftv u, leftv v);
BOOLEAN jjIMPORTFROM(leftv res, leftv u, leftv v);
BOOLEAN jjLIST_PL(leftv res, leftv v);

BOOLEAN jjVARIABLES_P(leftv res, leftv u);
BOOLEAN jjVARIABLES_ID(leftv res, leftv u);

int     iiRegularity(lists L);
leftv   singular_system(sleftv h);
BOOLEAN jjSYSTEM(leftv res, leftv v);
void    iiDebug();
BOOLEAN iiCheckRing(int i);
poly    iiHighCorner(ideal i, int ak);
char *  iiConvName(const char *libname);
BOOLEAN iiLoadLIB(FILE *fp, const char *libnamebuf, const char *newlib,
                         idhdl pl, BOOLEAN autoexport, BOOLEAN tellerror);

// converts a resolution into a list of modules
lists syConvRes(syStrategy syzstr,BOOLEAN toDel=FALSE,int add_row_shift=0);
// converts a list of modules into a minimal resolution
syStrategy syForceMin(lists li);
// converts a list of modules into a resolution
syStrategy syConvList(lists li,BOOLEAN toDel);

BOOLEAN syBetti1(leftv res, leftv u);
BOOLEAN syBetti2(leftv res, leftv u, leftv w);

/* ================================================================== */
/* Expressions : */
BOOLEAN iiExprArith1(leftv res, sleftv* a, int op);
BOOLEAN iiExprArith2(leftv res, sleftv* a, int op, sleftv* b,
                     BOOLEAN proccall=FALSE);
BOOLEAN iiExprArith3(leftv res, int op, leftv a, leftv b, leftv c);
BOOLEAN iiExprArithM(leftv res, sleftv* a, int op);
BOOLEAN iiApply(leftv res,leftv a, int op, leftv proc);

typedef BOOLEAN (*proc1)(leftv,leftv);

#ifdef __GNUC__
#if (__GNUC__ < 3)
#define INIT_BUG 1
void    jjInitTab1();
#endif
#endif

#ifdef GENTABLE
typedef char * (*Proc1)(char *);
struct sValCmd1
{
  proc1 p;
  short cmd;
  short res;
  short arg;
  short valid_for;
};

typedef BOOLEAN (*proc2)(leftv,leftv,leftv);
struct sValCmd2
{
  proc2 p;
  short cmd;
  short res;
  short arg1;
  short arg2;
  short valid_for;
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
  short valid_for;
};
struct sValCmdM
{
  proc1 p;
  short cmd;
  short res;
  short number_of_args; /* -1: any, -2: any >0, .. */
  short valid_for;
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
BOOLEAN iiAlias(leftv p);

int iiTokType(int op);
/* ================================================================== */
int     iiDeclCommand(leftv sy, leftv name, int lev, int t, idhdl* root,
  BOOLEAN isring = FALSE, BOOLEAN init_b=TRUE);
BOOLEAN iiMake_proc(idhdl pn, package pack, sleftv* sl);
// from misc.cc:
char *  showOption();
BOOLEAN setOption(leftv res, leftv v);
/* ================================================================== */
char * versionString();
/* ================================================================== */
void  singular_example(char *str);

BOOLEAN iiTryLoadLib(leftv v, const char *id);

int iiAddCproc(const char *libname, const char *procname, BOOLEAN pstatic,
               BOOLEAN(*func)(leftv res, leftv v));

void iiCheckPack(package &p);
#ifndef SING_NDEBUG
void checkall();
#endif

void rSetHdl(idhdl h);
ring rInit(sleftv* pn, sleftv* rv, sleftv* ord);
idhdl  rDefault(const char *s);

idhdl rSimpleFindHdl(ring r, idhdl root, idhdl n=NULL);
idhdl rFindHdl(ring r, idhdl n);
void   rKill(idhdl h);
void   rKill(ring r);
lists scIndIndset(ideal S, BOOLEAN all, ideal Q);
BOOLEAN mpKoszul(leftv res,leftv c/*ip*/, leftv b/*in*/, leftv id);
BOOLEAN mpJacobi(leftv res,leftv a);
BOOLEAN jjRESULTANT(leftv res, leftv u, leftv v, leftv w);
BOOLEAN kQHWeight(leftv res,leftv v);
BOOLEAN kWeight(leftv res,leftv id);
BOOLEAN loSimplex( leftv res, leftv args );
BOOLEAN loNewtonP( leftv res, leftv arg1 );
BOOLEAN nuMPResMat( leftv res, leftv arg1, leftv arg2 );
BOOLEAN nuLagSolve( leftv res, leftv arg1, leftv arg2, leftv arg3 );
BOOLEAN nuVanderSys( leftv res, leftv arg1, leftv arg2, leftv arg3);
BOOLEAN nuUResSolve( leftv res, leftv args );

BOOLEAN jjCHARSERIES(leftv res, leftv u);
/*
BOOLEAN jjRESULTANT(leftv res, leftv u, leftv v, leftv w);
#if 0
BOOLEAN jjIS_SQR_FREE(leftv res, leftv u);
#endif
*/
/* ================================================================== */
void paPrint(const char *n,package p);
/* ================================================================== */


BOOLEAN iiTestAssume(leftv a, leftv b);
#endif

