/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longalg.cc,v 1.66 2002-02-26 11:36:41 Singular Exp $ */
/*
* ABSTRACT:   algebraic numbers
*/

#include <stdio.h>
#include <string.h>
#include "mod2.h"
#include "structs.h"
#include "tok.h"
#include "omalloc.h"
#include "febase.h"
#include "longrat.h"
#include "modulop.h"
#include "numbers.h"
#include "polys.h"
#include "ideals.h"
#include "ipid.h"
#include "ring.h"
#ifdef HAVE_FACTORY
#include "clapsing.h"
#endif
#include "longalg.h"

naIdeal naI=NULL;

napoly naMinimalPoly;
int naNumbOfPar;
#ifndef LONGALGNEW
int napMonomSize;
char **naParNames;
#else /* LONGALGNEW */
#define naParNames (currRing->parameter)
#endif /* LONGALGNEW */
static int naIsChar0;
static int naPrimeM;

#ifdef LDEBUG
#define naTest(a) naDBTest(a,__FILE__,__LINE__)
BOOLEAN naDBTest(number a, char *f,int l);
#else
#define naTest(a)
#endif

number (*naMap)(number from);
/* procedure variables */
static numberfunc
                nacMult, nacSub, nacAdd, nacDiv, nacIntDiv;
static number   (*nacGcd)(number a, number b, const ring r);
static number   (*nacLcm)(number a, number b, const ring r);
static void     (*nacDelete)(number *a, const ring r);
       number   (*nacInit)(int i);
static int      (*nacInt)(number &n);
       void     (*nacNormalize)(number &a);
static number   (*nacNeg)(number a);
static void     (*nacWrite)(number &a);
       number   (*nacCopy)(number a);
static number   (*nacInvers)(number a);
       BOOLEAN  (*nacIsZero)(number a);
static BOOLEAN  (*nacIsOne)(number a);
static BOOLEAN  (*nacIsMOne)(number a);
static BOOLEAN  (*nacGreaterZero)(number a);
static char   * (*nacRead) (char *s, number *a);
#ifndef LONGALGNEW
extern void     napDelete(napoly *p);
#endif /* not LONGALGNEW */
static napoly napRedp(napoly q);
static napoly napTailred(napoly q);
static BOOLEAN napDivPoly(napoly p, napoly q);
static int napExpi(int i, napoly a, napoly b);

static number nadGcd( number a, number b, const ring r) { return nacInit(1); }
/*2
*  sets the appropriate operators
*/
void naSetChar(int i, ring r)
{
  if (naI!=NULL)
  {
    int j;
    for (j=naI->anz-1; j>=0; j--)
       napDelete (&naI->liste[j]);
    omFreeSize((ADDRESS)naI->liste,naI->anz*sizeof(napoly));
    omFreeBin((ADDRESS)naI, snaIdeal_bin);
    naI=NULL;
  }
  naMap = naCopy;
  /*------------ set naMinimalPoly -----------------------------------*/
  if (r->minpoly!=NULL)
    naMinimalPoly=((lnumber)r->minpoly)->z;
  else
    naMinimalPoly = NULL;
  naNumbOfPar=rPar(r);
#ifndef LONGALGNEW
  naParNames=r->parameter;
  napMonomSize = RECA_SIZE + naNumbOfPar*SIZEOF_PARAMETER;
#endif /* not LONGALGNEW */
  if (i == 1)
  {
    naIsChar0 = 1;
    nacDelete      = nlDelete;
    nacInit        = nlInit;
    nacInt         = nlInt;
    nacCopy        = nlCopy;
    nacAdd         = nlAdd;
    nacSub         = nlSub;
    nacMult        = nlMult;
    nacDiv         = nlDiv;
    nacIntDiv      = nlIntDiv;
    nacInvers      = nlInvers;
    nacNormalize   = nlNormalize;
    nacNeg         = nlNeg;
    nacIsZero      = nlIsZero;
    nacRead        = nlRead;
    nacWrite       = nlWrite;
    nacGreaterZero = nlGreaterZero;
    nacIsOne       = nlIsOne;
    nacIsMOne      = nlIsMOne;
    nacGcd         = nlGcd;
    nacLcm         = nlLcm;
  }
  else if (i < 0)
  {
    naIsChar0 = 0;
    nacDelete      = ndDelete;
    npSetChar(-i, r->algring); // to be changes HS
    nacInit        = npInit;
    nacInt         = npInt;
    nacCopy        = ndCopy;
    nacAdd         = npAdd;
    nacSub         = npSub;
    nacMult        = npMult;
    nacDiv         = npDiv;
    nacIntDiv      = npDiv;
    nacInvers      = npInvers;
    nacNormalize   = nDummy2;
    nacNeg         = npNeg;
    nacIsZero      = npIsZero;
    nacRead        = npRead;
    nacWrite       = npWrite;
    nacGreaterZero = npGreaterZero;
    nacIsOne       = npIsOne;
    nacIsMOne      = npIsMOne;
    nacGcd         = nadGcd;
    nacLcm         = nadGcd;
  }
#ifdef TEST
  else
  {
    Print("naSetChar:c=%d param=%d\n",i,rPar(r));
  }
#endif
}

/*============= procedure for polynomials: napXXXX =======================*/



#ifdef LDEBUG
static void napTest(napoly p)
{
  while (p != NULL)
  {
    if (naIsChar0)
      nlDBTest(napGetCoeff(p), "", 0);
    napIter(p);
  }
}
#else
#define napTest(p) ((void) 0)
#endif

#ifndef LONGALGNEW
/*3
* creates  an napoly poly
*/
static napoly napInit(int i)
{
  napoly a = (napoly)omAlloc0(napMonomSize);

  a->ko = nacInit(i);
  if (!nacIsZero(a->ko))
  {
    return a;
  }
  else
  {
    omFreeSize((ADDRESS)a, napMonomSize);
    return NULL;
  }
}
#define napMultCopy(p,q)  napMult(napCopy(p),napCopy(q))
#define napIsConstant(p)  (napDeg(p)==0)
#define napSetCoeff(p,n) {nacDelete(&((p)->ko),currRing);(p)->ko=n;}
#else /* LONGALGNEW */
#define napInit(i)       (napoly)p_ISet(i,currRing->algring)
#define napSetCoeff(p,n) {nacDelete(&napGetCoeff(p),currRing);napGetCoeff(p)=n;}
#define napDelete1(p)    p_LmDelete((poly *)p, currRing->algring)
#define napCopy(p)       (napoly)p_Copy((poly)p,currRing->algring)
#define nap_Copy(p,r)       (napoly)p_Copy((poly)p,r->algring)
#define napComp(p,q)     p_LmCmp((poly)p,(poly)q, currRing->algring)
#define napMultT(A,E)    A=(napoly)p_Mult_mm((poly)A,(poly)E,currRing->algring)
#define napMult(A,B)     (napoly)p_Mult_q((poly)A,(poly)B,currRing->algring)
#define napMultCopy(A,B)   (napoly)pp_Mult_qq((poly)A,(poly)B,currRing->algring)
#define napIsConstant(p) p_LmIsConstant(p,currRing->algring)
#define napDeg(p)        (int)p_ExpVectorQuerSum(p, currRing->algring)
#endif /* LONGALGNEW */

/*3
* creates  an napoly
*/
napoly napInitz(number z)
{
#ifndef LONGALGNEW
  napoly a = (napoly)omAlloc0(napMonomSize);
#else /* LONGALGNEW */
  napoly a = (napoly)p_Init(currRing->algring);
#endif /* LONGALGNEW */
  napGetCoeff(a) = z;
  return a;
}

#ifndef LONGALGNEW
/*3
*  deletes head of  an napoly poly
*/
static void napDelete1(napoly *p)
{
  napoly h = *p;

  if (h!=NULL)
  {
    *p = h->ne;
    nacDelete(&(h->ko),currRing);
    omFreeSize((ADDRESS)h, napMonomSize);
  }
}

/*3
*  delete an napoly poly
*/
void napDelete(napoly *p)
{
  napoly w, h = *p;

  while (h!=NULL)
  {
    w = h;
    h = h->ne;
    nacDelete(&(w->ko),currRing);
    omFreeSize((ADDRESS)w, napMonomSize);
  }
  *p = NULL;
}

void nap_Delete(napoly *p, const ring r)
{
  napoly w, h = *p;

  while (h!=NULL)
  {
    w = h;
    h = h->ne;
    n_Delete(&(w->ko),currRing->algring);
    omFree((ADDRESS)w);
  }
  *p = NULL;
}


/*3
* copy a napoly. poly
*/
static napoly napCopy(napoly p)
{
  if (p==NULL) return NULL;

  napoly w, a;

  omCheckAddrSize(p,napMonomSize);
  a = w = (napoly)omAlloc(napMonomSize);
  memcpy(w->e, p->e, naNumbOfPar * SIZEOF_PARAMETER);
  w->ko = nacCopy(p->ko);
  loop
  {
    p=p->ne;
    if (p==NULL) break;
    omCheckAddrSize(p,napMonomSize);
    a->ne = (napoly)omAlloc(napMonomSize);
    a = a->ne;
    memcpy(a->e, p->e, naNumbOfPar * SIZEOF_PARAMETER);
    a->ko = nacCopy(p->ko);
  }
  a->ne = NULL;
  return w;
}
#endif /* not LONGALGNEW */

