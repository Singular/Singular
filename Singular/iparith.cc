/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: iparith.cc,v 1.278 2002-02-28 17:56:34 mschulze Exp $ */

/*
* ABSTRACT: table driven kernel interface, used by interpreter
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "mod2.h"
#include "tok.h"
#include "ipid.h"
#include "intvec.h"
#include "omalloc.h"
#include "polys.h"
#include "febase.h"
#include "sdb.h"
#include "longalg.h"
#include "ideals.h"
#include "matpol.h"
#include "kstd1.h"
#include "timer.h"
#include "ring.h"
#include "subexpr.h"
#include "lists.h"
#include "longalg.h"
#include "numbers.h"
#include "stairc.h"
#include "maps.h"
#include "syz.h"
#include "weight.h"
#include "ipconv.h"
#include "ipprint.h"
#include "sing_dld.h"
#include "attrib.h"
#include "silink.h"
#include "sparsmat.h"
#include "algmap.h"
#include "units.h"
#ifdef HAVE_FACTORY
#include "clapsing.h"
#include "kstdfac.h"
#endif
#ifdef HAVE_FGLM
#include "fglm.h"
#endif

#include "ipshell.h"

#include "mpr_inout.h"

/*=============== types =====================*/
struct sValCmdTab
{
  short cmd;
  short start;
};

typedef sValCmdTab jjValCmdTab[];

/* ifdef GENTABLE: definitions are in ipshell.h */
#ifndef GENTABLE
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
#endif

/*============= proc =======================*/
static BOOLEAN jjLOAD(leftv res, leftv v, BOOLEAN autoexport = FALSE);
static int iiTabIndex(const jjValCmdTab dArithTab, const int len, const int op);
#ifdef MDEBUG
#define jjMakeSub(A) jjDBMakeSub(A,__FILE__,__LINE__)
static Subexpr jjDBMakeSub(leftv e,char *f, int l);
#else
static Subexpr jjMakeSub(leftv e);
#endif

/*============= vars ======================*/
extern int cmdtok;
extern BOOLEAN expected_parms;

#define ii_div_by_0 "div. by 0"
int iiOp; /* the current operation*/

#ifdef GENTABLE
cmdnames cmds[] =
{  // name-string alias tokval          toktype
  { "$INVALID$",   0, -1,                 0},
  { "and",         0, '&' ,               LOGIC_OP},
  { "attrib",      0, ATTRIB_CMD ,        CMD_123},
  { "bareiss",     0, BAREISS_CMD ,       CMD_123},
  { "betti",       0, BETTI_CMD ,         CMD_12},
  { "break",       0, BREAK_CMD ,         BREAK_CMD},
  { "breakpoint",  0, BREAKPOINT_CMD ,    CMD_M},
  { "char",        0, CHARACTERISTIC_CMD ,CMD_1},
  { "char_series", 0, CHAR_SERIES_CMD ,   CMD_1},
  { "charstr",     0, CHARSTR_CMD ,       CMD_1},
  { "cleardenom",  0, CONTENT_CMD ,       CMD_1},
  { "close",       0, CLOSE_CMD ,         CMD_1},
  { "coef",        0, COEF_CMD ,          CMD_M},
  { "coeffs",      0, COEFFS_CMD ,        CMD_23},
  { "continue",    0, CONTINUE_CMD ,      CONTINUE_CMD},
  { "contract",    0, CONTRACT_CMD ,      CMD_2},
  { "convhull",    0, NEWTONPOLY_CMD,     CMD_1},
  { "dbprint",     0, DBPRINT_CMD ,       CMD_M},
  { "def",         0, DEF_CMD ,           ROOT_DECL},
  { "defined",     0, DEFINED_CMD ,       CMD_1},
  { "deg",         0, DEG_CMD ,           CMD_12},
  { "degree",      0, DEGREE_CMD ,        CMD_1},
  { "delete",      0, DELETE_CMD ,        CMD_2},
  { "det",         0, DET_CMD ,           CMD_1},
  { "diff",        0, DIFF_CMD ,          CMD_2},
  { "dim",         0, DIM_CMD ,           CMD_1},
  { "div",         0, INTDIV_CMD ,        MULDIV_OP},
  { "division",    0, DIVISION_CMD ,      CMD_M},
#ifdef DRING
  { "dring",       0, DRING_CMD ,         DRING_CMD},
#endif
  { "dump",        0, DUMP_CMD,           CMD_1},
  { "extgcd",      0, EXTGCD_CMD ,        CMD_2},
  { "EXTGCD",      2, EXTGCD_CMD ,        CMD_2},
  { "ERROR",       0, ERROR_CMD ,         CMD_1},
  { "eliminate",   0, ELIMINATION_CMD,    CMD_23},
  { "else",        0, ELSE_CMD ,          ELSE_CMD},
  { "eval",        0, EVAL ,              EVAL},
  { "example",     0, EXAMPLE_CMD ,       EXAMPLE_CMD},
  { "execute",     0, EXECUTE_CMD ,       CMD_1},
  { "export",      0, EXPORT_CMD ,        EXPORT_CMD},
#ifdef HAVE_NAMESPACES
  { "exportto",    0, EXPORTTO_CMD ,      CMD_M},
#endif
  { "factorize",   0, FAC_CMD ,           CMD_12},
  { "fetch",       0, FETCH_CMD ,         CMD_2},
  { "fglm",        0, FGLM_CMD ,          CMD_2},
  { "fglmquot",    0, FGLMQUOT_CMD,       CMD_2},
  { "find",        0, FIND_CMD ,          CMD_23},
  { "finduni",     0, FINDUNI_CMD,        CMD_1},
  { "forif",       0, IF_CMD ,            IF_CMD},
  { "freemodule",  0, FREEMODULE_CMD ,    CMD_1},
  { "facstd",      0, FACSTD_CMD ,        CMD_12},
  { "gen",         0, E_CMD ,             CMD_1},
  { "getdump",     0, GETDUMP_CMD,        CMD_1},
  { "gcd",         0, GCD_CMD ,           CMD_2},
  { "GCD",         2, GCD_CMD ,           CMD_2},
  { "hilb",        0, HILBERT_CMD ,       CMD_123},
  { "highcorner",  0, HIGHCORNER_CMD,     CMD_1},
  { "homog",       0, HOMOG_CMD ,         CMD_12},
  { "hres",        0, HRES_CMD ,          CMD_2},
  { "ideal",       0, IDEAL_CMD ,         IDEAL_CMD},
  { "if",          0, IF_CMD ,            IF_CMD},
  { "imap",        0, IMAP_CMD ,          CMD_2},
  { "impart",      0, IMPART_CMD ,        CMD_1},
#ifdef HAVE_NAMESPACES
  { "importfrom",  0, IMPORTFROM_CMD ,    CMD_M},
#endif
  { "indepSet",    0, INDEPSET_CMD ,      CMD_12},
  { "insert",      0, INSERT_CMD ,        CMD_23},
  { "int",         0, INT_CMD ,           ROOT_DECL},
  { "interred",    0, INTERRED_CMD ,      CMD_1},
  { "intersect",   0, INTERSECT_CMD ,     CMD_M},
  { "intmat",      0, INTMAT_CMD ,        INTMAT_CMD},
  { "intvec",      0, INTVEC_CMD ,        ROOT_DECL_LIST},
  { "jacob",       0, JACOB_CMD ,         CMD_1},
  { "jet",         0, JET_CMD ,           CMD_M},
  { "kbase",       0, KBASE_CMD ,         CMD_12},
  { "keepring",    0, KEEPRING_CMD ,      KEEPRING_CMD},
  { "kill",        0, KILL_CMD ,          KILL_CMD},
  { "killattrib",  0, KILLATTR_CMD ,      CMD_12},
  { "koszul",      0, KOSZUL_CMD ,        CMD_23},
  { "kres",        0, KRES_CMD ,          CMD_2},
  { "laguerre",    0, LAGSOLVE_CMD,       CMD_3},
  { "lead",        0, LEAD_CMD ,          CMD_1},
  { "leadcoef",    0, LEADCOEF_CMD ,      CMD_1},
  { "leadexp",     0, LEADEXP_CMD ,       CMD_1},
  { "leadmonom",   0, LEADMONOM_CMD ,     CMD_1},
  { "LIB",         0, LIB_CMD ,           SYSVAR},
  { "lift",        0, LIFT_CMD ,          CMD_23},
  { "liftstd",     0, LIFTSTD_CMD ,       CMD_2},
  { "link",        0, LINK_CMD ,          ROOT_DECL},
  { "listvar",     0, LISTVAR_CMD ,       LISTVAR_CMD},
  { "list",        0, LIST_CMD ,          ROOT_DECL_LIST},
  { "load",        0, LOAD_CMD ,          CMD_12},
  { "lres",        0, LRES_CMD ,          CMD_2},
  { "map",         0, MAP_CMD ,           RING_DECL},
  { "matrix",      0, MATRIX_CMD ,        MATRIX_CMD},
  { "maxideal",    0, MAXID_CMD ,         CMD_1},
  { "memory",      0, MEMORY_CMD ,        CMD_1},
  { "minbase",     0, MINBASE_CMD ,       CMD_1},
  { "minor",       0, MINOR_CMD ,         CMD_23},
  { "minres",      0, MINRES_CMD ,        CMD_1},
  { "mod",         0, INTMOD_CMD ,        MULDIV_OP},
  { "module",      0, MODUL_CMD ,         MODUL_CMD},
  { "modulo",      0, MODULO_CMD ,        CMD_2},
  { "monitor",     0, MONITOR_CMD ,       CMD_12},
  { "mpresmat",    0, MPRES_CMD,          CMD_2},
  { "mult",        0, MULTIPLICITY_CMD ,  CMD_1},
  #ifdef OLD_RES
  { "mres",        0, MRES_CMD ,          CMD_23},
  #else
  { "mres",        0, MRES_CMD ,          CMD_2},
  #endif
  { "mstd",        0, MSTD_CMD ,          CMD_1},
  { "nameof",      0, NAMEOF_CMD ,        CMD_1},
  { "names",       0, NAMES_CMD ,         CMD_M},
  { "ncols",       0, COLS_CMD ,          CMD_1},
  { "not",         0, NOT ,               NOT},
  { "npars",       0, NPARS_CMD ,         CMD_1},
  #ifdef OLD_RES
  { "nres",        0, RES_CMD ,           CMD_23},
  #else
  { "nres",        0, RES_CMD ,           CMD_2},
  #endif
  { "nrows",       0, ROWS_CMD ,          CMD_1},
  { "number",      0, NUMBER_CMD ,        RING_DECL},
  { "nvars",       0, NVARS_CMD ,         CMD_1},
  { "open",        0, OPEN_CMD ,          CMD_1},
  { "option",      0, OPTION_CMD ,        CMD_M},
  { "or",          0, '|' ,               LOGIC_OP},
  { "ord",         0, ORD_CMD ,           CMD_1},
  { "ordstr",      0, ORDSTR_CMD ,        CMD_1},
  { "package",     0, PACKAGE_CMD ,       ROOT_DECL},
  { "par",         0, PAR_CMD ,           CMD_1},
  { "parameter",   0, PARAMETER ,         PARAMETER},
  { "pardeg",      0, PARDEG_CMD ,        CMD_1},
  { "parstr",      0, PARSTR_CMD ,        CMD_12},
  { "poly",        0, POLY_CMD ,          RING_DECL},
  { "preimage",    0, PREIMAGE_CMD ,      CMD_13},
  { "prime",       0, PRIME_CMD ,         CMD_1},
  { "print",       0, PRINT_CMD ,         CMD_12},
  { "prune",       0, PRUNE_CMD ,         CMD_1},
  { "proc",        0, PROC_CMD ,          PROC_CMD},
  { "qhweight",    0, QHWEIGHT_CMD ,      CMD_1},
  { "qring",       0, QRING_CMD ,         ROOT_DECL},
  { "quote",       0, QUOTE ,             QUOTE},
  { "quotient",    0, QUOTIENT_CMD ,      CMD_2},
  { "random",      0, RANDOM_CMD ,        CMD_23},
  { "read",        0, READ_CMD ,          CMD_12},
  { "reduce",      0, REDUCE_CMD ,        CMD_M},
  { "regularity",  0, REGULARITY_CMD ,    CMD_1},
  { "repart",      0, REPART_CMD ,        CMD_1},
  { "reservedName",0, RESERVEDNAME_CMD ,  CMD_M},
  { "resolution",  0, RESOLUTION_CMD ,    RING_DECL},
  { "resultant",   0, RESULTANT_CMD,      CMD_3},
  { "return",      0, RETURN ,            RETURN},
  { "RETURN",      0, END_GRAMMAR ,       RETURN},
  { "ring",        0, RING_CMD ,          RING_CMD},
  { "ringlist",    0, RINGLIST_CMD ,      CMD_1},
  { "rvar",        0, IS_RINGVAR ,        CMD_1},
  { "setring",     0, SETRING_CMD ,       SETRING_CMD},
  { "simplex",     0, SIMPLEX_CMD,        CMD_M},
  { "simplify",    0, SIMPLIFY_CMD ,      CMD_2},
  { "size",        0, COUNT_CMD ,         CMD_1},
  { "sortvec",     0, SORTVEC_CMD ,       CMD_1},
  #ifdef OLD_RES
  { "sres",        0, SRES_CMD ,          CMD_23},
  #else
  { "sres",        0, SRES_CMD ,          CMD_2},
  #endif
  { "status",      0, STATUS_CMD,         CMD_M},
  { "std",         0, STD_CMD ,           CMD_123},
  { "string",      0, STRING_CMD ,        ROOT_DECL_LIST},
  { "subst",       0, SUBST_CMD ,         CMD_M},
  { "system",      0, SYSTEM_CMD,         CMD_M},
  { "syz",         0, SYZYGY_CMD ,        CMD_1},
  { "test",        0, TEST_CMD ,          CMD_M},
  { "trace",       0, TRACE_CMD ,         CMD_1},
  { "transpose",   0, TRANSPOSE_CMD ,     CMD_1},
  { "type",        0, TYPE_CMD ,          TYPE_CMD},
  { "typeof",      0, TYPEOF_CMD ,        CMD_1},
#ifdef HAVE_NAMESPACES
  { "unload",      0, UNLOAD_CMD ,        CMD_M},
#endif
  { "uressolve",   0, URSOLVE_CMD,        CMD_M},
  { "vandermonde", 0, VANDER_CMD,         CMD_3},
  { "var",         0, VAR_CMD ,           CMD_1},
  { "varstr",      0, VARSTR_CMD ,        CMD_12},
  { "vdim",        0, VDIM_CMD ,          CMD_1},
  { "vector",      0, VECTOR_CMD ,        RING_DECL},
  { "wedge",       0, WEDGE_CMD ,         CMD_2},
  { "weight",      0, WEIGHT_CMD ,        CMD_1},
  { "whileif",     0, IF_CMD ,            IF_CMD},
  { "write",       0, WRITE_CMD ,         CMD_M},
/* delete for next version:*/
  { "IN",          1, LEAD_CMD ,          CMD_1},
  { "NF",          1, REDUCE_CMD ,        CMD_M},
  { "multiplicity",1, MULTIPLICITY_CMD ,  CMD_1},
  { "verbose",     2, OPTION_CMD ,        CMD_M},
//  { "rank",        1, ROWS_CMD ,          CMD_1},
//  { "Current",     0, -1 ,                SYSVAR},
//  { "Top",         0, -1 ,                SYSVAR},
//  { "Up",          0, -1 ,                SYSVAR},

/* set sys vars*/
//#ifdef SRING
  { "alternating", 0, VALTVARS ,          SYSVAR},
//#endif
  { "degBound",    0, VMAXDEG ,           SYSVAR},
  { "echo",        0, VECHO ,             SYSVAR},
  { "minpoly",     0, VMINPOLY ,          SYSVAR},
  { "multBound",   0, VMAXMULT ,          SYSVAR},
  { "noether",     0, VNOETHER ,          SYSVAR},
  { "pagelength",  0, VPAGELENGTH ,       SYSVAR},
  { "pagewidth",   0, VCOLMAX ,           SYSVAR},
  { "printlevel",  0, VPRINTLEVEL ,       SYSVAR},
  { "short",       0, VSHORTOUT ,         SYSVAR},
  { "timer",       0, VTIMER ,            SYSVAR},
  { "rtimer",      0, VRTIMER,            SYSVAR},
  { "TRACE",       0, TRACE ,             SYSVAR},
  { "voice",       0, VOICE ,             SYSVAR},

/* other reserved words:scanner.l */
  { "pause",       2, -1 ,             0},
  { "while",       0, -1 ,             0},
  { "for",         0, -1 ,             0},
  { "help",        0, -1 ,             0},
  { "newline",     0, -1 ,             0},
  { "exit",        0, -1 ,             0},
  { "quit",        0, -1 ,             0},
/* end of list marker */
  { NULL, 0, 0, 0}
};
#endif

