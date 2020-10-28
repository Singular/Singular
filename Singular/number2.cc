#include "kernel/mod2.h" // general settings/macros
#include "reporter/reporter.h"  // for Print, WerrorS
#include "coeffs/numbers.h" // nRegister, coeffs.h
#include "coeffs/rmodulon.h" // ZnmInfo
#include "coeffs/bigintmat.h" // bigintmat
#include "coeffs/longrat.h" // BIGINTs: nlMPZ
#include "polys/ext_fields/algext.h" // AlgExtInfo
#include "misc/prime.h" // IsPrime
#include "Singular/blackbox.h" // blackbox type
#include "Singular/ipshell.h" // IsPrime
#include "Singular/ipconv.h" // iiConvert etc.

#include "Singular/ipid.h" // for SModulFunctions, leftv

#include "Singular/number2.h"

char *crString(coeffs c)
{
  if (c==NULL)
  {
    return omStrDup("oo");
  }
  return omStrDup(nCoeffName(c));
}
void crPrint(coeffs c)
{
  char *s=crString(c);
  PrintS(s);
  omFree(s);
}

// -----------------------------------------------------------
// interpreter stuff for cring/coeffs
// -----------------------------------------------------------
BOOLEAN jjCRING_Zp(leftv res, leftv a, leftv b)
{
  coeffs c1=(coeffs)a->Data();
  long   i2=(long)b->Data();
  if (c1->type==n_Z)
  {
    if (i2==IsPrime(i2))
    {
      #ifndef TEST_ZN_AS_ZP
      res->data=(void *)nInitChar(n_Zp,(void*)i2);
      #else
      ZnmInfo info;
      mpz_t modBase;
      mpz_init_set_ui(modBase,i2);
      info.base= modBase;
      info.exp= 1;
      res->data=(void *)nInitChar(n_Zn,&info);
      mpz_clear(modBase);
      #endif
    }
    else
    {
      ZnmInfo info;
      mpz_t modBase;
      mpz_init_set_ui(modBase,i2);
      info.base= modBase;
      info.exp= 1;
      if (mpz_popcount((mpz_ptr)modBase)==1) // is a power of 2
      {
        i2=SI_LOG2(i2);
        // is exponent <=2^(8*sizeof(unsigned long))
        if (i2<(8*sizeof(unsigned long)))
        {
          mpz_clear(modBase);
          res->data=(void *) nInitChar(n_Z2m,(void*)i2);
          return FALSE;
        }
        else
        {
          mpz_set_ui(modBase,2);
          info.exp=i2;
          res->data=(void *) nInitChar(n_Znm,&info);
          mpz_clear(modBase);
          return FALSE;
        }
      }
      res->data=(void *)nInitChar(n_Zn,&info);
      mpz_clear(modBase);
    }
    return FALSE;
  }
  return TRUE;
}
BOOLEAN jjCRING_Zm(leftv res, leftv a, leftv b)
{
  coeffs c1=(coeffs)a->Data();
  number i2=(number)b->Data();
  if (c1->type==n_Z)
  {
    ZnmInfo info;
    mpz_t modBase;
    nlMPZ(modBase,i2,coeffs_BIGINT);
    info.base= (mpz_ptr)modBase;
    info.exp= 1;
    if (mpz_popcount(modBase)==1) // is a power of 2
    {
      // is exponent <=2^(8*sizeof(unsigned long))
      mp_bitcnt_t l=mpz_scan1 (modBase,0);
      if ((l>0) && (l<=8*sizeof(unsigned long)))
      {
        res->data=(void *) nInitChar(n_Z2m,(void*)(long)l);
      }
      else
      {
        mpz_set_ui(modBase,2);
        info.exp= l;
        res->data=(void *) nInitChar(n_Znm,&info);
      }
      mpz_clear(modBase);
      return FALSE;
    }
    res->data=(void *)nInitChar(n_Zn,&info);
    mpz_clear(modBase);
    return FALSE;
  }
  return TRUE;
}

BOOLEAN jjEQUAL_CR(leftv res, leftv a, leftv b)
{
  coeffs a2=(coeffs)a->Data();
  coeffs b2=(coeffs)b->Data();
  res->data=(void*)(long)(a2==b2);
  if (iiOp==NOTEQUAL) res->data=(char *)(!(long)res->data);
  return FALSE;
}

BOOLEAN jjR2_CR(leftv res, leftv a)              // ring ->cring
{
  ring r=(ring)a->Data();
  AlgExtInfo extParam;
  extParam.r = r;
  coeffs cf=nInitChar(n_transExt,&extParam);
  res->data=(void*)cf;
  return FALSE;
}