/*3
* copy a napoly. poly
*/
static napoly napCopyNeg(napoly p)
{
#ifndef LONGALGNEW
  napoly w, a;

  if (p==NULL) return NULL;
  omCheckAddrSize(p,napMonomSize);
  a = w = (napoly)omAlloc(napMonomSize);
  memcpy(w->e, p->e, naNumbOfPar * SIZEOF_PARAMETER);
  w->ko = nacNeg(nacCopy(p->ko));
  loop
  {
    p=p->ne;
    if (p==NULL) break;
    omCheckAddrSize(p,napMonomSize);
    a->ne = (napoly)omAlloc(napMonomSize);
    a = a->ne;
    memcpy(a->e, p->e, naNumbOfPar * SIZEOF_PARAMETER);
    a->ko = nacNeg(nacCopy(p->ko));
  }
  a->ne = NULL;
  return w;
#else /* LONGALGNEW */
  napoly r=napCopy(p);
  r=(napoly)p_Neg((poly)r, currRing->algring);
  return r;
#endif /* LONGALGNEW */
}

#ifndef LONGALGNEW
/*3
* Compare exponents of p and q
*/
static int  napComp(napoly p, napoly q)
{
  int  i = 0;

  omCheckAddrSize(p,napMonomSize);
  omCheckAddrSize(q,napMonomSize);
  while (p->e[i] == q->e[i])
  {
    i++;
    if (i >= naNumbOfPar)
      return 0;
  }
  if (p->e[i]  >  q->e[i])
    return 1;
  else
    return - 1;
}

/*3
*  addition of napoly. polys
*/
napoly napAdd(napoly p1, napoly p2)
{
  napoly a1, p, a2, a;
  int  c;  number t;

  if (p1==NULL) return p2;
  if (p2==NULL) return p1;
  omCheckAddrSize(p1,napMonomSize);
  omCheckAddrSize(p2,napMonomSize);
  a1 = p1;
  a2 = p2;
  a = p  = (napoly)omAlloc(napMonomSize);
  loop
  {
    c = napComp(a1, a2);
    if (c == 1)
    {
      a = a->ne = a1;
      a1 = a1->ne;
      if (a1==NULL)
      {
        a->ne= a2;
        break;
      }
      omCheckAddrSize(a1,napMonomSize);
    }
    else if (c == -1)
    {
      a = a->ne = a2;
      a2 = a2->ne;
      if (a2==NULL)
      {
        a->ne = a1;
        break;
      }
      omCheckAddrSize(a2,napMonomSize);
    }
    else
    {
      t = nacAdd(a1->ko, a2->ko);
      napDelete1(&a2);
      if (nacIsZero(t))
      {
        nacDelete(&t,currRing);
        napDelete1(&a1);
      }
      else
      {
        nacDelete(&(a1->ko),currRing);
        a1->ko = t;
        a = a->ne = a1;
        a1 = a1->ne;
      }
      if (a1==NULL)
      {
        a->ne = a2;
        break;
      }
      else if (a2==NULL)
      {
        a->ne = a1;
        break;
      }
      omCheckAddrSize(a1,napMonomSize);
      omCheckAddrSize(a2,napMonomSize);
    }
  }
  a = p->ne;
  omFreeSize((ADDRESS)p, napMonomSize);
  return a;
}


/*3
* multiply a napoly. poly by -1
*/
napoly napNeg(napoly a)
{
  napoly p = a;

  while (p!=NULL)
  {
    omCheckAddrSize(p,napMonomSize);
    p->ko = nacNeg(p->ko);
    p = p->ne;
  }
  return a;
}
#endif /* not LONGALGNEW */

/*3
* returns ph * z
*/
static void napMultN(napoly p, number z)
{
  number t;

  while (p!=NULL)
  {
    t = nacMult(napGetCoeff(p), z);
    nacNormalize(t);
    nacDelete(&napGetCoeff(p),currRing);
    napGetCoeff(p) = t;
    napIter(p);
  }
}

#ifndef LONGALGNEW
/*3
* update the polynomial a by multipying it by
* the (number) coefficient
* and the exponent vector (of) exp (a well initialized polynomial)
*/
static void napMultT(napoly a, napoly exp)
{
  int  i;
  number t, h;
  if (a==NULL)
    return;
  h = exp->ko;
  if (nacIsOne(h))
  {
    do
    {
      for (i = naNumbOfPar - 1; i >= 0; i--)
        a->e[i] += exp->e[i];
      a = a->ne;
    }
    while (a!=NULL);
  }
  else
  {
    h = nacNeg(h);
    if (nacIsOne(h))
    {
      h = nacNeg(h);
      do
      {
        a->ko = nacNeg(a->ko);
        for (i = naNumbOfPar - 1; i >= 0; i--)
          a->e[i] += exp->e[i];
        a = a->ne;
      }
      while (a!=NULL);
    }
    else
    {
      h = nacNeg(h);
      do
      {
        t = nacMult(a->ko, h);
        nacDelete(&(a->ko),currRing);
        a->ko = t;
        for (i = naNumbOfPar - 1; i >= 0; i--)
          a->e[i] += exp->e[i];
        a = a->ne;
      }
      while (a!=NULL);
    }
    // need to do this, since nacNeg might have changed h
    exp->ko = h;
  }
}
#endif /* LONGALGNEW */

#ifndef LONGALGNEW
/*3
* multiplication of napoly. polys
* multiply p1 with p2, p1 and p2 are destroyed
*/
static napoly napMult(napoly p1, napoly p2)
{
  napoly a1, a2, b, h;
  if (p1==NULL)
  {
    napDelete(&p2);
    return NULL;
  }
  if (p2==NULL)
  {
    napDelete(&p1);
    return NULL;
  }
  b = NULL;
  a1 = p1;
  a2 = p2;
  if (a2->ne!=NULL)
  {
    if (a1->ne!=NULL)
    {
      do
      {
        a1 = a1->ne;
        a2 = a2->ne;
      }
      while ((a1!=NULL) && (a2!=NULL));
      if (a1!=NULL)
      {
        a1 = p1;
        a2 = p2;
      }
      else
      {
        a1 = p2;
        a2 = p1;
      }
      do
      {
        if (a2->ne!=NULL)
          h = napCopy(a1);
        else
          h = a1;
        napMultT(h, a2);
        b = napAdd(h, b);
        napDelete1(&a2);
      }
      while (a2!=NULL);
    }
    else
    {
      napMultT(a2, a1);
      b = napAdd(a2, b);
      napDelete1(&a1);
    }
  }
  else
  {
    napMultT(a1, a2);
    b = napAdd(a1, b);
    napDelete1(&a2);
  }
  return b;
}
#endif /* not LONGALGNEW */

/*3
*  division with rest; f = g*q + r, returns r and destroy f
*/
static napoly napRemainder(napoly f, const napoly  g)
{
  napoly a, h, qq;

#ifndef LONGALGNEW
  qq = (napoly)omAlloc(napMonomSize);
#else /* LONGALGNEW */
  qq = (napoly)p_Init(currRing->algring);
#endif /* LONGALGNEW */
  napNext(qq) = NULL;
  a = f;
  do
  {
    napSetExp(qq,1, napGetExp(a,1) - napGetExp(g,1));
    napGetCoeff(qq) = nacDiv(napGetCoeff(a), napGetCoeff(g));
    napGetCoeff(qq) = nacNeg(napGetCoeff(qq));
    h = napCopy(g);
    napMultT(h, qq);
    nacDelete(&napGetCoeff(qq),currRing);
    a = napAdd(a, h);
  }
  while ((a!=NULL) && (napGetExp(a,1) >= napGetExp(g,1)));
#ifndef LONGALGNEW
  omFreeSize((ADDRESS)qq, napMonomSize);
#else /* LONGALGNEW */
  omFreeBinAddr(qq);
#endif /* LONGALGNEW */
  return a;
}

/*3
*  division with rest; f = g*q + r,  destroy f
*/
static void napDivMod(napoly f, napoly  g, napoly *q, napoly *r)
{
  napoly a, h, b, qq;

#ifndef LONGALGNEW
  qq = (napoly)omAlloc(napMonomSize);
#else /* LONGALGNEW */
  qq = (napoly)p_Init(currRing->algring);
#endif /* LONGALGNEW */
  napNext(qq) = b = NULL;
  a = f;
  do
  {
    napSetExp(qq,1, napGetExp(a,1) - napGetExp(g,1));
    napGetCoeff(qq) = nacDiv(napGetCoeff(a), napGetCoeff(g));
    b = napAdd(b, napCopy(qq));
    napGetCoeff(qq) = nacNeg(napGetCoeff(qq));
    h = napCopy(g);
    napMultT(h, qq);
    nacDelete(&napGetCoeff(qq),currRing);
    a = napAdd(a, h);
  }
  while ((a!=NULL) && (napGetExp(a,1) >= napGetExp(g,1)));
#ifndef LONGALGNEW
  omFreeSize((ADDRESS)qq, napMonomSize);
#else /* LONGALGNEW */
  omFreeBinAddr(qq);
#endif /* LONGALGNEW */
  *q = b;
  *r = a;
}

