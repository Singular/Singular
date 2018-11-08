/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: interpreter:
*           assignment of expressions and lists to objects or lists
*/

#include "kernel/mod2.h"

#define TRANSEXT_PRIVATES
#include "polys/ext_fields/transext.h"

#include "misc/options.h"
#include "misc/intvec.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"
#include "coeffs/bigintmat.h"


#include "polys/ext_fields/algext.h"

#include "polys/monomials/ring.h"
#include "polys/matpol.h"
#include "polys/monomials/maps.h"
#include "polys/nc/nc.h"
#include "polys/nc/sca.h"
#include "polys/prCopy.h"

#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/oswrapper/timer.h"
#include "kernel/combinatorics/stairc.h"
#include "kernel/GBEngine/syz.h"

//#include "weight.h"
#include "tok.h"
#include "ipid.h"
#include "idrec.h"
#include "subexpr.h"
#include "lists.h"
#include "ipconv.h"
#include "attrib.h"
#include "links/silink.h"
#include "ipshell.h"
#include "blackbox.h"
#include "Singular/number2.h"

/*=================== proc =================*/
static BOOLEAN jjECHO(leftv, leftv a)
{
  si_echo=(int)((long)(a->Data()));
  return FALSE;
}
static BOOLEAN jjPRINTLEVEL(leftv, leftv a)
{
  printlevel=(int)((long)(a->Data()));
  return FALSE;
}
static BOOLEAN jjCOLMAX(leftv, leftv a)
{
  colmax=(int)((long)(a->Data()));
  return FALSE;
}
static BOOLEAN jjTIMER(leftv, leftv a)
{
  timerv=(int)((long)(a->Data()));
  initTimer();
  return FALSE;
}
#ifdef HAVE_GETTIMEOFDAY
static BOOLEAN jjRTIMER(leftv, leftv a)
{
  rtimerv=(int)((long)(a->Data()));
  initRTimer();
  return FALSE;
}
#endif
static BOOLEAN jjMAXDEG(leftv, leftv a)
{
  Kstd1_deg=(int)((long)(a->Data()));
  if (Kstd1_deg!=0)
    si_opt_1 |=Sy_bit(OPT_DEGBOUND);
  else
    si_opt_1 &=(~Sy_bit(OPT_DEGBOUND));
  return FALSE;
}
static BOOLEAN jjMAXMULT(leftv, leftv a)
{
  Kstd1_mu=(int)((long)(a->Data()));
  if (Kstd1_mu!=0)
    si_opt_1 |=Sy_bit(OPT_MULTBOUND);
  else
    si_opt_1 &=(~Sy_bit(OPT_MULTBOUND));
  return FALSE;
}
static BOOLEAN jjTRACE(leftv, leftv a)
{
  traceit=(int)((long)(a->Data()));
  return FALSE;
}
static BOOLEAN jjSHORTOUT(leftv, leftv a)
{
  if (currRing != NULL)
  {
    BOOLEAN shortOut = (BOOLEAN)((long)a->Data());
#if HAVE_CAN_SHORT_OUT
    if (!shortOut)
      currRing->ShortOut = 0;
    else
    {
      if (currRing->CanShortOut)
        currRing->ShortOut = 1;
    }
#else
    currRing->ShortOut = shortOut;
    coeffs cf = currRing->cf;
    while (nCoeff_is_Extension(cf))
    {
      cf->extRing->ShortOut = shortOut;
      assume(cf->extRing != NULL);
      cf = cf->extRing->cf;
    }
#endif
  }
  return FALSE;
}
static void jjMINPOLY_red(idhdl h)
{
  switch(IDTYP(h))
  {
    case NUMBER_CMD:
    {
      number n=(number)IDDATA(h);
      number one = nInit(1);
      number nn=nMult(n,one);
      nDelete(&n);nDelete(&one);
      IDDATA(h)=(char*)nn;
      break;
    }
    case VECTOR_CMD:
    case POLY_CMD:
    {
      poly p=(poly)IDDATA(h);
      IDDATA(h)=(char*)p_MinPolyNormalize(p, currRing);
      break;
    }
    case IDEAL_CMD:
    case MODUL_CMD:
    case MAP_CMD:
    case MATRIX_CMD:
    {
      int i;
      ideal I=(ideal)IDDATA(h);
      for(i=IDELEMS(I)-1;i>=0;i--)
             I->m[i]=p_MinPolyNormalize(I->m[i], currRing);
      break;
    }
    case LIST_CMD:
    {
      lists L=(lists)IDDATA(h);
      int i=L->nr;
      for(;i>=0;i--)
      {
        jjMINPOLY_red((idhdl)&(L->m[i]));
      }
    }
    default:
    //case RESOLUTION_CMD:
       Werror("type %d too complex...set minpoly before",IDTYP(h)); break;
  }
}
static BOOLEAN jjMINPOLY(leftv, leftv a)
{
  if( !nCoeff_is_transExt(currRing->cf) && (currRing->idroot == NULL) && n_IsZero((number)a->Data(), currRing->cf) )
  {
#ifndef SING_NDEBUG
    WarnS("Set minpoly over non-transcendental ground field to 0?!");
    Warn("in >>%s<<",my_yylinebuf);
#endif
    return FALSE;
  }


  if ( !nCoeff_is_transExt(currRing->cf) )
  {
    WarnS("Trying to set minpoly over non-transcendental ground field...");
    if(!nCoeff_is_algExt(currRing->cf) )
    {
      WerrorS("cannot set minpoly for these coeffients");
      return TRUE;
    }
  }
  if ((rVar(currRing->cf->extRing)!=1)
  && !n_IsZero((number)a->Data(), currRing->cf) )
  {
    WerrorS("only univarite minpoly allowed");
    return TRUE;
  }

  BOOLEAN redefine_from_algext=FALSE;
  if ( currRing->idroot != NULL )
  {
    redefine_from_algext=(currRing->cf->extRing->qideal!=NULL);
//    return TRUE;
#ifndef SING_NDEBUG
    idhdl p = currRing->idroot;

    WarnS("no minpoly allowed if there are local objects belonging to the basering: ");

    while(p != NULL)
    {
      PrintS(p->String(TRUE)); Print("(%s)\n",IDID(p));
      p = p->next;
    }
#endif
  }

//  assume (currRing->idroot==NULL);

  number p = (number)a->CopyD(NUMBER_CMD);
  n_Normalize(p, currRing->cf);

  if (n_IsZero(p, currRing->cf))
  {
    n_Delete(&p, currRing->cf);
    if( nCoeff_is_transExt(currRing->cf) )
    {
#ifndef SING_NDEBUG
      WarnS("minpoly is already 0...");
#endif
      return FALSE;
    }
    WarnS("cannot set minpoly to 0 / alg. extension?");
    return TRUE;
  }

  // remove all object currently in the ring
  while(currRing->idroot!=NULL)
  {
#ifndef SING_NDEBUG
    Warn("killing a local object due to minpoly change: %s", IDID(currRing->idroot));
#endif
    killhdl2(currRing->idroot,&(currRing->idroot),currRing);
  }

  AlgExtInfo A;

  A.r = rCopy(currRing->cf->extRing); // Copy  ground field!
  // if minpoly was already set:
  if( currRing->cf->extRing->qideal != NULL ) id_Delete(&(A.r->qideal),A.r);
  ideal q = idInit(1,1);
  if ((p==NULL) ||(NUM((fraction)p)==NULL))
  {
    WerrorS("Could not construct the alg. extension: minpoly==0");
    // cleanup A: TODO
    rDelete( A.r );
    return TRUE;
  }
  if (!redefine_from_algext && (DEN((fraction)(p)) != NULL)) // minpoly must be a fraction with poly numerator...!!
  {
    poly n=DEN((fraction)(p));
    if(!p_IsConstantPoly(n,currRing->cf->extRing))
    {
      WarnS("denominator must be constant - ignoring it");
    }
    p_Delete(&n,currRing->cf->extRing);
    DEN((fraction)(p))=NULL;
  }

  if (redefine_from_algext) q->m[0]=(poly)p;
  else          q->m[0] = NUM((fraction)p);
  A.r->qideal = q;

#if 0
  PrintS("\nTrying to conver the currRing into an algebraic field: ");
  PrintS("Ground poly. ring: \n");
  rWrite( A.r );
  PrintS("\nGiven MinPOLY: ");
  p_Write( A.i->m[0], A.r );
#endif

  // :(
//  NUM((fractionObject *)p) = NULL; // makes 0/ NULL fraction - which should not happen!
//  n_Delete(&p, currRing->cf); // doesn't expect 0/ NULL :(
  if (!redefine_from_algext)
  {
    extern omBin fractionObjectBin;
    NUM((fractionObject *)p) = NULL; // not necessary, but still...
    omFreeBin((ADDRESS)p, fractionObjectBin);
  }

  coeffs new_cf = nInitChar(n_algExt, &A);
  if (new_cf==NULL)
  {
    WerrorS("Could not construct the alg. extension: llegal minpoly?");
    // cleanup A: TODO
    rDelete( A.r );
    return TRUE;
  }
  else
  {
    nKillChar(currRing->cf); currRing->cf=new_cf;
  }
  return FALSE;
}

