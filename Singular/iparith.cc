/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT: table driven kernel interface, used by interpreter
*/


#include <kernel/mod2.h>

#include <omalloc/omalloc.h>

#include <factory/factory.h>

#include <coeffs/bigintmat.h>
#include <coeffs/coeffs.h>
#include <coeffs/numbers.h>


#include <misc/options.h>
#include <misc/intvec.h>
#include <misc/sirandom.h>

#include <polys/prCopy.h>
#include <polys/matpol.h>
#include <polys/monomials/maps.h>
#include <polys/coeffrings.h>
#include <polys/sparsmat.h>
#include <polys/weight.h>
#include <polys/ext_fields/transext.h>
#include <polys/clapsing.h>

#include <kernel/combinatorics/stairc.h>
#include <kernel/combinatorics/hilb.h>

#include <kernel/linear_algebra/interpolation.h>
#include <kernel/linear_algebra/linearAlgebra.h>
#include <kernel/linear_algebra/MinorInterface.h>

#include <kernel/spectrum/GMPrat.h>
#include <kernel/groebner_walk/walkProc.h>
#include <kernel/oswrapper/timer.h>
#include <kernel/fglm/fglm.h>

#include <kernel/GBEngine/kstdfac.h>
#include <kernel/GBEngine/syz.h>
#include <kernel/GBEngine/kstd1.h>
#include <kernel/GBEngine/units.h>
#include <kernel/GBEngine/tgb.h>

#include <kernel/preimage.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>

#include <Singular/mod_lib.h>
#include <Singular/fevoices.h>
#include <Singular/tok.h>
#include <Singular/ipid.h>
#include <Singular/sdb.h>
#include <Singular/subexpr.h>
#include <Singular/lists.h>
#include <Singular/maps_ip.h>

#include <Singular/ipconv.h>
#include <Singular/ipprint.h>
#include <Singular/attrib.h>
#include <Singular/links/silink.h>
#include <Singular/misc_ip.h>
#include <Singular/linearAlgebra_ip.h>

#ifdef SINGULAR_4_1
#include <Singular/number2.h>
#endif

#  include <Singular/fglm.h>

#include <Singular/blackbox.h>
#include <Singular/newstruct.h>
#include <Singular/ipshell.h>
//#include <kernel/mpr_inout.h>

#include <reporter/si_signals.h>


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <vector>

lists rDecompose(const ring r);
ring rCompose(const lists  L, const BOOLEAN check_comp=TRUE);


// defaults for all commands: NO_PLURAL | NO_RING | ALLOW_ZERODIVISOR

#ifdef HAVE_PLURAL
  #include <kernel/GBEngine/ratgring.h>
  #include <kernel/GBEngine/nc.h>
  #include <polys/nc/nc.h>
  #include <polys/nc/sca.h>
  #define  PLURAL_MASK 3
#else /* HAVE_PLURAL */
  #define  PLURAL_MASK     0
#endif /* HAVE_PLURAL */

#ifdef HAVE_RINGS
  #define RING_MASK        4
  #define ZERODIVISOR_MASK 8
#else
  #define RING_MASK        0
  #define ZERODIVISOR_MASK 0
#endif
#define ALLOW_PLURAL     1
#define NO_PLURAL        0
#define COMM_PLURAL      2
#define ALLOW_RING       4
#define NO_RING          0
#define NO_ZERODIVISOR   8
#define ALLOW_ZERODIVISOR  0

// bit 4 for warning, if used at toplevel
#define WARN_RING        16

static BOOLEAN check_valid(const int p, const int op);

#ifdef SINGULAR_4_1
// helper routine to catch all library/test parts which need to be changed
// shall go away after the transition
static void iiReWrite(const char *s)
{
  Print("please rewrite the use of >>%s<< in >>%s<<\n"
        "%s is depreciated or changed in Singular 4-1\n",s,my_yylinebuf,s);
}
#endif

/*=============== types =====================*/
struct sValCmdTab
{
  short cmd;
  short start;
};

typedef sValCmdTab jjValCmdTab[];

struct _scmdnames
{
  char *name;
  short alias;
  short tokval;
  short toktype;
};
typedef struct _scmdnames cmdnames;


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

typedef struct
{
  cmdnames *sCmds;             /**< array of existing commands */
  struct sValCmd1 *psValCmd1;
  struct sValCmd2 *psValCmd2;
  struct sValCmd3 *psValCmd3;
  struct sValCmdM *psValCmdM;
  int nCmdUsed;      /**< number of commands used */
  int nCmdAllocated; /**< number of commands-slots allocated */
  int nLastIdentifier; /**< valid indentifieres are slot 1..nLastIdentifier */
} SArithBase;

/*---------------------------------------------------------------------*
 * File scope Variables (Variables share by several functions in
 *                       the same file )
 *
 *---------------------------------------------------------------------*/
static SArithBase sArithBase;  /**< Base entry for arithmetic */

/*---------------------------------------------------------------------*
 * Extern Functions declarations
 *
 *---------------------------------------------------------------------*/
static int _gentable_sort_cmds(const void *a, const void *b);
extern int iiArithRemoveCmd(char *szName);
extern int iiArithAddCmd(const char *szName, short nAlias, short nTokval,
                         short nToktype, short nPos=-1);

/*============= proc =======================*/
static int iiTabIndex(const jjValCmdTab dArithTab, const int len, const int op);
static Subexpr jjMakeSub(leftv e);

/*============= vars ======================*/
extern int cmdtok;
extern BOOLEAN expected_parms;

#define ii_div_by_0 "div. by 0"

int iiOp; /* the current operation*/

/*=================== simple helpers =================*/
poly pHeadProc(poly p)
{
  return pHead(p);
}

int iiTokType(int op)
{
  for (int i=0;i<sArithBase.nCmdUsed;i++)
  {
    if (sArithBase.sCmds[i].tokval==op)
      return sArithBase.sCmds[i].toktype;
  }
  return 0;
}

/*=================== operations with 2 args.: static proc =================*/
/* must be ordered: first operations for chars (infix ops),
 * then alphabetically */

