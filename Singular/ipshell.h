#ifndef IPSHELL_H
#define IPSHELL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
#include <stdio.h>
//#include "kernel/structs.h"
#include "kernel/ideals.h"
#include "Singular/lists.h"
#include "Singular/fevoices.h"

struct _ssubexpr;
typedef struct _ssubexpr *Subexpr;

BOOLEAN    spectrumProc ( leftv,leftv );
BOOLEAN    spectrumfProc( leftv,leftv );
BOOLEAN    spaddProc    ( leftv,leftv,leftv );
BOOLEAN    spmulProc    ( leftv,leftv,leftv );
BOOLEAN    semicProc   ( leftv,leftv,leftv );
BOOLEAN    semicProc3   ( leftv,leftv,leftv,leftv );

BOOLEAN iiAssignCR(leftv, leftv);

BOOLEAN iiARROW (leftv, char*,char *);

EXTERN_VAR leftv iiCurrArgs;
EXTERN_VAR idhdl iiCurrProc;
EXTERN_VAR int iiOp; /* the current operation*/
extern const char *  currid;
EXTERN_VAR int     iiRETURNEXPR_len;
EXTERN_INST_VAR sleftv  iiRETURNEXPR;
EXTERN_VAR ring   *iiLocalRing;
//extern cmdnames cmds[];
extern const char *lastreserved;
EXTERN_VAR int myynest;
EXTERN_VAR int printlevel;
EXTERN_VAR int si_echo;


EXTERN_VAR BOOLEAN yyInRingConstruction; /* 1: during ring construction */

int     IsCmd(const char *n, int & tok);

BOOLEAN iiPStart(idhdl pn, leftv sl);
BOOLEAN iiEStart(char* example, procinfo *pi);
BOOLEAN iiAllStart(procinfov pi, const char *p,feBufferTypes t, int l);
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

BOOLEAN iiWRITE(leftv res,leftv exprlist);
BOOLEAN iiExport(leftv v, int toLev);
BOOLEAN iiExport(leftv v, int toLev, package pack);
BOOLEAN iiInternalExport (leftv v, int toLev, package pack);
/// find the library of an proc
static inline char *  iiGetLibName(const procinfov pi) { return pi->libname; }
char *  iiGetLibProcBuffer( procinfov pi, int part=1 );
char *  iiProcName(char *buf, char & ct, char* &e);
char *  iiProcArgs(char *e,BOOLEAN withParenth);
BOOLEAN iiLibCmd( const char *newlib, BOOLEAN autoexport, BOOLEAN tellerror, BOOLEAN force );
/* sees wheter library lib has already been loaded
   if yes, writes filename of lib into where and returns TRUE,
   if  no, returns FALSE
*/
/// load lib/module given in v
BOOLEAN jjLOAD(const char *s, BOOLEAN autoexport = FALSE);
BOOLEAN jjLOAD_TRY(const char *s);
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
BOOLEAN iiGetLibStatus(const char *lib);
BOOLEAN iiLoadLIB(FILE *fp, const char *libnamebuf, const char *newlib,
                         idhdl pl, BOOLEAN autoexport, BOOLEAN tellerror);

// converts a resolution into a list of modules
lists syConvRes(syStrategy syzstr,BOOLEAN toDel=FALSE,int add_row_shift=0);
// converts a list of modules into a minimal resolution
//syStrategy syForceMin(lists li);
// converts a list of modules into a resolution
syStrategy syConvList(lists li);

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

#ifdef GENTABLE
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
extern const struct sValCmd2 dArith2[];
extern const struct sValCmd1 dArith1[];
extern const struct sValCmd3 dArith3[];
extern const struct sValCmdM dArithM[];
#endif

/* ================================================================== */
/* Assigments : */
BOOLEAN iiAssign(leftv left, leftv right, BOOLEAN toplevel=TRUE);
coeffs jjSetMinpoly(coeffs cf, number a);

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
BOOLEAN iiMake_proc(idhdl pn, package pack, leftv sl);
void* iiCallLibProc1(const char* n, void *arg, int arg_type, BOOLEAN &err);
leftv ii_CallLibProcM(const char* n, void **args, int *arg_types, const ring R, BOOLEAN &err);
ideal ii_CallProcId2Id(const char* lib,const char* proc, ideal arg,const ring R);
int ii_CallProcId2Int(const char* lib,const char* proc, ideal arg,const ring R);
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
ring rInit(leftv pn, leftv rv, leftv ord);
idhdl  rDefault(const char *s);

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

/* table interface for iiAddCproc */
/// apply an operation 'op' to an argument a
/// return TRUE on failure
BOOLEAN iiExprArith1Tab(leftv res,///< [out] pre-allocated result
                        leftv a,  ///< [in]  argument
                        int op,   ///< [in]  operation
                        const struct sValCmd1* dA1, ///< [in] table of possible proc
                                                  ///< assumes dArith1[0].cmd==op
                        int at,   ///< [in] a->Typ()
                        const struct sConvertTypes *dConvertTypes ///< [in] table of type conversions
                        );
/// apply an operation 'op' to arguments a and a->next
/// return TRUE on failure
BOOLEAN iiExprArith2Tab(leftv res,///< [out] pre-allocated result
                        leftv a,  ///< [in]  2 arguments
                        int op,   ///< [in]  operation
                        const struct sValCmd2* dA2,///< [in] table of possible proc
                                   ///< assumes dA2[0].cmd==op
                        int at,    ///< [in] a->Typ()
                        const struct sConvertTypes *dConvertTypes ///< [in] table of type conversions
                        );
/// apply an operation 'op' to arguments a, a->next and a->next->next
/// return TRUE on failure
BOOLEAN iiExprArith3Tab(leftv res, ///< [out] pre-allocated result
                        leftv a,   ///< [in]  3 arguments
                        int op,    ///< [in]  operation
                        const struct sValCmd3* dA3,///< [in] table of possible proc
                                   ///< assumes dA3[0].cmd==op
                        int at,    ///< [in] a->Typ()
                        const struct sConvertTypes *dConvertTypes ///< [in] table of type conversions
                        );

/// check a list of arguemys against a given field of types
/// return TRUE if the types match
/// return FALSE (and, if report) report an error via Werror otherwise
BOOLEAN iiCheckTypes(leftv args,/// < [in] argument list (may be NULL)
                      const short *type_list,///< [in] field of types
                                             ///< len, t1,t2,...
                      int report=0  /// ;in] report error?
                      );

BOOLEAN iiBranchTo(leftv r, leftv args);

lists rDecompose(const ring r);

lists rDecompose_list_cf(const ring r);
BOOLEAN rDecompose_CF(leftv res,const coeffs C);
ring rCompose(const lists  L, const BOOLEAN check_comp=TRUE, const long bitmask=0x7fff, const int isLetterplace=FALSE);

void iiSetReturn(const leftv h);
#endif

