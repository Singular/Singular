#include "kernel/mod2.h" // general settings/macros

#ifdef SINGULAR_4_1
#include <reporter/reporter.h>  // for Print, WerrorS
#include <coeffs/numbers.h> // nRegister, coeffs.h
#include <coeffs/rmodulon.h> // ZnmInfo
#include <coeffs/bigintmat.h> // bigintmat
#include <coeffs/longrat.h> // BIGINTs: nlGMP

#include <Singular/blackbox.h> // blackbox type
#include <Singular/ipshell.h> // IsPrime

#include <Singular/ipid.h> // for SModulFunctions, leftv

#include <Singular/number2.h>

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
  int    i2=(int)(long)b->Data();
  if (c1->type==n_Z)
  {
    if (i2==IsPrime(i2))
    {
      res->data=(void *)nInitChar(n_Zp,(void*)(long)i2);
    }
    else
    {
      ZnmInfo info;
      mpz_ptr modBase= (mpz_ptr) omAlloc(sizeof(mpz_t));
      mpz_init_set_ui(modBase,i2);
      info.base= modBase;
      info.exp= 1;
      res->data=(void *)nInitChar(n_Zn,&info);
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
    number modBase= (number) omAlloc(sizeof(mpz_t));
    nlGMP(i2,modBase,coeffs_BIGINT); // FIXME? TODO? // extern void   nlGMP(number &i, number n, const coeffs r); // to be replaced with n_MPZ(modBase,i2,coeffs_BIGINT); // ?
    info.base= (mpz_ptr)modBase;
    info.exp= 1;
    res->data=(void *)nInitChar(n_Zn,&info);
    return FALSE;
  }
  return TRUE;
}

// -----------------------------------------------------------
// interpreter stuff for Number/number2
// -----------------------------------------------------------
BOOLEAN jjNUMBER2_OP2(leftv res, leftv a, leftv b)
{
  int op=iiOp;
  // binary operations for number2
  number2 a2=NULL;
  number aa=NULL;
  number2 b2=NULL;
  number bb=NULL;
  if (a->Typ()==CNUMBER_CMD)
  {
    a2=(number2)a->Data();
    aa=a2->n;
  }
  if (b->Typ()==CNUMBER_CMD)
  {
    b2=(number2)b->Data();
    if ((a2!=NULL) && (a2->cf!=b2->cf))
    {
      WerrorS("Number not compatible");
      return TRUE;
    }
    bb=b2->n;
  }
  number2 r=(number2)omAlloc(sizeof(*r));
  if (a2!=NULL) r->cf=a2->cf;
  else          r->cf=b2->cf;
  if (r->cf==NULL) op=0; // force error
  else
  if (a2==NULL)
  {
    if (a->Typ()==INT_CMD) aa=n_Init((long)a->Data(),r->cf);
    else if (a->Typ()==BIGINT_CMD)
    {
      //aa=n_Init_bigint((number)a->Data(),coeffs_BIGINT,r->cf);
      nMapFunc nMap=n_SetMap(coeffs_BIGINT,r->cf);
      aa=nMap((number)a->Data(),coeffs_BIGINT,r->cf);
    }
    else op=0;
  }
  if ((b2==NULL) &&(op!='^') &&(op!=0))
  {
    if (b->Typ()==INT_CMD) bb=n_Init((long)b->Data(),r->cf);
    else if (b->Typ()==BIGINT_CMD)
    {
      //bb=n_Init_bigint((number)b->Data(),coeffs_BIGINT,r->cf);
      nMapFunc nMap=n_SetMap(coeffs_BIGINT,r->cf);
      bb=nMap((number)b->Data(),coeffs_BIGINT,r->cf);
    }
    else op=0;
  }
  switch(op)
  {
    case '+': r->n=n_Add(aa,bb,r->cf);break;
    case '-': r->n=n_Sub(aa,bb,r->cf);break;
    case '*': r->n=n_Mult(aa,bb,r->cf);break;
    case '/': r->n=n_Div(aa,bb,r->cf);break;
    case '%': r->n=n_IntMod(aa,bb,r->cf);break;

    case '^': n_Power(aa,(int)(long)b->Data(),&(r->n),r->cf); break;

    default: Werror("unknown binary operation %s(%d)",Tok2Cmdname(op),op);
             omFree(r);
             return TRUE;
  }
  res->data=(void*)r;
  r->cf->ref++;
  return FALSE;
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

BOOLEAN jjEQUAL_CR(leftv res, leftv a, leftv b)
{
  coeffs a2=(coeffs)a->Data();
  coeffs b2=(coeffs)b->Data();
  res->data=(void*)(long)(a2==b2);
  return FALSE;
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

#include <coeffs/bigintmat.h>
BOOLEAN jjBIM2_CR(leftv res, leftv a)              // bigintmat ->cring
{
  bigintmat *b=(bigintmat*)a->Data();
  coeffs cf=b->basecoeffs();
  cf->ref++;
  res->data=(void*)cf;
  return FALSE;
}

BOOLEAN jjR2_CR(leftv res, leftv a)              // ring ->cring
{
  ring r=(ring)a->Data();
  coeffs cf=r->cf;
  cf->ref++;
  res->data=(void*)cf;
  return FALSE;
}
#endif