static BOOLEAN jjOP_BIM_I(leftv res, leftv u, leftv v)
{
  bigintmat* aa= (bigintmat *)u->Data();
  int bb = (int)(long)(v->Data());
  if (errorreported) return TRUE;
  bigintmat *cc=NULL;
  switch (iiOp)
  {
    case '+': cc=bimAdd(aa,bb); break;
    case '-': cc=bimSub(aa,bb); break;
    case '*': cc=bimMult(aa,bb); break;
  }
  res->data=(char *)cc;
  return cc==NULL;
}
static BOOLEAN jjOP_I_BIM(leftv res, leftv u, leftv v)
{
  return jjOP_BIM_I(res, v, u);
}
static BOOLEAN jjOP_BIM_BI(leftv res, leftv u, leftv v)
{
  bigintmat* aa= (bigintmat *)u->Data();
  number bb = (number)(v->Data());
  if (errorreported) return TRUE;
  bigintmat *cc=NULL;
  switch (iiOp)
  {
    case '*': cc=bimMult(aa,bb,coeffs_BIGINT); break;
  }
  res->data=(char *)cc;
  return cc==NULL;
}
static BOOLEAN jjOP_BI_BIM(leftv res, leftv u, leftv v)
{
  return jjOP_BIM_BI(res, v, u);
}
static BOOLEAN jjOP_IV_I(leftv res, leftv u, leftv v)
{
  intvec* aa= (intvec *)u->CopyD(INTVEC_CMD);
  int bb = (int)(long)(v->Data());
  if (errorreported) return TRUE;
  switch (iiOp)
  {
    case '+': (*aa) += bb; break;
    case '-': (*aa) -= bb; break;
    case '*': (*aa) *= bb; break;
    case '/':
    case INTDIV_CMD: (*aa) /= bb; break;
    case '%': (*aa) %= bb; break;
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
  int bb = (int)(long)(v->Data());
  int i=si_min(aa->rows(),aa->cols());
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
static BOOLEAN jjCOLON(leftv res, leftv u, leftv v)
{
  int l=(int)(long)v->Data();
  if (l>0)
  {
    int d=(int)(long)u->Data();
    intvec *vv=new intvec(l);
    int i;
    for(i=l-1;i>=0;i--) { (*vv)[i]=d; }
    res->data=(char *)vv;
  }
  return (l<=0);
}
static BOOLEAN jjDOTDOT(leftv res, leftv u, leftv v)
{
  res->data=(char *)new intvec((int)(long)u->Data(),(int)(long)v->Data());
  return FALSE;
}
static void jjEQUAL_REST(leftv res,leftv u,leftv v);
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
  if(r==-2) { WerrorS("size incompatible"); return TRUE; }
  return FALSE;
}
static BOOLEAN jjCOMPARE_BIM(leftv res, leftv u, leftv v)
{
  bigintmat*    a = (bigintmat * )(u->Data());
  bigintmat*    b = (bigintmat * )(v->Data());
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
  if(r==-2) { WerrorS("size incompatible"); return TRUE; }
  return FALSE;
}
static BOOLEAN jjCOMPARE_IV_I(leftv res, leftv u, leftv v)
{
  intvec* a = (intvec * )(u->Data());
  int     b = (int)(long)(v->Data());
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
    number h=nSub(pGetCoeff(p),pGetCoeff(q));
    /* compare lead coeffs */
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
  int b=(int)(long)u->Data();
  int e=(int)(long)v->Data();
  int rc = 1;
  BOOLEAN overflow=FALSE;
  if (e >= 0)
  {
    if (b==0)
    {
      rc=(e==0);
    }
    else if ((e==0)||(b==1))
    {
      rc= 1;
    }
    else if (b== -1)
    {
      if (e&1) rc= -1;
      else     rc= 1;
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
    res->data = (char *)((long)rc);
    if (u!=NULL) return jjOP_REST(res,u,v);
    return FALSE;
  }
  else
  {
    WerrorS("exponent must be non-negative");
    return TRUE;
  }
}
static BOOLEAN jjPOWER_BI(leftv res, leftv u, leftv v)
{
  int e=(int)(long)v->Data();
  number n=(number)u->Data();
  if (e>=0)
  {
    n_Power(n,e,(number*)&res->data,coeffs_BIGINT);
  }
  else
  {
    WerrorS("exponent must be non-negative");
    return TRUE;
  }
  if (u!=NULL) return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjPOWER_N(leftv res, leftv u, leftv v)
{
  int e=(int)(long)v->Data();
  number n=(number)u->Data();
  int d=0;
  if (e<0)
  {
    n=nInvers(n);
    e=-e;
    d=1;
  }
  number r;
  nPower(n,e,(number*)&r);
  res->data=(char*)r;
  if (d) nDelete(&n);
  if (u!=NULL) return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjPOWER_P(leftv res, leftv u, leftv v)
{
  int v_i=(int)(long)v->Data();
  if (v_i<0)
  {
    WerrorS("exponent must be non-negative");
    return TRUE;
  }
  poly u_p=(poly)u->CopyD(POLY_CMD);
  if ((u_p!=NULL)
  && ((v_i!=0) &&
      ((long)pTotaldegree(u_p) > (signed long)currRing->bitmask / (signed long)v_i/2)))
  {
    Werror("OVERFLOW in power(d=%ld, e=%d, max=%ld)",
                                    pTotaldegree(u_p),v_i,currRing->bitmask/2);
    pDelete(&u_p);
    return TRUE;
  }
  res->data = (char *)pPower(u_p,v_i);
  if (u!=NULL) return jjOP_REST(res,u,v);
  return errorreported; /* pPower may set errorreported via Werror */
}
static BOOLEAN jjPOWER_ID(leftv res, leftv u, leftv v)
{
  res->data = (char *)id_Power((ideal)(u->Data()),(int)(long)(v->Data()), currRing);
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
  idhdl packhdl;
  switch(u->Typ())
  {
      case 0:
      {
        int name_err=0;
        if(isupper(u->name[0]))
        {
          const char *c=u->name+1;
          while((*c!='\0')&&(islower(*c)||(isdigit(*c)))) c++;
          if (*c!='\0')
            name_err=1;
          else
          {
            Print("%s of type 'ANY'. Trying load.\n", u->name);
            if(iiTryLoadLib(u, u->name))
            {
              Werror("'%s' no such package", u->name);
              return TRUE;
            }
            syMake(u,u->name,NULL);
          }
        }
        else name_err=1;
        if(name_err)
        { Werror("'%s' is an invalid package name",u->name);return TRUE;}
        // and now, after the loading: use next case !!! no break !!!
      }
      case PACKAGE_CMD:
        packhdl = (idhdl)u->data;
        if((!IDPACKAGE(packhdl)->loaded)
        && (IDPACKAGE(packhdl)->language > LANG_TOP))
        {
          Werror("'%s' not loaded", u->name);
          return TRUE;
        }
        if(v->rtyp == IDHDL)
        {
          v->name = omStrDup(v->name);
        }
        else if (v->rtyp!=0)
        {
          WerrorS("reserved name with ::");
          return TRUE;
        }
        v->req_packhdl=IDPACKAGE(packhdl);
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
  return FALSE;
}
static BOOLEAN jjPLUS_I(leftv res, leftv u, leftv v)
{
  unsigned int a=(unsigned int)(unsigned long)u->Data();
  unsigned int b=(unsigned int)(unsigned long)v->Data();
  unsigned int c=a+b;
  res->data = (char *)((long)c);
  if (((Sy_bit(31)&a)==(Sy_bit(31)&b))&&((Sy_bit(31)&a)!=(Sy_bit(31)&c)))
  {
    WarnS("int overflow(+), result may be wrong");
  }
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjPLUS_BI(leftv res, leftv u, leftv v)
{
  res->data = (char *)(n_Add((number)u->Data(), (number)v->Data(),coeffs_BIGINT));
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
static BOOLEAN jjPLUS_BIM(leftv res, leftv u, leftv v)
{
  res->data = (char *)bimAdd((bigintmat*)(u->Data()), (bigintmat*)(v->Data()));
  if (res->data==NULL)
  {
    WerrorS("bigintmat/cmatrix not compatible");
    return TRUE;
  }
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjPLUS_MA(leftv res, leftv u, leftv v)
{
  matrix A=(matrix)u->Data(); matrix B=(matrix)v->Data();
  res->data = (char *)(mp_Add(A , B, currRing));
  if (res->data==NULL)
  {
     Werror("matrix size not compatible(%dx%d, %dx%d)",
             MATROWS(A),MATCOLS(A),MATROWS(B),MATCOLS(B));
     return TRUE;
  }
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjPLUS_MA_P(leftv res, leftv u, leftv v)
{
  matrix m=(matrix)u->Data();
  matrix p= mp_InitP(m->nrows,m->ncols,(poly)(v->CopyD(POLY_CMD)),currRing);
  if (iiOp=='+')
    res->data = (char *)mp_Add(m , p,currRing);
  else
    res->data = (char *)mp_Sub(m , p,currRing);
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
  void *ap=u->Data(); void *bp=v->Data();
  int aa=(int)(long)ap;
  int bb=(int)(long)bp;
  int cc=aa-bb;
  unsigned int a=(unsigned int)(unsigned long)ap;
  unsigned int b=(unsigned int)(unsigned long)bp;
  unsigned int c=a-b;
  if (((Sy_bit(31)&a)!=(Sy_bit(31)&b))&&((Sy_bit(31)&a)!=(Sy_bit(31)&c)))
  {
    WarnS("int overflow(-), result may be wrong");
  }
  res->data = (char *)((long)cc);
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjMINUS_BI(leftv res, leftv u, leftv v)
{
  res->data = (char *)(n_Sub((number)u->Data(), (number)v->Data(),coeffs_BIGINT));
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
static BOOLEAN jjMINUS_BIM(leftv res, leftv u, leftv v)
{
  res->data = (char *)bimSub((bigintmat*)(u->Data()), (bigintmat*)(v->Data()));
  if (res->data==NULL)
  {
    WerrorS("bigintmat/cmatrix not compatible");
    return TRUE;
  }
  return jjPLUSMINUS_Gen(res,u,v);
}
static BOOLEAN jjMINUS_MA(leftv res, leftv u, leftv v)
{
  matrix A=(matrix)u->Data(); matrix B=(matrix)v->Data();
  res->data = (char *)(mp_Sub(A , B, currRing));
  if (res->data==NULL)
  {
     Werror("matrix size not compatible(%dx%d, %dx%d)",
             MATROWS(A),MATCOLS(A),MATROWS(B),MATCOLS(B));
     return TRUE;
  }
  return jjPLUSMINUS_Gen(res,u,v);
  return FALSE;
}
static BOOLEAN jjTIMES_I(leftv res, leftv u, leftv v)
{
  int a=(int)(long)u->Data();
  int b=(int)(long)v->Data();
  int64 c=(int64)a * (int64)b;
  if ((c>INT_MAX)||(c<INT_MIN))
    WarnS("int overflow(*), result may be wrong");
  res->data = (char *)((long)((int)c));
  if ((u->Next()!=NULL) || (v->Next()!=NULL))
    return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjTIMES_BI(leftv res, leftv u, leftv v)
{
  res->data = (char *)(n_Mult( (number)u->Data(), (number)v->Data(),coeffs_BIGINT));
  if ((v->next!=NULL) || (u->next!=NULL))
    return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjTIMES_N(leftv res, leftv u, leftv v)
{
  res->data = (char *)(nMult( (number)u->Data(), (number)v->Data()));
  number n=(number)res->data;
  nNormalize(n);
  res->data=(char *)n;
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
      if ((a!=NULL) && (b!=NULL)
      && ((long)pTotaldegree(a)>si_max((long)rVar(currRing),(long)currRing->bitmask/2)-(long)pTotaldegree(b)))
      {
        Warn("possible OVERFLOW in mult(d=%ld, d=%ld, max=%ld)",
          pTotaldegree(a),pTotaldegree(b),currRing->bitmask/2);
      }
      res->data = (char *)(pMult( a, b));
      pNormalize((poly)res->data);
      return FALSE;
    }
    // u->next exists: copy v
    b=pCopy((poly)v->Data());
    if ((a!=NULL) && (b!=NULL)
    && (pTotaldegree(a)+pTotaldegree(b)>si_max((long)rVar(currRing),(long)currRing->bitmask/2)))
    {
      Warn("possible OVERFLOW in mult(d=%ld, d=%ld, max=%ld)",
          pTotaldegree(a),pTotaldegree(b),currRing->bitmask/2);
    }
    res->data = (char *)(pMult( a, b));
    pNormalize((poly)res->data);
    return jjOP_REST(res,u,v);
  }
  // v->next exists: copy u
  a=pCopy((poly)u->Data());
  b=(poly)v->CopyD(POLY_CMD); // works also for VECTOR_CMD
  if ((a!=NULL) && (b!=NULL)
  && ((unsigned long)(pTotaldegree(a)+pTotaldegree(b))>=currRing->bitmask/2))
  {
    pDelete(&a);
    pDelete(&b);
    WerrorS("OVERFLOW");
    return TRUE;
  }
  res->data = (char *)(pMult( a, b));
  pNormalize((poly)res->data);
  return jjOP_REST(res,u,v);
}
static BOOLEAN jjTIMES_ID(leftv res, leftv u, leftv v)
{
  res->data = (char *)idMult((ideal)u->Data(),(ideal)v->Data());
  id_Normalize((ideal)res->data,currRing);
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
static BOOLEAN jjTIMES_BIM(leftv res, leftv u, leftv v)
{
  res->data = (char *)bimMult((bigintmat*)(u->Data()), (bigintmat*)(v->Data()));
  if (res->data==NULL)
  {
    WerrorS("bigintmat/cmatrix not compatible");
    return TRUE;
  }
  if ((v->next!=NULL) || (u->next!=NULL))
    return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjTIMES_MA_BI1(leftv res, leftv u, leftv v)
{
  nMapFunc nMap=n_SetMap(coeffs_BIGINT,currRing->cf);
  if (nMap==NULL) return TRUE;
  number n=nMap((number)v->Data(),coeffs_BIGINT,currRing->cf);
  poly p=pNSet(n);
  ideal I= (ideal)mp_MultP((matrix)u->CopyD(MATRIX_CMD),p,currRing);
  res->data = (char *)I;
  return FALSE;
}
static BOOLEAN jjTIMES_MA_BI2(leftv res, leftv u, leftv v)
{
  return jjTIMES_MA_BI1(res,v,u);
}
static BOOLEAN jjTIMES_MA_P1(leftv res, leftv u, leftv v)
{
  poly p=(poly)v->CopyD(POLY_CMD);
  int r=pMaxComp(p);/* recompute the rank for the case ideal*vector*/
  ideal I= (ideal)mp_MultP((matrix)u->CopyD(MATRIX_CMD),p,currRing);
  if (r>0) I->rank=r;
  id_Normalize(I,currRing);
  res->data = (char *)I;
  return FALSE;
}
static BOOLEAN jjTIMES_MA_P2(leftv res, leftv u, leftv v)
{
  poly p=(poly)u->CopyD(POLY_CMD);
  int r=pMaxComp(p);/* recompute the rank for the case ideal*vector*/
  ideal I= (ideal)pMultMp(p,(matrix)v->CopyD(MATRIX_CMD),currRing);
  if (r>0) I->rank=r;
  id_Normalize(I,currRing);
  res->data = (char *)I;
  return FALSE;
}
static BOOLEAN jjTIMES_MA_N1(leftv res, leftv u, leftv v)
{
  number n=(number)v->CopyD(NUMBER_CMD);
  poly p=pNSet(n);
  res->data = (char *)mp_MultP((matrix)u->CopyD(MATRIX_CMD),p,currRing);
  id_Normalize((ideal)res->data,currRing);
  return FALSE;
}
static BOOLEAN jjTIMES_MA_N2(leftv res, leftv u, leftv v)
{
  return jjTIMES_MA_N1(res,v,u);
}
static BOOLEAN jjTIMES_MA_I1(leftv res, leftv u, leftv v)
{
  res->data = (char *)mp_MultI((matrix)u->CopyD(MATRIX_CMD),(int)(long)v->Data(),currRing);
  id_Normalize((ideal)res->data,currRing);
  return FALSE;
}
static BOOLEAN jjTIMES_MA_I2(leftv res, leftv u, leftv v)
{
  return jjTIMES_MA_I1(res,v,u);
}
static BOOLEAN jjTIMES_MA(leftv res, leftv u, leftv v)
{
  matrix A=(matrix)u->Data(); matrix B=(matrix)v->Data();
  res->data = (char *)mp_Mult(A,B,currRing);
  if (res->data==NULL)
  {
     Werror("matrix size not compatible(%dx%d, %dx%d)",
             MATROWS(A),MATCOLS(A),MATROWS(B),MATCOLS(B));
     return TRUE;
  }
  id_Normalize((ideal)res->data,currRing);
  if ((v->next!=NULL) || (u->next!=NULL))
    return jjOP_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjGE_BI(leftv res, leftv u, leftv v)
{
  number h=n_Sub((number)u->Data(),(number)v->Data(),coeffs_BIGINT);
  res->data = (char *) (n_GreaterZero(h,coeffs_BIGINT)||(n_IsZero(h,coeffs_BIGINT)));
  n_Delete(&h,coeffs_BIGINT);
  return FALSE;
}
static BOOLEAN jjGE_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)(long)((int)((long)u->Data()) >= (int)((long)v->Data()));
  return FALSE;
}
static BOOLEAN jjGE_N(leftv res, leftv u, leftv v)
{
  res->data = (char *)(long) (nGreater((number)u->Data(),(number)v->Data())
                       || nEqual((number)u->Data(),(number)v->Data()));
  return FALSE;
}
static BOOLEAN jjGT_BI(leftv res, leftv u, leftv v)
{
  number h=n_Sub((number)u->Data(),(number)v->Data(),coeffs_BIGINT);
  res->data = (char *)(long) (n_GreaterZero(h,coeffs_BIGINT)&&(!n_IsZero(h,coeffs_BIGINT)));
  n_Delete(&h,coeffs_BIGINT);
  return FALSE;
}
static BOOLEAN jjGT_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)(long)((int)((long)u->Data()) > (int)((long)v->Data()));
  return FALSE;
}
static BOOLEAN jjGT_N(leftv res, leftv u, leftv v)
{
  res->data = (char *)(long)(nGreater((number)u->Data(),(number)v->Data()));
  return FALSE;
}
static BOOLEAN jjLE_BI(leftv res, leftv u, leftv v)
{
  return jjGE_BI(res,v,u);
}
static BOOLEAN jjLE_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)(long)((int)((long)u->Data()) <= (int)((long)v->Data()));
  return FALSE;
}
static BOOLEAN jjLE_N(leftv res, leftv u, leftv v)
{
  return jjGE_N(res,v,u);
}
static BOOLEAN jjLT_BI(leftv res, leftv u, leftv v)
{
  return jjGT_BI(res,v,u);
}
static BOOLEAN jjLT_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)(long)((int)((long)u->Data()) < (int)((long)v->Data()));
  return FALSE;
}
static BOOLEAN jjLT_N(leftv res, leftv u, leftv v)
{
  return jjGT_N(res,v,u);
}
static BOOLEAN jjDIVMOD_I(leftv res, leftv u, leftv v)
{
  if (iiOp=='/') Warn("int division with `/`: use `div` instead in line >>%s<<",my_yylinebuf);
  int a= (int)(long)u->Data();
  int b= (int)(long)v->Data();
  if (b==0)
  {
    WerrorS(ii_div_by_0);
    return TRUE;
  }
  int c=a%b;
  int r=0;
  switch (iiOp)
  {
    case '%':
        r=c;            break;
    case '/':
    case INTDIV_CMD:
        r=((a-c) /b);   break;
  }
  res->data=(void *)((long)r);
  return FALSE;
}
static BOOLEAN jjDIV_BI(leftv res, leftv u, leftv v)
{
  number q=(number)v->Data();
  if (n_IsZero(q,coeffs_BIGINT))
  {
    WerrorS(ii_div_by_0);
    return TRUE;
  }
  q = n_Div((number)u->Data(),q,coeffs_BIGINT);
  n_Normalize(q,coeffs_BIGINT);
  res->data = (char *)q;
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
  if ((pNext(q)!=NULL) && (!rField_is_Ring(currRing)))
  { /* This means that q != 0 consists of at least two terms.
       Moreover, currRing is over a field. */
    if(pGetComp(p)==0)
    {
      res->data=(void*)(singclap_pdivide(p /*(poly)(u->Data())*/ ,
                                         q /*(poly)(v->Data())*/ ,currRing));
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
          h=singclap_pdivide(I->m[i],q,currRing);
          pSetCompP(h,i+1);
          p=pAdd(p,h);
        }
      }
      idDelete(&I);
      res->data=(void *)p;
    }
  }
  else
  { /* This means that q != 0 consists of just one term,
       or that currRing is over a coefficient ring. */
#ifdef HAVE_RINGS
    if (!rField_is_Domain(currRing))
    {
      WerrorS("division only defined over coefficient domains");
      return TRUE;
    }
    if (pNext(q)!=NULL)
    {
      WerrorS("division over a coefficient domain only implemented for terms");
      return TRUE;
    }
#endif
    res->data = (char *)pDivideM(pCopy(p),pHead(q));
  }
  pNormalize((poly)res->data);
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
        MATELEM(mm,i,j) = singclap_pdivide( MATELEM(m,i,j) ,
                                           q /*(poly)(v->Data())*/, currRing );
      }
      else
        MATELEM(mm,i,j) = pDivideM(pCopy(MATELEM(m,i,j)),pHead(q));
    }
  }
  id_Normalize((ideal)mm,currRing);
  res->data=(char *)mm;
  return FALSE;
}
static BOOLEAN jjEQUAL_BI(leftv res, leftv u, leftv v)
{
  res->data = (char *)((long)n_Equal((number)u->Data(),(number)v->Data(),coeffs_BIGINT));
  jjEQUAL_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjEQUAL_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)((int)((long)u->Data()) == (int)((long)v->Data()));
  jjEQUAL_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjEQUAL_Ma(leftv res, leftv u, leftv v)
{
  res->data = (char *)((long)mp_Equal((matrix)u->Data(),(matrix)v->Data(),currRing));
  jjEQUAL_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjEQUAL_N(leftv res, leftv u, leftv v)
{
  res->data = (char *)((long)nEqual((number)u->Data(),(number)v->Data()));
  jjEQUAL_REST(res,u,v);
  return FALSE;
}
static BOOLEAN jjEQUAL_P(leftv res, leftv u, leftv v)
{
  poly p=(poly)u->Data();
  poly q=(poly)v->Data();
  res->data = (char *) ((long)pEqualPolys(p,q));
  jjEQUAL_REST(res,u,v);
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
  if (iiOp==NOTEQUAL) res->data=(char *)(!(long)res->data);
}
static BOOLEAN jjAND_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)((long)u->Data() && (long)v->Data());
  return FALSE;
}
static BOOLEAN jjOR_I(leftv res, leftv u, leftv v)
{
  res->data = (char *)((long)u->Data() || (long)v->Data());
  return FALSE;
}
static BOOLEAN jjINDEX_I(leftv res, leftv u, leftv v)
{
  res->rtyp=u->rtyp; u->rtyp=0;
  res->data=u->data; u->data=NULL;
  res->name=u->name; u->name=NULL;
  res->e=u->e;       u->e=NULL;
  if (res->e==NULL) res->e=jjMakeSub(v);
  else
  {
    Subexpr sh=res->e;
    while (sh->next != NULL) sh=sh->next;
    sh->next=jjMakeSub(v);
  }
  if (u->next!=NULL)
  {
    leftv rn=(leftv)omAlloc0Bin(sleftv_bin);
    BOOLEAN bo=iiExprArith2(rn,u->next,iiOp,v);
    res->next=rn;
    return bo;
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
    t.data=(char *)((long)(*iv)[i]);
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
    p->flag=u->flag;
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
  int i=(int)(long)v->Data();
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
  unsigned i=(unsigned)(long)v->Data();
  while (p!=NULL)
  {
    if (pGetComp(p)!=i)
    {
      if (r==p) r=pNext(p);
      if (o!=NULL)
      {
        if (pNext(o)!=NULL) pLmDelete(&pNext(o));
        p=pNext(o);
      }
      else
        pLmDelete(&p);
    }
    else
    {
      pSetComp(p, 0);
      p_SetmComp(p, currRing);
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
        if (((int)pGetComp(p))==(*iv)[i])
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
        pLmDelete(&p);
        if (p==NULL) break;
      }
    }
    pLmDelete(&r);
    res->data=(char *)r;
  }
  return FALSE;
}
static BOOLEAN jjKLAMMER_rest(leftv res, leftv u, leftv v);
static BOOLEAN jjKLAMMER(leftv res, leftv u, leftv v)
{
  if(u->name==NULL) return TRUE;
  char * nn = (char *)omAlloc(strlen(u->name) + 14);
  sprintf(nn,"%s(%d)",u->name,(int)(long)v->Data());
  omFree((ADDRESS)u->name);
  u->name=NULL;
  char *n=omStrDup(nn);
  omFree((ADDRESS)nn);
  syMake(res,n);
  if (u->next!=NULL) return jjKLAMMER_rest(res,u->next,v);
  return FALSE;
}
static BOOLEAN jjKLAMMER_IV(leftv res, leftv u, leftv v)
{
  intvec * iv=(intvec *)v->Data();
  leftv p=NULL;
  int i;
  long slen = strlen(u->name) + 14;
  char *n = (char*) omAlloc(slen);

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
  if (u->next!=NULL) return jjKLAMMER_rest(res,u->next,v);
  return FALSE;
}
static BOOLEAN jjKLAMMER_rest(leftv res, leftv u, leftv v)
{
  leftv tmp=(leftv)omAllocBin(sleftv_bin);
  memset(tmp,0,sizeof(sleftv));
  BOOLEAN b;
  if (v->Typ()==INTVEC_CMD)
    b=jjKLAMMER_IV(tmp,u,v);
  else
    b=jjKLAMMER(tmp,u,v);
  if (b)
  {
    omFreeBin(tmp,sleftv_bin);
    return TRUE;
  }
  leftv h=res;
  while (h->next!=NULL) h=h->next;
  h->next=tmp;
  return FALSE;
}
BOOLEAN jjPROC(leftv res, leftv u, leftv v)
{
  void *d;
  Subexpr e;
  int typ;
  BOOLEAN t=FALSE;
  idhdl tmp_proc=NULL;
  if ((u->rtyp!=IDHDL)||(u->e!=NULL))
  {
    tmp_proc=(idhdl)omAlloc0(sizeof(idrec));
    tmp_proc->id="_auto";
    tmp_proc->typ=PROC_CMD;
    tmp_proc->data.pinf=(procinfo *)u->Data();
    tmp_proc->ref=1;
    d=u->data; u->data=(void *)tmp_proc;
    e=u->e; u->e=NULL;
    t=TRUE;
    typ=u->rtyp; u->rtyp=IDHDL;
  }
  BOOLEAN sl;
  if (u->req_packhdl==currPack)
    sl = iiMake_proc((idhdl)u->data,NULL,v);
  else
    sl = iiMake_proc((idhdl)u->data,u->req_packhdl,v);
  if (t)
  {
    u->rtyp=typ;
    u->data=d;
    u->e=e;
    omFreeSize(tmp_proc,sizeof(idrec));
  }
  if (sl) return TRUE;
  memcpy(res,&iiRETURNEXPR,sizeof(sleftv));
  iiRETURNEXPR.Init();
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
static BOOLEAN jjCHINREM_BI(leftv res, leftv u, leftv v)
{
  intvec *c=(intvec*)u->Data();
  intvec* p=(intvec*)v->Data();
  int rl=p->length();
  number *x=(number *)omAlloc(rl*sizeof(number));
  number *q=(number *)omAlloc(rl*sizeof(number));
  int i;
  for(i=rl-1;i>=0;i--)
  {
    q[i]=n_Init((*p)[i], coeffs_BIGINT);
    x[i]=n_Init((*c)[i], coeffs_BIGINT);
  }
  number n=n_ChineseRemainderSym(x,q,rl,FALSE,coeffs_BIGINT);
  for(i=rl-1;i>=0;i--)
  {
    n_Delete(&(q[i]),coeffs_BIGINT);
    n_Delete(&(x[i]),coeffs_BIGINT);
  }
  omFree(x); omFree(q);
  res->data=(char *)n;
  return FALSE;
}
#if 0
static BOOLEAN jjCHINREM_P(leftv res, leftv u, leftv v)
{
  lists c=(lists)u->CopyD(); // list of poly
  intvec* p=(intvec*)v->Data();
  int rl=p->length();
  poly r=NULL,h, result=NULL;
  number *x=(number *)omAlloc(rl*sizeof(number));
  number *q=(number *)omAlloc(rl*sizeof(number));
  int i;
  for(i=rl-1;i>=0;i--)
  {
    q[i]=nlInit((*p)[i]);
  }
  loop
  {
    for(i=rl-1;i>=0;i--)
    {
      if (c->m[i].Typ()!=POLY_CMD)
      {
        Werror("poly expected at pos %d",i+1);
        for(i=rl-1;i>=0;i--)
        {
          nlDelete(&(q[i]),currRing);
        }
        omFree(x); omFree(q); // delete c
        return TRUE;
      }
      h=((poly)c->m[i].Data());
      if (r==NULL) r=h;
      else if (pLmCmp(r,h)==-1) r=h;
    }
    if (r==NULL) break;
    for(i=rl-1;i>=0;i--)
    {
      h=((poly)c->m[i].Data());
      if (pLmCmp(r,h)==0)
      {
        x[i]=pGetCoeff(h);
        h=pLmFreeAndNext(h);
        c->m[i].data=(char*)h;
      }
      else
        x[i]=nlInit(0);
    }
    number n=n_ChineseRemainder(x,q,rl,currRing->cf);
    for(i=rl-1;i>=0;i--)
    {
      nlDelete(&(x[i]),currRing);
    }
    h=pHead(r);
    pSetCoeff(h,n);
    result=pAdd(result,h);
  }
  for(i=rl-1;i>=0;i--)
  {
    nlDelete(&(q[i]),currRing);
  }
  omFree(x); omFree(q);
  res->data=(char *)result;
  return FALSE;
}
#endif
static BOOLEAN jjCHINREM_ID(leftv res, leftv u, leftv v)
{
  coeffs cf;
  lists c=(lists)u->CopyD(); // list of ideal or bigint/int
  lists pl=NULL;
  intvec *p=NULL;
  if (v->Typ()==LIST_CMD) pl=(lists)v->Data();
  else                    p=(intvec*)v->Data();
  int rl=c->nr+1;
  ideal result;
  ideal *x=(ideal *)omAlloc(rl*sizeof(ideal));
  number *xx=NULL;
  int i;
  int return_type=c->m[0].Typ();
  if ((return_type!=IDEAL_CMD)
  && (return_type!=MODUL_CMD)
  && (return_type!=MATRIX_CMD)
  && (return_type!=POLY_CMD))
  {
    if((return_type!=BIGINT_CMD)&&(return_type!=INT_CMD))
    {
      WerrorS("poly/ideal/module/matrix expected");
      omFree(x); // delete c
      return TRUE;
    }
    else
      return_type=BIGINT_CMD;
  }
  if (return_type==BIGINT_CMD)
    cf=coeffs_BIGINT;
  else
  {
    cf=currRing->cf;
    if (nCoeff_is_Extension(cf) && (cf->extRing!=NULL))
      cf=cf->extRing->cf;
  }
  nMapFunc nMap=n_SetMap(coeffs_BIGINT,cf);
  if (return_type!=BIGINT_CMD)
  {
    for(i=rl-1;i>=0;i--)
    {
      if (c->m[i].Typ()!=return_type)
      {
        Werror("%s expected at pos %d",Tok2Cmdname(return_type),i+1);
        omFree(x); // delete c
        return TRUE;
      }
      if (return_type==POLY_CMD)
      {
        x[i]=idInit(1,1);
        x[i]->m[0]=(poly)c->m[i].CopyD();
      }
      else
      {
        x[i]=(ideal)c->m[i].CopyD();
      }
      //c->m[i].Init();
    }
  }
  else
  {
    xx=(number *)omAlloc(rl*sizeof(number));
    if (nMap==NULL)
    {
      Werror("not implemented: map bigint -> %s", nCoeffString(cf));
      return TRUE;
    }
    for(i=rl-1;i>=0;i--)
    {
      if (c->m[i].Typ()==INT_CMD)
      {
        xx[i]=n_Init(((int)(long)c->m[i].Data()),cf);
      }
      else if (c->m[i].Typ()==BIGINT_CMD)
      {
        xx[i]=nMap((number)c->m[i].Data(),coeffs_BIGINT,cf);
      }
      else
      {
        Werror("bigint expected at pos %d",i+1);
        omFree(x); // delete c
        omFree(xx); // delete c
        return TRUE;
      }
    }
  }
  number *q=(number *)omAlloc(rl*sizeof(number));
  if (p!=NULL)
  {
    for(i=rl-1;i>=0;i--)
    {
      q[i]=n_Init((*p)[i], cf);
    }
  }
  else
  {
    for(i=rl-1;i>=0;i--)
    {
      if (pl->m[i].Typ()==INT_CMD)
      {
        q[i]=n_Init((int)(long)pl->m[i].Data(),cf);
      }
      else if (pl->m[i].Typ()==BIGINT_CMD)
      {
        q[i]=nMap((number)(pl->m[i].Data()),coeffs_BIGINT,cf);
      }
      else
      {
        Werror("bigint expected at pos %d",i+1);
        for(i++;i<rl;i++)
        {
          n_Delete(&(q[i]),cf);
        }
        omFree(x); // delete c
        omFree(q); // delete pl
        if (xx!=NULL) omFree(xx); // delete c
        return TRUE;
      }
    }
  }
  if (return_type==BIGINT_CMD)
  {
    number n=n_ChineseRemainderSym(xx,q,rl,TRUE,coeffs_BIGINT);
    res->data=(char *)n;
  }
  else
  {
    result=id_ChineseRemainder(x,q,rl,currRing);
    // deletes also x
    c->Clean();
    if (return_type==POLY_CMD)
    {
      res->data=(char *)result->m[0];
      result->m[0]=NULL;
      idDelete(&result);
    }
    else
      res->data=(char *)result;
  }
  for(i=rl-1;i>=0;i--)
  {
    n_Delete(&(q[i]),cf);
  }
  omFree(q);
  res->rtyp=return_type;
  return FALSE;
}
static BOOLEAN jjCOEF(leftv res, leftv u, leftv v)
{
  poly p=(poly)v->Data();
  if ((p==NULL)||(pNext(p)!=NULL)) return TRUE;
  res->data=(char *)mp_CoeffProc((poly)u->Data(),p /*(poly)v->Data()*/,currRing);
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
  res->data=(char *)mp_Coeffs((ideal)u->CopyD(),i,currRing);
  return FALSE;
}
static BOOLEAN jjCOEFFS2_KB(leftv res, leftv u, leftv v)
{
  poly p = pInit();
  int i;
  for (i=1; i<=currRing->N; i++)
  {
    pSetExp(p, i, 1);
  }
  pSetm(p);
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
static BOOLEAN jjDEG_M_IV(leftv res, leftv u, leftv v)
{
  short *iv=iv2array((intvec *)v->Data(),currRing);
  ideal I=(ideal)u->Data();
  int d=-1;
  int i;
  for(i=IDELEMS(I);i>=0;i--) d=si_max(d,(int)p_DegW(I->m[i],iv,currRing));
  omFreeSize( (ADDRESS)iv, (rVar(currRing)+1)*sizeof(short) );
  res->data = (char *)((long)d);
  return FALSE;
}
static BOOLEAN jjDEG_IV(leftv res, leftv u, leftv v)
{
  poly p=(poly)u->Data();
  if (p!=NULL)
  {
    short *iv=iv2array((intvec *)v->Data(),currRing);
    const long d = p_DegW(p,iv,currRing);
    omFreeSize( (ADDRESS)iv, (rVar(currRing)+1)*sizeof(short) );
    res->data = (char *)(d);
  }
  else
    res->data=(char *)(long)(-1);
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
static BOOLEAN jjDIM2(leftv res, leftv v, leftv w)
{
  assumeStdFlag(v);
#ifdef HAVE_RINGS
  if (rField_is_Ring(currRing))
  {
    //ring origR = currRing;
    //ring tempR = rCopy(origR);
    //coeffs new_cf=nInitChar(n_Q,NULL);
    //nKillChar(tempR->cf);
    //tempR->cf=new_cf;
    //rComplete(tempR);
    ideal vid = (ideal)v->Data();
    int i = idPosConstant(vid);
    if ((i != -1) && (n_IsUnit(pGetCoeff(vid->m[i]),currRing->cf)))
    { /* ideal v contains unit; dim = -1 */
      res->data = (char *)-1;
      return FALSE;
    }
    //rChangeCurrRing(tempR);
    //ideal vv = idrCopyR(vid, origR, currRing);
    ideal vv = id_Copy(vid, currRing);
    //ideal ww = idrCopyR((ideal)w->Data(), origR, currRing);
    ideal ww = id_Copy((ideal)w->Data(), currRing);
    /* drop degree zero generator from vv (if any) */
    if (i != -1) pDelete(&vv->m[i]);
    long d = (long)scDimInt(vv, ww);
    if (rField_is_Ring_Z(currRing) && (i == -1)) d++;
    res->data = (char *)d;
    idDelete(&vv); idDelete(&ww);
    //rChangeCurrRing(origR);
    //rDelete(tempR);
    return FALSE;
  }
#endif
  if(currRing->qideal==NULL)
    res->data = (char *)((long)scDimInt((ideal)(v->Data()),(ideal)w->Data()));
  else
  {
    ideal q=idSimpleAdd(currRing->qideal,(ideal)w->Data());
    res->data = (char *)((long)scDimInt((ideal)(v->Data()),q));
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
  if (m==NULL) return TRUE;
  // now make sure that all matices have the corect size:
  matrix T = id_Module2formatedMatrix(m,vl,ul,currRing);
  int i;
  if (MATCOLS(U) != ul)
  {
    int mul=si_min(ul,MATCOLS(U));
    matrix UU=mpNew(ul,ul);
    int j;
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
  // make sure that U is a diagonal matrix of units
  for(i=ul;i>0;i--)
  {
    if(MATELEM(U,i,i)==NULL) MATELEM(U,i,i)=pOne();
  }
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(3);
  L->m[0].rtyp=MATRIX_CMD;   L->m[0].data=(void *)T;
  L->m[1].rtyp=u->Typ();     L->m[1].data=(void *)R;
  L->m[2].rtyp=MATRIX_CMD;   L->m[2].data=(void *)U;
  res->data=(char *)L;
  return FALSE;
}
static BOOLEAN jjELIMIN(leftv res, leftv u, leftv v)
{
  res->data=(char *)idElimination((ideal)u->Data(),(poly)v->Data());
  //setFlag(res,FLAG_STD);
  return v->next!=NULL; //do not allow next like in eliminate(I,a(1..4))
}
static BOOLEAN jjELIMIN_IV(leftv res, leftv u, leftv v)
{
  poly p=pOne();
  intvec *iv=(intvec*)v->Data();
  for(int i=iv->length()-1; i>=0; i--)
  {
    pSetExp(p,(*iv)[i],1);
  }
  pSetm(p);
  res->data=(char *)idElimination((ideal)u->Data(),p);
  pLmDelete(&p);
  //setFlag(res,FLAG_STD);
  return FALSE;
}
static BOOLEAN jjEXPORTTO(leftv, leftv u, leftv v)
{
  //Print("exportto %s -> %s\n",v->Name(),u->Name() );
  return iiExport(v,0,IDPACKAGE((idhdl)u->data));
}
static BOOLEAN jjERROR(leftv, leftv u)
{
  WerrorS((char *)u->Data());
  extern int inerror;
  inerror=3;
  return TRUE;
}
static BOOLEAN jjEXTGCD_BI(leftv res, leftv u, leftv v)
{
  number uu=(number)u->Data();number vv=(number)v->Data();
  lists L=(lists)omAllocBin(slists_bin);
  number a,b;
  number p0=n_ExtGcd(uu,vv,&a,&b,coeffs_BIGINT);
  L->Init(3);
  L->m[0].rtyp=BIGINT_CMD;   L->m[0].data=(void *)p0;
  L->m[1].rtyp=BIGINT_CMD;   L->m[1].data=(void *)a;
  L->m[2].rtyp=BIGINT_CMD;   L->m[2].data=(void *)b;
  res->rtyp=LIST_CMD;
  res->data=(char *)L;
  return FALSE;
}
static BOOLEAN jjEXTGCD_I(leftv res, leftv u, leftv v)
{
  int uu=(int)(long)u->Data();int vv=(int)(long)v->Data();
  int p0=ABS(uu),p1=ABS(vv);
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
  if ( uu /*(int)(long)u->Data()*/ < 0 ) a=-a;
  if ( vv /*(int)(long)v->Data()*/ < 0 ) b=-b;
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(3);
  L->m[0].rtyp=INT_CMD;   L->m[0].data=(void *)(long)p0;
  L->m[1].rtyp=INT_CMD;   L->m[1].data=(void *)(long)a;
  L->m[2].rtyp=INT_CMD;   L->m[2].data=(void *)(long)b;
  res->rtyp=LIST_CMD;
  res->data=(char *)L;
  return FALSE;
}
static BOOLEAN jjEXTGCD_P(leftv res, leftv u, leftv v)
{
  poly r,pa,pb;
  BOOLEAN ret=singclap_extgcd((poly)u->Data(),(poly)v->Data(),r,pa,pb,currRing);
  if (ret) return TRUE;
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(3);
  res->data=(char *)L;
  L->m[0].data=(void *)r;
  L->m[0].rtyp=POLY_CMD;
  L->m[1].data=(void *)pa;
  L->m[1].rtyp=POLY_CMD;
  L->m[2].data=(void *)pb;
  L->m[2].rtyp=POLY_CMD;
  return FALSE;
}
extern int singclap_factorize_retry;
static BOOLEAN jjFAC_P2(leftv res, leftv u,leftv dummy)
{
  intvec *v=NULL;
  int sw=(int)(long)dummy->Data();
  int fac_sw=sw;
  if ((sw<0)||(sw>2)) fac_sw=1;
  singclap_factorize_retry=0;
  ideal f=singclap_factorize((poly)(u->CopyD()), &v, fac_sw,currRing);
  if (f==NULL)
    return TRUE;
  switch(sw)
  {
    case 0:
    case 2:
    {
      lists l=(lists)omAllocBin(slists_bin);
      l->Init(2);
      l->m[0].rtyp=IDEAL_CMD;
      l->m[0].data=(void *)f;
      l->m[1].rtyp=INTVEC_CMD;
      l->m[1].data=(void *)v;
      res->data=(void *)l;
      res->rtyp=LIST_CMD;
      return FALSE;
    }
    case 1:
      res->data=(void *)f;
      return FALSE;
    case 3:
      {
        poly p=f->m[0];
        int i=IDELEMS(f);
        f->m[0]=NULL;
        while(i>1)
        {
          i--;
          p=pMult(p,f->m[i]);
          f->m[i]=NULL;
        }
        res->data=(void *)p;
        res->rtyp=POLY_CMD;
      }
      return FALSE;
  }
  WerrorS("invalid switch");
  return TRUE;
}
static BOOLEAN jjFACSTD2(leftv res, leftv v, leftv w)
{
  ideal_list p,h;
  h=kStdfac((ideal)v->Data(),NULL,testHomog,NULL,(ideal)w->Data());
  p=h;
  int l=0;
  while (p!=NULL) { p=p->next;l++; }
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(l);
  l=0;
  while(h!=NULL)
  {
    L->m[l].data=(char *)h->d;
    L->m[l].rtyp=IDEAL_CMD;
    p=h->next;
    omFreeSize(h,sizeof(*h));
    h=p;
    l++;
  }
  res->data=(void *)L;
  return FALSE;
}
static BOOLEAN jjFAREY_BI(leftv res, leftv u, leftv v)
{
  if (rField_is_Q(currRing))
  {
    number uu=(number)u->Data();
    number vv=(number)v->Data();
    res->data=(char *)n_Farey(uu,vv,currRing->cf);
    return FALSE;
  }
  else return TRUE;
}
static BOOLEAN jjFAREY_ID(leftv res, leftv u, leftv v)
{
  ideal uu=(ideal)u->Data();
  number vv=(number)v->Data();
  res->data=(void*)id_Farey(uu,vv,currRing);
  res->rtyp=u->Typ();
  return FALSE;
}
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
    if ((nMap=n_SetMap(r->cf,currRing->cf))==NULL)
    {
      // Allow imap/fetch to be make an exception only for:
      if ( (rField_is_Q_a(r) &&  // Q(a..) -> Q(a..) || Q || Zp || Zp(a)
            (rField_is_Q(currRing) || rField_is_Q_a(currRing) ||
             (rField_is_Zp(currRing) || rField_is_Zp_a(currRing))))
           ||
           (rField_is_Zp_a(r) &&  // Zp(a..) -> Zp(a..) || Zp
            (rField_is_Zp(currRing, r->cf->ch) ||
             rField_is_Zp_a(currRing, r->cf->ch))) )
      {
        par_perm_size=rPar(r);
      }
      else
      {
        goto err_fetch;
      }
    }
    if ((iiOp!=FETCH_CMD) || (r->N!=currRing->N) || (rPar(r)!=rPar(currRing)))
    {
      perm=(int *)omAlloc0((r->N+1)*sizeof(int));
      if (par_perm_size!=0)
        par_perm=(int *)omAlloc0(par_perm_size*sizeof(int));
      op=IMAP_CMD;
      if (iiOp==IMAP_CMD)
      {
        int r_par=0;
        char ** r_par_names=NULL;
        if (r->cf->extRing!=NULL)
        {
          r_par=r->cf->extRing->N;
          r_par_names=r->cf->extRing->names;
        }
        int c_par=0;
        char ** c_par_names=NULL;
        if (currRing->cf->extRing!=NULL)
        {
          c_par=currRing->cf->extRing->N;
          c_par_names=currRing->cf->extRing->names;
        }
        maFindPerm(r->names,       r->N,       r_par_names, r_par,
                   currRing->names,currRing->N,c_par_names, c_par,
                   perm,par_perm, currRing->cf->type);
      }
      else
      {
        int i;
        if (par_perm_size!=0)
          for(i=si_min(rPar(r),rPar(currRing))-1;i>=0;i--) par_perm[i]=-(i+1);
        for(i=si_min(r->N,currRing->N);i>0;i--) perm[i]=i;
      }
    }
    if ((iiOp==FETCH_CMD) &&(BVERBOSE(V_IMAP)))
    {
      int i;
      for(i=0;i<si_min(r->N,currRing->N);i++)
      {
        Print("// var nr %d: %s -> %s\n",i,r->names[i],currRing->names[i]);
      }
      for(i=0;i<si_min(rPar(r),rPar(currRing));i++) // possibly empty loop
      {
        Print("// par nr %d: %s -> %s\n",
              i,rParameter(r)[i],rParameter(currRing)[i]);
      }
    }
    if (IDTYP(w)==ALIAS_CMD) w=(idhdl)IDDATA(w);
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
  Werror("no identity map from %s (%s -> %s)",u->Fullname(),
         nCoeffString(r->cf),
         nCoeffString(currRing->cf));
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
static BOOLEAN jjFWALK(leftv res, leftv u, leftv v)
{
  res->data=(char *)fractalWalkProc(u,v);
  setFlag( res, FLAG_STD );
  return FALSE;
}
static BOOLEAN jjGCD_I(leftv res, leftv u, leftv v)
{
  int uu=(int)(long)u->Data();int vv=(int)(long)v->Data();
  int p0=ABS(uu),p1=ABS(vv);
  int r;
  while ( p1!=0 )
  {
    r=p0 % p1;
    p0 = p1; p1 = r;
  }
  res->rtyp=INT_CMD;
  res->data=(char *)(long)p0;
  return FALSE;
}
static BOOLEAN jjGCD_BI(leftv res, leftv u, leftv v)
{
  number n1 = (number) u->Data();
  number n2 = (number) v->Data();
  res->data = n_Gcd(n1,n2,coeffs_BIGINT);
  return FALSE;
}
static BOOLEAN jjGCD_N(leftv res, leftv u, leftv v)
{
  number a=(number) u->Data();
  number b=(number) v->Data();
  if (nIsZero(a))
  {
    if (nIsZero(b)) res->data=(char *)nInit(1);
    else            res->data=(char *)nCopy(b);
  }
  else
  {
    if (nIsZero(b))  res->data=(char *)nCopy(a);
    //else res->data=(char *)n_Gcd(a, b, currRing->cf);
    else res->data=(char *)n_SubringGcd(a, b, currRing->cf);
  }
  return FALSE;
}
static BOOLEAN jjGCD_P(leftv res, leftv u, leftv v)
{
  res->data=(void *)singclap_gcd((poly)(u->CopyD(POLY_CMD)),
                                 (poly)(v->CopyD(POLY_CMD)),currRing);
  return FALSE;
}
static BOOLEAN jjHILBERT2(leftv res, leftv u, leftv v)
{
#ifdef HAVE_RINGS
  if (rField_is_Ring_Z(currRing))
  {
    ring origR = currRing;
    ring tempR = rCopy(origR);
    coeffs new_cf=nInitChar(n_Q,NULL);
    nKillChar(tempR->cf);
    tempR->cf=new_cf;
    rComplete(tempR);
    ideal uid = (ideal)u->Data();
    rChangeCurrRing(tempR);
    ideal uu = idrCopyR(uid, origR, currRing);
    sleftv uuAsLeftv; memset(&uuAsLeftv, 0, sizeof(uuAsLeftv));
    uuAsLeftv.rtyp = IDEAL_CMD;
    uuAsLeftv.data = uu; uuAsLeftv.next = NULL;
    if (hasFlag(u, FLAG_STD)) setFlag(&uuAsLeftv,FLAG_STD);
    assumeStdFlag(&uuAsLeftv);
    Print("// NOTE: computation of Hilbert series etc. is being\n");
    Print("//       performed for generic fibre, that is, over Q\n");
    intvec *module_w=(intvec*)atGet(&uuAsLeftv,"isHomog",INTVEC_CMD);
    intvec *iv=hFirstSeries(uu,module_w,currRing->qideal);
    int returnWithTrue = 1;
    switch((int)(long)v->Data())
    {
      case 1:
        res->data=(void *)iv;
        returnWithTrue = 0;
      case 2:
        res->data=(void *)hSecondSeries(iv);
        delete iv;
        returnWithTrue = 0;
    }
    if (returnWithTrue)
    {
      WerrorS(feNotImplemented);
      delete iv;
    }
    idDelete(&uu);
    rChangeCurrRing(origR);
    rDelete(tempR);
    if (returnWithTrue) return TRUE; else return FALSE;
  }
#endif
  assumeStdFlag(u);
  intvec *module_w=(intvec*)atGet(u,"isHomog",INTVEC_CMD);
  intvec *iv=hFirstSeries((ideal)u->Data(),module_w,currRing->qideal);
  switch((int)(long)v->Data())
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
  poly p=pOne(); pSetExp(p,i,1); pSetm(p);
  int d=pWTotaldegree(p);
  pLmDelete(p);
  if (d==1)
    res->data = (char *)p_Homogen((poly)u->Data(), i, currRing);
  else
    WerrorS("variable must have weight 1");
  return (d!=1);
}
static BOOLEAN jjHOMOG_ID(leftv res, leftv u, leftv v)
{
  int i=pVar((poly)v->Data());
  if (i==0)
  {
    WerrorS("ringvar expected");
    return TRUE;
  }
  pFDegProc deg;
  if (currRing->pLexOrder && (currRing->order[0]==ringorder_lp))
    deg=p_Totaldegree;
   else
    deg=currRing->pFDeg;
  poly p=pOne(); pSetExp(p,i,1); pSetm(p);
  int d=deg(p,currRing);
  pLmDelete(p);
  if (d==1)
    res->data = (char *)id_Homogen((ideal)u->Data(), i, currRing);
  else
    WerrorS("variable must have weight 1");
  return (d!=1);
}
static BOOLEAN jjHOMOG1_W(leftv res, leftv v, leftv u)
{
  intvec *w=new intvec(rVar(currRing));
  intvec *vw=(intvec*)u->Data();
  ideal v_id=(ideal)v->Data();
  pFDegProc save_FDeg=currRing->pFDeg;
  pLDegProc save_LDeg=currRing->pLDeg;
  BOOLEAN save_pLexOrder=currRing->pLexOrder;
  currRing->pLexOrder=FALSE;
  kHomW=vw;
  kModW=w;
  pSetDegProcs(currRing,kHomModDeg);
  res->data=(void *)(long)idHomModule(v_id,currRing->qideal,&w);
  currRing->pLexOrder=save_pLexOrder;
  kHomW=NULL;
  kModW=NULL;
  pRestoreDegProcs(currRing,save_FDeg,save_LDeg);
  if (w!=NULL) delete w;
  return FALSE;
}
static BOOLEAN jjINDEPSET2(leftv res, leftv u, leftv v)
{
  assumeStdFlag(u);
  res->data=(void *)scIndIndset((ideal)(u->Data()),(int)(long)(v->Data()),
                    currRing->qideal);
  return FALSE;
}
static BOOLEAN jjINTERSECT(leftv res, leftv u, leftv v)
{
  res->data=(char *)idSect((ideal)u->Data(),(ideal)v->Data());
  if (TEST_OPT_RETURN_SB) setFlag(res,FLAG_STD);
  return FALSE;
}
static BOOLEAN jjINTERPOLATION (leftv res, leftv l, leftv v)
{
  const lists L = (lists)l->Data();
  const int n = L->nr; assume (n >= 0);
  std::vector<ideal> V(n + 1);

  for(int i = n; i >= 0; i--) V[i] = (ideal)(L->m[i].Data());

  res->data=interpolation(V, (intvec*)v->Data());
  setFlag(res,FLAG_STD);
  return errorreported;
}
static BOOLEAN jjJanetBasis2(leftv res, leftv u, leftv v)
{
  extern BOOLEAN jjStdJanetBasis(leftv res, leftv v,int flag);
  return jjStdJanetBasis(res,u,(int)(long)v->Data());
}

static BOOLEAN jjJanetBasis(leftv res, leftv v)
{
  extern BOOLEAN jjStdJanetBasis(leftv res, leftv v,int flag);
  return jjStdJanetBasis(res,v,0);
}
static BOOLEAN jjJET_P(leftv res, leftv u, leftv v)
{
  res->data = (char *)pJet((poly)u->CopyD(), (int)(long)v->Data());
  return FALSE;
}
static BOOLEAN jjJET_ID(leftv res, leftv u, leftv v)
{
  res->data = (char *)id_Jet((ideal)u->Data(),(int)(long)v->Data(),currRing);
  return FALSE;
}
static BOOLEAN jjKBASE2(leftv res, leftv u, leftv v)
{
  assumeStdFlag(u);
  intvec *w_u=(intvec *)atGet(u,"isHomog",INTVEC_CMD);
  res->data = (char *)scKBase((int)(long)v->Data(),
                              (ideal)(u->Data()),currRing->qideal, w_u);
  if (w_u!=NULL)
  {
    atSet(res,omStrDup("isHomog"),ivCopy(w_u),INTVEC_CMD);
  }
  return FALSE;
}
static BOOLEAN jjPREIMAGE(leftv res, leftv u, leftv v, leftv w);
static BOOLEAN jjKERNEL(leftv res, leftv u, leftv v)
{
  return jjPREIMAGE(res,u,v,NULL);
}
static BOOLEAN jjKoszul(leftv res, leftv u, leftv v)
{
  return mpKoszul(res, u,v,NULL);
}
static BOOLEAN jjKoszul_Id(leftv res, leftv u, leftv v)
{
  sleftv h;
  memset(&h,0,sizeof(sleftv));
  h.rtyp=INT_CMD;
  h.data=(void *)(long)IDELEMS((ideal)v->Data());
  return mpKoszul(res, u, &h, v);
}
static BOOLEAN jjLIFT(leftv res, leftv u, leftv v)
{
  int ul= IDELEMS((ideal)u->Data());
  int vl= IDELEMS((ideal)v->Data());
  ideal m = idLift((ideal)u->Data(),(ideal)v->Data(),NULL,FALSE,
                   hasFlag(u,FLAG_STD));
  if (m==NULL) return TRUE;
  res->data = (char *)id_Module2formatedMatrix(m,ul,vl,currRing);
  return FALSE;
}
static BOOLEAN jjLIFTSTD(leftv res, leftv u, leftv v)
{
  if ((v->rtyp!=IDHDL)||(v->e!=NULL)) return TRUE;
  idhdl h=(idhdl)v->data;
  // CopyD for IDEAL_CMD and MODUL_CMD are identical:
  res->data = (char *)idLiftStd((ideal)u->Data(),
                                &(h->data.umatrix),testHomog);
  setFlag(res,FLAG_STD); v->flag=0;
  return FALSE;
}
static BOOLEAN jjLOAD2(leftv /*res*/, leftv, leftv v)
{
  return jjLOAD((char*)v->Data(),TRUE);
}
static BOOLEAN jjLOAD_E(leftv /*res*/, leftv v, leftv u)
{
  char * s=(char *)u->Data();
  if(strcmp(s, "with")==0)
    return jjLOAD((char*)v->Data(), TRUE);
  WerrorS("invalid second argument");
  WerrorS("load(\"libname\" [,\"with\"]);");
  return TRUE;
}
static BOOLEAN jjMODULO(leftv res, leftv u, leftv v)
{
  intvec *w_u=(intvec *)atGet(u,"isHomog",INTVEC_CMD);
  tHomog hom=testHomog;
  if (w_u!=NULL)
  {
    w_u=ivCopy(w_u);
    hom=isHomog;
  }
  intvec *w_v=(intvec *)atGet(v,"isHomog",INTVEC_CMD);
  if (w_v!=NULL)
  {
    w_v=ivCopy(w_v);
    hom=isHomog;
  }
  if ((w_u!=NULL) && (w_v==NULL))
    w_v=ivCopy(w_u);
  if ((w_v!=NULL) && (w_u==NULL))
    w_u=ivCopy(w_v);
  ideal u_id=(ideal)u->Data();
  ideal v_id=(ideal)v->Data();
  if (w_u!=NULL)
  {
     if ((*w_u).compare((w_v))!=0)
     {
       WarnS("incompatible weights");
       delete w_u; w_u=NULL;
       hom=testHomog;
     }
     else
     {
       if ((!idTestHomModule(u_id,currRing->qideal,w_v))
       || (!idTestHomModule(v_id,currRing->qideal,w_v)))
       {
         WarnS("wrong weights");
         delete w_u; w_u=NULL;
         hom=testHomog;
       }
     }
  }
  res->data = (char *)idModulo(u_id,v_id ,hom,&w_u);
  if (w_u!=NULL)
  {
    atSet(res,omStrDup("isHomog"),w_u,INTVEC_CMD);
  }
  delete w_v;
  if (TEST_OPT_RETURN_SB) setFlag(res,FLAG_STD);
  return FALSE;
}
static BOOLEAN jjMOD_BI(leftv res, leftv u, leftv v)
{
  number q=(number)v->Data();
  if (n_IsZero(q,coeffs_BIGINT))
  {
    WerrorS(ii_div_by_0);
    return TRUE;
  }
  res->data =(char *) n_IntMod((number)u->Data(),q,coeffs_BIGINT);
  return FALSE;
}
static BOOLEAN jjMOD_N(leftv res, leftv u, leftv v)
{
  number q=(number)v->Data();
  if (nIsZero(q))
  {
    WerrorS(ii_div_by_0);
    return TRUE;
  }
  res->data =(char *) n_IntMod((number)u->Data(),q,currRing->cf);
  return FALSE;
}
static BOOLEAN jjMONITOR2(leftv res, leftv u,leftv v);
static BOOLEAN jjMONITOR1(leftv res, leftv v)
{
  return jjMONITOR2(res,v,NULL);
}
static BOOLEAN jjMONITOR2(leftv, leftv u,leftv v)
{
#if 0
  char *opt=(char *)v->Data();
  int mode=0;
  while(*opt!='\0')
  {
    if (*opt=='i') mode |= SI_PROT_I;
    else if (*opt=='o') mode |= SI_PROT_O;
    opt++;
  }
  monitor((char *)(u->Data()),mode);
#else
  si_link l=(si_link)u->Data();
  if (slOpen(l,SI_LINK_WRITE,u)) return TRUE;
  if(strcmp(l->m->type,"ASCII")!=0)
  {
    Werror("ASCII link required, not `%s`",l->m->type);
    slClose(l);
    return TRUE;
  }
  SI_LINK_SET_CLOSE_P(l); // febase handles the FILE*
  if ( l->name[0]!='\0') // "" is the stop condition
  {
    const char *opt;
    int mode=0;
    if (v==NULL) opt=(const char*)"i";
    else         opt=(const char *)v->Data();
    while(*opt!='\0')
    {
      if (*opt=='i') mode |= SI_PROT_I;
      else if (*opt=='o') mode |= SI_PROT_O;
      opt++;
    }
    monitor((FILE *)l->data,mode);
  }
  else
    monitor(NULL,0);
  return FALSE;
#endif
}
static BOOLEAN jjMONOM(leftv res, leftv v)
{
  intvec *iv=(intvec *)v->Data();
  poly p=pOne();
  int i,e;
  BOOLEAN err=FALSE;
  for(i=si_min(currRing->N,iv->length()); i>0; i--)
  {
    e=(*iv)[i-1];
    if (e>=0) pSetExp(p,i,e);
    else err=TRUE;
  }
  if (iv->length()==(currRing->N+1))
  {
    res->rtyp=VECTOR_CMD;
    e=(*iv)[currRing->N];
    if (e>=0) pSetComp(p,e);
    else err=TRUE;
  }
  pSetm(p);
  res->data=(char*)p;
  if(err) { pDelete(&p); WerrorS("no negative exponent allowed"); }
  return err;
}
static BOOLEAN jjNEWSTRUCT2(leftv, leftv u, leftv v)
{
  // u: the name of the new type
  // v: the elements
  newstruct_desc d=newstructFromString((const char *)v->Data());
  if (d!=NULL) newstruct_setup((const char *)u->Data(),d);
  return d==NULL;
}
static BOOLEAN jjPARSTR2(leftv res, leftv u, leftv v)
{
  idhdl h=(idhdl)u->data;
  int i=(int)(long)v->Data();
  int p=0;
  if ((0<i)
  && (rParameter(IDRING(h))!=NULL)
  && (i<=(p=rPar(IDRING(h)))))
    res->data=omStrDup(rParameter(IDRING(h))[i-1]);
  else
  {
    Werror("par number %d out of range 1..%d",i,p);
    return TRUE;
  }
  return FALSE;
}
#ifdef HAVE_PLURAL
static BOOLEAN jjPlural_num_poly(leftv res, leftv a, leftv b)
{
  if( currRing->qideal != NULL )
  {
    WerrorS("basering must NOT be a qring!");
    return TRUE;
  }

  if (iiOp==NCALGEBRA_CMD)
  {
    return nc_CallPlural(NULL,NULL,(poly)a->Data(),(poly)b->Data(),currRing,false,true,false,currRing);
  }
  else
  {
    ring r=rCopy(currRing);
    BOOLEAN result=nc_CallPlural(NULL,NULL,(poly)a->Data(),(poly)b->Data(),r,false,true,false,currRing);
    res->data=r;
    if (r->qideal!=NULL) res->rtyp=QRING_CMD;
    return result;
  }
}
static BOOLEAN jjPlural_num_mat(leftv res, leftv a, leftv b)
{
  if( currRing->qideal != NULL )
  {
    WerrorS("basering must NOT be a qring!");
    return TRUE;
  }

  if (iiOp==NCALGEBRA_CMD)
  {
    return nc_CallPlural(NULL,(matrix)b->Data(),(poly)a->Data(),NULL,currRing,false,true,false,currRing);
  }
  else
  {
    ring r=rCopy(currRing);
    BOOLEAN result=nc_CallPlural(NULL,(matrix)b->Data(),(poly)a->Data(),NULL,r,false,true,false,currRing);
    res->data=r;
    if (r->qideal!=NULL) res->rtyp=QRING_CMD;
    return result;
  }
}
static BOOLEAN jjPlural_mat_poly(leftv res, leftv a, leftv b)
{
  if( currRing->qideal != NULL )
  {
    WerrorS("basering must NOT be a qring!");
    return TRUE;
  }

  if (iiOp==NCALGEBRA_CMD)
  {
    return nc_CallPlural((matrix)a->Data(),NULL,NULL,(poly)b->Data(),currRing,false,true,false,currRing);
  }
  else
  {
    ring r=rCopy(currRing);
    BOOLEAN result=nc_CallPlural((matrix)a->Data(),NULL,NULL,(poly)b->Data(),r,false,true,false,currRing);
    res->data=r;
    if (r->qideal!=NULL) res->rtyp=QRING_CMD;
    return result;
  }
}
static BOOLEAN jjPlural_mat_mat(leftv res, leftv a, leftv b)
{
  if( currRing->qideal != NULL )
  {
    WerrorS("basering must NOT be a qring!");
    return TRUE;
  }

  if (iiOp==NCALGEBRA_CMD)
  {
    return nc_CallPlural((matrix)a->Data(),(matrix)b->Data(),NULL,NULL,currRing,false,true,false,currRing);
  }
  else
  {
    ring r=rCopy(currRing);
    BOOLEAN result=nc_CallPlural((matrix)a->Data(),(matrix)b->Data(),NULL,NULL,r,false,true,false,currRing);
    res->data=r;
    if (r->qideal!=NULL) res->rtyp=QRING_CMD;
    return result;
  }
}
static BOOLEAN jjBRACKET(leftv res, leftv a, leftv b)
{
  res->data=NULL;

  if (rIsPluralRing(currRing))
  {
    const poly q = (poly)b->Data();

    if( q != NULL )
    {
      if( (poly)a->Data() != NULL )
      {
        poly p = (poly)a->CopyD(POLY_CMD); // p = copy!
        res->data = nc_p_Bracket_qq(p,q, currRing); // p will be destroyed!
      }
    }
  }
  return FALSE;
}
static BOOLEAN jjOPPOSE(leftv res, leftv a, leftv b)
{
  /* number, poly, vector, ideal, module, matrix */
  ring  r = (ring)a->Data();
  if (r == currRing)
  {
    res->data = b->Data();
    res->rtyp = b->rtyp;
    return FALSE;
  }
  if (!rIsLikeOpposite(currRing, r))
  {
    Werror("%s is not an opposite ring to current ring",a->Fullname());
    return TRUE;
  }
  idhdl w;
  if( ((w=r->idroot->get(b->Name(),myynest))!=NULL) && (b->e==NULL))
  {
    int argtype = IDTYP(w);
    switch (argtype)
    {
    case NUMBER_CMD:
      {
        /* since basefields are equal, we can apply nCopy */
        res->data = nCopy((number)IDDATA(w));
        res->rtyp = argtype;
        break;
      }
    case POLY_CMD:
    case VECTOR_CMD:
      {
        poly    q = (poly)IDDATA(w);
        res->data = pOppose(r,q,currRing);
        res->rtyp = argtype;
        break;
      }
    case IDEAL_CMD:
    case MODUL_CMD:
      {
        ideal   Q = (ideal)IDDATA(w);
        res->data = idOppose(r,Q,currRing);
        res->rtyp = argtype;
        break;
      }
    case MATRIX_CMD:
      {
        ring save = currRing;
        rChangeCurrRing(r);
        matrix  m = (matrix)IDDATA(w);
        ideal   Q = id_Matrix2Module(mp_Copy(m, currRing),currRing);
        rChangeCurrRing(save);
        ideal   S = idOppose(r,Q,currRing);
        id_Delete(&Q, r);
        res->data = id_Module2Matrix(S,currRing);
        res->rtyp = argtype;
        break;
      }
    default:
      {
        WerrorS("unsupported type in oppose");
        return TRUE;
      }
    }
  }
  else
  {
    Werror("identifier %s not found in %s",b->Fullname(),a->Fullname());
    return TRUE;
  }
  return FALSE;
}
#endif /* HAVE_PLURAL */

static BOOLEAN jjQUOT(leftv res, leftv u, leftv v)
{
  res->data = (char *)idQuot((ideal)u->Data(),(ideal)v->Data(),
    hasFlag(u,FLAG_STD),u->Typ()==v->Typ());
  id_DelMultiples((ideal)(res->data),currRing);
  if (TEST_OPT_RETURN_SB) setFlag(res,FLAG_STD);
  return FALSE;
}
static BOOLEAN jjRANDOM(leftv res, leftv u, leftv v)
{
  int i=(int)(long)u->Data();
  int j=(int)(long)v->Data();
  if (j-i <0) {WerrorS("invalid range for random"); return TRUE;}
  res->data =(char *)(long)((i > j) ? i : (siRand() % (j-i+1)) + i);
  return FALSE;
}
static BOOLEAN jjRANK2(leftv res, leftv u, leftv v)
{
  matrix m =(matrix)u->Data();
  int isRowEchelon = (int)(long)v->Data();
  if (isRowEchelon != 1) isRowEchelon = 0;
  int rank = luRank(m, isRowEchelon);
  res->data =(char *)(long)rank;
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
  res->data = (char *)kNF((ideal)v->Data(),currRing->qideal,(poly)u->Data());
  return FALSE;
}
static BOOLEAN jjREDUCE_ID(leftv res, leftv u, leftv v)
{
  assumeStdFlag(v);
  ideal ui=(ideal)u->Data();
  ideal vi=(ideal)v->Data();
  res->data = (char *)kNF(vi,currRing->qideal,ui);
  return FALSE;
}
#if 0
static BOOLEAN jjRES(leftv res, leftv u, leftv v)
{
  int maxl=(int)(long)v->Data();
  if (maxl<0)
  {
    WerrorS("length for res must not be negative");
    return TRUE;
  }
  int l=0;
  //resolvente r;
  syStrategy r;
  intvec *weights=NULL;
  int wmaxl=maxl;
  ideal u_id=(ideal)u->Data();

  maxl--;
  if (/*(*/ maxl==-1 /*)*/) /*&& (iiOp!=MRES_CMD)*/
  {
    maxl = currRing->N-1+2*(iiOp==MRES_CMD);
    if (currRing->qideal!=NULL)
    {
      Warn(
      "full resolution in a qring may be infinite, setting max length to %d",
      maxl+1);
    }
  }
  weights=(intvec*)atGet(u,"isHomog",INTVEC_CMD);
  if (weights!=NULL)
  {
    if (!idTestHomModule(u_id,currRing->qideal,weights))
    {
      WarnS("wrong weights given:");weights->show();PrintLn();
      weights=NULL;
    }
  }
  intvec *ww=NULL;
  int add_row_shift=0;
  if (weights!=NULL)
  {
     ww=ivCopy(weights);
     add_row_shift = ww->min_in();
     (*ww) -= add_row_shift;
  }
  else
    idHomModule(u_id,currRing->qideal,&ww);
  weights=ww;

  if ((iiOp == RES_CMD) || (iiOp == MRES_CMD))
  {
    r=syResolution(u_id,maxl, ww, iiOp==MRES_CMD);
  }
  else if (iiOp==SRES_CMD)
  //  r=sySchreyerResolvente(u_id,maxl+1,&l);
    r=sySchreyer(u_id,maxl+1);
  else if (iiOp == LRES_CMD)
  {
    int dummy;
    if((currRing->qideal!=NULL)||
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
    if((currRing->qideal!=NULL)||
    (!idHomIdeal (u_id,NULL)))
    {
       WerrorS
       ("`kres` not implemented for inhomogeneous input or qring");
       return TRUE;
    }
    r=syKosz(u_id,&dummy);
  }
  else
  {
    int dummy;
    if((currRing->qideal!=NULL)||
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
  if ((r->weights!=NULL) && (r->weights[0]!=NULL))
  {
    intvec *w=ivCopy(r->weights[0]);
    if (weights!=NULL) (*w) += add_row_shift;
    atSet(res,omStrDup("isHomog"),w,INTVEC_CMD);
    w=NULL;
  }
  else
  {
//#if 0
// need to set weights for ALL components (sres)
    if (weights!=NULL)
    {
      atSet(res,omStrDup("isHomog"),ivCopy(weights),INTVEC_CMD);
      r->weights = (intvec**)omAlloc0Bin(char_ptr_bin);
      (r->weights)[0] = ivCopy(weights);
    }
//#endif
  }
  if (ww!=NULL) { delete ww; ww=NULL; }
  return FALSE;
}
#else
static BOOLEAN jjRES(leftv res, leftv u, leftv v)
{
  int maxl=(int)(long)v->Data();
  if (maxl<0)
  {
    WerrorS("length for res must not be negative");
    return TRUE;
  }
  syStrategy r;
  intvec *weights=NULL;
  int wmaxl=maxl;
  ideal u_id=(ideal)u->Data();

  maxl--;
  if (/*(*/ maxl==-1 /*)*/) /*&& (iiOp!=MRES_CMD)*/
  {
    maxl = currRing->N-1+2*(iiOp==MRES_CMD);
    if (currRing->qideal!=NULL)
    {
      Warn(
      "full resolution in a qring may be infinite, setting max length to %d",
      maxl+1);
    }
  }
  weights=(intvec*)atGet(u,"isHomog",INTVEC_CMD);
  if (weights!=NULL)
  {
    if (!idTestHomModule(u_id,currRing->qideal,weights))
    {
      WarnS("wrong weights given:");weights->show();PrintLn();
      weights=NULL;
    }
  }
  intvec *ww=NULL;
  int add_row_shift=0;
  if (weights!=NULL)
  {
     ww=ivCopy(weights);
     add_row_shift = ww->min_in();
     (*ww) -= add_row_shift;
  }
  if ((iiOp == RES_CMD) || (iiOp == MRES_CMD))
  {
    r=syResolution(u_id,maxl, ww, iiOp==MRES_CMD);
  }
  else if (iiOp==SRES_CMD)
  //  r=sySchreyerResolvente(u_id,maxl+1,&l);
    r=sySchreyer(u_id,maxl+1);
  else if (iiOp == LRES_CMD)
  {
    int dummy;
    if((currRing->qideal!=NULL)||
    (!idHomIdeal (u_id,NULL)))
    {
       WerrorS
       ("`lres` not implemented for inhomogeneous input or qring");
       return TRUE;
    }
    if(currRing->N == 1)
      WarnS("the current implementation of `lres` may not work in the case of a single variable");
    r=syLaScala3(u_id,&dummy);
  }
  else if (iiOp == KRES_CMD)
  {
    int dummy;
    if((currRing->qideal!=NULL)||
    (!idHomIdeal (u_id,NULL)))
    {
       WerrorS
       ("`kres` not implemented for inhomogeneous input or qring");
       return TRUE;
    }
    r=syKosz(u_id,&dummy);
  }
  else
  {
    int dummy;
    if((currRing->qideal!=NULL)||
    (!idHomIdeal (u_id,NULL)))
    {
       WerrorS
       ("`hres` not implemented for inhomogeneous input or qring");
       return TRUE;
    }
    ideal u_id_copy=idCopy(u_id);
    idSkipZeroes(u_id_copy);
    r=syHilb(u_id_copy,&dummy);
    idDelete(&u_id_copy);
  }
  if (r==NULL) return TRUE;
  //res->data=(void *)liMakeResolv(r,l,wmaxl,u->Typ(),weights);
  r->list_length=wmaxl;
  res->data=(void *)r;
  if ((weights!=NULL) && (ww!=NULL)) { delete ww; ww=NULL; }
  if ((r->weights!=NULL) && (r->weights[0]!=NULL))
  {
    ww=ivCopy(r->weights[0]);
    if (weights!=NULL) (*ww) += add_row_shift;
    atSet(res,omStrDup("isHomog"),ww,INTVEC_CMD);
  }
  else
  {
    if (weights!=NULL)
    {
      atSet(res,omStrDup("isHomog"),ivCopy(weights),INTVEC_CMD);
    }
  }

  // test the La Scala case' output
  assume( ((iiOp == LRES_CMD) || (iiOp == HRES_CMD)) == (r->syRing != NULL) );
  assume( (r->syRing != NULL) == (r->resPairs != NULL) );

  if(iiOp != HRES_CMD)
    assume( (r->minres != NULL) || (r->fullres != NULL) ); // is wrong for HRES_CMD...
  else
    assume( (r->orderedRes != NULL) || (r->res != NULL) ); // analog for hres...

  return FALSE;
}
#endif
static BOOLEAN jjPFAC2(leftv res, leftv u, leftv v)
{
  number n1; int i;

  if ((u->Typ() == BIGINT_CMD) ||
     ((u->Typ() == NUMBER_CMD) && rField_is_Q(currRing)))
  {
    n1 = (number)u->CopyD();
  }
  else if (u->Typ() == INT_CMD)
  {
    i = (int)(long)u->Data();
    n1 = n_Init(i, coeffs_BIGINT);
  }
  else
  {
    return TRUE;
  }

  i = (int)(long)v->Data();

  lists l = primeFactorisation(n1, i);
  n_Delete(&n1, coeffs_BIGINT);
  res->data = (char*)l;
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
  int sw = (int)(long)v->Data();
  // CopyD for IDEAL_CMD and MODUL_CMD are identical:
  ideal id = (ideal)u->CopyD(IDEAL_CMD);
  if (sw & SIMPL_LMDIV)
  {
    id_DelDiv(id,currRing);
  }
  if (sw & SIMPL_LMEQ)
  {
    id_DelLmEquals(id,currRing);
  }
  if (sw & SIMPL_MULT)
  {
    id_DelMultiples(id,currRing);
  }
  else if(sw & SIMPL_EQU)
  {
    id_DelEquals(id,currRing);
  }
  if (sw & SIMPL_NULL)
  {
    idSkipZeroes(id);
  }
  if (sw & SIMPL_NORM)
  {
    id_Norm(id,currRing);
  }
  res->data = (char * )id;
  return FALSE;
}
extern int singclap_factorize_retry;
static BOOLEAN jjSQR_FREE2(leftv res, leftv u, leftv dummy)
{
  intvec *v=NULL;
  int sw=(int)(long)dummy->Data();
  int fac_sw=sw;
  if (sw<0) fac_sw=1;
  singclap_factorize_retry=0;
  ideal f=singclap_sqrfree((poly)(u->CopyD()), &v, fac_sw, currRing);
  if (f==NULL)
    return TRUE;
  switch(sw)
  {
    case 0:
    case 2:
    {
      lists l=(lists)omAllocBin(slists_bin);
      l->Init(2);
      l->m[0].rtyp=IDEAL_CMD;
      l->m[0].data=(void *)f;
      l->m[1].rtyp=INTVEC_CMD;
      l->m[1].data=(void *)v;
      res->data=(void *)l;
      res->rtyp=LIST_CMD;
      return FALSE;
    }
    case 1:
      res->data=(void *)f;
      return FALSE;
    case 3:
      {
        poly p=f->m[0];
        int i=IDELEMS(f);
        f->m[0]=NULL;
        while(i>1)
        {
          i--;
          p=pMult(p,f->m[i]);
          f->m[i]=NULL;
        }
        res->data=(void *)p;
        res->rtyp=POLY_CMD;
      }
      return FALSE;
  }
  WerrorS("invalid switch");
  return FALSE;
}
static BOOLEAN jjSTATUS2(leftv res, leftv u, leftv v)
{
  res->data = omStrDup(slStatus((si_link) u->Data(), (char *) v->Data()));
  return FALSE;
}
static BOOLEAN jjSTATUS2L(leftv res, leftv u, leftv v)
{
  res->data = (void *)(long)slStatusSsiL((lists) u->Data(), (int)(long) v->Data());
  //return (res->data== (void*)(long)-2);
  return FALSE;
}
static BOOLEAN jjSIMPL_P(leftv res, leftv u, leftv v)
{
  int sw = (int)(long)v->Data();
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
  intvec *w=(intvec *)atGet(u,"isHomog",INTVEC_CMD);
  tHomog hom=testHomog;
  ideal u_id=(ideal)(u->Data());
  if (w!=NULL)
  {
    if (!idTestHomModule(u_id,currRing->qideal,w))
    {
      WarnS("wrong weights:");w->show();PrintLn();
      w=NULL;
    }
    else
    {
      w=ivCopy(w);
      hom=isHomog;
    }
  }
  result=kStd(u_id,currRing->qideal,hom,&w,(intvec *)v->Data());
  idSkipZeroes(result);
  res->data = (char *)result;
  setFlag(res,FLAG_STD);
  if (w!=NULL) atSet(res,omStrDup("isHomog"),w,INTVEC_CMD);
  return FALSE;
}
static BOOLEAN jjSTD_1(leftv res, leftv u, leftv v);
static void jjSTD_1_ID(leftv res, ideal i0, int t0, ideal p0, attr a)
/* destroys i0, p0 */
/* result (with attributes) in res */
/* i0: SB*/
/* t0: type of p0*/
/* p0 new elements*/
/* a attributes of i0*/
{
  int tp;
  if (t0==IDEAL_CMD) tp=POLY_CMD;
  else               tp=VECTOR_CMD;
  for (int i=IDELEMS(p0)-1; i>=0; i--)
  {
    poly p=p0->m[i];
    p0->m[i]=NULL;
    if (p!=NULL)
    {
      sleftv u0,v0;
      memset(&u0,0,sizeof(sleftv));
      memset(&v0,0,sizeof(sleftv));
      v0.rtyp=tp;
      v0.data=(void*)p;
      u0.rtyp=t0;
      u0.data=i0;
      u0.attribute=a;
      setFlag(&u0,FLAG_STD);
      jjSTD_1(res,&u0,&v0);
      i0=(ideal)res->data;
      res->data=NULL;
      a=res->attribute;
      res->attribute=NULL;
      u0.CleanUp();
      v0.CleanUp();
      res->CleanUp();
    }
  }
  idDelete(&p0);
  res->attribute=a;
  res->data=(void *)i0;
  res->rtyp=t0;
}
static BOOLEAN jjSTD_1(leftv res, leftv u, leftv v)
{
  ideal result;
  assumeStdFlag(u);
  ideal i1=(ideal)(u->Data());
  ideal i0;
  int r=v->Typ();
  if ((/*v->Typ()*/r==POLY_CMD) ||(r==VECTOR_CMD))
  {
    i0=idInit(1,i1->rank); // TODO: rank is wrong (if v is a vector!)
    i0->m[0]=(poly)v->Data();
    int ii0=idElem(i0); /* size of i0 */
    i1=idSimpleAdd(i1,i0); //
    memset(i0->m,0,sizeof(poly)*IDELEMS(i0));
    idDelete(&i0);
    intvec *w=(intvec *)atGet(u,"isHomog",INTVEC_CMD);
    tHomog hom=testHomog;

    if (w!=NULL)
    {
      if (!idTestHomModule(i1,currRing->qideal,w))
      {
        // no warnung: this is legal, if i in std(i,p)
        // is homogeneous, but p not
        w=NULL;
      }
      else
      {
        w=ivCopy(w);
        hom=isHomog;
      }
    }
    BITSET save1;
    SI_SAVE_OPT1(save1);
    si_opt_1|=Sy_bit(OPT_SB_1);
    /* ii0 appears to be the position of the first element of il that
       does not belong to the old SB ideal */
    result=kStd(i1,currRing->qideal,hom,&w,NULL,0,ii0);
    SI_RESTORE_OPT1(save1);
    idDelete(&i1);
    idSkipZeroes(result);
    if (w!=NULL) atSet(res,omStrDup("isHomog"),w,INTVEC_CMD);
    res->data = (char *)result;
  }
  else /*IDEAL/MODULE*/
  {
    attr *aa=u->Attribute();
    attr a=NULL;
    if ((aa!=NULL)&&(*aa!=NULL)) a=(*aa)->Copy();
    jjSTD_1_ID(res,(ideal)u->CopyD(),r,(ideal)v->CopyD(),a);
  }
  if(!TEST_OPT_DEGBOUND) setFlag(res,FLAG_STD);
  return FALSE;
}
static BOOLEAN jjVARSTR2(leftv res, leftv u, leftv v)
{
  idhdl h=(idhdl)u->data;
  int i=(int)(long)v->Data();
  if ((0<i) && (i<=IDRING(h)->N))
    res->data=omStrDup(IDRING(h)->names[i-1]);
  else
  {
    Werror("var number %d out of range 1..%d",i,IDRING(h)->N);
    return TRUE;
  }
  return FALSE;
}
static BOOLEAN jjWAIT1ST2(leftv res, leftv u, leftv v)
{
// input: u: a list with links of type
//           ssi-fork, ssi-tcp, MPtcp-fork or MPtcp-launch
//        v: timeout for select in milliseconds
//           or 0 for polling
// returns: ERROR (via Werror): timeout negative
//           -1: the read state of all links is eof
//            0: timeout (or polling): none ready
//           i>0: (at least) L[i] is ready
  lists Lforks = (lists)u->Data();
  int t = (int)(long)v->Data();
  if(t < 0)
  {
    WerrorS("negative timeout"); return TRUE;
  }
  int i = slStatusSsiL(Lforks, t*1000);
  if(i == -2) /* error */
  {
    return TRUE;
  }
  res->data = (void*)(long)i;
  return FALSE;
}
static BOOLEAN jjWAITALL2(leftv res, leftv u, leftv v)
{
// input: u: a list with links of type
//           ssi-fork, ssi-tcp, MPtcp-fork or MPtcp-launch
//        v: timeout for select in milliseconds
//           or 0 for polling
// returns: ERROR (via Werror): timeout negative
//           -1: the read state of all links is eof
//           0: timeout (or polling): none ready
//           1: all links are ready
//              (caution: at least one is ready, but some maybe dead)
  lists Lforks = (lists)u->CopyD();
  int timeout = 1000*(int)(long)v->Data();
  if(timeout < 0)
  {
    WerrorS("negative timeout"); return TRUE;
  }
  int t = getRTimer()/TIMER_RESOLUTION;  // in seconds
  int i;
  int ret = -1;
  for(int nfinished = 0; nfinished < Lforks->nr+1; nfinished++)
  {
    i = slStatusSsiL(Lforks, timeout);
    if(i > 0) /* Lforks[i] is ready */
    {
      ret = 1;
      Lforks->m[i-1].CleanUp();
      Lforks->m[i-1].rtyp=DEF_CMD;
      Lforks->m[i-1].data=NULL;
      timeout = si_max(0,timeout - 1000*(getRTimer()/TIMER_RESOLUTION - t));
    }
    else /* terminate the for loop */
    {
      if(i == -2) /* error */
      {
        return TRUE;
      }
      if(i == 0) /* timeout */
      {
        ret = 0;
      }
      break;
    }
  }
  Lforks->Clean();
  res->data = (void*)(long)ret;
  return FALSE;
}
static BOOLEAN jjWEDGE(leftv res, leftv u, leftv v)
{
  res->data = (char *)mp_Wedge((matrix)u->Data(),(int)(long)v->Data(),currRing);
  return FALSE;
}
#define jjWRONG2 (proc2)jjWRONG
#define jjWRONG3 (proc3)jjWRONG
static BOOLEAN jjWRONG(leftv, leftv)
{
  return TRUE;
}

/*=================== operations with 1 arg.: static proc =================*/
/* must be ordered: first operations for chars (infix ops),
 * then alphabetically */

static BOOLEAN jjDUMMY(leftv res, leftv u)
{
  res->data = (char *)u->CopyD();
  return FALSE;
}
static BOOLEAN jjNULL(leftv, leftv)
{
  return FALSE;
}
//static BOOLEAN jjPLUSPLUS(leftv res, leftv u)
//{
//  res->data = (char *)((int)(long)u->Data()+1);
//  return FALSE;
//}
//static BOOLEAN jjMINUSMINUS(leftv res, leftv u)
//{
//  res->data = (char *)((int)(long)u->Data()-1);
//  return FALSE;
//}
static BOOLEAN jjPLUSPLUS(leftv, leftv u)
{
  if (IDTYP((idhdl)u->data)==INT_CMD)
  {
    int i=IDINT((idhdl)u->data);
    if (iiOp==PLUSPLUS) i++;
    else                i--;
    IDDATA((idhdl)u->data)=(char *)(long)i;
    return FALSE;
  }
  return TRUE;
}
static BOOLEAN jjUMINUS_BI(leftv res, leftv u)
{
  number n=(number)u->CopyD(BIGINT_CMD);
  n=n_InpNeg(n,coeffs_BIGINT);
  res->data = (char *)n;
  return FALSE;
}
static BOOLEAN jjUMINUS_I(leftv res, leftv u)
{
  res->data = (char *)(-(long)u->Data());
  return FALSE;
}
static BOOLEAN jjUMINUS_N(leftv res, leftv u)
{
  number n=(number)u->CopyD(NUMBER_CMD);
  n=nInpNeg(n);
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
  res->data = (char *)mp_MultP((matrix)u->CopyD(MATRIX_CMD),m1,currRing);
  return FALSE;
}
static BOOLEAN jjUMINUS_IV(leftv res, leftv u)
{
  intvec *iv=(intvec *)u->CopyD(INTVEC_CMD);
  (*iv)*=(-1);
  res->data = (char *)iv;
  return FALSE;
}
static BOOLEAN jjUMINUS_BIM(leftv res, leftv u)
{
  bigintmat *bim=(bigintmat *)u->CopyD(BIGINTMAT_CMD);
  (*bim)*=(-1);
  res->data = (char *)bim;
  return FALSE;
}
static BOOLEAN jjPROC1(leftv res, leftv u)
{
  return jjPROC(res,u,NULL);
}
static BOOLEAN jjBAREISS(leftv res, leftv v)
{
  //matrix m=(matrix)v->Data();
  //lists l=mpBareiss(m,FALSE);
  intvec *iv;
  ideal m;
  sm_CallBareiss((ideal)v->Data(),0,0,m,&iv, currRing);
  lists l=(lists)omAllocBin(slists_bin);
  l->Init(2);
  l->m[0].rtyp=MODUL_CMD;
  l->m[1].rtyp=INTVEC_CMD;
  l->m[0].data=(void *)m;
  l->m[1].data=(void *)iv;
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
static BOOLEAN jjBI2N(leftv res, leftv u)
{
  BOOLEAN bo=FALSE;
  number n=(number)u->CopyD();
  nMapFunc nMap=n_SetMap(coeffs_BIGINT,currRing->cf);
  if (nMap!=NULL)
    res->data=nMap(n,coeffs_BIGINT,currRing->cf);
  else
  {
    Werror("cannot convert bigint to cring %s", nCoeffString(currRing->cf));
    bo=TRUE;
  }
  n_Delete(&n,coeffs_BIGINT);
  return bo;
}
static BOOLEAN jjBI2P(leftv res, leftv u)
{
  sleftv tmp;
  BOOLEAN bo=jjBI2N(&tmp,u);
  if (!bo)
  {
    number n=(number) tmp.data;
    if (nIsZero(n)) { res->data=NULL;nDelete(&n); }
    else
    {
      res->data=(void *)pNSet(n);
    }
  }
  return bo;
}
static BOOLEAN jjCALL1MANY(leftv res, leftv u)
{
  return iiExprArithM(res,u,iiOp);
}
static BOOLEAN jjCHAR(leftv res, leftv v)
{
  res->data = (char *)(long)rChar((ring)v->Data());
  return FALSE;
}
static BOOLEAN jjCOLS(leftv res, leftv v)
{
  res->data = (char *)(long)MATCOLS((matrix)(v->Data()));
  return FALSE;
}
static BOOLEAN jjCOLS_BIM(leftv res, leftv v)
{
  res->data = (char *)(long)((bigintmat*)(v->Data()))->cols();
  return FALSE;
}
static BOOLEAN jjCOLS_IV(leftv res, leftv v)
{
  res->data = (char *)(long)((intvec*)(v->Data()))->cols();
  return FALSE;
}
static BOOLEAN jjCONTENT(leftv res, leftv v)
{
  // CopyD for POLY_CMD and VECTOR_CMD are identical:
  poly p=(poly)v->CopyD(POLY_CMD);
  if (p!=NULL) p_Cleardenom(p, currRing);
  res->data = (char *)p;
  return FALSE;
}
static BOOLEAN jjCOUNT_BI(leftv res, leftv v)
{
  res->data = (char *)(long)n_Size((number)v->Data(),coeffs_BIGINT);
  return FALSE;
}
static BOOLEAN jjCOUNT_N(leftv res, leftv v)
{
  res->data = (char *)(long)nSize((number)v->Data());
  return FALSE;
}
static BOOLEAN jjCOUNT_L(leftv res, leftv v)
{
  lists l=(lists)v->Data();
  res->data = (char *)(long)(lSize(l)+1);
  return FALSE;
}
static BOOLEAN jjCOUNT_M(leftv res, leftv v)
{
  matrix m=(matrix)v->Data();
  res->data = (char *)(long)(MATROWS(m)*MATCOLS(m));
  return FALSE;
}
static BOOLEAN jjCOUNT_IV(leftv res, leftv v)
{
  res->data = (char *)(long)((intvec*)(v->Data()))->length();
  return FALSE;
}
static BOOLEAN jjCOUNT_RG(leftv res, leftv v)
{
  ring r=(ring)v->Data();
  int elems=-1;
  if (rField_is_Zp(r)||rField_is_GF(r)) elems=r->cf->ch;
  else if (rField_is_Zp_a(r) && (r->cf->type==n_algExt))
  {
    extern int ipower ( int b, int n ); /* factory/cf_util */
    elems=ipower(r->cf->ch,r->cf->extRing->pFDeg(r->cf->extRing->qideal->m[0],r->cf->extRing));
  }
  res->data = (char *)(long)elems;
  return FALSE;
}
static BOOLEAN jjDEG(leftv res, leftv v)
{
  int dummy;
  poly p=(poly)v->Data();
  if (p!=NULL) res->data = (char *)currRing->pLDeg(p,&dummy,currRing);
  else res->data=(char *)-1;
  return FALSE;
}
static BOOLEAN jjDEG_M(leftv res, leftv u)
{
  ideal I=(ideal)u->Data();
  int d=-1;
  int dummy;
  int i;
  for(i=IDELEMS(I)-1;i>=0;i--)
    if (I->m[i]!=NULL) d=si_max(d,(int)currRing->pLDeg(I->m[i],&dummy,currRing));
  res->data = (char *)(long)d;
  return FALSE;
}
static BOOLEAN jjDEGREE(leftv res, leftv v)
{
  SPrintStart();
#ifdef HAVE_RINGS
  if (rField_is_Ring_Z(currRing))
  {
    ring origR = currRing;
    ring tempR = rCopy(origR);
    coeffs new_cf=nInitChar(n_Q,NULL);
    nKillChar(tempR->cf);
    tempR->cf=new_cf;
    rComplete(tempR);
    ideal vid = (ideal)v->Data();
    rChangeCurrRing(tempR);
    ideal vv = idrCopyR(vid, origR, currRing);
    sleftv vvAsLeftv; memset(&vvAsLeftv, 0, sizeof(vvAsLeftv));
    vvAsLeftv.rtyp = IDEAL_CMD;
    vvAsLeftv.data = vv; vvAsLeftv.next = NULL;
    if (hasFlag(v, FLAG_STD)) setFlag(&vvAsLeftv,FLAG_STD);
    assumeStdFlag(&vvAsLeftv);
    Print("// NOTE: computation of degree is being performed for\n");
    Print("//       generic fibre, that is, over Q\n");
    intvec *module_w=(intvec*)atGet(&vvAsLeftv,"isHomog",INTVEC_CMD);
    scDegree(vv,module_w,currRing->qideal);
    idDelete(&vv);
    rChangeCurrRing(origR);
    rDelete(tempR);
  }
#endif
  assumeStdFlag(v);
  intvec *module_w=(intvec*)atGet(v,"isHomog",INTVEC_CMD);
  scDegree((ideal)v->Data(),module_w,currRing->qideal);
  char *s=SPrintEnd();
  int l=strlen(s)-1;
  s[l]='\0';
  res->data=(void*)s;
  return FALSE;
}
static BOOLEAN jjDEFINED(leftv res, leftv v)
{
  if ((v->rtyp==IDHDL)
  && ((myynest==IDLEV((idhdl)v->data))||(0==IDLEV((idhdl)v->data))))
  {
    res->data=(void *)(long)(IDLEV((idhdl)v->data)+1);
  }
  else if (v->rtyp!=0) res->data=(void *)(-1);
  return FALSE;
}

/// Return the denominator of the input number
/// NOTE: the input number is normalized as a side effect
static BOOLEAN jjDENOMINATOR(leftv res, leftv v)
{
  number n = reinterpret_cast<number>(v->Data());
  res->data = reinterpret_cast<void*>(n_GetDenom(n, currRing));
  return FALSE;
}

/// Return the numerator of the input number
/// NOTE: the input number is normalized as a side effect
static BOOLEAN jjNUMERATOR(leftv res, leftv v)
{
  number n = reinterpret_cast<number>(v->Data());
  res->data = reinterpret_cast<void*>(n_GetNumerator(n, currRing));
  return FALSE;
}

static BOOLEAN jjDET(leftv res, leftv v)
{
  matrix m=(matrix)v->Data();
  poly p;
  if (sm_CheckDet((ideal)m,m->cols(),TRUE, currRing))
  {
    ideal I=id_Matrix2Module(mp_Copy(m, currRing),currRing);
    p=sm_CallDet(I, currRing);
    idDelete(&I);
  }
  else
    p=singclap_det(m,currRing);
  res ->data = (char *)p;
  return FALSE;
}
static BOOLEAN jjDET_BI(leftv res, leftv v)
{
  bigintmat * m=(bigintmat*)v->Data();
  int i,j;
  i=m->rows();j=m->cols();
  if(i==j)
    res->data = (char *)(long)singclap_det_bi(m,coeffs_BIGINT);
  else
  {
    Werror("det of %d x %d bigintmat",i,j);
    return TRUE;
  }
  return FALSE;
}
static BOOLEAN jjDET_I(leftv res, leftv v)
{
  intvec * m=(intvec*)v->Data();
  int i,j;
  i=m->rows();j=m->cols();
  if(i==j)
    res->data = (char *)(long)singclap_det_i(m,currRing);
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
  if (IDELEMS(I)<1) return TRUE;
  if (sm_CheckDet(I,IDELEMS(I),FALSE, currRing))
  {
    matrix m=id_Module2Matrix(id_Copy(I,currRing),currRing);
    p=singclap_det(m,currRing);
    idDelete((ideal *)&m);
  }
  else
    p=sm_CallDet(I, currRing);
  res->data = (char *)p;
  return FALSE;
}
static BOOLEAN jjDIM(leftv res, leftv v)
{
  assumeStdFlag(v);
#ifdef HAVE_RINGS
  if (rField_is_Ring(currRing))
  {
    //ring origR = currRing;
    //ring tempR = rCopy(origR);
    //coeffs new_cf=nInitChar(n_Q,NULL);
    //nKillChar(tempR->cf);
    //tempR->cf=new_cf;
    //rComplete(tempR);
    ideal vid = (ideal)v->Data();
    int i = idPosConstant(vid);
    if ((i != -1) && (n_IsUnit(pGetCoeff(vid->m[i]),currRing->cf)))
    { /* ideal v contains unit; dim = -1 */
      res->data = (char *)-1;
      return FALSE;
    }
    //rChangeCurrRing(tempR);
    //ideal vv = idrCopyR(vid, origR, currRing);
    ideal vv = id_Head(vid,currRing);
    /* drop degree zero generator from vv (if any) */
    if (i != -1) pDelete(&vv->m[i]);
    long d = (long)scDimInt(vv, currRing->qideal);
    if (rField_is_Ring_Z(currRing) && (i == -1)) d++;
    res->data = (char *)d;
    idDelete(&vv);
    //rChangeCurrRing(origR);
    //rDelete(tempR);
    return FALSE;
  }
#endif
  res->data = (char *)(long)scDimInt((ideal)(v->Data()),currRing->qideal);
  return FALSE;
}
static BOOLEAN jjDUMP(leftv, leftv v)
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
  int co=(int)(long)v->Data();
  if (co>0)
  {
    pSetComp((poly)res->data,co);
    pSetm((poly)res->data);
  }
  else WerrorS("argument of gen must be positive");
  return (co<=0);
}
static BOOLEAN jjEXECUTE(leftv, leftv v)
{
  char * d = (char *)v->Data();
  char * s = (char *)omAlloc(strlen(d) + 13);
  strcpy( s, (char *)d);
  strcat( s, "\n;RETURN();\n");
  newBuffer(s,BT_execute);
  return yyparse();
}
static BOOLEAN jjFACSTD(leftv res, leftv v)
{
  lists L=(lists)omAllocBin(slists_bin);
  if (currRing->cf->convSingNFactoryN!=NULL) /* conversion to factory*/
  {
    ideal_list p,h;
    h=kStdfac((ideal)v->Data(),NULL,testHomog,NULL);
    if (h==NULL)
    {
      L->Init(1);
      L->m[0].data=(char *)idInit(1);
      L->m[0].rtyp=IDEAL_CMD;
    }
    else
    {
      p=h;
      int l=0;
      while (p!=NULL) { p=p->next;l++; }
      L->Init(l);
      l=0;
      while(h!=NULL)
      {
        L->m[l].data=(char *)h->d;
        L->m[l].rtyp=IDEAL_CMD;
        p=h->next;
        omFreeSize(h,sizeof(*h));
        h=p;
        l++;
      }
    }
  }
  else
  {
    WarnS("no factorization implemented");
    L->Init(1);
    iiExprArith1(&(L->m[0]),v,STD_CMD);
  }
  res->data=(void *)L;
  return FALSE;
}
static BOOLEAN jjFAC_P(leftv res, leftv u)
{
  intvec *v=NULL;
  singclap_factorize_retry=0;
  ideal f=singclap_factorize((poly)(u->CopyD()), &v, 0,currRing);
  if (f==NULL) return TRUE;
  ivTest(v);
  lists l=(lists)omAllocBin(slists_bin);
  l->Init(2);
  l->m[0].rtyp=IDEAL_CMD;
  l->m[0].data=(void *)f;
  l->m[1].rtyp=INTVEC_CMD;
  l->m[1].data=(void *)v;
  res->data=(void *)l;
  return FALSE;
}
static BOOLEAN jjGETDUMP(leftv, leftv v)
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
  intvec *w=(intvec*)atGet(v,"isHomog",INTVEC_CMD);
  BOOLEAN delete_w=FALSE;
  ideal I=(ideal)v->Data();
  int i;
  poly p=NULL,po=NULL;
  int rk=id_RankFreeModule(I,currRing);
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
      WerrorS("module must be zero-dimensional");
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
      int d=(currRing->pFDeg(po,currRing)-(*w)[pGetComp(po)-1] - currRing->pFDeg(p,currRing)+(*w)[i-1]);
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
static BOOLEAN jjHILBERT(leftv, leftv v)
{
#ifdef HAVE_RINGS
  if (rField_is_Ring_Z(currRing))
  {
    ring origR = currRing;
    ring tempR = rCopy(origR);
    coeffs new_cf=nInitChar(n_Q,NULL);
    nKillChar(tempR->cf);
    tempR->cf=new_cf;
    rComplete(tempR);
    ideal vid = (ideal)v->Data();
    rChangeCurrRing(tempR);
    ideal vv = idrCopyR(vid, origR, currRing);
    sleftv vvAsLeftv; memset(&vvAsLeftv, 0, sizeof(vvAsLeftv));
    vvAsLeftv.rtyp = IDEAL_CMD;
    vvAsLeftv.data = vv; vvAsLeftv.next = NULL;
    if (hasFlag(v, FLAG_STD)) setFlag(&vvAsLeftv,FLAG_STD);
    assumeStdFlag(&vvAsLeftv);
    Print("// NOTE: computation of Hilbert series etc. is being\n");
    Print("//       performed for generic fibre, that is, over Q\n");
    intvec *module_w=(intvec*)atGet(&vvAsLeftv,"isHomog",INTVEC_CMD);
    //scHilbertPoly(vv,currRing->qideal);
    hLookSeries(vv,module_w,currRing->qideal);
    idDelete(&vv);
    rChangeCurrRing(origR);
    rDelete(tempR);
    return FALSE;
  }
#endif
  assumeStdFlag(v);
  intvec *module_w=(intvec*)atGet(v,"isHomog",INTVEC_CMD);
  //scHilbertPoly((ideal)v->Data(),currRing->qideal);
  hLookSeries((ideal)v->Data(),module_w,currRing->qideal);
  return FALSE;
}
static BOOLEAN jjHILBERT_IV(leftv res, leftv v)
{
#ifdef HAVE_RINGS
  if (rField_is_Ring_Z(currRing))
  {
    Print("// NOTE: computation of Hilbert series etc. is being\n");
    Print("//       performed for generic fibre, that is, over Q\n");
  }
#endif
  res->data=(void *)hSecondSeries((intvec *)v->Data());
  return FALSE;
}
static BOOLEAN jjHOMOG1(leftv res, leftv v)
{
  intvec *w=(intvec*)atGet(v,"isHomog",INTVEC_CMD);
  ideal v_id=(ideal)v->Data();
  if (w==NULL)
  {
    res->data=(void *)(long)idHomModule(v_id,currRing->qideal,&w);
    if (res->data!=NULL)
    {
      if (v->rtyp==IDHDL)
      {
        char *s_isHomog=omStrDup("isHomog");
        if (v->e==NULL)
          atSet((idhdl)(v->data),s_isHomog,w,INTVEC_CMD);
        else
          atSet((idhdl)(v->LData()),s_isHomog,w,INTVEC_CMD);
      }
      else if (w!=NULL) delete w;
    } // if res->data==NULL then w==NULL
  }
  else
  {
    res->data=(void *)(long)idTestHomModule(v_id,currRing->qideal,w);
    if((res->data==NULL) && (v->rtyp==IDHDL))
    {
      if (v->e==NULL)
        atKill((idhdl)(v->data),"isHomog");
      else
        atKill((idhdl)(v->LData()),"isHomog");
    }
  }
  return FALSE;
}
static BOOLEAN jjidMaxIdeal(leftv res, leftv v)
{
  res->data = (char *)idMaxIdeal((int)(long)v->Data());
  setFlag(res,FLAG_STD);
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
    if (rSamePolyRep(currRing, q))
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
  res->data = (char *)n_ImPart((number)v->Data(),currRing->cf);
  return FALSE;
}
static BOOLEAN jjINDEPSET(leftv res, leftv v)
{
  assumeStdFlag(v);
  res->data=(void *)scIndIntvec((ideal)(v->Data()),currRing->qideal);
  return FALSE;
}
static BOOLEAN jjINTERRED(leftv res, leftv v)
{
  ideal result=kInterRed((ideal)(v->Data()), currRing->qideal);
  #ifdef HAVE_RINGS
  if(rField_is_Ring(currRing))
    Warn("interred: this command is experimental over the integers");
  #endif
  if (TEST_OPT_PROT) { PrintLn(); mflush(); }
  res->data = result;
  return FALSE;
}
static BOOLEAN jjIS_RINGVAR_P(leftv res, leftv v)
{
  res->data = (char *)(long)pVar((poly)v->Data());
  return FALSE;
}
static BOOLEAN jjIS_RINGVAR_S(leftv res, leftv v)
{
  res->data = (char *)(long)(r_IsRingVar((char *)v->Data(), currRing->names,
                                                            currRing->N)+1);
  return FALSE;
}
static BOOLEAN jjIS_RINGVAR0(leftv res, leftv)
{
  res->data = (char *)0;
  return FALSE;
}
static BOOLEAN jjJACOB_P(leftv res, leftv v)
{
  ideal i=idInit(currRing->N,1);
  int k;
  poly p=(poly)(v->Data());
  for (k=currRing->N;k>0;k--)
  {
    i->m[k-1]=pDiff(p,k);
  }
  res->data = (char *)i;
  return FALSE;
}
static BOOLEAN jjDIFF_COEF(leftv res, leftv u, leftv v)
{
  if (!nCoeff_is_transExt(currRing->cf))
  {
    WerrorS("differentiation not defined in the coefficient ring");
    return TRUE;
  }
  number n = (number) u->Data();
  number k = (number) v->Data();
  res->data = ntDiff(n,k,currRing->cf);
  return FALSE;
}
/*2
 * compute Jacobi matrix of a module/matrix
 * Jacobi(M) := ( diff(Mt,var(1))|, ... ,| diff(Mt,var(currRing->N))  ),
 * where Mt := transpose(M)
 * Note that this is consistent with the current conventions for jacob in Singular,
 * whereas M2 computes its transposed.
 */
static BOOLEAN jjJACOB_M(leftv res, leftv a)
{
  ideal id = (ideal)a->Data();
  id = idTransp(id);
  int W = IDELEMS(id);

  ideal result = idInit(W * currRing->N, id->rank);
  poly *p = result->m;

  for( int v = 1; v <= currRing->N; v++ )
  {
    poly* q = id->m;
    for( int i = 0; i < W; i++, p++, q++ )
      *p = pDiff( *q, v );
  }
  idDelete(&id);

  res->data = (char *)result;
  return FALSE;
}


static BOOLEAN jjKBASE(leftv res, leftv v)
{
  assumeStdFlag(v);
  res->data = (char *)scKBase(-1,(ideal)(v->Data()),currRing->qideal);
  return FALSE;
}
static BOOLEAN jjL2R(leftv res, leftv v)
{
  res->data=(char *)syConvList((lists)v->Data(),FALSE);
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
  int s=currRing->N;
  if (v->Typ()==VECTOR_CMD) s++;
  intvec *iv=new intvec(s);
  if (p!=NULL)
  {
    for(int i = currRing->N;i;i--)
    {
      (*iv)[i-1]=pGetExp(p,i);
    }
    if (s!=currRing->N)
      (*iv)[currRing->N]=pGetComp(p);
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
static BOOLEAN jjLOAD1(leftv /*res*/, leftv v)
{
  return jjLOAD((char*)v->Data(),FALSE);
}
static BOOLEAN jjLISTRING(leftv res, leftv v)
{
  ring r=rCompose((lists)v->Data());
  if (r==NULL) return TRUE;
  if (r->qideal!=NULL) res->rtyp=QRING_CMD;
  res->data=(char *)r;
  return FALSE;
}
static BOOLEAN jjPFAC1(leftv res, leftv v)
{
  /* call method jjPFAC2 with second argument = 0 (meaning that no
     valid bound for the prime factors has been given) */
  sleftv tmp;
  memset(&tmp, 0, sizeof(tmp));
  tmp.rtyp = INT_CMD;
  return jjPFAC2(res, v, &tmp);
}
static BOOLEAN jjLU_DECOMP(leftv res, leftv v)
{
  /* computes the LU-decomposition of a matrix M;
     i.e., M = P * L * U, where
        - P is a row permutation matrix,
        - L is in lower triangular form,
        - U is in upper row echelon form
     Then, we also have P * M = L * U.
     A list [P, L, U] is returned. */
  matrix mat = (const matrix)v->Data();
  if (!idIsConstant((ideal)mat))
  {
    WerrorS("matrix must be constant");
    return TRUE;
  }
  matrix pMat;
  matrix lMat;
  matrix uMat;

  luDecomp(mat, pMat, lMat, uMat);

  lists ll = (lists)omAllocBin(slists_bin);
  ll->Init(3);
  ll->m[0].rtyp=MATRIX_CMD; ll->m[0].data=(void *)pMat;
  ll->m[1].rtyp=MATRIX_CMD; ll->m[1].data=(void *)lMat;
  ll->m[2].rtyp=MATRIX_CMD; ll->m[2].data=(void *)uMat;
  res->data=(char*)ll;

  return FALSE;
}
static BOOLEAN jjMEMORY(leftv res, leftv v)
{
  omUpdateInfo();
  switch(((int)(long)v->Data()))
  {
  case 0:
    res->data=(char *)n_Init(om_Info.UsedBytes,coeffs_BIGINT);
    break;
  case 1:
    res->data = (char *)n_Init(om_Info.CurrentBytesSystem,coeffs_BIGINT);
    break;
  case 2:
    res->data = (char *)n_Init(om_Info.MaxBytesSystem,coeffs_BIGINT);
    break;
  default:
    omPrintStats(stdout);
    omPrintInfo(stdout);
    omPrintBinStats(stdout);
    res->data = (char *)0;
    res->rtyp = NONE;
  }
  return FALSE;
  res->data = (char *)0;
  return FALSE;
}
//static BOOLEAN jjMONITOR1(leftv res, leftv v)
//{
//  return jjMONITOR2(res,v,NULL);
//}
static BOOLEAN jjMSTD(leftv res, leftv v)
{
  int t=v->Typ();
  ideal r,m;
  r=kMin_std((ideal)v->Data(),currRing->qideal,testHomog,NULL,m);
  lists l=(lists)omAllocBin(slists_bin);
  l->Init(2);
  l->m[0].rtyp=t;
  l->m[0].data=(char *)r;
  setFlag(&(l->m[0]),FLAG_STD);
  l->m[1].rtyp=t;
  l->m[1].data=(char *)m;
  res->data=(char *)l;
  return FALSE;
}
static BOOLEAN jjMULT(leftv res, leftv v)
{
  assumeStdFlag(v);
  res->data = (char *)(long)scMultInt((ideal)(v->Data()),currRing->qideal);
  return FALSE;
}
static BOOLEAN jjMINRES_R(leftv res, leftv v)
{
  intvec *weights=(intvec*)atGet(v,"isHomog",INTVEC_CMD);

  syStrategy tmp=(syStrategy)v->Data();
  tmp = syMinimize(tmp); // enrich itself!

  res->data=(char *)tmp;

  if (weights!=NULL)
    atSet(res, omStrDup("isHomog"),ivCopy(weights),INTVEC_CMD);

  return FALSE;
}
static BOOLEAN jjN2BI(leftv res, leftv v)
{
  number n,i; i=(number)v->Data();
  nMapFunc nMap=n_SetMap(currRing->cf,coeffs_BIGINT);
  if (nMap!=NULL)
    n=nMap(i,currRing->cf,coeffs_BIGINT);
  else goto err;
  res->data=(void *)n;
  return FALSE;
err:
  WerrorS("cannot convert to bigint"); return TRUE;
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
static BOOLEAN jjNAMES_I(leftv res, leftv v)
{
  res->data=ipNameListLev((IDROOT),(int)(long)v->Data());
  return FALSE;
}
static BOOLEAN jjNOT(leftv res, leftv v)
{
  res->data=(char*)(long)((long)v->Data()==0 ? 1 : 0);
  return FALSE;
}
static BOOLEAN jjNVARS(leftv res, leftv v)
{
  res->data = (char *)(long)(((ring)(v->Data()))->N);
  return FALSE;
}
static BOOLEAN jjOpenClose(leftv, leftv v)
{
  si_link l=(si_link)v->Data();
  if (iiOp==OPEN_CMD) return slOpen(l, SI_LINK_OPEN,v);
  else                return slClose(l);
}
static BOOLEAN jjORD(leftv res, leftv v)
{
  poly p=(poly)v->Data();
  res->data=(char *)( p==NULL ? -1 : currRing->pFDeg(p,currRing) );
  return FALSE;
}
static BOOLEAN jjPAR1(leftv res, leftv v)
{
  int i=(int)(long)v->Data();
  int p=0;
  p=rPar(currRing);
  if ((0<i) && (i<=p))
  {
    res->data=(char *)n_Param(i,currRing);
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
  number nn=(number)v->Data();
  res->data = (char *)(long)n_ParDeg(nn, currRing);
  return FALSE;
}
static BOOLEAN jjPARSTR1(leftv res, leftv v)
{
  if (currRing==NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
  int i=(int)(long)v->Data();
  int p=0;
  if ((0<i) && (rParameter(currRing)!=NULL) && (i<=(p=rPar(currRing))))
    res->data=omStrDup(rParameter(currRing)[i-1]);
  else
  {
    Werror("par number %d out of range 1..%d",i,p);
    return TRUE;
  }
  return FALSE;
}
static BOOLEAN jjP2BI(leftv res, leftv v)
{
  poly p=(poly)v->Data();
  if (p==NULL) { res->data=(char *)n_Init(0,coeffs_BIGINT); return FALSE; }
  if ((pNext(p)!=NULL)|| (!pIsConstant(p)))
  {
    WerrorS("poly must be constant");
    return TRUE;
  }
  number i=pGetCoeff(p);
  number n;
  nMapFunc nMap=n_SetMap(currRing->cf,coeffs_BIGINT);
  if (nMap!=NULL)
    n=nMap(i,currRing->cf,coeffs_BIGINT);
  else goto err;
  res->data=(void *)n;
  return FALSE;
err:
  WerrorS("cannot convert to bigint"); return TRUE;
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
  res->data = (char *)(long)n_Int(pGetCoeff(p),currRing->cf);
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
  int i = IsPrime((int)(long)(v->Data()));
  res->data = (char *)(long)(i > 1 ? i : 2);
  return FALSE;
}
static BOOLEAN jjPRUNE(leftv res, leftv v)
{
  intvec *w=(intvec *)atGet(v,"isHomog",INTVEC_CMD);
  ideal v_id=(ideal)v->Data();
  if (w!=NULL)
  {
    if (!idTestHomModule(v_id,currRing->qideal,w))
    {
      WarnS("wrong weights");
      w=NULL;
      // and continue at the non-homog case below
    }
    else
    {
      w=ivCopy(w);
      intvec **ww=&w;
      res->data = (char *)idMinEmbedding(v_id,FALSE,ww);
      atSet(res,omStrDup("isHomog"),*ww,INTVEC_CMD);
      return FALSE;
    }
  }
  res->data = (char *)idMinEmbedding(v_id);
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
  for(i=0; i<sArithBase.nCmdUsed; i++)
  {
    //Print("test %d, >>%s<<, tab:>>%s<<\n",i,s,sArithBase.sCmds[i].name);
    if (strcmp(s, sArithBase.sCmds[i].name) == 0)
    {
      res->data = (char *)1;
      return FALSE;
    }
  }
  //res->data = (char *)0;
  return FALSE;
}
static BOOLEAN jjRANK1(leftv res, leftv v)
{
  matrix m =(matrix)v->Data();
  int rank = luRank(m, 0);
  res->data =(char *)(long)rank;
  return FALSE;
}
static BOOLEAN jjREAD(leftv res, leftv v)
{
  return jjREAD2(res,v,NULL);
}
static BOOLEAN jjREGULARITY(leftv res, leftv v)
{
  res->data = (char *)(long)iiRegularity((lists)v->Data());
  return FALSE;
}
static BOOLEAN jjREPART(leftv res, leftv v)
{
  res->data = (char *)n_RePart((number)v->Data(),currRing->cf);
  return FALSE;
}
static BOOLEAN jjRINGLIST(leftv res, leftv v)
{
  ring r=(ring)v->Data();
  if (r!=NULL)
    res->data = (char *)rDecompose((ring)v->Data());
  return (r==NULL)||(res->data==NULL);
}
static BOOLEAN jjROWS(leftv res, leftv v)
{
  ideal i = (ideal)v->Data();
  res->data = (char *)i->rank;
  return FALSE;
}
static BOOLEAN jjROWS_BIM(leftv res, leftv v)
{
  res->data = (char *)(long)((bigintmat*)(v->Data()))->rows();
  return FALSE;
}
static BOOLEAN jjROWS_IV(leftv res, leftv v)
{
  res->data = (char *)(long)((intvec*)(v->Data()))->rows();
  return FALSE;
}
static BOOLEAN jjRPAR(leftv res, leftv v)
{
  res->data = (char *)(long)rPar(((ring)v->Data()));
  return FALSE;
}
static BOOLEAN jjSLIM_GB(leftv res, leftv u)
{
#ifdef HAVE_PLURAL
  const bool bIsSCA = rIsSCA(currRing);
#else
  const bool bIsSCA = false;
#endif

  if ((currRing->qideal!=NULL) && !bIsSCA)
  {
    WerrorS("qring not supported by slimgb at the moment");
    return TRUE;
  }
  if (rHasLocalOrMixedOrdering_currRing())
  {
    WerrorS("ordering must be global for slimgb");
    return TRUE;
  }
  intvec *w=(intvec *)atGet(u,"isHomog",INTVEC_CMD);
  // tHomog hom=testHomog;
  ideal u_id=(ideal)u->Data();
  if (w!=NULL)
  {
    if (!idTestHomModule(u_id,currRing->qideal,w))
    {
      WarnS("wrong weights");
      w=NULL;
    }
    else
    {
      w=ivCopy(w);
      // hom=isHomog;
    }
  }

  assume(u_id->rank>=id_RankFreeModule(u_id, currRing));
  res->data=(char *)t_rep_gb(currRing,
    u_id,u_id->rank);
  //res->data=(char *)t_rep_gb(currRing, u_id);

  if(!TEST_OPT_DEGBOUND) setFlag(res,FLAG_STD);
  if (w!=NULL) atSet(res,omStrDup("isHomog"),w,INTVEC_CMD);
  return FALSE;
}
static BOOLEAN jjSBA(leftv res, leftv v)
{
  ideal result;
  ideal v_id=(ideal)v->Data();
  intvec *w=(intvec *)atGet(v,"isHomog",INTVEC_CMD);
  tHomog hom=testHomog;
  if (w!=NULL)
  {
    if (!idTestHomModule(v_id,currRing->qideal,w))
    {
      WarnS("wrong weights");
      w=NULL;
    }
    else
    {
      hom=isHomog;
      w=ivCopy(w);
    }
  }
  result=kSba(v_id,currRing->qideal,hom,&w,1,0);
  idSkipZeroes(result);
  res->data = (char *)result;
  if(!TEST_OPT_DEGBOUND) setFlag(res,FLAG_STD);
  if (w!=NULL) atSet(res,omStrDup("isHomog"),w,INTVEC_CMD);
  return FALSE;
}
static BOOLEAN jjSBA_1(leftv res, leftv v, leftv u)
{
  ideal result;
  ideal v_id=(ideal)v->Data();
  intvec *w=(intvec *)atGet(v,"isHomog",INTVEC_CMD);
  tHomog hom=testHomog;
  if (w!=NULL)
  {
    if (!idTestHomModule(v_id,currRing->qideal,w))
    {
      WarnS("wrong weights");
      w=NULL;
    }
    else
    {
      hom=isHomog;
      w=ivCopy(w);
    }
  }
  result=kSba(v_id,currRing->qideal,hom,&w,(int)(long)u->Data(),0);
  idSkipZeroes(result);
  res->data = (char *)result;
  if(!TEST_OPT_DEGBOUND) setFlag(res,FLAG_STD);
  if (w!=NULL) atSet(res,omStrDup("isHomog"),w,INTVEC_CMD);
  return FALSE;
}
static BOOLEAN jjSBA_2(leftv res, leftv v, leftv u, leftv t)
{
  ideal result;
  ideal v_id=(ideal)v->Data();
  intvec *w=(intvec *)atGet(v,"isHomog",INTVEC_CMD);
  tHomog hom=testHomog;
  if (w!=NULL)
  {
    if (!idTestHomModule(v_id,currRing->qideal,w))
    {
      WarnS("wrong weights");
      w=NULL;
    }
    else
    {
      hom=isHomog;
      w=ivCopy(w);
    }
  }
  result=kSba(v_id,currRing->qideal,hom,&w,(int)(long)u->Data(),(int)(long)t->Data());
  idSkipZeroes(result);
  res->data = (char *)result;
  if(!TEST_OPT_DEGBOUND) setFlag(res,FLAG_STD);
  if (w!=NULL) atSet(res,omStrDup("isHomog"),w,INTVEC_CMD);
  return FALSE;
}
static BOOLEAN jjSTD(leftv res, leftv v)
{
  ideal result;
  ideal v_id=(ideal)v->Data();
  intvec *w=(intvec *)atGet(v,"isHomog",INTVEC_CMD);
  tHomog hom=testHomog;
  if (w!=NULL)
  {
    if (!idTestHomModule(v_id,currRing->qideal,w))
    {
      WarnS("wrong weights");
      w=NULL;
    }
    else
    {
      hom=isHomog;
      w=ivCopy(w);
    }
  }
  result=kStd(v_id,currRing->qideal,hom,&w);
  idSkipZeroes(result);
  res->data = (char *)result;
  if(!TEST_OPT_DEGBOUND) setFlag(res,FLAG_STD);
  if (w!=NULL) atSet(res,omStrDup("isHomog"),w,INTVEC_CMD);
  return FALSE;
}
static BOOLEAN jjSort_Id(leftv res, leftv v)
{
  res->data = (char *)idSort((ideal)v->Data());
  return FALSE;
}
static BOOLEAN jjSQR_FREE(leftv res, leftv u)
{
  singclap_factorize_retry=0;
  intvec *v=NULL;
  ideal f=singclap_sqrfree((poly)(u->CopyD()), &v, 0, currRing);
  if (f==NULL) return TRUE;
  ivTest(v);
  lists l=(lists)omAllocBin(slists_bin);
  l->Init(2);
  l->m[0].rtyp=IDEAL_CMD;
  l->m[0].data=(void *)f;
  l->m[1].rtyp=INTVEC_CMD;
  l->m[1].data=(void *)v;
  res->data=(void *)l;
  return FALSE;
}
#if 1
static BOOLEAN jjSYZYGY(leftv res, leftv v)
{
  intvec *w=NULL;
  res->data = (char *)idSyzygies((ideal)v->Data(),testHomog,&w);
  if (w!=NULL) delete w;
  if (TEST_OPT_RETURN_SB) setFlag(res,FLAG_STD);
  return FALSE;
}
#else
// activate, if idSyz handle module weights correctly !
static BOOLEAN jjSYZYGY(leftv res, leftv v)
{
  intvec *w=(intvec *)atGet(v,"isHomog",INTVEC_CMD);
  ideal v_id=(ideal)v->Data();
  tHomog hom=testHomog;
  int add_row_shift=0;
  if (w!=NULL)
  {
    w=ivCopy(w);
    add_row_shift=w->min_in();
    (*w)-=add_row_shift;
    if (idTestHomModule(v_id,currRing->qideal,w))
      hom=isHomog;
    else
    {
      //WarnS("wrong weights");
      delete w; w=NULL;
      hom=testHomog;
    }
  }
  res->data = (char *)idSyzygies(v_id,hom,&w);
  if (w!=NULL)
  {
    atSet(res,omStrDup("isHomog"),w,INTVEC_CMD);
  }
  return FALSE;
}
#endif
static BOOLEAN jjTRACE_IV(leftv res, leftv v)
{
  res->data = (char *)(long)ivTrace((intvec*)(v->Data()));
  return FALSE;
}
static BOOLEAN jjTRANSP_BIM(leftv res, leftv v)
{
  res->data = (char *)(((bigintmat*)(v->Data()))->transpose());
  return FALSE;
}
static BOOLEAN jjTRANSP_IV(leftv res, leftv v)
{
  res->data = (char *)ivTranp((intvec*)(v->Data()));
  return FALSE;
}
#ifdef HAVE_PLURAL
static BOOLEAN jjOPPOSITE(leftv res, leftv a)
{
  ring    r = (ring)a->Data();
  //if (rIsPluralRing(r))
  if (r->OrdSgn==1)
  {
    res->data = rOpposite(r);
  }
  else
  {
    WarnS("opposite only for global orderings");
    res->data = rCopy(r);
  }
  return FALSE;
}
static BOOLEAN jjENVELOPE(leftv res, leftv a)
{
  ring    r = (ring)a->Data();
  if (rIsPluralRing(r))
  {
    //    ideal   i;
//     if (a->rtyp == QRING_CMD)
//     {
//       i = r->qideal;
//       r->qideal = NULL;
//     }
    ring s = rEnvelope(r);
//     if (a->rtyp == QRING_CMD)
//     {
//       ideal is  = idOppose(r,i); /* twostd? */
//       is        = idAdd(is,i);
//       s->qideal = i;
//     }
    res->data = s;
  }
  else  res->data = rCopy(r);
  return FALSE;
}
static BOOLEAN jjTWOSTD(leftv res, leftv a)
{
  if (rIsPluralRing(currRing))  res->data=(ideal)twostd((ideal)a->Data());
  else  res->data=(ideal)a->CopyD();
  setFlag(res,FLAG_STD);
  setFlag(res,FLAG_TWOSTD);
  return FALSE;
}
#endif

static BOOLEAN jjTYPEOF(leftv res, leftv v)
{
  int t=(int)(long)v->data;
  switch (t)
  {
    case CRING_CMD:
    case INT_CMD:
    case POLY_CMD:
    case VECTOR_CMD:
    case STRING_CMD:
    case INTVEC_CMD:
    case IDEAL_CMD:
    case MATRIX_CMD:
    case MODUL_CMD:
    case MAP_CMD:
    case PROC_CMD:
    case RING_CMD:
    case QRING_CMD:
    case INTMAT_CMD:
    case BIGINTMAT_CMD:
    case NUMBER_CMD:
    #ifdef SINGULAR_4_1
    case CNUMBER_CMD:
    #endif
    case BIGINT_CMD:
    case LIST_CMD:
    case PACKAGE_CMD:
    case LINK_CMD:
    case RESOLUTION_CMD:
         res->data=omStrDup(Tok2Cmdname(t)); break;
    case DEF_CMD:
    case NONE:           res->data=omStrDup("none"); break;
    default:
    {
      if (t>MAX_TOK)
        res->data=omStrDup(getBlackboxName(t));
      else
        res->data=omStrDup("?unknown type?");
      break;
    }
  }
  return FALSE;
}
static BOOLEAN jjUNIVARIATE(leftv res, leftv v)
{
  res->data=(char *)(long)pIsUnivariate((poly)v->Data());
  return FALSE;
}
static BOOLEAN jjVAR1(leftv res, leftv v)
{
  int i=(int)(long)v->Data();
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
  int i=(int)(long)v->Data();
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
  res->data = (char *)(long)scMult0Int((ideal)v->Data(),currRing->qideal);
  return FALSE;
}
BOOLEAN jjWAIT1ST1(leftv res, leftv u)
{
// input: u: a list with links of type
//           ssi-fork, ssi-tcp, MPtcp-fork or MPtcp-launch
// returns: -1:  the read state of all links is eof
//          i>0: (at least) u[i] is ready
  lists Lforks = (lists)u->Data();
  int i = slStatusSsiL(Lforks, -1);
  if(i == -2) /* error */
  {
    return TRUE;
  }
  res->data = (void*)(long)i;
  return FALSE;
}
BOOLEAN jjWAITALL1(leftv res, leftv u)
{
// input: u: a list with links of type
//           ssi-fork, ssi-tcp, MPtcp-fork or MPtcp-launch
// returns: -1: the read state of all links is eof
//           1: all links are ready
//              (caution: at least one is ready, but some maybe dead)
  lists Lforks = (lists)u->CopyD();
  int i;
  int j = -1;
  for(int nfinished = 0; nfinished < Lforks->nr+1; nfinished++)
  {
    i = slStatusSsiL(Lforks, -1);
    if(i == -2) /* error */
    {
      return TRUE;
    }
    if(i == -1)
    {
      break;
    }
    j = 1;
    Lforks->m[i-1].CleanUp();
    Lforks->m[i-1].rtyp=DEF_CMD;
    Lforks->m[i-1].data=NULL;
  }
  res->data = (void*)(long)j;
  Lforks->Clean();
  return FALSE;
}

BOOLEAN jjLOAD(const char *s, BOOLEAN autoexport)
{
  char libnamebuf[256];
  lib_types LT = type_of_LIB(s, libnamebuf);

#ifdef HAVE_DYNAMIC_LOADING
  extern BOOLEAN load_modules(const char *newlib, char *fullpath, BOOLEAN autoexport);
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
      {
        char *plib = iiConvName(s);
        idhdl pl = IDROOT->get(plib,0);
        if (pl==NULL)
        {
          pl = enterid( plib,0, PACKAGE_CMD, &(basePack->idroot), TRUE );
          IDPACKAGE(pl)->language = LANG_SINGULAR;
          IDPACKAGE(pl)->libname=omStrDup(plib);
        }
        else if (IDTYP(pl)!=PACKAGE_CMD)
        {
          Werror("can not create package `%s`",plib);
          omFree(plib);
          return TRUE;
        }
        package savepack=currPack;
        currPack=IDPACKAGE(pl);
        IDPACKAGE(pl)->loaded=TRUE;
        char libnamebuf[256];
        FILE * fp = feFopen( s, "r", libnamebuf, TRUE );
        BOOLEAN bo=iiLoadLIB(fp, libnamebuf, s, pl, autoexport, TRUE);
        currPack=savepack;
        IDPACKAGE(pl)->loaded=(!bo);
        return bo;
      }
      case LT_BUILTIN:
        SModulFunc_t iiGetBuiltinModInit(const char*);
        return load_builtin(s,autoexport, iiGetBuiltinModInit(s));
      case LT_MACH_O:
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

static BOOLEAN jjstrlen(leftv res, leftv v)
{
  res->data = (char *)strlen((char *)v->Data());
  return FALSE;
}
static BOOLEAN jjpLength(leftv res, leftv v)
{
  res->data = (char *)(long)pLength((poly)v->Data());
  return FALSE;
}
static BOOLEAN jjidElem(leftv res, leftv v)
{
  res->data = (char *)(long)idElem((ideal)v->Data());
  return FALSE;
}
static BOOLEAN jjidFreeModule(leftv res, leftv v)
{
  res->data = (char *)id_FreeModule((int)(long)v->Data(), currRing);
  return FALSE;
}
static BOOLEAN jjidVec2Ideal(leftv res, leftv v)
{
  res->data = (char *)id_Vec2Ideal((poly)v->Data(), currRing);
  return FALSE;
}
static BOOLEAN jjrCharStr(leftv res, leftv v)
{
#ifdef SINGULAR_4_1
  iiReWrite("charstr");
#endif
  res->data = rCharStr((ring)v->Data());
  return FALSE;
}
static BOOLEAN jjpHead(leftv res, leftv v)
{
  res->data = (char *)pHead((poly)v->Data());
  return FALSE;
}
static BOOLEAN jjidHead(leftv res, leftv v)
{
  res->data = (char *)id_Head((ideal)v->Data(),currRing);
  setFlag(res,FLAG_STD);
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
  res->data = (char *)mp_Trace((matrix)v->Data(),currRing);
  return FALSE;
}
static BOOLEAN jjmpTransp(leftv res, leftv v)
{
  res->data = (char *)mp_Transp((matrix)v->Data(),currRing);
  return FALSE;
}
static BOOLEAN jjrOrdStr(leftv res, leftv v)
{
#ifdef SINGULAR_4_1
  iiReWrite("ordstr");
#endif
  res->data = rOrdStr((ring)v->Data());
  return FALSE;
}
static BOOLEAN jjrVarStr(leftv res, leftv v)
{
#ifdef SINGULAR_4_1
  iiReWrite("varstr");
#endif
  res->data = rVarStr((ring)v->Data());
  return FALSE;
}
static BOOLEAN jjrParStr(leftv res, leftv v)
{
#ifdef SINGULAR_4_1
  iiReWrite("varstr");
#endif
  res->data = rParStr((ring)v->Data());
  return FALSE;
}
static BOOLEAN jjCOUNT_RES(leftv res, leftv v)
{
  res->data=(char *)(long)sySize((syStrategy)v->Data());
  return FALSE;
}
static BOOLEAN jjDIM_R(leftv res, leftv v)
{
  res->data = (char *)(long)syDim((syStrategy)v->Data());
  return FALSE;
}
static BOOLEAN jjidTransp(leftv res, leftv v)
{
  res->data = (char *)idTransp((ideal)v->Data());
  return FALSE;
}
static BOOLEAN jjnInt(leftv res, leftv u)
{
  number n=(number)u->CopyD(); // n_Int may call n_Normalize
  res->data=(char *)(long)n_Int(n,currRing->cf);
  n_Delete(&n,currRing->cf);
  return FALSE;
}
static BOOLEAN jjnlInt(leftv res, leftv u)
{
  number n=(number)u->Data();
  res->data=(char *)(long)n_Int(n,coeffs_BIGINT );
  return FALSE;
}
/*=================== operations with 3 args.: static proc =================*/
/* must be ordered: first operations for chars (infix ops),
 * then alphabetically */
static BOOLEAN jjBRACK_S(leftv res, leftv u, leftv v,leftv w)
{
  char *s= (char *)u->Data();
  int   r = (int)(long)v->Data();
  int   c = (int)(long)w->Data();
  int l = strlen(s);

  if ( (r<1) || (r>l) || (c<0) )
  {
    Werror("wrong range[%d,%d] in string %s",r,c,u->Fullname());
    return TRUE;
  }
  res->data = (char *)omAlloc((long)(c+1));
  sprintf((char *)res->data,"%-*.*s",c,c,s+r-1);
  return FALSE;
}
static BOOLEAN jjBRACK_Im(leftv res, leftv u, leftv v,leftv w)
{
  intvec *iv = (intvec *)u->Data();
  int   r = (int)(long)v->Data();
  int   c = (int)(long)w->Data();
  if ((r<1)||(r>iv->rows())||(c<1)||(c>iv->cols()))
  {
    Werror("wrong range[%d,%d] in intmat %s(%d x %d)",
           r,c,u->Fullname(),iv->rows(),iv->cols());
    return TRUE;
  }
  res->data=u->data; u->data=NULL;
  res->rtyp=u->rtyp; u->rtyp=0;
  res->name=u->name; u->name=NULL;
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
static BOOLEAN jjBRACK_Bim(leftv res, leftv u, leftv v, leftv w)
{
  bigintmat *bim = (bigintmat *)u->Data();
  int   r = (int)(long)v->Data();
  int   c = (int)(long)w->Data();
  if ((r<1)||(r>bim->rows())||(c<1)||(c>bim->cols()))
  {
    Werror("wrong range[%d,%d] in bigintmat %s(%d x %d)",
           r,c,u->Fullname(),bim->rows(),bim->cols());
    return TRUE;
  }
  res->data=u->data; u->data=NULL;
  res->rtyp=u->rtyp; u->rtyp=0;
  res->name=u->name; u->name=NULL;
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
static BOOLEAN jjBRACK_Ma(leftv res, leftv u, leftv v,leftv w)
{
  matrix m= (matrix)u->Data();
  int   r = (int)(long)v->Data();
  int   c = (int)(long)w->Data();
  //Print("gen. elem %d, %d\n",r,c);
  if ((r<1)||(r>MATROWS(m))||(c<1)||(c>MATCOLS(m)))
  {
    Werror("wrong range[%d,%d] in matrix %s(%d x %d)",r,c,u->Fullname(),
      MATROWS(m),MATCOLS(m));
    return TRUE;
  }
  res->data=u->data; u->data=NULL;
  res->rtyp=u->rtyp; u->rtyp=0;
  res->name=u->name; u->name=NULL;
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
    t.data=(char *)(long)((*iv)[l]);
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
    else if (u->Typ() == BIGINTMAT_CMD)
      nok=jjBRACK_Bim(p,u,v,&t);
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
    t.data=(char *)(long)((*iv)[l]);
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
    else if (u->Typ() == BIGINTMAT_CMD)
      nok=jjBRACK_Bim(p,u,&t,w);
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
    t1.data=(char *)(long)((*vv)[vl]);
    for (wl=0;wl< wv->length(); wl++)
    {
      t2.data=(char *)(long)((*wv)[wl]);
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
      else if (u->Typ() == BIGINTMAT_CMD)
        nok=jjBRACK_Bim(p,u,&t1,&t2);
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
  memset(w,0,sizeof(sleftv));
  return jjPROC(res,u,v);
}
static BOOLEAN jjBAREISS3(leftv res, leftv u, leftv v, leftv w)
{
  intvec *iv;
  ideal m;
  lists l=(lists)omAllocBin(slists_bin);
  int k=(int)(long)w->Data();
  if (k>=0)
  {
    sm_CallBareiss((ideal)u->Data(),(int)(long)v->Data(),(int)(long)w->Data(),m,&iv, currRing);
    l->Init(2);
    l->m[0].rtyp=MODUL_CMD;
    l->m[1].rtyp=INTVEC_CMD;
    l->m[0].data=(void *)m;
    l->m[1].data=(void *)iv;
  }
  else
  {
    m=sm_CallSolv((ideal)u->Data(), currRing);
    l->Init(1);
    l->m[0].rtyp=IDEAL_CMD;
    l->m[0].data=(void *)m;
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
  mp_Monomials((matrix)res->data, rank, pVar((poly)v->Data()),(matrix)w->Data(),currRing);
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
  mp_Monomials((matrix)res->data, rank, pVar((poly)v->Data()),(matrix)w->Data(),currRing);
  return FALSE;
}
static BOOLEAN jjELIMIN_HILB(leftv res, leftv u, leftv v, leftv w)
{
  res->data=(char *)idElimination((ideal)u->Data(),(poly)v->Data(),
    (intvec *)w->Data());
  //setFlag(res,FLAG_STD);
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
  int n=(int)(long)w->Data();
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
static BOOLEAN jjFWALK3(leftv res, leftv u, leftv v, leftv w)
{
  if ((int)(long)w->Data()==0)
    res->data=(char *)walkProc(u,v);
  else
    res->data=(char *)fractalWalkProc(u,v);
  setFlag( res, FLAG_STD );
  return FALSE;
}
static BOOLEAN jjHILBERT3(leftv res, leftv u, leftv v, leftv w)
{
  intvec *wdegree=(intvec*)w->Data();
  if (wdegree->length()!=currRing->N)
  {
    Werror("weight vector must have size %d, not %d",
           currRing->N,wdegree->length());
    return TRUE;
  }
#ifdef HAVE_RINGS
  if (rField_is_Ring_Z(currRing))
  {
    ring origR = currRing;
    ring tempR = rCopy(origR);
    coeffs new_cf=nInitChar(n_Q,NULL);
    nKillChar(tempR->cf);
    tempR->cf=new_cf;
    rComplete(tempR);
    ideal uid = (ideal)u->Data();
    rChangeCurrRing(tempR);
    ideal uu = idrCopyR(uid, origR, currRing);
    sleftv uuAsLeftv; memset(&uuAsLeftv, 0, sizeof(uuAsLeftv));
    uuAsLeftv.rtyp = IDEAL_CMD;
    uuAsLeftv.data = uu; uuAsLeftv.next = NULL;
    if (hasFlag(u, FLAG_STD)) setFlag(&uuAsLeftv,FLAG_STD);
    assumeStdFlag(&uuAsLeftv);
    Print("// NOTE: computation of Hilbert series etc. is being\n");
    Print("//       performed for generic fibre, that is, over Q\n");
    intvec *module_w=(intvec*)atGet(&uuAsLeftv,"isHomog",INTVEC_CMD);
    intvec *iv=hFirstSeries(uu,module_w,currRing->qideal,wdegree);
    int returnWithTrue = 1;
    switch((int)(long)v->Data())
    {
      case 1:
        res->data=(void *)iv;
        returnWithTrue = 0;
      case 2:
        res->data=(void *)hSecondSeries(iv);
        delete iv;
        returnWithTrue = 0;
    }
    if (returnWithTrue)
    {
      WerrorS(feNotImplemented);
      delete iv;
    }
    idDelete(&uu);
    rChangeCurrRing(origR);
    rDelete(tempR);
    if (returnWithTrue) return TRUE; else return FALSE;
  }
#endif
  assumeStdFlag(u);
  intvec *module_w=(intvec *)atGet(u,"isHomog",INTVEC_CMD);
  intvec *iv=hFirstSeries((ideal)u->Data(),module_w,currRing->qideal,wdegree);
  switch((int)(long)v->Data())
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
static BOOLEAN jjHOMOG_ID_W(leftv res, leftv u, leftv v, leftv /*w*/)
{
  PrintS("TODO\n");
  int i=pVar((poly)v->Data());
  if (i==0)
  {
    WerrorS("ringvar expected");
    return TRUE;
  }
  poly p=pOne(); pSetExp(p,i,1); pSetm(p);
  int d=pWTotaldegree(p);
  pLmDelete(p);
  if (d==1)
    res->data = (char *)id_Homogen((ideal)u->Data(), i, currRing);
  else
    WerrorS("variable must have weight 1");
  return (d!=1);
}
static BOOLEAN jjHOMOG_P_W(leftv res, leftv u, leftv v,leftv /*w*/)
{
  PrintS("TODO\n");
  int i=pVar((poly)v->Data());
  if (i==0)
  {
    WerrorS("ringvar expected");
    return TRUE;
  }
  poly p=pOne(); pSetExp(p,i,1); pSetm(p);
  int d=pWTotaldegree(p);
  pLmDelete(p);
  if (d==1)
    res->data = (char *)p_Homogen((poly)u->Data(), i, currRing);
  else
    WerrorS("variable must have weight 1");
  return (d!=1);
}
static BOOLEAN jjINTMAT3(leftv res, leftv u, leftv v,leftv w)
{
  intvec* im= new intvec((int)(long)v->Data(),(int)(long)w->Data(), 0);
  intvec* arg = (intvec*) u->Data();
  int i, n = si_min(im->cols()*im->rows(), arg->cols()*arg->rows());

  for (i=0; i<n; i++)
  {
    (*im)[i] = (*arg)[i];
  }

  res->data = (char *)im;
  return FALSE;
}
static BOOLEAN jjJET_P_IV(leftv res, leftv u, leftv v, leftv w)
{
  short *iw=iv2array((intvec *)w->Data(),currRing);
  res->data = (char *)ppJetW((poly)u->Data(),(int)(long)v->Data(),iw);
  omFreeSize( (ADDRESS)iw, (rVar(currRing)+1)*sizeof(short) );
  return FALSE;
}
static BOOLEAN jjJET_P_P(leftv res, leftv u, leftv v, leftv w)
{
  if (!pIsUnit((poly)v->Data()))
  {
    WerrorS("2nd argument must be a unit");
    return TRUE;
  }
  res->data = (char *)p_Series((int)(long)w->Data(),(poly)u->CopyD(),(poly)v->CopyD(),NULL,currRing);
  return FALSE;
}
static BOOLEAN jjJET_ID_IV(leftv res, leftv u, leftv v, leftv w)
{
  res->data = (char *)id_JetW((ideal)u->Data(),(int)(long)v->Data(),
                             (intvec *)w->Data(),currRing);
  return FALSE;
}
static BOOLEAN jjJET_ID_M(leftv res, leftv u, leftv v, leftv w)
{
  if (!mp_IsDiagUnit((matrix)v->Data(), currRing))
  {
    WerrorS("2nd argument must be a diagonal matrix of units");
    return TRUE;
  }
  res->data = (char *)idSeries((int)(long)w->Data(),(ideal)u->CopyD(),
                               (matrix)v->CopyD());
  return FALSE;
}
static BOOLEAN currRingIsOverIntegralDomain ()
{
  /* true for fields and Z, false otherwise */
  if (rField_is_Ring_PtoM(currRing)) return FALSE;
  if (rField_is_Ring_2toM(currRing)) return FALSE;
  if (rField_is_Ring_ModN(currRing)) return FALSE;
  return TRUE;
}
static BOOLEAN jjMINOR_M(leftv res, leftv v)
{
  /* Here's the use pattern for the minor command:
        minor ( matrix_expression m, int_expression minorSize,
                optional ideal_expression IasSB, optional int_expression k,
                optional string_expression algorithm,
                optional int_expression cachedMinors,
                optional int_expression cachedMonomials )
     This method here assumes that there are at least two arguments.
     - If IasSB is present, it must be a std basis. All minors will be
       reduced w.r.t. IasSB.
     - If k is absent, all non-zero minors will be computed.
       If k is present and k > 0, the first k non-zero minors will be
       computed.
       If k is present and k < 0, the first |k| minors (some of which
       may be zero) will be computed.
       If k is present and k = 0, an error is reported.
     - If algorithm is absent, all the following arguments must be absent too.
       In this case, a heuristic picks the best-suited algorithm (among
       Bareiss, Laplace, and Laplace with caching).
       If algorithm is present, it must be one of "Bareiss", "bareiss",
       "Laplace", "laplace", "Cache", "cache". In the cases "Cache" and
       "cache" two more arguments may be given, determining how many entries
       the cache may have at most, and how many cached monomials there are at
       most. (Cached monomials are counted over all cached polynomials.)
       If these two additional arguments are not provided, 200 and 100000
       will be used as defaults.
  */
  matrix m;
  leftv u=v->next;
  v->next=NULL;
  int v_typ=v->Typ();
  if (v_typ==MATRIX_CMD)
  {
     m = (const matrix)v->Data();
  }
  else
  {
    if (v_typ==0)
    {
      Werror("`%s` is undefined",v->Fullname());
      return TRUE;
    }
    // try to convert to MATRIX:
    int ii=iiTestConvert(v_typ,MATRIX_CMD);
    BOOLEAN bo;
    sleftv tmp;
    if (ii>0) bo=iiConvert(v_typ,MATRIX_CMD,ii,v,&tmp);
    else bo=TRUE;
    if (bo)
    {
      Werror("cannot convert %s to matrix",Tok2Cmdname(v_typ));
      return TRUE;
    }
    m=(matrix)tmp.data;
  }
  const int mk = (const int)(long)u->Data();
  bool noIdeal = true; bool noK = true; bool noAlgorithm = true;
  bool noCacheMinors = true; bool noCacheMonomials = true;
  ideal IasSB; int k; char* algorithm; int cacheMinors; int cacheMonomials;

  /* here come the different cases of correct argument sets */
  if ((u->next != NULL) && (u->next->Typ() == IDEAL_CMD))
  {
    IasSB = (ideal)u->next->Data();
    noIdeal = false;
    if ((u->next->next != NULL) && (u->next->next->Typ() == INT_CMD))
    {
      k = (int)(long)u->next->next->Data();
      noK = false;
      assume(k != 0);
      if ((u->next->next->next != NULL) &&
          (u->next->next->next->Typ() == STRING_CMD))
      {
        algorithm = (char*)u->next->next->next->Data();
        noAlgorithm = false;
        if ((u->next->next->next->next != NULL) &&
            (u->next->next->next->next->Typ() == INT_CMD))
        {
          cacheMinors = (int)(long)u->next->next->next->next->Data();
          noCacheMinors = false;
          if ((u->next->next->next->next->next != NULL) &&
              (u->next->next->next->next->next->Typ() == INT_CMD))
          {
            cacheMonomials =
               (int)(long)u->next->next->next->next->next->Data();
            noCacheMonomials = false;
          }
        }
      }
    }
  }
  else if ((u->next != NULL) && (u->next->Typ() == INT_CMD))
  {
    k = (int)(long)u->next->Data();
    noK = false;
    assume(k != 0);
    if ((u->next->next != NULL) && (u->next->next->Typ() == STRING_CMD))
    {
      algorithm = (char*)u->next->next->Data();
      noAlgorithm = false;
      if ((u->next->next->next != NULL) &&
          (u->next->next->next->Typ() == INT_CMD))
      {
        cacheMinors = (int)(long)u->next->next->next->Data();
        noCacheMinors = false;
        if ((u->next->next->next->next != NULL) &&
            (u->next->next->next->next->Typ() == INT_CMD))
        {
          cacheMonomials = (int)(long)u->next->next->next->next->Data();
          noCacheMonomials = false;
        }
      }
    }
  }
  else if ((u->next != NULL) && (u->next->Typ() == STRING_CMD))
  {
    algorithm = (char*)u->next->Data();
    noAlgorithm = false;
    if ((u->next->next != NULL) && (u->next->next->Typ() == INT_CMD))
    {
      cacheMinors = (int)(long)u->next->next->Data();
      noCacheMinors = false;
      if ((u->next->next->next != NULL) &&
          (u->next->next->next->Typ() == INT_CMD))
      {
        cacheMonomials = (int)(long)u->next->next->next->Data();
        noCacheMonomials = false;
      }
    }
  }

  /* upper case conversion for the algorithm if present */
  if (!noAlgorithm)
  {
    if (strcmp(algorithm, "bareiss") == 0)
      algorithm = (char*)"Bareiss";
    if (strcmp(algorithm, "laplace") == 0)
      algorithm = (char*)"Laplace";
    if (strcmp(algorithm, "cache") == 0)
      algorithm = (char*)"Cache";
  }

  v->next=u;
  /* here come some tests */
  if (!noIdeal)
  {
    assumeStdFlag(u->next);
  }
  if ((!noK) && (k == 0))
  {
    WerrorS("Provided number of minors to be computed is zero.");
    return TRUE;
  }
  if ((!noAlgorithm) && (strcmp(algorithm, "Bareiss") != 0)
      && (strcmp(algorithm, "Laplace") != 0)
      && (strcmp(algorithm, "Cache") != 0))
  {
    WerrorS("Expected as algorithm one of 'B/bareiss', 'L/laplace', or 'C/cache'.");
    return TRUE;
  }
  if ((!noAlgorithm) && (strcmp(algorithm, "Bareiss") == 0)
      && (!currRingIsOverIntegralDomain()))
  {
    Werror("Bareiss algorithm not defined over coefficient rings %s",
           "with zero divisors.");
    return TRUE;
  }
  if ((mk < 1) || (mk > m->rows()) || (mk > m->cols()))
  {
    Werror("invalid size of minors: %d (matrix is (%d x %d))", mk,
           m->rows(), m->cols());
    return TRUE;
  }
  if ((!noAlgorithm) && (strcmp(algorithm, "Cache") == 0)
      && (noCacheMinors || noCacheMonomials))
  {
    cacheMinors = 200;
    cacheMonomials = 100000;
  }

  /* here come the actual procedure calls */
  if (noAlgorithm)
    res->data = getMinorIdealHeuristic(m, mk, (noK ? 0 : k),
                                       (noIdeal ? 0 : IasSB), false);
  else if (strcmp(algorithm, "Cache") == 0)
    res->data = getMinorIdealCache(m, mk, (noK ? 0 : k),
                                   (noIdeal ? 0 : IasSB), 3, cacheMinors,
                                   cacheMonomials, false);
  else
    res->data = getMinorIdeal(m, mk, (noK ? 0 : k), algorithm,
                              (noIdeal ? 0 : IasSB), false);
  if (v_typ!=MATRIX_CMD) idDelete((ideal *)&m);
  res->rtyp = IDEAL_CMD;
  return FALSE;
}
static BOOLEAN jjNEWSTRUCT3(leftv, leftv u, leftv v, leftv w)
{
  // u: the name of the new type
  // v: the parent type
  // w: the elements
  newstruct_desc d=newstructChildFromString((const char *)v->Data(),
                                            (const char *)w->Data());
  if (d!=NULL) newstruct_setup((const char *)u->Data(),d);
  return (d==NULL);
}
static BOOLEAN jjPREIMAGE(leftv res, leftv u, leftv v, leftv w)
{
  // handles preimage(r,phi,i) and kernel(r,phi)
  idhdl h;
  ring rr;
  map mapping;
  BOOLEAN kernel_cmd= (iiOp==KERNEL_CMD);

  if ((v->name==NULL) || (!kernel_cmd && (w->name==NULL)))
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
  ideal image;
  if (kernel_cmd) image=idInit(1,1);
  else
  {
    if ((h=rr->idroot->get(w->name,myynest))!=NULL)
    {
      if (h->typ==IDEAL_CMD)
      {
        image=IDIDEAL(h);
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
  }
  if (((currRing->qideal!=NULL) && (rHasLocalOrMixedOrdering_currRing()))
  || ((rr->qideal!=NULL) && (rHasLocalOrMixedOrdering(rr))))
  {
    WarnS("preimage in local qring may be wrong: use Ring::preimageLoc instead");
  }
  res->data=(char *)maGetPreimage(rr,mapping,image,currRing);
  if (kernel_cmd) idDelete(&image);
  return (res->data==NULL/* is of type ideal, should not be NULL*/);
}
static BOOLEAN jjRANDOM_Im(leftv res, leftv u, leftv v, leftv w)
{
  int di, k;
  int i=(int)(long)u->Data();
  int r=(int)(long)v->Data();
  int c=(int)(long)w->Data();
  if ((r<=0) || (c<=0)) return TRUE;
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
      (*iv)[k] = ((siRand() % di) - i);
    }
  }
  res->data = (char *)iv;
  return FALSE;
}
#ifdef SINGULAR_4_1
static BOOLEAN jjRANDOM_CF(leftv res, leftv u, leftv v, leftv w)
// <coeff>, par1, par2 -> number2
{
  coeffs cf=(coeffs)u->Data();
  if ((cf!=NULL) && (cf->cfRandom!=NULL))
  {
    number n= n_Random(siRand,(number)v->Data(),(number)w->Data(),cf);
    number2 nn=(number2)omAlloc(sizeof(*nn));
    nn->cf=cf;
    nn->n=n;
    res->data=nn;
    return FALSE;
  }
  return TRUE;
}
#endif
static BOOLEAN jjSUBST_Test(leftv v,leftv w,
  int &ringvar, poly &monomexpr)
{
  monomexpr=(poly)w->Data();
  poly p=(poly)v->Data();
#if 0
  if (pLength(monomexpr)>1)
  {
    Werror("`%s` substitutes a ringvar only by a term",
      Tok2Cmdname(SUBST_CMD));
    return TRUE;
  }
#endif
  if ((ringvar=pVar(p))==0)
  {
    if ((p!=NULL) && (currRing->cf->extRing!=NULL))
    {
      number n = pGetCoeff(p);
      ringvar= -n_IsParam(n, currRing);
    }
    if(ringvar==0)
    {
      WerrorS("ringvar/par expected");
      return TRUE;
    }
  }
  return FALSE;
}
static BOOLEAN jjSUBST_P(leftv res, leftv u, leftv v,leftv w)
{
  int ringvar;
  poly monomexpr;
  BOOLEAN nok=jjSUBST_Test(v,w,ringvar,monomexpr);
  if (nok) return TRUE;
  poly p=(poly)u->Data();
  if (ringvar>0)
  {
    if ((monomexpr!=NULL) && (p!=NULL) && (pTotaldegree(p)!=0) &&
    ((unsigned long)pTotaldegree(monomexpr) > (currRing->bitmask / (unsigned long)pTotaldegree(p)/2)))
    {
      Warn("possible OVERFLOW in subst, max exponent is %ld, subtituting deg %d by deg %d",currRing->bitmask/2, pTotaldegree(monomexpr), pTotaldegree(p));
      //return TRUE;
    }
    if ((monomexpr==NULL)||(pNext(monomexpr)==NULL))
      res->data = pSubst((poly)u->CopyD(res->rtyp),ringvar,monomexpr);
    else
      res->data= pSubstPoly(p,ringvar,monomexpr);
  }
  else
  {
    res->data=pSubstPar(p,-ringvar,monomexpr);
  }
  return FALSE;
}
static BOOLEAN jjSUBST_Id(leftv res, leftv u, leftv v,leftv w)
{
  int ringvar;
  poly monomexpr;
  BOOLEAN nok=jjSUBST_Test(v,w,ringvar,monomexpr);
  if (nok) return TRUE;
  ideal id=(ideal)u->Data();
  if (ringvar>0)
  {
    BOOLEAN overflow=FALSE;
    if (monomexpr!=NULL)
    {
      long deg_monexp=pTotaldegree(monomexpr);
      for(int i=IDELEMS(id)-1;i>=0;i--)
      {
        poly p=id->m[i];
        if ((p!=NULL) && (pTotaldegree(p)!=0) &&
        ((unsigned long)deg_monexp > (currRing->bitmask / (unsigned long)pTotaldegree(p)/2)))
        {
          overflow=TRUE;
          break;
        }
      }
    }
    if (overflow)
      Warn("possible OVERFLOW in subst, max exponent is %ld",currRing->bitmask/2);
    if ((monomexpr==NULL)||(pNext(monomexpr)==NULL))
    {
      if (res->rtyp==MATRIX_CMD) id=(ideal)mp_Copy((matrix)id,currRing);
      else                       id=id_Copy(id,currRing);
      res->data = id_Subst(id, ringvar, monomexpr, currRing);
    }
    else
      res->data = idSubstPoly(id,ringvar,monomexpr);
  }
  else
  {
    res->data = idSubstPar(id,-ringvar,monomexpr);
  }
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
  int mi=(int)(long)v->Data();
  int ni=(int)(long)w->Data();
  if ((mi<1)||(ni<1))
  {
    Werror("converting ideal to matrix: dimensions must be positive(%dx%d)",mi,ni);
    return TRUE;
  }
  matrix m=mpNew(mi,ni);
  ideal I=(ideal)u->CopyD(IDEAL_CMD);
  int i=si_min(IDELEMS(I),mi*ni);
  //for(i=i-1;i>=0;i--)
  //{
  //  m->m[i]=I->m[i];
  //  I->m[i]=NULL;
  //}
  memcpy(m->m,I->m,i*sizeof(poly));
  memset(I->m,0,i*sizeof(poly));
  id_Delete(&I,currRing);
  res->data = (char *)m;
  return FALSE;
}
static BOOLEAN jjMATRIX_Mo(leftv res, leftv u, leftv v,leftv w)
{
  int mi=(int)(long)v->Data();
  int ni=(int)(long)w->Data();
  if ((mi<1)||(ni<1))
  {
    Werror("converting module to matrix: dimensions must be positive(%dx%d)",mi,ni);
    return TRUE;
  }
  res->data = (char *)id_Module2formatedMatrix((ideal)u->CopyD(MODUL_CMD),
           mi,ni,currRing);
  return FALSE;
}
static BOOLEAN jjMATRIX_Ma(leftv res, leftv u, leftv v,leftv w)
{
  int mi=(int)(long)v->Data();
  int ni=(int)(long)w->Data();
  if ((mi<1)||(ni<1))
  {
     Werror("converting matrix to matrix: dimensions must be positive(%dx%d)",mi,ni);
    return TRUE;
  }
  matrix m=mpNew(mi,ni);
  matrix I=(matrix)u->CopyD(MATRIX_CMD);
  int r=si_min(MATROWS(I),mi);
  int c=si_min(MATCOLS(I),ni);
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      MATELEM(m,i,j)=MATELEM(I,i,j);
      MATELEM(I,i,j)=NULL;
    }
  }
  id_Delete((ideal *)&I,currRing);
  res->data = (char *)m;
  return FALSE;
}
static BOOLEAN jjLIFT3(leftv res, leftv u, leftv v, leftv w)
{
  if (w->rtyp!=IDHDL) return TRUE;
  int ul= IDELEMS((ideal)u->Data());
  int vl= IDELEMS((ideal)v->Data());
  ideal m
    = idLift((ideal)u->Data(),(ideal)v->Data(),NULL,FALSE,hasFlag(u,FLAG_STD),
             FALSE, (matrix *)(&(IDMATRIX((idhdl)(w->data)))));
  if (m==NULL) return TRUE;
  res->data = (char *)id_Module2formatedMatrix(m,ul,vl,currRing);
  return FALSE;
}
static BOOLEAN jjLIFTSTD3(leftv res, leftv u, leftv v, leftv w)
{
  if ((v->rtyp!=IDHDL)||(v->e!=NULL)) return TRUE;
  if ((w->rtyp!=IDHDL)||(w->e!=NULL)) return TRUE;
  idhdl hv=(idhdl)v->data;
  idhdl hw=(idhdl)w->data;
  // CopyD for IDEAL_CMD and MODUL_CMD are identical:
  res->data = (char *)idLiftStd((ideal)u->Data(),
                                &(hv->data.umatrix),testHomog,
                                &(hw->data.uideal));
  setFlag(res,FLAG_STD); v->flag=0; w->flag=0;
  return FALSE;
}
static BOOLEAN jjREDUCE3_CP(leftv res, leftv u, leftv v, leftv w)
{
  assumeStdFlag(v);
  if (!idIsZeroDim((ideal)v->Data()))
  {
    Werror("`%s` must be 0-dimensional",v->Name());
    return TRUE;
  }
  res->data = (char *)redNF((ideal)v->CopyD(),(poly)u->CopyD(),
    (poly)w->CopyD());
  return FALSE;
}
static BOOLEAN jjREDUCE3_CID(leftv res, leftv u, leftv v, leftv w)
{
  assumeStdFlag(v);
  if (!idIsZeroDim((ideal)v->Data()))
  {
    Werror("`%s` must be 0-dimensional",v->Name());
    return TRUE;
  }
  res->data = (char *)redNF((ideal)v->CopyD(),(ideal)u->CopyD(),
    (matrix)w->CopyD());
  return FALSE;
}
static BOOLEAN jjREDUCE3_P(leftv res, leftv u, leftv v, leftv w)
{
  assumeStdFlag(v);
  res->data = (char *)kNF((ideal)v->Data(),currRing->qideal,(poly)u->Data(),
    0,(int)(long)w->Data());
  return FALSE;
}
static BOOLEAN jjREDUCE3_ID(leftv res, leftv u, leftv v, leftv w)
{
  assumeStdFlag(v);
  res->data = (char *)kNF((ideal)v->Data(),currRing->qideal,(ideal)u->Data(),
    0,(int)(long)w->Data());
  return FALSE;
}
#ifdef OLD_RES
static BOOLEAN jjRES3(leftv res, leftv u, leftv v, leftv w)
{
  int maxl=(int)v->Data();
  ideal u_id=(ideal)u->Data();
  int l=0;
  resolvente r;
  intvec **weights=NULL;
  int wmaxl=maxl;
  maxl--;
  if ((maxl==-1) && (iiOp!=MRES_CMD))
    maxl = currRing->N-1;
  if ((iiOp == RES_CMD) || (iiOp == MRES_CMD))
  {
    intvec * iv=(intvec*)atGet(u,"isHomog",INTVEC_CMD);
    if (iv!=NULL)
    {
      l=1;
      if (!idTestHomModule(u_id,currRing->qideal,iv))
      {
        WarnS("wrong weights");
        iv=NULL;
      }
      else
      {
        weights = (intvec**)omAlloc0Bin(char_ptr_bin);
        weights[0] = ivCopy(iv);
      }
    }
    r=syResolvente(u_id,maxl,&l, &weights, iiOp==MRES_CMD);
  }
  else
    r=sySchreyerResolvente((ideal)u->Data(),maxl+1,&l);
  if (r==NULL) return TRUE;
  int t3=u->Typ();
  iiMakeResolv(r,l,wmaxl,w->name,t3,weights);
  return FALSE;
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
  res->data = (void *)(long)yes;
  return FALSE;
}
static BOOLEAN jjSTD_HILB_W(leftv res, leftv u, leftv v, leftv w)
{
  intvec *vw=(intvec *)w->Data(); // weights of vars
  if (vw->length()!=currRing->N)
  {
    Werror("%d weights for %d variables",vw->length(),currRing->N);
    return TRUE;
  }
  ideal result;
  intvec *ww=(intvec *)atGet(u,"isHomog",INTVEC_CMD);
  tHomog hom=testHomog;
  ideal u_id=(ideal)(u->Data());
  if (ww!=NULL)
  {
    if (!idTestHomModule(u_id,currRing->qideal,ww))
    {
      WarnS("wrong weights");
      ww=NULL;
    }
    else
    {
      ww=ivCopy(ww);
      hom=isHomog;
    }
  }
  result=kStd(u_id,
              currRing->qideal,
              hom,
              &ww,                  // module weights
              (intvec *)v->Data(),  // hilbert series
              0,0,                  // syzComp, newIdeal
              vw);                  // weights of vars
  idSkipZeroes(result);
  res->data = (char *)result;
  setFlag(res,FLAG_STD);
  if (ww!=NULL) atSet(res,omStrDup("isHomog"),ww,INTVEC_CMD);
  return FALSE;
}

/*=================== operations with many arg.: static proc =================*/
/* must be ordered: first operations for chars (infix ops),
 * then alphabetically */
static BOOLEAN jjBREAK0(leftv, leftv)
{
#ifdef HAVE_SDB
  sdb_show_bp();
#endif
  return FALSE;
}
static BOOLEAN jjBREAK1(leftv, leftv v)
{
#ifdef HAVE_SDB
  if(v->Typ()==PROC_CMD)
  {
    int lineno=0;
    if((v->next!=NULL) && (v->next->Typ()==INT_CMD))
    {
      lineno=(int)(long)v->next->Data();
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

static BOOLEAN jjCOEF_M(leftv, leftv v)
{
  short t[]={5,VECTOR_CMD,POLY_CMD,MATRIX_CMD,MATRIX_CMD,IDHDL};
  if (iiCheckTypes(v,t))
     return TRUE;
  idhdl c=(idhdl)v->next->next->data;
  if (v->next->next->next->rtyp!=IDHDL) return TRUE;
  idhdl m=(idhdl)v->next->next->next->data;
  idDelete((ideal *)&(c->data.uideal));
  idDelete((ideal *)&(m->data.uideal));
  mp_Coef2((poly)v->Data(),(poly)v->next->Data(),
    (matrix *)&(c->data.umatrix),(matrix *)&(m->data.umatrix),currRing);
  return FALSE;
}

static BOOLEAN jjDIVISION4(leftv res, leftv v)
{ // may have 3 or 4 arguments
  leftv v1=v;
  leftv v2=v1->next;
  leftv v3=v2->next;
  leftv v4=v3->next;
  assumeStdFlag(v2);

  int i1=iiTestConvert(v1->Typ(),MODUL_CMD);
  int i2=iiTestConvert(v2->Typ(),MODUL_CMD);

  if((i1==0)||(i2==0)
  ||(v3->Typ()!=INT_CMD)||((v4!=NULL)&&(v4->Typ()!=INTVEC_CMD)))
  {
    WarnS("<module>,<module>,<int>[,<intvec>] expected!");
    return TRUE;
  }

  sleftv w1,w2;
  iiConvert(v1->Typ(),MODUL_CMD,i1,v1,&w1);
  iiConvert(v2->Typ(),MODUL_CMD,i2,v2,&w2);
  ideal P=(ideal)w1.Data();
  ideal Q=(ideal)w2.Data();

  int n=(int)(long)v3->Data();
  short *w=NULL;
  if(v4!=NULL)
  {
    w = iv2array((intvec *)v4->Data(),currRing);
    short * w0 = w + 1;
    int i = currRing->N;
    while( (i > 0) && ((*w0) > 0) )
    {
      w0++;
      i--;
    }
    if(i>0)
      WarnS("not all weights are positive!");
  }

  matrix T;
  ideal R;
  idLiftW(P,Q,n,T,R,w);

  w1.CleanUp();
  w2.CleanUp();
  if(w!=NULL)
    omFreeSize( (ADDRESS)w, (rVar(currRing)+1)*sizeof(short) );

  lists L=(lists) omAllocBin(slists_bin);
  L->Init(2);
  L->m[1].rtyp=v1->Typ();
  if(v1->Typ()==POLY_CMD||v1->Typ()==VECTOR_CMD)
  {
    if(v1->Typ()==POLY_CMD)
      p_Shift(&R->m[0],-1,currRing);
    L->m[1].data=(void *)R->m[0];
    R->m[0]=NULL;
    idDelete(&R);
  }
  else if(v1->Typ()==IDEAL_CMD||v1->Typ()==MATRIX_CMD)
    L->m[1].data=(void *)id_Module2Matrix(R,currRing);
  else
  {
    L->m[1].rtyp=MODUL_CMD;
    L->m[1].data=(void *)R;
  }
  L->m[0].rtyp=MATRIX_CMD;
  L->m[0].data=(char *)T;

  res->data=L;
  res->rtyp=LIST_CMD;

  return FALSE;
}

//BOOLEAN jjDISPATCH(leftv res, leftv v)
//{
//  WerrorS("`dispatch`: not implemented");
//  return TRUE;
//}

//static BOOLEAN jjEXPORTTO_M(leftv res, leftv u)
//{
//  int l=u->listLength();
//  if (l<2) return TRUE;
//  BOOLEAN b;
//  leftv v=u->next;
//  leftv zz=v;
//  leftv z=zz;
//  u->next=NULL;
//  do
//  {
//    leftv z=z->next;
//    b=iiExprArith2(res,u,iiOp,z, (iiOp > 255));
//    if (b) break;
//  } while (z!=NULL);
//  u->next=zz;
//  return b;
//}
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
        number n=nInit((int)(long)h->Data());
        if (!nIsZero(n))
        {
          p=pNSet(n);
        }
        else
        {
          p=NULL;
          nDelete(&n);
        }
        break;
      }
      case BIGINT_CMD:
      {
        number b=(number)h->Data();
        nMapFunc nMap=n_SetMap(coeffs_BIGINT,currRing->cf);
        if (nMap==NULL) return TRUE;
        number n=nMap(b,coeffs_BIGINT,currRing->cf);
        if (!nIsZero(n))
        {
          p=pNSet(n);
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
          p=pNSet(n);
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
        rank=si_max(rank,(int)pMaxComp(p));
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
static BOOLEAN jjLU_INVERSE(leftv res, leftv v)
{
  /* computation of the inverse of a quadratic matrix A
     using the L-U-decomposition of A;
     There are two valid parametrisations:
     1) exactly one argument which is just the matrix A,
     2) exactly three arguments P, L, U which already
        realise the L-U-decomposition of A, that is,
        P * A = L * U, and P, L, and U satisfy the
        properties decribed in method 'jjLU_DECOMP';
        see there;
     If A is invertible, the list [1, A^(-1)] is returned,
     otherwise the list [0] is returned. Thus, the user may
     inspect the first entry of the returned list to see
     whether A is invertible. */
  matrix iMat; int invertible;
  short t1[]={1,MATRIX_CMD};
  short t2[]={3,MATRIX_CMD,MATRIX_CMD,MATRIX_CMD};
  if (iiCheckTypes(v,t1))
  {
    matrix aMat = (matrix)v->Data();
    int rr = aMat->rows();
    int cc = aMat->cols();
    if (rr != cc)
    {
      Werror("given matrix (%d x %d) is not quadratic, hence not invertible", rr, cc);
      return TRUE;
    }
    if (!idIsConstant((ideal)aMat))
    {
      WerrorS("matrix must be constant");
      return TRUE;
    }
    invertible = luInverse(aMat, iMat);
  }
  else if (iiCheckTypes(v,t2))
  {
     matrix pMat = (matrix)v->Data();
     matrix lMat = (matrix)v->next->Data();
     matrix uMat = (matrix)v->next->next->Data();
     int rr = uMat->rows();
     int cc = uMat->cols();
     if (rr != cc)
     {
       Werror("third matrix (%d x %d) is not quadratic, hence not invertible",
              rr, cc);
       return TRUE;
     }
      if (!idIsConstant((ideal)pMat)
      || (!idIsConstant((ideal)lMat))
      || (!idIsConstant((ideal)uMat))
      )
      {
        WerrorS("matricesx must be constant");
        return TRUE;
      }
     invertible = luInverseFromLUDecomp(pMat, lMat, uMat, iMat);
  }
  else
  {
    Werror("expected either one or three matrices");
    return TRUE;
  }

  /* build the return structure; a list with either one or two entries */
  lists ll = (lists)omAllocBin(slists_bin);
  if (invertible)
  {
    ll->Init(2);
    ll->m[0].rtyp=INT_CMD;    ll->m[0].data=(void *)(long)invertible;
    ll->m[1].rtyp=MATRIX_CMD; ll->m[1].data=(void *)iMat;
  }
  else
  {
    ll->Init(1);
    ll->m[0].rtyp=INT_CMD;    ll->m[0].data=(void *)(long)invertible;
  }

  res->data=(char*)ll;
  return FALSE;
}
static BOOLEAN jjLU_SOLVE(leftv res, leftv v)
{
  /* for solving a linear equation system A * x = b, via the
     given LU-decomposition of the matrix A;
     There is one valid parametrisation:
     1) exactly four arguments P, L, U, b;
        P, L, and U realise the L-U-decomposition of A, that is,
        P * A = L * U, and P, L, and U satisfy the
        properties decribed in method 'jjLU_DECOMP';
        see there;
        b is the right-hand side vector of the equation system;
     The method will return a list of either 1 entry or three entries:
     1) [0] if there is no solution to the system;
     2) [1, x, H] if there is at least one solution;
        x is any solution of the given linear system,
        H is the matrix with column vectors spanning the homogeneous
        solution space.
     The method produces an error if matrix and vector sizes do not fit. */
  short t[]={4,MATRIX_CMD,MATRIX_CMD,MATRIX_CMD,MATRIX_CMD};
  if (!iiCheckTypes(v,t))
  {
    WerrorS("expected exactly three matrices and one vector as input");
    return TRUE;
  }
  matrix pMat = (matrix)v->Data();
  matrix lMat = (matrix)v->next->Data();
  matrix uMat = (matrix)v->next->next->Data();
  matrix bVec = (matrix)v->next->next->next->Data();
  matrix xVec; int solvable; matrix homogSolSpace;
  if (pMat->rows() != pMat->cols())
  {
    Werror("first matrix (%d x %d) is not quadratic",
           pMat->rows(), pMat->cols());
    return TRUE;
  }
  if (lMat->rows() != lMat->cols())
  {
    Werror("second matrix (%d x %d) is not quadratic",
           lMat->rows(), lMat->cols());
    return TRUE;
  }
  if (lMat->rows() != uMat->rows())
  {
    Werror("second matrix (%d x %d) and third matrix (%d x %d) do not fit",
           lMat->rows(), lMat->cols(), uMat->rows(), uMat->cols());
    return TRUE;
  }
  if (uMat->rows() != bVec->rows())
  {
    Werror("third matrix (%d x %d) and vector (%d x 1) do not fit",
           uMat->rows(), uMat->cols(), bVec->rows());
    return TRUE;
  }
  if (!idIsConstant((ideal)pMat)
  ||(!idIsConstant((ideal)lMat))
  ||(!idIsConstant((ideal)uMat))
  )
  {
    WerrorS("matrices must be constant");
    return TRUE;
  }
  solvable = luSolveViaLUDecomp(pMat, lMat, uMat, bVec, xVec, homogSolSpace);

  /* build the return structure; a list with either one or three entries */
  lists ll = (lists)omAllocBin(slists_bin);
  if (solvable)
  {
    ll->Init(3);
    ll->m[0].rtyp=INT_CMD;    ll->m[0].data=(void *)(long)solvable;
    ll->m[1].rtyp=MATRIX_CMD; ll->m[1].data=(void *)xVec;
    ll->m[2].rtyp=MATRIX_CMD; ll->m[2].data=(void *)homogSolSpace;
  }
  else
  {
    ll->Init(1);
    ll->m[0].rtyp=INT_CMD;    ll->m[0].data=(void *)(long)solvable;
  }

  res->data=(char*)ll;
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
      (*iv)[i]=(int)(long)h->Data();
    }
    else if (h->Typ()==INTVEC_CMD)
    {
      intvec *ivv=(intvec*)h->Data();
      for(int j=0;j<ivv->length();j++,i++)
      {
        (*iv)[i]=(*ivv)[j];
      }
      i--;
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
  short t1[]={4,POLY_CMD,POLY_CMD,POLY_CMD,INTVEC_CMD};
  short t2[]={4,VECTOR_CMD,POLY_CMD,POLY_CMD,INTVEC_CMD};
  short t3[]={4,IDEAL_CMD,MATRIX_CMD,INT_CMD,INTVEC_CMD};
  short t4[]={4,MODUL_CMD,MATRIX_CMD,INT_CMD,INTVEC_CMD};
  leftv u1=u;
  leftv u2=u1->next;
  leftv u3=u2->next;
  leftv u4=u3->next;
  if (iiCheckTypes(u,t1)||iiCheckTypes(u,t2))
  {
    if(!pIsUnit((poly)u2->Data()))
    {
      WerrorS("2nd argument must be a unit");
      return TRUE;
    }
    res->rtyp=u1->Typ();
    res->data=(char*)pSeries((int)(long)u3->Data(),pCopy((poly)u1->Data()),
                             pCopy((poly)u2->Data()),(intvec*)u4->Data());
    return FALSE;
  }
  else
  if (iiCheckTypes(u,t3)||iiCheckTypes(u,t4))
  {
    if(!mp_IsDiagUnit((matrix)u2->Data(), currRing))
    {
      WerrorS("2nd argument must be a diagonal matrix of units");
      return TRUE;
    }
    res->rtyp=u1->Typ();
    res->data=(char*)idSeries(
                              (int)(long)u3->Data(),
                              idCopy((ideal)u1->Data()),
                              mp_Copy((matrix)u2->Data(), currRing),
                              (intvec*)u4->Data()
                             );
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
BOOLEAN jjLIST_PL(leftv res, leftv v)
{
  int sl=0;
  if (v!=NULL) sl = v->listLength();
  lists L;
  if((sl==1)&&(v->Typ()==RESOLUTION_CMD))
  {
    int add_row_shift = 0;
    intvec *weights=(intvec*)atGet(v,"isHomog",INTVEC_CMD);
    if (weights!=NULL)  add_row_shift=weights->min_in();
    L=syConvRes((syStrategy)v->Data(),FALSE,add_row_shift);
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
      if (h!=NULL)
      { /* e.g. not in the first step:
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
static BOOLEAN jjNAMES0(leftv res, leftv)
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
    Kstd1_deg=(int)(long)u3->Data();
    kModW=(intvec *)u4->Data();
    BITSET save2;
    SI_SAVE_OPT2(save2);
    si_opt_2|=Sy_bit(V_DEG_STOP);
    u2->next=NULL;
    BOOLEAN r=jjCALL2ARG(res,u);
    kModW=NULL;
    Kstd1_deg=save_d;
    SI_RESTORE_OPT2(save2);
    u->next->next=u3;
    return r;
  }
  else
  if((u1->Typ()==IDEAL_CMD)&&(u2->Typ()==MATRIX_CMD)&&(u3->Typ()==IDEAL_CMD)&&
     (u4->Typ()==INT_CMD))
  {
    assumeStdFlag(u3);
    if(!mp_IsDiagUnit((matrix)u2->Data(), currRing))
    {
      WerrorS("2nd argument must be a diagonal matrix of units");
      return TRUE;
    }
    res->rtyp=IDEAL_CMD;
    res->data=(char*)redNF(
                           idCopy((ideal)u3->Data()),
                           idCopy((ideal)u1->Data()),
                           mp_Copy((matrix)u2->Data(), currRing),
                           (int)(long)u4->Data()
                          );
    return FALSE;
  }
  else
  if((u1->Typ()==POLY_CMD)&&(u2->Typ()==POLY_CMD)&&(u3->Typ()==IDEAL_CMD)&&
     (u4->Typ()==INT_CMD))
  {
    assumeStdFlag(u3);
    if(!pIsUnit((poly)u2->Data()))
    {
      WerrorS("2nd argument must be a unit");
      return TRUE;
    }
    res->rtyp=POLY_CMD;
    res->data=(char*)redNF(idCopy((ideal)u3->Data()),pCopy((poly)u1->Data()),
                           pCopy((poly)u2->Data()),(int)(long)u4->Data());
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
    if(!mp_IsDiagUnit((matrix)u2->Data(), currRing))
    {
      WerrorS("2nd argument must be a diagonal matrix of units");
      return TRUE;
    }
    res->rtyp=IDEAL_CMD;
    res->data=(char*)redNF(
                           idCopy((ideal)u3->Data()),
                           idCopy((ideal)u1->Data()),
                           mp_Copy((matrix)u2->Data(),currRing),
                           (int)(long)u4->Data(),
                           (intvec*)u5->Data()
                          );
    return FALSE;
  }
  else
  if((u1->Typ()==POLY_CMD)&&(u2->Typ()==POLY_CMD)&&(u3->Typ()==IDEAL_CMD)&&
     (u4->Typ()==INT_CMD)&&(u5->Typ()==INTVEC_CMD))
  {
    assumeStdFlag(u3);
    if(!pIsUnit((poly)u2->Data()))
    {
      WerrorS("2nd argument must be a unit");
      return TRUE;
    }
    res->rtyp=POLY_CMD;
    res->data=(char*)redNF(idCopy((ideal)u3->Data()),pCopy((poly)u1->Data()),
                           pCopy((poly)u2->Data()),
                           (int)(long)u4->Data(),(intvec*)u5->Data());
    return FALSE;
  }
  else
  {
    Werror("%s(`ideal`,`ideal`,`matrix`,`int`,`intvec`) exppected",
           Tok2Cmdname(iiOp));
    return TRUE;
  }
}
static BOOLEAN jjRESERVED0(leftv, leftv)
{
  int i=1;
  int nCount = (sArithBase.nCmdUsed-1)/3;
  if((3*nCount)<sArithBase.nCmdUsed) nCount++;
  //Print("CMDS: %d/%d\n", sArithBase.nCmdUsed,
  //      sArithBase.nCmdAllocated);
  for(i=0; i<nCount; i++)
  {
    Print("%-20s",sArithBase.sCmds[i+1].name);
    if(i+1+nCount<sArithBase.nCmdUsed)
      Print("%-20s",sArithBase.sCmds[i+1+nCount].name);
    if(i+1+2*nCount<sArithBase.nCmdUsed)
      Print("%-20s",sArithBase.sCmds[i+1+2*nCount].name);
    //if ((i%3)==1) PrintLn();
    PrintLn();
  }
  PrintLn();
  printBlackboxTypes();
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
static BOOLEAN jjTEST(leftv, leftv v)
{
  do
  {
    if (v->Typ()!=INT_CMD)
      return TRUE;
    test_cmd((int)(long)v->Data());
    v=v->next;
  }
  while (v!=NULL);
  return FALSE;
}

#if defined(__alpha) && !defined(linux)
extern "C"
{
  void usleep(unsigned long usec);
};
#endif
static BOOLEAN jjFactModD_M(leftv res, leftv v)
{
  /* compute two factors of h(x,y) modulo x^(d+1) in K[[x]][y],
     see a detailed documentation in /kernel/linear_algebra/linearAlgebra.h

     valid argument lists:
     - (poly h, int d),
     - (poly h, int d, poly f0, poly g0),       optional: factors of h(0,y),
     - (poly h, int d, int xIndex, int yIndex), optional: indices of vars x & y
                                                          in list of ring vars,
     - (poly h, int d, poly f0, poly g0, int xIndex, int yIndec),
                                                optional: all 4 optional args
     (The defaults are xIndex = 1, yIndex = 2, f0 and g0 polynomials as found
      by singclap_factorize and h(0, y)
      has exactly two distinct monic factors [possibly with exponent > 1].)
     result:
     - list with the two factors f and g such that
       h(x,y) = f(x,y)*g(x,y) mod x^(d+1)   */

  poly h      = NULL;
  int  d      =    1;
  poly f0     = NULL;
  poly g0     = NULL;
  int  xIndex =    1;   /* default index if none provided */
  int  yIndex =    2;   /* default index if none provided */

  leftv u = v; int factorsGiven = 0;
  if ((u == NULL) || (u->Typ() != POLY_CMD))
  {
    WerrorS("expected arguments (poly, int [, poly, poly] [, int, int])");
    return TRUE;
  }
  else h = (poly)u->Data();
  u = u->next;
  if ((u == NULL) || (u->Typ() != INT_CMD))
  {
    WerrorS("expected arguments (poly, int [, poly, poly] [, int, int])");
    return TRUE;
  }
  else d = (int)(long)u->Data();
  u = u->next;
  if ((u != NULL) && (u->Typ() == POLY_CMD))
  {
    if ((u->next == NULL) || (u->next->Typ() != POLY_CMD))
    {
      WerrorS("expected arguments (poly, int [, poly, poly] [, int, int])");
      return TRUE;
    }
    else
    {
      f0 = (poly)u->Data();
      g0 = (poly)u->next->Data();
      factorsGiven = 1;
      u = u->next->next;
    }
  }
  if ((u != NULL) && (u->Typ() == INT_CMD))
  {
    if ((u->next == NULL) || (u->next->Typ() != INT_CMD))
    {
      WerrorS("expected arguments (poly, int [, poly, poly] [, int, int])");
      return TRUE;
    }
    else
    {
      xIndex = (int)(long)u->Data();
      yIndex = (int)(long)u->next->Data();
      u = u->next->next;
    }
  }
  if (u != NULL)
  {
    WerrorS("expected arguments (poly, int [, poly, poly] [, int, int])");
    return TRUE;
  }

  /* checks for provided arguments */
  if (pIsConstant(h) || (factorsGiven && (pIsConstant(f0) || pIsConstant(g0))))
  {
    WerrorS("expected non-constant polynomial argument(s)");
    return TRUE;
  }
  int n = rVar(currRing);
  if ((xIndex < 1) || (n < xIndex))
  {
    Werror("index for variable x (%d) out of range [1..%d]", xIndex, n);
    return TRUE;
  }
  if ((yIndex < 1) || (n < yIndex))
  {
    Werror("index for variable y (%d) out of range [1..%d]", yIndex, n);
    return TRUE;
  }
  if (xIndex == yIndex)
  {
    WerrorS("expected distinct indices for variables x and y");
    return TRUE;
  }

  /* computation of f0 and g0 if missing */
  if (factorsGiven == 0)
  {
    poly h0 = pSubst(pCopy(h), xIndex, NULL);
    intvec* v = NULL;
    ideal i = singclap_factorize(h0, &v, 0,currRing);

    ivTest(v);

    if (i == NULL) return TRUE;

    idTest(i);

    if ((v->rows() != 3) || ((*v)[0] =! 1) || (!nIsOne(pGetCoeff(i->m[0]))))
    {
      WerrorS("expected h(0,y) to have exactly two distinct monic factors");
      return TRUE;
    }
    f0 = pPower(pCopy(i->m[1]), (*v)[1]);
    g0 = pPower(pCopy(i->m[2]), (*v)[2]);
    idDelete(&i);
  }

  poly f; poly g;
  henselFactors(xIndex, yIndex, h, f0, g0, d, f, g);
  lists L = (lists)omAllocBin(slists_bin);
  L->Init(2);
  L->m[0].rtyp = POLY_CMD; L->m[0].data=(void*)f;
  L->m[1].rtyp = POLY_CMD; L->m[1].data=(void*)g;
  res->rtyp = LIST_CMD;
  res->data = (char*)L;
  return FALSE;
}
static BOOLEAN jjSTATUS_M(leftv res, leftv v)
{
  if ((v->Typ() != LINK_CMD) ||
      (v->next->Typ() != STRING_CMD) ||
      (v->next->next->Typ() != STRING_CMD) ||
      (v->next->next->next->Typ() != INT_CMD))
    return TRUE;
  jjSTATUS3(res, v, v->next, v->next->next);
#if defined(HAVE_USLEEP)
  if (((long) res->data) == 0L)
  {
    int i_s = (int)(long) v->next->next->next->Data();
    if (i_s > 0)
    {
      usleep((int)(long) v->next->next->next->Data());
      jjSTATUS3(res, v, v->next, v->next->next);
    }
  }
#elif defined(HAVE_SLEEP)
  if (((int) res->data) == 0)
  {
    int i_s = (int) v->next->next->next->Data();
    if (i_s > 0)
    {
      si_sleep((is - 1)/1000000 + 1);
      jjSTATUS3(res, v, v->next, v->next->next);
    }
  }
#endif
  return FALSE;
}
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
static BOOLEAN jjQRDS(leftv res, leftv INPUT)
{
  if ((INPUT->Typ() != MATRIX_CMD) ||
      (INPUT->next->Typ() != NUMBER_CMD) ||
      (INPUT->next->next->Typ() != NUMBER_CMD) ||
      (INPUT->next->next->next->Typ() != NUMBER_CMD))
  {
    WerrorS("expected (matrix, number, number, number) as arguments");
    return TRUE;
  }
  leftv u = INPUT; leftv v = u->next; leftv w = v->next; leftv x = w->next;
  res->data = (char *)qrDoubleShift((matrix)(u->Data()),
                                    (number)(v->Data()),
                                    (number)(w->Data()),
                                    (number)(x->Data()));
  return FALSE;
}
static BOOLEAN jjSTD_HILB_WP(leftv res, leftv INPUT)
{ ideal result;
  leftv u = INPUT;    /* an ideal, weighted homogeneous and standard */
  leftv v = u->next;  /* one additional polynomial or ideal */
  leftv h = v->next;  /* Hilbert vector */
  leftv w = h->next;  /* weight vector */
  assumeStdFlag(u);
  ideal i1=(ideal)(u->Data());
  ideal i0;
  if (((u->Typ()!=IDEAL_CMD)&&(u->Typ()!=MODUL_CMD))
  || (h->Typ()!=INTVEC_CMD)
  || (w->Typ()!=INTVEC_CMD))
  {
    WerrorS("expected `std(`ideal/module`,`poly/vector`,`intvec`,`intvec`)");
    return TRUE;
  }
  intvec *vw=(intvec *)w->Data(); // weights of vars
  /* merging std_hilb_w and std_1 */
  if (vw->length()!=currRing->N)
  {
    Werror("%d weights for %d variables",vw->length(),currRing->N);
    return TRUE;
  }
  int r=v->Typ();
  BOOLEAN cleanup_i0=FALSE;
  if ((r==POLY_CMD) ||(r==VECTOR_CMD))
  {
    i0=idInit(1,i1->rank);
    i0->m[0]=(poly)v->Data();
    cleanup_i0=TRUE;
  }
  else if (r==IDEAL_CMD)/* IDEAL */
  {
    i0=(ideal)v->Data();
  }
  else
  {
    WerrorS("expected `std(`ideal/module`,`poly/vector`,`intvec`,`intvec`)");
    return TRUE;
  }
  int ii0=idElem(i0);
  i1 = idSimpleAdd(i1,i0);
  if (cleanup_i0)
  {
    memset(i0->m,0,sizeof(poly)*IDELEMS(i0));
    idDelete(&i0);
  }
  intvec *ww=(intvec *)atGet(u,"isHomog",INTVEC_CMD);
  tHomog hom=testHomog;
  /* u_id from jjSTD_W is now i1 as in jjSTD_1 */
  if (ww!=NULL)
  {
    if (!idTestHomModule(i1,currRing->qideal,ww))
    {
      WarnS("wrong weights");
      ww=NULL;
    }
    else
    {
      ww=ivCopy(ww);
      hom=isHomog;
    }
  }
  BITSET save1;
  SI_SAVE_OPT1(save1);
  si_opt_1|=Sy_bit(OPT_SB_1);
  result=kStd(i1,
              currRing->qideal,
              hom,
              &ww,                  // module weights
              (intvec *)h->Data(),  // hilbert series
              0,                    // syzComp, whatever it is...
              IDELEMS(i1)-ii0,      // new ideal
              vw);                  // weights of vars
  SI_RESTORE_OPT1(save1);
  idDelete(&i1);
  idSkipZeroes(result);
  res->data = (char *)result;
  if (!TEST_OPT_DEGBOUND) setFlag(res,FLAG_STD);
  if (ww!=NULL) atSet(res,omStrDup("isHomog"),ww,INTVEC_CMD);
  return FALSE;
}


static Subexpr jjMakeSub(leftv e)
{
  assume( e->Typ()==INT_CMD );
  Subexpr r=(Subexpr)omAlloc0Bin(sSubexpr_bin);
  r->start =(int)(long)e->Data();
  return r;
}
#define D(A)    (A)
#define NULL_VAL NULL
#define IPARITH
#include "table.h"

#include "iparith.inc"

/*=================== operations with 2 args. ============================*/
/* must be ordered: first operations for chars (infix ops),
 * then alphabetically */

static BOOLEAN iiExprArith2TabIntern(leftv res, leftv a, int op, leftv b,
                                    BOOLEAN proccall,
                                    struct sValCmd2* dA2,
                                    int at, int bt,
                                    struct sConvertTypes *dConvertTypes)
{
  memset(res,0,sizeof(sleftv));
  BOOLEAN call_failed=FALSE;

  if (!errorreported)
  {
    int i=0;
    iiOp=op;
    while (dA2[i].cmd==op)
    {
      if ((at==dA2[i].arg1)
      && (bt==dA2[i].arg2))
      {
        res->rtyp=dA2[i].res;
        if (currRing!=NULL)
        {
          if (check_valid(dA2[i].valid_for,op)) break;
        }
        else
        {
          if (RingDependend(dA2[i].res))
          {
            WerrorS("no ring active");
            break;
          }
        }
        if (traceit&TRACE_CALL)
          Print("call %s(%s,%s)\n",iiTwoOps(op),Tok2Cmdname(at),Tok2Cmdname(bt));
        if ((call_failed=dA2[i].p(res,a,b)))
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
    if (dA2[i].cmd!=op)
    {
      int ai,bi;
      leftv an = (leftv)omAlloc0Bin(sleftv_bin);
      leftv bn = (leftv)omAlloc0Bin(sleftv_bin);
      BOOLEAN failed=FALSE;
      i=0; /*iiTabIndex(dArithTab2,JJTAB2LEN,op);*/
      //Print("op: %c, type: %s %s\n",op,Tok2Cmdname(at),Tok2Cmdname(bt));
      while (dA2[i].cmd==op)
      {
        //Print("test %s %s\n",Tok2Cmdname(dA2[i].arg1),Tok2Cmdname(dA2[i].arg2));
        if ((ai=iiTestConvert(at,dA2[i].arg1))!=0)
        {
          if ((bi=iiTestConvert(bt,dA2[i].arg2))!=0)
          {
            res->rtyp=dA2[i].res;
            if (currRing!=NULL)
            {
              if (check_valid(dA2[i].valid_for,op)) break;
            }
            else
            {
              if (RingDependend(dA2[i].res))
              {
                WerrorS("no ring active");
                break;
              }
            }
            if (traceit&TRACE_CALL)
              Print("call %s(%s,%s)\n",iiTwoOps(op),
              Tok2Cmdname(dA2[i].arg1),Tok2Cmdname(dA2[i].arg2));
            failed= ((iiConvert(at,dA2[i].arg1,ai,a,an))
            || (iiConvert(bt,dA2[i].arg2,bi,b,bn))
            || (call_failed=dA2[i].p(res,an,bn)));
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
        i=0; /*iiTabIndex(dArithTab2,JJTAB2LEN,op);*/
        s = iiTwoOps(op);
        if (proccall)
        {
          Werror("%s(`%s`,`%s`) failed"
                ,s,Tok2Cmdname(at),Tok2Cmdname(bt));
        }
        else
        {
          Werror("`%s` %s `%s` failed"
                ,Tok2Cmdname(at),s,Tok2Cmdname(bt));
        }
        if ((!call_failed) && BVERBOSE(V_SHOW_USE))
        {
          while (dA2[i].cmd==op)
          {
            if(((at==dA2[i].arg1)||(bt==dA2[i].arg2))
            && (dA2[i].res!=0)
            && (dA2[i].p!=jjWRONG2))
            {
              if (proccall)
                Werror("expected %s(`%s`,`%s`)"
                  ,s,Tok2Cmdname(dA2[i].arg1),Tok2Cmdname(dA2[i].arg2));
              else
                Werror("expected `%s` %s `%s`"
                  ,Tok2Cmdname(dA2[i].arg1),s,Tok2Cmdname(dA2[i].arg2));
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
  return TRUE;
}
BOOLEAN iiExprArith2Tab(leftv res, leftv a, int op,
                                    struct sValCmd2* dA2,
                                    int at,
                                    struct sConvertTypes *dConvertTypes)
{
  leftv b=a->next;
  a->next=NULL;
  int bt=b->Typ();
  BOOLEAN bo=iiExprArith2TabIntern(res,a,op,b,TRUE,dA2,at,bt,dConvertTypes);
  a->next=b;
  a->CleanUp();
  return bo;
}
BOOLEAN iiExprArith2(leftv res, leftv a, int op, leftv b, BOOLEAN proccall)
{
  memset(res,0,sizeof(sleftv));
  BOOLEAN call_failed=FALSE;

  if (!errorreported)
  {
#ifdef SIQ
    if (siq>0)
    {
      //Print("siq:%d\n",siq);
      command d=(command)omAlloc0Bin(sip_command_bin);
      memcpy(&d->arg1,a,sizeof(sleftv));
      //a->Init();
      memcpy(&d->arg2,b,sizeof(sleftv));
      //b->Init();
      d->argc=2;
      d->op=op;
      res->data=(char *)d;
      res->rtyp=COMMAND;
      return FALSE;
    }
#endif
    int at=a->Typ();
    int bt=b->Typ();
    // handling bb-objects ----------------------------------------------------
    if (at>MAX_TOK)
    {
      blackbox *bb=getBlackboxStuff(at);
      if (bb!=NULL)
      {
        if (!bb->blackbox_Op2(op,res,a,b)) return FALSE;
        if (errorreported) return TRUE;
        // else: no op defined
      }
      else          return TRUE;
    }
    else if ((bt>MAX_TOK)&&(op!='('))
    {
      blackbox *bb=getBlackboxStuff(bt);
      if (bb!=NULL)
      {
        if(!bb->blackbox_Op2(op,res,a,b)) return FALSE;
        if (errorreported) return TRUE;
        // else: no op defined
      }
      else          return TRUE;
    }
    int i=iiTabIndex(dArithTab2,JJTAB2LEN,op);
    return iiExprArith2TabIntern(res,a,op,b,proccall,dArith2+i,at,bt,dConvertTypes);
  }
  a->CleanUp();
  b->CleanUp();
  return TRUE;
}

/*==================== operations with 1 arg. ===============================*/
/* must be ordered: first operations for chars (infix ops),
 * then alphabetically */

BOOLEAN iiExprArith1Tab(leftv res, leftv a, int op, struct sValCmd1* dA1, int at, struct sConvertTypes *dConvertTypes)
{
  memset(res,0,sizeof(sleftv));
  BOOLEAN call_failed=FALSE;

  if (!errorreported)
  {
    BOOLEAN failed=FALSE;
    iiOp=op;
    int i = 0;
    while (dA1[i].cmd==op)
    {
      if (at==dA1[i].arg)
      {
        if (currRing!=NULL)
        {
          if (check_valid(dA1[i].valid_for,op)) break;
        }
        else
        {
          if (RingDependend(dA1[i].res))
          {
            WerrorS("no ring active");
            break;
          }
        }
        if (traceit&TRACE_CALL)
          Print("call %s(%s)\n",iiTwoOps(op),Tok2Cmdname(at));
        res->rtyp=dA1[i].res;
        if ((call_failed=dA1[i].p(res,a)))
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
    if (dA1[i].cmd!=op)
    {
      leftv an = (leftv)omAlloc0Bin(sleftv_bin);
      i=0;
      //Print("fuer %c , typ: %s\n",op,Tok2Cmdname(at));
      while (dA1[i].cmd==op)
      {
        int ai;
        //Print("test %s\n",Tok2Cmdname(dA1[i].arg));
        if ((ai=iiTestConvert(at,dA1[i].arg,dConvertTypes))!=0)
        {
          if (currRing!=NULL)
          {
            if (check_valid(dA1[i].valid_for,op)) break;
          }
          else
          {
            if (RingDependend(dA1[i].res))
            {
              WerrorS("no ring active");
              break;
            }
          }
          if (traceit&TRACE_CALL)
            Print("call %s(%s)\n",iiTwoOps(op),Tok2Cmdname(dA1[i].arg));
          res->rtyp=dA1[i].res;
          failed= ((iiConvert(at,dA1[i].arg,ai,a,an,dConvertTypes))
          || (call_failed=dA1[i].p(res,an)));
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
        i=0;
        const char *s = iiTwoOps(op);
        Werror("%s(`%s`) failed"
                ,s,Tok2Cmdname(at));
        if ((!call_failed) && BVERBOSE(V_SHOW_USE))
        {
          while (dA1[i].cmd==op)
          {
            if ((dA1[i].res!=0)
            && (dA1[i].p!=jjWRONG))
              Werror("expected %s(`%s`)"
                ,s,Tok2Cmdname(dA1[i].arg));
            i++;
          }
        }
      }
    }
    res->rtyp = UNKNOWN;
  }
  a->CleanUp();
  return TRUE;
}
BOOLEAN iiExprArith1(leftv res, leftv a, int op)
{
  memset(res,0,sizeof(sleftv));
  BOOLEAN call_failed=FALSE;

  if (!errorreported)
  {
#ifdef SIQ
    if (siq>0)
    {
      //Print("siq:%d\n",siq);
      command d=(command)omAlloc0Bin(sip_command_bin);
      memcpy(&d->arg1,a,sizeof(sleftv));
      //a->Init();
      d->op=op;
      d->argc=1;
      res->data=(char *)d;
      res->rtyp=COMMAND;
      return FALSE;
    }
#endif
    int at=a->Typ();
    // handling bb-objects ----------------------------------------------------
    if (at>MAX_TOK)
    {
      blackbox *bb=getBlackboxStuff(at);
      if (bb!=NULL)
      {
        if(!bb->blackbox_Op1(op,res,a)) return FALSE;
        if (errorreported) return TRUE;
        // else: no op defined
      }
      else          return TRUE;
    }

    BOOLEAN failed=FALSE;
    iiOp=op;
    int i=iiTabIndex(dArithTab1,JJTAB1LEN,op);
    return iiExprArith1Tab(res,a,op, dArith1+i,at,dConvertTypes);
  }
  a->CleanUp();
  return TRUE;
}

/*=================== operations with 3 args. ============================*/
/* must be ordered: first operations for chars (infix ops),
 * then alphabetically */

static BOOLEAN iiExprArith3TabIntern(leftv res, int op, leftv a, leftv b, leftv c,
  struct sValCmd3* dA3, int at, int bt, int ct,
  struct sConvertTypes *dConvertTypes)
{
  memset(res,0,sizeof(sleftv));
  BOOLEAN call_failed=FALSE;

  assume(dA3[0].cmd==op);

  if (!errorreported)
  {
    int i=0;
    iiOp=op;
    while (dA3[i].cmd==op)
    {
      if ((at==dA3[i].arg1)
      && (bt==dA3[i].arg2)
      && (ct==dA3[i].arg3))
      {
        res->rtyp=dA3[i].res;
        if (currRing!=NULL)
        {
          if (check_valid(dA3[i].valid_for,op)) break;
        }
        if (traceit&TRACE_CALL)
          Print("call %s(%s,%s,%s)\n",
            iiTwoOps(op),Tok2Cmdname(at),Tok2Cmdname(bt),Tok2Cmdname(ct));
        if ((call_failed=dA3[i].p(res,a,b,c)))
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
    if (dA3[i].cmd!=op)
    {
      int ai,bi,ci;
      leftv an = (leftv)omAlloc0Bin(sleftv_bin);
      leftv bn = (leftv)omAlloc0Bin(sleftv_bin);
      leftv cn = (leftv)omAlloc0Bin(sleftv_bin);
      BOOLEAN failed=FALSE;
      i=0;
      //while ((dA3[i].cmd!=op)&&(dA3[i].cmd!=0)) i++;
      while (dA3[i].cmd==op)
      {
        if ((ai=iiTestConvert(at,dA3[i].arg1))!=0)
        {
          if ((bi=iiTestConvert(bt,dA3[i].arg2))!=0)
          {
            if ((ci=iiTestConvert(ct,dA3[i].arg3))!=0)
            {
              res->rtyp=dA3[i].res;
              if (currRing!=NULL)
              {
                if (check_valid(dA3[i].valid_for,op)) break;
              }
              if (traceit&TRACE_CALL)
                Print("call %s(%s,%s,%s)\n",
                  iiTwoOps(op),Tok2Cmdname(dA3[i].arg1),
                  Tok2Cmdname(dA3[i].arg2),Tok2Cmdname(dA3[i].arg3));
              failed= ((iiConvert(at,dA3[i].arg1,ai,a,an))
                || (iiConvert(bt,dA3[i].arg2,bi,b,bn))
                || (iiConvert(ct,dA3[i].arg3,ci,c,cn))
                || (call_failed=dA3[i].p(res,an,bn,cn)));
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
        //while ((dA3[i].cmd!=op)&&(dA3[i].cmd!=0)) i++;
        const char *s = iiTwoOps(op);
        Werror("%s(`%s`,`%s`,`%s`) failed"
                ,s,Tok2Cmdname(at),Tok2Cmdname(bt),Tok2Cmdname(ct));
        if ((!call_failed) && BVERBOSE(V_SHOW_USE))
        {
          while (dA3[i].cmd==op)
          {
            if(((at==dA3[i].arg1)
            ||(bt==dA3[i].arg2)
            ||(ct==dA3[i].arg3))
            && (dA3[i].res!=0))
            {
              Werror("expected %s(`%s`,`%s`,`%s`)"
                  ,s,Tok2Cmdname(dA3[i].arg1)
                  ,Tok2Cmdname(dA3[i].arg2)
                  ,Tok2Cmdname(dA3[i].arg3));
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
  return TRUE;
}
BOOLEAN iiExprArith3(leftv res, int op, leftv a, leftv b, leftv c)
{
  memset(res,0,sizeof(sleftv));
  BOOLEAN call_failed=FALSE;

  if (!errorreported)
  {
#ifdef SIQ
    if (siq>0)
    {
      //Print("siq:%d\n",siq);
      command d=(command)omAlloc0Bin(sip_command_bin);
      memcpy(&d->arg1,a,sizeof(sleftv));
      //a->Init();
      memcpy(&d->arg2,b,sizeof(sleftv));
      //b->Init();
      memcpy(&d->arg3,c,sizeof(sleftv));
      //c->Init();
      d->op=op;
      d->argc=3;
      res->data=(char *)d;
      res->rtyp=COMMAND;
      return FALSE;
    }
#endif
    int at=a->Typ();
    // handling bb-objects ----------------------------------------------
    if (at>MAX_TOK)
    {
      blackbox *bb=getBlackboxStuff(at);
      if (bb!=NULL)
      {
        if(!bb->blackbox_Op3(op,res,a,b,c)) return FALSE;
        if (errorreported) return TRUE;
        // else: no op defined
      }
      else          return TRUE;
      if (errorreported) return TRUE;
    }
    int bt=b->Typ();
    int ct=c->Typ();

    iiOp=op;
    int i=0;
    while ((dArith3[i].cmd!=op)&&(dArith3[i].cmd!=0)) i++;
    return iiExprArith3TabIntern(res,op,a,b,c,dArith3+i,at,bt,ct,dConvertTypes);
  }
  a->CleanUp();
  b->CleanUp();
  c->CleanUp();
        //Print("op: %d,result typ:%d\n",op,res->rtyp);
  return TRUE;
}
BOOLEAN iiExprArith3Tab(leftv res, leftv a, int op,
                                    struct sValCmd3* dA3,
                                    int at,
                                    struct sConvertTypes *dConvertTypes)
{
  leftv b=a->next;
  a->next=NULL;
  int bt=b->Typ();
  leftv c=b->next;
  b->next=NULL;
  int ct=c->Typ();
  BOOLEAN bo=iiExprArith3TabIntern(res,op,a,b,c,dA3,at,bt,ct,dConvertTypes);
  b->next=c;
  a->next=b;
  a->CleanUp();
  return bo;
}
/*==================== operations with many arg. ===============================*/
/* must be ordered: first operations for chars (infix ops),
 * then alphabetically */

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
  memset(res,0,sizeof(sleftv));
  BOOLEAN bo;

  if (!errorreported)
  {
#ifdef SIQ
    if (siq>0)
    {
      //Print("siq:%d\n",siq);
      command d=(command)omAlloc0Bin(sip_command_bin);
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
            a->next->next->Init();
            /* no break */
          case 2:
            memcpy(&d->arg2,a->next,sizeof(sleftv));
            a->next->Init();
            a->next->next=d->arg2.next;
            d->arg2.next=NULL;
            /* no break */
          case 1:
            a->Init();
            a->next=d->arg1.next;
            d->arg1.next=NULL;
        }
        if (d->argc>3) a->next=NULL;
        a->name=NULL;
        a->rtyp=0;
        a->data=NULL;
        a->e=NULL;
        a->attribute=NULL;
        a->CleanUp();
      }
      res->rtyp=COMMAND;
      return FALSE;
    }
#endif
    if ((a!=NULL) && (a->Typ()>MAX_TOK))
    {
      blackbox *bb=getBlackboxStuff(a->Typ());
      if (bb!=NULL)
      {
        if(!bb->blackbox_OpM(op,res,a)) return FALSE;
        if (errorreported) return TRUE;
        // else: no op defined
      }
      else          return TRUE;
    }
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
        if (currRing!=NULL)
        {
          if (check_valid(dArithM[i].valid_for,op)) break;
        }
        if (traceit&TRACE_CALL)
          Print("call %s(... (%d args))\n", iiTwoOps(op),args);
        if ((failed=dArithM[i].p(res,a))==TRUE)
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
        const char *s = iiTwoOps(op);
        Werror("%s(...) failed",s);
      }
    }
    res->rtyp = UNKNOWN;
  }
  if (a!=NULL) a->CleanUp();
        //Print("op: %d,result typ:%d\n",op,res->rtyp);
  return TRUE;
}

/*=================== general utilities ============================*/
int IsCmd(const char *n, int & tok)
{
  int i;
  int an=1;
  int en=sArithBase.nLastIdentifier;

  loop
  //for(an=0; an<sArithBase.nCmdUsed; )
  {
    if(an>=en-1)
    {
      if (strcmp(n, sArithBase.sCmds[an].name) == 0)
      {
        i=an;
        break;
      }
      else if ((an!=en) && (strcmp(n, sArithBase.sCmds[en].name) == 0))
      {
        i=en;
        break;
      }
      else
      {
        // -- blackbox extensions:
        // return 0;
        return blackboxIsCmd(n,tok);
      }
    }
    i=(an+en)/2;
    if (*n < *(sArithBase.sCmds[i].name))
    {
      en=i-1;
    }
    else if (*n > *(sArithBase.sCmds[i].name))
    {
      an=i+1;
    }
    else
    {
      int v=strcmp(n,sArithBase.sCmds[i].name);
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
  lastreserved=sArithBase.sCmds[i].name;
  tok=sArithBase.sCmds[i].tokval;
  if(sArithBase.sCmds[i].alias==2)
  {
    Warn("outdated identifier `%s` used - please change your code",
    sArithBase.sCmds[i].name);
    sArithBase.sCmds[i].alias=1;
  }
  #if 0
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
  #endif
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
  return sArithBase.sCmds[i].toktype;
}
static int iiTabIndex(const jjValCmdTab dArithTab, const int len, const int op)
{
  // user defined types are not in the pre-computed table:
  if (op>MAX_TOK) return 0;

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

  // catch missing a cmd:
  // may be missing as a op for blackbox, if the first operand is "undef" instead of bb
  // Print("op %d (%c) unknown",op,op);
  return 0;
}

const char * Tok2Cmdname(int tok)
{
  if (tok <= 0)
  {
    return sArithBase.sCmds[0].name;
  }
  if (tok==ANY_TYPE) return "any_type";
  if (tok==COMMAND) return "command";
  if (tok==NONE) return "nothing";
  //if (tok==IFBREAK) return "if_break";
  //if (tok==VECTOR_FROM_POLYS) return "vector_from_polys";
  //if (tok==ORDER_VECTOR) return "ordering";
  //if (tok==REF_VAR) return "ref";
  //if (tok==OBJECT) return "object";
  //if (tok==PRINT_EXPR) return "print_expr";
  if (tok==IDHDL) return "identifier";
  if (tok==CRING_CMD) return "(c)ring";
  if (tok>MAX_TOK) return getBlackboxName(tok);
  int i;
  for(i=0; i<sArithBase.nCmdUsed; i++)
    //while (sArithBase.sCmds[i].tokval!=0)
  {
    if ((sArithBase.sCmds[i].tokval == tok)&&
        (sArithBase.sCmds[i].alias==0))
    {
      return sArithBase.sCmds[i].name;
    }
  }
  // try gain for alias/old names:
  for(i=0; i<sArithBase.nCmdUsed; i++)
  {
    if (sArithBase.sCmds[i].tokval == tok)
    {
      return sArithBase.sCmds[i].name;
    }
  }
  return sArithBase.sCmds[0].name;
}


/*---------------------------------------------------------------------*/
/**
 * @brief compares to entry of cmdsname-list

 @param[in] a
 @param[in] b

 @return <ReturnValue>
**/
/*---------------------------------------------------------------------*/
static int _gentable_sort_cmds( const void *a, const void *b )
{
  cmdnames *pCmdL = (cmdnames*)a;
  cmdnames *pCmdR = (cmdnames*)b;

  if(a==NULL || b==NULL)             return 0;

  /* empty entries goes to the end of the list for later reuse */
  if(pCmdL->name==NULL) return 1;
  if(pCmdR->name==NULL) return -1;

  /* $INVALID$ must come first */
  if(strcmp(pCmdL->name, "$INVALID$")==0) return -1;
  if(strcmp(pCmdR->name, "$INVALID$")==0) return  1;

  /* tokval=-1 are reserved names at the end */
  if (pCmdL->tokval==-1)
  {
    if (pCmdR->tokval==-1)
       return strcmp(pCmdL->name, pCmdR->name);
    /* pCmdL->tokval==-1, pCmdL goes at the end */
    return 1;
  }
  /* pCmdR->tokval==-1, pCmdR goes at the end */
  if(pCmdR->tokval==-1) return -1;

  return strcmp(pCmdL->name, pCmdR->name);
}

/*---------------------------------------------------------------------*/
/**
 * @brief initialisation of arithmetic structured data

 @retval 0 on success

**/
/*---------------------------------------------------------------------*/
int iiInitArithmetic()
{
  //printf("iiInitArithmetic()\n");
  memset(&sArithBase, 0, sizeof(sArithBase));
  iiInitCmdName();
  /* fix last-identifier */
#if 0
  /* we expect that gentable allready did every thing */
  for(sArithBase.nLastIdentifier=sArithBase.nCmdUsed-1;
      sArithBase.nLastIdentifier>0; sArithBase.nLastIdentifier--) {
    if(sArithBase.sCmds[sArithBase.nLastIdentifier].tokval>=0) break;
  }
#endif
  //Print("L=%d\n", sArithBase.nLastIdentifier);

  //iiArithAddCmd(szName, nAlias, nTokval, nToktype);
  //iiArithAddCmd("mygcd", 1, GCD_CMD, CMD_2);

  //iiArithAddCmd("Top", 0,-1,0);


  //for(i=0; i<sArithBase.nCmdUsed; i++) {
  //  printf("CMD[%03d] %s, %d, %d, %d\n", i,
  //         sArithBase.sCmds[i].name,
  //         sArithBase.sCmds[i].alias,
  //         sArithBase.sCmds[i].tokval,
  //         sArithBase.sCmds[i].toktype);
  //}
  //iiArithRemoveCmd("Top");
  //iiArithAddCmd("mygcd", 2, GCD_CMD, CMD_2);
  //iiArithRemoveCmd("mygcd");
  //iiArithAddCmd("kkk", 1, 1234, CMD_1);
  return 0;
}

int iiArithFindCmd(const char *szName)
{
  int an=0;
  int i = 0,v = 0;
  int en=sArithBase.nLastIdentifier;

  loop
  //for(an=0; an<sArithBase.nCmdUsed; )
  {
    if(an>=en-1)
    {
      if (strcmp(szName, sArithBase.sCmds[an].name) == 0)
      {
        //Print("RET-an=%d %s\n", an, sArithBase.sCmds[an].name);
        return an;
      }
      else if (strcmp(szName, sArithBase.sCmds[en].name) == 0)
      {
        //Print("RET-en=%d %s\n", en, sArithBase.sCmds[en].name);
        return en;
      }
      else
      {
        //Print("RET- 1\n");
        return -1;
      }
    }
    i=(an+en)/2;
    if (*szName < *(sArithBase.sCmds[i].name))
    {
      en=i-1;
    }
    else if (*szName > *(sArithBase.sCmds[i].name))
    {
      an=i+1;
    }
    else
    {
      v=strcmp(szName,sArithBase.sCmds[i].name);
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
        //Print("RET-i=%d %s\n", i, sArithBase.sCmds[i].name);
        return i;
      }
    }
  }
  //if(i>=0 && i<sArithBase.nCmdUsed)
  //  return i;
  //Print("RET-2\n");
  return -2;
}

char *iiArithGetCmd( int nPos )
{
  if(nPos<0) return NULL;
  if(nPos<sArithBase.nCmdUsed)
    return sArithBase.sCmds[nPos].name;
  return NULL;
}

int iiArithRemoveCmd(const char *szName)
{
  int nIndex;
  if(szName==NULL) return -1;

  nIndex = iiArithFindCmd(szName);
  if(nIndex<0 || nIndex>=sArithBase.nCmdUsed)
  {
    Print("'%s' not found (%d)\n", szName, nIndex);
    return -1;
  }
  omFree(sArithBase.sCmds[nIndex].name);
  sArithBase.sCmds[nIndex].name=NULL;
  qsort(sArithBase.sCmds, sArithBase.nCmdUsed, sizeof(cmdnames),
        (&_gentable_sort_cmds));
  sArithBase.nCmdUsed--;

  /* fix last-identifier */
  for(sArithBase.nLastIdentifier=sArithBase.nCmdUsed-1;
      sArithBase.nLastIdentifier>0; sArithBase.nLastIdentifier--)
  {
    if(sArithBase.sCmds[sArithBase.nLastIdentifier].tokval>=0) break;
  }
  //Print("L=%d\n", sArithBase.nLastIdentifier);
  return 0;
}

int iiArithAddCmd(
  const char *szName,
  short nAlias,
  short nTokval,
  short nToktype,
  short nPos
  )
{
  //printf("AddCmd(%s, %d, %d, %d, %d)\n", szName, nAlias,
  //       nTokval, nToktype, nPos);
  if(nPos>=0)
  {
    // no checks: we rely on a correct generated code in iparith.inc
    assume(nPos < sArithBase.nCmdAllocated);
    assume(szName!=NULL);
    sArithBase.sCmds[nPos].name    = omStrDup(szName);
    sArithBase.sCmds[nPos].alias   = nAlias;
    sArithBase.sCmds[nPos].tokval  = nTokval;
    sArithBase.sCmds[nPos].toktype = nToktype;
    sArithBase.nCmdUsed++;
    //if(nTokval>0) sArithBase.nLastIdentifier++;
  }
  else
  {
    if(szName==NULL) return -1;
    int nIndex = iiArithFindCmd(szName);
    if(nIndex>=0)
    {
      Print("'%s' already exists at %d\n", szName, nIndex);
      return -1;
    }

    if(sArithBase.nCmdUsed>=sArithBase.nCmdAllocated)
    {
      /* needs to create new slots */
      unsigned long nSize = (sArithBase.nCmdAllocated+1)*sizeof(cmdnames);
      sArithBase.sCmds = (cmdnames *)omRealloc(sArithBase.sCmds, nSize);
      if(sArithBase.sCmds==NULL) return -1;
      sArithBase.nCmdAllocated++;
    }
    /* still free slots available */
    sArithBase.sCmds[sArithBase.nCmdUsed].name    = omStrDup(szName);
    sArithBase.sCmds[sArithBase.nCmdUsed].alias   = nAlias;
    sArithBase.sCmds[sArithBase.nCmdUsed].tokval  = nTokval;
    sArithBase.sCmds[sArithBase.nCmdUsed].toktype = nToktype;
    sArithBase.nCmdUsed++;

    qsort(sArithBase.sCmds, sArithBase.nCmdUsed, sizeof(cmdnames),
          (&_gentable_sort_cmds));
    for(sArithBase.nLastIdentifier=sArithBase.nCmdUsed-1;
        sArithBase.nLastIdentifier>0; sArithBase.nLastIdentifier--)
    {
      if(sArithBase.sCmds[sArithBase.nLastIdentifier].tokval>=0) break;
    }
    //Print("L=%d\n", sArithBase.nLastIdentifier);
  }
  return 0;
}

static BOOLEAN check_valid(const int p, const int op)
{
  #ifdef HAVE_PLURAL
  if (rIsPluralRing(currRing))
  {
    if ((p & PLURAL_MASK)==0 /*NO_PLURAL*/)
    {
      WerrorS("not implemented for non-commutative rings");
      return TRUE;
    }
    else if ((p & PLURAL_MASK)==2 /*, COMM_PLURAL */)
    {
      Warn("assume commutative subalgebra for cmd `%s`",Tok2Cmdname(op));
      return FALSE;
    }
    /* else, ALLOW_PLURAL */
  }
  #endif
  #ifdef HAVE_RINGS
  if (rField_is_Ring(currRing))
  {
    if ((p & RING_MASK)==0 /*NO_RING*/)
    {
      WerrorS("not implemented for rings with rings as coeffients");
      return TRUE;
    }
    /* else ALLOW_RING */
    else if (((p & ZERODIVISOR_MASK)==NO_ZERODIVISOR)
    &&(!rField_is_Domain(currRing)))
    {
      WerrorS("domain required as coeffients");
      return TRUE;
    }
    /* else ALLOW_ZERODIVISOR */
    else if(((p & WARN_RING)==WARN_RING)&&(myynest==0))
    {
      WarnS("considering the image in Q[...]");
    }
  }
  #endif
  return FALSE;
}