/*3
*  returns z with z*x mod c = 1
*/
static napoly napInvers(napoly x, const napoly c)
{
  napoly y, r, qa, qn, q;
  number t, h;

  if (napGetExp(x,1) >= napGetExp(c,1))
    x = napRemainder(x, c);
  if (x==NULL)
  {
    goto zero_divisor;
  }
  if (napGetExp(x,1)==0)
  {
    if (!nacIsOne(napGetCoeff(x)))
    {
      h = nacInit(1);
      t = nacDiv(h, napGetCoeff(x));
      nacDelete(&napGetCoeff(x),currRing);
      nacDelete(&h,currRing);
      napGetCoeff(x) = t;
    }
    return x;
  }
  y = napCopy(c);
  napDivMod(y, x, &qa, &r);
  if (r==NULL)
  {
    goto zero_divisor;
  }
  if (napGetExp(r,1)==0)
  {
    h = nacInit(-1);
    t = nacDiv(h, napGetCoeff(r));
    nacNormalize(t);
    napMultN(qa, t);
    nacDelete(&h,currRing);
    nacDelete(&t,currRing);
    napDelete(&x);
    napDelete(&r);
    return qa;
  }
  y = x;
  x = r;
  napDivMod(y, x, &q, &r);
  if (r==NULL)
  {
    goto zero_divisor;
  }
  if (napGetExp(r,1)==0)
  {
    q = napMult(q, qa);
    q = napAdd(q, napInit(1));
    h = nacInit(1);
    t = nacDiv(h, napGetCoeff(r));
    napMultN(q, t);
    nacDelete(&h,currRing);
    nacDelete(&t,currRing);
    napDelete(&x);
    napDelete(&r);
    if (napGetExp(q,1) >= napGetExp(c,1))
      q = napRemainder(q, c);
    return q;
  }
  q = napMult(q, napCopy(qa));
  q = napAdd(q, napInit(1));
  qa = napNeg(qa);
  loop
  {
    y = x;
    x = r;
    napDivMod(y, x, &qn, &r);
    if (r==NULL)
    {
      break;
    }
    if (napGetExp(r,1)==0)
    {
      q = napMult(q, qn);
      q = napNeg(q);
      q = napAdd(q, qa);
      h = nacInit(1);
      t = nacDiv(h, napGetCoeff(r));
      napMultN(q, t);
      nacDelete(&h,currRing);
      nacDelete(&t,currRing);
      napDelete(&x);
      napDelete(&r);
      if (napGetExp(q,1) >= napGetExp(c,1))
        q = napRemainder(q, c);
      return q;
    }
    y = q;
    q = napMult(napCopy(q), qn);
    q = napNeg(q);
    q = napAdd(q, qa);
    qa = y;
  }
// zero divisor found:
zero_divisor:
  Werror("zero divisor found - your minpoly is not irreducible");
  return x;
}

#ifndef LONGALGNEW
/*3
* the degree of an napoly poly (used for test of "constant" et al.)
*/
static int  napDeg(napoly p)
{
  int  d = 0, i;

  omCheckAddrSize(p,napMonomSize);
  for (i = naNumbOfPar-1; i>=0; i--)
    d += p->e[i];
  return d;
}
#endif /* LONGALGNEW */

/*3
* the max degree of an napoly poly (used for test of "simple" et al.)
*/
static int  napMaxDeg(napoly p)
{
  int  d = 0;
  while(p!=NULL)
  {
    d=max(d,napDeg(p));
    napIter(p);
  }
  return d;
}

/*3
* the max degree of an napoly poly (used for test of "simple" et al.)
*/
static int  napMaxDegLen(napoly p, int &l)
{
  int  d = 0;
  int ll=0;
  while(p!=NULL)
  {
    d=max(d,napDeg(p));
    napIter(p);
    ll++;
  }
  l=ll;
  return d;
}


/*3
*writes a polynomial number
*/
void napWrite(napoly p)
{
  if (p==NULL)
    StringAppendS("0");
  else if (napIsConstant(p))
  {
    //StringAppendS("-1");
    nacWrite(napGetCoeff(p));
  }
  else
  {
    StringAppendS("(");
    loop
    {
      BOOLEAN wroteCoeff=FALSE;
      if ((napIsConstant(p))
      || ((!nacIsOne(napGetCoeff(p)))
        && (!nacIsMOne(napGetCoeff(p)))))
      {
        nacWrite(napGetCoeff(p));
        wroteCoeff=(currRing->ShortOut==0);
      }
      else if (nacIsMOne(napGetCoeff(p)))
      {
        StringAppendS("-");
      }
      int  i;
      for (i = 0; i < naNumbOfPar; i++)
      {
        if (napGetExp(p,i+1) > 0)
        {
          if (wroteCoeff)
            StringAppendS("*");
          else
            wroteCoeff=(currRing->ShortOut==0);
          StringAppendS(naParNames[i]);
          if (napGetExp(p,(i+1)) > 1)
          {
            if (currRing->ShortOut == 0)
              StringAppendS("^");
            StringAppend("%d", napGetExp(p,i+1));
          }
        }
      }
      napIter(p);
      if (p==NULL)
        break;
      if (nacGreaterZero(napGetCoeff(p)))
        StringAppendS("+");
    }
    StringAppendS(")");
  }
}


#ifndef LONGALGNEW
static char *napHandleMons(char *s, int i, PARAMETER_TYPE *ex)
#else /* LONGALGNEW */
static char *napHandleMons(char *s, int i, napoly ex)
#endif /* LONGALGNEW */
{
  int  j;
  if (strncmp(s,naParNames[i],strlen(naParNames[i]))==0)
  {
    s+=strlen(naParNames[i]);
    if ((*s >= '0') && (*s <= '9'))
    {
      s = eati(s, &j);
#ifndef LONGALGNEW
      ex[i] += j;
#else /* LONGALGNEW */
      napAddExp(ex,i+1,j);
#endif /* LONGALGNEW */
    }
    else
#ifndef LONGALGNEW
      ex[i]++;
#else /* LONGALGNEW */
      napAddExp(ex,i+1,1);
#endif /* LONGALGNEW */
  }
  return s;
}

/*3  reads a monomial  */
static char  *napRead(char *s, napoly *b)
{
  napoly a;
  int  i;
#ifndef LONGALGNEW
  a = (napoly)omAlloc0(napMonomSize);
#else /* LONGALGNEW */
  a = (napoly)p_Init(currRing->algring);
#endif /* LONGALGNEW */
  if ((*s >= '0') && (*s <= '9'))
  {
    s = nacRead(s, &napGetCoeff(a));
    if (nacIsZero(napGetCoeff(a)))
    {
      napDelete1(&a);
      *b = NULL;
      return s;
    }
  }
  else
    napGetCoeff(a) = nacInit(1);
  i = 0;
  char  *olds;
  loop
  {
    olds = s;
#ifndef LONGALGNEW
    s = napHandleMons(s, i, a->e);
#else /* LONGALGNEW */
    s = napHandleMons(s, i, a);
#endif /* LONGALGNEW */
    if (olds == s)
      i++;
    else
      i = 0;
    if ((*s == '\0') || (i >= naNumbOfPar))
      break;
  }
  *b = a;
  return s;
}

static int napExp(napoly a, napoly b)
{
  while (napNext(a)!=NULL) napIter(a);
  int m = napGetExp(a,1);
  if (m==0) return 0;
  while (napNext(b)!=NULL) napIter(b);
  int mm=napGetExp(b,1);
  if (m > mm) m = mm;
  return m;
}

/*
* finds the smallest i-th exponent in a and b
* used to find it in a fraction
*/
static int napExpi(int i, napoly a, napoly b)
{
  if (a==NULL || b==NULL) return 0;
  int m = napGetExp(a,i+1);
  if (m==0) return 0;
  while (napNext(a) != NULL)
  {
    napIter(a);
    if (m > napGetExp(a,i+1))
    {
      m = napGetExp(a,i+1);
      if (m==0) return 0;
    }
  }
  do
  {
    if (m > napGetExp(b,i+1))
    {
      m = napGetExp(b,i+1);
      if (m==0) return 0;
    }
    napIter(b);
  }
  while (b != NULL);
  return m;
}

static void napContent(napoly ph)
{
  number h,d;
  napoly p;

  p = ph;
  if (nacIsOne(napGetCoeff(p)))
    return;
  h = nacCopy(napGetCoeff(p));
  napIter(p);
  do
  {
    d=nacGcd(napGetCoeff(p), h, currRing);
    if(nacIsOne(d))
    {
      nacDelete(&h,currRing);
      nacDelete(&d,currRing);
      return;
    }
    nacDelete(&h,currRing);
    h = d;
    napIter(p);
  }
  while (p!=NULL);
  h = nacInvers(d);
  nacDelete(&d,currRing);
  p = ph;
  while (p!=NULL)
  {
    d = nacMult(napGetCoeff(p), h);
    nacDelete(&napGetCoeff(p),currRing);
    napGetCoeff(p) = d;
    napIter(p);
  }
  nacDelete(&h,currRing);
}

static void napCleardenom(napoly ph)
{
  number d, h;
  napoly p;

  if (!naIsChar0)
    return;
  p = ph;
  h = nacInit(1);
  while (p!=NULL)
  {
    d = nacLcm(h, napGetCoeff(p), currRing);
    nacDelete(&h,currRing);
    h = d;
    napIter(p);
  }
  if(!nacIsOne(h))
  {
    p = ph;
    while (p!=NULL)
    {
      d=nacMult(h, napGetCoeff(p));
      nacDelete(&napGetCoeff(p),currRing);
      nacNormalize(d);
      napGetCoeff(p) = d;
      napIter(p);
    }
    nacDelete(&h,currRing);
  }
  napContent(ph);
}

static napoly napGcd0(napoly a, napoly b)
{
  number x, y;
  if (!naIsChar0)
    return napInit(1);
  x = nacCopy(napGetCoeff(a));
  if (nacIsOne(x))
    return napInitz(x);
  while (napNext(a)!=NULL)
  {
    napIter(a);
    y = nacGcd(x, napGetCoeff(a), currRing);
    nacDelete(&x,currRing);
    x = y;
    if (nacIsOne(x))
      return napInitz(x);
  }
  do
  {
    y = nacGcd(x, napGetCoeff(b), currRing);
    nacDelete(&x,currRing);
    x = y;
    if (nacIsOne(x))
      return napInitz(x);
    napIter(b);
  }
  while (b!=NULL);
  return napInitz(x);
}

