/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longtrans.cc 12469 2011-02-25 13:38:49Z seelisch $ */
/*
* ABSTRACT:   numbers in transcendental field extensions, i.e.,
              in rational function fields
*/

#include <stdio.h>
#include <string.h>
#include <kernel/mod2.h>
#include <kernel/structs.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/longrat.h>
#include <kernel/modulop.h>
#include <kernel/numbers.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <kernel/ring.h>
#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include <factory/factory.h>
#include <kernel/clapsing.h>
#include <kernel/clapconv.h>
#endif
#include <kernel/longalg.h>
#include <kernel/longtrans.h>

struct sntIdeal
{
  int anz;
  napoly *liste;
};
typedef struct sntIdeal * ntIdeal;

ntIdeal ntI=NULL;

omBin sntIdeal_bin = omGetSpecBin(sizeof(sntIdeal));

#define ntParNames (currRing->parameter)
static int ntIsChar0;
static ring ntMapRing;

#ifdef LDEBUG
#define ntTest(a) ntDBTest(a,__FILE__,__LINE__)
BOOLEAN ntDBTest(number a, const char *f,const int l);
#else
#define ntTest(a)
#endif

number (*ntMap)(number from);
/* procedure variables */
static numberfunc
                ntcMult, ntcSub, ntcAdd, ntcDiv, ntcIntDiv;
static number   (*ntcGcd)(number a, number b, const ring r);
static number   (*ntcLcm)(number a, number b, const ring r);
static number   (*ntcInit)(int i, const ring r);
static int      (*ntcInt)(number &n, const ring r);
static void     (*ntcDelete)(number *a, const ring r);
#undef n_Delete
#define n_Delete(A,R) ntcDelete(A,R)
       void     (*ntcNormalize)(number &a);
static number   (*ntcNeg)(number a);
       number   (*ntcCopy)(number a);
static number   (*ntcInvers)(number a);
       BOOLEAN  (*ntcIsZero)(number a);
static BOOLEAN  (*ntcIsOne)(number a);
static BOOLEAN  (*ntcIsMOne)(number a);
static BOOLEAN  (*ntcGreaterZero)(number a);
static const char   * (*ntcRead) (const char *s, number *a);
static napoly ntpRedp(napoly q);
static napoly ntpTailred(napoly q);
static BOOLEAN ntpDivPoly(napoly p, napoly q);
static int ntpExpi(int i, napoly a, napoly b);
       ring ntcRing;

void ntCoefNormalize(number pp);

#define ntpCopy(p)       p_Copy(p,ntcRing)

static number ntdGcd( number a, number b, const ring r) { return ntcInit(1,r); }
/*2
*  sets the appropriate operators
*/
void ntSetChar(int i, ring r)
{
  if (ntI!=NULL)
  {
    int j;
    for (j=ntI->anz-1; j>=0; j--)
       p_Delete (&ntI->liste[j],ntcRing);
    omFreeSize((ADDRESS)ntI->liste,ntI->anz*sizeof(napoly));
    omFreeBin((ADDRESS)ntI, sntIdeal_bin);
    ntI=NULL;
  }
  ntMap = ntCopy;

  if (r->minideal!=NULL)
  {
    ntI=(ntIdeal)omAllocBin(sntIdeal_bin);
    ntI->anz=IDELEMS(r->minideal);
    ntI->liste=(napoly*)omAlloc(ntI->anz*sizeof(napoly));
    int j;
    for (j=ntI->anz-1; j>=0; j--)
    {
      lnumber a = (lnumber)pGetCoeff(r->minideal->m[j]);
      ntI->liste[j]=ntpCopy(a->z);
    }
  }

  naNumbOfPar=rPar(r);
  if (i == 1)
  {
    ntIsChar0 = 1;
  }
  else if (i < 0)
  {
    ntIsChar0 = 0;
    npSetChar(-i, r->algring); // to be changed HS
  }
#ifdef TEST
  else
  {
    Print("ntSetChar:c=%d param=%d\n",i,rPar(r));
  }
#endif
  ntcRing        = r->algring;
  ntcInit        = ntcRing->cf->cfInit;
  ntcInt         = ntcRing->cf->n_Int;
  ntcCopy        = ntcRing->cf->nCopy;
  ntcAdd         = ntcRing->cf->nAdd;
  ntcSub         = ntcRing->cf->nSub;
  ntcNormalize   = ntcRing->cf->nNormalize;
  ntcNeg         = ntcRing->cf->nNeg;
  ntcIsZero      = ntcRing->cf->nIsZero;
  ntcRead        = ntcRing->cf->nRead;
  ntcGreaterZero = ntcRing->cf->nGreaterZero;
  ntcIsOne       = ntcRing->cf->nIsOne;
  ntcIsMOne      = ntcRing->cf->nIsMOne;
  ntcGcd         = ntcRing->cf->nGcd;
  ntcLcm         = ntcRing->cf->nLcm;
  ntcMult        = ntcRing->cf->nMult;
  ntcDiv         = ntcRing->cf->nDiv;
  ntcIntDiv      = ntcRing->cf->nIntDiv;
  ntcInvers      = ntcRing->cf->nInvers;
  ntcDelete      = ntcRing->cf->cfDelete;
}

/*============= procedure for polynomials: ntpXXXX =======================*/



#ifdef LDEBUG
static void ntpTest(napoly p)
{
  while (p != NULL)
  {
    if (ntIsChar0)
      nlDBTest(pGetCoeff(p), "", 0);
    pIter(p);
  }
}
#else
#define ntpTest(p) ((void) 0)
#endif

#define ntpSetCoeff(p,n) {n_Delete(&pGetCoeff(p),ntcRing);pGetCoeff(p)=n;}
#define ntpComp(p,q)     p_LmCmp((poly)p,(poly)q, ntcRing)
#define ntpMultT(A,E)    A=(napoly)p_Mult_mm((poly)A,(poly)E,ntcRing)
#define ntpDeg(p)        (int)p_Totaldegree(p, ntcRing)

/*3
* creates an napoly
*/
napoly ntpInitz(number z)
{
  napoly a = (napoly)p_Init(ntcRing);
  pGetCoeff(a) = z;
  return a;
}

/*3
* copy a napoly. poly
*/
static napoly ntpCopyNeg(napoly p)
{
  napoly r=ntpCopy(p);
  r=(napoly)p_Neg((poly)r, ntcRing);
  return r;
}

/*3
* returns ph * z
*/
static void ntpMultN(napoly p, number z)
{
  number t;

  while (p!=NULL)
  {
    t = ntcMult(pGetCoeff(p), z);
    ntcNormalize(t);
    n_Delete(&pGetCoeff(p),ntcRing);
    pGetCoeff(p) = t;
    pIter(p);
  }
}

/*3
*  division with rest; f = g*q + r,  destroy f
*/
static void ntpDivMod(napoly f, napoly  g, napoly *q, napoly *r)
{
  napoly a, h, b, qq;

  qq = (napoly)p_Init(ntcRing);
  pNext(qq) = b = NULL;
  p_Normalize(g,ntcRing);
  p_Normalize(f,ntcRing);
  a = f;
  do
  {
    napSetExp(qq,1, p_GetExp(a,1,ntcRing) - p_GetExp(g,1,ntcRing));
    p_Setm(qq,ntcRing);
    pGetCoeff(qq) = ntcDiv(pGetCoeff(a), pGetCoeff(g));
    ntcNormalize(pGetCoeff(qq));
    b = napAdd(b, ntpCopy(qq));
    pGetCoeff(qq) = ntcNeg(pGetCoeff(qq));
    h = ntpCopy(g);
    ntpMultT(h, qq);
    p_Normalize(h,ntcRing);
    n_Delete(&pGetCoeff(qq),ntcRing);
    a = napAdd(a, h);
  }
  while ((a!=NULL) && (p_GetExp(a,1,ntcRing) >= p_GetExp(g,1,ntcRing)));
  omFreeBinAddr(qq);
  *q = b;
  *r = a;
}