#ifdef SINGULAR_4_2
// -----------------------------------------------------------
// interpreter stuff for Number/number2
// -----------------------------------------------------------
BOOLEAN jjNUMBER2_POW(leftv res, leftv a, leftv b)
{
  number2 a2=(number2)a->Data();
  if (a2->cf==NULL) return TRUE;
  number2 r=(number2)omAlloc0(sizeof(*r));
  r->cf=a2->cf;
  n_Power(a2->n,(int)(long)b->Data(),&(r->n),r->cf);
  return FALSE;
}
BOOLEAN jjNUMBER2_OP2(leftv res, leftv a, leftv b)
{
  int op=iiOp;
  // binary operations for number2
  number2 a2=NULL;
  number aa=NULL;
  number2 b2=NULL;
  number bb=NULL;
  leftv an = (leftv)omAlloc0Bin(sleftv_bin);
  leftv bn = (leftv)omAlloc0Bin(sleftv_bin);
  int ai,bi;
  int at=a->Typ();
  int bt=b->Typ();
  if ((ai=iiTestConvert(at,CNUMBER_CMD,dConvertTypes))!=0)
  {
    if ((bi=iiTestConvert(bt,CNUMBER_CMD,dConvertTypes))!=0)
    {
      iiConvert(at,CNUMBER_CMD,ai,a,an);
      iiConvert(bt,CNUMBER_CMD,bi,b,bn);
      a2=(number2)an->Data();
      b2=(number2)bn->Data();
      if (((a2!=NULL) && (b2!=NULL) && (a2->cf!=b2->cf))
      || (a2==NULL)
      || (b2==NULL))
      {
        an->CleanUp();
        bn->CleanUp();
        omFreeBin((ADDRESS)an, sleftv_bin);
        omFreeBin((ADDRESS)bn, sleftv_bin);
        WerrorS("Number not compatible");
        return TRUE;
      }
      aa=a2->n;
      bb=b2->n;
      number2 r=(number2)omAlloc0(sizeof(*r));
      r->cf=a2->cf;
      if (r->cf==NULL) op=0; // force error
      switch(op)
      {
        case '+': r->n=n_Add(aa,bb,r->cf);break;
        case '-': r->n=n_Sub(aa,bb,r->cf);break;
        case '*': r->n=n_Mult(aa,bb,r->cf);break;
        case '/': r->n=n_Div(aa,bb,r->cf);break;
        case '%': r->n=n_IntMod(aa,bb,r->cf);break;
        default: Werror("unknown binary operation %s(%d)",Tok2Cmdname(op),op);
             omFree(r);
             an->CleanUp();
             bn->CleanUp();
             omFreeBin((ADDRESS)an, sleftv_bin);
             omFreeBin((ADDRESS)bn, sleftv_bin);
             return TRUE;
      }
      res->data=(void*)r;
      r->cf->ref++;
      return FALSE;
    }
    else
    {
      an->CleanUp();
      omFreeBin((ADDRESS)an, sleftv_bin);
      Werror("cannot convert second operand (%s) to Number",b->Name());
      return TRUE;
    }
  }
  else
  {
    Werror("cannot convert first operand (%s) to Number",a->Name());
    return TRUE;
  }
}
BOOLEAN jjNUMBER2_OP1(leftv res, leftv a)
{
  int op=iiOp;
  // unary operations for number2
  number2 a2=(number2)a->Data();
  number2 r=(number2)omAlloc(sizeof(*r));
  r->cf=a2->cf;
  if (a2->cf==NULL) op=0; // force error
  switch(op)
  {
    case '-': r->n=n_Copy(a2->n,a2->cf);r->n=n_InpNeg(r->n,a2->cf);break;
    default: Werror("unknown unary operation %s(%d)",Tok2Cmdname(op),op);
             omFree(r);
             return TRUE;
  }
  res->data=(void*)r;
  r->cf->ref++;
  return FALSE;
}