/*3
* result =gcd(a,b)
*/
static napoly napGcd(napoly a, napoly b)
{
  int i;
  napoly g, x, y, h;
  if ((a==NULL)
  || ((napNext(a)==NULL)&&(nacIsZero(napGetCoeff(a)))))
  {
    if ((b==NULL)
    || ((napNext(b)==NULL)&&(nacIsZero(napGetCoeff(b)))))
    {
      return napInit(1);
    }
    return napCopy(b);
  }
  else
  if ((b==NULL)
  || ((napNext(b)==NULL)&&(nacIsZero(napGetCoeff(b)))))
  {
    return napCopy(a);
  }
  if (naMinimalPoly != NULL)
  {
    if (napGetExp(a,1) >= napGetExp(b,1))
    {
      x = a;
      y = b;
    }
    else
    {
      x = b;
      y = a;
    }
    if (!naIsChar0) g = napInit(1);
    else            g = napGcd0(x, y);
    if (napNext(y)==NULL)
    {
      napSetExp(g,1, napExp(x, y));
      return g;
    }
    x = napCopy(x);
    y = napCopy(y);
    loop
    {
      h = napRemainder(x, y);
      if (h==NULL)
      {
        napCleardenom(y);
        if (!nacIsOne(napGetCoeff(g)))
          napMultN(y, napGetCoeff(g));
        napDelete1(&g);
        return y;
      }
      else if (napNext(h)==NULL)
        break;
      x = y;
      y = h;
    }
    napDelete(&y);
    napDelete1(&h);
    napSetExp(g,1, napExp(a, b));
    return g;
  }
  // Hmm ... this is a memory leak
#ifndef LONGALGNEW
  // x = (napoly)omAlloc0(napMonomSize);
#else /* LONGALGNEW */
  // x = (napoly)p_Init(currRing->algring);
#endif /* LONGALGNEW */
  g=a;
  h=b;
  if (!naIsChar0) x = napInit(1);
  else            x = napGcd0(g,h);
  for (i=(naNumbOfPar-1); i>=0; i--)
  {
    napSetExp(x,i+1, napExpi(i,a,b));
  }
  return x;
}


number napLcm(napoly a)
{
  number h = nacInit(1);

  if (naIsChar0)
  {
    number d;
    napoly b = a;

    while (b!=NULL)
    {
      d = nacLcm(h, napGetCoeff(b), currRing);
      nacDelete(&h,currRing);
      h = d;
      napIter(b);
    }
  }
  return h;
}


/*2
* meins  (for reduction in algebraic extension)
* checks if head of p divides head of q
* doesn't delete p and q
*/
static BOOLEAN napDivPoly (napoly p, napoly q)
{
#ifndef LONGALGNEW
  int j=0;   /* evtl. von naNumber.. -1 abwaerts zaehlen */
#else /* LONGALGNEW */
  int j=1;   /* evtl. von naNumber.. -1 abwaerts zaehlen */
#endif /* LONGALGNEW */

#ifndef LONGALGNEW
  while (p->e[j] <= q->e[j])
#else /* LONGALGNEW */
  while (napGetExp(p,j) <= napGetExp(q,j))
#endif /* LONGALGNEW */
  {
    j++;
#ifndef LONGALGNEW
    if (j >= naNumbOfPar)
#else /* LONGALGNEW */
    if (j > naNumbOfPar)
#endif /* LONGALGNEW */
      return 1;
  }
  return 0;
}


/*2
* meins  (for reduction in algebraic extension)
* Normalform of poly with naI
* changes q and returns it
*/
napoly napRedp (napoly q)
{
#ifndef LONGALGNEW
  napoly h = (napoly)omAlloc0(napMonomSize);
#else /* LONGALGNEW */
  napoly h = (napoly)p_Init(currRing->algring);
#endif /* LONGALGNEW */
  int i=0,j;

  loop
  {
    if (napDivPoly (naI->liste[i], q))
    {
      /* h = lt(q)/lt(naI->liste[i])*/
      napGetCoeff(h) = nacCopy(napGetCoeff(q));
      for (j=naNumbOfPar; j>0; j--)
        napSetExp(h,j, napGetExp(q,j) - napGetExp(naI->liste[i],j));
      h = napMult (h, napCopy(naI->liste[i]));
      h = napNeg (h);
      q = napAdd (q, napCopy(h));
      napDelete (&napNext(h));
      if (q == NULL)
      {
        napDelete(&h);
        return q;
      }
      /* try to reduce further */
      i = 0;
    }
    else
    {
      i++;
      if (i >= naI->anz)
      {
        napDelete(&h);
        return q;
      }
    }
  }
}


/*2
* meins  (for reduction in algebraic extension)
* reduces the tail of Poly q
* needs q != NULL
* changes q and returns it
*/
napoly napTailred (napoly q)
{
  napoly h;

  h = napNext(q);
  while (h != NULL)
  {
     h = napRedp (h);
     if (h == NULL)
        return q;
     napIter(h);
  }
  return q;
}


/*================ procedure for rational functions: naXXXX =================*/

/*2
*  z:= i
*/
number naInit(int i)
{
  if (i!=0)
  {
#ifndef LONGALGNEW
    lnumber l = (lnumber)omAllocBin(rnumber_bin);
    l->z = napInit(i);
    if (l->z==NULL)
    {
      omFreeBin((ADDRESS)l,  rnumber_bin);
      return NULL;
    }
    l->s = 2;
    l->n = NULL;
    return (number)l;
#else /* LONGALGNEW */
    napoly z=napInit(i);
    if (z!=NULL)
    {
      lnumber l = (lnumber)omAllocBin(rnumber_bin);
      l->z = z;
      l->s = 2;
      l->n = NULL;
      return (number)l;
    }
#endif /* LONGALGNEW */
  }
  /*else*/
  return NULL;
}

number  naPar(int i)
{
  lnumber l = (lnumber)omAllocBin(rnumber_bin);
  l->s = 2;
  l->z = napInit(1);
  napSetExp(l->z,i,1);
  l->n = NULL;
  return (number)l;
}

int     naParDeg(number n)     /* i := deg(n) */
{
  lnumber l = (lnumber)n;
  if (l==NULL) return -1;
  return napDeg(l->z);
}

//int     naParDeg(number n)     /* i := deg(n) */
//{
//  lnumber l = (lnumber)n;
//  if (l==NULL) return -1;
//  return napMaxDeg(l->z)+napMaxDeg(l->n);
//}

int     naSize(number n)     /* size desc. */
{
  lnumber l = (lnumber)n;
  if (l==NULL) return -1;
  int len_z;
  int len_n;
  int o=napMaxDegLen(l->z,len_z)+napMaxDegLen(l->n,len_n);
  return (len_z+len_n)+o;
}

/*2
* convert a number to int (if possible)
*/
int naInt(number &n)
{
  lnumber l=(lnumber)n;
  if ((l!=NULL)&&(l->n==NULL)&&(napIsConstant(l->z)))
  {
    return nacInt(napGetCoeff(l->z));
  }
  return 0;
}

/*2
*  deletes p
*/
void naDelete(number *p, const ring r)
{
  lnumber l = (lnumber) * p;
  if (l==NULL) return;
  nap_Delete(&(l->z),r);
  nap_Delete(&(l->n),r);
  omFreeBin((ADDRESS)l,  rnumber_bin);
  *p = NULL;
}

/*2
* copy p to erg
*/
number naCopy(number p)
{
  if (p==NULL) return NULL;
  naTest(p);
  lnumber erg;
  lnumber src = (lnumber)p;
  erg = (lnumber)omAlloc0Bin(rnumber_bin);
  erg->z = napCopy(src->z);
  erg->n = napCopy(src->n);
  erg->s = src->s;
  return (number)erg;
}
number na_Copy(number p, ring r)
{
  if (p==NULL) return NULL;
  lnumber erg;
  lnumber src = (lnumber)p;
  erg = (lnumber)omAlloc0Bin(rnumber_bin);
  erg->z = nap_Copy(src->z,r);
  erg->n = nap_Copy(src->n,r);
  erg->s = src->s;
  return (number)erg;
}

/*2
* a dummy number: 0
*/
void naNew(number *z)
{
  *z = NULL;
}

/*2
*  addition; lu:= la + lb
*/
number naAdd(number la, number lb)
{
  napoly x, y;
  lnumber lu;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  if (a==NULL) return naCopy(lb);
  if (b==NULL) return naCopy(la);
  omCheckAddrSize(a,sizeof(snumber));
  omCheckAddrSize(b,sizeof(snumber));
  lu = (lnumber)omAllocBin(rnumber_bin);
  if (b->n!=NULL) x = napMultCopy(a->z, b->n);
  else            x = napCopy(a->z);
  if (a->n!=NULL) y = napMultCopy(b->z, a->n);
  else            y = napCopy(b->z);
  lu->z = napAdd(x, y);
  if (lu->z==NULL)
  {
    omFreeBin((ADDRESS)lu,  rnumber_bin);
    return (number)NULL;
  }
  if (a->n!=NULL)
  {
    if (b->n!=NULL) x = napMultCopy(a->n, b->n);
    else            x = napCopy(a->n);
  }
  else
  {
    if (b->n!=NULL) x = napCopy(b->n);
    else            x = NULL;
  }
#ifndef LONGALGNEW
  if ((x!=NULL) && (napDeg(x)==0) && nacIsOne(x->ko))
    napDelete(&x);
#else /* LONGALGNEW */
  if (x!=NULL)
  {
    assume(!napIsConstant(x));
  }
#endif /* LONGALGNEW */
  lu->n = x;
  lu->s = 0;
#ifdef LONGALGNEW
  if (lu->n!=NULL)
  {
     number luu=(number)lu;
     naNormalize(luu);
     lu=(lnumber)luu;
  }
#endif /* LONGALGNEW */
  naTest((number)lu);
  return (number)lu;
}

