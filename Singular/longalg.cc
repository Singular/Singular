/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longalg.cc,v 1.30 1999-03-08 18:11:47 Singular Exp $ */
/*
* ABSTRACT:   algebraic numbers
*/

#include <stdio.h>
#include <string.h>
#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
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

#define  FEHLER1 1
#define  FEHLER2 1
#define  FEHLER3 1

struct snaIdeal
{
  int anz;
  alg *liste;
};
typedef struct snaIdeal * naIdeal;

naIdeal naI=NULL;

//#define RECA_SIZE (sizeof(alg)+sizeof(number))
alg naMinimalPoly;
int naNumbOfPar;
int napMonomSize;
char **naParNames;
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
                nacMult, nacSub, nacAdd, nacDiv, nacIntDiv, nacGcd, nacLcm;
#ifdef LDEBUG
static void     (*nacDBDelete)(number *a,char *f,int l);
#define         nacDelete(A) nacDBDelete(A,__FILE__,__LINE__)
#else
static void     (*nacDelete)(number *a);
#endif
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
extern void     napDelete(alg *p);
static alg napRedp(alg q);
static alg napTailred(alg q);
static BOOLEAN napDivPoly(alg p, alg q);
static int napExpi(int i, alg a, alg b);

static number nadGcd( number a, number b) { return nacInit(1); }
/*2
*  sets the appropriate operators
*/
void naSetChar(int i, BOOLEAN complete, char ** param, int pars)
{
  if (naI!=NULL)
  {
    int j;
    for (j=naI->anz-1; j>=0; j--)
       napDelete (&naI->liste[j]);
    Free((ADDRESS)naI->liste,naI->anz*sizeof(alg));
    Free((ADDRESS)naI,sizeof(*naI));
    naI=NULL;
  }
  naMap = naCopy;
  naMinimalPoly = NULL;
  naParNames=param;
  naNumbOfPar=pars;
  napMonomSize = RECA_SIZE + naNumbOfPar*SIZEOF_PARAMETER;
  if (i == 1)
  {
    naIsChar0 = 1;
#ifdef LDEBUG
    nacDBDelete      = nlDBDelete;
#else
    nacDelete      = nlDelete;
#endif
    if (complete)
    {
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
  }
  else if (i < 0)
  {
    naIsChar0 = 0;
#ifdef LDEBUG
    nacDBDelete    = nDBDummy1;
#else
    nacDelete      = nDummy1;
#endif
    if (complete)
    {
      npSetChar(-i);
      nacInit        = npInit;
      nacInt         = npInt;
      nacCopy        = npCopy;
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
  }
#ifdef TEST
  else
  {
    Print("naSetChar:c=%d compl=%d param=%d\n",i,complete,param);
  }
#endif
}

/*============= procedure for polynomials: napXXXX =======================*/

#define napSetCoeff(p,n) {nacDelete(&((p)->ko));(p)->ko=n;}
#define napIter(A) A=(A)->ne


/*3
* creates  an alg poly
*/
static alg napInit(int i)
{
  alg a = (alg)Alloc0(napMonomSize);

  a->ko = nacInit(i);
  if (!nacIsZero(a->ko))
  {
    return a;
  }
  else
  {
    Free((ADDRESS)a, napMonomSize);
    return NULL;
  }
}

/*3
* creates  an alg poly
*/
alg napInitz(number z)
{
  alg a = (alg)Alloc0(napMonomSize);

  a->ko = z;
  return a;
}

/*3
*  deletes head of  an alg poly
*/
static void napDelete1(alg *p)
{
  alg h = *p;

  if (h!=NULL)
  {
    *p = h->ne;
    nacDelete(&(h->ko));
    Free((ADDRESS)h, napMonomSize);
  }
}

/*3
*  delete an alg poly
*/
void napDelete(alg *p)
{
  alg w, h = *p;

  while (h!=NULL)
  {
    w = h;
    h = h->ne;
    nacDelete(&(w->ko));
    Free((ADDRESS)w, napMonomSize);
  }
  *p = NULL;
}


/*3
* copy a alg. poly
*/
static alg napCopy(alg p)
{
  if (p==NULL) return NULL;

  alg w, a;

  mmTestP(p,napMonomSize);
  a = w = (alg)Alloc(napMonomSize);
  memcpy(w->e, p->e, naNumbOfPar * SIZEOF_PARAMETER);
  w->ko = nacCopy(p->ko);
  loop
  {
    p=p->ne;
    if (p==NULL) break;
    mmTestP(p,napMonomSize);
    a->ne = (alg)Alloc(napMonomSize);
    a = a->ne;
    memcpy(a->e, p->e, naNumbOfPar * SIZEOF_PARAMETER);
    a->ko = nacCopy(p->ko);
  }
  a->ne = NULL;
  return w;
}

/*3
* copy a alg. poly
*/
static alg napCopyNeg(alg p)
{
  alg w, a;

  if (p==NULL) return NULL;
  mmTestP(p,napMonomSize);
  a = w = (alg)Alloc(napMonomSize);
  memcpy(w->e, p->e, naNumbOfPar * SIZEOF_PARAMETER);
  w->ko = nacNeg(nacCopy(p->ko));
  loop
  {
    p=p->ne;
    if (p==NULL) break;
    mmTestP(p,napMonomSize);
    a->ne = (alg)Alloc(napMonomSize);
    a = a->ne;
    memcpy(a->e, p->e, naNumbOfPar * SIZEOF_PARAMETER);
    a->ko = nacNeg(nacCopy(p->ko));
  }
  a->ne = NULL;
  return w;
}

/*3
* Compare exponents of p and q
*/
static int  napComp(alg p, alg q)
{
  int  i = 0;

  mmTestP(p,napMonomSize);
  mmTestP(q,napMonomSize);
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
*  addition of alg. polys
*/
alg napAdd(alg p1, alg p2)
{
  alg a1, p, a2, a;
  int  c;  number t;

  if (p1==NULL) return p2;
  if (p2==NULL) return p1;
  mmTestP(p1,napMonomSize);
  mmTestP(p2,napMonomSize);
  a1 = p1;
  a2 = p2;
  a = p  = (alg)Alloc(napMonomSize);
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
      mmTestP(a1,napMonomSize);
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
      mmTestP(a2,napMonomSize);
    }
    else
    {
      t = nacAdd(a1->ko, a2->ko);
      napDelete1(&a2);
      if (nacIsZero(t))
      {
        nacDelete(&t);
        napDelete1(&a1);
      }
      else
      {
        nacDelete(&(a1->ko));
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
      mmTestP(a1,napMonomSize);
      mmTestP(a2,napMonomSize);
    }
  }
  a = p->ne;
  Free((ADDRESS)p, napMonomSize);
  return a;
}


/*3
* multiply a alg. poly by -1
*/
static alg napNeg(alg a)
{
  alg p = a;

  while (p!=NULL)
  {
    mmTestP(p,napMonomSize);
    p->ko = nacNeg(p->ko);
    p = p->ne;
  }
  return a;
}

/*3
* returns ph * z
*/
static void napMultN(alg p, number z)
{
  number t;

  while (p!=NULL)
  {
    mmTestP(p,napMonomSize);
    t = nacMult(p->ko, z);
    nacNormalize(t);
    nacDelete(&p->ko);
    p->ko = t;
    p = p->ne;
  }
}

/*3
* update the polynomial a by multipying it by
* the (number) coefficient
* and the exponent vector (of) exp (a well initialized polynomial)
*/
static void napMultT(alg a, alg exp)
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
        nacDelete(&(a->ko));
        a->ko = t;
        for (i = naNumbOfPar - 1; i >= 0; i--)
          a->e[i] += exp->e[i];
        a = a->ne;
      }
      while (a!=NULL);
    }
  }
}