BOOLEAN jjPOLY2_POW(leftv res, leftv a, leftv b)
{
  poly2 a2=(poly2)a->Data();
  if (a2->cf==NULL) return TRUE;
  poly2 r=(poly2)omAlloc0(sizeof(*r));
  r->cf=a2->cf;
  r->n=p_Power(p_Copy(a2->n,r->cf),(int)(long)b->Data(),r->cf);
  return FALSE;
}
BOOLEAN jjPOLY2_OP2(leftv res, leftv a, leftv b)
{
  int op=iiOp;
  // binary operations for poly2
  poly2 a2=NULL;
  poly aa=NULL;
  poly2 b2=NULL;
  poly bb=NULL;
  leftv an = (leftv)omAlloc0Bin(sleftv_bin);
  leftv bn = (leftv)omAlloc0Bin(sleftv_bin);
  int ai,bi;
  int at=a->Typ();
  int bt=b->Typ();
  if ((ai=iiTestConvert(at,CPOLY_CMD,dConvertTypes))!=0)
  {
    if ((bi=iiTestConvert(bt,CPOLY_CMD,dConvertTypes))!=0)
    {
      iiConvert(at,CPOLY_CMD,ai,a,an);
      iiConvert(bt,CPOLY_CMD,bi,b,bn);
      a2=(poly2)an->Data();
      b2=(poly2)bn->Data();
      if (((a2!=NULL) && (b2!=NULL) && (a2->cf!=b2->cf))
      || (a2==NULL)
      || (b2==NULL))
      {
        an->CleanUp();
        bn->CleanUp();
        omFreeBin((ADDRESS)an, sleftv_bin);
        omFreeBin((ADDRESS)bn, sleftv_bin);
        WerrorS("Poly not compatible");
        return TRUE;
      }
      aa=a2->n;
      bb=b2->n;
      poly2 r=(poly2)omAlloc0(sizeof(*r));
      r->cf=a2->cf;
      if (r->cf==NULL) op=0; // force error
      switch(op)
      {
        case '+': r->n=p_Add_q(p_Copy(aa,r->cf),p_Copy(bb,r->cf),r->cf);break;
        case '-': r->n=p_Sub(p_Copy(aa,r->cf),p_Copy(bb,r->cf),r->cf);break;
        case '*': r->n=pp_Mult_qq(aa,bb,r->cf);break;
        //case '/': r->n=n_Div(aa,bb,r->cf);break;
        //case '%': r->n=n_IntMod(aa,bb,r->cf);break;
        default: Werror("unknown binary operation %s(%d)",Tok2Cmdname(op),op);
             omFree(r);
             an->CleanUp();
             bn->CleanUp();
             omFreeBin((ADDRESS)an, sleftv_bin);
             omFreeBin((ADDRESS)bn, sleftv_bin);
             return TRUE;
      }
      res->data=(void*)r;
      r->cf->ref++;
      return FALSE;
    }
    else
    {
      an->CleanUp();
      omFreeBin((ADDRESS)an, sleftv_bin);
      Werror("cannot convert second operand (%s) to Poly",b->Name());
      return TRUE;
    }
  }
  else
  {
    Werror("cannot convert first operand (%s) to Poly",a->Name());
    return TRUE;
  }
}
BOOLEAN jjPOLY2_OP1(leftv res, leftv a)
{
  int op=iiOp;
  // unary operations for poly2
  poly2 a2=(poly2)a->Data();
  poly2 r=(poly2)omAlloc(sizeof(*r));
  r->cf=a2->cf;
  if (a2->cf==NULL) op=0; // force error
  switch(op)
  {
    case '-': r->n=p_Copy(a2->n,a2->cf);r->n=p_Neg(r->n,a2->cf);break;
    default: Werror("unknown unary operation %s(%d)",Tok2Cmdname(op),op);
             omFree(r);
             return TRUE;
  }
  res->data=(void*)r;
  r->cf->ref++;
  return FALSE;
}

BOOLEAN jjNUMBER2CR(leftv res, leftv a, leftv b)
{
  number2 r=(number2)omAlloc(sizeof(*r));
  r->cf=(coeffs)b->CopyD();
  BOOLEAN bo=FALSE;
  switch(a->Typ())
  {
    case INT_CMD:
      r->n=n_Init((long)a->Data(),r->cf); break;
    case BIGINT_CMD:
    {
      nMapFunc nMap=n_SetMap(coeffs_BIGINT,r->cf);
      r->n=nMap((number)a->Data(),coeffs_BIGINT,r->cf); break;
    }
    case NUMBER_CMD:
    {
      nMapFunc nMap=n_SetMap(currRing->cf,r->cf);
      if (nMap!=NULL)
        r->n=nMap((number)a->Data(),currRing->cf,r->cf);
      else
        bo=TRUE;
      break;
    }
    case CNUMBER_CMD:
    {
      number2 a2=(number2)a->Data();
      if (a2->cf==NULL) bo=TRUE;
      else
      {
        nMapFunc nMap=n_SetMap(a2->cf,r->cf);
        if (nMap!=NULL)
          r->n=nMap(a2->n,a2->cf,r->cf);
        else
          bo=TRUE;
      }
      break;
    }
    default: bo=TRUE; break;
  }
  if (bo)
  {
    Werror("no conversion to Number from %s",Tok2Cmdname(a->Typ()));
    omFreeSize(r,sizeof(*r));
  }
  else
    res->data=(void*)r;
  return bo;
}