/*2
*  subtraction; r:= la - lb
*/
number naSub(number la, number lb)
{
  lnumber lu;

  if (lb==NULL) return naCopy(la);
  if (la==NULL)
  {
    lu = (lnumber)naCopy(lb);
    lu->z = napNeg(lu->z);
    return (number)lu;
  }

  napoly x, y;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;

  omCheckAddrSize(a,sizeof(snumber));
  omCheckAddrSize(b,sizeof(snumber));
  lu = (lnumber)omAllocBin(rnumber_bin);
  if (b->n!=NULL) x = napMultCopy(a->z, b->n);
  else            x = napCopy(a->z);
  if (a->n!=NULL) y = napMult(napCopy(b->z), napCopyNeg(a->n));
  else            y = napCopyNeg(b->z);
  lu->z = napAdd(x, y);
  if (lu->z==NULL)
  {
    omFreeBin((ADDRESS)lu,  rnumber_bin);
    return (number)NULL;
  }
  if (a->n!=NULL)
  {
    if (b->n!=NULL) x = napMultCopy(a->n, b->n);
    else            x = napCopy(a->n);
  }
  else
  {
    if (b->n!=NULL) x = napCopy(b->n);
    else            x = NULL;
  }
#ifndef LONGALGNEW
  if ((x!=NULL)&& (napDeg(x)==0) && nacIsOne(x->ko))
    napDelete(&x);
#else /* LONGALGNEW */
  if (x!=NULL)
  {
    assume(!napIsConstant(x));
  }
#endif /* LONGALGNEW */
  lu->n = x;
  lu->s = 0;
#ifdef LONGALGNEW
  if (lu->n!=NULL)
  {
     number luu=(number)lu;
     naNormalize(luu);
     lu=(lnumber)luu;
  }
#endif /* LONGALGNEW */
  naTest((number)lu);
  return (number)lu;
}

/*2
*  multiplication; r:= la * lb
*/
number naMult(number la, number lb)
{
  if ((la==NULL) || (lb==NULL))
    return NULL;

  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  lnumber lo;
  napoly x;

  omCheckAddrSize(a,sizeof(snumber));
  omCheckAddrSize(b,sizeof(snumber));
  naTest(la);
  naTest(lb);

  lo = (lnumber)omAllocBin(rnumber_bin);
  lo->z = napMultCopy(a->z, b->z);

  if (a->n==NULL)
  {
    if (b->n==NULL)
      x = NULL;
    else
      x = napCopy(b->n);
  }
  else
  {
    if (b->n==NULL)
    {
      x = napCopy(a->n);
    }
    else
    {
      x = napMultCopy(b->n, a->n);
    }
  }
  if (naMinimalPoly!=NULL)
  {
    if (napGetExp(lo->z,1) >= napGetExp(naMinimalPoly,1))
      lo->z = napRemainder(lo->z, naMinimalPoly);
    if ((x!=NULL) && (napGetExp(x,1) >= napGetExp(naMinimalPoly,1)))
      x = napRemainder(x, naMinimalPoly);
  }
  if (naI!=NULL)
  {
    lo->z = napRedp (lo->z);
    if (lo->z != NULL)
       lo->z = napTailred (lo->z);
    if (x!=NULL)
    {
      x = napRedp (x);
      if (x!=NULL)
        x = napTailred (x);
    }
  }
  if ((x!=NULL) && (napIsConstant(x)) && nacIsOne(napGetCoeff(x)))
    napDelete(&x);
  lo->n = x;
  lo->s = 0;
  if(lo->z==NULL)
  {
    omFreeBin((ADDRESS)lo, rnumber_bin);
    lo=NULL;
  }
#if 0
  if (lo->n!=NULL)
  {
     number luu=(number)lo;
     naNormalize(luu);
     lo=(lnumber)luu;
  }
#endif
  naTest((number)lo);
  return (number)lo;
}

number naIntDiv(number la, number lb)
{
  lnumber res;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
#ifndef LONGALGNEW
  if ((a==NULL) || (a->z==NULL))
#else /* LONGALGNEW */
  if (a==NULL)
  {
    assume(a->z!=NULL);
#endif /* LONGALGNEW */
    return NULL;
#ifndef LONGALGNEW
  if ((b==NULL) || (b->z==NULL))
#else /* LONGALGNEW */
  }
  if (b==NULL)
#endif /* LONGALGNEW */
  {
#ifdef LONGALGNEW
    assume(b->z!=NULL);
#endif /* LONGALGNEW */
    WerrorS("div. by 0");
    return NULL;
  }
  assume(a->z!=NULL && b->z!=NULL);
  assume(a->n==NULL && b->n==NULL);
  res = (lnumber)omAllocBin(rnumber_bin);
  res->z = napCopy(a->z);
  res->n = napCopy(b->z);
  res->s = 0;
  number nres=(number)res;
  naNormalize(nres);

  //napDelete(&res->n);
  naTest(nres);
  return nres;
}

/*2
*  division; lo:= la / lb
*/
number naDiv(number la, number lb)
{
  lnumber lo;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  napoly x;

  if ((a==NULL) || (a->z==NULL))
    return NULL;

  if ((b==NULL) || (b->z==NULL))
  {
    WerrorS("div. by 0");
    return NULL;
  }
  omCheckAddrSize(a,sizeof(snumber));
  omCheckAddrSize(b,sizeof(snumber));
  lo = (lnumber)omAllocBin(rnumber_bin);
  if (b->n!=NULL)
    lo->z = napMultCopy(a->z, b->n);
  else
    lo->z = napCopy(a->z);
  if (a->n!=NULL)
    x = napMultCopy(b->z, a->n);
  else
    x = napCopy(b->z);
  if (naMinimalPoly!=NULL)
  {
    if (napGetExp(lo->z,1) >= napGetExp(naMinimalPoly,1))
      lo->z = napRemainder(lo->z, naMinimalPoly);
    if (napGetExp(x,1) >= napGetExp(naMinimalPoly,1))
      x = napRemainder(x, naMinimalPoly);
  }
  if (naI!=NULL)
  {
    lo->z = napRedp (lo->z);
    if (lo->z != NULL)
       lo->z = napTailred (lo->z);
    if (x!=NULL)
    {
      x = napRedp (x);
      if (x!=NULL)
        x = napTailred (x);
    }
  }
  if ((napIsConstant(x)) && nacIsOne(napGetCoeff(x)))
    napDelete(&x);
  lo->s = 0;
  lo->n = x;
#ifdef LONGALGNEW
  if (lo->n!=NULL)
  {
     number luu=(number)lo;
     naNormalize(luu);
     lo=(lnumber)luu;
  }
#endif /* LONGALGNEW */
  naTest((number)lo);
  return (number)lo;
}

/*2
*  za:= - za
*/
number naNeg(number za)
{
  if (za!=NULL)
  {
    lnumber e = (lnumber)za;
    naTest(za);
    e->z = napNeg(e->z);
  }
  return za;
}

/*2
* 1/a
*/
number naInvers(number a)
{
  lnumber lo;
  lnumber b = (lnumber)a;
  napoly x;

  if (b==NULL)
  {
    WerrorS("div. by 0");
    return NULL;
  }
  omCheckAddrSize(b,sizeof(snumber));
  lo = (lnumber)omAlloc0Bin(rnumber_bin);
  lo->s = b->s;
  if (b->n!=NULL)
    lo->z = napCopy(b->n);
  else
    lo->z = napInit(1);
  x = b->z;
  if ((!napIsConstant(x)) || !nacIsOne(napGetCoeff(x)))
    x = napCopy(x);
  else
  {
    lo->n = NULL;
    naTest((number)lo);
    return (number)lo;
  }
  if (naMinimalPoly!=NULL)
  {
    x = napInvers(x, naMinimalPoly);
    x = napMult(x, lo->z);
    if (napGetExp(x,1) >= napGetExp(naMinimalPoly,1))
      x = napRemainder(x, naMinimalPoly);
    lo->z = x;
    lo->n = NULL;
    lo->s = 2;
    while (x!=NULL)
    {
      nacNormalize(napGetCoeff(x));
      napIter(x);
    }
  }
  else
    lo->n = x;
#ifdef LONGALGNEW
  if (lo->n!=NULL)
  {
     number luu=(number)lo;
     naNormalize(luu);
     lo=(lnumber)luu;
  }
#endif /* LONGALGNEW */
  naTest((number)lo);
  return (number)lo;
}


BOOLEAN naIsZero(number za)
{
  lnumber zb = (lnumber)za;
  naTest(za);
#ifdef LDEBUG
  if ((zb!=NULL) && (zb->z==NULL)) WerrorS("internal zero error(2)");
#endif
  return ((zb==NULL) || (zb->z==NULL));
}