/*=================== operations with 2 args.: static proc =================*/
static BOOLEAN jjOP_IV_I(leftv res, leftv u, leftv v)
{
  intvec* aa= (intvec *)u->CopyD(INTVEC_CMD);
  int bb = (int)(v->Data());
  if (errorreported) return TRUE;
  switch (iiOp)
  {
    case '+': (*aa) += bb; break;
    case '-': (*aa) -= bb; break;
    case '*': (*aa) *= bb; break;
    case '/':
    case INTDIV_CMD: (*aa) /= bb; break;
    case '%':
    case INTMOD_CMD: (*aa) %= bb; break;
  }
  res->data=(char *)aa;
  return FALSE;
}
static BOOLEAN jjOP_I_IV(leftv res, leftv u, leftv v)
{
  return jjOP_IV_I(res,v,u);
}
static BOOLEAN jjOP_IM_I(leftv res, leftv u, leftv v)
{
  intvec* aa= (intvec *)u->CopyD(INTVEC_CMD);
  int bb = (int)(v->Data());
  int i=min(aa->rows(),aa->cols());
  switch (iiOp)
  {
    case '+': for (;i>0;i--) IMATELEM(*aa,i,i) += bb;
              break;
    case '-': for (;i>0;i--) IMATELEM(*aa,i,i) -= bb;
              break;
  }
  res->data=(char *)aa;
  return FALSE;
}
static BOOLEAN jjOP_I_IM(leftv res, leftv u, leftv v)
{
  return jjOP_IM_I(res,v,u);
}
static BOOLEAN jjDOTDOT(leftv res, leftv u, leftv v)
{
  res->data=(char *)new intvec((int)u->Data(),(int)v->Data());
  return FALSE;
}
static void jjEQUAL_REST(leftv res,leftv u,leftv v)
{
  if ((res->data) && (u->next!=NULL) && (v->next!=NULL))
  {
    int save_iiOp=iiOp;
    if (iiOp==NOTEQUAL)
      iiExprArith2(res,u->next,EQUAL_EQUAL,v->next);
    else
      iiExprArith2(res,u->next,iiOp,v->next);
    iiOp=save_iiOp;
  }
  if (iiOp==NOTEQUAL) res->data=(char *)(!(int)res->data);
}
static BOOLEAN jjCOMPARE_IV(leftv res, leftv u, leftv v)
{
  intvec*    a = (intvec * )(u->Data());
  intvec*    b = (intvec * )(v->Data());
  int r=a->compare(b);
  switch  (iiOp)
  {
    case '<':
      res->data  = (char *) (r<0);
      break;
    case '>':
      res->data  = (char *) (r>0);
      break;
    case LE:
      res->data  = (char *) (r<=0);
      break;
    case GE:
      res->data  = (char *) (r>=0);
      break;
    case EQUAL_EQUAL:
    case NOTEQUAL: /* negation handled by jjEQUAL_REST */
      res->data  = (char *) (r==0);
      break;
  }
  jjEQUAL_REST(res,u,v);
  return r==-2;
}
static BOOLEAN jjCOMPARE_IV_I(leftv res, leftv u, leftv v)
{
  intvec* a = (intvec * )(u->Data());
  int     b = (int )     (v->Data());
  int r=a->compare(b);
  switch  (iiOp)
  {
    case '<':
      res->data  = (char *) (r<0);
      break;
    case '>':
      res->data  = (char *) (r>0);
      break;
    case LE:
      res->data  = (char *) (r<=0);
      break;
    case GE:
      res->data  = (char *) (r>=0);
      break;
    case EQUAL_EQUAL:
    case NOTEQUAL: /* negation handled by jjEQUAL_REST */
      res->data  = (char *) (r==0);
      break;
  }
  jjEQUAL_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjCOMPARE_P(leftv res, leftv u, leftv v)
{
  poly p=(poly)u->Data();
  poly q=(poly)v->Data();
  int r=pCmp(p,q);
  if (r==0)
  {
    /* compare lead coeffs */
    number h=nSub(pGetCoeff(p),pGetCoeff(q));
    r = -1+nIsZero(h)+2*nGreaterZero(h); /* -1: <, 0:==, 1: > */
    nDelete(&h);
  }
  else if (p==NULL)
  {
    if (q==NULL)
    {
      /* compare 0, 0 */
      r=0;
    }
    else if(pIsConstant(q))
    {
      /* compare 0, const */
      r = 1-2*nGreaterZero(pGetCoeff(q)); /* -1: <, 1: > */
    }
  }
  else if (q==NULL)
  {
    if (pIsConstant(p))
    {
      /* compare const, 0 */
      r = -1+2*nGreaterZero(pGetCoeff(p)); /* -1: <, 1: > */
    }
  }
  switch  (iiOp)
  {
    case '<':
      res->data  = (char *) (r < 0);
      break;
    case '>':
      res->data  = (char *) (r > 0);
      break;
    case LE:
      res->data  = (char *) (r <= 0);
      break;
    case GE:
      res->data  = (char *) (r >= 0);
      break;
    //case EQUAL_EQUAL:
    //case NOTEQUAL: /* negation handled by jjEQUAL_REST */
    //  res->data  = (char *) (r == 0);
    //  break;
  }
  jjEQUAL_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjCOMPARE_S(leftv res, leftv u, leftv v)
{
  char*    a = (char * )(u->Data());
  char*    b = (char * )(v->Data());
  int result = strcmp(a,b);
  switch  (iiOp)
  {
    case '<':
      res->data  = (char *) (result  < 0);
      break;
    case '>':
      res->data  = (char *) (result  > 0);
      break;
    case LE:
      res->data  = (char *) (result  <= 0);
      break;
    case GE:
      res->data  = (char *) (result  >= 0);
      break;
    case EQUAL_EQUAL:
    case NOTEQUAL: /* negation handled by jjEQUAL_REST */
      res->data  = (char *) (result  == 0);
      break;
  }
  jjEQUAL_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjOP_REST(leftv res, leftv u, leftv v)
{
  if (u->Next()!=NULL)
  {
    u=u->next;
    res->next = (leftv)omAllocBin(sleftv_bin);
    return iiExprArith2(res->next,u,iiOp,v);
  }
  else if (v->Next()!=NULL)
  {
    v=v->next;
    res->next = (leftv)omAllocBin(sleftv_bin);
    return iiExprArith2(res->next,u,iiOp,v);
  }
  return FALSE;
}
static BOOLEAN jjPOWER_I(leftv res, leftv u, leftv v)
{
  int b=(int)u->Data();
  int e=(int)v->Data();
  int rc = 1;
  BOOLEAN overflow=FALSE;
  if (e >= 0)
  {
    if (b==0)
    {
      rc=0;
    }
    else
    {
      int oldrc;
      while ((e--)!=0)
      {
        oldrc=rc;
        rc *= b;
        if (!overflow)
        {
          if(rc/b!=oldrc) overflow=TRUE;
        }
      }
      if (overflow)
        WarnS("int overflow(^), result may be wrong");
    }
    res->data = (char *)rc;
    if (u!=NULL) return jjOP_REST(res,u,v);
    return FALSE;
  }
  else
  {
    WerrorS("exponent must be non-negative");
    return TRUE;
  }
}
static BOOLEAN jjPOWER_N(leftv res, leftv u, leftv v)
{
  int e=(int)v->Data();
  number n=(number)u->CopyD(NUMBER_CMD);
  if (e<0)
  {
    number m=nInvers(n);
    nDelete(&n);
    n=m;
    e=-e;
  }
  nPower(n,e,(number*)&res->data);
  nDelete(&n);
  if (u!=NULL) return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjPOWER_P(leftv res, leftv u, leftv v)
{
  res->data = (char *)pPower((poly)u->CopyD(POLY_CMD),(int)v->Data());
  if (u!=NULL) return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjPOWER_ID(leftv res, leftv u, leftv v)
{
  res->data = (char *)idPower((ideal)(u->Data()),(int)(v->Data()));
  if (u!=NULL) return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjPLUSMINUS_Gen(leftv res, leftv u, leftv v)
{
  u=u->next;
  v=v->next;
  if (u==NULL)
  {
    if (v==NULL) return FALSE;      /* u==NULL, v==NULL */
    if (iiOp=='-')                  /* u==NULL, v<>NULL, iiOp=='-'*/
    {
      do
      {
        if (res->next==NULL)
          res->next = (leftv)omAlloc0Bin(sleftv_bin);
        leftv tmp_v=v->next;
        v->next=NULL;
        BOOLEAN b=iiExprArith1(res->next,v,'-');
        v->next=tmp_v;
        if (b)
          return TRUE;
        v=tmp_v;
        res=res->next;
      } while (v!=NULL);
      return FALSE;
    }
    loop                            /* u==NULL, v<>NULL, iiOp=='+' */
    {
      res->next = (leftv)omAlloc0Bin(sleftv_bin);
      res=res->next;
      res->data = v->CopyD();
      res->rtyp = v->Typ();
      v=v->next;
      if (v==NULL) return FALSE;
    }
  }
  if (v!=NULL)                     /* u<>NULL, v<>NULL */
  {
    do
    {
      res->next = (leftv)omAlloc0Bin(sleftv_bin);
      leftv tmp_u=u->next; u->next=NULL;
      leftv tmp_v=v->next; v->next=NULL;
      BOOLEAN b=iiExprArith2(res->next,u,iiOp,v);
      u->next=tmp_u;
      v->next=tmp_v;
      if (b)
        return TRUE;
      u=tmp_u;
      v=tmp_v;
      res=res->next;
    } while ((u!=NULL) && (v!=NULL));
    return FALSE;
  }
  loop                             /* u<>NULL, v==NULL */
  {
    res->next = (leftv)omAlloc0Bin(sleftv_bin);
    res=res->next;
    res->data = u->CopyD();
    res->rtyp = u->Typ();
    u=u->next;
    if (u==NULL) return FALSE;
  }
}
static BOOLEAN jjCOLCOL(leftv res, leftv u, leftv v)
{
#ifdef HAVE_NAMESPACES
  BOOLEAN iiReLoadLib(idhdl packhdl);
  BOOLEAN iiTryLoadLib(leftv v, char *id);
  idhdl packhdl;

  switch(u->Typ())
  {
      case 0:
        //Print("%s of type 'ANY'. Trying load.\n", v->name);
        if(iiTryLoadLib(u, u->name))
        {
          Werror("'%s' no such package", u->name);
          return TRUE;
        }
        syMake(u,u->name,NULL);
        // else: use next case !!! no break !!!

      case PACKAGE_CMD:
        packhdl = (idhdl)u->data;
        if(!IDPACKAGE(packhdl)->loaded)
        {
          //if(iiReLoadLib(packhdl))
          //  Werror("unable to reload package '%s'", IDID(packhdl));
        }
        if(v->rtyp == IDHDL)
        {
          v->name = omStrDup(v->name);
        }
        namespaceroot->push( IDPACKAGE(packhdl), IDID(packhdl));
        syMake(v, v->name, packhdl);
        memcpy(res, v, sizeof(sleftv));
        memset(v, 0, sizeof(sleftv));
        namespaceroot->pop();
        break;

      case DEF_CMD:
        break;

      default:
        WerrorS("<package>::<id> expected");
        return TRUE;
  }
#else /* HAVE_NAMESPACES */
#ifdef HAVE_NS
  idhdl packhdl;

  switch(u->Typ())
  {
      case 0:
        Print("%s of type 'ANY'. Trying load.\n", v->name);
        //if(iiTryLoadLib(u, u->name))
        {
          Werror("'%s' no such package", u->name);
          return TRUE;
        }
        //syMake(u,u->name,NULL);
        // else: use next case !!! no break !!!

      case PACKAGE_CMD:
        packhdl = (idhdl)u->data;
        if((!IDPACKAGE(packhdl)->loaded)
        && (IDPACKAGE(packhdl)->language > LANG_TOP))
        {
          //if(iiReLoadLib(packhdl))
          //  Werror("unable to reload package '%s'", IDID(packhdl));
          Werror("'%s' not loaded", u->name);
          return TRUE;
        }
        if(v->rtyp == IDHDL)
        {
          v->name = omStrDup(v->name);
        }
        syMake(v, v->name, packhdl);
        memcpy(res, v, sizeof(sleftv));
        memset(v, 0, sizeof(sleftv));
        break;

      case DEF_CMD:
        break;

      default:
        WerrorS("<package>::<id> expected");
        return TRUE;
  }
#else
  WerrorS("package is not supported in this version");
#endif /* HAVE_NS */
#endif /* HAVE_NAMESPACES */
  return FALSE;
}
static BOOLEAN jjPLUS_I(leftv res, leftv u, leftv v)
{
  unsigned int a=(unsigned int)u->Data();
  unsigned int b=(unsigned int)v->Data();
  unsigned int c=a+b;
  res->data = (char *)c;
  if (((Sy_bit(31)&a)==(Sy_bit(31)&b))&&((Sy_bit(31)&a)!=(Sy_bit(31)&c)))
  {
    WarnS("int overflow(+), result may be wrong");
  }
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjPLUS_N(leftv res, leftv u, leftv v)
{
  res->data = (char *)(nAdd((number)u->Data(), (number)v->Data()));
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjPLUS_P(leftv res, leftv u, leftv v)
{
  res->data = (char *)(pAdd((poly)u->CopyD(POLY_CMD) , (poly)v->CopyD(POLY_CMD)));
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjPLUS_IV(leftv res, leftv u, leftv v)
{
  res->data = (char *)ivAdd((intvec*)(u->Data()), (intvec*)(v->Data()));
  if (res->data==NULL)
  {
     WerrorS("intmat size not compatible");
     return TRUE;
  }
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjPLUS_MA(leftv res, leftv u, leftv v)
{
  res->data = (char *)(mpAdd((matrix)u->Data() , (matrix)v->Data()));
  if (res->data==NULL)
  {
     WerrorS("matrix size not compatible");
     return TRUE;
  }
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjPLUS_MA_P(leftv res, leftv u, leftv v)
{
  matrix m=(matrix)u->Data();
  matrix p= mpInitP(m->nrows,m->ncols,(poly)(v->CopyD(POLY_CMD)));
  if (iiOp=='+')
    res->data = (char *)mpAdd(m , p);
  else
    res->data = (char *)mpSub(m , p);
  idDelete((ideal *)&p);
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjPLUS_P_MA(leftv res, leftv u, leftv v)
{
  return jjPLUS_MA_P(res,v,u);
}
static BOOLEAN jjPLUS_S(leftv res, leftv u, leftv v)
{
  char*    a = (char * )(u->Data());
  char*    b = (char * )(v->Data());
  char*    r = (char * )omAlloc(strlen(a) + strlen(b) + 1);
  strcpy(r,a);
  strcat(r,b);
  res->data=r;
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjPLUS_ID(leftv res, leftv u, leftv v)
{
  res->data = (char *)idAdd((ideal)u->Data(),(ideal)v->Data());
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjMINUS_I(leftv res, leftv u, leftv v)
{
  unsigned int a=(unsigned int)u->Data();
  unsigned int b=(unsigned int)v->Data();
  unsigned int c=a-b;
  if (((Sy_bit(31)&a)!=(Sy_bit(31)&b))&&((Sy_bit(31)&a)!=(Sy_bit(31)&c)))
  {
    WarnS("int overflow(-), result may be wrong");
  }
  res->data = (char *)c;
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjMINUS_N(leftv res, leftv u, leftv v)
{
  res->data = (char *)(nSub((number)u->Data(), (number)v->Data()));
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjMINUS_P(leftv res, leftv u, leftv v)
{
  res->data = (char *)(pSub((poly)u->CopyD(POLY_CMD) , (poly)v->CopyD(POLY_CMD)));
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjMINUS_IV(leftv res, leftv u, leftv v)
{
  res->data = (char *)ivSub((intvec*)(u->Data()), (intvec*)(v->Data()));
  if (res->data==NULL)
  {
     WerrorS("intmat size not compatible");
     return TRUE;
  }
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjMINUS_MA(leftv res, leftv u, leftv v)
{
  res->data = (char *)(mpSub((matrix)u->Data() , (matrix)v->Data()));
  if (res->data==NULL)
  {
     WerrorS("matrix size not compatible");
     return TRUE;
  }
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjTIMES_I(leftv res, leftv u, leftv v)
{
  int a=(int)u->Data();
  int b=(int)v->Data();
  int c=a * b;
  if ((b!=0) && (c/b !=a))
    WarnS("int overflow(*), result may be wrong");
  res->data = (char *)c;
  if ((u->Next()!=NULL) || (v->Next()!=NULL))
    return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjTIMES_N(leftv res, leftv u, leftv v)
{
  res->data = (char *)(nMult( (number)u->Data(), (number)v->Data()));
  if ((v->next!=NULL) || (u->next!=NULL))
    return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjTIMES_P(leftv res, leftv u, leftv v)
{
  poly a;
  poly b;

  if (v->next==NULL)
  {
    a=(poly)u->CopyD(POLY_CMD); // works also for VECTOR_CMD
    if (u->next==NULL)
    {
      b=(poly)v->CopyD(POLY_CMD); // works also for VECTOR_CMD
      res->data = (char *)(pMult( a, b));
      return FALSE;
    }
    // u->next exists: copy v
    b=pCopy((poly)v->Data());
    res->data = (char *)(pMult( a, b));
    return jjOP_REST(res,u,v);
  }
  // v->next exists: copy u
  a=pCopy((poly)u->Data());
  b=(poly)v->CopyD(POLY_CMD); // works also for VECTOR_CMD
  res->data = (char *)(pMult( a, b));
  return jjOP_REST(res,u,v);
}
static BOOLEAN jjTIMES_ID(leftv res, leftv u, leftv v)
{
  res->data = (char *)idMult((ideal)u->Data(),(ideal)v->Data());
  if ((v->next!=NULL) || (u->next!=NULL))
    return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjTIMES_IV(leftv res, leftv u, leftv v)
{
  res->data = (char *)ivMult((intvec*)(u->Data()), (intvec*)(v->Data()));
  if (res->data==NULL)
  {
     WerrorS("intmat size not compatible");
     return TRUE;
  }
  if ((v->next!=NULL) || (u->next!=NULL))
    return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjTIMES_MA_P1(leftv res, leftv u, leftv v)
{
  poly p=(poly)v->CopyD(POLY_CMD);
  int r=pMaxComp(p);/* recompute the rank for the case ideal*vector*/
  ideal I= (ideal)mpMultP((matrix)u->CopyD(MATRIX_CMD),p);
  if (r>0) I->rank=r;
  res->data = (char *)I;
  return FALSE;
}
static BOOLEAN jjTIMES_MA_P2(leftv res, leftv u, leftv v)
{
  return jjTIMES_MA_P1(res,v,u);
}
static BOOLEAN jjTIMES_MA_N1(leftv res, leftv u, leftv v)
{
  number n=(number)v->CopyD(NUMBER_CMD);
  poly p=pOne();
  pSetCoeff(p,n);
  res->data = (char *)mpMultP((matrix)u->CopyD(MATRIX_CMD),p);
  return FALSE;
}
static BOOLEAN jjTIMES_MA_N2(leftv res, leftv u, leftv v)
{
  return jjTIMES_MA_N1(res,v,u);
}
static BOOLEAN jjTIMES_MA_I1(leftv res, leftv u, leftv v)
{
  res->data = (char *)mpMultI((matrix)u->CopyD(MATRIX_CMD),(int)v->Data());
  return FALSE;
}
static BOOLEAN jjTIMES_MA_I2(leftv res, leftv u, leftv v)
{
  return jjTIMES_MA_I1(res,v,u);
}
static BOOLEAN jjTIMES_MA(leftv res, leftv u, leftv v)
{
  res->data = (char *)mpMult((matrix)u->Data(),(matrix)v->Data());
  if (res->data==NULL)
  {
     WerrorS("matrix size not compatible");
     return TRUE;
  }
  if ((v->next!=NULL) || (u->next!=NULL))
    return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjGE_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)((int)u->Data() >= (int)v->Data());
  return FALSE;
}
static BOOLEAN jjGE_N(leftv res, leftv u, leftv v)
{
  number h=nSub((number)u->Data(),(number)v->Data());
  res->data = (char *) (nGreaterZero(h)||(nIsZero(h)));
  nDelete(&h);
  return FALSE;
}
static BOOLEAN jjGT_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)((int)u->Data()>(int)v->Data());
  return FALSE;
}
static BOOLEAN jjGT_N(leftv res, leftv u, leftv v)
{
  number h=nSub((number)u->Data(),(number)v->Data());
  res->data = (char *) (nGreaterZero(h)&&(!nIsZero(h)));
  nDelete(&h);
  return FALSE;
}
static BOOLEAN jjLE_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)((int)u->Data() <= (int)v->Data());
  return FALSE;
}
static BOOLEAN jjLE_N(leftv res, leftv u, leftv v)
{
  number h=nSub((number)v->Data(),(number)u->Data());
  res->data = (char *) (nGreaterZero(h)||nIsZero(h));
  nDelete(&h);
  return FALSE;
}
static BOOLEAN jjLT_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)((int)u->Data() < (int)v->Data());
  return FALSE;
}
static BOOLEAN jjLT_N(leftv res, leftv u, leftv v)
{
  number h=nSub((number)v->Data(),(number)u->Data());
  res->data = (char *) (nGreaterZero(h)&&(!nIsZero(h)));
  nDelete(&h);
  return FALSE;
}
static BOOLEAN jjDIVMOD_I(leftv res, leftv u, leftv v)
{
  int a= (int) u->Data();
  int b= (int) v->Data();
  if (b==0)
  {
    WerrorS(ii_div_by_0);
    return TRUE;
  }
  int bb=ABS(b);
  int c=a%bb;
  if(c<0) c+=bb;
  int r=0;
  switch (iiOp)
  {
    case INTMOD_CMD:
        r=c;            break;
    case '%':
        r= (a % b);     break;
    case INTDIV_CMD:
        r=((a-c) /b);   break;
    case '/':
        r= (a / b);     break;
  }
  res->data=(void *)r;
  return FALSE;
}
static BOOLEAN jjDIV_N(leftv res, leftv u, leftv v)
{
  number q=(number)v->Data();
  if (nIsZero(q))
  {
    WerrorS(ii_div_by_0);
    return TRUE;
  }
  q = nDiv((number)u->Data(),q);
  nNormalize(q);
  res->data = (char *)q;
  return FALSE;
}
static BOOLEAN jjDIV_P(leftv res, leftv u, leftv v)
{
  poly q=(poly)v->Data();
  if (q==NULL)
  {
    WerrorS(ii_div_by_0);
    return TRUE;
  }
  poly p=(poly)(u->Data());
  if (p==NULL)
  {
    res->data=NULL;
    return FALSE;
  }
  if (pNext(q)!=NULL)
  {
#ifdef HAVE_FACTORY
    if(pGetComp(p)==0)
    {
      res->data=(void*)(singclap_pdivide(p /*(poly)(u->Data())*/ ,
                                         q /*(poly)(v->Data())*/ ));
    }
    else
    {
      int comps=pMaxComp(p);
      ideal I=idInit(comps,1);
      p=pCopy(p);
      poly h;
      int i;
      // conversion to a list of polys:
      while (p!=NULL)
      {
        i=pGetComp(p)-1;
        h=pNext(p);
        pNext(p)=NULL;
        pSetComp(p,0);
        I->m[i]=pAdd(I->m[i],p);
        p=h;
      }
      // division and conversion to vector:
      h=NULL;
      p=NULL;
      for(i=comps-1;i>=0;i--)
      {
        if (I->m[i]!=NULL)
        {
          h=singclap_pdivide(I->m[i],q);
          pSetCompP(h,i+1);
          p=pAdd(p,h);
        }
      }
      idDelete(&I);
      res->data=(void *)p;
    }
#else
    WerrorS("division only by a monomial");
    return TRUE;
#endif
  }
  else
  {
    res->data = (char *)pDivideM(pCopy(p),pHead(q));
  }
  return FALSE;
}
static BOOLEAN jjDIV_Ma(leftv res, leftv u, leftv v)
{
  poly q=(poly)v->Data();
  if (q==NULL)
  {
    WerrorS(ii_div_by_0);
    return TRUE;
  }
  matrix m=(matrix)(u->Data());
  int r=m->rows();
  int c=m->cols();
  matrix mm=mpNew(r,c);
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      if (pNext(q)!=NULL)
      {
      #ifdef HAVE_FACTORY
        MATELEM(mm,i,j) = singclap_pdivide( MATELEM(m,i,j) ,
                                           q /*(poly)(v->Data())*/ );
      #else
        WerrorS("division only by a monomial");
        return TRUE;
      #endif
      }
      else
        MATELEM(mm,i,j) = pDivideM(pCopy(MATELEM(m,i,j)),pHead(q));
    }
  }
  res->data=(char *)mm;
  return FALSE;
}
static BOOLEAN jjEQUAL_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)((int)u->Data() == (int)v->Data());
  jjEQUAL_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjEQUAL_Ma(leftv res, leftv u, leftv v)
{
  res->data = (char *)mpEqual((matrix)u->Data(),(matrix)v->Data());
  jjEQUAL_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjEQUAL_N(leftv res, leftv u, leftv v)
{
  res->data = (char *)nEqual((number)u->Data(),(number)v->Data());
  jjEQUAL_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjEQUAL_P(leftv res, leftv u, leftv v)
{
  poly p=(poly)u->Data();
  poly q=(poly)v->Data();
  if (p==NULL)
  {
    res->data=(char *)(q==NULL);
  }
  else
  {
    if (q==NULL) res->data=(char *)FALSE;
    else
    {
      int r=pCmp(p,q);
      if (r==0)
      {
        p=pSub(pCopy(p),pCopy(q));
        res->data = (char *) (p==NULL);
        pDelete(&p);
      }
      else
        res->data = (char *) FALSE;
    }
  }
  jjEQUAL_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjAND_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)((int)u->Data() && (int)v->Data());
  return FALSE;
}
static BOOLEAN jjOR_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)((int)u->Data() || (int)v->Data());
  return FALSE;
}
static BOOLEAN jjINDEX_I(leftv res, leftv u, leftv v)
{
  res->rtyp=u->rtyp; u->rtyp=0;
  res->data=u->data; u->data=NULL;
  res->name=u->name; u->name=NULL;
#ifdef HAVE_NAMESPACES
  res->packhdl=u->packhdl; u->packhdl=NULL;
  res->req_packhdl=u->req_packhdl; u->req_packhdl=NULL;
#endif /* HAVE_NAMESPACES */
  res->e=u->e;       u->e=NULL;
  if (res->e==NULL) res->e=jjMakeSub(v);
  else
  {
    Subexpr sh=res->e;
    while (sh->next != NULL) sh=sh->next;
    sh->next=jjMakeSub(v);
  }
  return FALSE;
}
static BOOLEAN jjINDEX_IV(leftv res, leftv u, leftv v)
{
  if ((u->rtyp!=IDHDL)||(u->e!=NULL))
  {
    WerrorS("indexed object must have a name");
    return TRUE;
  }
  intvec * iv=(intvec *)v->Data();
  leftv p=NULL;
  int i;
  sleftv t;

  memset(&t,0,sizeof(t));
  t.rtyp=INT_CMD;
  for (i=0;i<iv->length(); i++)
  {
    t.data=(char *)(*iv)[i];
    if (p==NULL)
    {
      p=res;
    }
    else
    {
      p->next=(leftv)omAlloc0Bin(sleftv_bin);
      p=p->next;
    }
    p->rtyp=IDHDL;
    p->data=u->data;
    p->name=u->name;
#ifdef HAVE_NAMESPACES
    p->packhdl=u->packhdl;
#endif /* HAVE_NAMESPACES */
    p->flag|=u->flag;
    p->e=jjMakeSub(&t);
  }
  u->rtyp=0;
  u->data=NULL;
  u->name=NULL;
  return FALSE;
}
static BOOLEAN jjINDEX_P(leftv res, leftv u, leftv v)
{
  poly p=(poly)u->Data();
  int i=(int)v->Data();
  int j=0;
  while (p!=NULL)
  {
    j++;
    if (j==i)
    {
      res->data=(char *)pHead(p);
      return FALSE;
    }
    pIter(p);
  }
  return FALSE;
}
static BOOLEAN jjINDEX_P_IV(leftv res, leftv u, leftv v)
{
  poly p=(poly)u->Data();
  poly r=NULL;
  intvec *iv=(intvec *)v->CopyD(INTVEC_CMD);
  int i;
  int sum=0;
  for(i=iv->length()-1;i>=0;i--)
    sum+=(*iv)[i];
  int j=0;
  while ((p!=NULL) && (sum>0))
  {
    j++;
    for(i=iv->length()-1;i>=0;i--)
    {
      if (j==(*iv)[i])
      {
        r=pAdd(r,pHead(p));
        sum-=j;
        (*iv)[i]=0;
        break;
      }
    }
    pIter(p);
  }
  delete iv;
  res->data=(char *)r;
  return FALSE;
}
static BOOLEAN jjINDEX_V(leftv res, leftv u, leftv v)
{
  poly p=(poly)u->CopyD(VECTOR_CMD);
  poly r=p; // pointer to the beginning of component i
  poly o=NULL;
  int i=(int)v->Data();
  while (p!=NULL)
  {
    if (pGetComp(p)!=i)
    {
      if (r==p) r=pNext(p);
      if (o!=NULL)
      {
        pDeleteLm(&pNext(o));
        p=pNext(o);
      }
      else
        pDeleteLm(&p);
    }
    else
    {
      pSetComp(p, 0);
      o=p;
      p=pNext(o);
    }
  }
  res->data=(char *)r;
  return FALSE;
}
static BOOLEAN jjINDEX_V_IV(leftv res, leftv u, leftv v)
{
  poly p=(poly)u->CopyD(VECTOR_CMD);
  if (p!=NULL)
  {
    poly r=pOne();
    poly hp=r;
    intvec *iv=(intvec *)v->Data();
    int i;
    loop
    {
      for(i=0;i<iv->length();i++)
      {
        if (pGetComp(p)==(*iv)[i])
        {
          poly h;
          pSplit(p,&h);
          pNext(hp)=p;
          p=h;
          pIter(hp);
          break;
        }
      }
      if (p==NULL) break;
      if (i==iv->length())
      {
        pDeleteLm(&p);
        if (p==NULL) break;
      }
    }
    pDeleteLm(&r);
    res->data=(char *)r;
  }
  return FALSE;
}
static BOOLEAN jjKLAMMER(leftv res, leftv u, leftv v)
{
  if(u->name==NULL) return TRUE;
  char * nn = (char *)omAlloc(strlen(u->name) + 14);
  sprintf(nn,"%s(%d)",u->name,(int)v->Data());
  omFree((ADDRESS)u->name);
  u->name=NULL;
  char *n=omStrDup(nn);
  omFree((ADDRESS)nn);
#ifdef HAVE_NAMESPACES
  if(u->req_packhdl != NULL)
  {
    namespaceroot->push( IDPACKAGE(u->req_packhdl), IDID(u->req_packhdl));
    syMake(res,n);
    namespaceroot->pop();
  }
  else
#endif /* HAVE_NAMESPACES */
    syMake(res,n);
  return FALSE;
}
static BOOLEAN jjKLAMMER_IV(leftv res, leftv u, leftv v)
{
  intvec * iv=(intvec *)v->Data();
  leftv p=NULL;
  int i;
  int slen = strlen(u->name) + 14;
  char *n = (char*) omAlloc(slen);
#ifdef HAVE_NAMESPACES
  BOOLEAN needpop=FALSE;

  if(u->req_packhdl != NULL)
  {
    namespaceroot->push( IDPACKAGE(u->req_packhdl), IDID(u->req_packhdl));
    needpop = TRUE;
  }
#endif /* HAVE_NAMESPACES */

  for (i=0;i<iv->length(); i++)
  {
    if (p==NULL)
    {
      p=res;
    }
    else
    {
      p->next=(leftv)omAlloc0Bin(sleftv_bin);
      p=p->next;
    }
    sprintf(n,"%s(%d)",u->name,(*iv)[i]);
    syMake(p,omStrDup(n));
  }
  omFree((ADDRESS)u->name);
  u->name = NULL;
  omFreeSize(n, slen);
#ifdef HAVE_NAMESPACES
  if(needpop) namespaceroot->pop();
#endif /* HAVE_NAMESPACES */
  return FALSE;
}
static BOOLEAN jjPROC(leftv res, leftv u, leftv v)
{
#ifdef HAVE_NAMESPACES
  leftv sl = iiMake_proc((idhdl)u->data,u,v);
#else /* HAVE_NAMESPACES */
#ifdef HAVE_NS
  leftv sl;
  if (u->req_packhdl==currPack)
    sl = iiMake_proc((idhdl)u->data,NULL,v);
  else
    sl = iiMake_proc((idhdl)u->data,u->req_packhdl,v);
#else /* HAVE_NS */
  leftv sl = iiMake_proc((idhdl)u->data,v);
#endif /* HAVE_NS */
#endif /* HAVE_NAMESPACES */
  if (sl==NULL)
  {
    return TRUE;
  }
  else
  {
    memcpy(res,sl,sizeof(sleftv));
  }
  return FALSE;
}
static BOOLEAN jjMAP(leftv res, leftv u, leftv v)
{
  //Print("try to map %s with %s\n",$3.Name(),$1.Name());
  leftv sl=NULL;
  if ((v->e==NULL)&&(v->name!=NULL))
  {
    map m=(map)u->Data();
    sl=iiMap(m,v->name);
  }
  else
  {
    Werror("%s(<name>) expected",u->Name());
  }
  if (sl==NULL) return TRUE;
  memcpy(res,sl,sizeof(sleftv));
  omFreeBin((ADDRESS)sl, sleftv_bin);
  return FALSE;
}
static BOOLEAN jjCALL2MANY(leftv res, leftv u, leftv v)
{
  u->next=(leftv)omAllocBin(sleftv_bin);
  memcpy(u->next,v,sizeof(sleftv));
  BOOLEAN r=iiExprArithM(res,u,iiOp);
  // iiExprArithM did the CleanUp
  return r;
}
static BOOLEAN jjCOEF(leftv res, leftv u, leftv v)
{
  res->data=(char *)mpCoeffProc((poly)u->Data(),(poly)v->Data());
  return FALSE;
}
static BOOLEAN jjCOEFFS_Id(leftv res, leftv u, leftv v)
{
  int i=pVar((poly)v->Data());
  if (i==0)
  {
    WerrorS("ringvar expected");
    return TRUE;
  }
  res->data=(char *)mpCoeffs((ideal)u->CopyD(),i);
  return FALSE;
}
static BOOLEAN jjCOEFFS2_KB(leftv res, leftv u, leftv v)
{
  poly p = pInit();
  int i;

  for (i=1; i<=pVariables; i++)
  {
    pSetExp(p, i, 1);
  }
  res->data = (void*)idCoeffOfKBase((ideal)(u->Data()),
                                    (ideal)(v->Data()), p);
  pDelete(&p);
  return FALSE;
}
static BOOLEAN jjCONTRACT(leftv res, leftv u, leftv v)
{
  res->data=(char *)idDiffOp((ideal)u->Data(),(ideal)v->Data(),FALSE);
  return FALSE;
}
static BOOLEAN jjDEG_IV(leftv res, leftv u, leftv v)
{
  short *iv=iv2array((intvec *)v->Data());
  res->data = (char *)pDegW((poly)u->Data(),iv);
  omFreeSize((ADDRESS)iv,(pVariables+1)*sizeof(short));
  return FALSE;
}
static BOOLEAN jjDIFF_P(leftv res, leftv u, leftv v)
{
  int i=pVar((poly)v->Data());
  if (i==0)
  {
    WerrorS("ringvar expected");
    return TRUE;
  }
  res->data=(char *)pDiff((poly)(u->Data()),i);
  return FALSE;
}
static BOOLEAN jjDIFF_ID(leftv res, leftv u, leftv v)
{
  int i=pVar((poly)v->Data());
  if (i==0)
  {
    WerrorS("ringvar expected");
    return TRUE;
  }
  res->data=(char *)idDiff((matrix)(u->Data()),i);
  return FALSE;
}
static BOOLEAN jjDIFF_ID_ID(leftv res, leftv u, leftv v)
{
  res->data=(char *)idDiffOp((ideal)u->Data(),(ideal)v->Data());
  return FALSE;
}

static BOOLEAN jjELIMIN(leftv res, leftv u, leftv v)
{
  res->data=(char *)idElimination((ideal)u->Data(),(poly)v->Data());
  setFlag(res,FLAG_STD);
  return FALSE;
}
static BOOLEAN jjERROR(leftv res, leftv u)
{
  WerrorS((char *)u->Data());
  return TRUE;
}
static BOOLEAN jjDIM2(leftv res, leftv v, leftv w)
{
  assumeStdFlag(v);
  if(currQuotient==NULL)
    res->data = (char *)scDimInt((ideal)(v->Data()),(ideal)w->Data());
  else
  {
    ideal q=idSimpleAdd(currQuotient,(ideal)w->Data());
    res->data = (char *)scDimInt((ideal)(v->Data()),q);
    idDelete(&q);
  }
  return FALSE;
}
static BOOLEAN jjDIVISION(leftv res, leftv u, leftv v)
{
  ideal vi=(ideal)v->Data();
  int vl= IDELEMS(vi);
  ideal ui=(ideal)u->Data();
  int ul= IDELEMS(ui);
  ideal R; matrix U;
  ideal m = idLift(vi,ui,&R, FALSE,hasFlag(v,FLAG_STD),TRUE,&U);
  // now make sure that all matices have the corect size:
  matrix T = idModule2formatedMatrix(m,vl,ul);
  if (MATCOLS(U) != ul)
  {
    int mul=min(ul,MATCOLS(U));
    matrix UU=mpNew(ul,ul);
    int i,j;
    for(i=mul;i>0;i--)
    {
      for(j=mul;j>0;j--)
      {
        MATELEM(UU,i,j)=MATELEM(U,i,j);
        MATELEM(U,i,j)=NULL;
      }
    }
    idDelete((ideal *)&U);
    U=UU;
  }
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(3);
  L->m[0].rtyp=MATRIX_CMD;   L->m[0].data=(void *)T;
  L->m[1].rtyp=MATRIX_CMD;   L->m[1].data=(void *)U;
  L->m[2].rtyp=u->Typ();     L->m[2].data=(void *)R;
  res->data=(char *)L;
  return FALSE;
}
static BOOLEAN jjEXTGCD_I(leftv res, leftv u, leftv v)
{
  int p0=ABS((int)u->Data()),p1=ABS((int)v->Data());
  int f0 = 1, f1 = 0, g0 = 0, g1 = 1, q, r;

  while ( p1!=0 )
  {
    q=p0 / p1;
    r=p0 % p1;
    p0 = p1; p1 = r;
    r = g0 - g1 * q;
    g0 = g1; g1 = r;
    r = f0 - f1 * q;
    f0 = f1; f1 = r;
  }
  int a = f0;
  int b = g0;
  if ( (int)u->Data() < 0 ) a=-a;
  if ( (int)v->Data() < 0 ) b=-b;
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(3);
  L->m[0].rtyp=INT_CMD;   L->m[0].data=(void *)p0;
  L->m[1].rtyp=INT_CMD;   L->m[1].data=(void *)a;
  L->m[2].rtyp=INT_CMD;   L->m[2].data=(void *)b;
  res->rtyp=LIST_CMD;
  res->data=(char *)L;
  return FALSE;
}
#ifdef HAVE_FACTORY
static BOOLEAN jjFACSTD2(leftv res, leftv v, leftv w)
{
  res->data=(void *)kStdfac((ideal)v->Data(),NULL,testHomog,NULL,
           (ideal)w->Data());
  setFlag(res,FLAG_STD);
  return FALSE;
}
#endif
static BOOLEAN jjFETCH(leftv res, leftv u, leftv v)
{
  ring r=(ring)u->Data();
  idhdl w;
  int op=iiOp;
  nMapFunc nMap;

  if ((w=r->idroot->get(v->Name(),myynest))!=NULL)
  {
    int *perm=NULL;
    int *par_perm=NULL;
    int par_perm_size=0;
    BOOLEAN bo;
    //if (!nSetMap(rInternalChar(r),r->parameter,rPar(r),r->minpoly))
    if ((nMap=nSetMap(r))==NULL)
    {
      if (rEqual(r,currRing))
      {
        nMap=nCopy;
      }
      else
      // Allow imap/fetch to be make an exception only for:
      if ( (rField_is_Q_a(r) &&  // Q(a..) -> Q(a..) || Q || Zp || Zp(a)
            (rField_is_Q() || rField_is_Q_a() ||
             (rField_is_Zp() || rField_is_Zp_a())))
           ||
           (rField_is_Zp_a(r) &&  // Zp(a..) -> Zp(a..) || Zp
            (rField_is_Zp(currRing, rInternalChar(r)) ||
             rField_is_Zp_a(currRing, rInternalChar(r)))) )
      {
        par_perm_size=rPar(r);
        BITSET save_test=test;
        naSetChar(rInternalChar(r),r);
        nSetChar(currRing);
        test=save_test;
      }
      else
      {
        goto err_fetch;
      }
    }
    if ((iiOp!=FETCH_CMD) || (r->N!=pVariables) || (rPar(r)!=rPar(currRing)))
    {
      perm=(int *)omAlloc0((r->N+1)*sizeof(int));
      if (par_perm_size!=0)
        par_perm=(int *)omAlloc0(par_perm_size*sizeof(int));
      op=IMAP_CMD;
      if (iiOp==IMAP_CMD)
      {
        maFindPerm(r->names,       r->N,       r->parameter,        r->P,
                   currRing->names,currRing->N,currRing->parameter, currRing->P,
                   perm,par_perm, currRing->ch);
      }
      else
      {
        int i;
        if (par_perm_size!=0)
          for(i=0;i<min(rPar(r),rPar(currRing));i++) par_perm[i]=-(i+1);
        for(i=1;i<=min(r->N,pVariables);i++) perm[i]=i;
      }
    }
    if ((iiOp==FETCH_CMD) &&(BVERBOSE(V_IMAP)))
    {
      int i;
      for(i=0;i<min(r->N,pVariables);i++)
      {
        Print("// var nr %d: %s -> %s\n",i,r->names[i],currRing->names[i]);
      }
      for(i=0;i<min(rPar(r),rPar(currRing));i++) // possibly empty loop
      {
        Print("// par nr %d: %s -> %s\n",
              i,r->parameter[i],currRing->parameter[i]);
      }
    }
    sleftv tmpW;
    memset(&tmpW,0,sizeof(sleftv));
    tmpW.rtyp=IDTYP(w);
    tmpW.data=IDDATA(w);
    if ((bo=maApplyFetch(op,NULL,res,&tmpW, r,
                         perm,par_perm,par_perm_size,nMap)))
    {
      Werror("cannot map %s of type %s(%d)",v->name, Tok2Cmdname(w->typ),w->typ);
    }
    if (perm!=NULL)
      omFreeSize((ADDRESS)perm,(r->N+1)*sizeof(int));
    if (par_perm!=NULL)
      omFreeSize((ADDRESS)par_perm,par_perm_size*sizeof(int));
    return bo;
  }
  else
  {
    Werror("identifier %s not found in %s",v->Fullname(),u->Fullname());
  }
  return TRUE;
err_fetch:
  Werror("no identity map from %s",u->Fullname());
  return TRUE;
}
static BOOLEAN jjFIND2(leftv res, leftv u, leftv v)
{
  /*4
  * look for the substring what in the string where
  * return the position of the first char of what in where
  * or 0
  */
  char *where=(char *)u->Data();
  char *what=(char *)v->Data();
  char *found = strstr(where,what);
  if (found != NULL)
  {
    res->data=(char *)((found-where)+1);
  }
  /*else res->data=NULL;*/
  return FALSE;
}
static BOOLEAN jjGCD_I(leftv res, leftv u, leftv v)
{
  int p0=ABS((int)u->Data()),p1=ABS((int)v->Data());
  int r;

  while ( p1!=0 )
  {
    r=p0 % p1;
    p0 = p1; p1 = r;
  }
  res->rtyp=INT_CMD;
  res->data=(char *)p0;
  return FALSE;
}
static BOOLEAN jjGCD_N(leftv res, leftv u, leftv v)
{
  res->data=(char *)nGcd((number) u->Data(), (number) v->Data(), currRing);
  return FALSE;
}
static BOOLEAN jjHILBERT2(leftv res, leftv u, leftv v)
{
  assumeStdFlag(u);
  intvec *module_w=(intvec*)atGet(u,"isHomog");
  intvec *iv=hFirstSeries((ideal)u->Data(),module_w,currQuotient);
  switch((int)v->Data())
  {
    case 1:
      res->data=(void *)iv;
      return FALSE;
    case 2:
      res->data=(void *)hSecondSeries(iv);
      delete iv;
      return FALSE;
  }
  WerrorS(feNotImplemented);
  delete iv;
  return TRUE;
}
static BOOLEAN jjHOMOG_P(leftv res, leftv u, leftv v)
{
  int i=pVar((poly)v->Data());
  if (i==0)
  {
    WerrorS("ringvar expected");
    return TRUE;
  }
  res->data = (char *)pHomogen((poly)u->Data(),i);
  return FALSE;
}
static BOOLEAN jjHOMOG_ID(leftv res, leftv u, leftv v)
{
  int i=pVar((poly)v->Data());
  if (i==0)
  {
    WerrorS("ringvar expected");
    return TRUE;
  }
  res->data = (char *)idHomogen((ideal)u->Data(),i);
  return FALSE;
}
static BOOLEAN jjINDEPSET2(leftv res, leftv u, leftv v)
{
  assumeStdFlag(u);
  res->data=(void *)scIndIndset((ideal)(u->Data()),(int)(v->Data()),
                    currQuotient);
  return FALSE;
}
static BOOLEAN jjINTERSECT(leftv res, leftv u, leftv v)
{
  res->data=(char *)idSect((ideal)u->Data(),(ideal)v->Data());
  setFlag(res,FLAG_STD);
  return FALSE;
}
static BOOLEAN jjJET_P(leftv res, leftv u, leftv v)
{
  res->data = (char *)pJet((poly)u->CopyD(), (int)v->Data());
  return FALSE;
}
static BOOLEAN jjJET_ID(leftv res, leftv u, leftv v)
{
  res->data = (char *)idJet((ideal)u->Data(),(int)v->Data());
  return FALSE;
}
static BOOLEAN jjKBASE2(leftv res, leftv u, leftv v)
{
  assumeStdFlag(u);
  res->data = (char *)scKBase((int)v->Data(),
                              (ideal)(u->Data()),currQuotient);
  return FALSE;
}
static BOOLEAN jjKoszul(leftv res, leftv u, leftv v)
{
  return mpKoszul(res, u,v);
}
static BOOLEAN jjKoszul_Id(leftv res, leftv u, leftv v)
{
  sleftv h;
  memset(&h,0,sizeof(sleftv));
  h.rtyp=INT_CMD;
  h.data=(void *)IDELEMS((ideal)v->Data());
  return mpKoszul(res, u, &h, v);
}
static BOOLEAN jjLIFT(leftv res, leftv u, leftv v)
{
  ideal m;
  BITSET save_test=test;
  int ul= IDELEMS((ideal)u->Data());
  int vl= IDELEMS((ideal)v->Data());
  m = idLift((ideal)u->Data(),(ideal)v->Data(),NULL,FALSE,hasFlag(u,FLAG_STD));
  res->data = (char *)idModule2formatedMatrix(m,ul,vl);
  test=save_test;
  return FALSE;
}
static BOOLEAN jjLIFTSTD(leftv res, leftv u, leftv v)
{
  if ((v->rtyp!=IDHDL)||(v->e!=NULL)) return TRUE;
  idhdl h=(idhdl)v->data;
  // CopyD for IDEAL_CMD and MODUL_CMD are identical:
  res->data = (char *)idLiftStd((ideal)u->Data(),
                                &(h->data.umatrix),testHomog);
  setFlag(res,FLAG_STD);
  return FALSE;
}
static BOOLEAN jjMINOR(leftv res, leftv u, leftv v)
{
  res->data = (char *)idMinors((matrix)u->Data(),(int)v->Data());
  return (res->data==NULL);
}
static BOOLEAN jjMODULO(leftv res, leftv u, leftv v)
{
  res->data = (char *)idModulo((ideal)u->Data(),(ideal)v->Data());
  return FALSE;
}
static BOOLEAN jjMONITOR2(leftv res, leftv u,leftv v)
{
  char *opt=(char *)v->Data();
  int mode=0;
  while(*opt!='\0')
  {
    if (*opt=='i') mode |= PROT_I;
    else if (*opt=='o') mode |= PROT_O;
    opt++;
  }
  monitor((char *)(u->Data()),mode);
  return FALSE;
}
static BOOLEAN jjPARSTR2(leftv res, leftv u, leftv v)
{
  idhdl h=(idhdl)u->data;
  int i=(int)v->Data();
  int p=0;
  if ((0<i)
  && (IDRING(h)->parameter!=NULL)
  && (i<=(p=rPar(IDRING(h)))))
    res->data=omStrDup(IDRING(h)->parameter[i-1]);
  else
  {
    Werror("par number %d out of range 1..%d",i,p);
    return TRUE;
  }
  return FALSE;
}
static BOOLEAN jjQUOT(leftv res, leftv u, leftv v)
{
  res->data = (char *)idQuot((ideal)u->Data(),(ideal)v->Data(),
    hasFlag(u,FLAG_STD),u->Typ()==v->Typ());
  idDelMultiples((ideal)(res->data));
  return FALSE;
}
static BOOLEAN jjRANDOM(leftv res, leftv u, leftv v)
{
  int i=(int)u->Data();
  int j=(int)v->Data();
#ifdef buildin_rand
  res->data =(char *)((i > j) ? i : (siRand() % (j-i+1)) + i);
#else
  res->data =(char *)((i > j) ? i : (rand() % (j-i+1)) + i);
#endif
  return FALSE;
}
static BOOLEAN jjREAD2(leftv res, leftv u, leftv v)
{
  si_link l=(si_link)u->Data();
  leftv r=slRead(l,v);
  if (r==NULL)
  {
    const char *s;
    if ((l!=NULL)&&(l->name!=NULL)) s=l->name;
    else                            s=sNoName;
    Werror("cannot read from `%s`",s);
    return TRUE;
  }
  memcpy(res,r,sizeof(sleftv));
  omFreeBin((ADDRESS)r, sleftv_bin);
  return FALSE;
}
static BOOLEAN jjREDUCE_P(leftv res, leftv u, leftv v)
{
  assumeStdFlag(v);
  res->data = (char *)kNF((ideal)v->Data(),currQuotient,(poly)u->Data());
  return FALSE;
}
static BOOLEAN jjREDUCE_ID(leftv res, leftv u, leftv v)
{
  assumeStdFlag(v);
  ideal ui=(ideal)u->Data();
  idTest(ui);
  ideal vi=(ideal)v->Data();
  idTest(vi);
  res->data = (char *)kNF(vi,currQuotient,ui);
  return FALSE;
}
static BOOLEAN jjRES(leftv res, leftv u, leftv v)
{
  int maxl=(int)v->Data();
  if (maxl<0)
  {
    WerrorS("length for res must not be negative");
    return TRUE;
  }
  int l=0;
  //resolvente r;
  syStrategy r;
  intvec **weights=NULL;
  int wmaxl=maxl;
  ideal u_id=(ideal)u->Data();

  maxl--;
  if ((maxl==-1) /*&& (iiOp!=MRES_CMD)*/)
  {
    maxl = pVariables-1+2*(iiOp==MRES_CMD);
    if (currQuotient!=NULL)
    {
      Warn(
      "full resolution in a qring may be infinite, setting max length to %d",
      maxl+1);
    }
  }
  if ((iiOp == RES_CMD) || (iiOp == MRES_CMD))
  {
    intvec * iv=(intvec*)atGet(u,"isHomog");
    r=syResolution(u_id,maxl, iv, iiOp==MRES_CMD);
  }
  else if (iiOp==SRES_CMD)
  //  r=sySchreyerResolvente(u_id,maxl+1,&l);
    r=sySchreyer(u_id,maxl+1);
  else if (iiOp == LRES_CMD)
  {
    int dummy;
    if((currQuotient!=NULL)||
    (!idHomIdeal (u_id,NULL)))
    {
       WerrorS
       ("`lres` not implemented for inhomogeneous input or qring");
       return TRUE;
    }
    r=syLaScala3(u_id,&dummy);
  }
  else if (iiOp == KRES_CMD)
  {
    int dummy;
    if((currQuotient!=NULL)||
    (!idHomIdeal (u_id,NULL)))
    {
       WerrorS
       ("`lres` not implemented for inhomogeneous input or qring");
       return TRUE;
    }
    r=syKosz(u_id,&dummy);
  }
  else
  {
    int dummy;
    if((currQuotient!=NULL)||
    (!idHomIdeal (u_id,NULL)))
    {
       WerrorS
       ("`hres` not implemented for inhomogeneous input or qring");
       return TRUE;
    }
    r=syHilb(u_id,&dummy);
  }
  if (r==NULL) return TRUE;
  //res->data=(void *)liMakeResolv(r,l,wmaxl,u->Typ(),weights);
  r->list_length=wmaxl;
  res->data=(void *)r;
  return FALSE;
}
static BOOLEAN jjRSUM(leftv res, leftv u, leftv v)
{
  ring r;
  int i=rSum((ring)u->Data(),(ring)v->Data(),r);
  res->data = (char *)r;
  return (i==-1);
}
#define SIMPL_LMDIV 32
#define SIMPL_LMEQ  16
#define SIMPL_MULT 8
#define SIMPL_EQU  4
#define SIMPL_NULL 2
#define SIMPL_NORM 1
static BOOLEAN jjSIMPL_ID(leftv res, leftv u, leftv v)
{
  int sw = (int)v->Data();
  // CopyD for IDEAL_CMD and MODUL_CMD are identical:
  ideal id = (ideal)u->CopyD(IDEAL_CMD);
  if (sw & SIMPL_LMDIV)
  {
    idDelDiv(id);
  }
  if (sw & SIMPL_LMEQ)
  {
    idDelLmEquals(id);
  }
  if (sw & SIMPL_NULL)
  {
    idSkipZeroes(id);
  }
  if (sw & SIMPL_NORM)
  {
    idNorm(id);
  }
  if (sw & SIMPL_MULT)
  {
    idDelMultiples(id);
  }
  else if(sw & SIMPL_EQU)
  {
    idDelEquals(id);
  }
  res->data = (char * )id;
  return FALSE;
}
static BOOLEAN jjSTATUS2(leftv res, leftv u, leftv v)
{
  res->data = omStrDup(slStatus((si_link) u->Data(), (char *) v->Data()));
  return FALSE;
}
static BOOLEAN jjSIMPL_P(leftv res, leftv u, leftv v)
{
  int sw = (int)v->Data();
  // CopyD for POLY_CMD and VECTOR_CMD are identical:
  poly p = (poly)u->CopyD(POLY_CMD);
  if (sw & SIMPL_NORM)
  {
    pNorm(p);
  }
  res->data = (char * )p;
  return FALSE;
}
static BOOLEAN jjSTD_HILB(leftv res, leftv u, leftv v)
{
  ideal result;
  intvec *w=(intvec *)atGet(u,"isHomog");
  tHomog hom=testHomog;
  if (w!=NULL)
  {
    w=ivCopy(w);
    hom=isHomog;
  }
  result=kStd((ideal)(u->Data()),currQuotient,hom,&w,(intvec *)v->Data());
  idSkipZeroes(result);
  res->data = (char *)result;
  setFlag(res,FLAG_STD);
  if (w!=NULL) atSet(res,omStrDup("isHomog"),w,INTVEC_CMD);
  return FALSE;
}
static BOOLEAN jjSTD_1(leftv res, leftv u, leftv v)
{
  assumeStdFlag(u);
  ideal result;
  intvec *w=(intvec *)atGet(u,"isHomog");
  tHomog hom=testHomog;
  if (w!=NULL)
  {
    w=ivCopy(w);
    hom=isHomog;
  }
  ideal i1=(ideal)u->Data();
  ideal i0=idInit(1,i1->rank);
  i0->m[0]=(poly)v->Data();
  i1=idSimpleAdd(i1,i0);
  i0->m[0]=NULL;
  idDelete(&i0);
  BITSET save_test=test;
  test|=Sy_bit(OPT_SB_1);
  result=kStd(i1,currQuotient,hom,&w,NULL,0,IDELEMS(i1)-1);
  test=save_test;
  idDelete(&i1);
  idSkipZeroes(result);
  res->data = (char *)result;
  setFlag(res,FLAG_STD);
  if (w!=NULL) atSet(res,omStrDup("isHomog"),w,INTVEC_CMD);
  return FALSE;
}
static BOOLEAN jjVARSTR2(leftv res, leftv u, leftv v)
{
  idhdl h=(idhdl)u->data;
  int i=(int)v->Data();
  if ((0<i) && (i<=IDRING(h)->N))
    res->data=omStrDup(IDRING(h)->names[i-1]);
  else
  {
    Werror("var number %d out of range 1..%d",i,IDRING(h)->N);
    return TRUE;
  }
  return FALSE;
}
static BOOLEAN jjWEDGE(leftv res, leftv u, leftv v)
{
  res->data = (char *)mpWedge((matrix)u->Data(),(int)v->Data());
  return FALSE;
}
#define jjWRONG2 (proc2)jjWRONG
#define jjWRONG3 (proc3)jjWRONG
static BOOLEAN jjWRONG(leftv res, leftv u)
{
  return TRUE;
}

static BOOLEAN jjLOAD_E(leftv res, leftv v, leftv u)
{
  char * s=(char *)u->Data();
  if(strcmp(s, "with")==0)
    return jjLOAD(res, v, TRUE);
  WerrorS("invalid second argument");
  WerrorS("load(\"libname\" [,\"with\"]);");
  return TRUE;
}

/*=================== operations with 2 args.: table =================*/

struct sValCmd2 dArith2[]=
{
// operations:
// proc        cmd              res             arg1        arg2
 {jjCOLCOL,    COLONCOLON,     ANY_TYPE,       DEF_CMD,    DEF_CMD      PROFILER}
,{jjPLUS_I,    '+',            INT_CMD,        INT_CMD,    INT_CMD        PROFILER}
,{jjPLUS_N,    '+',            NUMBER_CMD,     NUMBER_CMD, NUMBER_CMD     PROFILER}
,{jjPLUS_P,    '+',            POLY_CMD,       POLY_CMD,   POLY_CMD PROFILER}
,{jjPLUS_P,    '+',            VECTOR_CMD,     VECTOR_CMD, VECTOR_CMD PROFILER}
,{jjPLUS_ID,   '+',            IDEAL_CMD,      IDEAL_CMD,  IDEAL_CMD PROFILER}
,{jjPLUS_ID,   '+',            MODUL_CMD,      MODUL_CMD,  MODUL_CMD PROFILER}
,{jjPLUS_P_MA, '+',            MATRIX_CMD,     POLY_CMD,   MATRIX_CMD PROFILER}
,{jjPLUS_MA_P, '+',            MATRIX_CMD,     MATRIX_CMD, POLY_CMD PROFILER}
,{jjPLUS_MA,   '+',            MATRIX_CMD,     MATRIX_CMD, MATRIX_CMD PROFILER}
,{jjPLUS_S,    '+',            STRING_CMD,     STRING_CMD, STRING_CMD PROFILER}
,{jjOP_IV_I,   '+',            INTVEC_CMD,     INTVEC_CMD, INT_CMD PROFILER}
,{jjOP_I_IV,   '+',            INTVEC_CMD,     INT_CMD,    INTVEC_CMD PROFILER}
,{jjOP_IM_I,   '+',            INTMAT_CMD,     INTMAT_CMD, INT_CMD PROFILER}
,{jjOP_I_IM,   '+',            INTMAT_CMD,     INT_CMD,    INTMAT_CMD PROFILER}
,{jjPLUS_IV,   '+',            INTVEC_CMD,     INTVEC_CMD, INTVEC_CMD PROFILER}
,{jjPLUS_IV,   '+',            INTMAT_CMD,     INTMAT_CMD, INTMAT_CMD PROFILER}
,{lAdd,        '+',            LIST_CMD,       LIST_CMD,   LIST_CMD PROFILER}
,{jjRSUM,      '+',            RING_CMD,       RING_CMD,   RING_CMD PROFILER}
,{jjMINUS_I,   '-',            INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjMINUS_N,   '-',            NUMBER_CMD,     NUMBER_CMD, NUMBER_CMD PROFILER}
,{jjMINUS_P,   '-',            POLY_CMD,       POLY_CMD,   POLY_CMD PROFILER}
,{jjMINUS_P,   '-',            VECTOR_CMD,     VECTOR_CMD, VECTOR_CMD PROFILER}
,{jjPLUS_MA_P, '-',            MATRIX_CMD,     MATRIX_CMD, POLY_CMD PROFILER}
,{jjMINUS_MA,  '-',            MATRIX_CMD,     MATRIX_CMD, MATRIX_CMD PROFILER}
,{jjOP_IV_I,   '-',            INTVEC_CMD,     INTVEC_CMD, INT_CMD PROFILER}
,{jjOP_IM_I,   '-',            INTMAT_CMD,     INTMAT_CMD, INT_CMD PROFILER}
,{jjMINUS_IV,  '-',            INTVEC_CMD,     INTVEC_CMD, INTVEC_CMD PROFILER}
,{jjMINUS_IV,  '-',            INTMAT_CMD,     INTMAT_CMD, INTMAT_CMD PROFILER}
,{jjTIMES_I,   '*',            INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjTIMES_N,   '*',            NUMBER_CMD,     NUMBER_CMD, NUMBER_CMD PROFILER}
,{jjTIMES_P,   '*',            POLY_CMD,       POLY_CMD,   POLY_CMD PROFILER}
,{jjTIMES_P,   '*',            VECTOR_CMD,     POLY_CMD,   VECTOR_CMD PROFILER}
,{jjTIMES_P,   '*',            VECTOR_CMD,     VECTOR_CMD, POLY_CMD PROFILER}
,{jjTIMES_MA_P1,'*',           IDEAL_CMD,      IDEAL_CMD,  POLY_CMD PROFILER}
,{jjTIMES_MA_P2,'*',           IDEAL_CMD,      POLY_CMD,   IDEAL_CMD PROFILER}
,{jjTIMES_ID,  '*',            IDEAL_CMD,      IDEAL_CMD,  IDEAL_CMD PROFILER}
,{jjTIMES_MA_P1,'*',           MODUL_CMD,      IDEAL_CMD,  VECTOR_CMD PROFILER}
,{jjTIMES_MA_P2,'*',           MODUL_CMD,      VECTOR_CMD, IDEAL_CMD PROFILER}
,{jjTIMES_ID,  '*',            MODUL_CMD,      IDEAL_CMD,  MODUL_CMD PROFILER}
,{jjTIMES_ID,  '*',            MODUL_CMD,      MODUL_CMD,  IDEAL_CMD PROFILER}
,{jjTIMES_MA_P1,'*',           MATRIX_CMD,     MATRIX_CMD, POLY_CMD PROFILER}
,{jjTIMES_MA_P2,'*',           MATRIX_CMD,     POLY_CMD,   MATRIX_CMD PROFILER}
,{jjTIMES_MA_N1,'*',           MATRIX_CMD,     MATRIX_CMD, NUMBER_CMD PROFILER}
,{jjTIMES_MA_N2,'*',           MATRIX_CMD,     NUMBER_CMD, MATRIX_CMD PROFILER}
,{jjTIMES_MA_I1,'*',           MATRIX_CMD,     MATRIX_CMD, INT_CMD PROFILER}
,{jjTIMES_MA_I2,'*',           MATRIX_CMD,     INT_CMD,    MATRIX_CMD PROFILER}
,{jjTIMES_MA,  '*',            MATRIX_CMD,     MATRIX_CMD, MATRIX_CMD PROFILER}
,{jjOP_IV_I,   '*',            INTVEC_CMD,     INTVEC_CMD, INT_CMD PROFILER}
,{jjOP_I_IV,   '*',            INTVEC_CMD,     INT_CMD,    INTVEC_CMD PROFILER}
,{jjOP_IV_I,   '*',            INTMAT_CMD,     INTMAT_CMD, INT_CMD PROFILER}
,{jjOP_I_IV,   '*',            INTMAT_CMD,     INT_CMD,    INTMAT_CMD PROFILER}
,{jjTIMES_IV,  '*',            INTVEC_CMD,     INTMAT_CMD, INTVEC_CMD PROFILER}
,{jjTIMES_IV,  '*',            INTMAT_CMD,     INTMAT_CMD, INTMAT_CMD PROFILER}
,{jjTIMES_IV,  '*',            INTMAT_CMD,     INTVEC_CMD, INTMAT_CMD PROFILER}
,{jjDIV_N,     '/',            NUMBER_CMD,     NUMBER_CMD, NUMBER_CMD PROFILER}
,{jjDIV_P,     '/',            POLY_CMD,       POLY_CMD,   POLY_CMD PROFILER}
,{jjDIV_P,     '/',            VECTOR_CMD,     VECTOR_CMD, POLY_CMD PROFILER}
,{jjDIV_Ma,    '/',            MATRIX_CMD,     MATRIX_CMD, POLY_CMD PROFILER}
,{jjDIVMOD_I,  '/',            INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjOP_IV_I,   '/',            INTVEC_CMD,     INTVEC_CMD, INT_CMD PROFILER}
,{jjOP_IV_I,   '/',            INTMAT_CMD,     INTMAT_CMD, INT_CMD PROFILER}
,{jjDIVMOD_I,  INTDIV_CMD,     INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjOP_IV_I,   INTDIV_CMD,     INTVEC_CMD,     INTVEC_CMD, INT_CMD PROFILER}
,{jjOP_IV_I,   INTDIV_CMD,     INTMAT_CMD,     INTMAT_CMD, INT_CMD PROFILER}
,{jjDIVMOD_I,  '%',            INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjOP_IV_I,   '%',            INTVEC_CMD,     INTVEC_CMD, INT_CMD PROFILER}
,{jjOP_IV_I,   '%',            INTMAT_CMD,     INTMAT_CMD, INT_CMD PROFILER}
,{jjDIVMOD_I,  INTMOD_CMD,     INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjOP_IV_I,   INTMOD_CMD,     INTVEC_CMD,     INTVEC_CMD, INT_CMD PROFILER}
,{jjOP_IV_I,   INTMOD_CMD,     INTMAT_CMD,     INTMAT_CMD, INT_CMD PROFILER}
,{jjPOWER_I,   '^',            INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjPOWER_N,   '^',            NUMBER_CMD,     NUMBER_CMD, INT_CMD PROFILER}
,{jjPOWER_P,   '^',            POLY_CMD,       POLY_CMD,   INT_CMD PROFILER}
,{jjPOWER_ID,  '^',            IDEAL_CMD,      IDEAL_CMD,  INT_CMD PROFILER}
,{jjLE_I,      LE,             INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjLE_N,      LE,             INT_CMD,        NUMBER_CMD, NUMBER_CMD PROFILER}
,{jjCOMPARE_S, LE,             INT_CMD,        STRING_CMD, STRING_CMD PROFILER}
,{jjCOMPARE_IV_I,LE,           INT_CMD,        INTVEC_CMD, INT_CMD PROFILER}
,{jjCOMPARE_IV,LE,             INT_CMD,        INTVEC_CMD, INTVEC_CMD PROFILER}
,{jjCOMPARE_P, LE,             INT_CMD,        POLY_CMD,   POLY_CMD PROFILER}
,{jjCOMPARE_P, LE,             INT_CMD,        VECTOR_CMD, VECTOR_CMD PROFILER}
,{jjLT_I,      '<',            INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjLT_N,      '<',            INT_CMD,        NUMBER_CMD, NUMBER_CMD PROFILER}
,{jjCOMPARE_IV_I,'<',           INT_CMD,        INTVEC_CMD, INT_CMD PROFILER}
,{jjCOMPARE_IV,'<',            INT_CMD,        INTVEC_CMD, INTVEC_CMD PROFILER}
,{jjCOMPARE_S, '<',            INT_CMD,        STRING_CMD, STRING_CMD PROFILER}
,{jjCOMPARE_P, '<',            INT_CMD,        POLY_CMD,   POLY_CMD PROFILER}
,{jjCOMPARE_P, '<',            INT_CMD,        VECTOR_CMD, VECTOR_CMD PROFILER}
,{jjGE_I,      GE,             INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjGE_N,      GE,             INT_CMD,        NUMBER_CMD, NUMBER_CMD PROFILER}
,{jjCOMPARE_S, GE,             INT_CMD,        STRING_CMD, STRING_CMD PROFILER}
,{jjCOMPARE_IV_I,GE,           INT_CMD,        INTVEC_CMD, INT_CMD PROFILER}
,{jjCOMPARE_IV,GE,             INT_CMD,        INTVEC_CMD, INTVEC_CMD PROFILER}
,{jjCOMPARE_P, GE,             INT_CMD,        POLY_CMD,   POLY_CMD PROFILER}
,{jjCOMPARE_P, GE,             INT_CMD,        VECTOR_CMD, VECTOR_CMD PROFILER}
,{jjGT_I,      '>',            INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjGT_N,      '>',            INT_CMD,        NUMBER_CMD, NUMBER_CMD PROFILER}
,{jjCOMPARE_S, '>',            INT_CMD,        STRING_CMD, STRING_CMD PROFILER}
,{jjCOMPARE_IV_I,'>',          INT_CMD,        INTVEC_CMD, INT_CMD PROFILER}
,{jjCOMPARE_IV,'>',            INT_CMD,        INTVEC_CMD, INTVEC_CMD PROFILER}
,{jjCOMPARE_P, '>',            INT_CMD,        POLY_CMD,   POLY_CMD PROFILER}
,{jjCOMPARE_P, '>',            INT_CMD,        VECTOR_CMD, VECTOR_CMD PROFILER}
,{jjAND_I,     '&',            INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjOR_I,      '|',            INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjEQUAL_I,   EQUAL_EQUAL,    INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjEQUAL_N,   EQUAL_EQUAL,    INT_CMD,        NUMBER_CMD, NUMBER_CMD PROFILER}
,{jjCOMPARE_S, EQUAL_EQUAL,    INT_CMD,        STRING_CMD, STRING_CMD PROFILER}
,{jjEQUAL_P,   EQUAL_EQUAL,    INT_CMD,        POLY_CMD,   POLY_CMD PROFILER}
,{jjEQUAL_P,   EQUAL_EQUAL,    INT_CMD,        VECTOR_CMD, VECTOR_CMD PROFILER}
,{jjCOMPARE_IV_I,EQUAL_EQUAL,  INT_CMD,        INTVEC_CMD, INT_CMD PROFILER}
,{jjCOMPARE_IV,EQUAL_EQUAL,    INT_CMD,        INTVEC_CMD, INTVEC_CMD PROFILER}
,{jjCOMPARE_IV,EQUAL_EQUAL,    INT_CMD,        INTMAT_CMD, INTMAT_CMD PROFILER}
,{jjEQUAL_Ma,  EQUAL_EQUAL,    INT_CMD,        MATRIX_CMD, MATRIX_CMD PROFILER}
,{jjWRONG2,    EQUAL_EQUAL,    0,              IDEAL_CMD,  IDEAL_CMD PROFILER}
,{jjWRONG2,    EQUAL_EQUAL,    0,              MODUL_CMD,  MODUL_CMD PROFILER}
,{jjWRONG2,    EQUAL_EQUAL,    0,              IDEAL_CMD,  MODUL_CMD PROFILER}
,{jjWRONG2,    EQUAL_EQUAL,    0,              MODUL_CMD,  IDEAL_CMD PROFILER}
,{jjEQUAL_I,   NOTEQUAL,       INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjEQUAL_N,   NOTEQUAL,       INT_CMD,        NUMBER_CMD, NUMBER_CMD PROFILER}
,{jjCOMPARE_S, NOTEQUAL,       INT_CMD,        STRING_CMD, STRING_CMD PROFILER}
,{jjEQUAL_P,   NOTEQUAL,       INT_CMD,        POLY_CMD,   POLY_CMD PROFILER}
,{jjEQUAL_P,   NOTEQUAL,       INT_CMD,        VECTOR_CMD, VECTOR_CMD PROFILER}
,{jjCOMPARE_IV,NOTEQUAL,       INT_CMD,        INTVEC_CMD, INTVEC_CMD PROFILER}
,{jjCOMPARE_IV,NOTEQUAL,       INT_CMD,        INTMAT_CMD, INTMAT_CMD PROFILER}
,{jjEQUAL_Ma,  NOTEQUAL,       INT_CMD,        MATRIX_CMD, MATRIX_CMD PROFILER}
,{jjWRONG2,    NOTEQUAL,       0,              IDEAL_CMD,  IDEAL_CMD PROFILER}
,{jjWRONG2,    NOTEQUAL,       0,              MODUL_CMD,  MODUL_CMD PROFILER}
,{jjWRONG2,    NOTEQUAL,       0,              IDEAL_CMD,  MODUL_CMD PROFILER}
,{jjWRONG2,    NOTEQUAL,       0,              MODUL_CMD,  IDEAL_CMD PROFILER}
,{jjDOTDOT,    DOTDOT,         INTVEC_CMD,     INT_CMD,    INT_CMD PROFILER}
,{jjINDEX_I,   '[',            INT_CMD,        INTVEC_CMD, INT_CMD PROFILER}
,{jjINDEX_IV,  '[',            INT_CMD,        INTVEC_CMD, INTVEC_CMD PROFILER}
,{jjINDEX_I,   '[',            POLY_CMD,       IDEAL_CMD,  INT_CMD PROFILER}
,{jjINDEX_I,   '[',            POLY_CMD,       MAP_CMD,    INT_CMD PROFILER}
,{jjINDEX_IV,  '[',            POLY_CMD,       IDEAL_CMD,  INTVEC_CMD PROFILER}
,{jjINDEX_I,   '[',            VECTOR_CMD,     MODUL_CMD,  INT_CMD PROFILER}
,{jjINDEX_IV,  '[',            VECTOR_CMD,     MODUL_CMD,  INTVEC_CMD PROFILER}
,{jjINDEX_I,   '[',            STRING_CMD,     STRING_CMD, INT_CMD PROFILER}
,{jjINDEX_IV,  '[',            STRING_CMD,     STRING_CMD, INTVEC_CMD PROFILER}
,{jjINDEX_I,   '[',            ANY_TYPE/*set by p*/,LIST_CMD, INT_CMD PROFILER}
,{jjINDEX_IV,  '[',            ANY_TYPE/*set by p*/,LIST_CMD, INTVEC_CMD PROFILER}
,{jjINDEX_P,   '[',            POLY_CMD,       POLY_CMD,   INT_CMD PROFILER}
,{jjINDEX_P_IV,'[',            POLY_CMD,       POLY_CMD,   INTVEC_CMD PROFILER}
,{jjINDEX_V,   '[',            POLY_CMD,       VECTOR_CMD, INT_CMD PROFILER}
,{jjINDEX_V_IV,'[',            VECTOR_CMD,     VECTOR_CMD, INTVEC_CMD PROFILER}
,{jjPROC,      '(',            ANY_TYPE/*set by p*/,PROC_CMD, DEF_CMD PROFILER}
,{jjMAP,       '(',            ANY_TYPE/*set by p*/,MAP_CMD, DEF_CMD PROFILER}
,{jjKLAMMER,   '(',            ANY_TYPE/*set by p*/,ANY_TYPE, INT_CMD PROFILER}
,{jjKLAMMER_IV,'(',            ANY_TYPE/*set by p*/,ANY_TYPE, INTVEC_CMD PROFILER}
// and the procedures with 2 arguments:
,{atATTRIB2,   ATTRIB_CMD,     NONE/*set by p*/,DEF_CMD,   STRING_CMD PROFILER}
,{jjWRONG2,    BAREISS_CMD,    0,              DEF_CMD,    DEF_CMD PROFILER}
,{syBetti2,    BETTI_CMD,      INTMAT_CMD,     RESOLUTION_CMD, INT_CMD PROFILER}
,{jjCOEF,      COEF_CMD,       MATRIX_CMD,     POLY_CMD,   POLY_CMD PROFILER}
,{jjCOEFFS_Id, COEFFS_CMD,     MATRIX_CMD,     IDEAL_CMD,  POLY_CMD PROFILER}
,{jjCOEFFS_Id, COEFFS_CMD,     MATRIX_CMD,     MODUL_CMD,  POLY_CMD PROFILER}
,{jjCOEFFS2_KB,COEFFS_CMD,     MATRIX_CMD,     IDEAL_CMD,  IDEAL_CMD, PROFILER}
,{jjCOEFFS2_KB,COEFFS_CMD,     MATRIX_CMD,     MODUL_CMD,  MODUL_CMD, PROFILER}
,{jjCONTRACT,  CONTRACT_CMD,   MATRIX_CMD,     IDEAL_CMD,  IDEAL_CMD PROFILER}
,{jjDEG_IV,    DEG_CMD,        INT_CMD,        POLY_CMD,   INTVEC_CMD PROFILER}
,{lDelete,     DELETE_CMD,     LIST_CMD,       LIST_CMD,   INT_CMD PROFILER}
,{jjDIFF_P,    DIFF_CMD,       POLY_CMD,       POLY_CMD,   POLY_CMD PROFILER}
,{jjDIFF_P,    DIFF_CMD,       VECTOR_CMD,     VECTOR_CMD, POLY_CMD PROFILER}
,{jjDIFF_ID,   DIFF_CMD,       IDEAL_CMD,      IDEAL_CMD,  POLY_CMD PROFILER}
,{jjDIFF_ID_ID,DIFF_CMD,       MATRIX_CMD,     IDEAL_CMD,  IDEAL_CMD PROFILER}
,{jjDIFF_ID,   DIFF_CMD,       MODUL_CMD,      MODUL_CMD,  POLY_CMD PROFILER}
,{jjDIFF_ID,   DIFF_CMD,       MATRIX_CMD,     MATRIX_CMD, POLY_CMD PROFILER}
,{jjDIM2,      DIM_CMD,        INT_CMD,        IDEAL_CMD,  IDEAL_CMD PROFILER}
,{jjDIM2,      DIM_CMD,        INT_CMD,        MODUL_CMD,  IDEAL_CMD PROFILER}
,{jjDIVISION,  DIVISION_CMD,   LIST_CMD,       IDEAL_CMD,  IDEAL_CMD PROFILER}
,{jjDIVISION,  DIVISION_CMD,   LIST_CMD,       MODUL_CMD,  MODUL_CMD PROFILER}
,{jjELIMIN,    ELIMINATION_CMD,IDEAL_CMD,      IDEAL_CMD,  POLY_CMD PROFILER}
,{jjELIMIN,    ELIMINATION_CMD,MODUL_CMD,      MODUL_CMD,  POLY_CMD PROFILER}
,{jjEXTGCD_I,  EXTGCD_CMD,     LIST_CMD,       INT_CMD,    INT_CMD PROFILER}
#ifdef HAVE_FACTORY
,{jjEXTGCD_P,  EXTGCD_CMD,     LIST_CMD,       POLY_CMD,   POLY_CMD PROFILER}
,{jjSQR_FREE_DEC,FAC_CMD,      IDEAL_CMD,      POLY_CMD,   INT_CMD PROFILER}
,{jjFACSTD2,    FACSTD_CMD,    LIST_CMD,       IDEAL_CMD,  IDEAL_CMD PROFILER}
#else
,{jjWRONG2,    EXTGCD_CMD,     LIST_CMD,       POLY_CMD,   POLY_CMD PROFILER}
,{jjWRONG2,    FAC_CMD,        IDEAL_CMD,      POLY_CMD,   INT_CMD PROFILER}
,{jjWRONG2,    FACSTD_CMD,     LIST_CMD,       IDEAL_CMD,  IDEAL_CMD PROFILER}
#endif
,{jjFETCH,     FETCH_CMD,      ANY_TYPE/*set by p*/,RING_CMD,  ANY_TYPE PROFILER}
,{jjFETCH,     FETCH_CMD,      ANY_TYPE/*set by p*/,QRING_CMD, ANY_TYPE PROFILER}
#ifdef HAVE_FGLM
,{fglmProc,    FGLM_CMD,       IDEAL_CMD,      RING_CMD,   DEF_CMD PROFILER}
,{fglmProc,    FGLM_CMD,       IDEAL_CMD,      QRING_CMD,  DEF_CMD PROFILER}
,{fglmQuotProc,FGLMQUOT_CMD,   IDEAL_CMD,      IDEAL_CMD,  POLY_CMD PROFILER}
#else
,{jjWRONG2,    FGLM_CMD,       IDEAL_CMD,      RING_CMD,   DEF_CMD PROFILER}
,{jjWRONG2,    FGLM_CMD,       IDEAL_CMD,      QRING_CMD,  DEF_CMD PROFILER}
,{jjWRONG2,    FGLMQUOT_CMD,   IDEAL_CMD,      POLY_CMD,   IDEAL_CMD PROFILER}
#endif
,{jjFIND2,     FIND_CMD,       INT_CMD,        STRING_CMD, STRING_CMD PROFILER}
,{jjGCD_I,     GCD_CMD,        INT_CMD,        INT_CMD,    INT_CMD PROFILER}
#ifdef HAVE_FACTORY
,{jjGCD_P,     GCD_CMD,        POLY_CMD,       POLY_CMD,   POLY_CMD PROFILER}
#else
,{jjWRONG2,    GCD_CMD,        POLY_CMD,       POLY_CMD,   POLY_CMD PROFILER}
#endif
,{jjGCD_N,     GCD_CMD,        NUMBER_CMD,     NUMBER_CMD, NUMBER_CMD PROFILER}
,{jjHILBERT2,  HILBERT_CMD,    INTVEC_CMD,     IDEAL_CMD,  INT_CMD PROFILER}
,{jjHILBERT2,  HILBERT_CMD,    INTVEC_CMD,     MODUL_CMD,  INT_CMD PROFILER}
,{jjHOMOG_P,   HOMOG_CMD,      POLY_CMD,       POLY_CMD,   POLY_CMD PROFILER}
,{jjHOMOG_P,   HOMOG_CMD,      VECTOR_CMD,     VECTOR_CMD, POLY_CMD PROFILER}
,{jjHOMOG_ID,  HOMOG_CMD,      IDEAL_CMD,      IDEAL_CMD,  POLY_CMD PROFILER}
,{jjHOMOG_ID,  HOMOG_CMD,      MODUL_CMD,      MODUL_CMD,  POLY_CMD PROFILER}
,{jjRES,       HRES_CMD,       RESOLUTION_CMD, IDEAL_CMD,  INT_CMD PROFILER}
,{jjCALL2MANY, IDEAL_CMD,      IDEAL_CMD,      DEF_CMD,    DEF_CMD PROFILER}
,{jjFETCH,     IMAP_CMD,       ANY_TYPE/*set by p*/,RING_CMD,  ANY_TYPE PROFILER}
,{jjFETCH,     IMAP_CMD,       ANY_TYPE/*set by p*/,QRING_CMD, ANY_TYPE PROFILER}
,{jjINDEPSET2, INDEPSET_CMD,   LIST_CMD,       IDEAL_CMD,  INT_CMD PROFILER}
,{lInsert,     INSERT_CMD,     LIST_CMD,       LIST_CMD,   DEF_CMD PROFILER}
,{jjINTERSECT, INTERSECT_CMD,  IDEAL_CMD,      IDEAL_CMD,  IDEAL_CMD PROFILER}
,{jjINTERSECT, INTERSECT_CMD,  MODUL_CMD,      MODUL_CMD,  MODUL_CMD PROFILER}
,{jjJET_P,     JET_CMD,        POLY_CMD,       POLY_CMD,   INT_CMD PROFILER}
,{jjJET_ID,    JET_CMD,        IDEAL_CMD,      IDEAL_CMD,  INT_CMD PROFILER}
,{jjJET_P,     JET_CMD,        VECTOR_CMD,     VECTOR_CMD, INT_CMD PROFILER}
,{jjJET_ID,    JET_CMD,        MODUL_CMD,      MODUL_CMD,  INT_CMD PROFILER}
,{jjJET_ID,    JET_CMD,        MATRIX_CMD,     MATRIX_CMD,  INT_CMD PROFILER}
,{jjKBASE2,    KBASE_CMD,      IDEAL_CMD,      IDEAL_CMD,  INT_CMD PROFILER}
,{jjKBASE2,    KBASE_CMD,      MODUL_CMD,      MODUL_CMD,  INT_CMD PROFILER}
,{atKILLATTR2, KILLATTR_CMD,   NONE,           IDHDL,      STRING_CMD PROFILER}
,{jjKoszul,    KOSZUL_CMD,     MATRIX_CMD,     INT_CMD,    INT_CMD PROFILER}
,{jjKoszul_Id, KOSZUL_CMD,     MATRIX_CMD,     INT_CMD,    IDEAL_CMD PROFILER}
,{jjRES,       KRES_CMD,       RESOLUTION_CMD, IDEAL_CMD,  INT_CMD PROFILER}
,{jjLIFT,      LIFT_CMD,       MATRIX_CMD,     IDEAL_CMD,  IDEAL_CMD PROFILER}
,{jjLIFT,      LIFT_CMD,       MATRIX_CMD,     MODUL_CMD,  MODUL_CMD PROFILER}
,{jjLIFTSTD,   LIFTSTD_CMD,    IDEAL_CMD,      IDEAL_CMD,  MATRIX_CMD PROFILER}
,{jjLIFTSTD,   LIFTSTD_CMD,    MODUL_CMD,      MODUL_CMD,  MATRIX_CMD PROFILER}
,{jjCALL2MANY, LIST_CMD,       LIST_CMD,       DEF_CMD,    DEF_CMD PROFILER}
,{jjRES,       LRES_CMD,       RESOLUTION_CMD, IDEAL_CMD,  INT_CMD PROFILER}
,{jjMINOR,     MINOR_CMD,      IDEAL_CMD,      MATRIX_CMD, INT_CMD PROFILER}
,{jjCALL2MANY, MODUL_CMD,      MODUL_CMD,      DEF_CMD,    DEF_CMD PROFILER}
,{jjMODULO,    MODULO_CMD,     MODUL_CMD,      IDEAL_CMD,  IDEAL_CMD PROFILER}
,{jjMODULO,    MODULO_CMD,     MODUL_CMD,      MODUL_CMD,  MODUL_CMD PROFILER}
,{jjMONITOR2,  MONITOR_CMD,    NONE,           STRING_CMD, STRING_CMD PROFILER}
//,{jjRES,       MRES_CMD,       LIST_CMD,       IDEAL_CMD,  INT_CMD PROFILER}
//,{jjRES,       MRES_CMD,       LIST_CMD,       MODUL_CMD,  INT_CMD PROFILER}
,{jjRES,       MRES_CMD,       RESOLUTION_CMD, IDEAL_CMD,  INT_CMD PROFILER}
,{jjRES,       MRES_CMD,       RESOLUTION_CMD, MODUL_CMD,  INT_CMD PROFILER}
,{jjPARSTR2,   PARSTR_CMD,     STRING_CMD,     RING_CMD,   INT_CMD PROFILER}
,{jjPARSTR2,   PARSTR_CMD,     STRING_CMD,     QRING_CMD,  INT_CMD PROFILER}
,{jjPRINT_FORMAT, PRINT_CMD,   ANY_TYPE,       DEF_CMD,    STRING_CMD PROFILER}
,{jjQUOT,      QUOTIENT_CMD,   IDEAL_CMD,      IDEAL_CMD,  IDEAL_CMD PROFILER}
,{jjQUOT,      QUOTIENT_CMD,   MODUL_CMD,      MODUL_CMD,  IDEAL_CMD PROFILER}
,{jjQUOT,      QUOTIENT_CMD,   IDEAL_CMD,      MODUL_CMD,  MODUL_CMD PROFILER}
,{jjRANDOM,    RANDOM_CMD,     INT_CMD,        INT_CMD,    INT_CMD PROFILER}
,{jjREAD2,     READ_CMD,       STRING_CMD,     LINK_CMD,   STRING_CMD PROFILER}
,{jjREDUCE_P,  REDUCE_CMD,     POLY_CMD,       POLY_CMD,   IDEAL_CMD PROFILER}
,{jjREDUCE_P,  REDUCE_CMD,     VECTOR_CMD,     VECTOR_CMD, IDEAL_CMD PROFILER}
,{jjREDUCE_P,  REDUCE_CMD,     VECTOR_CMD,     VECTOR_CMD, MODUL_CMD PROFILER}
,{jjREDUCE_ID, REDUCE_CMD,     IDEAL_CMD,      IDEAL_CMD,  IDEAL_CMD PROFILER}
,{jjREDUCE_ID, REDUCE_CMD,     MODUL_CMD,      MODUL_CMD,  MODUL_CMD PROFILER}
,{jjREDUCE_ID, REDUCE_CMD,     MODUL_CMD,      MODUL_CMD,  IDEAL_CMD PROFILER}
//,{jjRES,       RES_CMD,        LIST_CMD,       IDEAL_CMD,  INT_CMD PROFILER}
//,{jjRES,       RES_CMD,        LIST_CMD,       MODUL_CMD,  INT_CMD PROFILER}
,{jjRES,       RES_CMD,        RESOLUTION_CMD, IDEAL_CMD,  INT_CMD PROFILER}
,{jjRES,       RES_CMD,        RESOLUTION_CMD, MODUL_CMD,  INT_CMD PROFILER}
,{jjSTATUS2,   STATUS_CMD,     STRING_CMD,     LINK_CMD,   STRING_CMD PROFILER}
,{jjSIMPL_P,   SIMPLIFY_CMD,   POLY_CMD,       POLY_CMD,   INT_CMD PROFILER}
,{jjSIMPL_P,   SIMPLIFY_CMD,   VECTOR_CMD,     VECTOR_CMD, INT_CMD PROFILER}
,{jjSIMPL_ID,  SIMPLIFY_CMD,   IDEAL_CMD,      IDEAL_CMD,  INT_CMD PROFILER}
,{jjSIMPL_ID,  SIMPLIFY_CMD,   MODUL_CMD,      MODUL_CMD,  INT_CMD PROFILER}
//,{jjRES,       SRES_CMD,       LIST_CMD,       IDEAL_CMD,  INT_CMD PROFILER}
//,{jjRES,       SRES_CMD,       LIST_CMD,       MODUL_CMD,  INT_CMD PROFILER}
,{jjRES,       SRES_CMD,       RESOLUTION_CMD, IDEAL_CMD,  INT_CMD PROFILER}
,{jjRES,       SRES_CMD,       RESOLUTION_CMD, MODUL_CMD,  INT_CMD PROFILER}
,{jjCALL2MANY, SYSTEM_CMD,     ANY_TYPE/*set by p*/,STRING_CMD, DEF_CMD PROFILER}
,{jjSTD_1,     STD_CMD,        IDEAL_CMD,      IDEAL_CMD,  POLY_CMD PROFILER}
,{jjSTD_1,     STD_CMD,        MODUL_CMD,      MODUL_CMD,  VECTOR_CMD PROFILER}
,{jjSTD_HILB,  STD_CMD,        IDEAL_CMD,      IDEAL_CMD,  INTVEC_CMD PROFILER}
,{jjSTD_HILB,  STD_CMD,        MODUL_CMD,      MODUL_CMD,  INTVEC_CMD PROFILER}
,{jjVARSTR2,   VARSTR_CMD,     STRING_CMD,     RING_CMD,   INT_CMD PROFILER}
,{jjVARSTR2,   VARSTR_CMD,     STRING_CMD,     QRING_CMD,  INT_CMD PROFILER}
,{jjWEDGE,     WEDGE_CMD,      MATRIX_CMD,     MATRIX_CMD, INT_CMD PROFILER}
,{jjLOAD_E,    LOAD_CMD,       NONE,           STRING_CMD, STRING_CMD PROFILER}
,{nuMPResMat,  MPRES_CMD,      MODUL_CMD,      IDEAL_CMD,  INT_CMD PROFILER}
,{NULL,        0,              0,              0,          0 PROFILER}
};
/*=================== operations with 1 arg.: static proc =================*/
static BOOLEAN jjDUMMY(leftv res, leftv u)
{
  res->data = (char *)u->CopyD();
  return FALSE;
}
static BOOLEAN jjNULL(leftv res, leftv u)
{
  return FALSE;
}
//static BOOLEAN jjPLUSPLUS(leftv res, leftv u)
//{
//  res->data = (char *)((int)u->Data()+1);
//  return FALSE;
//}
//static BOOLEAN jjMINUSMINUS(leftv res, leftv u)
//{
//  res->data = (char *)((int)u->Data()-1);
//  return FALSE;
//}
static BOOLEAN jjPLUSPLUS(leftv res, leftv u)
{
  if (IDTYP((idhdl)u->data)==INT_CMD)
  {
    if (iiOp==PLUSPLUS) IDINT((idhdl)u->data)++;
    else                IDINT((idhdl)u->data)--;
    return FALSE;
  }
  return TRUE;
}
static BOOLEAN jjUMINUS_I(leftv res, leftv u)
{
  res->data = (char *)(-(int)u->Data());
  return FALSE;
}
static BOOLEAN jjUMINUS_N(leftv res, leftv u)
{
  number n=(number)u->CopyD(NUMBER_CMD);
  n=nNeg(n);
  res->data = (char *)n;
  return FALSE;
}
static BOOLEAN jjUMINUS_P(leftv res, leftv u)
{
  res->data = (char *)pNeg((poly)u->CopyD(POLY_CMD));
  return FALSE;
}
static BOOLEAN jjUMINUS_MA(leftv res, leftv u)
{
  poly m1=pISet(-1);
  res->data = (char *)mpMultP((matrix)u->CopyD(MATRIX_CMD),m1);
  return FALSE;
}
static BOOLEAN jjUMINUS_IV(leftv res, leftv u)
{
  intvec *iv=(intvec *)u->CopyD(INTVEC_CMD);
  (*iv)*=(-1);
  res->data = (char *)iv;
  return FALSE;
}
static BOOLEAN jjPROC1(leftv res, leftv u)
{
  if ((u->rtyp!=IDHDL) || (u->e!=NULL))
    return TRUE;
#ifdef HAVE_NAMESPACES
  leftv sl = iiMake_proc((idhdl) u->data,u,NULL);
#else /* HAVE_NAMESPACES */
#ifdef HAVE_NS
  leftv sl = iiMake_proc((idhdl) u->data,u->req_packhdl,NULL);
#else /* HAVE_NS */
  leftv sl = iiMake_proc((idhdl) u->data,NULL);
#endif /* HAVE_NS */
#endif /* HAVE_NAMESPACES */
  if (sl==NULL)
  {
    return TRUE;
  }
  else
  {
    memcpy(res,sl,sizeof(sleftv));
  }
  return FALSE;
}
static BOOLEAN jjBAREISS(leftv res, leftv v)
{
  //matrix m=(matrix)v->Data();
  //lists l=mpBareiss(m,FALSE);
  lists l=smCallNewBareiss((ideal)v->Data(),0,0);
  res->data = (char *)l;
  return FALSE;
}
//static BOOLEAN jjBAREISS_IM(leftv res, leftv v)
//{
//  intvec *m=(intvec *)v->CopyD(INTMAT_CMD);
//  ivTriangMat(m);
//  res->data = (char *)m;
//  return FALSE;
//}
static BOOLEAN jjCALL1MANY(leftv res, leftv u)
{
  return iiExprArithM(res,u,iiOp);
}
static BOOLEAN jjCHAR(leftv res, leftv v)
{
  res->data = (char *)rChar((ring)v->Data());
  return FALSE;
}
static BOOLEAN jjCOLS(leftv res, leftv v)
{
  res->data = (char *)MATCOLS((matrix)(v->Data()));
  return FALSE;
}
static BOOLEAN jjCOLS_IV(leftv res, leftv v)
{
  res->data = (char *)((intvec*)(v->Data()))->cols();
  return FALSE;
}
static BOOLEAN jjCONTENT(leftv res, leftv v)
{
  // CopyD for POLY_CMD and VECTOR_CMD are identical:
  poly p=(poly)v->CopyD(POLY_CMD);
  if (p!=NULL) pCleardenom(p);
  res->data = (char *)p;
  return FALSE;
}
static BOOLEAN jjCOUNT_N(leftv res, leftv v)
{
  res->data = (char *)nSize((number)v->Data());
  return FALSE;
}
static BOOLEAN jjCOUNT_L(leftv res, leftv v)
{
  lists l=(lists)v->Data();
  res->data = (char *)(l->nr+1);
  return FALSE;
}
static BOOLEAN jjCOUNT_M(leftv res, leftv v)
{
  matrix m=(matrix)v->Data();
  res->data = (char *)(MATROWS(m)*MATCOLS(m));
  return FALSE;
}
static BOOLEAN jjCOUNT_IV(leftv res, leftv v)
{
  res->data = (char *)((intvec*)(v->Data()))->length();
  return FALSE;
}
static BOOLEAN jjCOUNT_RG(leftv res, leftv v)
{
  ring r=(ring)v->Data();
  int elems=-1;
  if (rField_is_Zp(r)||rField_is_GF(r)) elems=rInternalChar(r);
  else if (rField_is_Zp_a(r) && (r->minpoly!=NULL))
  {
    elems=ABS(rInternalChar(r)*naParDeg(r->minpoly));
  }
  res->data = (char *)elems;
  return FALSE;
}
static BOOLEAN jjDEG(leftv res, leftv v)
{
  int dummy;
  poly p=(poly)v->Data();
  if (p!=NULL) res->data = (char *)pLDeg(p,&dummy);
  else res->data=(char *)-1;
  return FALSE;
}
static BOOLEAN jjDEGREE(leftv res, leftv v)
{
  assumeStdFlag(v);
  scDegree((ideal)v->Data(),currQuotient);
  return FALSE;
}
static BOOLEAN jjDEFINED(leftv res, leftv v)
{
  if ((v->rtyp==IDHDL)
  && ((myynest==IDLEV((idhdl)v->data))||(0==IDLEV((idhdl)v->data))))
  {
    res->data=(void *)(IDLEV((idhdl)v->data)+1);
  }
  else if (v->rtyp!=0) res->data=(void *)(-1);
  return FALSE;
}
#ifdef HAVE_FACTORY
static BOOLEAN jjDET(leftv res, leftv v)
{
  matrix m=(matrix)v->Data();
  poly p;
  if (smCheckDet((ideal)m,m->cols(),TRUE))
  {
    ideal I=idMatrix2Module(mpCopy(m));
    p=smCallDet(I);
    idDelete(&I);
  }
  else
    p=singclap_det(m);
  res ->data = (char *)p;
  return FALSE;
}
static BOOLEAN jjDET_I(leftv res, leftv v)
{
  intvec * m=(intvec*)v->Data();
  int i,j;
  i=m->rows();j=m->cols();
  if(i==j)
    res->data = (char *)singclap_det_i(m);
  else
  {
    Werror("det of %d x %d intmat",i,j);
    return TRUE;
  }
  return FALSE;
}
static BOOLEAN jjDET_S(leftv res, leftv v)
{
  ideal I=(ideal)v->Data();
  poly p;
  if (smCheckDet(I,IDELEMS(I),FALSE))
  {
    matrix m=idModule2Matrix(idCopy(I));
    p=singclap_det(m);
    idDelete((ideal *)&m);
  }
  else
    p=smCallDet(I);
  res->data = (char *)p;
  return FALSE;
}
#endif
static BOOLEAN jjDIM(leftv res, leftv v)
{
  assumeStdFlag(v);
  res->data = (char *)scDimInt((ideal)(v->Data()),currQuotient);
  return FALSE;
}
static BOOLEAN jjDUMP(leftv res, leftv v)
{
  si_link l = (si_link)v->Data();
  if (slDump(l))
  {
    const char *s;
    if ((l!=NULL)&&(l->name!=NULL)) s=l->name;
    else                            s=sNoName;
    Werror("cannot dump to `%s`",s);
    return TRUE;
  }
  else
    return FALSE;
}
static BOOLEAN jjE(leftv res, leftv v)
{
  res->data = (char *)pOne();
  pSetComp((poly)res->data,(int)v->Data());
  pSetm((poly)res->data);
  return FALSE;
}
static BOOLEAN jjEXECUTE(leftv res, leftv v)
{
  char * d = (char *)v->Data();
  char * s = (char *)omAlloc(strlen(d) + 13);
  strcpy( s, (char *)d);
  strcat( s, "\n;RETURN();\n");
  newBuffer(s,BT_execute);
  return yyparse();
}
#ifdef HAVE_FACTORY
static BOOLEAN jjFACSTD(leftv res, leftv v)
{
  res->data=(void *)kStdfac((ideal)v->Data(),NULL,testHomog,NULL);
  setFlag(res,FLAG_STD);
  return FALSE;
}
#endif
static BOOLEAN jjGETDUMP(leftv res, leftv v)
{
  si_link l = (si_link)v->Data();
  if (slGetDump(l))
  {
    const char *s;
    if ((l!=NULL)&&(l->name!=NULL)) s=l->name;
    else                            s=sNoName;
    Werror("cannot get dump from `%s`",s);
    return TRUE;
  }
  else
    return FALSE;
}
static BOOLEAN jjHIGHCORNER(leftv res, leftv v)
{
  assumeStdFlag(v);
  ideal I=(ideal)v->Data();
  res->data=(void *)iiHighCorner(I,0);
  return FALSE;
}
static BOOLEAN jjHIGHCORNER_M(leftv res, leftv v)
{
  assumeStdFlag(v);
  intvec *w=(intvec*)atGet(v,"isHomog");
  BOOLEAN delete_w=FALSE;
  ideal I=(ideal)v->Data();
  int i;
  poly p=NULL,po=NULL;
  int rk=idRankFreeModule(I);
  if (w==NULL)
  {
    w = new intvec(rk);
    delete_w=TRUE;
  }
  for(i=rk;i>0;i--)
  {
    p=iiHighCorner(I,i);
    if (p==NULL)
    {
      Werror("module must be zero-dimensional");
      if (delete_w) delete w;
      return TRUE;
    }
    if (po==NULL)
    {
      po=p;
    }
    else
    {
      // now po!=NULL, p!=NULL
      int d=(pFDeg(po)-(*w)[pGetComp(po)-1] - pFDeg(p)+(*w)[i-1]);
      if (d==0)
        d=pLmCmp(po,p);
      if (d > 0)
      {
        pDelete(&p);
      }
      else // (d < 0)
      {
        pDelete(&po); po=p;
      }
    }
  }
  if (delete_w) delete w;
  res->data=(void *)po;
  return FALSE;
}
static BOOLEAN jjHILBERT(leftv res, leftv v)
{
  assumeStdFlag(v);
  intvec *module_w=(intvec*)atGet(v,"isHomog");
  //scHilbertPoly((ideal)v->Data(),currQuotient);
  hLookSeries((ideal)v->Data(),module_w,currQuotient);
  return FALSE;
}
static BOOLEAN jjHILBERT_IV(leftv res, leftv v)
{
  res->data=(void *)hSecondSeries((intvec *)v->Data());
  return FALSE;
}
static BOOLEAN jjHOMOG1(leftv res, leftv v)
{
  intvec *w;
  res->data=(void *)idHomModule((ideal)v->Data(),currQuotient,&w);
  if ((v->rtyp==IDHDL)&&(w!=NULL))
    atSet((idhdl)v->data,omStrDup("isHomog"),w,INTVEC_CMD);
  return FALSE;
}
static BOOLEAN jjIDEAL_Ma(leftv res, leftv v)
{
  matrix mat=(matrix)v->CopyD(MATRIX_CMD);
  IDELEMS((ideal)mat)=MATCOLS(mat)*MATROWS(mat);
  if (IDELEMS((ideal)mat)==0)
  {
    idDelete((ideal *)&mat);
    mat=(matrix)idInit(1,1);
  }
  else
  {
    MATROWS(mat)=1;
    mat->rank=1;
    idTest((ideal)mat);
  }
  res->data=(char *)mat;
  return FALSE;
}
static BOOLEAN jjIDEAL_Map(leftv res, leftv v)
{
  map m=(map)v->CopyD(MAP_CMD);
  omFree((ADDRESS)m->preimage);
  m->preimage=NULL;
  ideal I=(ideal)m;
  I->rank=1;
  res->data=(char *)I;
  return FALSE;
}
static BOOLEAN jjIDEAL_R(leftv res, leftv v)
{
  if (currRing!=NULL)
  {
    ring q=(ring)v->Data();
    if (rEqual(currRing, q, 0))
    {
      if (q->qideal==NULL)
        res->data=(char *)idInit(1,1);
      else
        res->data=(char *)idCopy(q->qideal);
      return FALSE;
    }
  }
  WerrorS("can only get ideal from identical qring");
  return TRUE;
}
static BOOLEAN jjIm2Iv(leftv res, leftv v)
{
  intvec *iv = (intvec *)v->CopyD(INTMAT_CMD);
  iv->makeVector();
  res->data = iv;
  return FALSE;
}
static BOOLEAN jjIMPART(leftv res, leftv v)
{
  res->data = (char *)nImPart((number)v->Data());
  return FALSE;
}
static BOOLEAN jjINDEPSET(leftv res, leftv v)
{
  assumeStdFlag(v);
  res->data=(void *)scIndIntvec((ideal)(v->Data()),currQuotient);
  return FALSE;
}
static BOOLEAN jjINTERRED(leftv res, leftv v)
{
  ideal result=kInterRed((ideal)(v->Data()),currQuotient);
  //if (result==NULL) return TRUE;
  idSkipZeroes(result);
  res->data = result;
  return FALSE;
}
static BOOLEAN jjIS_RINGVAR_P(leftv res, leftv v)
{
  res->data = (char *)pVar((poly)v->Data());
  return FALSE;
}
static BOOLEAN jjIS_RINGVAR_S(leftv res, leftv v)
{
  res->data = (char *)(r_IsRingVar((char *)v->Data(), currRing)+1);
  return FALSE;
}
static BOOLEAN jjIS_RINGVAR0(leftv res, leftv v)
{
  res->data = (char *)0;
  return FALSE;
}
static BOOLEAN jjJACOB_P(leftv res, leftv v)
{
  ideal i=idInit(pVariables,1);
  int k;
  poly p=(poly)(v->Data());
  for (k=pVariables;k>0;k--)
  {
    i->m[k-1]=pDiff(p,k);
  }
  res->data = (char *)i;
  return FALSE;
}
static BOOLEAN jjKBASE(leftv res, leftv v)
{
  assumeStdFlag(v);
  res->data = (char *)scKBase(-1,(ideal)(v->Data()),currQuotient);
  return FALSE;
}
#ifdef MDEBUG
static BOOLEAN jjpHead(leftv res, leftv v)
{
  res->data=(char *)pHead((poly)v->Data());
  return FALSE;
}
#endif
static BOOLEAN jjL2R(leftv res, leftv v)
{
  res->data=(char *)syConvList((lists)v->Data());
  if (res->data != NULL)
    return FALSE;
  else
    return TRUE;
}
static BOOLEAN jjLEADCOEF(leftv res, leftv v)
{
  poly p=(poly)v->Data();
  if (p==NULL)
  {
    res->data=(char *)nInit(0);
  }
  else
  {
    res->data=(char *)nCopy(pGetCoeff(p));
  }
  return FALSE;
}
static BOOLEAN jjLEADEXP(leftv res, leftv v)
{
  poly p=(poly)v->Data();
  int s=pVariables;
  if (v->Typ()==VECTOR_CMD) s++;
  intvec *iv=new intvec(s);
  if (p!=NULL)
  {
    for(int i = pVariables;i;i--)
    {
      (*iv)[i-1]=pGetExp(p,i);
    }
    if (s!=pVariables)
      (*iv)[pVariables]=pGetComp(p);
  }
  res->data=(char *)iv;
  return FALSE;
}
static BOOLEAN jjLEADMONOM(leftv res, leftv v)
{
  poly p=(poly)v->Data();
  if (p == NULL)
  {
    res->data = (char*) NULL;
  }
  else
  {
    poly lm = pLmInit(p);
    pSetCoeff(lm, nInit(1));
    res->data = (char*) lm;
  }
  return FALSE;
}
static BOOLEAN jjLISTRING(leftv res, leftv v)
{
  ring r=rCompose((lists)v->Data());
  if (r==NULL) return TRUE;
  if (r->qideal!=NULL) res->rtyp=QRING_CMD;
  res->data=(char *)r;
  return FALSE;
}
static BOOLEAN jjMEMORY(leftv res, leftv v)
{
  omUpdateInfo();
  switch(((int)v->Data()))
  {
  case 0:
    res->data = (char *)om_Info.UsedBytes;
    break;
  case 1:
    res->data = (char *)om_Info.CurrentBytesSystem;
    break;
  case 2:
    res->data = (char *)om_Info.MaxBytesSystem;
    break;

  default:
    omPrintStats(stdout);
    omPrintInfo(stdout);
    omPrintBinStats(stdout);
    res->data = (char *)0;
  }
  return FALSE;
  res->data = (char *)0;
  return FALSE;
}
static BOOLEAN jjMONITOR1(leftv res, leftv v)
{
  monitor((char *)(v->Data()),PROT_I);
  return FALSE;
}
static BOOLEAN jjMSTD(leftv res, leftv v)
{
  res->data=(void *)min_std((ideal)v->Data(),currQuotient,testHomog,NULL);
  return FALSE;
}
static BOOLEAN jjMULT(leftv res, leftv v)
{
  assumeStdFlag(v);
  res->data = (char *)scMultInt((ideal)(v->Data()),currQuotient);
  return FALSE;
}
static BOOLEAN jjNAMEOF(leftv res, leftv v)
{
  res->data = (char *)v->name;
  if (res->data==NULL) res->data=omStrDup("");
  v->name=NULL;
  return FALSE;
}
static BOOLEAN jjNAMES(leftv res, leftv v)
{
  res->data=ipNameList(((ring)v->Data())->idroot);
  return FALSE;
}
static BOOLEAN jjNVARS(leftv res, leftv v)
{
  res->data = (char *)(((ring)(v->Data()))->N);
  return FALSE;
}
static BOOLEAN jjOpenClose(leftv res, leftv v)
{
  si_link l=(si_link)v->Data();
  if (iiOp==OPEN_CMD) return slOpen(l, SI_LINK_OPEN);
  else                return slClose(l);
}
static BOOLEAN jjORD(leftv res, leftv v)
{
  poly p=(poly)v->Data();
  res->data=(char *)( p==NULL ? -1 : pFDeg(p) );
  return FALSE;
}
static BOOLEAN jjPAR1(leftv res, leftv v)
{
  int i=(int)v->Data();
  int p=0;
  p=rPar(currRing);
  if ((0<i) && (i<=p))
  {
    res->data=(char *)nPar(i);
  }
  else
  {
    Werror("par number %d out of range 1..%d",i,p);
    return TRUE;
  }
  return FALSE;
}
static BOOLEAN jjPARDEG(leftv res, leftv v)
{
  res->data = (char *)nParDeg((number)v->Data());
  return FALSE;
}
static BOOLEAN jjPARSTR1(leftv res, leftv v)
{
  if (currRing==NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
  int i=(int)v->Data();
  int p=0;
  if ((0<i) && (currRing->parameter!=NULL) && (i<=(p=rPar(currRing))))
    res->data=omStrDup(currRing->parameter[i-1]);
  else
  {
    Werror("par number %d out of range 1..%d",i,p);
    return TRUE;
  }
  return FALSE;
}
static BOOLEAN jjP2I(leftv res, leftv v)
{
  poly p=(poly)v->Data();
  if (p==NULL) { /*res->data=(char *)0;*/ return FALSE; }
  if ((pNext(p)!=NULL)|| (!pIsConstant(p)))
  {
    WerrorS("poly must be constant");
    return TRUE;
  }
  res->data = (char *)nInt(pGetCoeff(p));
  return FALSE;
}
static BOOLEAN jjPREIMAGE_R(leftv res, leftv v)
{
  map mapping=(map)v->Data();
  syMake(res,omStrDup(mapping->preimage));
  return FALSE;
}
static BOOLEAN jjPRIME(leftv res, leftv v)
{
  int i = IsPrime((int)(v->Data()));
  res->data = (char *)(i > 1 ? i : 2);
  return FALSE;
}
static BOOLEAN jjPRUNE(leftv res, leftv v)
{
  res->data = (char *)idMinEmbedding((ideal)v->Data());
  return FALSE;
}
static BOOLEAN jjP2N(leftv res, leftv v)
{
  number n;
  poly p;
  if (((p=(poly)v->Data())!=NULL)
  && (pIsConstant(p)))
  {
    n=nCopy(pGetCoeff(p));
  }
  else
  {
    n=nInit(0);
  }
  res->data = (char *)n;
  return FALSE;
}
static BOOLEAN jjRESERVEDNAME(leftv res, leftv v)
{
  char *s= (char *)v->Data();
  int i = 1;
  int l = strlen(s);
  while (cmds[i].tokval!=0)
  {
    if (strcmp(s, cmds[i].name) == 0)
    {
      res->data = (char *)1;
      return FALSE;
    }
    i++;
  }
  //res->data = (char *)0;
  return FALSE;
}
static BOOLEAN jjREAD(leftv res, leftv v)
{
  return jjREAD2(res,v,NULL);
}
static BOOLEAN jjREGULARITY(leftv res, leftv v)
{
  res->data = (char *)iiRegularity((lists)v->Data());
  return FALSE;
}
static BOOLEAN jjREPART(leftv res, leftv v)
{
  res->data = (char *)nRePart((number)v->Data());
  return FALSE;
}
static BOOLEAN jjRINGLIST(leftv res, leftv v)
{
  ring r=(ring)v->Data();
  if (r!=NULL)
    res->data = (char *)rDecompose((ring)v->Data());
  return (r==NULL);
}
static BOOLEAN jjROWS(leftv res, leftv v)
{
  ideal i = (ideal)v->Data();
  res->data = (char *)i->rank;
  return FALSE;
}
static BOOLEAN jjROWS_IV(leftv res, leftv v)
{
  res->data = (char *)((intvec*)(v->Data()))->rows();
  return FALSE;
}
static BOOLEAN jjRPAR(leftv res, leftv v)
{
  res->data = (char *)rPar(((ring)v->Data()));
  return FALSE;
}
static BOOLEAN jjSTD(leftv res, leftv v)
{
  ideal result;
  intvec *w=(intvec *)atGet(v,"isHomog");
  tHomog hom=testHomog;
  if (w!=NULL)
  {
    w=ivCopy(w);
    hom=isHomog;
  }
  result=kStd((ideal)(v->Data()),currQuotient,hom,&w);
  idSkipZeroes(result);
  res->data = (char *)result;
  setFlag(res,FLAG_STD);
  if (w!=NULL) atSet(res,omStrDup("isHomog"),w,INTVEC_CMD);
  return FALSE;
}
static BOOLEAN jjSort_Id(leftv res, leftv v)
{
  res->data = (char *)idSort((ideal)v->Data());
  return FALSE;
}
static BOOLEAN jjSYZYGY(leftv res, leftv v)
{
  intvec *w=NULL;
  res->data = (char *)idSyzygies((ideal)v->Data(),testHomog,&w);
  if (w!=NULL) delete w;
  return FALSE;
}
static BOOLEAN jjTRACE_IV(leftv res, leftv v)
{
  res->data = (char *)ivTrace((intvec*)(v->Data()));
  return FALSE;
}
static BOOLEAN jjTRANSP_IV(leftv res, leftv v)
{
  res->data = (char *)ivTranp((intvec*)(v->Data()));
  return FALSE;
}
static BOOLEAN jjTYPEOF(leftv res, leftv v)
{
  switch ((int)v->data)
  {
    case INT_CMD:     res->data=omStrDup("int"); break;
    case POLY_CMD:   res->data=omStrDup("poly"); break;
    case VECTOR_CMD:  res->data=omStrDup("vector"); break;
    case STRING_CMD:  res->data=omStrDup("string"); break;
    case INTVEC_CMD:  res->data=omStrDup("intvec"); break;
    case IDEAL_CMD:   res->data=omStrDup("ideal"); break;
    case MATRIX_CMD: res->data=omStrDup("matrix"); break;
    case MODUL_CMD:   res->data=omStrDup("module"); break;
    case MAP_CMD:     res->data=omStrDup("map"); break;
    case PROC_CMD:    res->data=omStrDup("proc"); break;
    case RING_CMD:    res->data=omStrDup("ring"); break;
    case QRING_CMD:   res->data=omStrDup("qring"); break;
    case INTMAT_CMD:  res->data=omStrDup("intmat"); break;
    case NUMBER_CMD:  res->data=omStrDup("number"); break;
    case LIST_CMD:   res->data=omStrDup("list"); break;
    case PACKAGE_CMD: res->data=omStrDup("package"); break;
    case LINK_CMD:   res->data=omStrDup("link"); break;
    case RESOLUTION_CMD:res->data=omStrDup("resolution");break;
    case DEF_CMD:
    case NONE:    res->data=omStrDup("none"); break;
    default:       res->data=omStrDup("?unknown type?");
  }
  return FALSE;
}
static BOOLEAN jjVAR1(leftv res, leftv v)
{
  int i=(int)v->Data();
  if ((0<i) && (i<=currRing->N))
  {
    poly p=pOne();
    pSetExp(p,i,1);
    pSetm(p);
    res->data=(char *)p;
  }
  else
  {
    Werror("var number %d out of range 1..%d",i,currRing->N);
    return TRUE;
  }
  return FALSE;
}
static BOOLEAN jjVARSTR1(leftv res, leftv v)
{
  if (currRing==NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
  int i=(int)v->Data();
  if ((0<i) && (i<=currRing->N))
    res->data=omStrDup(currRing->names[i-1]);
  else
  {
    Werror("var number %d out of range 1..%d",i,currRing->N);
    return TRUE;
  }
  return FALSE;
}
static BOOLEAN jjVDIM(leftv res, leftv v)
{
  assumeStdFlag(v);
  res->data = (char *)scMult0Int((ideal)v->Data(),currQuotient);
  return FALSE;
}

static BOOLEAN jjLOAD1(leftv res, leftv v)
{
  return jjLOAD(res, v);
}

static BOOLEAN jjLOAD(leftv res, leftv v, BOOLEAN autoexport)
{
  char * s=(char *)v->CopyD();
  char libnamebuf[256];
  lib_types LT = type_of_LIB(s, libnamebuf);
#ifdef HAVE_DYNAMIC_LOADING
  extern BOOLEAN load_modules(char *newlib, char *fullpath, BOOLEAN tellerror);
#endif /* HAVE_DYNAMIC_LOADING */

  switch(LT)
  {
      default:
      case LT_NONE:
        Werror("%s: unknown type", s);
        break;
      case LT_NOTFOUND:
        Werror("cannot open %s", s);
        break;

      case LT_SINGULAR:
#ifdef HAVE_NAMESPACES
        return iiLibCmd(s, autoexport);
#else
        return iiLibCmd(s);
#endif

      case LT_ELF:
      case LT_HPUX:
#ifdef HAVE_DYNAMIC_LOADING
        return load_modules(s, libnamebuf, autoexport);
#else /* HAVE_DYNAMIC_LOADING */
        WerrorS("Dynamic modules are not supported by this version of Singular");
        break;
#endif /* HAVE_DYNAMIC_LOADING */
  }
  return TRUE;
}

/*=================== operations with 1 arg.: table =================*/

#ifdef INIT_BUG
#define XS(A) -((short)A)
#define jjstrlen       (proc1)1
#define jjpLength      (proc1)2
#define jjidElem       (proc1)3
#define jjmpDetBareiss (proc1)4
#define jjidFreeModule (proc1)5
#define jjidVec2Ideal  (proc1)6
#define jjrCharStr     (proc1)7
#ifndef MDEBUG
#define jjpHead        (proc1)8
#endif
#define jjidHead       (proc1)9
#define jjidMaxIdeal   (proc1)10
#define jjidMinBase    (proc1)11
#define jjsyMinBase    (proc1)12
#define jjpMaxComp     (proc1)13
#define jjmpTrace      (proc1)14
#define jjmpTransp     (proc1)15
#define jjrOrdStr      (proc1)16
#define jjrVarStr      (proc1)18
#define jjrParStr      (proc1)19
#define jjCOUNT_RES    (proc1)22
#define jjDIM_R        (proc1)23
#define jjMINRES_R     (proc1)24
#define jjidTransp     (proc1)25

extern struct sValCmd1 dArith1[];
void jjInitTab1()
{
  int i=0;
  for (;dArith1[i].cmd!=0;i++)
  {
    if (dArith1[i].res<0)
    {
      switch ((int)dArith1[i].p)
      {
        case (int)jjstrlen:       dArith1[i].p=(proc1)strlen; break;
        case (int)jjpLength:      dArith1[i].p=(proc1)pLength; break;
        case (int)jjidElem:       dArith1[i].p=(proc1)idElem; break;
        case (int)jjidVec2Ideal:  dArith1[i].p=(proc1)idVec2Ideal; break;
#ifndef HAVE_FACTORY
        case (int)jjmpDetBareiss: dArith1[i].p=(proc1)mpDetBareiss; break;
#endif
        case (int)jjidFreeModule: dArith1[i].p=(proc1)idFreeModule; break;
        case (int)jjrCharStr:     dArith1[i].p=(proc1)rCharStr; break;
#ifndef MDEBUG
        case (int)jjpHead:        dArith1[i].p=(proc1)pHeadProc; break;
#endif
        case (int)jjidHead:       dArith1[i].p=(proc1)idHead; break;
        case (int)jjidMaxIdeal:   dArith1[i].p=(proc1)idMaxIdeal; break;
        case (int)jjidMinBase:    dArith1[i].p=(proc1)idMinBase; break;
        case (int)jjsyMinBase:    dArith1[i].p=(proc1)syMinBase; break;
        case (int)jjpMaxComp:     dArith1[i].p=(proc1)pMaxCompProc; break;
        case (int)jjmpTrace:      dArith1[i].p=(proc1)mpTrace; break;
        case (int)jjmpTransp:     dArith1[i].p=(proc1)mpTransp; break;
        case (int)jjrOrdStr:      dArith1[i].p=(proc1)rOrdStr; break;
        case (int)jjrVarStr:      dArith1[i].p=(proc1)rVarStr; break;
        case (int)jjrParStr:      dArith1[i].p=(proc1)rParStr; break;
        case (int)jjCOUNT_RES:    dArith1[i].p=(proc1)syLength; break;
        case (int)jjDIM_R:        dArith1[i].p=(proc1)syDim; break;
        case (int)jjMINRES_R:     dArith1[i].p=(proc1)syMinimize; break;
        case (int)jjidTransp:     dArith1[i].p=(proc1)idTransp; break;
#ifdef GENTABLE
        default: Werror("missing proc1-definition for %d",(int)dArith1[i].p);
#endif
      }
    }
  }
}
#else
#if defined(PROC_BUG)
#define XS(A) A
static BOOLEAN jjstrlen(leftv res, leftv v)
{
  res->data = (char *)strlen((char *)v->Data());
  return FALSE;
}
static BOOLEAN jjpLength(leftv res, leftv v)
{
  res->data = (char *)pLength((poly)v->Data());
  return FALSE;
}
static BOOLEAN jjidElem(leftv res, leftv v)
{
  res->data = (char *)idElem((ideal)v->Data());
  return FALSE;
}
static BOOLEAN jjmpDetBareiss(leftv res, leftv v)
{
  res->data = (char *)mpDetBareiss((matrix)v->Data());
  return FALSE;
}
static BOOLEAN jjidFreeModule(leftv res, leftv v)
{
  res->data = (char *)idFreeModule((int)v->Data());
  return FALSE;
}
static BOOLEAN jjidVec2Ideal(leftv res, leftv v)
{
  res->data = (char *)idVec2Ideal((poly)v->Data());
  return FALSE;
}
static BOOLEAN jjrCharStr(leftv res, leftv v)
{
  res->data = rCharStr((ring)v->Data());
  return FALSE;
}
#ifndef MDEBUG
static BOOLEAN jjpHead(leftv res, leftv v)
{
  res->data = (char *)pHead((poly)v->Data());
  return FALSE;
}
#endif
static BOOLEAN jjidHead(leftv res, leftv v)
{
  res->data = (char *)idHead((ideal)v->Data());
  return FALSE;
}
static BOOLEAN jjidMaxIdeal(leftv res, leftv v)
{
  res->data = (char *)idMaxIdeal((int)v->Data());
  return FALSE;
}
static BOOLEAN jjidMinBase(leftv res, leftv v)
{
  res->data = (char *)idMinBase((ideal)v->Data());
  return FALSE;
}
static BOOLEAN jjsyMinBase(leftv res, leftv v)
{
  res->data = (char *)syMinBase((ideal)v->Data());
  return FALSE;
}
static BOOLEAN jjpMaxComp(leftv res, leftv v)
{
  res->data = (char *)pMaxComp((poly)v->Data());
  return FALSE;
}
static BOOLEAN jjmpTrace(leftv res, leftv v)
{
  res->data = (char *)mpTrace((matrix)v->Data());
  return FALSE;
}
static BOOLEAN jjmpTransp(leftv res, leftv v)
{
  res->data = (char *)mpTransp((matrix)v->Data());
  return FALSE;
}
static BOOLEAN jjrOrdStr(leftv res, leftv v)
{
  res->data = rOrdStr((ring)v->Data());
  return FALSE;
}
static BOOLEAN jjrVarStr(leftv res, leftv v)
{
  res->data = rVarStr((ring)v->Data());
  return FALSE;
}
static BOOLEAN jjrParStr(leftv res, leftv v)
{
  res->data = rParStr((ring)v->Data());
  return FALSE;
}
static BOOLEAN jjCOUNT_RES(leftv res, leftv v)
{
  res->data=(char *)syLength((syStrategy)v->Data());
  return FALSE;
}
static BOOLEAN jjDIM_R(leftv res, leftv v)
{
  res->data = (char *)syDim((syStrategy)v->Data());
  return FALSE;
}
static BOOLEAN jjMINRES_R(leftv res, leftv v)
{
  res->data=(char *)syMinimize((syStrategy)v->Data());
  return FALSE;
}
static BOOLEAN jjidTransp(leftv res, leftv v)
{
  res->data = (char *)idTransp((ideal)v->Data());
  return FALSE;
}
#else
#define XS(A)          -((short)A)
#define jjstrlen       (proc1)strlen
#define jjpLength      (proc1)pLength
#define jjidElem       (proc1)idElem
#define jjmpDetBareiss (proc1)mpDetBareiss
#define jjidFreeModule (proc1)idFreeModule
#define jjidVec2Ideal  (proc1)idVec2Ideal
#define jjrCharStr     (proc1)rCharStr
#ifndef MDEBUG
#define jjpHead        (proc1)pHeadProc
#endif
#define jjidHead       (proc1)idHead
#define jjidMaxIdeal   (proc1)idMaxIdeal
#define jjidMinBase    (proc1)idMinBase
#define jjsyMinBase    (proc1)syMinBase
#define jjpMaxComp     (proc1)pMaxCompProc
#define jjmpTrace      (proc1)mpTrace
#define jjmpTransp     (proc1)mpTransp
#define jjrOrdStr      (proc1)rOrdStr
#define jjrVarStr      (proc1)rVarStr
#define jjrParStr      (proc1)rParStr
#define jjCOUNT_RES    (proc1)syLength
#define jjDIM_R        (proc1)syDim
#define jjMINRES_R     (proc1)syMinimize
#define jjidTransp     (proc1)idTransp
#endif
#endif
static BOOLEAN jjnInt(leftv res, leftv u)
{
  number n=(number)u->CopyD(NUMBER_CMD);
  res->data=(char *)nInt(n);
  nDelete(&n);
  return FALSE;
}
#define s short
struct sValCmd1 dArith1[]=
{
// operations:
// proc         cmd               res             arg
// {jjPLUSPLUS,   PLUSPLUS,        INT_CMD,        INT_CMD }
//,{jjMINUSMINUS, MINUSMINUS,      INT_CMD,        INT_CMD }
 {jjPLUSPLUS,   PLUSPLUS,        NONE,           IDHDL }
,{jjPLUSPLUS,   MINUSMINUS,      NONE,           IDHDL }
,{jjUMINUS_I,   '-',             INT_CMD,        INT_CMD }
,{jjUMINUS_N,   '-',             NUMBER_CMD,     NUMBER_CMD }
,{jjUMINUS_P,   '-',             POLY_CMD,       POLY_CMD }
,{jjUMINUS_P,   '-',             VECTOR_CMD,     VECTOR_CMD }
,{jjUMINUS_MA,  '-',             MATRIX_CMD,     MATRIX_CMD }
,{jjUMINUS_IV,  '-',             INTVEC_CMD,     INTVEC_CMD }
,{jjUMINUS_IV,  '-',             INTMAT_CMD,     INTMAT_CMD }
,{jjPROC1,      '(',             ANY_TYPE/*set by p*/,PROC_CMD }
,{jjLOAD1,      '(',             NONE,           STRING_CMD }
// and the procedures with 1 argument:
,{atATTRIB1,    ATTRIB_CMD,      NONE,           DEF_CMD }
//,{jjBAREISS_IM, BAREISS_CMD,     INTMAT_CMD,     INTMAT_CMD }
,{jjBAREISS,    BAREISS_CMD,     LIST_CMD,       MODUL_CMD }
,{jjBETTI,      BETTI_CMD,       INTMAT_CMD,     LIST_CMD }
,{syBetti1,     BETTI_CMD,       INTMAT_CMD,     RESOLUTION_CMD }
,{jjCHAR,       CHARACTERISTIC_CMD, INT_CMD,     RING_CMD }
,{jjCHAR,       CHARACTERISTIC_CMD, INT_CMD,     QRING_CMD }
#ifdef HAVE_FACTORY
,{jjCHARSERIES, CHAR_SERIES_CMD, MATRIX_CMD,     IDEAL_CMD }
#else
,{jjWRONG,      CHAR_SERIES_CMD, MATRIX_CMD,     IDEAL_CMD }
#endif
,{jjrCharStr,   CHARSTR_CMD,     XS(STRING_CMD), RING_CMD }
,{jjrCharStr,   CHARSTR_CMD,     XS(STRING_CMD), QRING_CMD }
,{jjOpenClose,  CLOSE_CMD,       NONE,           LINK_CMD }
//,{jjWRONG,      COLS_CMD,        0,              VECTOR_CMD }
,{jjCOLS,       COLS_CMD,        INT_CMD,        MATRIX_CMD }
,{jjCOLS,       COLS_CMD,        INT_CMD,        IDEAL_CMD }
,{jjCOLS,       COLS_CMD,        INT_CMD,        MODUL_CMD }
,{jjCOLS_IV,    COLS_CMD,        INT_CMD,        INTMAT_CMD }
,{jjWRONG,      COLS_CMD,        0,              INTVEC_CMD }
,{jjCONTENT,    CONTENT_CMD,     POLY_CMD,       POLY_CMD }
,{jjCONTENT,    CONTENT_CMD,     VECTOR_CMD,     VECTOR_CMD }
,{jjCOUNT_N,    COUNT_CMD,       INT_CMD,        NUMBER_CMD }
,{jjCOUNT_RES,  COUNT_CMD,       XS(INT_CMD),    RESOLUTION_CMD }
,{jjstrlen,     COUNT_CMD,       XS(INT_CMD),    STRING_CMD }
,{jjpLength,    COUNT_CMD,       XS(INT_CMD),    POLY_CMD }
,{jjpLength,    COUNT_CMD,       XS(INT_CMD),    VECTOR_CMD }
,{jjidElem,     COUNT_CMD,       XS(INT_CMD),    IDEAL_CMD }
,{jjidElem,     COUNT_CMD,       XS(INT_CMD),    MODUL_CMD }
,{jjCOUNT_M,    COUNT_CMD,       INT_CMD,        MATRIX_CMD }
,{jjCOUNT_IV,   COUNT_CMD,       INT_CMD,        INTVEC_CMD }
,{jjCOUNT_IV,   COUNT_CMD,       INT_CMD,        INTMAT_CMD }
,{jjCOUNT_L,    COUNT_CMD,       INT_CMD,        LIST_CMD }
,{jjCOUNT_RG,   COUNT_CMD,       INT_CMD,        RING_CMD }
,{jjNULL,       DEF_CMD,         DEF_CMD,        INT_CMD }
,{jjWRONG,      DEF_CMD,         0,              ANY_TYPE }
,{jjDEG,        DEG_CMD,         INT_CMD,        POLY_CMD }
,{jjDEG,        DEG_CMD,         INT_CMD,        VECTOR_CMD }
,{jjDEGREE,     DEGREE_CMD,      NONE,           IDEAL_CMD }
,{jjDEGREE,     DEGREE_CMD,      NONE,           MODUL_CMD }
,{jjDEFINED,    DEFINED_CMD,     INT_CMD,        DEF_CMD }
#ifdef HAVE_FACTORY
,{jjDET_I,      DET_CMD,         INT_CMD,        INTMAT_CMD }
,{jjDET,        DET_CMD,         POLY_CMD,       MATRIX_CMD }
,{jjDET_S,      DET_CMD,         POLY_CMD,       MODUL_CMD }
#else
,{jjWRONG,      DET_CMD,         INT_CMD,        INTMAT_CMD }
,{jjmpDetBareiss,DET_CMD,        XS(POLY_CMD),   MATRIX_CMD }
#endif
,{jjDIM,        DIM_CMD,         INT_CMD,        IDEAL_CMD }
,{jjDIM,        DIM_CMD,         INT_CMD,        MODUL_CMD }
,{jjDIM_R,      DIM_CMD,         XS(INT_CMD),    RESOLUTION_CMD }
,{jjDUMP,       DUMP_CMD,        NONE,           LINK_CMD }
,{jjE,          E_CMD,           VECTOR_CMD,     INT_CMD }
,{jjEXECUTE,    EXECUTE_CMD,     NONE,           STRING_CMD }
,{jjERROR,      ERROR_CMD,       NONE,           STRING_CMD }
#ifdef HAVE_FACTORY
,{jjFAC_P,      FAC_CMD,         LIST_CMD,       POLY_CMD }
#else
,{jjWRONG,      FAC_CMD,         LIST_CMD,       POLY_CMD }
#endif
#ifdef HAVE_FGLM
,{findUniProc,  FINDUNI_CMD,     IDEAL_CMD,      IDEAL_CMD}
#else
,{jjWRONG,      FINDUNI_CMD,     IDEAL_CMD,      IDEAL_CMD}
#endif
,{jjidFreeModule,FREEMODULE_CMD, XS(MODUL_CMD),  INT_CMD }
#ifdef HAVE_FACTORY
,{jjFACSTD,     FACSTD_CMD,      LIST_CMD,       IDEAL_CMD }
#else
,{jjWRONG,      FACSTD_CMD,      LIST_CMD,       IDEAL_CMD }
#endif
,{jjGETDUMP,    GETDUMP_CMD,     NONE,           LINK_CMD }
,{jjHIGHCORNER, HIGHCORNER_CMD,  POLY_CMD,       IDEAL_CMD }
,{jjHIGHCORNER_M, HIGHCORNER_CMD,VECTOR_CMD,     MODUL_CMD }
,{jjHILBERT,    HILBERT_CMD,     NONE,           IDEAL_CMD }
,{jjHILBERT,    HILBERT_CMD,     NONE,           MODUL_CMD }
,{jjHILBERT_IV, HILBERT_CMD,     INTVEC_CMD,     INTVEC_CMD }
,{jjHOMOG1,     HOMOG_CMD,       INT_CMD,        IDEAL_CMD }
,{jjHOMOG1,     HOMOG_CMD,       INT_CMD,        MODUL_CMD }
,{jjidVec2Ideal,IDEAL_CMD,       XS(IDEAL_CMD),  VECTOR_CMD }
,{jjIDEAL_Ma,   IDEAL_CMD,       IDEAL_CMD,      MATRIX_CMD }
,{jjIDEAL_R,    IDEAL_CMD,       IDEAL_CMD,      QRING_CMD }
,{jjIDEAL_R,    IDEAL_CMD,       IDEAL_CMD,      RING_CMD }
,{jjIDEAL_Map,  IDEAL_CMD,       IDEAL_CMD,      MAP_CMD }
,{jjDUMMY,      IDEAL_CMD,       IDEAL_CMD,      IDEAL_CMD }
,{jjIMPART,     IMPART_CMD,      NUMBER_CMD,     NUMBER_CMD }
,{jjINDEPSET,   INDEPSET_CMD,    INTVEC_CMD,     IDEAL_CMD }
,{jjDUMMY,      INT_CMD,         INT_CMD,        INT_CMD }
,{jjnInt,       INT_CMD,         INT_CMD,        NUMBER_CMD }
,{jjP2I,        INT_CMD,         INT_CMD,        POLY_CMD }
,{jjINTERRED,   INTERRED_CMD,    IDEAL_CMD,      IDEAL_CMD }
,{jjINTERRED,   INTERRED_CMD,    MODUL_CMD,      MODUL_CMD }
,{jjDUMMY,      INTMAT_CMD,      INTMAT_CMD,     INTMAT_CMD }
,{jjIm2Iv,      INTVEC_CMD,      INTVEC_CMD,     INTMAT_CMD }
,{jjDUMMY,      INTVEC_CMD,      INTVEC_CMD,     INTVEC_CMD }
,{jjIS_RINGVAR_P, IS_RINGVAR,    INT_CMD,        POLY_CMD }
,{jjIS_RINGVAR_S, IS_RINGVAR,    INT_CMD,        STRING_CMD }
,{jjIS_RINGVAR0,IS_RINGVAR,      INT_CMD,        ANY_TYPE }
,{jjJACOB_P,    JACOB_CMD,       IDEAL_CMD,      POLY_CMD }
,{mpJacobi,     JACOB_CMD,       MATRIX_CMD,     IDEAL_CMD }
,{jjKBASE,      KBASE_CMD,       IDEAL_CMD,      IDEAL_CMD }
,{jjKBASE,      KBASE_CMD,       MODUL_CMD,      MODUL_CMD }
,{atKILLATTR1,  KILLATTR_CMD,    NONE,           IDHDL }
#ifdef MDEBUG
,{jjpHead,      LEAD_CMD,        POLY_CMD,       POLY_CMD }
#else
,{jjpHead,      LEAD_CMD,        XS(POLY_CMD),   POLY_CMD }
#endif
,{jjidHead,     LEAD_CMD,        XS(IDEAL_CMD),  IDEAL_CMD }
#ifdef MDEBUG
,{jjpHead,      LEAD_CMD,        VECTOR_CMD,     VECTOR_CMD }
#else
,{jjpHead,      LEAD_CMD,        XS(VECTOR_CMD), VECTOR_CMD }
#endif
,{jjidHead,     LEAD_CMD,        XS(MODUL_CMD),  MODUL_CMD }
,{jjLEADCOEF,   LEADCOEF_CMD,    NUMBER_CMD,     POLY_CMD }
,{jjLEADCOEF,   LEADCOEF_CMD,    NUMBER_CMD,     VECTOR_CMD }
,{jjLEADEXP,    LEADEXP_CMD,     INTVEC_CMD,     POLY_CMD }
,{jjLEADEXP,    LEADEXP_CMD,     INTVEC_CMD,     VECTOR_CMD }
,{jjLEADMONOM,  LEADMONOM_CMD,   POLY_CMD,       POLY_CMD }
,{jjLEADMONOM,  LEADMONOM_CMD,   VECTOR_CMD,     VECTOR_CMD }
,{jjLOAD1,      LIB_CMD,         NONE,           STRING_CMD }
,{jjDUMMY,      LINK_CMD,        LINK_CMD,       LINK_CMD}
,{jjCALL1MANY,  LIST_CMD,        LIST_CMD,       DEF_CMD }
,{jjWRONG,      MAP_CMD,         0,              ANY_TYPE}
,{jjDUMMY,      MATRIX_CMD,      MATRIX_CMD,     MATRIX_CMD }
,{jjidMaxIdeal, MAXID_CMD,       XS(IDEAL_CMD),  INT_CMD }
,{jjMEMORY,     MEMORY_CMD,      INT_CMD,        INT_CMD }
,{jjidMinBase,  MINBASE_CMD,     XS(IDEAL_CMD),  IDEAL_CMD }
,{jjidMinBase,  MINBASE_CMD,     XS(MODUL_CMD),  MODUL_CMD }
,{jjMINRES,     MINRES_CMD,      LIST_CMD,       LIST_CMD }
,{jjMINRES_R,   MINRES_CMD,      XS(RESOLUTION_CMD), RESOLUTION_CMD }
,{jjDUMMY,      MODUL_CMD,       MODUL_CMD,      MODUL_CMD }
,{jjMONITOR1,   MONITOR_CMD,     NONE,           STRING_CMD }
,{jjMULT,       MULTIPLICITY_CMD,  INT_CMD,      IDEAL_CMD }
,{jjMULT,       MULTIPLICITY_CMD,  INT_CMD,      MODUL_CMD }
,{jjMSTD,       MSTD_CMD,        LIST_CMD,       IDEAL_CMD }
,{jjMSTD,       MSTD_CMD,        LIST_CMD,       MODUL_CMD }
,{jjNAMEOF,     NAMEOF_CMD,      STRING_CMD,     ANY_TYPE }
,{jjNAMES,      NAMES_CMD,       LIST_CMD,       PACKAGE_CMD }
,{jjNAMES,      NAMES_CMD,       LIST_CMD,       RING_CMD }
,{jjNAMES,      NAMES_CMD,       LIST_CMD,       QRING_CMD }
,{jjDUMMY,      NUMBER_CMD,      NUMBER_CMD,     NUMBER_CMD }
,{jjP2N,        NUMBER_CMD,      NUMBER_CMD,     POLY_CMD }
,{jjRPAR,       NPARS_CMD,       INT_CMD,        RING_CMD }
,{jjRPAR,       NPARS_CMD,       INT_CMD,        QRING_CMD }
,{jjNVARS,      NVARS_CMD,       INT_CMD,        RING_CMD }
,{jjNVARS,      NVARS_CMD,       INT_CMD,        QRING_CMD }
,{jjOpenClose,  OPEN_CMD,        NONE,           LINK_CMD }
,{jjORD,        ORD_CMD,         INT_CMD,        POLY_CMD }
,{jjORD,        ORD_CMD,         INT_CMD,        VECTOR_CMD }
,{jjrOrdStr,    ORDSTR_CMD,      XS(STRING_CMD), RING_CMD }
,{jjrOrdStr,    ORDSTR_CMD,      XS(STRING_CMD), QRING_CMD }
,{jjPAR1,       PAR_CMD,         NUMBER_CMD,     INT_CMD }
,{jjPARDEG,     PARDEG_CMD,      INT_CMD,        NUMBER_CMD }
,{jjPARSTR1,    PARSTR_CMD,      STRING_CMD,     INT_CMD }
,{jjrParStr,    PARSTR_CMD,      XS(STRING_CMD), RING_CMD }
,{jjrParStr,    PARSTR_CMD,      XS(STRING_CMD), QRING_CMD }
,{jjDUMMY,      POLY_CMD,        POLY_CMD,       POLY_CMD }
,{jjPREIMAGE_R, PREIMAGE_CMD,    RING_CMD,       MAP_CMD }
,{jjPRIME,      PRIME_CMD,       INT_CMD,        INT_CMD }
,{jjPRINT,      PRINT_CMD,       NONE,           LIST_CMD}
,{jjPRINT,      PRINT_CMD,       NONE,           DEF_CMD}
,{jjPRUNE,      PRUNE_CMD,       MODUL_CMD,      MODUL_CMD }
,{kQHWeight,    QHWEIGHT_CMD,    INTVEC_CMD,     IDEAL_CMD }
,{kQHWeight,    QHWEIGHT_CMD,    INTVEC_CMD,     MODUL_CMD }
,{jjWRONG,      QRING_CMD,       0,              ANY_TYPE}
,{jjREAD,       READ_CMD,        STRING_CMD,     LINK_CMD }
,{jjREGULARITY, REGULARITY_CMD,  INT_CMD,        LIST_CMD }
,{jjREPART,     REPART_CMD,      NUMBER_CMD,     NUMBER_CMD }
,{jjRESERVEDNAME,RESERVEDNAME_CMD, INT_CMD,      STRING_CMD }
,{jjL2R,        RESOLUTION_CMD,  RESOLUTION_CMD, LIST_CMD }
,{jjDUMMY,      RESOLUTION_CMD,  RESOLUTION_CMD, RESOLUTION_CMD }
,{jjRINGLIST,   RINGLIST_CMD,    LIST_CMD,       RING_CMD }
,{jjRINGLIST,   RINGLIST_CMD,    LIST_CMD,       QRING_CMD }
,{jjDUMMY,      RING_CMD,        RING_CMD,       RING_CMD}
,{jjLISTRING,   RING_CMD,        RING_CMD,       LIST_CMD}
//,{jjWRONG,      ROWS_CMD,        0,              POLY_CMD }
,{jjpMaxComp,   ROWS_CMD,        XS(INT_CMD),    VECTOR_CMD }
,{jjROWS,       ROWS_CMD,        INT_CMD,        MODUL_CMD }
,{jjROWS,       ROWS_CMD,        INT_CMD,        MATRIX_CMD }
,{jjROWS_IV,    ROWS_CMD,        INT_CMD,        INTMAT_CMD }
,{jjCOUNT_IV,   ROWS_CMD,        INT_CMD,        INTVEC_CMD }
,{jjSort_Id,    SORTVEC_CMD,     INTVEC_CMD,     IDEAL_CMD }
,{jjSort_Id,    SORTVEC_CMD,     INTVEC_CMD,     MODUL_CMD }
,{jjSTD,        STD_CMD,         IDEAL_CMD,      IDEAL_CMD }
,{jjSTD,        STD_CMD,         MODUL_CMD,      MODUL_CMD }
,{jjDUMMY,      STRING_CMD,      STRING_CMD,     STRING_CMD }
,{jjSYSTEM,     SYSTEM_CMD,      NONE,           STRING_CMD }
,{jjSYZYGY,     SYZYGY_CMD,      MODUL_CMD,      IDEAL_CMD }
,{jjSYZYGY,     SYZYGY_CMD,      MODUL_CMD,      MODUL_CMD }
,{jjWRONG,      TRACE_CMD,       0,              INTVEC_CMD }
,{jjWRONG,      TRACE_CMD,       0,              IDEAL_CMD }
,{jjTRACE_IV,   TRACE_CMD,       INT_CMD,        INTMAT_CMD }
,{jjmpTrace,    TRACE_CMD,       XS(POLY_CMD),   MATRIX_CMD }
,{jjTRANSP_IV,  TRANSPOSE_CMD,   INTMAT_CMD,     INTVEC_CMD }
,{jjTRANSP_IV,  TRANSPOSE_CMD,   INTMAT_CMD,     INTMAT_CMD }
,{jjmpTransp,   TRANSPOSE_CMD,   XS(MATRIX_CMD), MATRIX_CMD }
,{jjidTransp,   TRANSPOSE_CMD,   XS(MODUL_CMD),  MODUL_CMD }
,{jjTYPEOF,     TYPEOF_CMD,      STRING_CMD,     ANY_TYPE }
,{jjDUMMY,      VECTOR_CMD,      VECTOR_CMD,     VECTOR_CMD }
,{jjVDIM,       VDIM_CMD,        INT_CMD,        IDEAL_CMD }
,{jjVDIM,       VDIM_CMD,        INT_CMD,        MODUL_CMD }
,{jjVAR1,       VAR_CMD,         POLY_CMD,       INT_CMD }
,{jjVARSTR1,    VARSTR_CMD,      STRING_CMD,     INT_CMD }
,{jjrVarStr,    VARSTR_CMD,      XS(STRING_CMD), RING_CMD }
,{jjrVarStr,    VARSTR_CMD,      XS(STRING_CMD), QRING_CMD }
,{kWeight,      WEIGHT_CMD,      INTVEC_CMD,     IDEAL_CMD }
,{kWeight,      WEIGHT_CMD,      INTVEC_CMD,     MODUL_CMD }
,{jjLOAD1,      LOAD_CMD,        NONE,           STRING_CMD }
,{loNewtonP,    NEWTONPOLY_CMD,  IDEAL_CMD,      IDEAL_CMD}
,{NULL,         0,               0,              0}
};
#undef s
/*=================== operations with 3 args.: static proc =================*/
static BOOLEAN jjBRACK_S(leftv res, leftv u, leftv v,leftv w)
{
  char *s= (char *)u->Data();
  int   r = (int)v->Data();
  int   c = (int)w->Data();
  int l = strlen(s);

  if ( (r<1) || (r>l) || (c<0) )
  {
    Werror("wrong range[%d,%d] in string %s",r,c,u->Fullname());
    return TRUE;
  }
  res->data = (char *)omAlloc(c+1);
  sprintf((char *)res->data,"%-*.*s",c,c,s+r-1);
  return FALSE;
}
static BOOLEAN jjBRACK_Im(leftv res, leftv u, leftv v,leftv w)
{
  intvec *iv = (intvec *)u->Data();
  int   r = (int)v->Data();
  int   c = (int)w->Data();
  if ((r<1)||(r>iv->rows())||(c<1)||(c>iv->cols()))
  {
    Werror("wrong range[%d,%d] in intmat %s(%d x %d)",
           r,c,u->Fullname(),iv->rows(),iv->cols());
    return TRUE;
  }
  res->data=u->data;
  u->data=NULL;
  res->rtyp=u->rtyp;
  u->rtyp=0;
  res->name=u->name;
  u->name=NULL;
  Subexpr e=jjMakeSub(v);
          e->next=jjMakeSub(w);
  if (u->e==NULL) res->e=e;
  else
  {
    Subexpr h=u->e;
    while (h->next!=NULL) h=h->next;
    h->next=e;
    res->e=u->e;
    u->e=NULL;
  }
  return FALSE;
}
static BOOLEAN jjBRACK_Ma(leftv res, leftv u, leftv v,leftv w)
{
  matrix m= (matrix)u->Data();
  int   r = (int)v->Data();
  int   c = (int)w->Data();
  //Print("gen. elem %d, %d\n",r,c);
  if ((r<1)||(r>MATROWS(m))||(c<1)||(c>MATCOLS(m)))
  {
    Werror("wrong range[%d,%d] in matrix %s(%d x %d)",r,c,u->Fullname(),
      MATROWS(m),MATCOLS(m));
    return TRUE;
  }
  res->data=u->data;
  u->data=NULL;
  res->rtyp=u->rtyp;
  u->rtyp=0;
  res->name=u->name;
  u->name=NULL;
  Subexpr e=jjMakeSub(v);
          e->next=jjMakeSub(w);
  if (u->e==NULL)
    res->e=e;
  else
  {
    Subexpr h=u->e;
    while (h->next!=NULL) h=h->next;
    h->next=e;
    res->e=u->e;
    u->e=NULL;
  }
  return FALSE;
}
static BOOLEAN jjBRACK_Ma_I_IV(leftv res, leftv u, leftv v,leftv w)
{
  sleftv t;
  sleftv ut;
  leftv p=NULL;
  intvec *iv=(intvec *)w->Data();
  int l;
  BOOLEAN nok;

  if ((u->rtyp!=IDHDL)||(u->e!=NULL))
  {
    WerrorS("cannot build expression lists from unnamed objects");
    return TRUE;
  }
  memcpy(&ut,u,sizeof(ut));
  memset(&t,0,sizeof(t));
  t.rtyp=INT_CMD;
  for (l=0;l< iv->length(); l++)
  {
    t.data=(char *)(*iv)[l];
    if (p==NULL)
    {
      p=res;
    }
    else
    {
      p->next=(leftv)omAlloc0Bin(sleftv_bin);
      p=p->next;
    }
    memcpy(u,&ut,sizeof(ut));
    if (u->Typ() == MATRIX_CMD)
      nok=jjBRACK_Ma(p,u,v,&t);
    else /* INTMAT_CMD */
      nok=jjBRACK_Im(p,u,v,&t);
    if (nok)
    {
      while (res->next!=NULL)
      {
        p=res->next->next;
        omFreeBin((ADDRESS)res->next, sleftv_bin);
        // res->e aufraeumen !!!!
        res->next=p;
      }
      return TRUE;
    }
  }
  return FALSE;
}
static BOOLEAN jjBRACK_Ma_IV_I(leftv res, leftv u, leftv v,leftv w)
{
  sleftv t;
  sleftv ut;
  leftv p=NULL;
  intvec *iv=(intvec *)v->Data();
  int l;
  BOOLEAN nok;

  if ((u->rtyp!=IDHDL)||(u->e!=NULL))
  {
    WerrorS("cannot build expression lists from unnamed objects");
    return TRUE;
  }
  memcpy(&ut,u,sizeof(ut));
  memset(&t,0,sizeof(t));
  t.rtyp=INT_CMD;
  for (l=0;l< iv->length(); l++)
  {
    t.data=(char *)((*iv)[l]);
    if (p==NULL)
    {
      p=res;
    }
    else
    {
      p->next=(leftv)omAlloc0Bin(sleftv_bin);
      p=p->next;
    }
    memcpy(u,&ut,sizeof(ut));
    if (u->Typ() == MATRIX_CMD)
      nok=jjBRACK_Ma(p,u,&t,w);
    else /* INTMAT_CMD */
      nok=jjBRACK_Im(p,u,&t,w);
    if (nok)
    {
      while (res->next!=NULL)
      {
        p=res->next->next;
        omFreeBin((ADDRESS)res->next, sleftv_bin);
        // res->e aufraeumen !!
        res->next=p;
      }
      return TRUE;
    }
  }
  return FALSE;
}
static BOOLEAN jjBRACK_Ma_IV_IV(leftv res, leftv u, leftv v,leftv w)
{
  sleftv t1,t2,ut;
  leftv p=NULL;
  intvec *vv=(intvec *)v->Data();
  intvec *wv=(intvec *)w->Data();
  int vl;
  int wl;
  BOOLEAN nok;

  if ((u->rtyp!=IDHDL)||(u->e!=NULL))
  {
    WerrorS("cannot build expression lists from unnamed objects");
    return TRUE;
  }
  memcpy(&ut,u,sizeof(ut));
  memset(&t1,0,sizeof(sleftv));
  memset(&t2,0,sizeof(sleftv));
  t1.rtyp=INT_CMD;
  t2.rtyp=INT_CMD;
  for (vl=0;vl< vv->length(); vl++)
  {
    t1.data=(char *)((*vv)[vl]);
    for (wl=0;wl< wv->length(); wl++)
    {
      t2.data=(char *)((*wv)[wl]);
      if (p==NULL)
      {
        p=res;
      }
      else
      {
        p->next=(leftv)omAlloc0Bin(sleftv_bin);
        p=p->next;
      }
      memcpy(u,&ut,sizeof(ut));
      if (u->Typ() == MATRIX_CMD)
        nok=jjBRACK_Ma(p,u,&t1,&t2);
      else /* INTMAT_CMD */
        nok=jjBRACK_Im(p,u,&t1,&t2);
      if (nok)
      {
        res->CleanUp();
        return TRUE;
      }
    }
  }
  return FALSE;
}
static BOOLEAN jjPROC3(leftv res, leftv u, leftv v, leftv w)
{
  v->next=(leftv)omAllocBin(sleftv_bin);
  memcpy(v->next,w,sizeof(sleftv));
  BOOLEAN r=iiExprArith2(res,u,'(',v);
  v->rtyp=0; v->data=NULL;
  w->rtyp=0; w->data=NULL;
  return r;
}
static BOOLEAN jjCALL3MANY(leftv res, leftv u, leftv v, leftv w)
{
  u->next=(leftv)omAllocBin(sleftv_bin);
  memcpy(u->next,v,sizeof(sleftv));
  u->next->next=(leftv)omAllocBin(sleftv_bin);
  memcpy(u->next->next,w,sizeof(sleftv));
  BOOLEAN r=iiExprArithM(res,u,iiOp);
  // iiExprArithM did the CleanUp
  w->rtyp=0; w->data=NULL;
  return r;
}
static BOOLEAN jjBAREISS3(leftv res, leftv u, leftv v, leftv w)
{
  lists l;
  int k=(int)w->Data();
  if (k>=0)
  {
    l=smCallNewBareiss((ideal)u->Data(),(int)v->Data(),(int)w->Data());
  }
  else
  {
    l=smCallSolv((ideal)u->Data());
  }
  res->data = (char *)l;
  return FALSE;
}
static BOOLEAN jjCOEFFS3_Id(leftv res, leftv u, leftv v, leftv w)
{
  if ((w->rtyp!=IDHDL)||(w->e!=NULL))
  {
    WerrorS("3rd argument must be a name of a matrix");
    return TRUE;
  }
  ideal i=(ideal)u->Data();
  int rank=(int)i->rank;
  BOOLEAN r=jjCOEFFS_Id(res,u,v);
  if (r) return TRUE;
  mpMonomials((matrix)res->data, rank, pVar((poly)v->Data()),(matrix)w->Data());
  return FALSE;
}
static BOOLEAN jjCOEFFS3_KB(leftv res, leftv u, leftv v, leftv w)
{
  res->data=(void*)idCoeffOfKBase((ideal)(u->Data()),
           (ideal)(v->Data()),(poly)(w->Data()));
  return FALSE;
}
static BOOLEAN jjCOEFFS3_P(leftv res, leftv u, leftv v, leftv w)
{
  if ((w->rtyp!=IDHDL)||(w->e!=NULL))
  {
    WerrorS("3rd argument must be a name of a matrix");
    return TRUE;
  }
  // CopyD for POLY_CMD and VECTOR_CMD are identical:
  poly p=(poly)u->CopyD(POLY_CMD);
  ideal i=idInit(1,1);
  i->m[0]=p;
  sleftv t;
  memset(&t,0,sizeof(t));
  t.data=(char *)i;
  t.rtyp=IDEAL_CMD;
  int rank=1;
  if (u->Typ()==VECTOR_CMD)
  {
    i->rank=rank=pMaxComp(p);
    t.rtyp=MODUL_CMD;
  }
  BOOLEAN r=jjCOEFFS_Id(res,&t,v);
  t.CleanUp();
  if (r) return TRUE;
  mpMonomials((matrix)res->data, rank, pVar((poly)v->Data()),(matrix)w->Data());
  return FALSE;
}
static BOOLEAN jjELIMIN_HILB(leftv res, leftv u, leftv v, leftv w)
{
  res->data=(char *)idElimination((ideal)u->Data(),(poly)v->Data(),
    (intvec *)w->Data());
  setFlag(res,FLAG_STD);
  return FALSE;
}
static BOOLEAN jjFIND3(leftv res, leftv u, leftv v, leftv w)
{
  /*4
  * look for the substring what in the string where
  * starting at position n
  * return the position of the first char of what in where
  * or 0
  */
  int n=(int)w->Data();
  char *where=(char *)u->Data();
  char *what=(char *)v->Data();
  char *found;
  if ((1>n)||(n>(int)strlen(where)))
  {
    Werror("start position %d out of range",n);
    return TRUE;
  }
  found = strchr(where+n-1,*what);
  if (*(what+1)!='\0')
  {
    while((found !=NULL) && (strncmp(found+1,what+1,strlen(what+1))!=0))
    {
      found=strchr(found+1,*what);
    }
  }
  if (found != NULL)
  {
    res->data=(char *)((found-where)+1);
  }
  return FALSE;
}
static BOOLEAN jjHILBERT3(leftv res, leftv u, leftv v, leftv w)
{
  assumeStdFlag(u);
  intvec *module_w=(intvec *)atGet(u,"isHomog");
  intvec *wdegree=(intvec*)w->Data();
  if (wdegree->length()!=pVariables)
  {
    Werror("weight vector must have size %d, not %d",
           pVariables,wdegree->length());
    return TRUE;
  }
  intvec *iv=hFirstSeries((ideal)u->Data(),module_w,currQuotient,wdegree);
  switch((int)v->Data())
  {
    case 1:
      res->data=(void *)iv;
      return FALSE;
    case 2:
      res->data=(void *)hSecondSeries(iv);
      delete iv;
      return FALSE;
  }
  WerrorS(feNotImplemented);
  delete iv;
  return TRUE;
}
static BOOLEAN jjINTMAT3(leftv res, leftv u, leftv v,leftv w)
{
  intvec* im= new intvec((int)v->Data(),(int)w->Data(), 0);
  intvec* arg = (intvec*) u->Data();
  int i, n = min(im->cols()*im->rows(), arg->cols()*arg->rows());

  for (i=0; i<n; i++)
  {
    (*im)[i] = (*arg)[i];
  }

  res->data = (char *)im;
  return FALSE;
}
static BOOLEAN jjJET_P_IV(leftv res, leftv u, leftv v, leftv w)
{
  short *iw=iv2array((intvec *)w->Data());
  res->data = (char *)ppJetW((poly)u->Data(),(int)v->Data(),iw);
  omFreeSize((ADDRESS)iw,(pVariables+1)*sizeof(short));
  return FALSE;
}
static BOOLEAN jjJET_P_P(leftv res, leftv u, leftv v, leftv w)
{
  if (!pIsUnit((poly)v->Data()))
  {
    WerrorS("2nd argument must be a unit");
    return TRUE;
  }
  res->data = (char *)pSeries((int)w->Data(),(poly)u->CopyD(),(poly)v->CopyD());
  return FALSE;
}
static BOOLEAN jjJET_ID_IV(leftv res, leftv u, leftv v, leftv w)
{
  res->data = (char *)idJetW((ideal)u->Data(),(int)v->Data(),
                             (intvec *)w->Data());
  return FALSE;
}
static BOOLEAN jjJET_ID_M(leftv res, leftv u, leftv v, leftv w)
{
  if (!mpIsDiagUnit((matrix)v->Data()))
  {
    WerrorS("2nd argument must be a diagonal matrix of units");
    return TRUE;
  }
  res->data = (char *)idSeries((int)w->Data(),(ideal)u->CopyD(),
                               (matrix)v->CopyD());
  return FALSE;
}
static BOOLEAN jjMINOR3(leftv res, leftv u, leftv v, leftv w)
{
  assumeStdFlag(w);
  res->data = (char *)idMinors(
                        (matrix)u->Data(),(int)v->Data(),(ideal)w->Data());
  return FALSE;
}
static BOOLEAN jjPREIMAGE(leftv res, leftv u, leftv v, leftv w)
{
  idhdl h;
  ring rr;
  map mapping;

  if ((v->name==NULL) || (w->name==NULL))
  {
    WerrorS("2nd/3rd arguments must have names");
    return TRUE;
  }
  rr=(ring)u->Data();
  const char *ring_name=u->Name();
  if ((h=rr->idroot->get(v->name,myynest))!=NULL)
  {
    if (h->typ==MAP_CMD)
    {
      mapping=IDMAP(h);
      idhdl preim_ring=IDROOT->get(mapping->preimage,myynest);
      if ((preim_ring==NULL)
      || (IDRING(preim_ring)!=currRing))
      {
        Werror("preimage ring `%s` is not the basering",mapping->preimage);
        return TRUE;
      }
    }
    else if (h->typ==IDEAL_CMD)
    {
      mapping=IDMAP(h);
    }
    else
    {
      Werror("`%s` is no map nor ideal",IDID(h));
      return TRUE;
    }
  }
  else
  {
    Werror("`%s` is not defined in `%s`",v->name,ring_name);
    return TRUE;
  }
  if ((h=rr->idroot->get(w->name,myynest))!=NULL)
  {
    if (h->typ==IDEAL_CMD)
    {
      res->data=(char *)maGetPreimage(rr,mapping,IDIDEAL(h));
    }
    else
    {
      Werror("`%s` is no ideal",IDID(h));
      return TRUE;
    }
  }
  else
  {
    Werror("`%s` is not defined in `%s`",w->name,ring_name);
    return TRUE;
  }
  return FALSE;
}
static BOOLEAN jjRANDOM_Im(leftv res, leftv u, leftv v, leftv w)
{
  int di, k;
  int i=(int)u->Data();
  int r=(int)v->Data();
  int c=(int)w->Data();
  intvec *iv = new intvec(r, c, 0);
  if (iv->rows()==0)
  {
    delete iv;
    return TRUE;
  }
  if (i!=0)
  {
    if (i<0) i = -i;
    di = 2 * i + 1;
    for (k=0; k<iv->length(); k++)
    {
#ifdef buildin_rand
      (*iv)[k] = ((siRand() % di) - i);
#else
      (*iv)[k] = ((rand() % di) - i);
#endif
    }
  }
  res->data = (char *)iv;
  return FALSE;
}
static BOOLEAN jjSUBST_Test(leftv v,leftv w,
  int &ringvar, poly &monomexpr)
{
  monomexpr=(poly)w->Data();
  if (pLength(monomexpr)>1)
  {
    Werror("`%s` substitutes a ringvar only by a term",
      Tok2Cmdname(SUBST_CMD));
    return TRUE;
  }
  if (!(ringvar=pVar((poly)v->Data())))
  {
    WerrorS("ringvar expected");
    return TRUE;
  }
  return FALSE;
}
static BOOLEAN jjSUBST_P(leftv res, leftv u, leftv v,leftv w)
{
  int ringvar;
  poly monomexpr;
  BOOLEAN nok=jjSUBST_Test(v,w,ringvar,monomexpr);
  if (nok) return TRUE;
  res->data = pSubst((poly)u->CopyD(res->rtyp),ringvar,monomexpr);
  return FALSE;
}
static BOOLEAN jjSUBST_Id(leftv res, leftv u, leftv v,leftv w)
{
  int ringvar;
  poly monomexpr;
  BOOLEAN nok=jjSUBST_Test(v,w,ringvar,monomexpr);
  if (nok) return TRUE;
  res->data = idSubst((ideal)u->CopyD(res->rtyp),ringvar,monomexpr);
  return FALSE;
}
// we do not want to have jjSUBST_Id_X inlined:
static BOOLEAN jjSUBST_Id_X(leftv res, leftv u, leftv v,leftv w,
                            int input_type);
static BOOLEAN jjSUBST_Id_I(leftv res, leftv u, leftv v,leftv w)
{
  return jjSUBST_Id_X(res,u,v,w,INT_CMD);
}
static BOOLEAN jjSUBST_Id_N(leftv res, leftv u, leftv v,leftv w)
{
  return jjSUBST_Id_X(res,u,v,w,NUMBER_CMD);
}
static BOOLEAN jjSUBST_Id_X(leftv res, leftv u, leftv v,leftv w, int input_type)
{
  sleftv tmp;
  memset(&tmp,0,sizeof(tmp));
  // do not check the result, conversion from int/number to poly works always
  iiConvert(input_type,POLY_CMD,iiTestConvert(input_type,POLY_CMD),w,&tmp);
  BOOLEAN b=jjSUBST_Id(res,u,v,&tmp);
  tmp.CleanUp();
  return b;
}
static BOOLEAN jjMATRIX_Id(leftv res, leftv u, leftv v,leftv w)
{
  matrix m=mpNew((int)v->Data(),(int)w->Data());
  ideal I=(ideal)u->CopyD(IDEAL_CMD);
  int i=min(IDELEMS(I),(int)v->Data()*(int)w->Data());
  //for(i=i-1;i>=0;i--)
  //{
  //  m->m[i]=I->m[i];
  //  I->m[i]=NULL;
  //}
  memcpy4(m->m,I->m,i*sizeof(poly));
  memset(I->m,0,i*sizeof(poly));
  idDelete(&I);
  res->data = (char *)m;
  return FALSE;
}
static BOOLEAN jjSUBST_Par_N(leftv res, leftv u, leftv v,leftv w)
{
  // u: to change poly
  // v: number (parameter)
  // w: image of u (number)
  int i;
  lnumber a=(lnumber)v->Data();
  if (naParDeg((number)a)!=1)
  {
    WerrorS("first argument is not a parameter");
    return TRUE;
  }
  int r=rPar(currRing);
  ideal G=idMaxIdeal(1);
  ideal F=idInit(r,1);
  int par=-1;
  for(i=1;i<=r;i++)
  {
    if (napGetExp(a->z,i)!=0) { par=i; break;}
  }
  for(i=0;i<r;i++)
  {
    if (i==par) F->m[i]=(poly)w->CopyD();
    else
    {
      F->m[i]=pOne();pSetCoeff( F->m[i],naPar(i+1));
    }
  }
  res->data=(poly)maAlgpolyMap(currRing,(poly)u->Data(),F,G);
  idDelete(&F);
  idDelete(&G);
  return FALSE;
}
static BOOLEAN jjMATRIX_Mo(leftv res, leftv u, leftv v,leftv w)
{
  res->data = (char *)idModule2formatedMatrix((ideal)u->CopyD(MODUL_CMD),
           (int)v->Data(),(int)w->Data());
  return FALSE;
}
static BOOLEAN jjMATRIX_Ma(leftv res, leftv u, leftv v,leftv w)
{
  matrix m=mpNew((int)v->Data(),(int)w->Data());
  matrix I=(matrix)u->CopyD(MATRIX_CMD);
  int r=min(MATROWS(I),(int)v->Data());
  int c=min(MATCOLS(I),(int)w->Data());
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      MATELEM(m,i,j)=MATELEM(I,i,j);
      MATELEM(I,i,j)=NULL;
    }
  }
  idDelete((ideal *)&I);
  res->data = (char *)m;
  return FALSE;
}
static BOOLEAN jjLIFT3(leftv res, leftv u, leftv v, leftv w)
{
  if (w->rtyp!=IDHDL) return TRUE;
  BITSET save_test=test;
  int ul= IDELEMS((ideal)u->Data());
  int vl= IDELEMS((ideal)v->Data());
  ideal m
    = idLift((ideal)u->Data(),(ideal)v->Data(),NULL,FALSE,hasFlag(u,FLAG_STD),
             FALSE, (matrix *)(&(IDMATRIX((idhdl)(w->data)))));
  res->data = (char *)idModule2formatedMatrix(m,ul,vl);
  test=save_test;
  return FALSE;
}
static BOOLEAN jjREDUCE3_CP(leftv res, leftv u, leftv v, leftv w)
{
  assumeStdFlag(w);
  if (!idIsZeroDim((ideal)w->Data()))
  {
    Werror("`%s` must be 0-dimensional",v->Name());
    return TRUE;
  }
  res->data = (char *)redNF((ideal)w->CopyD(),(poly)u->CopyD(),
    (poly)v->CopyD());
  return FALSE;
}
static BOOLEAN jjREDUCE3_CID(leftv res, leftv u, leftv v, leftv w)
{
  assumeStdFlag(w);
  if (!idIsZeroDim((ideal)w->Data()))
  {
    Werror("`%s` must be 0-dimensional",v->Name());
    return TRUE;
  }
  res->data = (char *)redNF((ideal)w->CopyD(),(ideal)u->CopyD(),
    (matrix)v->CopyD());
  return FALSE;
}
static BOOLEAN jjREDUCE3_P(leftv res, leftv u, leftv v, leftv w)
{
  assumeStdFlag(v);
  res->data = (char *)kNF((ideal)v->Data(),currQuotient,(poly)u->Data(),
    0,(int)w->Data());
  return FALSE;
}
static BOOLEAN jjREDUCE3_ID(leftv res, leftv u, leftv v, leftv w)
{
  assumeStdFlag(v);
  res->data = (char *)kNF((ideal)v->Data(),currQuotient,(ideal)u->Data(),
    0,(int)w->Data());
  return FALSE;
}
#ifdef OLD_RES
static BOOLEAN jjRES3(leftv res, leftv u, leftv v, leftv w)
{
  int maxl=(int)v->Data();
  int l=0;
  resolvente r;
  intvec **weights=NULL;
  int wmaxl=maxl;
  maxl--;
  if ((maxl==-1) && (iiOp!=MRES_CMD))
    maxl = pVariables-1;
  if ((iiOp == RES_CMD) || (iiOp == MRES_CMD))
  {
    intvec * iv=(intvec*)atGet(u,"isHomog");
    if (iv!=NULL)
    {
      weights = (intvec**)omAlloc0Bin(void_ptr_bin);
      weights[0] = ivCopy(iv);
      l=1;
    }
    r=syResolvente((ideal)u->Data(),maxl,&l, &weights, iiOp==MRES_CMD);
  }
  else
    r=sySchreyerResolvente((ideal)u->Data(),maxl+1,&l);
  if (r==NULL) return TRUE;
  int t3=u->Typ();
  iiMakeResolv(r,l,wmaxl,w->name,t3,weights);
  return FALSE;
  return TRUE;
}
#endif
static BOOLEAN jjRING3(leftv res, leftv u, leftv v, leftv w)
{
  res->data=(void *)rInit(u,v,w);
  return (res->data==NULL);
}
static BOOLEAN jjSTATUS3(leftv res, leftv u, leftv v, leftv w)
{
  int yes;
  jjSTATUS2(res, u, v);
  yes = (strcmp((char *) res->data, (char *) w->Data()) == 0);
  omFree((ADDRESS) res->data);
  res->data = (void *) yes;
  return FALSE;
}
static BOOLEAN jjSTD_HILB_W(leftv res, leftv u, leftv v, leftv w)
{
  ideal result;
  intvec *ww=(intvec *)atGet(u,"isHomog");
  tHomog hom=testHomog;
  if (ww!=NULL)
  {
    ww=ivCopy(ww);
    hom=isHomog;
  }
  result=kStd((ideal)(u->Data()),
              currQuotient,
              hom,
              &ww,                  // module weights
              (intvec *)v->Data(),  // hilbert series
              0,0,                  // syzComp, newIdeal
              (intvec *)w->Data()); // weights of vars
  idSkipZeroes(result);
  res->data = (char *)result;
  setFlag(res,FLAG_STD);
  if (ww!=NULL) atSet(res,omStrDup("isHomog"),ww,INTVEC_CMD);
  return FALSE;
}

/*=================== operations with 3 args.: table =================*/
struct sValCmd3 dArith3[]=
{
// operations:
// proc             cmd          res         arg1        arg2        arg3
 {jjBRACK_S,        '[',        STRING_CMD, STRING_CMD, INT_CMD,    INT_CMD }
,{jjBRACK_Im,       '[',        INT_CMD,    INTMAT_CMD, INT_CMD,    INT_CMD }
,{jjBRACK_Ma_I_IV,  '[',        INT_CMD,    INTMAT_CMD, INT_CMD,    INTVEC_CMD }
,{jjBRACK_Ma_IV_I,  '[',        INT_CMD,    INTMAT_CMD, INTVEC_CMD, INT_CMD }
,{jjBRACK_Ma_IV_IV, '[',        INT_CMD,    INTMAT_CMD, INTVEC_CMD, INTVEC_CMD }
,{jjBRACK_Ma,       '[',        POLY_CMD,   MATRIX_CMD, INT_CMD,    INT_CMD }
,{jjBRACK_Ma_I_IV,  '[',        POLY_CMD,   MATRIX_CMD, INT_CMD,    INTVEC_CMD }
,{jjBRACK_Ma_IV_I,  '[',        POLY_CMD,   MATRIX_CMD, INTVEC_CMD, INT_CMD }
,{jjBRACK_Ma_IV_IV, '[',        POLY_CMD,   MATRIX_CMD, INTVEC_CMD, INTVEC_CMD }
,{jjPROC3,          '(',        ANY_TYPE,   PROC_CMD,   DEF_CMD,    DEF_CMD }
,{atATTRIB3,        ATTRIB_CMD, NONE,       IDHDL,      STRING_CMD, DEF_CMD }
,{jjBAREISS3,       BAREISS_CMD,LIST_CMD,   MODUL_CMD,  INT_CMD,    INT_CMD }
,{jjCOEFFS3_P,      COEFFS_CMD, MATRIX_CMD, POLY_CMD,   POLY_CMD,   MATRIX_CMD }
,{jjCOEFFS3_P,      COEFFS_CMD, MATRIX_CMD, VECTOR_CMD, POLY_CMD,   MATRIX_CMD }
,{jjCOEFFS3_Id,     COEFFS_CMD, MATRIX_CMD, IDEAL_CMD,  POLY_CMD,   MATRIX_CMD }
,{jjCOEFFS3_Id,     COEFFS_CMD, MATRIX_CMD, MODUL_CMD,  POLY_CMD,   MATRIX_CMD }
,{jjCOEFFS3_KB,     COEFFS_CMD, MATRIX_CMD, IDEAL_CMD,  IDEAL_CMD,  POLY_CMD }
,{jjCOEFFS3_KB,     COEFFS_CMD, MATRIX_CMD, MODUL_CMD,  MODUL_CMD,  POLY_CMD }
,{jjELIMIN_HILB,    ELIMINATION_CMD,IDEAL_CMD, IDEAL_CMD, POLY_CMD, INTVEC_CMD }
,{jjFIND3,          FIND_CMD,   INT_CMD,    STRING_CMD, STRING_CMD, INT_CMD }
,{jjHILBERT3,       HILBERT_CMD,INTVEC_CMD, IDEAL_CMD,  INT_CMD,    INTVEC_CMD }
,{jjHILBERT3,       HILBERT_CMD,INTVEC_CMD, MODUL_CMD,  INT_CMD,    INTVEC_CMD }
,{jjCALL3MANY,      IDEAL_CMD,  IDEAL_CMD,  DEF_CMD,    DEF_CMD,    DEF_CMD }
//,{jjCALL3MANY,      INTERSECT_CMD,  NONE,   DEF_CMD,    DEF_CMD,    DEF_CMD }
,{lInsert3,         INSERT_CMD, LIST_CMD,   LIST_CMD,   DEF_CMD,    INT_CMD }
,{jjINTMAT3,        INTMAT_CMD, INTMAT_CMD, INTMAT_CMD, INT_CMD,    INT_CMD }
,{jjCALL3MANY,      INTVEC_CMD, INTVEC_CMD, DEF_CMD,    DEF_CMD,    DEF_CMD }
,{jjJET_P_IV,       JET_CMD,    POLY_CMD,   POLY_CMD,   INT_CMD,    INTVEC_CMD }
,{jjJET_ID_IV,      JET_CMD,    IDEAL_CMD,  IDEAL_CMD,  INT_CMD,    INTVEC_CMD }
,{jjJET_P_IV,       JET_CMD,    VECTOR_CMD, VECTOR_CMD, INT_CMD,    INTVEC_CMD }
,{jjJET_ID_IV,      JET_CMD,    MODUL_CMD,  MODUL_CMD,  INT_CMD,    INTVEC_CMD }
,{jjJET_P_P,        JET_CMD,    POLY_CMD,   POLY_CMD,   POLY_CMD,   INT_CMD }
,{jjJET_P_P,        JET_CMD,    VECTOR_CMD, VECTOR_CMD, POLY_CMD,   INT_CMD }
,{jjJET_ID_M,       JET_CMD,    IDEAL_CMD,  IDEAL_CMD,  MATRIX_CMD, INT_CMD }
,{jjJET_ID_M,       JET_CMD,    MODUL_CMD,  MODUL_CMD,  MATRIX_CMD, INT_CMD }
,{jjWRONG3,         JET_CMD,    POLY_CMD,   POLY_CMD,   INT_CMD,    INT_CMD }
,{mpKoszul,         KOSZUL_CMD, MATRIX_CMD, INT_CMD,    INT_CMD,    IDEAL_CMD }
,{jjCALL3MANY,      LIST_CMD,   LIST_CMD,   DEF_CMD,    DEF_CMD,    DEF_CMD }
,{jjMATRIX_Id,      MATRIX_CMD, MATRIX_CMD, IDEAL_CMD,  INT_CMD,    INT_CMD }
,{jjMATRIX_Mo,      MATRIX_CMD, MATRIX_CMD, MODUL_CMD,  INT_CMD,    INT_CMD }
,{jjMATRIX_Ma,      MATRIX_CMD, MATRIX_CMD, MATRIX_CMD, INT_CMD,    INT_CMD }
,{jjMINOR3,         MINOR_CMD,  IDEAL_CMD,  MATRIX_CMD, INT_CMD,    IDEAL_CMD}
,{jjCALL3MANY,      MODUL_CMD,  MODUL_CMD,  DEF_CMD,    DEF_CMD,    DEF_CMD }
#ifdef OLD_RES
,{jjRES3,           MRES_CMD,   NONE,       IDEAL_CMD,  INT_CMD,    ANY_TYPE }
,{jjRES3,           MRES_CMD,   NONE,       MODUL_CMD,  INT_CMD,    ANY_TYPE }
#endif
,{jjLIFT3,          LIFT_CMD,   MATRIX_CMD, IDEAL_CMD,  IDEAL_CMD,  MATRIX_CMD }
,{jjLIFT3,          LIFT_CMD,   MATRIX_CMD, MODUL_CMD,  MODUL_CMD,  MATRIX_CMD }
,{jjPREIMAGE,       PREIMAGE_CMD, IDEAL_CMD, RING_CMD,  ANY_TYPE,   ANY_TYPE }
,{jjPREIMAGE,       PREIMAGE_CMD, IDEAL_CMD, QRING_CMD, ANY_TYPE,   ANY_TYPE }
,{jjRANDOM_Im,      RANDOM_CMD, INTMAT_CMD, INT_CMD,    INT_CMD,    INT_CMD }
,{jjREDUCE3_P,      REDUCE_CMD, POLY_CMD,   POLY_CMD,   IDEAL_CMD,  INT_CMD }
,{jjREDUCE3_P,      REDUCE_CMD, VECTOR_CMD, VECTOR_CMD, IDEAL_CMD,  INT_CMD }
,{jjREDUCE3_P,      REDUCE_CMD, VECTOR_CMD, VECTOR_CMD, MODUL_CMD,  INT_CMD }
,{jjREDUCE3_ID,     REDUCE_CMD, IDEAL_CMD,  IDEAL_CMD,  IDEAL_CMD,  INT_CMD }
,{jjREDUCE3_ID,     REDUCE_CMD, MODUL_CMD,  MODUL_CMD,  MODUL_CMD,  INT_CMD }
,{jjREDUCE3_ID,     REDUCE_CMD, MODUL_CMD,  MODUL_CMD,  IDEAL_CMD,  INT_CMD }
,{jjREDUCE3_CP,     REDUCE_CMD, POLY_CMD,   POLY_CMD,   POLY_CMD,   IDEAL_CMD }
,{jjREDUCE3_CP,     REDUCE_CMD, VECTOR_CMD, VECTOR_CMD, POLY_CMD,   MODUL_CMD }
,{jjREDUCE3_CID,    REDUCE_CMD, IDEAL_CMD,  IDEAL_CMD,  MATRIX_CMD, IDEAL_CMD }
,{jjREDUCE3_CID,    REDUCE_CMD, MODUL_CMD,  MODUL_CMD,  MATRIX_CMD, MODUL_CMD }
#ifdef OLD_RES
,{jjRES3,           RES_CMD,    NONE,       IDEAL_CMD,  INT_CMD,    ANY_TYPE }
,{jjRES3,           RES_CMD,    NONE,       MODUL_CMD,  INT_CMD,    ANY_TYPE }
#endif
#ifdef HAVE_FACTORY
,{jjRESULTANT,      RESULTANT_CMD, POLY_CMD,POLY_CMD,   POLY_CMD,   POLY_CMD }
#else
,{jjWRONG3,         RESULTANT_CMD, POLY_CMD,POLY_CMD,   POLY_CMD,   POLY_CMD }
#endif
,{jjRING3,          RING_CMD,   RING_CMD,   DEF_CMD,    DEF_CMD,    DEF_CMD }
#ifdef OLD_RES
,{jjRES3,           SRES_CMD,   NONE,       IDEAL_CMD,  INT_CMD,    ANY_TYPE }
,{jjRES3,           SRES_CMD,   NONE,       MODUL_CMD,  INT_CMD,    ANY_TYPE }
#endif
,{jjSTATUS3,        STATUS_CMD, INT_CMD,    LINK_CMD,   STRING_CMD, STRING_CMD}
,{jjSTD_HILB_W,     STD_CMD,    IDEAL_CMD,  IDEAL_CMD,  INTVEC_CMD, INTVEC_CMD}
,{jjSTD_HILB_W,     STD_CMD,    MODUL_CMD,  MODUL_CMD,  INTVEC_CMD, INTVEC_CMD}
,{jjSUBST_Par_N,    SUBST_CMD,  POLY_CMD,   POLY_CMD,   NUMBER_CMD, POLY_CMD }
,{jjSUBST_P,        SUBST_CMD,  POLY_CMD,   POLY_CMD,   POLY_CMD,   POLY_CMD }
,{jjSUBST_P,        SUBST_CMD,  POLY_CMD,   POLY_CMD,   POLY_CMD,   POLY_CMD }
,{jjSUBST_P,        SUBST_CMD,  VECTOR_CMD, VECTOR_CMD, POLY_CMD,   POLY_CMD }
,{jjSUBST_Id,       SUBST_CMD,  IDEAL_CMD,  IDEAL_CMD,  POLY_CMD,   POLY_CMD }
,{jjSUBST_Id,       SUBST_CMD,  MODUL_CMD,  MODUL_CMD,  POLY_CMD,   POLY_CMD }
,{jjSUBST_Id,       SUBST_CMD,  MATRIX_CMD, MATRIX_CMD, POLY_CMD,   POLY_CMD }
,{jjSUBST_Id_I,     SUBST_CMD,  MATRIX_CMD, MATRIX_CMD, POLY_CMD,   INT_CMD }
,{jjSUBST_Id_N,     SUBST_CMD,  MATRIX_CMD, MATRIX_CMD, POLY_CMD,   NUMBER_CMD }
,{jjCALL3MANY,      SYSTEM_CMD, NONE,       STRING_CMD, DEF_CMD,    DEF_CMD  }
,{nuLagSolve,       LAGSOLVE_CMD,LIST_CMD,  POLY_CMD,   INT_CMD,    INT_CMD  }
,{nuVanderSys,      VANDER_CMD, POLY_CMD,   IDEAL_CMD,  IDEAL_CMD,  INT_CMD  }
,{NULL,             0,          0,          0,          0,          0 }
};
/*=================== operations with many arg.: static proc =================*/
static BOOLEAN jjBREAK0(leftv res, leftv v)
{
#ifdef HAVE_SDB
  sdb_show_bp();
#endif
  return FALSE;
}
static BOOLEAN jjBREAK1(leftv res, leftv v)
{
#ifdef HAVE_SDB
  if(v->Typ()==PROC_CMD)
  {
    int lineno=0;
    if((v->next!=NULL) && (v->next->Typ()==INT_CMD))
    {
      lineno=(int)v->next->Data();
    }
    return sdb_set_breakpoint(v->Name(),lineno);
  }
  return TRUE;
#else
 return FALSE;
#endif
}
static BOOLEAN jjCALL1ARG(leftv res, leftv v)
{
  return iiExprArith1(res,v,iiOp);
}
static BOOLEAN jjCALL2ARG(leftv res, leftv u)
{
  leftv v=u->next;
  u->next=NULL;
  BOOLEAN b=iiExprArith2(res,u,iiOp,v, (iiOp > 255));
  u->next=v;
  return b;
}
static BOOLEAN jjCALL3ARG(leftv res, leftv u)
{
  leftv v = u->next;
  leftv w = v->next;
  u->next = NULL;
  v->next = NULL;
  BOOLEAN b = iiExprArith3(res, iiOp, u, v, w);
  u->next = v;
  v->next = w;
  return b;
}

static BOOLEAN jjCOEF_M(leftv res, leftv v)
{
  if((v->Typ() != VECTOR_CMD)
  || (v->next->Typ() != POLY_CMD)
  || (v->next->next->Typ() != MATRIX_CMD)
  || (v->next->next->next->Typ() != MATRIX_CMD))
     return TRUE;
  if (v->next->next->rtyp!=IDHDL) return TRUE;
  idhdl c=(idhdl)v->next->next->data;
  if (v->next->next->next->rtyp!=IDHDL) return TRUE;
  idhdl m=(idhdl)v->next->next->next->data;
  idDelete((ideal *)&(c->data.uideal));
  idDelete((ideal *)&(m->data.uideal));
  mpCoef2((poly)v->Data(),(poly)v->next->Data(),
    (matrix *)&(c->data.umatrix),(matrix *)&(m->data.umatrix));
  return FALSE;
}

static BOOLEAN jjDIVISION4(leftv res, leftv v)
{
  leftv v1=v;
  leftv v2=v1->next;
  leftv v3=v2->next;
  leftv v4=v3->next;

  int i1=iiTestConvert(v1->Typ(),MODUL_CMD);
  int i2=iiTestConvert(v2->Typ(),MODUL_CMD);

  if(i1==0||i2==0||v3->Typ()!=INT_CMD||v4->Typ()!=INTVEC_CMD)
  {
    WarnS("<module>,<module>,<int>,<intvec> expected!");
    return TRUE;
  }

  sleftv w1,w2;
  iiConvert(v1->Typ(),MODUL_CMD,i1,v1,&w1);
  iiConvert(v2->Typ(),MODUL_CMD,i2,v2,&w2);
  ideal P=(ideal)w1.Data();
  ideal Q=(ideal)w2.Data();

  int n=(int)v3->Data();
  short *w=iv2array((intvec *)v4->Data());

  short *w0=w+1;
  int i=pVariables;
  while(i>0&&*w0>0)
  {
    w0++;
    i--;
  }
  if(i>0)
    WarnS("not all weights are positive!");

  lists L=idLiftW(P,Q,n,w);

  omFree(w);

  L->m[1].rtyp=v1->Typ();
  ideal R=(ideal)L->m[1].data;
  if(v1->Typ()==POLY_CMD||v1->Typ()==VECTOR_CMD)
  {
    L->m[1].data=(void *)R->m[0];
    R->m[0]=NULL;
    idDelete(&R);
  }
  else if(v1->Typ()==MATRIX_CMD)
    L->m[1].data=(void *)idModule2Matrix(R);
  else if(v1->Typ()!=IDEAL_CMD&&v1->Typ()!=MODUL_CMD)
    L->m[1].rtyp=MODUL_CMD;
  res->data=L;
  res->rtyp=LIST_CMD;

  return FALSE;
}

static BOOLEAN jjIDEAL_PL(leftv res, leftv v)
{
  int s=1;
  leftv h=v;
  if (h!=NULL) s=exprlist_length(h);
  ideal id=idInit(s,1);
  int rank=1;
  int i=0;
  poly p;
  while (h!=NULL)
  {
    switch(h->Typ())
    {
      case POLY_CMD:
      {
        p=(poly)h->CopyD(POLY_CMD);
        break;
      }
      case INT_CMD:
      {
        number n=nInit((int)h->Data());
        if (!nIsZero(n))
        {
          p=pOne();
          pSetCoeff(p,n);
        }
        else
        {
          p=NULL;
          nDelete(&n);
        }
        break;
      }
      case NUMBER_CMD:
      {
        number n=(number)h->CopyD(NUMBER_CMD);
        if (!nIsZero(n))
        {
          p=pOne();
          pSetCoeff(p,n);
        }
        else
        {
          p=NULL;
          nDelete(&n);
        }
        break;
      }
      case VECTOR_CMD:
      {
        p=(poly)h->CopyD(VECTOR_CMD);
        if (iiOp!=MODUL_CMD)
        {
          idDelete(&id);
          pDelete(&p);
          return TRUE;
        }
        rank=max(rank,pMaxComp(p));
        break;
      }
      default:
      {
        idDelete(&id);
        return TRUE;
      }
    }
    if ((iiOp==MODUL_CMD)&&(p!=NULL)&&(pGetComp(p)==0))
    {
      pSetCompP(p,1);
    }
    id->m[i]=p;
    i++;
    h=h->next;
  }
  id->rank=rank;
  res->data=(char *)id;
  return FALSE;
}
static BOOLEAN jjINTERSECT_PL(leftv res, leftv v)
{
  leftv h=v;
  int l=v->listLength();
  resolvente r=(resolvente)omAlloc0(l*sizeof(ideal));
  BOOLEAN *copied=(BOOLEAN *)omAlloc0(l*sizeof(BOOLEAN));
  int t=0;
  // try to convert to IDEAL_CMD
  while (h!=NULL)
  {
    if (iiTestConvert(h->Typ(),IDEAL_CMD)!=0)
    {
      t=IDEAL_CMD;
    }
    else break;
    h=h->next;
  }
  // if failure, try MODUL_CMD
  if (t==0)
  {
    h=v;
    while (h!=NULL)
    {
      if (iiTestConvert(h->Typ(),MODUL_CMD)!=0)
      {
        t=MODUL_CMD;
      }
      else break;
      h=h->next;
    }
  }
  // check for success  in converting
  if (t==0)
  {
    WerrorS("cannot convert to ideal or module");
    return TRUE;
  }
  // call idMultSect
  h=v;
  int i=0;
  sleftv tmp;
  while (h!=NULL)
  {
    if (h->Typ()==t)
    {
      r[i]=(ideal)h->Data(); /*no copy*/
      h=h->next;
    }
    else if(iiConvert(h->Typ(),t,iiTestConvert(h->Typ(),t),h,&tmp))
    {
      omFreeSize((ADDRESS)copied,l*sizeof(BOOLEAN));
      omFreeSize((ADDRESS)r,l*sizeof(ideal));
      Werror("cannot convert arg. %d to %s",i+1,Tok2Cmdname(t));
      return TRUE;
    }
    else
    {
      r[i]=(ideal)tmp.Data(); /*now it's a copy*/
      copied[i]=TRUE;
      h=tmp.next;
    }
    i++;
  }
  res->rtyp=t;
  res->data=(char *)idMultSect(r,i);
  while(i>0)
  {
    i--;
    if (copied[i]) idDelete(&(r[i]));
  }
  omFreeSize((ADDRESS)copied,l*sizeof(BOOLEAN));
  omFreeSize((ADDRESS)r,l*sizeof(ideal));
  return FALSE;
}
static BOOLEAN jjINTVEC_PL(leftv res, leftv v)
{
  int i=0;
  leftv h=v;
  if (h!=NULL) i=exprlist_length(h);
  intvec *iv=new intvec(i);
  i=0;
  while (h!=NULL)
  {
    if(h->Typ()==INT_CMD)
    {
      (*iv)[i]=(int)h->Data();
    }
    else
    {
      delete iv;
      return TRUE;
    }
    i++;
    h=h->next;
  }
  res->data=(char *)iv;
  return FALSE;
}
static BOOLEAN jjJET4(leftv res, leftv u)
{
  leftv u1=u;
  leftv u2=u1->next;
  leftv u3=u2->next;
  leftv u4=u3->next;
  if((u1->Typ()==POLY_CMD)&&(u2->Typ()==POLY_CMD)&&(u3->Typ()==INT_CMD)&&
     (u4->Typ()==INTVEC_CMD)||
     (u1->Typ()==VECTOR_CMD)&&(u2->Typ()==POLY_CMD)&&(u3->Typ()==INT_CMD)&&
     (u4->Typ()==INTVEC_CMD))
  {
    if(!pIsUnit((poly)u2->Data()))
    {
      Werror("2nd argument must be a unit");
      return TRUE;
    }
    res->rtyp=u1->Typ();
    res->data=(char*)pSeries((int)u3->Data(),pCopy((poly)u1->Data()),
                             pCopy((poly)u2->Data()),(intvec*)u4->Data());
    return FALSE;
  }
  else
  if((u1->Typ()==IDEAL_CMD)&&(u2->Typ()==MATRIX_CMD)&&(u3->Typ()==INT_CMD)&&
     (u4->Typ()==INTVEC_CMD)||
     (u1->Typ()==MODUL_CMD)&&(u2->Typ()==MATRIX_CMD)&&(u3->Typ()==INT_CMD)&&
     (u4->Typ()==INTVEC_CMD))
  {
    if(!mpIsDiagUnit((matrix)u2->Data()))
    {
      Werror("2nd argument must be a diagonal matrix of units");
      return TRUE;
    }
    res->rtyp=u1->Typ();
    res->data=(char*)idSeries((int)u3->Data(),idCopy((ideal)u1->Data()),
                              mpCopy((matrix)u2->Data()),(intvec*)u4->Data());
    return FALSE;
  }
  else
  {
    Werror("%s(`poly`,`poly`,`int`,`intvec`) exppected",
           Tok2Cmdname(iiOp));
    return TRUE;
  }
}
static BOOLEAN jjKLAMMER_PL(leftv res, leftv u)
{
  if ((yyInRingConstruction)
  && ((strcmp(u->Name(),"real")==0) || (strcmp(u->Name(),"complex")==0)))
  {
    memcpy(res,u,sizeof(sleftv));
    memset(u,0,sizeof(sleftv));
    return FALSE;
  }
  leftv v=u->next;
  BOOLEAN b;
  if(v==NULL)
    b=iiExprArith1(res,u,iiOp);
  else
  {
    u->next=NULL;
    b=iiExprArith2(res,u,iiOp,v);
    u->next=v;
  }
  return b;
}
static BOOLEAN jjLIST_PL(leftv res, leftv v)
{
  int sl=0;
  if (v!=NULL) sl = v->listLength();
  lists L;
  if((sl==1)&&(v->Typ()==RESOLUTION_CMD))
  {
    L=syConvRes((syStrategy)v->Data());
  }
  else
  {
    L=(lists)omAllocBin(slists_bin);
    leftv h=NULL;
    int i;
    int rt;

    L->Init(sl);
    for (i=0;i<sl;i++)
    {
      if (h!=NULL) { /* e.g. not in the first step:
                     * h is the pointer to the old sleftv,
                     * v is the pointer to the next sleftv
                     * (in this moment) */
                     h->next=v;
                   }
      h=v;
      v=v->next;
      h->next=NULL;
      rt=h->Typ();
      if (rt==0)
      {
        L->Clean();
        Werror("`%s` is undefined",h->Fullname());
        return TRUE;
      }
      if ((rt==RING_CMD)||(rt==QRING_CMD))
      {
        L->m[i].rtyp=rt;  L->m[i].data=h->Data();
        ((ring)L->m[i].data)->ref++;
      }
      else
        L->m[i].Copy(h);
    }
  }
  res->data=(char *)L;
  return FALSE;
}
static BOOLEAN jjNAMES0(leftv res, leftv v)
{
  res->data=(void *)ipNameList(IDROOT);
  return FALSE;
}
static BOOLEAN jjOPTION_PL(leftv res, leftv v)
{
  if(v==NULL)
  {
    res->data=(char *)showOption();
    return FALSE;
  }
  res->rtyp=NONE;
  return setOption(res,v);
}
static BOOLEAN jjREDUCE4(leftv res, leftv u)
{
  leftv u1=u;
  leftv u2=u1->next;
  leftv u3=u2->next;
  leftv u4=u3->next;
  if((u3->Typ()==INT_CMD)&&(u4->Typ()==INTVEC_CMD))
  {
    int save_d=Kstd1_deg;
    Kstd1_deg=(int)u3->Data();
    kModW=(intvec *)u4->Data();
    BITSET save=verbose;
    verbose|=Sy_bit(V_DEG_STOP);
    u2->next=NULL;
    BOOLEAN r=jjCALL2ARG(res,u);
    kModW=NULL;
    Kstd1_deg=save_d;
    verbose=save;
    u->next->next=u3;
    return r;
  }
  else
  if((u1->Typ()==IDEAL_CMD)&&(u2->Typ()==MATRIX_CMD)&&(u3->Typ()==IDEAL_CMD)&&
     (u4->Typ()==INT_CMD))
  {
    assumeStdFlag(u3);
    if(!mpIsDiagUnit((matrix)u2->Data()))
    {
      Werror("2nd argument must be a diagonal matrix of units");
      return TRUE;
    }
    res->rtyp=IDEAL_CMD;
    res->data=(char*)redNF(idCopy((ideal)u3->Data()),idCopy((ideal)u1->Data()),
                           mpCopy((matrix)u2->Data()),(int)u4->Data());
    return FALSE;
  }
  else
  if((u1->Typ()==POLY_CMD)&&(u2->Typ()==POLY_CMD)&&(u3->Typ()==IDEAL_CMD)&&
     (u4->Typ()==INT_CMD))
  {
    assumeStdFlag(u3);
    if(!pIsUnit((poly)u2->Data()))
    {
      Werror("2nd argument must be a unit");
      return TRUE;
    }
    res->rtyp=POLY_CMD;
    res->data=(char*)redNF(idCopy((ideal)u3->Data()),pCopy((poly)u1->Data()),
                           pCopy((poly)u2->Data()),(int)u4->Data());
    return FALSE;
  }
  else
  {
    Werror("%s(`poly`,`ideal`,`int`,`intvec`) expected",Tok2Cmdname(iiOp));
    return TRUE;
  }
}
static BOOLEAN jjREDUCE5(leftv res, leftv u)
{
  leftv u1=u;
  leftv u2=u1->next;
  leftv u3=u2->next;
  leftv u4=u3->next;
  leftv u5=u4->next;
  if((u1->Typ()==IDEAL_CMD)&&(u2->Typ()==MATRIX_CMD)&&(u3->Typ()==IDEAL_CMD)&&
     (u4->Typ()==INT_CMD)&&(u5->Typ()==INTVEC_CMD))
  {
    assumeStdFlag(u3);
    if(!mpIsDiagUnit((matrix)u2->Data()))
    {
      Werror("2nd argument must be a diagonal matrix of units");
      return TRUE;
    }
    res->rtyp=IDEAL_CMD;
    res->data=(char*)redNF(idCopy((ideal)u3->Data()),idCopy((ideal)u1->Data()),
                           mpCopy((matrix)u2->Data()),
                           (int)u4->Data(),(intvec*)u5->Data());
    return FALSE;
  }
  else
  if((u1->Typ()==POLY_CMD)&&(u2->Typ()==POLY_CMD)&&(u3->Typ()==IDEAL_CMD)&&
     (u4->Typ()==INT_CMD)&&(u5->Typ()==INTVEC_CMD))
  {
    assumeStdFlag(u3);
    if(!pIsUnit((poly)u2->Data()))
    {
      Werror("2nd argument must be a unit");
      return TRUE;
    }
    res->rtyp=POLY_CMD;
    res->data=(char*)redNF(idCopy((ideal)u3->Data()),pCopy((poly)u1->Data()),
                           pCopy((poly)u2->Data()),
                           (int)u4->Data(),(intvec*)u5->Data());
    return FALSE;
  }
  else
  {
    Werror("%s(`ideal`,`ideal`,`matrix`,`int`,`intvec`) exppected",
           Tok2Cmdname(iiOp));
    return TRUE;
  }
}
static BOOLEAN jjRESERVED0(leftv res, leftv v)
{
  int i=1;
  loop
  {
    Print("%-20s",cmds[i].name);
    i++;
    if(cmds[i].name==NULL)
      break;
    if ((i%3)==1) PrintLn();
  }
  PrintLn();
  return FALSE;
}
static BOOLEAN jjSTRING_PL(leftv res, leftv v)
{
  if (v == NULL)
  {
    res->data = omStrDup("");
    return FALSE;
  }
  int n = v->listLength();
  if (n == 1)
  {
    res->data = v->String();
    return FALSE;
  }

  char** slist = (char**) omAlloc(n*sizeof(char*));
  int i, j;

  for (i=0, j=0; i<n; i++, v = v ->next)
  {
    slist[i] = v->String();
    assume(slist[i] != NULL);
    j+=strlen(slist[i]);
  }
  char* s = (char*) omAlloc((j+1)*sizeof(char));
  *s='\0';
  for (i=0;i<n;i++)
  {
    strcat(s, slist[i]);
    omFree(slist[i]);
  }
  omFreeSize(slist, n*sizeof(char*));
  res->data = s;
  return FALSE;
}
static BOOLEAN jjTEST(leftv res, leftv v)
{
  do
  {
    if (v->Typ()!=INT_CMD)
      return TRUE;
    test_cmd((int)v->Data());
    v=v->next;
  }
  while (v!=NULL);
  return FALSE;
}

#ifndef __MWERKS__
#if defined(__alpha) && !defined(linux)
extern "C"
{
  void usleep(unsigned long usec);
};
#endif

static BOOLEAN jjSTATUS_M(leftv res, leftv v)
{
  if ((v->Typ() != LINK_CMD) ||
      (v->next->Typ() != STRING_CMD) ||
      (v->next->next->Typ() != STRING_CMD) ||
      (v->next->next->next->Typ() != INT_CMD))
    return TRUE;
  jjSTATUS3(res, v, v->next, v->next->next);
#if defined(HAVE_USLEEP)
  if (((int) res->data) == 0)
  {
    int i_s = (int) v->next->next->next->Data();
    if (i_s > 0)
    {
      usleep((int) v->next->next->next->Data());
      jjSTATUS3(res, v, v->next, v->next->next);
    }
  }
#elif defined(HAVE_SLEEP)
  if (((int) res->data) == 0)
  {
    int i_s = (int) v->next->next->next->Data();
    if (i_s > 0)
    {
      sleep((is - 1)/1000000 + 1);
      jjSTATUS3(res, v, v->next, v->next->next);
    }
  }
#endif
  return FALSE;
}
#endif
static BOOLEAN jjSUBST_M(leftv res, leftv u)
{
  leftv v = u->next; // number of args > 0
  if (v==NULL) return TRUE;
  leftv w = v->next;
  if (w==NULL) return TRUE;
  leftv rest = w->next;;

  u->next = NULL;
  v->next = NULL;
  w->next = NULL;
  BOOLEAN b = iiExprArith3(res, iiOp, u, v, w);
  if ((rest!=NULL) && (!b))
  {
    sleftv tmp_res;
    leftv tmp_next=res->next;
    res->next=rest;
    memset(&tmp_res,0,sizeof(tmp_res));
    b = iiExprArithM(&tmp_res,res,iiOp);
    memcpy(res,&tmp_res,sizeof(tmp_res));
    res->next=tmp_next;
  }
  u->next = v;
  v->next = w;
  // rest was w->next, but is already cleaned
  return b;
}

#ifdef HAVE_NAMESPACES
static BOOLEAN jjIMPORTFROM(leftv res, leftv v);

static BOOLEAN jjEXPORTTO(leftv res, leftv v)
{
  BOOLEAN nok=TRUE;
  leftv u=v;
  if(u->rtyp==NSHDL)
  {
    namehdl ns = (namehdl)(u->data);
    idhdl h = namespaceroot->root->get(ns->name, 0, TRUE);
    //Print("Export to '%s', lev %d\n", ns->name, ns->myynest);
    while(v->next!=NULL)
    {
      nok = iiInternalExport(v->next, ns->myynest, h);
      if(nok) { return nok; }

      v = v->next;
    }
    return FALSE;
  }
  if(u->Typ()==PACKAGE_CMD)
  {
    //PrintS("export to package\n");
    while(v->next!=NULL)
    {
      nok = iiInternalExport(v->next, 0, (idhdl)u->data);
      if(nok) return nok;
      v = v->next;
    }
    return FALSE;
  }
  return TRUE;
}
#endif /* HAVE_NAMESPACES */
#ifdef HAVE_NAMESPACES
static BOOLEAN jjIMPORTFROM(leftv res, leftv v)
{
  BOOLEAN nok=FALSE;
  PrintS("jjIMPORT_FROM()\n");
  if(v->rtyp==NSHDL)
  {
    PrintS("Import from toplevel\n");
//While-schleife!!!
    return FALSE;
  }
  if(v->Typ()==PACKAGE_CMD)
  {
    Print("Import from package %s\n", v->name);
    while(v->next!=NULL)
    {
      //nok = iiInternalImport(v->next, 0, v->data);
      if(nok) return nok;
      v = v->next;
    }
    return FALSE;
  }
  return TRUE;
}
#endif
#ifdef HAVE_NAMESPACES
static BOOLEAN jjUNLOAD(leftv res, leftv v)
{
  if(v->Typ()==PACKAGE_CMD)
  {
    char *typ;
    idhdl h = (idhdl)v->data;
    package d=(package)v->Data();
    switch (d->language)
    {
      case LANG_C:
        typ="object";
        break;
      case LANG_SINGULAR:
      case LANG_NONE:
      default:
        killhdl(h);
    }
    return FALSE;
  }
  return TRUE;
}
#endif
/*=================== operations with many arg.: table =================*/
/* number_of_args:  -1: any, -2: any >0, .. */
struct sValCmdM dArithM[]=
{
// operations:
// proc         cmd               res            number_of_args
 {jjKLAMMER_PL,  '(',           ANY_TYPE,           -2  }
,{jjBREAK0,    BREAKPOINT_CMD,  NONE,               0  }
,{jjBREAK1,    BREAKPOINT_CMD,  NONE,               -2  }
,{jjCALL2ARG,  COEF_CMD,        MATRIX_CMD,         2  }
,{jjCOEF_M,    COEF_CMD,        NONE,               4  }
,{jjCALL2ARG,  DIVISION_CMD,    ANY_TYPE/*or set by p*/,2  }
,{jjDIVISION4, DIVISION_CMD,    ANY_TYPE/*or set by p*/,4  }
,{jjDBPRINT,   DBPRINT_CMD,     NONE,               -2 }
,{jjCALL1ARG,  IDEAL_CMD,       IDEAL_CMD,          1  }
,{jjIDEAL_PL,  IDEAL_CMD,       IDEAL_CMD,          -1 }
,{jjCALL2ARG,  INTERSECT_CMD,   IDEAL_CMD,          2  }
,{jjINTERSECT_PL,INTERSECT_CMD, IDEAL_CMD,          -2 }
,{jjCALL1ARG,  INTVEC_CMD,      INTVEC_CMD,         1  }
,{jjINTVEC_PL, INTVEC_CMD,      INTVEC_CMD,         -2 }
,{jjCALL2ARG,  JET_CMD,         POLY_CMD,/*or set by p*/           2  }
,{jjCALL3ARG,  JET_CMD,         POLY_CMD,/*or set by p*/           3  }
,{jjJET4,      JET_CMD,         POLY_CMD,/*or set by p*/           4  }
,{jjLIST_PL,   LIST_CMD,        LIST_CMD,           -1 }
,{jjCALL1ARG,  MODUL_CMD,       MODUL_CMD,          1  }
,{jjIDEAL_PL,  MODUL_CMD,       MODUL_CMD,          -1 }
,{jjCALL1ARG,  NAMES_CMD,       LIST_CMD,            1 }
,{jjNAMES0,    NAMES_CMD,       LIST_CMD,            0 }
,{jjOPTION_PL, OPTION_CMD,      STRING_CMD/*or set by p*/,-1 }
,{jjCALL2ARG,  REDUCE_CMD,      IDEAL_CMD/*or set by p*/,          2  }
,{jjCALL3ARG,  REDUCE_CMD,      IDEAL_CMD/*or set by p*/,          3  }
,{jjREDUCE4,   REDUCE_CMD,      IDEAL_CMD/*or set by p*/,          4  }
,{jjREDUCE5,   REDUCE_CMD,      IDEAL_CMD/*or set by p*/,          5  }
,{jjCALL1ARG,  RESERVEDNAME_CMD, INT_CMD,            1 }
,{jjRESERVED0, RESERVEDNAME_CMD, NONE,               0 }
,{jjSTRING_PL, STRING_CMD,      STRING_CMD,         -1 }
,{jjCALL3ARG,  SUBST_CMD,       NONE/*set by p*/,   3 }
,{jjSUBST_M,   SUBST_CMD,       NONE/*set by p*/,   -2 }
,{jjSYSTEM,    SYSTEM_CMD,      NONE/*or set by p*/,-2 }
,{jjTEST,      TEST_CMD,        NONE,               -2 }
,{iiWRITE,     WRITE_CMD,       NONE,               -2 }
,{jjCALL2ARG,  STATUS_CMD,      STRING_CMD,          2 }
,{jjCALL3ARG,  STATUS_CMD,      INT_CMD,             3 }
#ifndef __MWERKS__
,{jjSTATUS_M,  STATUS_CMD,      INT_CMD,             4 }
#endif
#ifdef HAVE_NAMESPACES
,{jjIMPORTFROM,IMPORTFROM_CMD,  ANY_TYPE,           -2 }
,{jjEXPORTTO,  EXPORTTO_CMD,    NONE,               -2 }
,{jjUNLOAD,    UNLOAD_CMD,      NONE,               -2 }
#endif /* HAVE_NAMESPACES */
,{loSimplex,   SIMPLEX_CMD,     LIST_CMD,            6 }
,{nuUResSolve, URSOLVE_CMD,     LIST_CMD,            4 }
,{NULL,        0,               0,                   0 }
};
#ifdef MDEBUG
static Subexpr jjDBMakeSub(leftv e,char *f, int l)
#else
static Subexpr jjMakeSub(leftv e)
#endif
{
  assume( e->Typ()==INT_CMD );
  Subexpr r=(Subexpr)omAlloc0Bin(sSubexpr_bin);
  r->start =(int)e->Data();
  return r;
}

/*================ generating tables ============================*/
#ifdef GENTABLE
extern struct sValAssign dAssign[];
struct sValCmdTab dArithTab1[]={ {0,0}};
#define JJTAB1LEN 0
struct sValCmdTab dArithTab2[]={ {0,0}};
#define JJTAB2LEN 0
void ttGen1()
{
  FILE *outfile = myfopen("iparith.inc","w");
  int i,j,l1=0,l2=0;
  currRing=(ring)omAllocBin(sip_sring_bin);
  fprintf(outfile,
  "/****************************************\n"
  "*  Computer Algebra System SINGULAR     *\n"
  "****************************************/\n\n");
/*-------------------------------------------------------------------*/
  fprintf(outfile,"// syntax table for Singular\n//\n");
  fprintf(outfile,"// - search for an exact match of the argument types\n");
  fprintf(outfile,"// - otherwise search for the first possibility\n");
  fprintf(outfile,"//   with converted types of the arguments\n");
  fprintf(outfile,"// - otherwise report an error\n//\n");

  int op;
  i=0;
  while ((op=dArith1[i].cmd)!=0)
  {
    if (dArith1[i].p==jjWRONG)
      fprintf(outfile,"// DUMMY ");
    char *s = iiTwoOps(op);
    fprintf(outfile,"// operation: %s (%s)  ->  %s\n",
          s,
          Tok2Cmdname(dArith1[i].arg),
          Tok2Cmdname(ABS(dArith1[i].res)));
    i++;
  }
  fprintf(outfile,"/*---------------------------------------------*/\n");
  i=0;
  while ((op=dArith2[i].cmd)!=0)
  {
    if (dArith2[i].p==jjWRONG2)
      fprintf(outfile,"// DUMMY ");
    char *s = iiTwoOps(op);
    fprintf(outfile,"// operation: %s (%s, %s)  ->  %s\n",
          s,
          Tok2Cmdname(dArith2[i].arg1),
          Tok2Cmdname(dArith2[i].arg2),
          Tok2Cmdname(dArith2[i].res));
    i++;
  }
  fprintf(outfile,"/*---------------------------------------------*/\n");
  i=0;
  while ((op=dArith3[i].cmd)!=0)
  {
    char *s = iiTwoOps(op);
    if (dArith3[i].p==jjWRONG3)
      fprintf(outfile,"// DUMMY ");
    fprintf(outfile,"// operation: %s (%s, %s, %s)  ->  %s\n",
          s,
          Tok2Cmdname(dArith3[i].arg1),
          Tok2Cmdname(dArith3[i].arg2),
          Tok2Cmdname(dArith3[i].arg3),
          Tok2Cmdname(dArith3[i].res));
    i++;
  }
  fprintf(outfile,"/*---------------------------------------------*/\n");
  i=0;
  while ((op=dArithM[i].cmd)!=0)
  {
    char *s = iiTwoOps(op);
    fprintf(outfile,"// operation: %s (...)  ->  %s",
          s,
          Tok2Cmdname(dArithM[i].res));
    switch(dArithM[i].number_of_args)
    {
      case -2:
         fprintf(outfile," ( number of arguments >0 )\n");
         break;
      case -1:
         fprintf(outfile," ( any number of arguments )\n");
         break;
      default:
         fprintf(outfile," ( %d arguments )\n",dArithM[i].number_of_args);
         break;
    }
    i++;
  }
  fprintf(outfile,"/*---------------------------------------------*/\n");
  i=0;
  while ((op=dAssign[i].res)!=0)
  {
    fprintf(outfile,"// assign: %s =  %s\n",
          Tok2Cmdname(op/*dAssign[i].res*/),
          Tok2Cmdname(dAssign[i].arg));
    i++;
  }
/*-------------------------------------------------------------------*/
  fprintf(outfile,"/*---------------------------------------------*/\n");
  for (j=257;j<=MAX_TOK+1;j++)
  {
    for(i=257;i<=MAX_TOK+1;i++)
    {
      if ((i!=j) && (j!=IDHDL) && (j!=DEF_CMD) && (j!=ANY_TYPE)
      && iiTestConvert(i,j))
      {
        fprintf(outfile,"// convert %s -> %s\n",
          Tok2Cmdname(i), Tok2Cmdname(j));
        if (j==ANY_TYPE) break;
      }
    }
  }
  fprintf(outfile,"/*---------------------------------------------*/\n");
  char ops[]="=><+*/[.^,%(;";
  for(i=0;ops[i]!='\0';i++)
    fprintf(outfile,"// token %d : %c\n", (int)ops[i], ops[i]);
  for (i=257;i<=MAX_TOK;i++)
  {
    char *s=iiTwoOps(i);
    if (s[0]!='$')
    {
      fprintf(outfile,"// token %d : %s\n", i, s);
    }
  }
/*-------------------------------------------------------------------*/
  fprintf(outfile,"/*--max. token: %d, gr: %d --*/\n",MAX_TOK,UMINUS);
/*-------------------------------------------------------------------*/
  fprintf(outfile,"/*---------------------------------------------*/\n");
  fprintf(outfile,
  "struct sValCmdTab dArithTab1[]=\n"
  "{\n");
  for (j=1;j<=MAX_TOK+1;j++)
  {
    for(i=0;dArith1[i].cmd!=0;i++)
    {
      if (dArith1[i].cmd==j)
      {
        fprintf(outfile," { %d,%d },\n",j,i);
        l1++;
        break;
      }
    }
  }
  fprintf(outfile," { 10000,0 }\n};\n");
  fprintf(outfile,"#define JJTAB1LEN %d\n",l1);
/*-------------------------------------------------------------------*/
  fprintf(outfile,
  "struct sValCmdTab dArithTab2[]=\n"
  "{\n");
  for (j=1;j<=MAX_TOK+1;j++)
  {
    for(i=0;dArith2[i].cmd!=0;i++)
    {
      if (dArith2[i].cmd==j)
      {
        fprintf(outfile," { %d,%d },\n",j,i);
        l2++;
        break;
      }
    }
  }
  fprintf(outfile," { 10000,0 }\n};\n");
  fprintf(outfile,"#define JJTAB2LEN %d\n",l2);
  fclose(outfile);
}
/*-------------------------------------------------------------------*/
void ttGen2()
{
  FILE *outfile = myfopen("iparith.inc","a");
  fprintf(outfile,
  "/****************************************\n"
  "*  Computer Algebra System SINGULAR     *\n"
  "****************************************/\n\n");
/*-------------------------------------------------------------------*/
  fprintf(outfile,"// identifier table for Singular\n//\n");

  fprintf(outfile,
  "cmdnames cmds[] =\n"
  "{  // name-string     alias  tokval toktype\n"
  "{ \"$INVALID$\",            0,  -1, 0},\n");
  int i=1;
  int m=-1;
  int id_nr=0;
  BOOLEAN f=FALSE;
  loop
  {
    while (cmds[i].tokval!=0)
    {
      if ((cmds[i].tokval!=-1)&&(cmds[i].name!=NULL))
      {
        if(m==-1)
        {
          m=i;
          f=TRUE;
        }
        else if(strcmp(cmds[m].name,cmds[i].name)>0)
        {
          m=i;
          f=TRUE;
        }
      }
      i++;
    }
    if(f)
    {
      id_nr++;
      if(cmds[m].tokval==VRTIMER) fprintf(outfile,"#ifdef HAVE_RTIMER\n");
      fprintf(outfile,"  {\"%s\", %*d, %3d, ",cmds[m].name,
                                             20-strlen(cmds[m].name),
                                             cmds[m].alias,
                                             cmds[m].tokval);
      switch(cmds[m].toktype)
      {
        case CMD_1:            fprintf(outfile,"CMD_1 },\n"); break;
        case CMD_2:            fprintf(outfile,"CMD_2 },\n"); break;
        case CMD_3:            fprintf(outfile,"CMD_3 },\n"); break;
        case CMD_12:           fprintf(outfile,"CMD_12 },\n"); break;
        case CMD_123 :         fprintf(outfile,"CMD_123 },\n"); break;
        case CMD_23:           fprintf(outfile,"CMD_23 },\n"); break;
        case CMD_M:            fprintf(outfile,"CMD_M },\n"); break;
        case SYSVAR:           fprintf(outfile,"SYSVAR },\n"); break;
        case ROOT_DECL:        fprintf(outfile,"ROOT_DECL },\n"); break;
        case ROOT_DECL_LIST:   fprintf(outfile,"ROOT_DECL_LIST },\n"); break;
        case RING_DECL:        fprintf(outfile,"RING_DECL },\n"); break;
        case NONE:             fprintf(outfile,"NONE },\n"); break;
        default:               if((cmds[m].toktype>' ')
                               &&(cmds[m].toktype<127))
                               {
                                 fprintf(outfile,"'%c' },\n",cmds[m].toktype);
                               }
                               else
                               {
                                 fprintf(outfile,"%d },\n",cmds[m].toktype);
                               }
                               break;
      }
      if(cmds[m].tokval==VRTIMER) fprintf(outfile,"#endif\n");
      cmds[m].name=NULL;
      m=-1;
      i=1;
      f=FALSE;
    }
    else break;
  }
  fprintf(outfile,
"/* list of scanner identifiers/only for feread/reservedName */\n");
  f=FALSE;
  i=1;m=-1;
  loop
  {
    while (cmds[i].tokval!=0)
    {
      if (cmds[i].name!=NULL)
      {
        if(m==-1)
        {
          m=i;
          f=TRUE;
        }
        else if(strcmp(cmds[m].name,cmds[i].name)>0)
        {
          m=i;
          f=TRUE;
        }
      }
      i++;
    }
    if(f)
    {
      fprintf(outfile,"  {\"%s\", %*d,  -1, 0 },\n",cmds[m].name,
                                             20-strlen(cmds[m].name),
                                             0/*cmds[m].alias*/
                                             /*-1 cmds[m].tokval*/
                                             /*0 cmds[m].toktype*/);
      cmds[m].name=NULL;
      m=-1;
      i=1;
      f=FALSE;
    }
    else break;
  }
  fprintf(outfile,
"/* end of list marker */\n"
"  { NULL, 0, 0, 0}\n"
"};\n"
"#ifdef HAVE_RTIMER\n"
"#define LAST_IDENTIFIER %d\n"
"#else\n"
"#define LAST_IDENTIFIER %d\n"
"#endif\n",id_nr,id_nr-1);
  fclose(outfile);
}
/*-------------------------------------------------------------------*/
#if 0
void ttGen3()
{
  FILE *outfile = myfopen("mpsr_tok.inc","w");
  fprintf(outfile,
  "/****************************************\n"
  "*  Computer Algebra System SINGULAR     *\n"
  "****************************************/\n\n");
/*-------------------------------------------------------------------*/
  fprintf(outfile,"// token table for Singular\n//\n");

  fprintf(outfile,
  "short vtok[] =\n"
  "{\n");
  // operations with 1 arg: ===========================================
  int i=0;
  while (dArith1[i].cmd!=0)
  {
    if ((dArith1[i].p!=jjWRONG)
    &&((i==0)||(dArith1[i].cmd!=dArith1[i-1].cmd)))
    {
      fprintf(outfile,"  %d,\n",dArith1[i].cmd);
    }
    i++;
  }
  // operations with 2 args: ===========================================
  i=0;
  while (dArith2[i].cmd!=0)
  {
    if ((dArith2[i].p!=jjWRONG2)
    &&((i==0)||(dArith2[i].cmd!=dArith2[i-1].cmd)))
    {
      fprintf(outfile,"  %d,\n",dArith2[i].cmd);
    }
    i++;
  }
  // operations with 3 args: ===========================================
  i=0;
  while (dArith3[i].cmd!=0)
  {
    if (
    ((i==0)||(dArith3[i].cmd!=dArith3[i-1].cmd)))
    {
      fprintf(outfile,"  %d,\n",dArith3[i].cmd);
    }
    i++;
  }
  // operations with many args: ===========================================
  i=0;
  while (dArithM[i].cmd!=0)
  {
    if (
    ((i==0)||(dArithM[i].cmd!=dArithM[i-1].cmd)))
    {
      fprintf(outfile,"  %d,\n",dArithM[i].cmd);
    }
    i++;
  }
  // ====================================================================
  fprintf(outfile,
  "/* end of list marker */\n"
  " %d };\n",MAX_TOK);
  fclose(outfile);
}
#endif
/*-------------------------------------------------------------------*/
#else
#include "iparith.inc"
#endif

/*=================== operations with 2 args. ============================*/

BOOLEAN iiExprArith2(leftv res, leftv a, int op, leftv b, BOOLEAN proccall)
{
#ifndef GENTABLE
  memset(res,0,sizeof(sleftv));

  if (!errorreported)
  {
#ifdef SIQ
    if (siq>0)
    {
      //Print("siq:%d\n",siq);
      command d=(command)omAlloc0Bin(ip_command_bin);
      memcpy(&d->arg1,a,sizeof(sleftv));
      memcpy(&d->arg2,b,sizeof(sleftv));
      d->argc=2;
      d->op=op;
      res->data=(char *)d;
      res->rtyp=COMMAND;
      return FALSE;
    }
#endif
    int at=a->Typ();
    int bt=b->Typ();
    int i=iiTabIndex(dArithTab2,JJTAB2LEN,op);
    int index=i;

    iiOp=op;
    while (dArith2[i].cmd==op)
    {
      if ((at==dArith2[i].arg1)
      && (bt==dArith2[i].arg2))
      {
        res->rtyp=dArith2[i].res;
        if (dArith2[i].p(res,a,b))
        {
          break;// leave loop, goto error handling
        }
        a->CleanUp();
        b->CleanUp();
        //Print("op: %d,result typ:%d\n",op,res->rtyp);
        return FALSE;
      }
      i++;
    }
    // implicite type conversion ----------------------------------------------
    if (dArith2[i].cmd!=op)
    {
      int ai,bi;
      leftv an = (leftv)omAlloc0Bin(sleftv_bin);
      leftv bn = (leftv)omAlloc0Bin(sleftv_bin);
      BOOLEAN failed=FALSE;
      i=index; /*iiTabIndex(dArithTab2,JJTAB2LEN,op);*/
      //Print("op: %c, type: %s %s\n",op,Tok2Cmdname(at),Tok2Cmdname(bt));
      while (dArith2[i].cmd==op)
      {
        //Print("test %s %s\n",Tok2Cmdname(dArith2[i].arg1),Tok2Cmdname(dArith2[i].arg2));
        if ((ai=iiTestConvert(at,dArith2[i].arg1))!=0)
        {
          if ((bi=iiTestConvert(bt,dArith2[i].arg2))!=0)
          {
            res->rtyp=dArith2[i].res;
            failed= ((iiConvert(at,dArith2[i].arg1,ai,a,an))
            || (iiConvert(bt,dArith2[i].arg2,bi,b,bn))
            || (dArith2[i].p(res,an,bn)));
            // everything done, clean up temp. variables
            if (failed)
            {
              // leave loop, goto error handling
              break;
            }
            else
            {
              // everything ok, clean up and return
              an->CleanUp();
              bn->CleanUp();
              omFreeBin((ADDRESS)an, sleftv_bin);
              omFreeBin((ADDRESS)bn, sleftv_bin);
              a->CleanUp();
              b->CleanUp();
              return FALSE;
            }
          }
        }
        i++;
      }
      an->CleanUp();
      bn->CleanUp();
      omFreeBin((ADDRESS)an, sleftv_bin);
      omFreeBin((ADDRESS)bn, sleftv_bin);
    }
    // error handling ---------------------------------------------------
    const char *s=NULL;
    if (!errorreported)
    {
      if ((at==0) && (a->Fullname()!=sNoName))
      {
        s=a->Fullname();
      }
      else if ((bt==0) && (b->Fullname()!=sNoName))
      {
        s=b->Fullname();
      }
      if (s!=NULL)
        Werror("`%s` is not defined",s);
      else
      {
        i=index; /*iiTabIndex(dArithTab2,JJTAB2LEN,op);*/
        s = iiTwoOps(op);
        if (proccall)
        {
          Werror("%s(`%s`,`%s`) is not supported"
                ,s,Tok2Cmdname(at),Tok2Cmdname(bt));
        }
        else
        {
          Werror("`%s` %s `%s` is not supported"
                ,Tok2Cmdname(at),s,Tok2Cmdname(bt));
        }
        if (BVERBOSE(V_SHOW_USE))
        {
          while (dArith2[i].cmd==op)
          {
            if(((at==dArith2[i].arg1)||(bt==dArith2[i].arg2))
            && (dArith2[i].res!=0)
            && (dArith2[i].p!=jjWRONG2))
            {
              if (proccall)
                Werror("expected %s(`%s`,`%s`)"
                  ,s,Tok2Cmdname(dArith2[i].arg1),Tok2Cmdname(dArith2[i].arg2));
              else
                Werror("expected `%s` %s `%s`"
                  ,Tok2Cmdname(dArith2[i].arg1),s,Tok2Cmdname(dArith2[i].arg2));
            }
            i++;
          }
        }
      }
    }
    res->rtyp = UNKNOWN;
  }
  a->CleanUp();
  b->CleanUp();
#endif
  return TRUE;
}

/*==================== operations with 1 arg. ===============================*/

BOOLEAN iiExprArith1(leftv res, leftv a, int op)
{
#ifndef GENTABLE
  memset(res,0,sizeof(sleftv));

  if (!errorreported)
  {
#ifdef SIQ
    if (siq>0)
    {
      //Print("siq:%d\n",siq);
      command d=(command)omAlloc0Bin(ip_command_bin);
      memcpy(&d->arg1,a,sizeof(sleftv));
      d->op=op;
      d->argc=1;
      res->data=(char *)d;
      res->rtyp=COMMAND;
      return FALSE;
    }
#endif
    int at=a->Typ();
    BOOLEAN failed=FALSE;

    iiOp=op;
    int i=iiTabIndex(dArithTab1,JJTAB1LEN,op);
    int ti = i;
    while (dArith1[i].cmd==op)
    {
      if (at==dArith1[i].arg)
      {
        int r=res->rtyp=dArith1[i].res;
        if (r<0)
        {
          res->rtyp=-r;
          #ifdef PROC_BUG
          dArith1[i].p(res,a);
          #else
          res->data=(char *)((Proc1)dArith1[i].p)((char *)a->Data());
          #endif
        }
        else if (dArith1[i].p(res,a))
        {
          break;// leave loop, goto error handling
        }
        if (a->Next()!=NULL)
        {
          res->next=(leftv)omAllocBin(sleftv_bin);
          failed=iiExprArith1(res->next,a->next,op);
        }
        a->CleanUp();
        return failed;
      }
      i++;
    }
    // implicite type conversion --------------------------------------------
    if (dArith1[i].cmd!=op)
    {
      leftv an = (leftv)omAlloc0Bin(sleftv_bin);
      i=ti;
      //Print("fuer %c , typ: %s\n",op,Tok2Cmdname(at));
      while (dArith1[i].cmd==op)
      {
        int ai;
        //Print("test %s\n",Tok2Cmdname(dArith1[i].arg));
        if ((ai=iiTestConvert(at,dArith1[i].arg))!=0)
        {
          int r=res->rtyp=dArith1[i].res;
          if (r<0)
          {
            res->rtyp=-r;
            failed= iiConvert(at,dArith1[i].arg,ai,a,an);
            if (!failed)
            {
              #ifdef PROC_BUG
              dArith1[i].p(res,a);
              #else
              res->data=(char *)((Proc1)dArith1[i].p)((char *)an->Data());
              #endif
            }
          }
          else
          {
            failed= ((iiConvert(at,dArith1[i].arg,ai,a,an))
            || (dArith1[i].p(res,an)));
          }
          // everything done, clean up temp. variables
          if (failed)
          {
            // leave loop, goto error handling
            break;
          }
          else
          {
            if (an->Next() != NULL)
            {
              res->next = (leftv)omAllocBin(sleftv_bin);
              failed=iiExprArith1(res->next,an->next,op);
            }
            // everything ok, clean up and return
            an->CleanUp();
            omFreeBin((ADDRESS)an, sleftv_bin);
            a->CleanUp();
            return failed;
          }
        }
        i++;
      }
      an->CleanUp();
      omFreeBin((ADDRESS)an, sleftv_bin);
    }
    // error handling
    if (!errorreported)
    {
      if ((at==0) && (a->Fullname()!=sNoName))
      {
        Werror("`%s` is not defined",a->Fullname());
      }
      else
      {
        i=ti;
        char *s = iiTwoOps(op);
        Werror("%s(`%s`) is not supported"
                ,s,Tok2Cmdname(at));
        if (BVERBOSE(V_SHOW_USE))
        {
          while (dArith1[i].cmd==op)
          {
            if ((dArith1[i].res!=0)
            && (dArith1[i].p!=jjWRONG))
              Werror("expected %s(`%s`)"
                ,s,Tok2Cmdname(dArith1[i].arg));
            i++;
          }
        }
      }
    }
    res->rtyp = UNKNOWN;
  }
  a->CleanUp();
#endif
  return TRUE;
}

/*=================== operations with 3 args. ============================*/

BOOLEAN iiExprArith3(leftv res, int op, leftv a, leftv b, leftv c)
{
#ifndef GENTABLE
  memset(res,0,sizeof(sleftv));

  if (!errorreported)
  {
#ifdef SIQ
    if (siq>0)
    {
      //Print("siq:%d\n",siq);
      command d=(command)omAlloc0Bin(ip_command_bin);
      memcpy(&d->arg1,a,sizeof(sleftv));
      memcpy(&d->arg2,b,sizeof(sleftv));
      memcpy(&d->arg3,c,sizeof(sleftv));
      d->op=op;
      d->argc=3;
      res->data=(char *)d;
      res->rtyp=COMMAND;
      return FALSE;
    }
#endif
    int at=a->Typ();
    int bt=b->Typ();
    int ct=c->Typ();

    iiOp=op;
    int i=0;
    while ((dArith3[i].cmd!=op)&&(dArith3[i].cmd!=0)) i++;
    while (dArith3[i].cmd==op)
    {
      if ((at==dArith3[i].arg1)
      && (bt==dArith3[i].arg2)
      && (ct==dArith3[i].arg3))
      {
        res->rtyp=dArith3[i].res;
        if (dArith3[i].p(res,a,b,c))
        {
          break;// leave loop, goto error handling
        }
        a->CleanUp();
        b->CleanUp();
        c->CleanUp();
        return FALSE;
      }
      i++;
    }
    // implicite type conversion ----------------------------------------------
    if (dArith3[i].cmd!=op)
    {
      int ai,bi,ci;
      leftv an = (leftv)omAlloc0Bin(sleftv_bin);
      leftv bn = (leftv)omAlloc0Bin(sleftv_bin);
      leftv cn = (leftv)omAlloc0Bin(sleftv_bin);
      BOOLEAN failed=FALSE;
      i=0;
      while ((dArith3[i].cmd!=op)&&(dArith3[i].cmd!=0)) i++;
      while (dArith3[i].cmd==op)
      {
        if ((ai=iiTestConvert(at,dArith3[i].arg1))!=0)
        {
          if ((bi=iiTestConvert(bt,dArith3[i].arg2))!=0)
          {
            if ((ci=iiTestConvert(ct,dArith3[i].arg3))!=0)
            {
              res->rtyp=dArith3[i].res;
              failed= ((iiConvert(at,dArith3[i].arg1,ai,a,an))
                || (iiConvert(bt,dArith3[i].arg2,bi,b,bn))
                || (iiConvert(ct,dArith3[i].arg3,ci,c,cn))
                || (dArith3[i].p(res,an,bn,cn)));
              // everything done, clean up temp. variables
              if (failed)
              {
                // leave loop, goto error handling
                break;
              }
              else
              {
                // everything ok, clean up and return
                an->CleanUp();
                bn->CleanUp();
                cn->CleanUp();
                omFreeBin((ADDRESS)an, sleftv_bin);
                omFreeBin((ADDRESS)bn, sleftv_bin);
                omFreeBin((ADDRESS)cn, sleftv_bin);
                a->CleanUp();
                b->CleanUp();
                c->CleanUp();
        //Print("op: %d,result typ:%d\n",op,res->rtyp);
                return FALSE;
              }
            }
          }
        }
        i++;
      }
      an->CleanUp();
      bn->CleanUp();
      cn->CleanUp();
      omFreeBin((ADDRESS)an, sleftv_bin);
      omFreeBin((ADDRESS)bn, sleftv_bin);
      omFreeBin((ADDRESS)cn, sleftv_bin);
    }
    // error handling ---------------------------------------------------
    if (!errorreported)
    {
      const char *s=NULL;
      if ((at==0) && (a->Fullname()!=sNoName))
      {
        s=a->Fullname();
      }
      else if ((bt==0) && (b->Fullname()!=sNoName))
      {
        s=b->Fullname();
      }
      else if ((ct==0) && (c->Fullname()!=sNoName))
      {
        s=c->Fullname();
      }
      if (s!=NULL)
        Werror("`%s` is not defined",s);
      else
      {
        i=0;
        while ((dArith3[i].cmd!=op)&&(dArith3[i].cmd!=0)) i++;
        char *s = iiTwoOps(op);
        Werror("%s(`%s`,`%s`,`%s`) is not supported"
                ,s,Tok2Cmdname(at),Tok2Cmdname(bt),Tok2Cmdname(ct));
        if (BVERBOSE(V_SHOW_USE))
        {
          while (dArith3[i].cmd==op)
          {
            if(((at==dArith3[i].arg1)
            ||(bt==dArith3[i].arg2)
            ||(ct==dArith3[i].arg3))
            && (dArith3[i].res!=0))
            {
              Werror("expected %s(`%s`,`%s`,`%s`)"
                  ,s,Tok2Cmdname(dArith3[i].arg1)
                  ,Tok2Cmdname(dArith3[i].arg2)
                  ,Tok2Cmdname(dArith3[i].arg3));
            }
            i++;
          }
        }
      }
    }
    res->rtyp = UNKNOWN;
  }
  a->CleanUp();
  b->CleanUp();
  c->CleanUp();
        //Print("op: %d,result typ:%d\n",op,res->rtyp);
#endif
  return TRUE;
}
/*==================== operations with many arg. ===============================*/

BOOLEAN jjANY2LIST(leftv res, leftv v, int cnt)
{
  // cnt = 0: all
  // cnt = 1: only first one
  leftv next;
  BOOLEAN failed = TRUE;
  if(v==NULL) return failed;
  res->rtyp = LIST_CMD;
  if(cnt) v->next = NULL;
  next = v->next;             // saving next-pointer
  failed = jjLIST_PL(res, v);
  v->next = next;             // writeback next-pointer
  return failed;
}

BOOLEAN iiExprArithM(leftv res, leftv a, int op)
{
#ifndef GENTABLE
  memset(res,0,sizeof(sleftv));

  if (!errorreported)
  {
#ifdef SIQ
    if (siq>0)
    {
      //Print("siq:%d\n",siq);
      command d=(command)omAlloc0Bin(ip_command_bin);
      d->op=op;
      res->data=(char *)d;
      if (a!=NULL)
      {
        d->argc=a->listLength();
        // else : d->argc=0;
        memcpy(&d->arg1,a,sizeof(sleftv));
        switch(d->argc)
        {
          case 3:
            memcpy(&d->arg3,a->next->next,sizeof(sleftv));
            a->next->next->rtyp=0;
            a->next->next->data=NULL;
            a->next->next->name=NULL;
            a->next->next->attribute=NULL;
          case 2:
            memcpy(&d->arg2,a->next,sizeof(sleftv));
            a->next->rtyp=0;
            a->next->name=NULL;
            a->next->data=NULL;
            a->next->attribute=NULL;
            d->arg2.next=NULL;
          case 1:
            d->arg1.next=NULL;
        }
        if (d->argc>3) a->next=NULL;
        a->rtyp=0;
        a->data=NULL;
        a->name=NULL;
        a->CleanUp();
      }
      res->rtyp=COMMAND;
      return FALSE;
    }
#endif
    BOOLEAN failed=FALSE;
    int args=0;
    if (a!=NULL) args=a->listLength();

    iiOp=op;
    int i=0;
    while ((dArithM[i].cmd!=op)&&(dArithM[i].cmd!=0)) i++;
    while (dArithM[i].cmd==op)
    {
      if ((args==dArithM[i].number_of_args)
      || (dArithM[i].number_of_args==-1)
      || ((dArithM[i].number_of_args==-2)&&(args>0)))
      {
        res->rtyp=dArithM[i].res;
        if (dArithM[i].p(res,a))
        {
          break;// leave loop, goto error handling
        }
        if (a!=NULL) a->CleanUp();
        //Print("op: %d,result typ:%d\n",op,res->rtyp);
        return failed;
      }
      i++;
    }
    // error handling
    if (!errorreported)
    {
      if ((args>0) && (a->rtyp==0) && (a->Name()!=sNoName))
      {
        Werror("`%s` is not defined",a->Fullname());
      }
      else
      {
        char *s = iiTwoOps(op);
        Werror("%s(...) is not supported",s);
      }
    }
    res->rtyp = UNKNOWN;
  }
  if (a!=NULL) a->CleanUp();
        //Print("op: %d,result typ:%d\n",op,res->rtyp);
#endif
  return TRUE;
}

/*=================== general utilities ============================*/
int IsCmd(char *n, int & tok)
{
  int an=1;
  int i,v;
#ifndef GENTABLE
  int en=LAST_IDENTIFIER;

  loop
  {
    if(an>=en-1)
    {
      if (strcmp(n, cmds[an].name) == 0)
      {
        i=an;
        break;
      }
      else if ((an!=en) && (strcmp(n, cmds[en].name) == 0))
      {
        i=en;
        break;
      }
      else
      {
        return 0;
      }
    }
    i=(an+en)/2;
    if (*n < *(cmds[i].name))
    {
      en=i-1;
    }
    else if (*n > *(cmds[i].name))
    {
      an=i+1;
    }
    else
    {
      v=strcmp(n,cmds[i].name);
      if(v<0)
      {
        en=i-1;
      }
      else if(v>0)
      {
        an=i+1;
      }
      else /*v==0*/
      {
        break;
      }
    }
  }
  lastreserved=cmds[i].name;
  tok=cmds[i].tokval;
  if(cmds[i].alias==2)
  {
    Warn("outdated identifier `%s` used - please change your code",
    cmds[i].name);
    cmds[i].alias=1;
  }
  if (currRingHdl==NULL)
  {
    #ifdef SIQ
    if (siq<=0)
    {
    #endif
      if ((tok>=BEGIN_RING) && (tok<=END_RING))
      {
        WerrorS("no ring active");
        return 0;
      }
    #ifdef SIQ
    }
    #endif
  }
  if (!expected_parms)
  {
    switch (tok)
    {
      case IDEAL_CMD:
      case INT_CMD:
      case INTVEC_CMD:
      case MAP_CMD:
      case MATRIX_CMD:
      case MODUL_CMD:
      case POLY_CMD:
      case PROC_CMD:
      case RING_CMD:
      case STRING_CMD:
        cmdtok = tok;
        break;
    }
  }
  return cmds[i].toktype;
#else
  return 0;
#endif
}
static int iiTabIndex(const jjValCmdTab dArithTab, const int len, const int op)
{
#ifndef GENTABLE
  int a=0;
  int e=len;
  int p=len/2;
  do
  {
     if (op==dArithTab[p].cmd) return dArithTab[p].start;
     if (op<dArithTab[p].cmd) e=p-1;
     else   a = p+1;
     p=a+(e-a)/2;
  }
  while ( a <= e);

#endif
  assume(0);
  return 0;
}