/*3
*  returns z with z*x mod c = 1
*/
static napoly ntpInvers(napoly x, const napoly c)
{
  napoly y, r, qa, qn, q;
  number t, h;

  if (p_GetExp(x,1,ntcRing) >= p_GetExp(c,1,ntcRing))
    x = napRemainder(x, c);
  if (x==NULL)
  {
    goto zero_divisor;
  }
  if (p_GetExp(x,1,ntcRing)==0)
  {
    if (!ntcIsOne(pGetCoeff(x)))
    {
      ntcNormalize(pGetCoeff(x));
      t = ntcInvers(pGetCoeff(x));
      ntcNormalize(t);
      n_Delete(&pGetCoeff(x),ntcRing);
      pGetCoeff(x) = t;
    }
    return x;
  }
  y = ntpCopy(c);
  ntpDivMod(y, x, &qa, &r);
  if (r==NULL)
  {
    goto zero_divisor;
  }
  if (p_GetExp(r,1,ntcRing)==0)
  {
    ntcNormalize(pGetCoeff(r));
    t = ntcInvers(pGetCoeff(r));
    ntcNormalize(t);
    t = ntcNeg(t);
    ntpMultN(qa, t);
    n_Delete(&t,ntcRing);
    p_Normalize(qa,ntcRing);
    p_Delete(&x,ntcRing);
    p_Delete(&r,ntcRing);
    return qa;
  }
  y = x;
  x = r;
  ntpDivMod(y, x, &q, &r);
  if (r==NULL)
  {
    goto zero_divisor;
  }
  if (p_GetExp(r,1,ntcRing)==0)
  {
    q = p_Mult_q(q, qa,ntcRing);
    q = napAdd(q, p_ISet(1,ntcRing));
    ntcNormalize(pGetCoeff(r));
    t = ntcInvers(pGetCoeff(r));
    ntpMultN(q, t);
    p_Normalize(q,ntcRing);
    n_Delete(&t,ntcRing);
    p_Delete(&x,ntcRing);
    p_Delete(&r,ntcRing);
    if (p_GetExp(q,1,ntcRing) >= p_GetExp(c,1,ntcRing))
      q = napRemainder(q, c);
    return q;
  }
  q = p_Mult_q(q, ntpCopy(qa),ntcRing);
  q = napAdd(q, p_ISet(1,ntcRing));
  qa = napNeg(qa);
  loop
  {
    y = x;
    x = r;
    ntpDivMod(y, x, &qn, &r);
    if (r==NULL)
    {
      break;
    }
    if (p_GetExp(r,1,ntcRing)==0)
    {
      q = p_Mult_q(q, qn,ntcRing);
      q = napNeg(q);
      q = napAdd(q, qa);
      ntcNormalize(pGetCoeff(r));
      t = ntcInvers(pGetCoeff(r));
      //ntcNormalize(t);
      ntpMultN(q, t);
      p_Normalize(q,ntcRing);
      n_Delete(&t,ntcRing);
      p_Delete(&x,ntcRing);
      p_Delete(&r,ntcRing);
      if (p_GetExp(q,1,ntcRing) >= p_GetExp(c,1,ntcRing))
        q = napRemainder(q, c);
      return q;
    }
    y = q;
    q = p_Mult_q(ntpCopy(q), qn, ntcRing);
    q = napNeg(q);
    q = napAdd(q, qa);
    qa = y;
  }
// zero divisor found:
zero_divisor:
  Werror("zero divisor found - your minpoly is not irreducible");
  return x;
}

/*3
* the max degree of an napoly poly (used for test of "simple" et al.)
*/
static int  ntpMaxDeg(napoly p)
{
  int  d = 0;
  while(p!=NULL)
  {
    d=si_max(d,ntpDeg(p));
    pIter(p);
  }
  return d;
}

/*3
* the max degree of an napoly poly (used for test of "simple" et al.)
*/
static int  ntpMaxDegLen(napoly p, int &l)
{
  int  d = 0;
  int ll=0;
  while(p!=NULL)
  {
    d=si_max(d,ntpDeg(p));
    pIter(p);
    ll++;
  }
  l=ll;
  return d;
}


/*3
*writes a polynomial number
*/
void ntpWrite(napoly p,const BOOLEAN has_denom, const ring r)
{
  ring ntcring=r->algring;
  if (p==NULL)
    StringAppendS("0");
  else if (p_LmIsConstant(p,ntcring))
  {
    BOOLEAN kl=FALSE;
    if (has_denom)
    {
      number den=ntcring->cf->cfGetDenom(pGetCoeff(p),ntcring );
      kl=!n_IsOne(den,ntcring);
      n_Delete(&den, ntcring);
    }
    if (kl) StringAppendS("(");
    //StringAppendS("-1");
    n_Write(pGetCoeff(p),ntcring);
    if (kl) StringAppendS(")");
  }
  else
  {
    StringAppendS("(");
    loop
    {
      BOOLEAN wroteCoeff=FALSE;
      if ((p_LmIsConstant(p,ntcring))
      || ((!n_IsOne(pGetCoeff(p),ntcring))
        && (!n_IsMOne(pGetCoeff(p),ntcring))))
      {
        n_Write(pGetCoeff(p),ntcring);
        wroteCoeff=(r->ShortOut==0);
      }
      else if (n_IsMOne(pGetCoeff(p),ntcring))
      {
        StringAppendS("-");
      }
      int  i;
      for (i = 0; i < r->P; i++)
      {
        int e=p_GetExp(p,i+1,ntcring);
        if (e > 0)
        {
          if (wroteCoeff)
            StringAppendS("*");
          else
            wroteCoeff=(r->ShortOut==0);
          StringAppendS(r->parameter[i]);
          if (e > 1)
          {
            if (r->ShortOut == 0)
              StringAppendS("^");
            StringAppend("%d", e);
          }
        }
      } /*for*/
      pIter(p);
      if (p==NULL)
        break;
      if (n_GreaterZero(pGetCoeff(p),ntcring))
        StringAppendS("+");
    }
    StringAppendS(")");
  }
}


static const char *ntpHandleMons(const char *s, int i, napoly ex)
{
  int  j;
  if (strncmp(s,ntParNames[i],strlen(ntParNames[i]))==0)
  {
    s+=strlen(ntParNames[i]);
    if ((*s >= '0') && (*s <= '9'))
    {
      s = eati(s, &j);
      napAddExp(ex,i+1,j);
    }
    else
      napAddExp(ex,i+1,1);
  }
  return s;
}
static const char *ntpHandlePars(const char *s, int i, napoly ex)
{
  int  j;
  if (strcmp(s,ntParNames[i])==0)
  {
    s+=strlen(ntParNames[i]);
    napSetExp(ex,i+1,1);
  }
  return s;
}