BOOLEAN naGreaterZero(number za)
{
  lnumber zb = (lnumber)za;
#ifdef LDEBUG
  if ((zb!=NULL) && (zb->z==NULL)) WerrorS("internal zero error(3)");
#endif
  naTest(za);
  if ((zb!=NULL) && (zb->z!=NULL))
  {
    if (zb->n!=NULL)
    {
      if ((napIsConstant(zb->z)) && !nacGreaterZero(napGetCoeff(zb->z)))
        return FALSE;
      return TRUE;
    }
    if ((napIsConstant(zb->z)) && !nacGreaterZero(napGetCoeff(zb->z)))
      return FALSE;
  }
  return TRUE;
}


/*2
* a = b ?
*/
BOOLEAN naEqual (number a, number b)
{
  if(a==b) return TRUE;
  if((a==NULL)&&(b!=NULL)) return FALSE;
  if((b==NULL)&&(a!=NULL)) return FALSE;

  lnumber aa=(lnumber)a;
  lnumber bb=(lnumber)b;

  int an_deg=0;
  if(aa->n!=NULL)
    an_deg=napDeg(aa->n);
  int bn_deg=0;
  if(bb->n!=NULL)
    bn_deg=napDeg(bb->n);
  if(an_deg+napDeg(bb->z)!=bn_deg+napDeg(aa->z))
    return FALSE;
#if 0
  naNormalize(a);
  aa=(lnumber)a;
  naNormalize(b);
  bb=(lnumber)b;
  if((aa->n==NULL)&&(bb->n!=NULL)) return FALSE;
  if((bb->n==NULL)&&(aa->n!=NULL)) return FALSE;
  if(napComp(aa->z,bb->z)!=0) return FALSE;
  if((aa->n!=NULL) && (napComp(aa->n,bb->n))) return FALSE;
#endif
  number h = naSub(a, b);
  BOOLEAN bo = naIsZero(h);
  naDelete(&h,currRing);
  return bo;
}


BOOLEAN naGreater (number a, number b)
{
  if (naIsZero(a))
    return FALSE;
  if (naIsZero(b))
    return TRUE; /* a!= 0)*/
  return napDeg(((lnumber)a)->z)>napDeg(((lnumber)b)->z);
}

/*2
* reads a number
*/
char  *naRead(char *s, number *p)
{
  napoly x;
  lnumber a;
  s = napRead(s, &x);
  if (x==NULL)
  {
    *p = NULL;
    return s;
  }
  *p = (number)omAlloc0Bin(rnumber_bin);
  a = (lnumber)*p;
  if ((naMinimalPoly!=NULL)
  && (napGetExp(x,1) >= napGetExp(naMinimalPoly,1)))
    a->z = napRemainder(x, naMinimalPoly);
  else if (naI!=NULL)
  {
    a->z = napRedp(x);
    if (a->z != NULL)
      a->z = napTailred (a->z);
  }
  else
    a->z = x;
  if(a->z==NULL)
  {
    omFreeBin((ADDRESS)*p, rnumber_bin);
    *p=NULL;
  }
  else
  {
    a->n = NULL;
    a->s = 0;
    naTest(*p);
  }
  return s;
}

/*2
* tries to convert a number to a name
*/
char * naName(number n)
{
  lnumber ph = (lnumber)n;
  if ((ph==NULL)||(ph->z==NULL))
    return NULL;
  int i;
  char *s=(char *)omAlloc(4* naNumbOfPar);
  char *t=(char *)omAlloc(8);
  s[0]='\0';
  for (i = 0; i <= naNumbOfPar - 1; i++)
  {
    if (napGetExp(ph->z,i+1) > 0)
    {
      if (napGetExp(ph->z,i+1) >1)
      {
        sprintf(t,"%s%d",naParNames[i],(int)napGetExp(ph->z,i+1));
        strcat(s,t);
      }
      else
      {
        strcat(s,naParNames[i]);
      }
    }
  }
  omFreeSize((ADDRESS)t,8);
  if (s[0]=='\0')
  {
    omFree((ADDRESS)s);
    return NULL;
  }
  return s;
}

/*2
*  writes a number
*/
void naWrite(number &phn)
{
  lnumber ph = (lnumber)phn;
  if ((ph==NULL)||(ph->z==NULL))
    StringAppendS("0");
  else
  {
    phn->s = 0;
    naNormalize(phn);
    napWrite(ph->z);
    if (ph->n!=NULL)
    {
      StringAppendS("/");
      napWrite(ph->n);
    }
  }
}

/*2
* za == 1 ?
*/
BOOLEAN naIsOne(number za)
{
  lnumber a = (lnumber)za;
  napoly x, y;
  number t;
  if (a==NULL) return FALSE;
  omCheckAddrSize(a,sizeof(snumber));
#ifdef LDEBUG
  if (a->z==NULL) WerrorS("internal zero error(4)");
#endif
  if (a->n==NULL)
  {
    if (napIsConstant(a->z)) return nacIsOne(napGetCoeff(a->z));
    else                 return FALSE;
  }
  x = a->z;
  y = a->n;
  do
  {
    if (napComp(x, y))
      return FALSE;
    else
    {
      t = nacSub(napGetCoeff(x), napGetCoeff(y));
      if (!nacIsZero(t))
      {
        nacDelete(&t,currRing);
        return FALSE;
      }
      else
        nacDelete(&t,currRing);
    }
    napIter(x);
    napIter(y);
  }
  while ((x!=NULL) && (y!=NULL));
  if ((x!=NULL) || (y!=NULL)) return FALSE;
  napDelete(&a->z);
  napDelete(&a->n);
  a->z = napInit(1);
  a->n = NULL;
  a->s = 2;
  return TRUE;
}

/*2
* za == -1 ?
*/
BOOLEAN naIsMOne(number za)
{
  lnumber a = (lnumber)za;
  napoly x, y;
  number t;
  if (a==NULL) return FALSE;
  omCheckAddrSize(a,sizeof(snumber));
#ifdef LDEBUG
  if (a->z==NULL)
  {
    WerrorS("internal zero error(5)");
    return FALSE;
  }
#endif
  if (a->n==NULL)
  {
    if (napIsConstant(a->z)) return nacIsMOne(napGetCoeff(a->z));
    /*else                   return FALSE;*/
  }
  return FALSE;
}

/*2
* returns the i-th power of p (i>=0)
*/
void naPower(number p, int i, number *rc)
{
  number x;
  *rc = naInit(1);
  for (; i > 0; i--)
  {
    x = naMult(*rc, p);
    naDelete(rc,currRing);
    *rc = x;
  }
}

/*2
* result =gcd(a,b)
*/
number naGcd(number a, number b, const ring r)
{
  lnumber x, y;
  lnumber result = (lnumber)omAlloc0Bin(rnumber_bin);

  x = (lnumber)a;
  y = (lnumber)b;
  if (naNumbOfPar == 1)
  {
    if (naMinimalPoly!=NULL)
    {
      if (napNext(x->z)!=NULL)
        result->z = napCopy(x->z);
      else
        result->z = napGcd0(x->z, y->z);
    }
    else
      result->z = napGcd(x->z, y->z);
  }
  else
    result->z = napGcd(x->z, y->z); // change from napGcd0
  naTest((number)result);
  return (number)result;
}