/*3
* multiplication of alg. polys
* multiply p1 with p2, p1 and p2 are destroyed
*/
static alg napMult(alg p1, alg p2)
{
  alg a1, a2, b, h;
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

/*3
*  division with rest; f = g*q + r, returns r and destroy f
*/
static alg napRemainder(alg f, const alg  g)
{
  alg a, h, qq;

  qq = (alg)Alloc(napMonomSize);
  qq->ne = NULL;
  a = f;
  do
  {
    mmTestP(a,napMonomSize);
    mmTestP(g,napMonomSize);
    qq->e[0] = a->e[0] - g->e[0];
    qq->ko = nacDiv(a->ko, g->ko);
    qq->ko = nacNeg(qq->ko);
    h = napCopy(g);
    napMultT(h, qq);
    nacDelete(&(qq->ko));
    a = napAdd(a, h);
  }
  while ((a!=NULL) && (a->e[0] >= g->e[0]));
  Free((ADDRESS)qq, napMonomSize);
  return a;
}

/*3
*  division with rest; f = g*q + r,  destroy f
*/
static void napDivMod(alg f, alg  g, alg *q, alg *r)
{
  alg a, h, b, qq;

  qq = (alg)Alloc(napMonomSize);
  qq->ne = b = NULL;
  a = f;
  do
  {
    mmTestP(a,napMonomSize);
    mmTestP(g,napMonomSize);
    qq->e[0] = a->e[0] - g->e[0];
    qq->ko = nacDiv(a->ko, g->ko);
    b = napAdd(b, napCopy(qq));
    qq->ko = nacNeg(qq->ko);
    h = napCopy(g);
    napMultT(h, qq);
    nacDelete(&(qq->ko));
    a = napAdd(a, h);
  }
  while ((a!=NULL) && (a->e[0] >= g->e[0]));
  Free((ADDRESS)qq, napMonomSize);
  *q = b;
  *r = a;
}

/*3
*  returns z with z*x mod c = 1
*/
static alg napInvers(alg x, const alg c)
{
  alg y, r, qa, qn, q;
  number t, h;

  if (x->e[0] >= c->e[0])
    x = napRemainder(x, c);
  if (x->e[0]==0)
  {
    if (!nacIsOne(x->ko))
    {
      h = nacInit(1);
      t = nacDiv(h, x->ko);
      nacDelete(&(x->ko));
      nacDelete(&h);
      x->ko = t;
    }
    return x;
  }
  y = napCopy(c);
  napDivMod(y, x, &qa, &r);
  if (r->e[0]==0)
  {
    h = nacInit(-1);
    t = nacDiv(h, r->ko);
    nacNormalize(t);
    napMultN(qa, t);
    nacDelete(&h);
    nacDelete(&t);
    napDelete(&x);
    napDelete(&r);
    return qa;
  }
  y = x;
  x = r;
  napDivMod(y, x, &q, &r);
  if (r->e[0]==0)
  {
    q = napMult(q, qa);
    q = napAdd(q, napInit(1));
    h = nacInit(1);
    t = nacDiv(h, r->ko);
    napMultN(q, t);
    nacDelete(&h);
    nacDelete(&t);
    napDelete(&x);
    napDelete(&r);
    if (q->e[0] >= c->e[0])
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
    if (r->e[0]==0)
    {
      q = napMult(q, qn);
      q = napNeg(q);
      q = napAdd(q, qa);
      h = nacInit(1);
      t = nacDiv(h, r->ko);
      napMultN(q, t);
      nacDelete(&h);
      nacDelete(&t);
      napDelete(&x);
      napDelete(&r);
      if (q->e[0] >= c->e[0])
        q = napRemainder(q, c);
      return q;
    }
    y = q;
    q = napMult(napCopy(q), qn);
    q = napNeg(q);
    q = napAdd(q, qa);
    qa = y;
  }
}

/*3
* the degree of an alg poly (used for test of "constant" et al.)
*/
static int  napDeg(alg p)
{
  int  d = 0, i;

  mmTestP(p,napMonomSize);
  for (i = naNumbOfPar-1; i>=0; i--)
    d += p->e[i];
  return d;
}

/*3
* the max degree of an alg poly (used for test of "simple" et al.)
*/
static int  napMaxDeg(alg p)
{
  int  d = 0;
  mmTestP(p,napMonomSize);
  while(p!=NULL)
  {
    d=max(d,napDeg(p));
    p=p->ne;
  }
  return d;
}

/*3
* the max degree of an alg poly (used for test of "simple" et al.)
*/
static int  napMaxDegLen(alg p, int &l)
{
  int  d = 0;
  int ll=0;
  mmTestP(p,napMonomSize);
  while(p!=NULL)
  {
    d=max(d,napDeg(p));
    p=p->ne;
    ll++;
  }
  l=ll;
  return d;
}


/*3
*writes a polynomial number
*/
void napWrite(alg p)
{
  if (p==NULL)
    StringAppendS("0");
  else if (napDeg(p)==0)
  {
    //StringAppendS("-1");
    nacWrite(p->ko);
  }
  else
  {
    StringAppendS("(");
    loop
    {
      BOOLEAN wroteCoeff=FALSE;
      mmTestP(p,napMonomSize);
      if ((napDeg(p)==0)
      || ((!nacIsOne(p->ko))
        && (!nacIsMOne(p->ko))))
      {
        nacWrite(p->ko);
        wroteCoeff=(pShortOut==0);
      }
      else if (nacIsMOne(p->ko))
      {
        StringAppend("-");
      }
      int  i;
      for (i = 0; i <= naNumbOfPar - 1; i++)
      {
        if (p->e[i] > 0)
        {
          if (wroteCoeff)
            StringAppendS("*");
          else
            wroteCoeff=(pShortOut==0);
          StringAppend(naParNames[i]);
          if (p->e[i] > 1)
          {
            if (pShortOut == 0)
              StringAppendS("^");
            StringAppend("%d", p->e[i]);
          }
        }
      }
      p = p->ne;
      if (p==NULL)
        break;
      if (nacGreaterZero(p->ko))
        StringAppendS("+");
    }
    StringAppendS(")");
  }
}


static char *napHandleMons(char *s, int i, PARAMETER_TYPE *ex)
{
  int  j;
  if (strncmp(s,naParNames[i],strlen(naParNames[i]))==0)
  {
    s+=strlen(naParNames[i]);
    if ((*s >= '0') && (*s <= '9'))
    {
      s = eati(s, &j);
      ex[i] += j;
    }
    else
      ex[i]++;
  }
  return s;
}

/*3  reads a monomial  */
static char  *napRead(char *s, alg *b)
{
  alg a;
  int  i;
  a = (alg)Alloc0(napMonomSize);
  if ((*s >= '0') && (*s <= '9'))
  {
    s = nacRead(s, &(a->ko));
    if (nacIsZero(a->ko))
    {
      napDelete1(&a);
      *b = NULL;
      return s;
    }
  }
  else
    a->ko = nacInit(1);
  i = 0;
  char  *olds;
  loop
  {
    olds = s;
    s = napHandleMons(s, i, a->e);
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

static int napExp(alg a, alg b)
{
  while (a->ne!=NULL) a = a->ne;
  int m = a->e[0];
  if (m==0) return 0;
  while (b->ne!=NULL) b = b->ne;
  if (m > b->e[0]) m = b->e[0];
  return m;
}

#if FEHLER2
/*meins
* finds the smallest i-th exponent in a and b
* used to find it in a fraction
*/
static int napExpi(int i, alg a, alg b)
{
  if (a==NULL || b==NULL) return 0;
  int m = a->e[i];
  if (m==0) return 0;
  while (a->ne != NULL)
  {
    a = a->ne;
    if (m > a->e[i])
    {
      m = a->e[i];
      if (m==0) return 0;
    }
  }
  do
  {
    if (m > b->e[i])
    {
      m = b->e[i];
      if (m==0) return 0;
    }
    b = b->ne;
  }
  while (b != NULL);
  return m;
}
#endif

static void napContent(alg ph)
{
  number h,d;
  alg p;

  p = ph;
  if (nacIsOne(p->ko))
    return;
  h = nacCopy(p->ko);
  p = p->ne;
  do
  {
    d=nacGcd(p->ko, h);
    if(nacIsOne(d))
    {
      nacDelete(&h);
      nacDelete(&d);
      return;
    }
    nacDelete(&h);
    h = d;
    p = p->ne;
  }
  while (p!=NULL);
  h = nacInvers(d);
  nacDelete(&d);
  p = ph;
  while (p!=NULL)
  {
    d = nacMult(p->ko, h);
    nacDelete(&(p->ko));
    p->ko = d;
    p = p->ne;
  }
  nacDelete(&h);
}

static void napCleardenom(alg ph)
{
  number d, h;
  alg p;

  if (!naIsChar0)
    return;
  p = ph;
  h = nacInit(1);
  while (p!=NULL)
  {
    d = nacLcm(h, p->ko);
    nacDelete(&h);
    h = d;
    p = p->ne;
  }
  if(!nacIsOne(h))
  {
    p = ph;
    while (p!=NULL)
    {
      d=nacMult(h, p->ko);
      nacDelete(&(p->ko));
      p->ko = d;
      p = p->ne;
    }
    nacDelete(&h);
  }
  napContent(ph);
}

static alg napGcd0(alg a, alg b)
{
  number x, y;
  if (!naIsChar0)
    return napInit(1);
  x = nacCopy(a->ko);
  if (nacIsOne(x))
    return napInitz(x);
  while (a->ne!=NULL)
  {
    a = a->ne;
    y = nacGcd(x, a->ko);
    nacDelete(&x);
    x = y;
    if (nacIsOne(x))
      return napInitz(x);
  }
  do
  {
    y = nacGcd(x, b->ko);
    nacDelete(&x);
    x = y;
    if (nacIsOne(x))
      return napInitz(x);
    b = b->ne;
  }
  while (b!=NULL);
  return napInitz(x);
}

/*3
* result =gcd(a,b)
*/
static alg napGcd(alg a, alg b)
{
  int i;
  alg g, x, y, h;
  if ((a==NULL)
  || ((a->ne==NULL)&&(nacIsZero(a->ko))))
  {
    if ((b==NULL)
    || ((b->ne==NULL)&&(nacIsZero(b->ko))))
    {
      return napInit(1);
    }
    return napCopy(b);
  }
  else
  if ((b==NULL)
  || ((b->ne==NULL)&&(nacIsZero(b->ko))))
  {
    return napCopy(a);
  }
  if (naMinimalPoly != NULL)
  {
    if (a->e[0] >= b->e[0])
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
    if (y->ne==NULL)
    {
      g->e[0] = napExp(x, y);
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
        if (!nacIsOne(g->ko)) napMultN(y, g->ko);
        napDelete1(&g);
        return y;
      }
      else if (h->ne==NULL)
        break;
      x = y;
      y = h;
    }
    napDelete(&y);
    napDelete1(&h);
    g->e[0] = napExp(a, b);
    return g;
  }
  x = (alg)Alloc0(napMonomSize);
  g=a;
  h=b;
  if (!naIsChar0) x = napInit(1);
  else            x = napGcd0(g,h);
  for (i=(naNumbOfPar-1); i>=0; i--)
  {
    x->e[i] = napExpi(i,a,b);
  }
  return x;
}


number napLcm(alg a)
{
  number h = nacInit(1);

  if (naIsChar0)
  {
    number d;
    alg b = a;

    while (b!=NULL)
    {
      d = nacLcm(h, b->ko);
      nacDelete(&h);
      h = d;
      b = b->ne;
    }
  }
  return h;
}


/*2
* meins  (for reduction in algebraic extension)
* checks if head of p divides head of q
* doesn't delete p and q
*/
BOOLEAN napDivPoly (alg p, alg q)
{
  int j=0;   /* evtl. von naNumber.. -1 abwaerts zaehlen */

  while (p->e[j] <= q->e[j])
  {
    j++;
    if (j >= naNumbOfPar)
      return 1;
  }
  return 0;
}


/*2
* meins  (for reduction in algebraic extension)
* Normalform of poly with naI
* changes q and returns it
*/
alg napRedp (alg q)
{
  alg h = (alg)Alloc0(napMonomSize);
  int i=0,j;

  loop
  {
    if (napDivPoly (naI->liste[i], q))
    {
      mmTestP((ADDRESS)q,napMonomSize);
      //StringSetS("");
      //napWrite(q);
      //napWrite(naI->liste[i]);
      //Print(StringAppendS("\n"));
      /* h = lt(q)/lt(naI->liste[i])*/
      h->ko = nacCopy(q->ko);
      for (j=naNumbOfPar-1; j>=0; j--)
        h->e[j] = q->e[j] - naI->liste[i]->e[j];
      h = napMult (h, napCopy(naI->liste[i]));
      h = napNeg (h);
      q = napAdd (q, napCopy(h));
      napDelete (&(h->ne));
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
alg napTailred (alg q)
{
  alg h;

  h = q->ne;
  while (h != NULL)
  {
     h = napRedp (h);
     if (h == NULL)
        return q;
     h = h->ne;
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
    lnumber l = (lnumber)Alloc(sizeof(rnumber));
    l->z = napInit(i);
    if (l->z==NULL)
    {
      Free((ADDRESS)l, sizeof(rnumber));
      return NULL;
    }
    l->s = 2;
    l->n = NULL;
    return (number)l;
  }
  /*else*/
  return NULL;
}

number  naPar(int i)
{
  lnumber l = (lnumber)Alloc(sizeof(rnumber));
  l->s = 2;
  l->z = napInit(1);
  l->z->e[i-1]=1;
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
  if ((l!=NULL)&&(l->n==NULL)&&(napDeg(l->z)==0))
  {
    return nacInt(l->z->ko);
  }
  return 0;
}

/*2
*  deletes p
*/
#ifdef LDEBUG
void naDBDelete(number *p,char *f, int lno)
#else
void naDelete(number *p)
#endif
{
  lnumber l = (lnumber) * p;
  if (l==NULL) return;
  napDelete(&(l->z));
  napDelete(&(l->n));
#if defined(MDEBUG) && defined(LDEBUG)
  mmDBFreeBlock((ADDRESS)l, sizeof(rnumber),f,lno);
#else
  Free((ADDRESS)l, sizeof(rnumber));
#endif
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
  erg = (lnumber)Alloc0(sizeof(rnumber));
  erg->z = napCopy(src->z);
  erg->n = napCopy(src->n);
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
  alg x, y;
  lnumber lu;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  if (a==NULL) return naCopy(lb);
  if (b==NULL) return naCopy(la);
  mmTestP(a,sizeof(rnumber));
  mmTestP(b,sizeof(rnumber));
  lu = (lnumber)Alloc(sizeof(rnumber));
  if (b->n!=NULL) x = napMult(napCopy(a->z), napCopy(b->n));
  else            x = napCopy(a->z);
  if (a->n!=NULL) y = napMult(napCopy(b->z), napCopy(a->n));
  else            y = napCopy(b->z);
  lu->z = napAdd(x, y);
  if (lu->z==NULL)
  {
    Free((ADDRESS)lu, sizeof(rnumber));
    return (number)NULL;
  }
  if (a->n!=NULL)
  {
    if (b->n!=NULL) x = napMult(napCopy(a->n), napCopy(b->n));
    else            x = napCopy(a->n);
  }
  else
  {
    if (b->n!=NULL) x = napCopy(b->n);
    else            x = NULL;
  }
  if ((x!=NULL) && (napDeg(x)==0) && nacIsOne(x->ko))
    napDelete(&x);
  lu->n = x;
  lu->s = 0;
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
    //if (lb!=NULL)
    //{
      lu = (lnumber)naCopy(lb);
      lu->z = napNeg(lu->z);
      return (number)lu;
    //}
    //else
    //  return NULL;
  }

  alg x, y;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;

  mmTestP(a,sizeof(rnumber));
  mmTestP(b,sizeof(rnumber));
  lu = (lnumber)Alloc(sizeof(rnumber));
  if (b->n!=NULL) x = napMult(napCopy(a->z), napCopy(b->n));
  else            x = napCopy(a->z);
  if (a->n!=NULL) y = napMult(napCopy(b->z), napCopyNeg(a->n));
  else            y = napCopyNeg(b->z);
  lu->z = napAdd(x, y);
  if (lu->z==NULL)
  {
    Free((ADDRESS)lu, sizeof(rnumber));
    return (number)NULL;
  }
  if (a->n!=NULL)
  {
    if (b->n!=NULL) x = napMult(napCopy(a->n), napCopy(b->n));
    else            x = napCopy(a->n);
  }
  else
  {
    if (b->n!=NULL) x = napCopy(b->n);
    else            x = NULL;
  }
  if ((x!=NULL)&& (napDeg(x)==0) && nacIsOne(x->ko))
    napDelete(&x);
  lu->n = x;
  lu->s = 0;
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
  alg x;

  mmTestP(a,sizeof(rnumber));
  mmTestP(b,sizeof(rnumber));
  naTest(la);
  naTest(lb);

  lo = (lnumber)Alloc(sizeof(rnumber));
  lo->z = napMult(napCopy(a->z), napCopy(b->z));

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
      x = napMult(napCopy(b->n), napCopy(a->n));
    }
  }
  if (naMinimalPoly!=NULL)
  {
    if (lo->z->e[0] >= naMinimalPoly->e[0])
      lo->z = napRemainder(lo->z, naMinimalPoly);
    if ((x!=NULL) && (x->e[0] >= naMinimalPoly->e[0]))
      x = napRemainder(x, naMinimalPoly);
  }
#if FEHLER1
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
#endif
  if ((x!=NULL) && (napDeg(x)==0) && nacIsOne(x->ko))
    napDelete(&x);
  lo->n = x;
  lo->s = 0;
  if(lo->z==NULL)
  {
    Free((ADDRESS)lo,sizeof(rnumber));
    lo=NULL;
  }
  naTest((number)lo);
  return (number)lo;
}

number naIntDiv(number la, number lb)
{
  lnumber res;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  if ((a==NULL) || (a->z==NULL))
    return NULL;
  if ((b==NULL) || (b->z==NULL))
  {
    WerrorS("div. by 0");
    return NULL;
  }
  res = (lnumber)Alloc(sizeof(rnumber));
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
  alg x;

  if ((a==NULL) || (a->z==NULL))
    return NULL;

  if ((b==NULL) || (b->z==NULL))
  {
    WerrorS("div. by 0");
    return NULL;
  }
  mmTestP(a,sizeof(rnumber));
  mmTestP(b,sizeof(rnumber));
  lo = (lnumber)Alloc(sizeof(rnumber));
  if (b->n!=NULL)
    lo->z = napMult(napCopy(a->z), napCopy(b->n));
  else
    lo->z = napCopy(a->z);
  if (a->n!=NULL)
    x = napMult(napCopy(b->z), napCopy(a->n));
  else
    x = napCopy(b->z);
  if (naMinimalPoly!=NULL)
  {
    if (lo->z->e[0] >= naMinimalPoly->e[0])
      lo->z = napRemainder(lo->z, naMinimalPoly);
    if (x->e[0] >= naMinimalPoly->e[0])
      x = napRemainder(x, naMinimalPoly);
  }
#if FEHLER1
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
#endif
  if ((napDeg(x)==0) && nacIsOne(x->ko))
    napDelete(&x);
  lo->s = 0;
  lo->n = x;
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
  alg x;

  if (b==NULL)
  {
    WerrorS("div. by 0");
    return NULL;
  }
  mmTestP(b,sizeof(rnumber));
  lo = (lnumber)Alloc0(sizeof(rnumber));
  lo->s = b->s;
  if (b->n!=NULL)
    lo->z = napCopy(b->n);
  else
    lo->z = napInit(1);
  x = b->z;
  if ((napDeg(x)!=0) || !nacIsOne(x->ko))
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
    if (x->e[0] >= naMinimalPoly->e[0])
      x = napRemainder(x, naMinimalPoly);
    lo->z = x;
    lo->n = NULL;
    lo->s = 2;
    while (x!=NULL)
    {
      nacNormalize(x->ko);
      x = x->ne;
    }
  }
  else
    lo->n = x;
  naTest((number)lo);
  return (number)lo;
}


BOOLEAN naIsZero(number za)
{
  lnumber zb = (lnumber)za;
  naTest(za);
#ifdef TEST
  if ((zb!=NULL) && (zb->z==NULL)) WerrorS("internal zero error(2)");
#endif
  return ((zb==NULL) || (zb->z==NULL));
}


BOOLEAN naGreaterZero(number za)
{
  lnumber zb = (lnumber)za;
#ifdef TEST
  if ((zb!=NULL) && (zb->z==NULL)) WerrorS("internal zero error(3)");
#endif
  naTest(za);
  if ((zb!=NULL) && (zb->z!=NULL))
  {
    if (zb->n!=NULL) return TRUE;
    if ((napDeg(zb->z)==0) && !nacGreaterZero(zb->z->ko)) return FALSE;
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
  naDelete(&h);
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
  alg x;
  lnumber a;
  s = napRead(s, &x);
  if (x==NULL)
  {
    *p = NULL;
    return s;
  }
  *p = (number)Alloc0(sizeof(rnumber));
  a = (lnumber)*p;
  if ((naMinimalPoly!=NULL) && (x->e[0] >= naMinimalPoly->e[0]))
    a->z = napRemainder(x, naMinimalPoly);
#if FEHLER3
  else if (naI!=NULL)
  {
    a->z = napRedp(x);
    if (a->z != NULL)
      a->z = napTailred (a->z);
  }
#endif
  else
    a->z = x;
  if(a->z==NULL)
  {
    Free((ADDRESS)*p,sizeof(rnumber));
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
  char *s=(char *)AllocL(4* naNumbOfPar);
  char *t=(char *)Alloc(8);
  s[0]='\0';
  for (i = 0; i <= naNumbOfPar - 1; i++)
  {
    if (ph->z->e[i] > 0)
    {
      if (ph->z->e[i] >1)
      {
        sprintf(t,"%s%d",naParNames[i],ph->z->e[i]);
        strcat(s,t);
      }
      else
      {
        strcat(s,naParNames[i]);
      }
    }
  }
  Free((ADDRESS)t,8);
  if (s[0]=='\0')
  {
    FreeL((ADDRESS)s);
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
  alg x, y;
  number t;
  if (a==NULL) return FALSE;
  mmTestP(a,sizeof(rnumber));
#ifdef TEST
  if (a->z==NULL) WerrorS("internal zero error(4)");
#endif
  if (a->n==NULL)
  {
    if (napDeg(a->z)==0) return nacIsOne(a->z->ko);
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
      t = nacSub(x->ko, y->ko);
      if (!nacIsZero(t))
      {
        nacDelete(&t);
        return FALSE;
      }
      else
        nacDelete(&t);
    }
    x = x->ne;
    y = y->ne;
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
  alg x, y;
  number t;
  if (a==NULL) return FALSE;
  mmTestP(a,sizeof(rnumber));
#ifdef TEST
  if (a->z==NULL)
  {
    WerrorS("internal zero error(5)");
    return FALSE;
  }
#endif
  if (a->n==NULL)
  {
    if (napDeg(a->z)==0) return nacIsMOne(a->z->ko);
    /*else                 return FALSE;*/
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
    naDelete(rc);
    *rc = x;
  }
}

/*2
* result =gcd(a,b)
*/
number naGcd(number a, number b)
{
  lnumber x, y;
  lnumber result = (lnumber)Alloc0(sizeof(rnumber));

  x = (lnumber)a;
  y = (lnumber)b;
  if (naNumbOfPar == 1)
  {
    if (naMinimalPoly!=NULL)
    {
      if (x->z->ne!=NULL)
        result->z = napCopy(x->z);
      else
        result->z = napGcd0(x->z, y->z);
    }
    else
      result->z = napGcd(x->z, y->z);
  }
  else
    result->z = napGcd(x->z, y->z); // change frpm napGcd0
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

  naTest(pp);
  lnumber p = (lnumber)pp;

  if ((p==NULL) /*|| (p->s==2)*/)
    return;
  p->s = 2;
  alg x = p->z;
  alg y = p->n;
  if ((y!=NULL) && (naMinimalPoly!=NULL))
  {
    y = napInvers(y, naMinimalPoly);
    x = napMult(x, y);
    if (x->e[0] >= naMinimalPoly->e[0])
    x = napRemainder(x, naMinimalPoly);
    p->z = x;
    p->n = y = NULL;
  }
  /* normalize all coefficients in n and z (if in Q) */
  if (naIsChar0)
  {
    while(x!=NULL)
    {
      nacNormalize(x->ko);
      x=x->ne;
    }
    x = p->z;
  }
  if (y==NULL) return;
  if (naIsChar0)
  {
    while(y!=NULL)
    {
      nacNormalize(y->ko);
      y=y->ne;
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
    nacDelete(&n);
    while(x!=NULL)
    {
      nacNormalize(x->ko);
      x=x->ne;
    }
    x = p->z;
    while(y!=NULL)
    {
      nacNormalize(y->ko);
      y=y->ne;
    }
    y = p->n;
  }
#if FEHLER1
  if (naMinimalPoly == NULL)
  {
    int i;
    for (i=naNumbOfPar-1; i>=0; i--)
    {
      alg xx=x;
      alg yy=y;
      int m = napExpi(i, yy, xx);
      if (m != 0)          // in this case xx!=NULL!=yy
      {
        while (xx != NULL)
        {
           xx->e[i] -= m;
           xx = xx->ne;
        }
        while (yy != NULL)
        {
          yy->e[i] -= m;
          yy = yy->ne;
        }
      }
    }
  }
#endif
  if (napDeg(y)==0) /* i.e. y=const => simplify to (1/c)*z / monom */
  {
    if (nacIsOne(y->ko))
    {
      napDelete1(&y);
      p->n = NULL;
      return;
    }
    number h1 = nacInvers(y->ko);
    nacNormalize(h1);
    napMultN(x, h1);
    nacDelete(&h1);
    napDelete1(&y);
    p->n = NULL;
    return;
  }
#ifndef FACTORY_GCD_TEST
  if (naNumbOfPar == 1) /* apply built-in gcd */
  {
    alg x1,y1;
    if (x->e[0] >= y->e[0])
    {
      x1 = napCopy(x);
      y1 = napCopy(y);
    }
    else
    {
      x1 = napCopy(y);
      y1 = napCopy(x);
    }
    alg r;
    loop
    {
      r = napRemainder(x1, y1);
      if ((r==NULL) || (r->ne==NULL)) break;
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
      nacDelete(&n);
      while(x!=NULL)
      {
        nacNormalize(x->ko);
        x=x->ne;
      }
      x = p->z;
      while(y!=NULL)
      {
        nacNormalize(y->ko);
        y=y->ne;
      }
      y = p->n;
    }
    if (y->ne==NULL)
    {
      if (nacIsOne(y->ko))
      {
        if (y->e[0]==0)
        {
          napDelete1(&y);
          p->n = NULL;
        }
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
    alg xx,yy;
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
    number d=nacGcd(g,napGetCoeff(x));
    if(nacIsOne(d))
    {
      nacDelete(&g);
      nacDelete(&d);
      return;
    }
    nacDelete(&g);
    g = d;
    napIter(x);
  }
  while (y!=NULL)
  {
    number d=nacGcd(g,napGetCoeff(y));
    if(nacIsOne(d))
    {
      nacDelete(&g);
      nacDelete(&d);
      return;
    }
    nacDelete(&g);
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
  nacDelete(&g);
}

/*2
* returns in result->n 1
* and in     result->z the lcm(a->z,b->n)
*/
number naLcm(number la, number lb)
{
  lnumber result;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  result = (lnumber)Alloc0(sizeof(rnumber));
  //if (((naMinimalPoly==NULL) && (naI==NULL)) || !naIsChar0)
  //{
  //  result->z = napInit(1);
  //  return (number)result;
  //}
  naNormalize(lb);
  naTest(la);
  naTest(lb);
  alg x = napCopy(a->z);
  number t = napLcm(b->z); // get all denom of b->z
  if (!nacIsOne(t))
  {
    number bt, r;
    alg xx=x;
    while (xx!=NULL)
    {
      bt = nacGcd(t, xx->ko);
      r = nacMult(t, xx->ko);
      nacDelete(&(xx->ko));
      xx->ko = nacDiv(r, bt);
      nacNormalize(xx->ko);
      nacDelete(&bt);
      nacDelete(&r);
      xx=xx->ne;
    }
  }
  nacDelete(&t);
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
    Free((ADDRESS)naI,sizeof(*naI));
    naI=NULL;
  }
  else
  {
    lnumber h;
    number a;
    alg x;

    naI=(naIdeal)Alloc(sizeof(*naI));
    naI->anz=IDELEMS(I);
    naI->liste=(alg*)Alloc(naI->anz*sizeof(alg));
    for (i=IDELEMS(I)-1; i>=0; i--)
    {
      h=(lnumber)pGetCoeff(I->m[i]);
      /* We only need the enumerator of h, as we expect it to be a polynomial */
      naI->liste[i]=napCopy(h->z);
      /* If it isn't normalized (lc = 1) do this */
      if (!nacIsOne(naI->liste[i]->ko))
      {
        x=naI->liste[i];
        a=nacCopy(x->ko);
        a=nacDiv(nacInit(1),a);
        napMultN(x,a);
        nacDelete(&a);
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
  lnumber l=(lnumber)Alloc(sizeof(rnumber));
  l->s=2;
  l->z=(alg)Alloc0(napMonomSize);
  int i=(int)c;
  if (i>(naPrimeM>>2)) i-=naPrimeM;
  l->z->ko=nlInit(i);
  l->n=NULL;
  return (number)l;
}

/*2
* map Q -> Q(a)
*/
number naMap00(number c)
{
  if (nlIsZero(c)) return NULL;
  lnumber l=(lnumber)Alloc(sizeof(rnumber));
  l->s=0;
  l->z=(alg)Alloc0(napMonomSize);
  l->z->ko=nlCopy(c);
  l->n=NULL;
  return (number)l;
}

/*2
* map Z/p -> Z/p(a)
*/
number naMapPP(number c)
{
  if (npIsZero(c)) return NULL;
  lnumber l=(lnumber)Alloc(sizeof(rnumber));
  l->s=2;
  l->z=(alg)Alloc0(napMonomSize);
  l->z->ko=c; /* omit npCopy, because npCopy is a no-op */
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
  lnumber l=(lnumber)Alloc(sizeof(rnumber));
  l->s=2;
  l->z=(alg)Alloc0(napMonomSize);
  l->z->ko=n;
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
  lnumber l=(lnumber)Alloc(sizeof(rnumber));
  l->s=2;
  l->z=(alg)Alloc0(napMonomSize);
  l->z->ko=n;
  l->n=NULL;
  return (number)l;
}

static number (*nacMap)(number);
static int naParsToCopy;
static alg napMap(alg p)
{
  alg w, a;

  if (p==NULL) return NULL;
  a = w = (alg)Alloc0(napMonomSize);
  memcpy(a->e, p->e, naParsToCopy * SIZEOF_PARAMETER);
  w->ko = nacMap(p->ko);
  loop
  {
    p=p->ne;
    if (p==NULL) break;
    a->ne = (alg)Alloc0(napMonomSize);
    a = a->ne;
    memcpy(a->e, p->e, naParsToCopy * SIZEOF_PARAMETER);
    a->ko = nacMap(p->ko);
  }
  a->ne = NULL;
  return w;
}

/*2
* map _(a) -> _(b)
*/
number naMapQaQb(number c)
{
  if (c==NULL) return NULL;
  lnumber erg= (lnumber)Alloc0(sizeof(rnumber));
  lnumber src =(lnumber)c;
  erg->s=src->s;
  erg->z=napMap(src->z);
  erg->n=napMap(src->n);
  return (number)erg;
}

BOOLEAN naSetMap(int c, char ** par, int nop, number minpol)
{
  if (rField_is_Q_a()) /* -> Q(a) */
  {
    if (c == 0)
    {
      nMap = naMap00;   /*Q -> Q(a)*/
      return TRUE;
    }
    if (c>1)
    {
      if (par==NULL)
      {
        naPrimeM = c;
        nMap = naMapP0;  /* Z/p -> Q(a)*/
        return TRUE;
      }
      else
      {
        return FALSE;   /* GF(q) -> Q(a) */
      }
    }
    if (c<0)
    {
      return FALSE;   /* Z/p(a) -> Q(b)*/
    }
    if (c==1)
    {
      int i;
      naParsToCopy=0;
      for(i=0;i<nop;i++)
      {
        if ((i>=rPar(currRing))
        ||(strcmp(par[i],currRing->parameter[i])!=0))
           return FALSE;
        naParsToCopy++;
      }
      nacMap=nacCopy;
      nMap=naMapQaQb;
      return TRUE;   /* Q(a) -> Q(a) */
    }
  }
  /*-----------------------------------------------------*/
  if (rField_is_Zp_a()) /* -> Z/p(a) */
  {
    if (c == 0)
    {
      nMap = naMap0P;   /*Q -> Z/p(a)*/
      return TRUE;
    }
    if (c>1)
    {
      if (par==NULL)
      {
        if (c==npPrimeM)
        {
          nMap = naMapPP;  /* Z/p -> Z/p(a)*/
          return TRUE;
        }
        else
        {
          naPrimeM = c;
          nMap = naMapPP1;  /* Z/p' -> Z/p(a)*/
          return TRUE;
        }
      }
      else
      {
        return FALSE;   /* GF(q) ->Z/p(a) */
      }
    }
    if (c<0)
    {
      if (c==rChar())
      {
        nacMap=nacCopy;
      }
      else
      {
        npMapPrime=-c;
        nacMap = npMapP;
      }
      int i;
      naParsToCopy=0;
      for(i=0;i<nop;i++)
      {
        if ((i>=rPar(currRing))
        ||(strcmp(par[i],currRing->parameter[i])!=0))
           return FALSE;
        naParsToCopy++;
      }
      nMap=naMapQaQb;
      return TRUE;   /* Z/p(a),Z/p'(a) -> Z/p(b)*/
    }
    if (c==1)
    {
      return FALSE;   /* Q(a) -> Z/p(b) */
    }
  }
  return FALSE;      /* default */
}

/*2
* convert a alg number into a poly
*/
poly naPermNumber(number z, int * par_perm, int P)
{
  if (z==NULL) return NULL;
  poly res=NULL;
  poly p;
  napoly za=((lnumber)z)->z;
  do
  {
    p=pInit();
    pNext(p)=NULL;
    nNew(&pGetCoeff(p));
    int i;
    for(i=pVariables;i;i--)
       pSetExp(p,i, 0);
    pSetComp(p, 0);
    napoly pa=NULL;
    if (currRing->parameter!=NULL)
    {
      pGetCoeff(p)=(number)Alloc0(sizeof(rnumber));
      ((lnumber)pGetCoeff(p))->s=2;
      ((lnumber)pGetCoeff(p))->z=napInitz(nacCopy(napGetCoeff(za)));
      pa=((lnumber)pGetCoeff(p))->z;
    }
    else
    {
      pGetCoeff(p)=nCopy(napGetCoeff(za));
    }
    for(i=0;i<P;i++)
    {
      if(za->e[i]!=0)
      {
        if(par_perm[i]>0)
          pSetExp(p,par_perm[i],za->e[i]);
        else if((par_perm[i]<0)&&(pa!=NULL))
          pa->e[-par_perm[i]-1]=za->e[i];
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
    za=za->ne;
  }
  while (za!=NULL);
  pTest(res);
  return res;
}

#ifdef LDEBUG
BOOLEAN naDBTest(number a, char *f,int l)
{
  lnumber x=(lnumber)a;
  if (x == NULL)
    return TRUE;
#ifdef MDEBUG
  mmDBTestBlock(a,sizeof(rnumber),f,l);
#endif
  alg p = x->z;
  if (p==NULL)
  {
    Print("0/* in %s:%d\n",f,l);
    return FALSE;
  }
  while(p!=NULL)
  {
    if ((naIsChar0 && nlIsZero(p->ko))
    || ((!naIsChar0) && npIsZero(p->ko)))
    {
      Print("coeff 0 in %s:%d\n",f,l);
      return FALSE;
    }
    if((naMinimalPoly!=NULL)&&(p->e[0]>naMinimalPoly->e[0])
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
    if (naIsChar0 && !(nlDBTest(p->ko,f,l)))
      return FALSE;
#ifdef MDEBUG
    mmDBTestBlock(p,napMonomSize,f,l);
#endif
    p = p->ne;
  }
  p = x->n;
  while(p!=NULL)
  {
    if (naIsChar0 && !(nlDBTest(p->ko,f,l)))
      return FALSE;
#ifdef MDEBUG
    if (!mmDBTestBlock(p,napMonomSize,f,l))
      return FALSE;
#endif
    p = p->ne;
  }
  return TRUE;
}
#endif