static BOOLEAN jjNOETHER(leftv, leftv a)
{
  poly p=(poly)a->CopyD(POLY_CMD);
  pDelete(&(currRing->ppNoether));
  (currRing->ppNoether)=p;
  return FALSE;
}
/*=================== proc =================*/
static void jiAssignAttr(leftv l,leftv r)
{
  // get the attribute of th right side
  // and set it to l
  leftv rv=r->LData();
  if (rv!=NULL)
  {
    if (rv->e==NULL)
    {
      if (rv->attribute!=NULL)
      {
        attr la;
        if (r->rtyp!=IDHDL)
        {
          la=rv->attribute;
          rv->attribute=NULL;
        }
        else
        {
          la=rv->attribute->Copy();
        }
        l->attribute=la;
      }
      l->flag=rv->flag;
    }
  }
  if (l->rtyp==IDHDL)
  {
    idhdl h=(idhdl)l->data;
    IDATTR(h)=l->attribute;
    IDFLAG(h)=l->flag;
  }
}
static BOOLEAN jiA_INT(leftv res, leftv a, Subexpr e)
{
  if (e==NULL)
  {
    res->data=(void *)a->Data();
    jiAssignAttr(res,a);
  }
  else
  {
    int i=e->start-1;
    if (i<0)
    {
      Werror("index[%d] must be positive",i+1);
      return TRUE;
    }
    intvec *iv=(intvec *)res->data;
    if (e->next==NULL)
    {
      if (i>=iv->length())
      {
        intvec *iv1=new intvec(i+1);
        (*iv1)[i]=(int)((long)(a->Data()));
        intvec *ivn=ivAdd(iv,iv1);
        delete iv;
        delete iv1;
        res->data=(void *)ivn;
      }
      else
        (*iv)[i]=(int)((long)(a->Data()));
    }
    else
    {
      int c=e->next->start;
      if ((i>=iv->rows())||(c<1)||(c>iv->cols()))
      {
        Werror("wrong range [%d,%d] in intmat %s(%d,%d)",i+1,c,res->Name(),iv->rows(),iv->cols());
        return TRUE;
      }
      else
        IMATELEM(*iv,i+1,c) = (int)((long)(a->Data()));
    }
  }
  return FALSE;
}
static BOOLEAN jiA_NUMBER(leftv res, leftv a, Subexpr)
{
  number p=(number)a->CopyD(NUMBER_CMD);
  if (res->data!=NULL) nDelete((number *)&res->data);
  nNormalize(p);
  res->data=(void *)p;
  jiAssignAttr(res,a);
  return FALSE;
}
#ifdef SINGULAR_4_2
static BOOLEAN jiA_NUMBER2(leftv res, leftv a, Subexpr e)
{
  number2 n=(number2)a->CopyD(CNUMBER_CMD);
  if (e==NULL)
  {
    if (res->data!=NULL)
    {
      number2 nn=(number2)res->data;
      n2Delete(nn);
    }
    res->data=(void *)n;
    jiAssignAttr(res,a);
  }
  else
  {
    int i=e->start-1;
    if (i<0)
    {
      Werror("index[%d] must be positive",i+1);
      return TRUE;
    }
    bigintmat *iv=(bigintmat *)res->data;
    if (e->next==NULL)
    {
      WerrorS("only one index given");
      return TRUE;
    }
    else
    {
      int c=e->next->start;
      if ((i>=iv->rows())||(c<1)||(c>iv->cols()))
      {
        Werror("wrong range [%d,%d] in cmatrix %s(%d,%d)",i+1,c,res->Name(),iv->rows(),iv->cols());
        return TRUE;
      }
      else if (iv->basecoeffs()==n->cf)
      {
        n_Delete((number *)&BIMATELEM(*iv,i+1,c),iv->basecoeffs());
        BIMATELEM(*iv,i+1,c) = n->n;
      }
      else
      {
        WerrorS("different base");
        return TRUE;
      }
    }
  }
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_NUMBER2_I(leftv res, leftv a, Subexpr e)
{
  if (e==NULL)
  {
    if (res->data!=NULL)
    {
      number2 nn=(number2)res->data;
      number2 n=n2Init((long)a->Data(),nn->cf);
      n2Delete(nn);
      res->data=(void *)n;
    }
    else
    {
      WerrorS("no Ring avialable for conversion from int");
      return TRUE;
    }
  }
  else
  {
    int i=e->start-1;
    if (i<0)
    {
      Werror("index[%d] must be positive",i+1);
      return TRUE;
    }
    bigintmat *iv=(bigintmat *)res->data;
    if (e->next==NULL)
    {
      WerrorS("only one index given");
      return TRUE;
    }
    else
    {
      int c=e->next->start;
      if ((i>=iv->rows())||(c<1)||(c>iv->cols()))
      {
        Werror("wrong range [%d,%d] in cmatrix %s(%d,%d)",i+1,c,res->Name(),iv->rows(),iv->cols());
        return TRUE;
      }
      else
      {
        n_Delete((number *)&BIMATELEM(*iv,i+1,c),iv->basecoeffs());
        BIMATELEM(*iv,i+1,c) = n_Init((long)a->Data(),iv->basecoeffs());
      }
    }
  }
  return FALSE;
}
static BOOLEAN jiA_NUMBER2_N(leftv res, leftv a, Subexpr e)
{
  if (e==NULL)
  {
    if (res->data!=NULL)
    {
      number2 nn=(number2)res->data;
      number2 n=(number2)omAlloc(sizeof(*n));
      n->cf=currRing->cf; n->cf->ref++;
      n->n=(number)a->CopyD(NUMBER_CMD);
      n2Delete(nn);
      res->data=(void *)n;
    }
    else
    {
      number2 n=(number2)omAlloc(sizeof(*n));
      n->cf=currRing->cf; n->cf->ref++;
      n->n=(number)a->CopyD(NUMBER_CMD);
      res->data=(void *)n;
    }
  }
  else return TRUE; // TODO: list elements
  return FALSE;
}
static BOOLEAN jiA_POLY2(leftv res, leftv a, Subexpr e)
{
  poly2 n=(poly2)a->CopyD(CPOLY_CMD);
  if (e==NULL)
  {
    if (res->data!=NULL)
    {
      poly2 nn=(poly2)res->data;
      p2Delete(nn);
    }
    res->data=(void *)n;
    jiAssignAttr(res,a);
  }
  else
  {
    int i=e->start-1;
    if (i<0)
    {
      Werror("index[%d] must be positive",i+1);
      return TRUE;
    }
    WerrorS("not yet"); // TODO: list elem
    return TRUE;
  }
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_POLY2_P(leftv res, leftv a, Subexpr e)
{
  if (e==NULL)
  {
    if (res->data!=NULL)
    {
      poly2 nn=(poly2)res->data;
      poly2 n=(poly2)omAlloc(sizeof(*n));
      n->cf=currRing; n->cf->ref++;
      n->n=(poly)a->CopyD(POLY_CMD);
      p2Delete(nn);
      res->data=(void *)n;
    }
    else
    {
      poly2 n=(poly2)omAlloc(sizeof(*n));
      n->cf=currRing; n->cf->ref++;
      n->n=(poly)a->CopyD(POLY_CMD);
      res->data=(void *)n;
    }
  }
  else return TRUE; // TODO: list elements
  return FALSE;
}
#endif
static BOOLEAN jiA_BIGINT(leftv res, leftv a, Subexpr e)
{
  number p=(number)a->CopyD(BIGINT_CMD);
  if (e==NULL)
  {
    if (res->data!=NULL) n_Delete((number *)&res->data,coeffs_BIGINT);
    res->data=(void *)p;
  }
  else
  {
    int i=e->start-1;
    if (i<0)
    {
      Werror("index[%d] must be positive",i+1);
      return TRUE;
    }
    bigintmat *iv=(bigintmat *)res->data;
    if (e->next==NULL)
    {
      WerrorS("only one index given");
      return TRUE;
    }
    else
    {
      int c=e->next->start;
      if ((i>=iv->rows())||(c<1)||(c>iv->cols()))
      {
        Werror("wrong range [%d,%d] in bigintmat %s(%d,%d)",i+1,c,res->Name(),iv->rows(),iv->cols());
        return TRUE;
      }
      else
      {
        n_Delete((number *)&BIMATELEM(*iv,i+1,c),iv->basecoeffs());
        BIMATELEM(*iv,i+1,c) = p;
      }
    }
  }
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_LIST_RES(leftv res, leftv a,Subexpr)
{
  syStrategy r=(syStrategy)a->CopyD(RESOLUTION_CMD);
  if (res->data!=NULL) ((lists)res->data)->Clean();
  int add_row_shift = 0;
  intvec *weights=(intvec*)atGet(a,"isHomog",INTVEC_CMD);
  if (weights!=NULL)  add_row_shift=weights->min_in();
  res->data=(void *)syConvRes(r,TRUE,add_row_shift);
  //jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_LIST(leftv res, leftv a,Subexpr)
{
  lists l=(lists)a->CopyD(LIST_CMD);
  if (res->data!=NULL) ((lists)res->data)->Clean();
  res->data=(void *)l;
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_POLY(leftv res, leftv a,Subexpr e)
{
  poly p=(poly)a->CopyD(POLY_CMD);
  pNormalize(p);
  if (e==NULL)
  {
    if ((p!=NULL) && TEST_V_QRING && (currRing->qideal!=NULL)
    && (!hasFlag(a,FLAG_QRING)))
    {
      jjNormalizeQRingP(p);
      setFlag(res,FLAG_QRING);
    }
    if (res->data!=NULL) pDelete((poly*)&res->data);
    res->data=(void*)p;
    jiAssignAttr(res,a);
  }
  else
  {
    int i,j;
    matrix m=(matrix)res->data;
    i=e->start;
    if (e->next==NULL)
    {
      j=i; i=1;
      // for all ideal like data types: check indices
      if (j>MATCOLS(m))
      {
        if (TEST_V_ALLWARN)
        {
          Warn("increase ideal %d -> %d in %s",MATCOLS(m),j,my_yylinebuf);
        }
        pEnlargeSet(&(m->m),MATCOLS(m),j-MATCOLS(m));
        MATCOLS(m)=j;
      }
      else if (j<=0)
      {
        Werror("index[%d] must be positive",j/*e->start*/);
        return TRUE;
      }
    }
    else
    {
      // for matrices: indices are correct (see ipExprArith3(..,'['..) )
      j=e->next->start;
    }
    if ((p!=NULL) && TEST_V_QRING && (currRing->qideal!=NULL))
    {
      jjNormalizeQRingP(p);
    }
    if (res->rtyp==SMATRIX_CMD)
    {
      p=pSub(p,SMATELEM(m,i-1,j-1,currRing));
      pSetCompP(p,i);
      m->m[j-1]=pAdd(m->m[j-1],p);
    }
    else
    {
      pDelete(&MATELEM(m,i,j));
      MATELEM(m,i,j)=p;
      /* for module: update rank */
      if ((p!=NULL) && (pGetComp(p)!=0))
      {
        m->rank=si_max(m->rank,pMaxComp(p));
      }
    }
  }
  return FALSE;
}
static BOOLEAN jiA_1x1INTMAT(leftv res, leftv a,Subexpr e)
{
  if (/*(*/ res->rtyp!=INTMAT_CMD /*)*/) /*|| (e!=NULL) - TRUE because of type int */
  {
    // no error message: assignment simply fails
    return TRUE;
  }
  intvec* am=(intvec*)a->CopyD(INTMAT_CMD);
  if ((am->rows()!=1) || (am->cols()!=1))
  {
    WerrorS("must be 1x1 intmat");
    delete am;
    return TRUE;
  }
  intvec* m=(intvec *)res->data;
  // indices are correct (see ipExprArith3(..,'['..) )
  int i=e->start;
  int j=e->next->start;
  IMATELEM(*m,i,j)=IMATELEM(*am,1,1);
  delete am;
  return FALSE;
}
static BOOLEAN jiA_1x1MATRIX(leftv res, leftv a,Subexpr e)
{
  if (/*(*/ res->rtyp!=MATRIX_CMD /*)*/) /*|| (e!=NULL) - TRUE because of type poly */
  {
    // no error message: assignment simply fails
    return TRUE;
  }
  matrix am=(matrix)a->CopyD(MATRIX_CMD);
  if ((MATROWS(am)!=1) || (MATCOLS(am)!=1))
  {
    WerrorS("must be 1x1 matrix");
    idDelete((ideal *)&am);
    return TRUE;
  }
  matrix m=(matrix)res->data;
  // indices are correct (see ipExprArith3(..,'['..) )
  int i=e->start;
  int j=e->next->start;
  pDelete(&MATELEM(m,i,j));
  pNormalize(MATELEM(am,1,1));
  MATELEM(m,i,j)=MATELEM(am,1,1);
  MATELEM(am,1,1)=NULL;
  idDelete((ideal *)&am);
  return FALSE;
}
static BOOLEAN jiA_STRING(leftv res, leftv a, Subexpr e)
{
  if (e==NULL)
  {
    void* tmp = res->data;
    res->data=(void *)a->CopyD(STRING_CMD);
    jiAssignAttr(res,a);
    omfree(tmp);
  }
  else
  {
    char *s=(char *)res->data;
    if ((e->start>0)&&(e->start<=(int)strlen(s)))
      s[e->start-1]=(char)(*((char *)a->Data()));
    else
    {
      Werror("string index %d out of range 1..%d",e->start,(int)strlen(s));
      return TRUE;
    }
  }
  return FALSE;
}
static BOOLEAN jiA_PROC(leftv res, leftv a, Subexpr)
{
  extern procinfo *iiInitSingularProcinfo(procinfo *pi, const char *libname,
                                          const char *procname, int line,
                                          long pos, BOOLEAN pstatic=FALSE);
  if(res->data!=NULL) piKill((procinfo *)res->data);
  if(a->Typ()==STRING_CMD)
  {
    res->data = (void *)omAlloc0Bin(procinfo_bin);
    ((procinfo *)(res->data))->language=LANG_NONE;
    iiInitSingularProcinfo((procinfo *)res->data,"",res->name,0,0);
    ((procinfo *)res->data)->data.s.body=(char *)a->CopyD(STRING_CMD);
  }
  else
    res->data=(void *)a->CopyD(PROC_CMD);
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_INTVEC(leftv res, leftv a, Subexpr)
{
  //if ((res->data==NULL) || (res->Typ()==a->Typ()))
  {
    if (res->data!=NULL) delete ((intvec *)res->data);
    res->data=(void *)a->CopyD(INTVEC_CMD);
    jiAssignAttr(res,a);
    return FALSE;
  }
#if 0
  else
  {
    intvec *r=(intvec *)(res->data);
    intvec *s=(intvec *)(a->Data());
    int i=si_min(r->length(), s->length())-1;
    for(;i>=0;i--)
    {
      (*r)[i]=(*s)[i];
    }
    return FALSE; //(r->length()< s->length());
  }
#endif
}
static BOOLEAN jiA_BIGINTMAT(leftv res, leftv a, Subexpr)
{
  if (res->data!=NULL) delete ((bigintmat *)res->data);
  res->data=(void *)a->CopyD(BIGINTMAT_CMD);
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_BUCKET(leftv res, leftv a, Subexpr e)
// there should be no assign bucket:=bucket, here we have poly:=bucket
{
  sBucket_pt b=(sBucket_pt)a->CopyD();
  poly p; int l;
  sBucketDestroyAdd(b,&p,&l);
  sleftv tmp;
  tmp.Init();
  tmp.rtyp=POLY_CMD;
  tmp.data=p;
  return jiA_POLY(res,&tmp,e);
}
static BOOLEAN jiA_IDEAL(leftv res, leftv a, Subexpr)
{
  if (res->data!=NULL) idDelete((ideal*)&res->data);
  res->data=(void *)a->CopyD(MATRIX_CMD);
  if (a->rtyp==IDHDL) id_Normalize((ideal)a->Data(), currRing);
  else                id_Normalize((ideal)res->data, currRing);
  jiAssignAttr(res,a);
  if (((res->rtyp==IDEAL_CMD)||(res->rtyp==MODUL_CMD))
  && (IDELEMS((ideal)(res->data))==1)
  && (currRing->qideal==NULL)
  && (!rIsPluralRing(currRing))
  )
  {
    setFlag(res,FLAG_STD);
  }
  if (TEST_V_QRING && (currRing->qideal!=NULL)&& (!hasFlag(res,FLAG_QRING))) jjNormalizeQRingId(res);
  return FALSE;
}
static BOOLEAN jiA_RESOLUTION(leftv res, leftv a, Subexpr)
{
  if (res->data!=NULL) syKillComputation((syStrategy)res->data);
  res->data=(void *)a->CopyD(RESOLUTION_CMD);
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_MODUL_P(leftv res, leftv a, Subexpr)
/* module = poly */
{
  if (res->data!=NULL) idDelete((ideal*)&res->data);
  ideal I=idInit(1,1);
  I->m[0]=(poly)a->CopyD(POLY_CMD);
  if (I->m[0]!=NULL) pSetCompP(I->m[0],1);
  pNormalize(I->m[0]);
  res->data=(void *)I;
  if (TEST_V_QRING && (currRing->qideal!=NULL))
  {
    if (hasFlag(a,FLAG_QRING)) setFlag(res,FLAG_QRING);
    else                       jjNormalizeQRingId(res);
  }
  return FALSE;
}
static BOOLEAN jiA_IDEAL_M(leftv res, leftv a, Subexpr)
{
  if (res->data!=NULL) idDelete((ideal*)&res->data);
  matrix m=(matrix)a->CopyD(MATRIX_CMD);
  if (TEST_V_ALLWARN)
    if (MATROWS(m)>1)
      Warn("assign matrix with %d rows to an ideal in >>%s<<",MATROWS(m),my_yylinebuf);
  IDELEMS((ideal)m)=MATROWS(m)*MATCOLS(m);
  ((ideal)m)->rank=1;
  MATROWS(m)=1;
  id_Normalize((ideal)m, currRing);
  res->data=(void *)m;
  if (TEST_V_QRING && (currRing->qideal!=NULL)) jjNormalizeQRingId(res);
  return FALSE;
}
static BOOLEAN jiA_IDEAL_Mo(leftv res, leftv a, Subexpr)
{
  ideal m=(ideal)a->CopyD(MODUL_CMD);
  if (m->rank>1)
  {
    Werror("rank of module is %ld in assignment to ideal",m->rank);
    return TRUE;
  }
  if (res->data!=NULL) idDelete((ideal*)&res->data);
  id_Normalize(m, currRing);
  id_Shift(m,-1,currRing);
  m->rank=1;
  res->data=(void *)m;
  if (TEST_V_QRING && (currRing->qideal!=NULL)) jjNormalizeQRingId(res);
  return FALSE;
}
static BOOLEAN jiA_LINK(leftv res, leftv a, Subexpr)
{
  si_link l=(si_link)res->data;

  if (l!=NULL) slCleanUp(l);

  if (a->Typ() == STRING_CMD)
  {
    if (l == NULL)
    {
      l = (si_link) omAlloc0Bin(sip_link_bin);
      res->data = (void *) l;
    }
    return slInit(l, (char *) a->Data());
  }
  else if (a->Typ() == LINK_CMD)
  {
    if (l != NULL) omFreeBin(l, sip_link_bin);
    res->data = slCopy((si_link)a->Data());
    return FALSE;
  }
  return TRUE;
}
// assign map -> map
static BOOLEAN jiA_MAP(leftv res, leftv a, Subexpr)
{
  if (res->data!=NULL)
  {
    omFree((ADDRESS)((map)res->data)->preimage);
    ((map)res->data)->preimage=NULL;
    idDelete((ideal*)&res->data);
  }
  res->data=(void *)a->CopyD(MAP_CMD);
  jiAssignAttr(res,a);
  return FALSE;
}
// assign ideal -> map
static BOOLEAN jiA_MAP_ID(leftv res, leftv a, Subexpr)
{
  map f=(map)res->data;
  char *rn=f->preimage; // save the old/already assigned preimage ring name
  f->preimage=NULL;
  idDelete((ideal *)&f);
  res->data=(void *)a->CopyD(IDEAL_CMD);
  f=(map)res->data;
  id_Normalize((ideal)f, currRing);
  f->preimage = rn;
  return FALSE;
}
static BOOLEAN jiA_QRING(leftv res, leftv a,Subexpr e)
{
  // the follwing can only happen, if:
  //   - the left side is of type qring AND not an id
  if ((e!=NULL)||(res->rtyp!=IDHDL))
  {
    WerrorS("qring_id expected");
    return TRUE;
  }
  ring old_ring=(ring)res->Data();

  coeffs newcf = currRing->cf;
  ideal id = (ideal)a->Data(); //?
  const int cpos = idPosConstant(id);
  if(rField_is_Ring(currRing))
    if (cpos >= 0)
    {
        newcf = n_CoeffRingQuot1(p_GetCoeff(id->m[cpos], currRing), currRing->cf);
        if(newcf == NULL)
          return TRUE;
    }
  //qr=(ring)res->Data();
  //if (qr!=NULL) omFreeBin((ADDRESS)qr, ip_sring_bin);
  ring qr = rCopy(currRing);
  assume(qr->cf == currRing->cf);

  if ( qr->cf != newcf )
  {
    nKillChar ( qr->cf ); // ???
    qr->cf = newcf;
  }
                 // we have to fill it, but the copy also allocates space
  idhdl h=(idhdl)res->data; // we have res->rtyp==IDHDL
  IDRING(h)=qr;

  ideal qid;

  if((rField_is_Ring(currRing)) && (cpos != -1))
  {
    int i, j;
    int *perm = (int *)omAlloc0((qr->N+1)*sizeof(int));

    for(i=qr->N;i>0;i--)
      perm[i]=i;

    nMapFunc nMap = n_SetMap(currRing->cf, newcf);
    qid = idInit(IDELEMS(id)-1,1);
    for(i = 0, j = 0; i<IDELEMS(id); i++)
      if( i != cpos )
        qid->m[j++] = p_PermPoly(id->m[i], perm, currRing, qr, nMap, NULL, 0);
  }
  else
    qid = idrCopyR(id,currRing,qr);

  idSkipZeroes(qid);
  //idPrint(qid);
  if ((idElem(qid)>1) || rIsSCA(currRing) || (currRing->qideal!=NULL))
    assumeStdFlag(a);

  if (currRing->qideal!=NULL) /* we are already in a qring! */
  {
    ideal tmp=idSimpleAdd(qid,currRing->qideal);
    // both ideals should be GB, so dSimpleAdd is sufficient
    idDelete(&qid);
    qid=tmp;
    // delete the qr copy of quotient ideal!!!
    idDelete(&qr->qideal);
  }
  if (idElem(qid)==0)
  {
    qr->qideal = NULL;
    id_Delete(&qid,currRing);
    IDTYP(h)=RING_CMD;
  }
  else
    qr->qideal = qid;

  // qr is a copy of currRing with the new qideal!
  #ifdef HAVE_PLURAL
  if(rIsPluralRing(currRing) &&(qr->qideal!=NULL))
  {
    if (!hasFlag(a,FLAG_TWOSTD))
    {
      Warn("%s is no twosided standard basis",a->Name());
    }

    if( nc_SetupQuotient(qr, currRing) )
    {
//      WarnS("error in nc_SetupQuotient");
    }
  }
  #endif
  //rWrite(qr);
  rSetHdl((idhdl)res->data);
  if (old_ring!=NULL)
  {
    rDelete(old_ring);
  }
  return FALSE;
}

static BOOLEAN jiA_RING(leftv res, leftv a, Subexpr e)
{
  BOOLEAN have_id=TRUE;
  if ((e!=NULL)||(res->rtyp!=IDHDL))
  {
    //WerrorS("id expected");
    //return TRUE;
    have_id=FALSE;
  }
  ring r=(ring)a->Data();
  if ((r==NULL)||(r->cf==NULL)) return TRUE;
  if (have_id)
  {
    idhdl rl=(idhdl)res->data;
    if (IDRING(rl)!=NULL) rKill(rl);
    IDRING(rl)=r;
    if ((IDLEV((idhdl)a->data)!=myynest) && (r==currRing))
      currRingHdl=(idhdl)res->data;
  }
  else
  {
    if (e==NULL) res->data=(char *)r;
    else
    {
      WerrorS("id expected");
      return TRUE;
    }
  }
  r->ref++;
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_PACKAGE(leftv res, leftv a, Subexpr)
{
  res->data=(void *)a->CopyD(PACKAGE_CMD);
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_DEF(leftv res, leftv, Subexpr)
{
  res->data=(void *)0;
  return FALSE;
}
static BOOLEAN jiA_CRING(leftv res, leftv a, Subexpr)
{
  coeffs r=(coeffs)a->Data();
  if (r==NULL) return TRUE;
  if (res->data!=NULL) nKillChar((coeffs)res->data);
  res->data=(void *)a->CopyD(CRING_CMD);
  jiAssignAttr(res,a);
  return FALSE;
}

/*=================== table =================*/
#define IPASSIGN
#define D(A)     A
#define NULL_VAL NULL
#include "table.h"
/*=================== operations ============================*/
/*2
* assign a = b
*/
static BOOLEAN jiAssign_1(leftv l, leftv r, BOOLEAN toplevel)
{
  int rt=r->Typ();
  if (rt==0)
  {
    if (!errorreported) Werror("`%s` is undefined",r->Fullname());
    return TRUE;
  }

  int lt=l->Typ();
  if (lt==0)
  {
    if (!errorreported) Werror("left side `%s` is undefined",l->Fullname());
    return TRUE;
  }
  if(rt==NONE)
  {
    if ((!TEST_V_ASSIGN_NONE)||(lt!=DEF_CMD))
    {
      WarnS("right side is not a datum, assignment ignored");
      Warn("in line >>%s<<",my_yylinebuf);
      // if (!errorreported)
      //   WerrorS("right side is not a datum");
      //return TRUE;
    }
    return FALSE;
  }

  if (lt==DEF_CMD)
  {

    if (TEST_V_ALLWARN
    && (rt!=RING_CMD)
    && (l->name!=NULL)
    && (l->e==NULL)
    && (iiCurrArgs==NULL) /* not in proc header */
    )
    {
      Warn("use `%s` instead of `def` in %s:%d:%s",Tok2Cmdname(rt),
            currentVoice->filename,yylineno,my_yylinebuf);
    }
    if (l->rtyp==IDHDL)
    {
      if (rt==BUCKET_CMD) IDTYP((idhdl)l->data)=POLY_CMD;
      else                IDTYP((idhdl)l->data)=rt;
    }
    else if (l->name!=NULL)
    {
      int rrt;
      if (rt==BUCKET_CMD) rrt=POLY_CMD;
      else                rrt=rt;
      sleftv ll;
      iiDeclCommand(&ll,l,myynest,rrt,&IDROOT);
      memcpy(l,&ll,sizeof(sleftv));
    }
    else
    {
      if (rt==BUCKET_CMD) l->rtyp=POLY_CMD;
      else                l->rtyp=rt;
    }
    lt=l->Typ();
  }
  else
  {
    if ((l->data==r->data)&&(l->e==NULL)&&(r->e==NULL))
      return FALSE;
  }
  leftv ld=l;
  if (l->rtyp==IDHDL)
  {
    if (lt!=RING_CMD)
      ld=(leftv)l->data;
  }
  else if (toplevel)
  {
    WerrorS("error in assign: left side is not an l-value");
    return TRUE;
  }
  if (lt>MAX_TOK)
  {
    blackbox *bb=getBlackboxStuff(lt);
#ifdef BLACKBOX_DEVEL
    Print("bb-assign: bb=%lx\n",bb);
#endif
    return (bb==NULL) || bb->blackbox_Assign(l,r);
  }
  int start=0;
  while ((dAssign[start].res!=lt)
      && (dAssign[start].res!=0)) start++;
  int i=start;
  while ((dAssign[i].res==lt)
      && (dAssign[i].arg!=rt)) i++;
  if (dAssign[i].res==lt)
  {
    if (traceit&TRACE_ASSIGN) Print("assign %s=%s\n",Tok2Cmdname(lt),Tok2Cmdname(rt));
    BOOLEAN b;
    b=dAssign[i].p(ld,r,l->e);
    if(l!=ld) /* i.e. l is IDHDL, l->data is ld */
    {
      l->flag=ld->flag;
      l->attribute=ld->attribute;
    }
    return b;
  }
  // implicite type conversion ----------------------------------------------
  if (dAssign[i].res!=lt)
  {
    int ri;
    leftv rn = (leftv)omAlloc0Bin(sleftv_bin);
    BOOLEAN failed=FALSE;
    i=start;
    //while ((dAssign[i].res!=lt)
    //  && (dAssign[i].res!=0)) i++;
    while (dAssign[i].res==lt)
    {
      if ((ri=iiTestConvert(rt,dAssign[i].arg))!=0)
      {
        failed= iiConvert(rt,dAssign[i].arg,ri,r,rn);
        if(!failed)
        {
          failed= dAssign[i].p(ld,rn,l->e);
          if (traceit&TRACE_ASSIGN)
            Print("assign %s=%s ok? %d\n",Tok2Cmdname(lt),Tok2Cmdname(rn->rtyp),!failed);
        }
        // everything done, clean up temp. variables
        rn->CleanUp();
        omFreeBin((ADDRESS)rn, sleftv_bin);
        if (failed)
        {
          // leave loop, goto error handling
          break;
        }
        else
        {
          if(l!=ld) /* i.e. l is IDHDL, l->data is ld */
          {
            l->flag=ld->flag;
            l->attribute=ld->attribute;
          }
          // everything ok, return
          return FALSE;
        }
     }
     i++;
    }
    // error handling ---------------------------------------------------
    if (!errorreported)
    {
      if ((l->rtyp==IDHDL) && (l->e==NULL))
        Werror("`%s`(%s) = `%s` is not supported",
          Tok2Cmdname(lt),l->Name(),Tok2Cmdname(rt));
      else
         Werror("`%s` = `%s` is not supported"
             ,Tok2Cmdname(lt),Tok2Cmdname(rt));
      if (BVERBOSE(V_SHOW_USE))
      {
        i=0;
        while ((dAssign[i].res!=lt)
          && (dAssign[i].res!=0)) i++;
        while (dAssign[i].res==lt)
        {
          Werror("expected `%s` = `%s`"
              ,Tok2Cmdname(lt),Tok2Cmdname(dAssign[i].arg));
          i++;
        }
      }
    }
  }
  return TRUE;
}
/*2
* assign sys_var = val
*/
static BOOLEAN iiAssign_sys(leftv l, leftv r)
{
  int rt=r->Typ();

  if (rt==0)
  {
    if (!errorreported) Werror("`%s` is undefined",r->Fullname());
    return TRUE;
  }
  int i=0;
  int lt=l->rtyp;
  while (((dAssign_sys[i].res!=lt)
      || (dAssign_sys[i].arg!=rt))
    && (dAssign_sys[i].res!=0)) i++;
  if (dAssign_sys[i].res!=0)
  {
    if (!dAssign_sys[i].p(l,r))
    {
      // everything ok, clean up
      return FALSE;
    }
  }
  // implicite type conversion ----------------------------------------------
  if (dAssign_sys[i].res==0)
  {
    int ri;
    leftv rn = (leftv)omAlloc0Bin(sleftv_bin);
    BOOLEAN failed=FALSE;
    i=0;
    while ((dAssign_sys[i].res!=lt)
      && (dAssign_sys[i].res!=0)) i++;
    while (dAssign_sys[i].res==lt)
    {
      if ((ri=iiTestConvert(rt,dAssign_sys[i].arg))!=0)
      {
        failed= ((iiConvert(rt,dAssign_sys[i].arg,ri,r,rn))
            || (dAssign_sys[i].p(l,rn)));
        // everything done, clean up temp. variables
        rn->CleanUp();
        omFreeBin((ADDRESS)rn, sleftv_bin);
        if (failed)
        {
          // leave loop, goto error handling
          break;
        }
        else
        {
          // everything ok, return
          return FALSE;
        }
     }
     i++;
    }
    // error handling ---------------------------------------------------
    if(!errorreported)
    {
      Werror("`%s` = `%s` is not supported"
             ,Tok2Cmdname(lt),Tok2Cmdname(rt));
      if (BVERBOSE(V_SHOW_USE))
      {
        i=0;
        while ((dAssign_sys[i].res!=lt)
          && (dAssign_sys[i].res!=0)) i++;
        while (dAssign_sys[i].res==lt)
        {
          Werror("expected `%s` = `%s`"
              ,Tok2Cmdname(lt),Tok2Cmdname(dAssign_sys[i].arg));
          i++;
        }
      }
    }
  }
  return TRUE;
}
static BOOLEAN jiA_INTVEC_L(leftv l,leftv r)
{
  /* right side is intvec, left side is list (of int)*/
  BOOLEAN nok;
  int i=0;
  leftv l1=l;
  leftv h;
  sleftv t;
  intvec *iv=(intvec *)r->Data();
  memset(&t,0,sizeof(sleftv));
  t.rtyp=INT_CMD;
  while ((i<iv->length())&&(l!=NULL))
  {
    t.data=(char *)(long)(*iv)[i];
    h=l->next;
    l->next=NULL;
    nok=jiAssign_1(l,&t,TRUE);
    l->next=h;
    if (nok) return TRUE;
    i++;
    l=h;
  }
  l1->CleanUp();
  r->CleanUp();
  return FALSE;
}
static BOOLEAN jiA_VECTOR_L(leftv l,leftv r)
{
  /* right side is vector, left side is list (of poly)*/
  BOOLEAN nok;
  leftv l1=l;
  ideal I=idVec2Ideal((poly)r->Data());
  leftv h;
  sleftv t;
  int i=0;
  memset(&t,0,sizeof(sleftv));
  while (l!=NULL)
  {
    t.rtyp=POLY_CMD;
    if (i>=IDELEMS(I))
    {
      t.data=NULL;
    }
    else
    {
      t.data=(char *)I->m[i];
      I->m[i]=NULL;
    }
    h=l->next;
    l->next=NULL;
    nok=jiAssign_1(l,&t,TRUE);
    l->next=h;
    t.CleanUp();
    if (nok)
    {
      idDelete(&I);
      return TRUE;
    }
    i++;
    l=h;
  }
  idDelete(&I);
  l1->CleanUp();
  r->CleanUp();
  //if (TEST_V_QRING && (currRing->qideal!=NULL)) jjNormalizeQRingP(l);
  return FALSE;
}
static BOOLEAN jjA_L_LIST(leftv l, leftv r)
/* left side: list/def, has to be a "real" variable
*  right side: expression list
*/
{
  int sl = r->listLength();
  lists L=(lists)omAllocBin(slists_bin);
  lists oldL;
  leftv h=NULL,o_r=r;
  int i;
  int rt;

  L->Init(sl);
  for (i=0;i<sl;i++)
  {
    if (h!=NULL) { /* e.g. not in the first step:
                   * h is the pointer to the old sleftv,
                   * r is the pointer to the next sleftv
                   * (in this moment) */
                   h->next=r;
                 }
    h=r;
    r=r->next;
    h->next=NULL;
    rt=h->Typ();
    if ((rt==0)||(rt==NONE)||(rt==DEF_CMD))
    {
      L->Clean();
      Werror("`%s` is undefined",h->Fullname());
      //listall();
      goto err;
    }
    //if (rt==RING_CMD)
    //{
    //  L->m[i].rtyp=rt;
    //  L->m[i].data=h->Data();
    //  ((ring)L->m[i].data)->ref++;
    //}
    //else
      L->m[i].CleanUp();
      L->m[i].Copy(h);
      if(errorreported)
      {
        L->Clean();
        goto err;
      }
  }
  oldL=(lists)l->Data();
  if (oldL!=NULL) oldL->Clean();
  if (l->rtyp==IDHDL)
  {
    IDLIST((idhdl)l->data)=L;
    IDTYP((idhdl)l->data)=LIST_CMD; // was possibly DEF_CMD
    if (lRingDependend(L)) ipMoveId((idhdl)l->data);
  }
  else
  {
    l->LData()->data=L;
    if ((l->e!=NULL) && (l->rtyp==DEF_CMD))
      l->rtyp=LIST_CMD;
  }
err:
  o_r->CleanUp();
  return errorreported;
}
static BOOLEAN jjA_L_INTVEC(leftv l,leftv r,intvec *iv)
{
  /* left side is intvec/intmat, right side is list (of int,intvec,intmat)*/
  leftv hh=r;
  int i = 0;
  while (hh!=NULL)
  {
    if (i>=iv->length())
    {
      if (traceit&TRACE_ASSIGN)
      {
        Warn("expression list length(%d) does not match intmat size(%d)",
             iv->length()+exprlist_length(hh),iv->length());
      }
      break;
    }
    if (hh->Typ() == INT_CMD)
    {
      (*iv)[i++] = (int)((long)(hh->Data()));
    }
    else if ((hh->Typ() == INTVEC_CMD)
            ||(hh->Typ() == INTMAT_CMD))
    {
      intvec *ivv = (intvec *)(hh->Data());
      int ll = 0,l = si_min(ivv->length(),iv->length());
      for (; l>0; l--)
      {
        (*iv)[i++] = (*ivv)[ll++];
      }
    }
    else
    {
      delete iv;
      return TRUE;
    }
    hh = hh->next;
  }
  if (l->rtyp==IDHDL)
  {
    if (IDINTVEC((idhdl)l->data)!=NULL) delete IDINTVEC((idhdl)l->data);
    IDINTVEC((idhdl)l->data)=iv;
  }
  else
  {
    if (l->data!=NULL) delete ((intvec*)l->data);
    l->data=(char*)iv;
  }
  return FALSE;
}
static BOOLEAN jjA_L_BIGINTMAT(leftv l,leftv r,bigintmat *bim)
{
  /* left side is bigintmat, right side is list (of int,intvec,intmat)*/
  leftv hh=r;
  int i = 0;
  if (bim->length()==0) { WerrorS("bigintmat is 1x0"); delete bim; return TRUE; }
  while (hh!=NULL)
  {
    if (i>=bim->cols()*bim->rows())
    {
      if (traceit&TRACE_ASSIGN)
      {
        Warn("expression list length(%d) does not match bigintmat size(%d x %d)",
              exprlist_length(hh),bim->rows(),bim->cols());
      }
      break;
    }
    if (hh->Typ() == INT_CMD)
    {
      number tp = n_Init((int)((long)(hh->Data())), coeffs_BIGINT);
      bim->set(i++, tp);
      n_Delete(&tp, coeffs_BIGINT);
    }
    else if (hh->Typ() == BIGINT_CMD)
    {
      bim->set(i++, (number)(hh->Data()));
    }
    /*
    ((hh->Typ() == INTVEC_CMD)
            ||(hh->Typ() == INTMAT_CMD))
    {
      intvec *ivv = (intvec *)(hh->Data());
      int ll = 0,l = si_min(ivv->length(),iv->length());
      for (; l>0; l--)
      {
        (*iv)[i++] = (*ivv)[ll++];
      }
    }*/
    else
    {
      delete bim;
      return TRUE;
    }
    hh = hh->next;
  }
  if (IDBIMAT((idhdl)l->data)!=NULL) delete IDBIMAT((idhdl)l->data);
  IDBIMAT((idhdl)l->data)=bim;
  return FALSE;
}
static BOOLEAN jjA_L_STRING(leftv l,leftv r)
{
  /* left side is string, right side is list of string*/
  leftv hh=r;
  int sl = 1;
  char *s;
  char *t;
  int tl;
  /* find the length */
  while (hh!=NULL)
  {
    if (hh->Typ()!= STRING_CMD)
    {
      return TRUE;
    }
    sl += strlen((char *)hh->Data());
    hh = hh->next;
  }
  s = (char * )omAlloc(sl);
  sl=0;
  hh = r;
  while (hh!=NULL)
  {
    t=(char *)hh->Data();
    tl=strlen(t);
    memcpy(s+sl,t,tl);
    sl+=tl;
    hh = hh->next;
  }
  s[sl]='\0';
  omFree((ADDRESS)IDDATA((idhdl)(l->data)));
  IDDATA((idhdl)(l->data))=s;
  return FALSE;
}
static BOOLEAN jiA_MATRIX_L(leftv l,leftv r)
{
  /* right side is matrix, left side is list (of poly)*/
  BOOLEAN nok=FALSE;
  int i;
  matrix m=(matrix)r->CopyD(MATRIX_CMD);
  leftv h;
  leftv ol=l;
  leftv o_r=r;
  sleftv t;
  memset(&t,0,sizeof(sleftv));
  t.rtyp=POLY_CMD;
  int mxn=MATROWS(m)*MATCOLS(m);
  loop
  {
    i=0;
    while ((i<mxn /*MATROWS(m)*MATCOLS(m)*/)&&(l!=NULL))
    {
      t.data=(char *)m->m[i];
      m->m[i]=NULL;
      h=l->next;
      l->next=NULL;
      idhdl hh=NULL;
      if ((l->rtyp==IDHDL)&&(l->Typ()==DEF_CMD)) hh=(idhdl)l->data;
      nok=jiAssign_1(l,&t,TRUE);
      if (hh!=NULL) { ipMoveId(hh);hh=NULL;}
      l->next=h;
      if (nok)
      {
        idDelete((ideal *)&m);
        goto ende;
      }
      i++;
      l=h;
    }
    idDelete((ideal *)&m);
    h=r;
    r=r->next;
    if (l==NULL)
    {
      if (r!=NULL)
      {
        WarnS("list length mismatch in assign (l>r)");
        nok=TRUE;
      }
      break;
    }
    else if (r==NULL)
    {
      WarnS("list length mismatch in assign (l<r)");
      nok=TRUE;
      break;
    }
    if ((r->Typ()==IDEAL_CMD)||(r->Typ()==MATRIX_CMD))
    {
      m=(matrix)r->CopyD(MATRIX_CMD);
      mxn=MATROWS(m)*MATCOLS(m);
    }
    else if (r->Typ()==POLY_CMD)
    {
      m=mpNew(1,1);
      MATELEM(m,1,1)=(poly)r->CopyD(POLY_CMD);
      pNormalize(MATELEM(m,1,1));
      mxn=1;
    }
    else
    {
      nok=TRUE;
      break;
    }
  }
ende:
  o_r->CleanUp();
  ol->CleanUp();
  return nok;
}
static BOOLEAN jiA_STRING_L(leftv l,leftv r)
{
  /*left side are strings, right side is a string*/
  /*e.g. s[2..3]="12" */
  /*the case s=t[1..4] is handled in iiAssign,
  * the case s[2..3]=t[3..4] is handled in iiAssgn_rec*/
  BOOLEAN nok=FALSE;
  sleftv t;
  leftv h,l1=l;
  int i=0;
  char *ss;
  char *s=(char *)r->Data();
  int sl=strlen(s);

  memset(&t,0,sizeof(sleftv));
  t.rtyp=STRING_CMD;
  while ((i<sl)&&(l!=NULL))
  {
    ss=(char *)omAlloc(2);
    ss[1]='\0';
    ss[0]=s[i];
    t.data=ss;
    h=l->next;
    l->next=NULL;
    nok=jiAssign_1(l,&t,TRUE);
    if (nok)
    {
      break;
    }
    i++;
    l=h;
  }
  r->CleanUp();
  l1->CleanUp();
  return nok;
}
static BOOLEAN jiAssign_list(leftv l, leftv r)
{
  int i=l->e->start-1;
  if (i<0)
  {
    Werror("index[%d] must be positive",i+1);
    return TRUE;
  }
  if(l->attribute!=NULL)
  {
    atKillAll((idhdl)l);
    l->attribute=NULL;
  }
  l->flag=0;
  lists li;
  if (l->rtyp==IDHDL)
  {
    li=IDLIST((idhdl)l->data);
  }
  else
  {
    li=(lists)l->data;
  }
  if (i>li->nr)
  {
    if (TEST_V_ALLWARN)
    {
      Warn("increase list %d -> %d in %s",li->nr,i,my_yylinebuf);
    }
    li->m=(leftv)omreallocSize(li->m,(li->nr+1)*sizeof(sleftv),(i+1)*sizeof(sleftv));
    memset(&(li->m[li->nr+1]),0,(i-li->nr)*sizeof(sleftv));
    int j=li->nr+1;
    for(;j<=i;j++)
      li->m[j].rtyp=DEF_CMD;
    li->nr=i;
  }
  leftv ld=&(li->m[i]);
  ld->e=l->e->next;
  BOOLEAN b;
  sleftv tmp;
  memset(&tmp,0,sizeof(sleftv));
  if (/*(ld->rtyp!=LIST_CMD)
  &&*/(ld->e==NULL)
  && (ld->Typ()!=r->Typ()))
  {
    tmp.rtyp=DEF_CMD;
    b=iiAssign(&tmp,r,FALSE);
    ld->CleanUp();
    memcpy(ld,&tmp,sizeof(sleftv));
  }
  else if ((ld->e==NULL)
  && (ld->Typ()==r->Typ())
  && (ld->Typ()<MAX_TOK))
  {
    tmp.rtyp=r->Typ();
    tmp.data=(char*)idrecDataInit(r->Typ());
    b=iiAssign(&tmp,r,FALSE);
    ld->CleanUp();
    memcpy(ld,&tmp,sizeof(sleftv));
  }
  else
  {
    b=iiAssign(ld,r,FALSE);
    if (l->e!=NULL) l->e->next=ld->e;
    ld->e=NULL;
  }
  return b;
}
static BOOLEAN jiAssign_rec(leftv l, leftv r)
{
  leftv l1=l;
  leftv r1=r;
  leftv lrest;
  leftv rrest;
  BOOLEAN b;
  do
  {
    lrest=l->next;
    rrest=r->next;
    l->next=NULL;
    r->next=NULL;
    b=iiAssign(l,r);
    l->next=lrest;
    r->next=rrest;
    l=lrest;
    r=rrest;
  } while  ((!b)&&(l!=NULL));
  l1->CleanUp();
  r1->CleanUp();
  return b;
}
BOOLEAN iiAssign(leftv l, leftv r, BOOLEAN toplevel)
{
  if (errorreported) return TRUE;
  int ll=l->listLength();
  int rl;
  int lt=l->Typ();
  int rt=NONE;
  BOOLEAN b;
  if (l->rtyp==ALIAS_CMD)
  {
    Werror("`%s` is read-only",l->Name());
  }

  if (l->rtyp==IDHDL)
  {
    atKillAll((idhdl)l->data);
    IDFLAG((idhdl)l->data)=0;
    l->attribute=NULL;
    toplevel=FALSE;
  }
  else if (l->attribute!=NULL)
    atKillAll((idhdl)l);
  l->flag=0;
  if (ll==1)
  {
    /* l[..] = ... */
    if(l->e!=NULL)
    {
      BOOLEAN like_lists=0;
      blackbox *bb=NULL;
      int bt;
      if (((bt=l->rtyp)>MAX_TOK)
      || ((l->rtyp==IDHDL) && ((bt=IDTYP((idhdl)l->data))>MAX_TOK)))
      {
        bb=getBlackboxStuff(bt);
        like_lists=BB_LIKE_LIST(bb); // bb like a list
      }
      else if (((l->rtyp==IDHDL) && (IDTYP((idhdl)l->data)==LIST_CMD))
        || (l->rtyp==LIST_CMD))
      {
        like_lists=2; // bb in a list
      }
      if(like_lists)
      {
        if (traceit&TRACE_ASSIGN) PrintS("assign list[..]=...or similar\n");
        if (like_lists==1)
        {
          // check blackbox/newtype type:
          if(bb->blackbox_CheckAssign(bb,l,r)) return TRUE;
        }
        b=jiAssign_list(l,r);
        if((!b) && (like_lists==2))
        {
          //Print("jjA_L_LIST: - 2 \n");
          if((l->rtyp==IDHDL) && (l->data!=NULL))
          {
            ipMoveId((idhdl)l->data);
            l->attribute=IDATTR((idhdl)l->data);
            l->flag=IDFLAG((idhdl)l->data);
          }
        }
        r->CleanUp();
        Subexpr h;
        while (l->e!=NULL)
        {
          h=l->e->next;
          omFreeBin((ADDRESS)l->e, sSubexpr_bin);
          l->e=h;
        }
        return b;
      }
    }
    if (lt>MAX_TOK)
    {
      blackbox *bb=getBlackboxStuff(lt);
#ifdef BLACKBOX_DEVEL
      Print("bb-assign: bb=%lx\n",bb);
#endif
      return (bb==NULL) || bb->blackbox_Assign(l,r);
    }
    // end of handling elems of list and similar
    rl=r->listLength();
    if (rl==1)
    {
      /* system variables = ... */
      if(((l->rtyp>=VECHO)&&(l->rtyp<=VPRINTLEVEL))
      ||((l->rtyp>=VALTVARS)&&(l->rtyp<=VMINPOLY)))
      {
        b=iiAssign_sys(l,r);
        r->CleanUp();
        //l->CleanUp();
        return b;
      }
      rt=r->Typ();
      /* a = ... */
      if ((lt!=MATRIX_CMD)
      &&(lt!=BIGINTMAT_CMD)
      &&(lt!=CMATRIX_CMD)
      &&(lt!=INTMAT_CMD)
      &&((lt==rt)||(lt!=LIST_CMD)))
      {
        b=jiAssign_1(l,r,toplevel);
        if (l->rtyp==IDHDL)
        {
          if ((lt==DEF_CMD)||(lt==LIST_CMD))
          {
            ipMoveId((idhdl)l->data);
          }
          l->attribute=IDATTR((idhdl)l->data);
          l->flag=IDFLAG((idhdl)l->data);
          l->CleanUp();
        }
        r->CleanUp();
        return b;
      }
      if (((lt!=LIST_CMD)
        &&((rt==MATRIX_CMD)
          ||(rt==BIGINTMAT_CMD)
          ||(rt==CMATRIX_CMD)
          ||(rt==INTMAT_CMD)
          ||(rt==INTVEC_CMD)
          ||(rt==MODUL_CMD)))
      ||((lt==LIST_CMD)
        &&(rt==RESOLUTION_CMD))
      )
      {
        b=jiAssign_1(l,r,toplevel);
        if((l->rtyp==IDHDL)&&(l->data!=NULL))
        {
          if ((lt==DEF_CMD) || (lt==LIST_CMD))
          {
            //Print("ipAssign - 3.0\n");
            ipMoveId((idhdl)l->data);
          }
          l->attribute=IDATTR((idhdl)l->data);
          l->flag=IDFLAG((idhdl)l->data);
        }
        r->CleanUp();
        Subexpr h;
        while (l->e!=NULL)
        {
          h=l->e->next;
          omFreeBin((ADDRESS)l->e, sSubexpr_bin);
          l->e=h;
        }
        return b;
      }
    }
    if (rt==NONE) rt=r->Typ();
  }
  else if (ll==(rl=r->listLength()))
  {
    b=jiAssign_rec(l,r);
    return b;
  }
  else
  {
    if (rt==NONE) rt=r->Typ();
    if (rt==INTVEC_CMD)
      return jiA_INTVEC_L(l,r);
    else if (rt==VECTOR_CMD)
      return jiA_VECTOR_L(l,r);
    else if ((rt==IDEAL_CMD)||(rt==MATRIX_CMD))
      return jiA_MATRIX_L(l,r);
    else if ((rt==STRING_CMD)&&(rl==1))
      return jiA_STRING_L(l,r);
    Werror("length of lists in assignment does not match (l:%d,r:%d)",
      ll,rl);
    return TRUE;
  }

  leftv hh=r;
  BOOLEAN nok=FALSE;
  BOOLEAN map_assign=FALSE;
  switch (lt)
  {
    case INTVEC_CMD:
      nok=jjA_L_INTVEC(l,r,new intvec(exprlist_length(r)));
      break;
    case INTMAT_CMD:
    {
      nok=jjA_L_INTVEC(l,r,new intvec(IDINTVEC((idhdl)l->data)));
      break;
    }
    case BIGINTMAT_CMD:
    {
      nok=jjA_L_BIGINTMAT(l, r, new bigintmat(IDBIMAT((idhdl)l->data)));
      break;
    }
    case MAP_CMD:
    {
      // first element in the list sl (r) must be a ring
      if ((rt == RING_CMD)&&(r->e==NULL))
      {
        omFree((ADDRESS)IDMAP((idhdl)l->data)->preimage);
        IDMAP((idhdl)l->data)->preimage = omStrDup (r->Fullname());
        /* advance the expressionlist to get the next element after the ring */
        hh = r->next;
      }
      else
      {
        WerrorS("expected ring-name");
        nok=TRUE;
        break;
      }
      if (hh==NULL) /* map-assign: map f=r; */
      {
        WerrorS("expected image ideal");
        nok=TRUE;
        break;
      }
      if ((hh->next==NULL)&&(hh->Typ()==IDEAL_CMD))
      {
        BOOLEAN bo=jiAssign_1(l,hh,toplevel); /* map-assign: map f=r,i; */
        omFreeBin(hh,sleftv_bin);
        return bo;
      }
      //no break, handle the rest like an ideal:
      map_assign=TRUE;
    }
    case MATRIX_CMD:
    case IDEAL_CMD:
    case MODUL_CMD:
    {
      sleftv t;
      matrix olm = (matrix)l->Data();
      int rk;
      char *pr=((map)olm)->preimage;
      BOOLEAN module_assign=(/*l->Typ()*/ lt==MODUL_CMD);
      matrix lm ;
      int  num;
      int j,k;
      int i=0;
      int mtyp=MATRIX_CMD; /*Type of left side object*/
      int etyp=POLY_CMD;   /*Type of elements of left side object*/

      if (lt /*l->Typ()*/==MATRIX_CMD)
      {
        rk=olm->rows();
        num=olm->cols()*rk /*olm->rows()*/;
        lm=mpNew(olm->rows(),olm->cols());
        int el;
        if ((traceit&TRACE_ASSIGN) && (num!=(el=exprlist_length(hh))))
        {
          Warn("expression list length(%d) does not match matrix size(%d)",el,num);
        }
      }
      else /* IDEAL_CMD or MODUL_CMD */
      {
        num=exprlist_length(hh);
        lm=(matrix)idInit(num,1);
        if (module_assign)
        {
          rk=0;
          mtyp=MODUL_CMD;
          etyp=VECTOR_CMD;
        }
        else
          rk=1;
      }

      int ht;
      loop
      {
        if (hh==NULL)
          break;
        else
        {
          matrix rm;
          ht=hh->Typ();
          if ((j=iiTestConvert(ht,etyp))!=0)
          {
            nok=iiConvert(ht,etyp,j,hh,&t);
            hh->next=t.next;
            if (nok)
            { Werror("can not convert %s(%s) -> %s",Tok2Cmdname(ht),hh->Name(),Tok2Cmdname(etyp));
               break;
            }
            lm->m[i]=(poly)t.CopyD(etyp);
            pNormalize(lm->m[i]);
            if (module_assign) rk=si_max(rk,(int)pMaxComp(lm->m[i]));
            i++;
          }
          else
          if ((j=iiTestConvert(ht,mtyp))!=0)
          {
            nok=iiConvert(ht,mtyp,j,hh,&t);
            hh->next=t.next;
            if (nok)
            { Werror("can not convert %s(%s) -> %s",Tok2Cmdname(ht),hh->Name(),Tok2Cmdname(mtyp));
               break;
            }
            rm = (matrix)t.CopyD(mtyp);
            if (module_assign)
            {
              j = si_min(num,rm->cols());
              rk=si_max(rk,(int)rm->rank);
            }
            else
              j = si_min(num-i,rm->rows() * rm->cols());
            for(k=0;k<j;k++,i++)
            {
              lm->m[i]=rm->m[k];
              pNormalize(lm->m[i]);
              rm->m[k]=NULL;
            }
            idDelete((ideal *)&rm);
          }
          else
          {
            nok=TRUE;
            if (nok)
            { Werror("can not convert %s(%s) -> %s",Tok2Cmdname(ht),hh->Name(),Tok2Cmdname(mtyp));
            }
            break;
          }
          t.next=NULL;t.CleanUp();
          if (i==num) break;
          hh=hh->next;
        }
      }
      if (nok)
        idDelete((ideal *)&lm);
      else
      {
        idDelete((ideal *)&olm);
        if (module_assign)   lm->rank=rk;
        else if (map_assign) ((map)lm)->preimage=pr;
        l=l->LData();
        if (l->rtyp==IDHDL)
          IDMATRIX((idhdl)l->data)=lm;
        else
          l->data=(char *)lm;
      }
      break;
    }
    case STRING_CMD:
      nok=jjA_L_STRING(l,r);
      break;
    //case DEF_CMD:
    case LIST_CMD:
      nok=jjA_L_LIST(l,r);
      break;
    case NONE:
    case 0:
      Werror("cannot assign to %s",l->Fullname());
      nok=TRUE;
      break;
    default:
      WerrorS("assign not impl.");
      nok=TRUE;
      break;
  } /* end switch: typ */
  if (nok && (!errorreported)) WerrorS("incompatible type in list assignment");
  r->CleanUp();
  return nok;
}
void jjNormalizeQRingId(leftv I)
{
  if ((currRing->qideal!=NULL) && (!hasFlag(I,FLAG_QRING)))
  {
    if (I->e==NULL)
    {
      ideal I0=(ideal)I->Data();
      switch (I->Typ())
      {
        case IDEAL_CMD:
        case MODUL_CMD:
        {
          ideal F=idInit(1,1);
          ideal II=kNF(F,currRing->qideal,I0);
          idDelete(&F);
          if (I->rtyp!=IDHDL)
          {
            idDelete((ideal*)&(I0));
            I->data=II;
          }
          else
          {
            idhdl h=(idhdl)I->data;
            idDelete((ideal*)&IDIDEAL(h));
            IDIDEAL(h)=II;
            setFlag(h,FLAG_QRING);
          }
          break;
        }
        default: break;
      }
      setFlag(I,FLAG_QRING);
    }
  }
}
void jjNormalizeQRingP(poly &p)
{
  if((p!=NULL) && (currRing->qideal!=NULL))
  {
    ideal F=idInit(1,1);
    poly p2=kNF(F,currRing->qideal,p);
    pNormalize(p2);
    idDelete(&F);
    pDelete(&p);
    p=p2;
  }
}
BOOLEAN jjIMPORTFROM(leftv, leftv u, leftv v)
{
  //Print("importfrom %s::%s ->.\n",v->Name(),u->Name() );
  assume(u->Typ()==PACKAGE_CMD);
  char *vn=(char *)v->Name();
  idhdl h=((package)(u->Data()))->idroot->get(vn /*v->Name()*/, myynest);
  if (h!=NULL)
  {
    //check for existence
    if (((package)(u->Data()))==basePack)
    {
      WarnS("source and destination packages are identical");
      return FALSE;
    }
    idhdl t=basePack->idroot->get(vn /*v->Name()*/, myynest);
    if (t!=NULL)
    {
      if (BVERBOSE(V_REDEFINE)) Warn("redefining %s (%s)",vn,my_yylinebuf);
      killhdl(t);
    }
    sleftv tmp_expr;
    if (iiDeclCommand(&tmp_expr,v,myynest,DEF_CMD,&IDROOT)) return TRUE;
    sleftv h_expr;
    memset(&h_expr,0,sizeof(h_expr));
    h_expr.rtyp=IDHDL;
    h_expr.data=h;
    h_expr.name=vn;
    return iiAssign(&tmp_expr,&h_expr);
  }
  else
  {
    Werror("`%s` not found in `%s`",v->Name(), u->Name());
    return TRUE;
  }
  return FALSE;
}