/*2
* naNumbOfPar = 1:
* clears denominator         algebraic case;
* tries to simplify ratio    transcendental case;
*
* cancels monomials
* occuring in denominator
* and enumerator  ?          naNumbOfPar != 1;
*
* #defines for Factory:
* FACTORY_GCD_TEST: do not apply built in gcd for
*   univariate polynomials, always use Factory
*/
void naNormalize(number &pp)
{

  //naTest(pp); // input may not be "normal"
  lnumber p = (lnumber)pp;

  if ((p==NULL) /*|| (p->s==2)*/)
    return;
  p->s = 2;
  napoly x = p->z;
  napoly y = p->n;
  if ((y!=NULL) && (naMinimalPoly!=NULL))
  {
    y = napInvers(y, naMinimalPoly);
    x = napMult(x, y);
    if (napGetExp(x,1) >= napGetExp(naMinimalPoly,1))
      x = napRemainder(x, naMinimalPoly);
    p->z = x;
    p->n = y = NULL;
  }
  /* check for degree of x too high: */
  if ((x!=NULL) && (naMinimalPoly!=NULL) && (x!=naMinimalPoly)
  && (napGetExp(x,1)>napGetExp(naMinimalPoly,1)))
  // DO NOT REDUCE naMinimalPoly with itself
  {
    x = napRemainder(x, naMinimalPoly);
    p->z = x;
  }
  /* normalize all coefficients in n and z (if in Q) */
  if (naIsChar0)
  {
    while(x!=NULL)
    {
      nacNormalize(napGetCoeff(x));
      napIter(x);
    }
    x = p->z;
  }
  if (y==NULL) return;
  if (naIsChar0)
  {
    while(y!=NULL)
    {
      nacNormalize(napGetCoeff(y));
      napIter(y);
    }
    y = p->n;
  }
  // p->n !=NULL:
  /* collect all denoms from y and multiply x and y by it */
  if (naIsChar0)
  {
    number n=napLcm(y);
    napMultN(x,n);
    napMultN(y,n);
    nacDelete(&n,currRing);
    while(x!=NULL)
    {
      nacNormalize(napGetCoeff(x));
      napIter(x);
    }
    x = p->z;
    while(y!=NULL)
    {
      nacNormalize(napGetCoeff(y));
      napIter(y);
    }
    y = p->n;
  }
  if (naMinimalPoly == NULL)
  {
    int i;
    for (i=naNumbOfPar-1; i>=0; i--)
    {
      napoly xx=x;
      napoly yy=y;
      int m = napExpi(i, yy, xx);
      if (m != 0)          // in this case xx!=NULL!=yy
      {
        while (xx != NULL)
        {
          napAddExp(xx,i+1, -m);
          napIter(xx);
        }
        while (yy != NULL)
        {
          napAddExp(yy,i+1, -m);
          napIter(yy);
        }
      }
    }
  }
  if (napIsConstant(y)) /* i.e. => simplify to (1/c)*z / monom */
  {
    if (nacIsOne(napGetCoeff(y)))
    {
      napDelete1(&y);
      p->n = NULL;
      naTest(pp);
      return;
    }
    number h1 = nacInvers(napGetCoeff(y));
    nacNormalize(h1);
    napMultN(x, h1);
    nacDelete(&h1,currRing);
    napDelete1(&y);
    p->n = NULL;
    naTest(pp);
    return;
  }
#ifndef FACTORY_GCD_TEST
  if (naNumbOfPar == 1) /* apply built-in gcd */
  {
    napoly x1,y1;
    if (napGetExp(x,1) >= napGetExp(y,1))
    {
      x1 = napCopy(x);
      y1 = napCopy(y);
    }
    else
    {
      x1 = napCopy(y);
      y1 = napCopy(x);
    }
    napoly r;
    loop
    {
      r = napRemainder(x1, y1);
      if ((r==NULL) || (napNext(r)==NULL)) break;
      x1 = y1;
      y1 = r;
    }
    if (r!=NULL)
    {
      napDelete(&r);
      napDelete(&y1);
    }
    else
    {
      napDivMod(x, y1, &(p->z), &r);
      napDivMod(y, y1, &(p->n), &r);
      napDelete(&y1);
    }
    x = p->z;
    y = p->n;
    /* collect all denoms from y and multiply x and y by it */
    if (naIsChar0)
    {
      number n=napLcm(y);
      napMultN(x,n);
      napMultN(y,n);
      nacDelete(&n,currRing);
      while(x!=NULL)
      {
        nacNormalize(napGetCoeff(x));
        napIter(x);
      }
      x = p->z;
      while(y!=NULL)
      {
        nacNormalize(napGetCoeff(y));
        napIter(y);
      }
      y = p->n;
    }
    if (napNext(y)==NULL)
    {
      if (nacIsOne(napGetCoeff(y)))
      {
        if (napGetExp(y,1)==0)
        {
          napDelete1(&y);
          p->n = NULL;
        }
        naTest(pp);
        return;
      }
    }
  }
#endif /* FACTORY_GCD_TEST */
#ifdef HAVE_FACTORY
#ifndef FACTORY_GCD_TEST
  else
#endif
  {
    napoly xx,yy;
    singclap_algdividecontent(x,y,xx,yy);
    if (xx!=NULL)
    {
      p->z=xx;
      p->n=yy;
      napDelete(&x);
      napDelete(&y);
    }
  }
#endif
  /* remove common factors from z and n */
  x=p->z;
  y=p->n;
  if(!nacGreaterZero(napGetCoeff(y)))
  {
    x=napNeg(x);
    y=napNeg(y);
  }
  number g=nacCopy(napGetCoeff(x));
  napIter(x);
  while (x!=NULL)
  {
    number d=nacGcd(g,napGetCoeff(x), currRing);
    if(nacIsOne(d))
    {
      nacDelete(&g,currRing);
      nacDelete(&d,currRing);
      naTest(pp);
      return;
    }
    nacDelete(&g,currRing);
    g = d;
    napIter(x);
  }
  while (y!=NULL)
  {
    number d=nacGcd(g,napGetCoeff(y), currRing);
    if(nacIsOne(d))
    {
      nacDelete(&g,currRing);
      nacDelete(&d,currRing);
      naTest(pp);
      return;
    }
    nacDelete(&g,currRing);
    g = d;
    napIter(y);
  }
  x=p->z;
  y=p->n;
  while (x!=NULL)
  {
    number d = nacIntDiv(napGetCoeff(x),g);
    napSetCoeff(x,d);
    napIter(x);
  }
  while (y!=NULL)
  {
    number d = nacIntDiv(napGetCoeff(y),g);
    napSetCoeff(y,d);
    napIter(y);
  }
  nacDelete(&g,currRing);
  naTest(pp);
}

/*2
* returns in result->n 1
* and in     result->z the lcm(a->z,b->n)
*/
number naLcm(number la, number lb, const ring r)
{
  lnumber result;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  result = (lnumber)omAlloc0Bin(rnumber_bin);
  //if (((naMinimalPoly==NULL) && (naI==NULL)) || !naIsChar0)
  //{
  //  result->z = napInit(1);
  //  return (number)result;
  //}
  naNormalize(lb);
  naTest(la);
  naTest(lb);
  napoly x = napCopy(a->z);
  number t = napLcm(b->z); // get all denom of b->z
  if (!nacIsOne(t))
  {
    number bt, r;
    napoly xx=x;
    while (xx!=NULL)
    {
      bt = nacGcd(t, napGetCoeff(xx), currRing);
      r = nacMult(t, napGetCoeff(xx));
      nacDelete(&napGetCoeff(xx),currRing);
      napGetCoeff(xx) = nacDiv(r, bt);
      nacNormalize(napGetCoeff(xx));
      nacDelete(&bt,currRing);
      nacDelete(&r,currRing);
      napIter(xx);
    }
  }
  nacDelete(&t,currRing);
  result->z = x;
#ifdef HAVE_FACTORY
  if (b->n!=NULL)
  {
    result->z=singclap_alglcm(result->z,b->n);
    napDelete(&x);
  }
#endif
  naTest(la);
  naTest(lb);
  naTest((number)result);
  return ((number)result);
}

/*2
* input: a set of constant polynomials
* sets the global variable naI
*/
void naSetIdeal(ideal I)
{
  int i;

  if (idIs0(I))
  {
    for (i=naI->anz-1; i>=0; i--)
      napDelete(&naI->liste[i]);
    omFreeBin((ADDRESS)naI, snaIdeal_bin);
    naI=NULL;
  }
  else
  {
    lnumber h;
    number a;
    napoly x;

    naI=(naIdeal)omAllocBin(snaIdeal_bin);
    naI->anz=IDELEMS(I);
    naI->liste=(napoly*)omAlloc(naI->anz*sizeof(napoly));
    for (i=IDELEMS(I)-1; i>=0; i--)
    {
      h=(lnumber)pGetCoeff(I->m[i]);
      /* We only need the enumerator of h, as we expect it to be a polynomial */
      naI->liste[i]=napCopy(h->z);
      /* If it isn't normalized (lc = 1) do this */
      if (!nacIsOne(napGetCoeff(naI->liste[i])))
      {
        x=naI->liste[i];
        a=nacCopy(napGetCoeff(x));
        a=nacDiv(nacInit(1),a);
        napMultN(x,a);
        nacDelete(&a,currRing);
      }
    }
  }
}

/*2
* map Z/p -> Q(a)
*/
number naMapP0(number c)
{
  if (npIsZero(c)) return NULL;
  lnumber l=(lnumber)omAllocBin(rnumber_bin);
  l->s=2;
#ifndef LONGALGNEW
  l->z=(napoly)omAlloc0(napMonomSize);
#else /* LONGALGNEW */
  l->z=(napoly)p_Init(currRing->algring);
#endif /* LONGALGNEW */
  int i=(int)c;
  if (i>(naPrimeM>>2)) i-=naPrimeM;
  napGetCoeff(l->z)=nlInit(i);
  l->n=NULL;
  return (number)l;
}

/*2
* map Q -> Q(a)
*/
number naMap00(number c)
{
  if (nlIsZero(c)) return NULL;
  lnumber l=(lnumber)omAllocBin(rnumber_bin);
  l->s=0;
#ifndef LONGALGNEW
  l->z=(napoly)omAlloc0(napMonomSize);
#else /* LONGALGNEW */
  l->z=(napoly)p_Init(currRing->algring);
#endif /* LONGALGNEW */
  napGetCoeff(l->z)=nlCopy(c);
  l->n=NULL;
  return (number)l;
}

/*2
* map Z/p -> Z/p(a)
*/
number naMapPP(number c)
{
  if (npIsZero(c)) return NULL;
  lnumber l=(lnumber)omAllocBin(rnumber_bin);
  l->s=2;
#ifndef LONGALGNEW
  l->z=(napoly)omAlloc0(napMonomSize);
#else /* LONGALGNEW */
  l->z=(napoly)p_Init(currRing->algring);
#endif /* LONGALGNEW */
  napGetCoeff(l->z)=c; /* omit npCopy, because npCopy is a no-op */
  l->n=NULL;
  return (number)l;
}

/*2
* map Z/p' -> Z/p(a)
*/
number naMapPP1(number c)
{
  if (npIsZero(c)) return NULL;
  int i=(int)c;
  if (i>naPrimeM) i-=naPrimeM;
  number n=npInit(i);
  if (npIsZero(n)) return NULL;
  lnumber l=(lnumber)omAllocBin(rnumber_bin);
  l->s=2;
#ifndef LONGALGNEW
  l->z=(napoly)omAlloc0(napMonomSize);
#else /* LONGALGNEW */
  l->z=(napoly)p_Init(currRing->algring);
#endif /* LONGALGNEW */
  napGetCoeff(l->z)=n;
  l->n=NULL;
  return (number)l;
}