/*3  reads a monomial  */
static const char  *ntpRead(const char *s, napoly *b)
{
  napoly a;
  int  i;
  a = (napoly)p_Init(ntcRing);
  if ((*s >= '0') && (*s <= '9'))
  {
    s = ntcRead(s, &pGetCoeff(a));
    if (ntcIsZero(pGetCoeff(a)))
    {
      p_LmDelete(&a,ntcRing);
      *b = NULL;
      return s;
    }
  }
  else
    pGetCoeff(a) = ntcInit(1,ntcRing);
  i = 0;
  const char  *olds=s;
  loop
  {
    s = ntpHandlePars(s, i, a);
    if (olds == s)
      i++;
    else if (*s == '\0')
    {
      *b = a;
      return s;
    }
    if (i >= naNumbOfPar)
      break;
  }
  i=0;
  loop
  {
    olds = s;
    s = ntpHandleMons(s, i, a);
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

static int ntpExp(napoly a, napoly b)
{
  while (pNext(a)!=NULL) pIter(a);
  int m = p_GetExp(a,1,ntcRing);
  if (m==0) return 0;
  while (pNext(b)!=NULL) pIter(b);
  int mm=p_GetExp(b,1,ntcRing);
  if (m > mm) m = mm;
  return m;
}

/*
* finds the smallest i-th exponent in a and b
* used to find it in a fraction
*/
static int ntpExpi(int i, napoly a, napoly b)
{
  if (a==NULL || b==NULL) return 0;
  int m = p_GetExp(a,i+1,ntcRing);
  if (m==0) return 0;
  while (pNext(a) != NULL)
  {
    pIter(a);
    if (m > p_GetExp(a,i+1,ntcRing))
    {
      m = p_GetExp(a,i+1,ntcRing);
      if (m==0) return 0;
    }
  }
  do
  {
    if (m > p_GetExp(b,i+1,ntcRing))
    {
      m = p_GetExp(b,i+1,ntcRing);
      if (m==0) return 0;
    }
    pIter(b);
  }
  while (b != NULL);
  return m;
}

static void ntpContent(napoly ph)
{
  number h,d;
  napoly p;

  p = ph;
  if (ntcIsOne(pGetCoeff(p)))
    return;
  h = ntcCopy(pGetCoeff(p));
  pIter(p);
  do
  {
    d=ntcGcd(pGetCoeff(p), h, ntcRing);
    if(ntcIsOne(d))
    {
      n_Delete(&h,ntcRing);
      n_Delete(&d,ntcRing);
      return;
    }
    n_Delete(&h,ntcRing);
    h = d;
    pIter(p);
  }
  while (p!=NULL);
  h = ntcInvers(d);
  n_Delete(&d,ntcRing);
  p = ph;
  while (p!=NULL)
  {
    d = ntcMult(pGetCoeff(p), h);
    n_Delete(&pGetCoeff(p),ntcRing);
    pGetCoeff(p) = d;
    pIter(p);
  }
  n_Delete(&h,ntcRing);
}

static void ntpCleardenom(napoly ph)
{
  number d, h;
  napoly p;

  if (!ntIsChar0)
    return;
  p = ph;
  h = ntcInit(1,ntcRing);
  while (p!=NULL)
  {
    d = ntcLcm(h, pGetCoeff(p), ntcRing);
    n_Delete(&h,ntcRing);
    h = d;
    pIter(p);
  }
  if(!ntcIsOne(h))
  {
    p = ph;
    while (p!=NULL)
    {
      d=ntcMult(h, pGetCoeff(p));
      n_Delete(&pGetCoeff(p),ntcRing);
      ntcNormalize(d);
      pGetCoeff(p) = d;
      pIter(p);
    }
    n_Delete(&h,ntcRing);
  }
  ntpContent(ph);
}

static napoly ntpGcd0(napoly a, napoly b)
{
  number x, y;
  if (!ntIsChar0)
    return p_ISet(1,ntcRing);
  x = ntcCopy(pGetCoeff(a));
  if (ntcIsOne(x))
    return ntpInitz(x);
  while (pNext(a)!=NULL)
  {
    pIter(a);
    y = ntcGcd(x, pGetCoeff(a), ntcRing);
    n_Delete(&x,ntcRing);
    x = y;
    if (ntcIsOne(x))
      return ntpInitz(x);
  }
  do
  {
    y = ntcGcd(x, pGetCoeff(b), ntcRing);
    n_Delete(&x,ntcRing);
    x = y;
    if (ntcIsOne(x))
      return ntpInitz(x);
    pIter(b);
  }
  while (b!=NULL);
  return ntpInitz(x);
}

/*3
* result =gcd(a,b)
*/
static napoly ntpGcd(napoly a, napoly b)
{
  int i;
  napoly g, x, y, h;
  if ((a==NULL)
  || ((pNext(a)==NULL)&&(ntcIsZero(pGetCoeff(a)))))
  {
    if ((b==NULL)
    || ((pNext(b)==NULL)&&(ntcIsZero(pGetCoeff(b)))))
    {
      return p_ISet(1,ntcRing);
    }
    return ntpCopy(b);
  }
  else
  if ((b==NULL)
  || ((pNext(b)==NULL)&&(ntcIsZero(pGetCoeff(b)))))
  {
    return ntpCopy(a);
  }
  // Hmm ... this is a memory leak
  // x = (napoly)p_Init(ntcRing);
  g=a;
  h=b;
  if (!ntIsChar0) x = p_ISet(1,ntcRing);
  else            x = ntpGcd0(g,h);
  for (i=(naNumbOfPar-1); i>=0; i--)
  {
    napSetExp(x,i+1, ntpExpi(i,a,b));
    p_Setm(x,ntcRing);
  }
  return x;
}


number ntpLcm(napoly a)
{
  number h = ntcInit(1,ntcRing);

  if (ntIsChar0)
  {
    number d;
    napoly b = a;

    while (b!=NULL)
    {
      d = ntcLcm(h, pGetCoeff(b), ntcRing);
      n_Delete(&h,ntcRing);
      h = d;
      pIter(b);
    }
  }
  return h;
}


/*2
* meins  (for reduction in algebraic extension)
* checks if head of p divides head of q
* doesn't delete p and q
*/
static BOOLEAN ntpDivPoly (napoly p, napoly q)
{
  int j=1;   /* evtl. von ntNumber.. -1 abwaerts zaehlen */

  while (p_GetExp(p,j,ntcRing) <= p_GetExp(q,j,ntcRing))
  {
    j++;
    if (j > naNumbOfPar)
      return 1;
  }
  return 0;
}


/*2
* meins  (for reduction in algebraic extension)
* Normalform of poly with ntI
* changes q and returns it
*/
napoly ntpRedp (napoly q)
{
  napoly h = (napoly)p_Init(ntcRing);
  int i=0,j;

  loop
  {
    if (ntpDivPoly (ntI->liste[i], q))
    {
      /* h = lt(q)/lt(ntI->liste[i])*/
      pGetCoeff(h) = ntcCopy(pGetCoeff(q));
      for (j=naNumbOfPar; j>0; j--)
        napSetExp(h,j, p_GetExp(q,j,ntcRing) - p_GetExp(ntI->liste[i],j,ntcRing));
      p_Setm(h,ntcRing);
      h = p_Mult_q(h, ntpCopy(ntI->liste[i]),ntcRing);
      h = napNeg (h);
      q = napAdd (q, ntpCopy(h));
      p_Delete (&pNext(h),ntcRing);
      if (q == NULL)
      {
        p_Delete(&h,ntcRing);
        return q;
      }
      /* try to reduce further */
      i = 0;
    }
    else
    {
      i++;
      if (i >= ntI->anz)
      {
        p_Delete(&h,ntcRing);
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
napoly ntpTailred (napoly q)
{
  napoly h;

  h = pNext(q);
  while (h != NULL)
  {
     h = ntpRedp (h);
     if (h == NULL)
        return q;
     pIter(h);
  }
  return q;
}


/*================ procedure for rational functions: ntXXXX =================*/

/*2
*  z:= i
*/
number ntInit(int i, const ring r)
{
  if (i!=0)
  {
    number c=n_Init(i,r->algring);
    if (!n_IsZero(c,r->algring))
    {
      poly z=p_Init(r->algring);
      pSetCoeff0(z,c);
      lnumber l = (lnumber)omAllocBin(rnumber_bin);
      l->z = z;
      l->s = 2;
      l->n = NULL;
      return (number)l;
    }
  }
  /*else*/
  return NULL;
}

number  ntPar(int i)
{
  lnumber l = (lnumber)omAllocBin(rnumber_bin);
  l->s = 2;
  l->z = p_ISet(1,ntcRing);
  napSetExp(l->z,i,1);
  p_Setm(l->z,ntcRing);
  l->n = NULL;
  return (number)l;
}

int     ntParDeg(number n)     /* i := deg(n) */
{
  lnumber l = (lnumber)n;
  if (l==NULL) return -1;
  return ntpDeg(l->z);
}

//int     ntParDeg(number n)     /* i := deg(n) */
//{
//  lnumber l = (lnumber)n;
//  if (l==NULL) return -1;
//  return ntpMaxDeg(l->z)+ntpMaxDeg(l->n);
//}

int     ntSize(number n)     /* size desc. */
{
  lnumber l = (lnumber)n;
  if (l==NULL) return -1;
  int len_z;
  int len_n;
  int o=ntpMaxDegLen(l->z,len_z)+ntpMaxDegLen(l->n,len_n);
  return (len_z+len_n)+o;
}

/*2
* convert a number to int (if possible)
*/
int ntInt(number &n, const ring r)
{
  lnumber l=(lnumber)n;
  if ((l!=NULL)&&(l->n==NULL)&&(p_IsConstant(l->z,r->algring)))
  {
    return ntcInt(pGetCoeff(l->z),r->algring);
  }
  return 0;
}

/*2
*  deletes p
*/
void ntDelete(number *p, const ring r)
{
  if ((*p)!=NULL)
  {
    lnumber l = (lnumber) * p;
    if (l==NULL) return;
    p_Delete(&(l->z),r->algring);
    p_Delete(&(l->n),r->algring);
    omFreeBin((ADDRESS)l,  rnumber_bin);
  }
  *p = NULL;
}

/*2
* copy p to erg
*/
number ntCopy(number p)
{
  if (p==NULL) return NULL;
  ntTest(p);
  lnumber erg;
  lnumber src = (lnumber)p;
  erg = (lnumber)omAlloc0Bin(rnumber_bin);
  erg->z = p_Copy(src->z, ntcRing);
  erg->n = p_Copy(src->n, ntcRing);
  erg->s = src->s;
  return (number)erg;
}
number nt_Copy(number p, const ring r)
{
  if (p==NULL) return NULL;
  lnumber erg;
  lnumber src = (lnumber)p;
  erg = (lnumber)omAlloc0Bin(rnumber_bin);
  erg->z = p_Copy(src->z,r->algring);
  erg->n = p_Copy(src->n,r->algring);
  erg->s = src->s;
  return (number)erg;
}

/*2
*  addition; lu:= la + lb
*/
number ntAdd(number la, number lb)
{
  if (la==NULL) return ntCopy(lb);
  if (lb==NULL) return ntCopy(la);

  napoly x, y;
  lnumber lu;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  #ifdef LDEBUG
  omCheckAddrSize(a,sizeof(snumber));
  omCheckAddrSize(b,sizeof(snumber));
  #endif
  if (b->n!=NULL) x = pp_Mult_qq(a->z, b->n,ntcRing);
  else            x = ntpCopy(a->z);
  if (a->n!=NULL) y = pp_Mult_qq(b->z, a->n,ntcRing);
  else            y = ntpCopy(b->z);
  napoly res = napAdd(x, y);
  if (res==NULL)
  {
    return (number)NULL;
  }
  lu = (lnumber)omAllocBin(rnumber_bin);
  lu->z=res;
  if (a->n!=NULL)
  {
    if (b->n!=NULL) x = pp_Mult_qq(a->n, b->n,ntcRing);
    else            x = ntpCopy(a->n);
  }
  else
  {
    if (b->n!=NULL) x = ntpCopy(b->n);
    else            x = NULL;
  }
  //if (x!=NULL)
  //{
  //  if (p_LmIsConstant(x,ntcRing))
  //  {
  //    number inv=ntcInvers(pGetCoeff(x));
  //    ntpMultN(lu->z,inv);
  //    n_Delete(&inv,ntcRing);
  //    ntpDelete(&x);
  //  }
  //}
  lu->n = x;
  lu->s = FALSE;
  if (/*lu->n*/ x!=NULL)
  {
     number luu=(number)lu;
     //if (p_IsConstant(lu->n,ntcRing)) ntCoefNormalize(luu);
     //else
                ntNormalize(luu);
     lu=(lnumber)luu;
  }
  //else lu->s=2;
  ntTest((number)lu);
  return (number)lu;
}

/*2
*  subtraction; r:= la - lb
*/
number ntSub(number la, number lb)
{
  lnumber lu;

  if (lb==NULL) return ntCopy(la);
  if (la==NULL)
  {
    lu = (lnumber)ntCopy(lb);
    lu->z = napNeg(lu->z);
    return (number)lu;
  }

  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;

  #ifdef LDEBUG
  omCheckAddrSize(a,sizeof(snumber));
  omCheckAddrSize(b,sizeof(snumber));
  #endif

  napoly x, y;
  if (b->n!=NULL) x = pp_Mult_qq(a->z, b->n,ntcRing);
  else            x = ntpCopy(a->z);
  if (a->n!=NULL) y = p_Mult_q(ntpCopy(b->z), ntpCopyNeg(a->n),ntcRing);
  else            y = ntpCopyNeg(b->z);
  napoly res = napAdd(x, y);
  if (res==NULL)
  {
    return (number)NULL;
  }
  lu = (lnumber)omAllocBin(rnumber_bin);
  lu->z=res;
  if (a->n!=NULL)
  {
    if (b->n!=NULL) x = pp_Mult_qq(a->n, b->n,ntcRing);
    else            x = ntpCopy(a->n);
  }
  else
  {
    if (b->n!=NULL) x = ntpCopy(b->n);
    else            x = NULL;
  }
  lu->n = x;
  lu->s = FALSE;
  if (/*lu->n*/ x!=NULL)
  {
     number luu=(number)lu;
     //if (p_IsConstant(lu->n,ntcRing)) ntCoefNormalize(luu);
     //else
                         ntNormalize(luu);
     lu=(lnumber)luu;
  }
  //else lu->s=2;
  ntTest((number)lu);
  return (number)lu;
}

/*2
*  multiplication; r:= la * lb
*/
number ntMult(number la, number lb)
{
  if ((la==NULL) || (lb==NULL))
    return NULL;

  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  lnumber lo;
  napoly x;

  #ifdef LDEBUG
  omCheckAddrSize(a,sizeof(snumber));
  omCheckAddrSize(b,sizeof(snumber));
  #endif
  ntTest(la);
  ntTest(lb);

  lo = (lnumber)omAllocBin(rnumber_bin);
  lo->z = pp_Mult_qq(a->z, b->z,ntcRing);

  if (a->n==NULL)
  {
    if (b->n==NULL)
      x = NULL;
    else
      x = ntpCopy(b->n);
  }
  else
  {
    if (b->n==NULL)
    {
      x = ntpCopy(a->n);
    }
    else
    {
      x = pp_Mult_qq(b->n, a->n, ntcRing);
    }
  }
  if (ntI!=NULL)
  {
    lo->z = ntpRedp (lo->z);
    if (lo->z != NULL)
       lo->z = ntpTailred (lo->z);
    if (x!=NULL)
    {
      x = ntpRedp (x);
      if (x!=NULL)
        x = ntpTailred (x);
    }
  }
  if ((x!=NULL) && (p_LmIsConstant(x,ntcRing)) && ntcIsOne(pGetCoeff(x)))
    p_Delete(&x,ntcRing);
  lo->n = x;
  lo->s = 0;
  if(lo->z==NULL)
  {
    omFreeBin((ADDRESS)lo, rnumber_bin);
    lo=NULL;
  }
  else if (lo->n!=NULL)
  {
    number luu=(number)lo;
    // if (p_IsConstant(lo->n,ntcRing)) ntCoefNormalize(luu);
    // else
                      ntNormalize(luu);
    lo=(lnumber)luu;
  }
  //if (naMinimalPoly==NULL) lo->s=2;
  ntTest((number)lo);
  return (number)lo;
}

number ntIntDiv(number la, number lb)
{
  lnumber res;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  if (a==NULL)
  {
    return NULL;
  }
  if (b==NULL)
  {
    WerrorS(nDivBy0);
    return NULL;
  }
  assume(a->z!=NULL && b->z!=NULL);
  assume(a->n==NULL && b->n==NULL);
  res = (lnumber)omAllocBin(rnumber_bin);
  res->z = ntpCopy(a->z);
  res->n = ntpCopy(b->z);
  res->s = 0;
  number nres=(number)res;
  ntNormalize(nres);

  //ntpDelete(&res->n);
  ntTest(nres);
  return nres;
}

/*2
*  division; lo:= la / lb
*/
number ntDiv(number la, number lb)
{
  lnumber lo;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  napoly x;

  if (a==NULL)
    return NULL;

  if (b==NULL)
  {
    WerrorS(nDivBy0);
    return NULL;
  }
  #ifdef LDEBUG
  omCheckAddrSize(a,sizeof(snumber));
  omCheckAddrSize(b,sizeof(snumber));
  #endif
  lo = (lnumber)omAllocBin(rnumber_bin);
  if (b->n!=NULL)
    lo->z = pp_Mult_qq(a->z, b->n,ntcRing);
  else
    lo->z = ntpCopy(a->z);
  if (a->n!=NULL)
    x = pp_Mult_qq(b->z, a->n, ntcRing);
  else
    x = ntpCopy(b->z);
  if (ntI!=NULL)
  {
    lo->z = ntpRedp (lo->z);
    if (lo->z != NULL)
       lo->z = ntpTailred (lo->z);
    if (x!=NULL)
    {
      x = ntpRedp (x);
      if (x!=NULL)
        x = ntpTailred (x);
    }
  }
  if ((p_LmIsConstant(x,ntcRing)) && ntcIsOne(pGetCoeff(x)))
    p_Delete(&x,ntcRing);
  lo->n = x;
  lo->s = 0;
  if (lo->n!=NULL)
  {
    number luu=(number)lo;
     //if (p_IsConstant(lo->n,ntcRing)) ntCoefNormalize(luu);
     //else
                         ntNormalize(luu);
    lo=(lnumber)luu;
  }
  //else lo->s=2;
  ntTest((number)lo);
  return (number)lo;
}

/*2
*  za:= - za, inplace
*/
number ntNeg(number za)
{
  if (za!=NULL)
  {
    lnumber e = (lnumber)za;
    ntTest(za);
    e->z = napNeg(e->z);
  }
  return za;
}

/*2
* 1/a
*/
number ntInvers(number a)
{
  lnumber lo;
  lnumber b = (lnumber)a;
  napoly x;

  if (b==NULL)
  {
    WerrorS(nDivBy0);
    return NULL;
  }
  #ifdef LDEBUG
  omCheckAddrSize(b,sizeof(snumber));
  #endif
  lo = (lnumber)omAlloc0Bin(rnumber_bin);
  lo->s = b->s;
  if (b->n!=NULL)
    lo->z = ntpCopy(b->n);
  else
    lo->z = p_ISet(1,ntcRing);
  x = b->z;
  if ((!p_LmIsConstant(x,ntcRing)) || !ntcIsOne(pGetCoeff(x)))
    x = ntpCopy(x);
  else
  {
    lo->n = NULL;
    ntTest((number)lo);
    return (number)lo;
  }
  lo->n = x;
  if (lo->n!=NULL)
  {
     number luu=(number)lo;
     //if (p_IsConstant(lo->n,ntcRing)) ntCoefNormalize(luu);
     //else
                           ntNormalize(luu);
     lo=(lnumber)luu;
  }
  ntTest((number)lo);
  return (number)lo;
}


BOOLEAN ntIsZero(number za)
{
  lnumber zb = (lnumber)za;
  ntTest(za);
#ifdef LDEBUG
  if ((zb!=NULL) && (zb->z==NULL)) WerrorS("internal zero error(2)");
#endif
  return (zb==NULL);
}


BOOLEAN ntGreaterZero(number za)
{
  lnumber zb = (lnumber)za;
#ifdef LDEBUG
  if ((zb!=NULL) && (zb->z==NULL)) WerrorS("internal zero error(3)");
#endif
  ntTest(za);
  if (zb!=NULL)
  {
    return (ntcGreaterZero(pGetCoeff(zb->z))||(!p_LmIsConstant(zb->z,ntcRing)));
  }
  /* else */ return FALSE;
}


/*2
* a = b ?
*/
BOOLEAN ntEqual (number a, number b)
{
  if(a==b) return TRUE;
  if((a==NULL)&&(b!=NULL)) return FALSE;
  if((b==NULL)&&(a!=NULL)) return FALSE;

  lnumber aa=(lnumber)a;
  lnumber bb=(lnumber)b;

  int an_deg=0;
  if(aa->n!=NULL)
    an_deg=ntpDeg(aa->n);
  int bn_deg=0;
  if(bb->n!=NULL)
    bn_deg=ntpDeg(bb->n);
  if(an_deg+ntpDeg(bb->z)!=bn_deg+ntpDeg(aa->z))
    return FALSE;
#if 0
  ntNormalize(a);
  aa=(lnumber)a;
  ntNormalize(b);
  bb=(lnumber)b;
  if((aa->n==NULL)&&(bb->n!=NULL)) return FALSE;
  if((bb->n==NULL)&&(aa->n!=NULL)) return FALSE;
  if(ntpComp(aa->z,bb->z)!=0) return FALSE;
  if((aa->n!=NULL) && (ntpComp(aa->n,bb->n))) return FALSE;
#endif
  number h = ntSub(a, b);
  BOOLEAN bo = ntIsZero(h);
  ntDelete(&h,currRing);
  return bo;
}


BOOLEAN ntGreater (number a, number b)
{
  if (ntIsZero(a))
    return FALSE;
  if (ntIsZero(b))
    return TRUE; /* a!= 0)*/
  return ntpDeg(((lnumber)a)->z)>ntpDeg(((lnumber)b)->z);
}

/*2
* reads a number
*/
const char  *ntRead(const char *s, number *p)
{
  napoly x;
  lnumber a;
  s = ntpRead(s, &x);
  if (x==NULL)
  {
    *p = NULL;
    return s;
  }
  *p = (number)omAlloc0Bin(rnumber_bin);
  a = (lnumber)*p;
  if (ntI!=NULL)
  {
    a->z = ntpRedp(x);
    if (a->z != NULL)
      a->z = ntpTailred (a->z);
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
    ntTest(*p);
  }
  return s;
}

/*2
* tries to convert a number to a name
*/
char * ntName(number n)
{
  lnumber ph = (lnumber)n;
  if (ph==NULL)
    return NULL;
  int i;
  char *s=(char *)omAlloc(4* naNumbOfPar);
  char *t=(char *)omAlloc(8);
  s[0]='\0';
  for (i = 0; i <= naNumbOfPar - 1; i++)
  {
    int e=p_GetExp(ph->z,i+1,ntcRing);
    if (e > 0)
    {
      if (e >1)
      {
        sprintf(t,"%s%d",ntParNames[i],e);
        strcat(s,t);
      }
      else
      {
        strcat(s,ntParNames[i]);
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
void ntWrite(number &phn, const ring r)
{
  lnumber ph = (lnumber)phn;
  if (ph==NULL)
    StringAppendS("0");
  else
  {
    phn->s = 0;
    BOOLEAN has_denom=(ph->n!=NULL);
    ntpWrite(ph->z,has_denom/*(ph->n!=NULL)*/,r);
    if (has_denom/*(ph->n!=NULL)*/)
    {
      StringAppendS("/");
      ntpWrite(ph->n,TRUE,r);
    }
  }
}

/*2
* za == 1 ?
*/
BOOLEAN ntIsOne(number za)
{
  lnumber a = (lnumber)za;
  napoly x, y;
  number t;
  if (a==NULL) return FALSE;
#ifdef LDEBUG
  omCheckAddrSize(a,sizeof(snumber));
  if (a->z==NULL)
  {
    WerrorS("internal zero error(4)");
    return FALSE;
  }
#endif
  if (a->n==NULL)
  {
    if (p_LmIsConstant(a->z,ntcRing))
    {
      return ntcIsOne(pGetCoeff(a->z));
    }
    else                 return FALSE;
  }
#if 0
  x = a->z;
  y = a->n;
  do
  {
    if (ntpComp(x, y))
      return FALSE;
    else
    {
      t = ntcSub(pGetCoeff(x), pGetCoeff(y));
      if (!ntcIsZero(t))
      {
        n_Delete(&t,ntcRing);
        return FALSE;
      }
      else
        n_Delete(&t,ntcRing);
    }
    pIter(x);
    pIter(y);
  }
  while ((x!=NULL) && (y!=NULL));
  if ((x!=NULL) || (y!=NULL)) return FALSE;
  p_Delete(&a->z,ntcRing);
  p_Delete(&a->n,ntcRing);
  a->z = p_ISet(1,ntcRing);
  a->n = NULL;
  return TRUE;
#else
  return FALSE;
#endif
}

/*2
* za == -1 ?
*/
BOOLEAN ntIsMOne(number za)
{
  lnumber a = (lnumber)za;
  napoly x, y;
  number t;
  if (a==NULL) return FALSE;
#ifdef LDEBUG
  omCheckAddrSize(a,sizeof(snumber));
  if (a->z==NULL)
  {
    WerrorS("internal zero error(5)");
    return FALSE;
  }
#endif
  if (a->n==NULL)
  {
    if (p_LmIsConstant(a->z,ntcRing)) return ntcIsMOne(pGetCoeff(a->z));
    /*else                   return FALSE;*/
  }
  return FALSE;
}

/*2
* returns the i-th power of p (i>=0)
*/
void ntPower(number p, int i, number *rc)
{
  number x;
  *rc = ntInit(1,currRing);
  for (; i > 0; i--)
  {
    x = ntMult(*rc, p);
    ntDelete(rc,currRing);
    *rc = x;
  }
}

/*2
* result =gcd(a,b)
*/
number ntGcd(number a, number b, const ring r)
{
  if (a==NULL)  return ntCopy(b);
  if (b==NULL)  return ntCopy(a);

  lnumber x, y;
  lnumber result = (lnumber)omAlloc0Bin(rnumber_bin);

  x = (lnumber)a;
  y = (lnumber)b;
#ifndef HAVE_FACTORY
  result->z = ntpGcd(x->z, y->z); // change from ntpGcd0
#else
  int c=ABS(nGetChar());
  if (c==1) c=0;
  setCharacteristic( c );

  napoly rz=ntpGcd(x->z, y->z);
  CanonicalForm F, G, R;
  R=convSingPFactoryP(rz,r->algring);
  p_Normalize(x->z,ntcRing);
  F=convSingPFactoryP(x->z,r->algring)/R;
  p_Normalize(y->z,ntcRing);
  G=convSingPFactoryP(y->z,r->algring)/R;
  F = gcd( F, G );
  if (F.isOne())
    result->z= rz;
  else
  {
    p_Delete(&rz,r->algring);
    result->z=convFactoryPSingP( F*R,r->algring );
    p_Normalize(result->z,ntcRing);
  }
#endif
  ntTest((number)result);
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
//#define FACTORY_GCD_TEST
void ntCoefNormalize(number pp)
{
  if (pp==NULL) return;
  lnumber p = (lnumber)pp;
  number nz; // all denom. of the numerator
  nz=p_GetAllDenom(p->z,ntcRing);
  BOOLEAN norm=FALSE;
  if (!n_IsOne(nz,ntcRing))
  {
    norm=TRUE;
    p->z=p_Mult_nn(p->z,nz,ntcRing);
    if (p->n==NULL)
    {
      p->n=p_NSet(nz,ntcRing);
    }
    else
    {
      p->n=p_Mult_nn(p->n,nz,ntcRing);
      n_Delete(&nz, ntcRing);
    }
  }
  else
  {
    n_Delete(&nz, ntcRing);
  }
  if (norm)
  {
    norm=FALSE;
    p_Normalize(p->z,ntcRing);
    p_Normalize(p->n,ntcRing);
  }
  number nn;
  nn=p_GetAllDenom(p->n,ntcRing);
  if (!n_IsOne(nn,ntcRing))
  {
    norm=TRUE;
    p->n=p_Mult_nn(p->n,nn,ntcRing);
    p->z=p_Mult_nn(p->z,nn,ntcRing);
    n_Delete(&nn, ntcRing);
  }
  else
  {
    n_Delete(&nn, ntcRing);
  }
  if (norm)
  {
    p_Normalize(p->z,ntcRing);
    p_Normalize(p->n,ntcRing);
  }
  // remove common factors in n, z:
  if (p->n!=NULL)
  {
    poly pp=p->z;
    nz=n_Copy(pGetCoeff(pp),ntcRing);
    pIter(pp);
    while(pp!=NULL)
    {
      if (n_IsOne(nz,ntcRing)) break;
      number d=n_Gcd(nz,pGetCoeff(pp),ntcRing);
      n_Delete(&nz,ntcRing); nz=d;
      pIter(pp);
    }
    if (!n_IsOne(nz,ntcRing))
    {
      pp=p->n;
      nn=n_Copy(pGetCoeff(pp),ntcRing);
      pIter(pp);
      while(pp!=NULL)
      {
        if (n_IsOne(nn,ntcRing)) break;
        number d=n_Gcd(nn,pGetCoeff(pp),ntcRing);
        n_Delete(&nn,ntcRing); nn=d;
        pIter(pp);
      }
      number ng=n_Gcd(nz,nn,ntcRing);
      n_Delete(&nn,ntcRing);
      if (!n_IsOne(ng,ntcRing))
      {
        number ni=n_Invers(ng,ntcRing);
	p->z=p_Mult_nn(p->z,ni,ntcRing);
	p->n=p_Mult_nn(p->n,ni,ntcRing);
        p_Normalize(p->z,ntcRing);
        p_Normalize(p->n,ntcRing);
	n_Delete(&ni,ntcRing);
      }
      n_Delete(&ng,ntcRing);
    }
    n_Delete(&nz,ntcRing);
  }
  if (p->n!=NULL)
  {
    if(!ntcGreaterZero(pGetCoeff(p->n)))
    {
      p->z=napNeg(p->z);
      p->n=napNeg(p->n);
    }

    if (/*(p->n!=NULL) && */
    (p_IsConstant(p->n,ntcRing))
    && (n_IsOne(pGetCoeff(p->n),ntcRing)))
    {
      p_Delete(&(p->n), ntcRing);
      p->n = NULL;
    }
  }
}

void ntNormalize(number &pp)
{

  //ntTest(pp); // input may not be "normal"
  lnumber p = (lnumber)pp;

  if (p==NULL)
    return;
  ntCoefNormalize(pp);
  p->s = 2;
  napoly x = p->z;
  napoly y = p->n;

  BOOLEAN norm=FALSE;

  if (y==NULL) return;

  if ((x!=NULL) && (y!=NULL))
  {
    int i;
    for (i=naNumbOfPar-1; i>=0; i--)
    {
      napoly xx=x;
      napoly yy=y;
      int m = ntpExpi(i, yy, xx);
      if (m != 0)          // in this case xx!=NULL!=yy
      {
        while (xx != NULL)
        {
          napAddExp(xx,i+1, -m);
          pIter(xx);
        }
        while (yy != NULL)
        {
          napAddExp(yy,i+1, -m);
          pIter(yy);
        }
      }
    }
  }
  if (p_LmIsConstant(y,ntcRing)) /* i.e. => simplify to (1/c)*z / monom */
  {
    if (ntcIsOne(pGetCoeff(y)))
    {
      p_LmDelete(&y,ntcRing);
      p->n = NULL;
      ntTest(pp);
      return;
    }
    number h1 = ntcInvers(pGetCoeff(y));
    ntcNormalize(h1);
    ntpMultN(x, h1);
    n_Delete(&h1,ntcRing);
    p_LmDelete(&y,ntcRing);
    p->n = NULL;
    ntTest(pp);
    return;
  }
#ifndef FACTORY_GCD_TEST
  if (naNumbOfPar == 1) /* apply built-in gcd */
  {
    napoly x1,y1;
    if (p_GetExp(x,1,ntcRing) >= p_GetExp(y,1,ntcRing))
    {
      x1 = ntpCopy(x);
      y1 = ntpCopy(y);
    }
    else
    {
      x1 = ntpCopy(y);
      y1 = ntpCopy(x);
    }
    napoly r;
    loop
    {
      r = napRemainder(x1, y1);
      if ((r==NULL) || (pNext(r)==NULL)) break;
      x1 = y1;
      y1 = r;
    }
    if (r!=NULL)
    {
      p_Delete(&r,ntcRing);
      p_Delete(&y1,ntcRing);
    }
    else
    {
      ntpDivMod(x, y1, &(p->z), &r);
      ntpDivMod(y, y1, &(p->n), &r);
      p_Delete(&y1,ntcRing);
    }
    x = p->z;
    y = p->n;
    /* collect all denoms from y and multiply x and y by it */
    if (ntIsChar0)
    {
      number n=ntpLcm(y);
      ntpMultN(x,n);
      ntpMultN(y,n);
      n_Delete(&n,ntcRing);
      while(x!=NULL)
      {
        ntcNormalize(pGetCoeff(x));
        pIter(x);
      }
      x = p->z;
      while(y!=NULL)
      {
        ntcNormalize(pGetCoeff(y));
        pIter(y);
      }
      y = p->n;
    }
    if (pNext(y)==NULL)
    {
      if (ntcIsOne(pGetCoeff(y)))
      {
        if (p_GetExp(y,1,ntcRing)==0)
        {
          p_LmDelete(&y,ntcRing);
          p->n = NULL;
        }
        ntTest(pp);
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
      p_Delete(&x,ntcRing);
      p_Delete(&y,ntcRing);
    }
  }
#endif
  /* remove common factors from z and n */
  x=p->z;
  y=p->n;
  if(!ntcGreaterZero(pGetCoeff(y)))
  {
    x=napNeg(x);
    y=napNeg(y);
  }
  number g=ntcCopy(pGetCoeff(x));
  pIter(x);
  while (x!=NULL)
  {
    number d=ntcGcd(g,pGetCoeff(x), ntcRing);
    if(ntcIsOne(d))
    {
      n_Delete(&g,ntcRing);
      n_Delete(&d,ntcRing);
      ntTest(pp);
      return;
    }
    n_Delete(&g,ntcRing);
    g = d;
    pIter(x);
  }
  while (y!=NULL)
  {
    number d=ntcGcd(g,pGetCoeff(y), ntcRing);
    if(ntcIsOne(d))
    {
      n_Delete(&g,ntcRing);
      n_Delete(&d,ntcRing);
      ntTest(pp);
      return;
    }
    n_Delete(&g,ntcRing);
    g = d;
    pIter(y);
  }
  x=p->z;
  y=p->n;
  while (x!=NULL)
  {
    number d = ntcIntDiv(pGetCoeff(x),g);
    ntpSetCoeff(x,d);
    pIter(x);
  }
  while (y!=NULL)
  {
    number d = ntcIntDiv(pGetCoeff(y),g);
    ntpSetCoeff(y,d);
    pIter(y);
  }
  n_Delete(&g,ntcRing);
  ntTest(pp);
}

/*2
* returns in result->n 1
* and in     result->z the lcm(a->z,b->n)
*/
number ntLcm(number la, number lb, const ring r)
{
  lnumber result;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  result = (lnumber)omAlloc0Bin(rnumber_bin);
  //if (((naMinimalPoly==NULL) && (ntI==NULL)) || !ntIsChar0)
  //{
  //  result->z = p_ISet(1,ntcRing);
  //  return (number)result;
  //}
  //ntNormalize(lb);
  ntTest(la);
  ntTest(lb);
  napoly x = p_Copy(a->z, r->algring);
  number t = ntpLcm(b->z); // get all denom of b->z
  if (!ntcIsOne(t))
  {
    number bt, rr;
    napoly xx=x;
    while (xx!=NULL)
    {
      bt = ntcGcd(t, pGetCoeff(xx), r->algring);
      rr = ntcMult(t, pGetCoeff(xx));
      n_Delete(&pGetCoeff(xx),r->algring);
      pGetCoeff(xx) = ntcDiv(rr, bt);
      ntcNormalize(pGetCoeff(xx));
      n_Delete(&bt,r->algring);
      n_Delete(&rr,r->algring);
      pIter(xx);
    }
  }
  n_Delete(&t,r->algring);
  result->z = x;
#ifdef HAVE_FACTORY
  if (b->n!=NULL)
  {
    result->z=singclap_alglcm(result->z,b->n);
    p_Delete(&x,r->algring);
  }
#endif
  ntTest(la);
  ntTest(lb);
  ntTest((number)result);
  return ((number)result);
}

/*2
* input: a set of constant polynomials
* sets the global variable ntI
*/
void ntSetIdeal(ideal I)
{
  int i;

  if (idIs0(I))
  {
    for (i=ntI->anz-1; i>=0; i--)
      p_Delete(&ntI->liste[i],ntcRing);
    omFreeBin((ADDRESS)ntI, sntIdeal_bin);
    ntI=NULL;
  }
  else
  {
    lnumber h;
    number a;
    napoly x;

    ntI=(ntIdeal)omAllocBin(sntIdeal_bin);
    ntI->anz=IDELEMS(I);
    ntI->liste=(napoly*)omAlloc(ntI->anz*sizeof(napoly));
    for (i=IDELEMS(I)-1; i>=0; i--)
    {
      h=(lnumber)pGetCoeff(I->m[i]);
      /* We only need the enumerator of h, as we expect it to be a polynomial */
      ntI->liste[i]=ntpCopy(h->z);
      /* If it isn't normalized (lc = 1) do this */
      if (!ntcIsOne(pGetCoeff(ntI->liste[i])))
      {
        x=ntI->liste[i];
        ntcNormalize(pGetCoeff(x));
        a=ntcCopy(pGetCoeff(x));
        number aa=ntcInvers(a);
        n_Delete(&a,ntcRing);
        ntpMultN(x,aa);
        n_Delete(&aa,ntcRing);
      }
    }
  }
}

/*2
* map Z/p -> Q(a)
*/
number ntMapP0(number c)
{
  if (npIsZero(c)) return NULL;
  lnumber l=(lnumber)omAllocBin(rnumber_bin);
  l->s=2;
  l->z=(napoly)p_Init(ntcRing);
  int i=(int)((long)c);
  if (i>((long)ntMapRing->ch>>2)) i-=(long)ntMapRing->ch;
  pGetCoeff(l->z)=nlInit(i, ntcRing);
  l->n=NULL;
  return (number)l;
}

/*2
* map Q -> Q(a)
*/
number ntMap00(number c)
{
  if (nlIsZero(c)) return NULL;
  lnumber l=(lnumber)omAllocBin(rnumber_bin);
  l->s=0;
  l->z=(napoly)p_Init(ntcRing);
  pGetCoeff(l->z)=nlCopy(c);
  l->n=NULL;
  return (number)l;
}

/*2
* map Z/p -> Z/p(a)
*/
number ntMapPP(number c)
{
  if (npIsZero(c)) return NULL;
  lnumber l=(lnumber)omAllocBin(rnumber_bin);
  l->s=2;
  l->z=(napoly)p_Init(ntcRing);
  pGetCoeff(l->z)=c; /* omit npCopy, because npCopy is a no-op */
  l->n=NULL;
  return (number)l;
}

/*2
* map Z/p' -> Z/p(a)
*/
number ntMapPP1(number c)
{
  if (npIsZero(c)) return NULL;
  int i=(int)((long)c);
  if (i>(long)ntMapRing->ch) i-=(long)ntMapRing->ch;
  number n=npInit(i,ntMapRing);
  if (npIsZero(n)) return NULL;
  lnumber l=(lnumber)omAllocBin(rnumber_bin);
  l->s=2;
  l->z=(napoly)p_Init(ntcRing);
  pGetCoeff(l->z)=n;
  l->n=NULL;
  return (number)l;
}

/*2
* map Q -> Z/p(a)
*/
number ntMap0P(number c)
{
  if (nlIsZero(c)) return NULL;
  number n=npInit(nlModP(c,npPrimeM),ntcRing);
  if (npIsZero(n)) return NULL;
  npTest(n);
  lnumber l=(lnumber)omAllocBin(rnumber_bin);
  l->s=2;
  l->z=(napoly)p_Init(ntcRing);
  pGetCoeff(l->z)=n;
  l->n=NULL;
  return (number)l;
}

static number (*ntcMap)(number);
static int ntParsToCopy;
static napoly ntpMap(napoly p)
{
  napoly w, a;

  if (p==NULL) return NULL;
  a = w = (napoly)p_Init(ntcRing);
  int i;
  for(i=1;i<=ntParsToCopy;i++)
    napSetExp(a,i,napGetExpFrom(p,i,ntMapRing));
  p_Setm(a,ntcRing);
  pGetCoeff(w) = ntcMap(pGetCoeff(p));
  loop
  {
    pIter(p);
    if (p==NULL) break;
    pNext(a) = (napoly)p_Init(ntcRing);
    pIter(a);
    for(i=1;i<=ntParsToCopy;i++)
      napSetExp(a,i,napGetExpFrom(p,i,ntMapRing));
    p_Setm(a,ntcRing);
    pGetCoeff(a) = ntcMap(pGetCoeff(p));
  }
  pNext(a) = NULL;
  return w;
}

static napoly ntpPerm(napoly p,const int *par_perm,const ring src_ring,const nMapFunc nMap)
{
  napoly w, a;

  if (p==NULL) return NULL;
  w = (napoly)p_Init(ntcRing);
  int i;
  BOOLEAN not_null=TRUE;
  loop
  {
    for(i=1;i<=rPar(src_ring);i++)
    {
      int e;
      if (par_perm!=NULL) e=par_perm[i-1];
      else                e=-i;
      int ee=napGetExpFrom(p,i,src_ring);
      if (e<0)
        napSetExp(w,-e,ee);
      else if (ee>0)
        not_null=FALSE;
    }
    pGetCoeff(w) = nMap(pGetCoeff(p));
    p_Setm(w,ntcRing);
    pIter(p);
    if (!not_null)
    {
      if (p==NULL)
      {
        p_Delete(&w,ntcRing);
        return NULL;
      }
      /* else continue*/
      n_Delete(&(pGetCoeff(w)),ntcRing);
    }
    else
    {
      if (p==NULL) return w;
      else
      {
        pNext(w)=ntpPerm(p,par_perm,src_ring,nMap);
        return w;
      }
    }
  }
}

/*2
* map _(a) -> _(b)
*/
number ntMapQaQb(number c)
{
  if (c==NULL) return NULL;
  lnumber erg= (lnumber)omAlloc0Bin(rnumber_bin);
  lnumber src =(lnumber)c;
  erg->s=src->s;
  erg->z=ntpMap(src->z);
  erg->n=ntpMap(src->n);
  return (number)erg;
}

nMapFunc ntSetMap(const ring src, const ring dst)
{
  ntMapRing=src;
  if (rField_is_Q_a(dst)) /* -> Q(a) */
  {
    if (rField_is_Q(src))
    {
      return ntMap00;   /*Q -> Q(a)*/
    }
    if (rField_is_Zp(src))
    {
      return ntMapP0;  /* Z/p -> Q(a)*/
    }
    if (rField_is_Q_a(src))
    {
      int i;
      ntParsToCopy=0;
      for(i=0;i<rPar(src);i++)
      {
        if ((i>=rPar(dst))
        ||(strcmp(src->parameter[i],dst->parameter[i])!=0))
           return NULL;
        ntParsToCopy++;
      }
      ntcMap=ntcCopy;
      if ((ntParsToCopy==rPar(dst))&&(ntParsToCopy==rPar(src)))
        return ntCopy;    /* Q(a) -> Q(a) */
      return ntMapQaQb;   /* Q(a..) -> Q(a..) */
    }
  }
  /*-----------------------------------------------------*/
  if (rField_is_Zp_a(dst)) /* -> Z/p(a) */
  {
    if (rField_is_Q(src))
    {
      return ntMap0P;   /*Q -> Z/p(a)*/
    }
    if (rField_is_Zp(src))
    {
      if (src->ch==dst->ch)
      {
        return ntMapPP;  /* Z/p -> Z/p(a)*/
      }
      else
      {
        return ntMapPP1;  /* Z/p' -> Z/p(a)*/
      }
    }
    if (rField_is_Zp_a(src))
    {
      if (rChar(src)==rChar(dst))
      {
        ntcMap=ntcCopy;
      }
      else
      {
        ntcMap = npMapP;
      }
      int i;
      ntParsToCopy=0;
      for(i=0;i<rPar(src);i++)
      {
        if ((i>=rPar(dst))
        ||(strcmp(src->parameter[i],dst->parameter[i])!=0))
           return NULL;
        ntParsToCopy++;
      }
      if ((ntParsToCopy==rPar(dst))&&(ntParsToCopy==rPar(src))
      && (ntcMap==ntcCopy))
        return ntCopy;    /* Z/p(a) -> Z/p(a) */
      return ntMapQaQb;   /* Z/p(a),Z/p'(a) -> Z/p(b)*/
    }
  }
  return NULL;      /* default */
}

/*2
* convert a napoly number into a poly
*/
poly ntPermNumber(number z, int * par_perm, int P, ring oldRing)
{
  if (z==NULL) return NULL;
  poly res=NULL;
  poly p;
  napoly za=((lnumber)z)->z;
  napoly zb=((lnumber)z)->n;
  nMapFunc nMap=ntSetMap(oldRing,currRing);
  if (currRing->parameter!=NULL)
    nMap=currRing->algring->cf->cfSetMap(oldRing->algring, ntcRing);
  else
    nMap=currRing->cf->cfSetMap(oldRing->algring, currRing);
  if (nMap==NULL) return NULL; /* emergency exit only */
  do
  {
    p = pInit();
    pNext(p)=NULL;
    nNew(&pGetCoeff(p));
    int i;
    for(i=pVariables;i;i--)
       pSetExp(p,i, 0);
    if (rRing_has_Comp(currRing)) pSetComp(p, 0);
    napoly pa=NULL;
    lnumber pan;
    if (currRing->parameter!=NULL)
    {
      assume(oldRing->algring!=NULL);
      pGetCoeff(p)=(number)omAlloc0Bin(rnumber_bin);
      pan=(lnumber)pGetCoeff(p);
      pan->s=2;
      pan->z=ntpInitz(nMap(pGetCoeff(za)));
      pa=pan->z;
    }
    else
    {
      pGetCoeff(p)=nMap(pGetCoeff(za));
    }
    for(i=0;i<P;i++)
    {
      if(napGetExpFrom(za,i+1,oldRing)!=0)
      {
        if(par_perm==NULL)
        {
          if ((rPar(currRing)>=i) && (pa!=NULL))
          {
            napSetExp(pa,i+1,napGetExpFrom(za,i+1,oldRing));
            p_Setm(pa,ntcRing);
          }
          else
          {
            pDelete(&p);
            break;
          }
        }
        else if(par_perm[i]>0)
          pSetExp(p,par_perm[i],napGetExpFrom(za,i+1,oldRing));
        else if((par_perm[i]<0)&&(pa!=NULL))
        {
          napSetExp(pa,-par_perm[i], napGetExpFrom(za,i+1,oldRing));
          p_Setm(pa,ntcRing);
        }
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
      if (zb!=NULL)
      {
        if  (currRing->P>0)
        {
          pan->n=ntpPerm(zb,par_perm,oldRing,nMap);
          if(pan->n==NULL) /* error in mapping or mapping to variable */
            pDelete(&p);
        }
        else
          pDelete(&p);
      }
      pTest(p);
      res=pAdd(res,p);
    }
    pIter(za);
  }
  while (za!=NULL);
  pTest(res);
  return res;
}

number   ntGetDenom(number &n, const ring r)
{
  lnumber x=(lnumber)n;
  if (x->n!=NULL)
  {
    lnumber rr=(lnumber)omAlloc0Bin(rnumber_bin);
    rr->z=p_Copy(x->n,r->algring);
    rr->s = 2;
    return (number)rr;
  }
  return n_Init(1,r);
}

number   ntGetNumerator(number &n, const ring r)
{
  lnumber x=(lnumber)n;
  lnumber rr=(lnumber)omAlloc0Bin(rnumber_bin);
  rr->z=p_Copy(x->z,r->algring);
  rr->s = 2;
  return (number)rr;
}

#ifdef LDEBUG
BOOLEAN ntDBTest(number a, const char *f,const int l)
{
  lnumber x=(lnumber)a;
  if (x == NULL)
    return TRUE;
  #ifdef LDEBUG
  omCheckAddrSize(a, sizeof(snumber));
  #endif
  napoly p = x->z;
  if (p==NULL)
  {
    Print("0/* in %s:%d\n",f,l);
    return FALSE;
  }
  while(p!=NULL)
  {
    if ((ntIsChar0 && nlIsZero(pGetCoeff(p)))
    || ((!ntIsChar0) && npIsZero(pGetCoeff(p))))
    {
      Print("coeff 0 in %s:%d\n",f,l);
      return FALSE;
    }
    //if (ntIsChar0 && (((int)p->ko &3) == 0) && (p->ko->s==0) && (x->s==2))
    //{
    //  Print("normalized with non-normal coeffs in %s:%d\n",f,l);
    //  return FALSE;
    //}
    if (ntIsChar0 && !(nlDBTest(pGetCoeff(p),f,l)))
      return FALSE;
    pIter(p);
  }
  p = x->n;
  while(p!=NULL)
  {
    if (ntIsChar0 && !(nlDBTest(pGetCoeff(p),f,l)))
      return FALSE;
    pIter(p);
  }
  return TRUE;
}
#endif