BOOLEAN jjN2_CR(leftv res, leftv a)              // number2 ->cring
{
  number2 n=(number2)a->Data();
  n->cf->ref++;
  res->data=(void*)n->cf;
  return FALSE;
}

BOOLEAN jjP2_R(leftv res, leftv a)              // poly2 ->ring
{
  poly2 n=(poly2)a->Data();
  n->cf->ref++;
  res->data=(void*)n->cf;
  return FALSE;
}

BOOLEAN jjCM_CR(leftv res, leftv a)              // cmatrix ->cring
{
  bigintmat *b=(bigintmat*)a->Data();
  coeffs cf=b->basecoeffs();
  if (cf!=NULL)
  {
    cf->ref++;
  }
  res->data=(void*)cf;
  return FALSE;
}

BOOLEAN jjCMATRIX_3(leftv res, leftv r, leftv c,leftv cf)
{
  bigintmat *b=new bigintmat((int)(long)r->Data(),
                             (int)(long)c->Data(),
                             (coeffs)cf->Data());
  res->data=(char*)b;
  return FALSE;
}

BOOLEAN jjN2_N(leftv res, leftv a)              // number2 ->number
{
  number2 n2=(number2)a->Data();
  BOOLEAN bo=TRUE;
  if (currRing!=NULL)
  {
    nMapFunc nMap=n_SetMap(n2->cf,currRing->cf);
    if (nMap!=NULL)
    {
      res->data=(void*)nMap(n2->n,n2->cf,currRing->cf);
      bo=FALSE;
    }
  }
  return bo;
}

// -----------------------------------------------------------
// operations with Number/number2
// -----------------------------------------------------------
number2 n2Copy(const number2 d)
{
  number2 r=NULL;
  if ((d!=NULL)&&(d->cf!=NULL))
  {
    r=(number2)omAlloc(sizeof(*r));
    d->cf->ref++;
    r->cf=d->cf;
    if (d->cf!=NULL)
      r->n=n_Copy(d->n,d->cf);
    else
      r->n=NULL;
  }
  return r;
}
void n2Delete(number2 &d)
{
  if (d!=NULL)
  {
    if (d->cf!=NULL)
    {
      n_Delete(&d->n,d->cf);
      nKillChar(d->cf);
    }
    omFreeSize(d,sizeof(*d));
    d=NULL;
  }
}
char *n2String(number2 d, BOOLEAN typed)
{
  StringSetS("");
  if ((d!=NULL) && (d->cf!=NULL))
  {
    if (typed) StringAppendS("Number(");
    n_Write(d->n,d->cf);
    if (typed) StringAppendS(")");
  }
  else StringAppendS("oo");
  return StringEndS();
}

void n2Print(number2 d)
{
  char *s=n2String(d,FALSE);
  PrintS(s);
  omFree(s);
}

// -----------------------------------------------------------
// operations with Poly/poly2
// -----------------------------------------------------------

poly2 p2Copy(const poly2 d)
{
  poly2 r=NULL;
  if ((d!=NULL)&&(d->cf!=NULL))
  {
    r=(poly2)omAlloc(sizeof(*r));
    d->cf->ref++;
    r->cf=d->cf;
    if (d->cf!=NULL)
      r->n=p_Copy(d->n,d->cf);
    else
      r->n=NULL;
  }
  return r;
}
void p2Delete(poly2 &d)
{
  if (d!=NULL)
  {
    if (d->cf!=NULL)
    {
      p_Delete(&d->n,d->cf);
      rKill(d->cf);
    }
    omFreeSize(d,sizeof(*d));
    d=NULL;
  }
}
char *p2String(poly2 d, BOOLEAN typed)
{
  StringSetS("");
  if ((d!=NULL) && (d->cf!=NULL))
  {
    if (typed) StringAppendS("Poly(");
    p_Write0(d->n,d->cf);
    if (typed) StringAppendS(")");
  }
  else StringAppendS("oo");
  return StringEndS();
}

void p2Print(poly2 d)
{
  char *s=p2String(d,FALSE);
  PrintS(s);
  omFree(s);
}

// ---------------------------------------------------------------------
BOOLEAN jjBIM2_CR(leftv res, leftv a)              // bigintmat ->cring
{
  bigintmat *b=(bigintmat*)a->Data();
  coeffs cf=b->basecoeffs();
  if (cf==NULL) return TRUE;
  cf->ref++;
  res->data=(void*)cf;
  return FALSE;
}

#endif