/*2
* map Q -> Z/p(a)
*/
number naMap0P(number c)
{
  if (nlIsZero(c)) return NULL;
  number n=npInit(nlInt(c));
  if (npIsZero(n)) return NULL;
  lnumber l=(lnumber)omAllocBin(rnumber_bin);
  l->s=2;
#ifndef LONGALGNEW
  l->z=(napoly)omAlloc0(napMonomSize);
#else /* LONGALGNEW */
  l->z=(napoly)p_Init(currRing->algring);
#endif /* LONGALGNEW */
  napGetCoeff(l->z)=n;
  l->n=NULL;
  return (number)l;
}

static number (*nacMap)(number);
static int naParsToCopy;
#ifdef LONGALGNEW
static ring napMapRing;
#endif /* LONGALGNEW */
static napoly napMap(napoly p)
{
  napoly w, a;

  if (p==NULL) return NULL;
#ifndef LONGALGNEW
  a = w = (napoly)omAlloc0(napMonomSize);
  memcpy(a->e, p->e, naParsToCopy * SIZEOF_PARAMETER);
  w->ko = nacMap(p->ko);
#else /* LONGALGNEW */
  a = w = (napoly)p_Init(currRing->algring);
  int i;
  for(i=1;i<=naParsToCopy;i++)
    napSetExp(a,i,napGetExpFrom(p,i,napMapRing));
  napGetCoeff(w) = nacMap(napGetCoeff(p));
#endif /* LONGALGNEW */
  loop
  {
    napIter(p);
    if (p==NULL) break;
#ifndef LONGALGNEW
    a->ne = (napoly)omAlloc0(napMonomSize);
    a = a->ne;
    memcpy(a->e, p->e, naParsToCopy * SIZEOF_PARAMETER);
#else /* LONGALGNEW */
    napNext(a) = (napoly)p_Init(currRing->algring);
    napIter(a);
    for(i=1;i<=naParsToCopy;i++)
      napSetExp(a,i,napGetExpFrom(p,i,napMapRing));
#endif /* LONGALGNEW */
    napGetCoeff(a) = nacMap(napGetCoeff(p));
  }
  napNext(a) = NULL;
  return w;
}

/*2
* map _(a) -> _(b)
*/
number naMapQaQb(number c)
{
  if (c==NULL) return NULL;
  lnumber erg= (lnumber)omAlloc0Bin(rnumber_bin);
  lnumber src =(lnumber)c;
  erg->s=src->s;
  erg->z=napMap(src->z);
  erg->n=napMap(src->n);
  if (naMinimalPoly!=NULL)
  {
    if (napGetExp(erg->z,1) >= napGetExp(naMinimalPoly,1))
    {
      erg->z = napRemainder(erg->z, naMinimalPoly);
      if (erg->z==NULL)
      {
        number t_erg=(number)erg;
        naDelete(&t_erg,currRing);
        return (number)NULL;
      }
    }
    if (erg->n!=NULL)
    {
      if (napGetExp(erg->n,1) >= napGetExp(naMinimalPoly,1))
        erg->n = napRemainder(erg->n, naMinimalPoly);
#ifndef LONGALGNEW
      if ((napDeg(erg->n)==0) && nacIsOne(erg->n->ko))
#else /* LONGALGNEW */
      if ((napIsConstant(erg->n)) && nacIsOne(napGetCoeff(erg->n)))
#endif /* LONGALGNEW */
        napDelete(&(erg->n));
    }
  }
  return (number)erg;
}

nMapFunc naSetMap(ring src, ring dst)
{
  if (rField_is_Q_a(dst)) /* -> Q(a) */
  {
    if (rField_is_Q(src))
    {
      return naMap00;   /*Q -> Q(a)*/
    }
    if (rField_is_Zp(src))
    {
      naPrimeM = rChar(src);
      return naMapP0;  /* Z/p -> Q(a)*/
    }
    if (rField_is_Q_a(src))
    {
      int i;
      naParsToCopy=0;
      for(i=0;i<rPar(src);i++)
      {
        if ((i>=rPar(dst))
        ||(strcmp(src->parameter[i],dst->parameter[i])!=0))
           return NULL;
        naParsToCopy++;
      }
#ifdef LONGALGNEW
      napMapRing=src;
#endif /* LONGALGNEW */
      nacMap=nacCopy;
      return naMapQaQb;   /* Q(a) -> Q(a) */
    }
  }
  /*-----------------------------------------------------*/
  if (rField_is_Zp_a(dst)) /* -> Z/p(a) */
  {
    if (rField_is_Q(src))
    {
      return naMap0P;   /*Q -> Z/p(a)*/
    }
    if (rField_is_Zp(src))
    {
      int c=rChar(src);
      if (c==npPrimeM)
      {
        return naMapPP;  /* Z/p -> Z/p(a)*/
      }
      else
      {
        naPrimeM = c;
        return naMapPP1;  /* Z/p' -> Z/p(a)*/
      }
    }
    if (rField_is_Zp_a(src))
    {
      if (rChar(src)==rChar(dst))
      {
        nacMap=nacCopy;
      }
      else
      {
        npMapPrime=rChar(src);
        nacMap = npMapP;
      }
      int i;
      naParsToCopy=0;
      for(i=0;i<rPar(src);i++)
      {
        if ((i>=rPar(dst))
        ||(strcmp(src->parameter[i],dst->parameter[i])!=0))
           return NULL;
        naParsToCopy++;
      }
#ifdef LONGALGNEW
      napMapRing=src;
#endif /* LONGALGNEW */
      return naMapQaQb;   /* Z/p(a),Z/p'(a) -> Z/p(b)*/
    }
  }
  return NULL;      /* default */
}

/*2
* convert a napoly number into a poly
*/
poly naPermNumber(number z, int * par_perm, int P, ring r)
{
  if (z==NULL) return NULL;
  poly res=NULL;
  poly p;
  napoly za=((lnumber)z)->z;
  nMapFunc nMap=NULL;
  if (currRing->parameter!=NULL)
    nMap=currRing->algring->cf->cfSetMap(r->algring, currRing->algring);
  else
    nMap=currRing->cf->cfSetMap(r->algring, currRing);
  if (nMap==NULL) return NULL; /* emergency exit only */
  do
  {
    p = pInit();
    pNext(p)=NULL;
    nNew(&pGetCoeff(p));
    int i;
    for(i=pVariables;i;i--)
       pSetExp(p,i, 0);
    pSetComp(p, 0);
    napoly pa=NULL;
    if (currRing->parameter!=NULL)
    {
      assume(r->algring!=NULL);
      pGetCoeff(p)=(number)omAlloc0Bin(rnumber_bin);
      ((lnumber)pGetCoeff(p))->s=2;
      ((lnumber)pGetCoeff(p))->z=napInitz(nMap(napGetCoeff(za)));
      pa=((lnumber)pGetCoeff(p))->z;
    }
    else
    {
      pGetCoeff(p)=nMap(napGetCoeff(za));
    }
    for(i=0;i<P;i++)
    {
      if(napGetExpFrom(za,i+1,r)!=0)
      {
        if(par_perm==NULL)
        {
          if ((rPar(currRing)>=i) && (pa!=NULL))
            napSetExp(pa,i+1,napGetExpFrom(za,i+1,r));
          else
          {
            pDelete(&p);
            break;
          }
        }
        else if(par_perm[i]>0)
          pSetExp(p,par_perm[i],napGetExpFrom(za,i+1,r));
        else if((par_perm[i]<0)&&(pa!=NULL))
          napSetExp(pa,-par_perm[i], napGetExpFrom(za,i+1,r));
        else
        {
          pDelete(&p);
          break;
        }
      }
    }
    if (p!=NULL)
    {
      pSetm(p);
      pTest(p);
      res=pAdd(res,p);
    }
    napIter(za);
  }
  while (za!=NULL);
  pTest(res);
  return res;
}

number   naGetDenom(number &n)
{
  naNormalize(n);
  lnumber x=(lnumber)n;
  if (x->n!=NULL)
  {
    lnumber r=(lnumber)omAlloc0Bin(rnumber_bin);
    r->z=napCopy(naGetDenom0(x));
    r->s = 2;
    return (number)r;
  }
  return naInit(1);
}

#ifdef LDEBUG
BOOLEAN naDBTest(number a, char *f,int l)
{
  lnumber x=(lnumber)a;
  if (x == NULL)
    return TRUE;
  omCheckAddrSize(a, sizeof(snumber));
  napoly p = x->z;
  if (p==NULL)
  {
    Print("0/* in %s:%d\n",f,l);
    return FALSE;
  }
  while(p!=NULL)
  {
    if ((naIsChar0 && nlIsZero(napGetCoeff(p)))
    || ((!naIsChar0) && npIsZero(napGetCoeff(p))))
    {
      Print("coeff 0 in %s:%d\n",f,l);
      return FALSE;
    }
    if((naMinimalPoly!=NULL)&&(napGetExp(p,1)>napGetExp(naMinimalPoly,1))
    &&(p!=naMinimalPoly))
    {
      Print("deg>minpoly in %s:%d\n",f,l);
      return FALSE;
    }
    //if (naIsChar0 && (((int)p->ko &3) == 0) && (p->ko->s==0) && (x->s==2))
    //{
    //  Print("normalized with non-normal coeffs in %s:%d\n",f,l);
    //  return FALSE;
    //}
    if (naIsChar0 && !(nlDBTest(napGetCoeff(p),f,l)))
      return FALSE;
    napIter(p);
  }
  p = naGetDenom0(x);
  while(p!=NULL)
  {
    if (naIsChar0 && !(nlDBTest(napGetCoeff(p),f,l)))
      return FALSE;
    napIter(p);
  }
  return TRUE;
}
#endif

