/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longtrans.cc 12469 2011-02-25 13:38:49Z seelisch $ */
/*
* ABSTRACT:   numbers in transcendental field extensions, i.e.,
              in rational function fields
*/

#if 0
#include <stdio.h>
#include <string.h>
#include <omalloc/omalloc.h>
#include <resources/feFopen.h>
#include <coeffs/longrat.h>
#include <coeffs/modulop.h>
#include <coeffs/numbers.h>
#include <polys/polys.h>
#include <polys/simpleideals.h>
#include <polys/monomials/ring.h>
#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include <factory/factory.h>
//#include <kernel/clapsing.h>
//#include <kernel/clapconv.h>
#endif
#include <polys/ext_fields/longtrans.h>
#include <polys/ext_fields/longalg.h>

ring nacRing;
int  ntIsChar0;
ring ntMapRing;
int  ntParsToCopy;
int  ntNumbOfPar;

numberfunc  nacMult, nacSub, nacAdd, nacDiv, nacIntDiv;
number      (*ntMap)(number from);
number      (*nacGcd)(number a, number b, const ring r);
number      (*nacLcm)(number a, number b, const ring r);
number      (*nacInit)(int i, const ring r);
int         (*nacInt)(number &n, const ring r);
void        (*nacNormalize)(number &a);
number      (*nacNeg)(number a);
number      (*nacCopy)(number a);
number      (*nacInvers)(number a);
BOOLEAN     (*nacIsZero)(number a);
BOOLEAN     (*nacIsOne)(number a);
BOOLEAN     (*nacGreaterZero)(number a);
BOOLEAN     (*nacGreater)(number a, number b);
number      (*nacMap)(number);

#ifdef LDEBUG
#define ntTest(a) ntDBTest(a,__FILE__,__LINE__)
BOOLEAN ntDBTest(number a, const char *f,const int l);
#else
#define ntTest(a)
#endif

//static number ntdGcd( number a, number b, const ring r) { return nacInit(1,r); }
/*2
*  sets the appropriate operators
*/
void ntSetChar(int i, ring r)
{
  assume((r->minpoly  == NULL) &&
         (r->minideal == NULL)    );
  
  if (naI!=NULL)
  {
    int j;
    for (j=naI->anz-1; j>=0; j--)
       p_Delete (&naI->liste[j],nacRing);
    omFreeSize((ADDRESS)naI->liste,naI->anz*sizeof(napoly));
    omFreeBin((ADDRESS)naI, snaIdeal_bin);
    naI=NULL;
  }
  ntMap = ntCopy;
  naMinimalPoly = NULL;

  ntNumbOfPar=rPar(r);
  if (i == 1)
    ntIsChar0 = 1;
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
  nacRing        = r->algring;
  nacInit        = nacRing->cf->cfInit;
  nacInt         = nacRing->cf->n_Int;
  nacCopy        = nacRing->cf->nCopy;
  nacAdd         = nacRing->cf->nAdd;
  nacSub         = nacRing->cf->nSub;
  nacNormalize   = nacRing->cf->nNormalize;
  nacNeg         = nacRing->cf->nNeg;
  nacIsZero      = nacRing->cf->nIsZero;
  nacGreaterZero = nacRing->cf->nGreaterZero;
  nacGreater     = nacRing->cf->nGreater;
  nacIsOne       = nacRing->cf->nIsOne;
  nacGcd         = nacRing->cf->nGcd;
  nacLcm         = nacRing->cf->nLcm;
  nacMult        = nacRing->cf->nMult;
  nacDiv         = nacRing->cf->nDiv;
  nacIntDiv      = nacRing->cf->nIntDiv;
  nacInvers      = nacRing->cf->nInvers;
}

/*============= procedure for polynomials: napXXXX =======================*/

#ifdef LDEBUG
void napTest(napoly p)
{
  if (ntIsChar0)
  {
    while (p != NULL)
    {
      nlDBTest(pGetCoeff(p), "", 0);
      pIter(p);
    }
  }
}
#else
#define napTest(p) ((void) 0)
#endif

/* creates a new napoly that consists of a
   single coefficient (provided as a number);
   the provided number is NOT const */
napoly napInitz(number z)
{
  napoly a = (napoly)p_Init(nacRing);
  pGetCoeff(a) = z;
  return a;
}

/* creates a new napoly which is the
   negative inverse of the argument;
   keeps p */
napoly napCopyNeg(const napoly p)
{
  napoly r = napCopy(p);
  r = (napoly)p_Neg((poly)r, nacRing);
  return r;
}

/* modifies the napoly p to p*z, i.e.
   in-place multiplication of p with the number z;
   keeps z */
void napMultN(napoly p, const number z)
{
  number t;
  while (p != NULL)
  {
    t = nacMult(pGetCoeff(p), z);
    nacNormalize(t);
    n_Delete(&pGetCoeff(p),nacRing);
    pGetCoeff(p) = t;
    pIter(p);
  }
}

/* division of f by g with remainder
   (with respect to the first variable),
   f = g * q + r,
   assumes that the exponent of the first variable
   in f is greater than or equal to that in g
   sets q, r; destroys f; keeps g */
void napDivMod(napoly f, const napoly  g, napoly *q, napoly *r)
{
  napoly a, h, b, qq;

  qq = (napoly)p_Init(nacRing);
  pNext(qq) = b = NULL;
  p_Normalize(g, nacRing);
  p_Normalize(f, nacRing);
  a = f;
  assume(p_GetExp(a, 1, nacRing) >= p_GetExp(g, 1, nacRing));
  do
  {
    napSetExp(qq, 1, p_GetExp(a, 1, nacRing) - p_GetExp(g, 1, nacRing));
    p_Setm(qq, nacRing);
    pGetCoeff(qq) = nacDiv(pGetCoeff(a), pGetCoeff(g));
    nacNormalize(pGetCoeff(qq));
    b = napAdd(b, napCopy(qq));
    pGetCoeff(qq) = nacNeg(pGetCoeff(qq));
    h = napCopy(g);
    napMultT(h, qq);
    p_Normalize(h, nacRing);
    n_Delete(&pGetCoeff(qq), nacRing);
    a = napAdd(a, h);
  }
  while ((a != NULL) &&
         (p_GetExp(a, 1, nacRing) >= p_GetExp(g, 1, nacRing)));
  omFreeBinAddr(qq);
  *q = b;
  *r = a;
}

/* remainder of division of f by g
   (with respect to the first variable),
   f = g * q + r,
   assumes that the exponent of the first variable
   in f is greater than or equal to that in g
   destroys f; keeps g; returns r */
napoly napRemainder(napoly f, const napoly g)
{
  napoly a, h, qq;

  qq = (napoly)p_Init(nacRing);
  pNext(qq) = NULL;
  p_Normalize(g, nacRing);
  p_Normalize(f, nacRing);
  a = f;
  assume(p_GetExp(a, 1, nacRing) >= p_GetExp(g, 1, nacRing));
  do
  {
    napSetExp(qq, 1, p_GetExp(a, 1, nacRing) - p_GetExp(g, 1, nacRing));
    napSetm(qq);
    pGetCoeff(qq) = nacDiv(pGetCoeff(a), pGetCoeff(g));
    pGetCoeff(qq) = nacNeg(pGetCoeff(qq));
    nacNormalize(pGetCoeff(qq));
    h = napCopy(g);
    napMultT(h, qq);
    p_Normalize(h, nacRing);
    n_Delete(&pGetCoeff(qq), nacRing);
    a = napAdd(a, h);
  }
  while ((a != NULL) &&
         (p_GetExp(a,1,nacRing) >= p_GetExp(g,1,nacRing)));
  omFreeBinAddr(qq);
  return a;
}

/* returns z such that z * x mod c = 1;
   if there is no solution, an error is reported and
   some intermediate version of x is returned;
   modifies x; keeps c */
napoly napInvers(napoly x, const napoly c)
{
  napoly y, r, qa, qn, q;
  number t;

  if (p_GetExp(x, 1, nacRing) >= p_GetExp(c, 1, nacRing))
    x = napRemainder(x, c);
  if (x == NULL)
  {
    WerrorS("zero divisor found - your minpoly is not irreducible");
    return NULL;
  }
  if (p_GetExp(x, 1, nacRing) == 0)
  {
    if (!nacIsOne(pGetCoeff(x)))
    {
      nacNormalize(pGetCoeff(x));
      t = nacInvers(pGetCoeff(x));
      nacNormalize(t);
      n_Delete(&pGetCoeff(x), nacRing);
      pGetCoeff(x) = t;
    }
    return x;
  }
  y = napCopy(c);
  napDivMod(y, x, &qa, &r);
  if (r == NULL)
  {
    WerrorS("x is not invertible modulo c(1)");
    return x;
  }
  if (p_GetExp(r, 1, nacRing) == 0)
  {
    nacNormalize(pGetCoeff(r));
    t = nacInvers(pGetCoeff(r));
    nacNormalize(t);
    t = nacNeg(t);
    napMultN(qa, t);
    n_Delete(&t, nacRing);
    p_Normalize(qa, nacRing);
    p_Delete(&x, nacRing);
    p_Delete(&r, nacRing);
    return qa;
  }
  y = x;
  x = r;
  napDivMod(y, x, &q, &r);
  if (r == NULL)
  {
    WerrorS("x is not invertible modulo c(2)");
    return x;
  }
  if (p_GetExp(r, 1, nacRing) == 0)
  {
    q = p_Mult_q(q, qa,nacRing);
    q = napAdd(q, p_ISet(1, nacRing));
    nacNormalize(pGetCoeff(r));
    t = nacInvers(pGetCoeff(r));
    napMultN(q, t);
    p_Normalize(q, nacRing);
    n_Delete(&t, nacRing);
    p_Delete(&x, nacRing);
    p_Delete(&r, nacRing);
    if (p_GetExp(q, 1, nacRing) >= p_GetExp(c, 1, nacRing))
      q = napRemainder(q, c);
    return q;
  }
  q = p_Mult_q(q, napCopy(qa), nacRing);
  q = napAdd(q, p_ISet(1, nacRing));
  qa = napNeg(qa);
  loop
  {
    y = x;
    x = r;
    napDivMod(y, x, &qn, &r);
    if (r == NULL)
    {
      WerrorS("zero divisor found - your minpoly is not irreducible");
      return x;
    }
    if (p_GetExp(r, 1, nacRing) == 0)
    {
      q = p_Mult_q(q, qn, nacRing);
      q = napNeg(q);
      q = napAdd(q, qa);
      nacNormalize(pGetCoeff(r));
      t = nacInvers(pGetCoeff(r));
      //nacNormalize(t);
      napMultN(q, t);
      p_Normalize(q, nacRing);
      n_Delete(&t, nacRing);
      p_Delete(&x, nacRing);
      p_Delete(&r, nacRing);
      if (p_GetExp(q, 1, nacRing) >= p_GetExp(c, 1, nacRing))
        q = napRemainder(q, c);
      return q;
    }
    y = q;
    q = p_Mult_q(napCopy(q), qn, nacRing);
    q = napNeg(q);
    q = napAdd(q, qa);
    qa = y;
  }
}

/* the degree of a napoly, i.e. the
   maximum of all terms' degrees;
   keeps p */
int napMaxDeg(napoly p)
{
  int  d = 0;
  while (p != NULL)
  {
    d=si_max(d, napDeg(p));
    pIter(p);
  }
  return d;
}

/* the degree of a napoly, i.e. the
   maximum of all terms' degrees;
   fills l with the number of terms;
   keeps p */
int napMaxDegLen(napoly p, int &l)
{
  int d = 0;
  l = 0;
  while (p != NULL)
  {
    d = si_max(d, napDeg(p));
    pIter(p);
    l++;
  }
  return d;
}


/* writes a napoly, i.e. a number in the ground field;
   if has_denom is TRUE, the output is ready to be
   followed by a non-trivial denominator;
   r is assumed to be a polynomial ring over an algebraic
   or transcendental field extension;
   keeps all arguments */
void napWrite(napoly p, const BOOLEAN has_denom, const ring r)
{
  ring nacring = r->algring;
  if (p == NULL) StringAppendS("0");
  else if (p_LmIsConstant(p, nacring))
  {
    BOOLEAN kl = FALSE;
    if (has_denom)
    {
      number den = nacring->cf->cfGetDenom(pGetCoeff(p), nacring);
      kl = !n_IsOne(den, nacring);
      n_Delete(&den, nacring);
    }
    if (kl) StringAppendS("(");
    n_Write(pGetCoeff(p), nacring);
    if (kl) StringAppendS(")");
  }
  else
  {
    StringAppendS("(");
    loop
    {
      BOOLEAN wroteCoeff = FALSE;
      if ((p_LmIsConstant(p, nacring)) ||
           ((!n_IsOne(pGetCoeff(p), nacring)) &&
            (!n_IsMOne(pGetCoeff(p),nacring))))
      {
        n_Write(pGetCoeff(p), nacring);
        wroteCoeff = (r->ShortOut == 0);
      }
      else if (n_IsMOne(pGetCoeff(p), nacring)) StringAppendS("-");
      for (int i = 0; i < r->P; i++)
      {
        int e = p_GetExp(p, i+1, nacring);
        if (e > 0)
        {
          if (wroteCoeff) StringAppendS("*");
          else            wroteCoeff=(r->ShortOut==0);
          StringAppendS(r->parameter[i]);
          if (e > 1)
          {
            if (r->ShortOut == 0) StringAppendS("^");
            StringAppend("%d", e);
          }
        }
      }
      pIter(p);
      if (p == NULL) break;
      if (n_GreaterZero(pGetCoeff(p),nacring)) StringAppendS("+");
    }
    StringAppendS(")");
  }
}

/* helper for napRead */
const char* napHandleMons(const char* s, int i, napoly ex)
{
  int j;
  if (strncmp(s, ntParNames[i], strlen(ntParNames[i])) == 0)
  {
    s += strlen(ntParNames[i]);
    if ((*s >= '0') && (*s <= '9'))
    {
      s = eati(s, &j);
      napAddExp(ex, i+1, j);
    }
    else
      napAddExp(ex, i+1, 1);
  }
  return s;
}

/* helper for napRead */
const char* napHandlePars(const char *s, int i, napoly ex)
{
  if (strcmp(s, ntParNames[i]) == 0)
  {
    s += strlen(ntParNames[i]);
    napSetExp(ex, i+1, 1);
  }
  return s;
}

/* reads a monomial into the napoly b;
   returns the latter portion of s which
   comes "after" the monomial that has
   just been read;
   modifies b */
const char* napRead(const char *s, napoly *b)
{
  napoly a;
  int i;
  a = (napoly)p_Init(nacRing);
  if ((*s >= '0') && (*s <= '9'))
  {
    s = nacRing->cf->nRead(s, &pGetCoeff(a));
    if (nacIsZero(pGetCoeff(a)))
    {
      p_LmDelete(&a, nacRing);
      *b = NULL;
      return s;
    }
  }
  else pGetCoeff(a) = nacInit(1,nacRing);
  i = 0;
  const char* olds = s;
  loop
  {
    s = napHandlePars(s, i, a);
    if (olds == s) i++;
    else if (*s == '\0')
    {
      *b = a;
      return s;
    }
    if (i >= ntNumbOfPar) break;
  }
  i = 0;
  loop
  {
    olds = s;
    s = napHandleMons(s, i, a);
    if (olds == s) i++;
    else i = 0;
    if ((*s == '\0') || (i >= ntNumbOfPar)) break;
  }
  *b = a;
  return s;
}

/* considers the lowest terms la of a and lb of b;
   returns the minimum of the two exponents of the
   first variable in la and lb;
   assumes a != NULL, b != NULL;
   keeps a and b */
int napExp(napoly a, napoly b)
{
  assume(a != NULL);
  assume(b != NULL);
  while (pNext(a) != NULL) pIter(a);
  int m = p_GetExp(a, 1, nacRing);
  if (m == 0) return 0;
  while (pNext(b) != NULL) pIter(b);
  int mm = p_GetExp(b, 1, nacRing);
  if (m > mm) m = mm;
  return m;
}

/* returns the smallest i-th exponent in a and b;
   used to find it in a fraction;
   keeps a and b */
int napExpi(int i, napoly a, napoly b)
{
  if (a == NULL || b == NULL) return 0;
  int m = p_GetExp(a, i+1, nacRing);
  if (m == 0) return 0;
  while (pNext(a) != NULL)
  {
    pIter(a);
    if (m > p_GetExp(a, i+1, nacRing))
    {
      m = p_GetExp(a, i+1, nacRing);
      if (m == 0) return 0;
    }
  }
  do
  {
    if (m > p_GetExp(b, i+1, nacRing))
    {
      m = p_GetExp(b, i+1, nacRing);
      if (m == 0) return 0;
    }
    pIter(b);
  }
  while (b != NULL);
  return m;
}

/* divides out the content of the given napoly;
   assumes that ph != NULL;
   modifies ph */
void napContent(napoly ph)
{
  number h, d;
  napoly p;

  assume(ph != NULL);
  p = ph;
  if (nacIsOne(pGetCoeff(p))) return;
  h = nacCopy(pGetCoeff(p));
  pIter(p);
  while (p != NULL)
  {
    d = nacGcd(pGetCoeff(p), h, nacRing);
    if (nacIsOne(d))
    {
      n_Delete(&h,nacRing);
      n_Delete(&d,nacRing);
      return;
    }
    n_Delete(&h, nacRing);
    h = d;
    pIter(p);
  }
  h = nacInvers(d);
  n_Delete(&d, nacRing);
  p = ph;
  while (p != NULL)
  {
    d = nacMult(pGetCoeff(p), h);
    n_Delete(&pGetCoeff(p), nacRing);
    pGetCoeff(p) = d;
    pIter(p);
  }
  n_Delete(&h, nacRing);
}

/* removes denominators of coefficients in ph
   by multiplication with lcm of those;
   if char != 0, then nothing is done;
   modifies ph */
void napCleardenom(napoly ph)
{
  number d, h;
  napoly p;

  if (!ntIsChar0) return;
  p = ph;
  h = nacInit(1,nacRing);
  while (p!=NULL)
  {
    d = nacLcm(h, pGetCoeff(p), nacRing); // uses denominator of pGetCoeff(p)
    n_Delete(&h,nacRing);
    h = d;
    pIter(p);
  }
  if(!nacIsOne(h))
  {
    p = ph;
    while (p!=NULL)
    {
      d=nacMult(h, pGetCoeff(p));
      n_Delete(&pGetCoeff(p),nacRing);
      nacNormalize(d);
      pGetCoeff(p) = d;
      pIter(p);
    }
  }
  n_Delete(&h,nacRing);
  napContent(ph);
}

/* returns the gcd of all coefficients in a and b;
   assumes a != NULL, b != NULL;
   keeps a, keeps b */
napoly napGcd0(napoly a, napoly b)
{
  number x, y;
  assume(a != NULL);
  assume(b != NULL);
  if (!ntIsChar0) return p_ISet(1, nacRing);
  x = nacCopy(pGetCoeff(a));
  if (nacIsOne(x)) return napInitz(x);
  pIter(a);
  while (a!=NULL)
  {
    y = nacGcd(x, pGetCoeff(a), nacRing);
    n_Delete(&x,nacRing);
    x = y;
    if (nacIsOne(x)) return napInitz(x);
    pIter(a);
  }
  do
  {
    y = nacGcd(x, pGetCoeff(b), nacRing);
    n_Delete(&x,nacRing);
    x = y;
    if (nacIsOne(x)) return napInitz(x);
    pIter(b);
  }
  while (b!=NULL);
  return napInitz(x);
}

/* returns the gcd of a and b;
   if char != 0, then the constant poly 1 is returned;
   if a = b = 0, then the constant poly 1 is returned;
   if a = 0 != b, then b is returned;
   if a != 0 = b, then a is returned;
   keeps a, keeps b */
napoly napGcd(napoly a, napoly b)
{
  int i;
  napoly g, x, y, h;
  if (a == NULL)
  {
    if (b == NULL)    return p_ISet(1,nacRing);
    else              return napCopy(b);
  }
  else if (b == NULL) return napCopy(a);
  
  if (naMinimalPoly != NULL)
  { // we have an algebraic extension
    if (p_GetExp(a,1,nacRing) >= p_GetExp(b,1,nacRing))
    {
      x = a;
      y = b;
    }
    else
    {
      x = b;
      y = a;
    }
    if (!ntIsChar0) g = p_ISet(1,nacRing);
    else            g = napGcd0(x, y);
    if (pNext(y)==NULL)
    {
      napSetExp(g,1, napExp(x, y));
      p_Setm(g,nacRing);
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
        if (!nacIsOne(pGetCoeff(g)))
          napMultN(y, pGetCoeff(g));
        p_LmDelete(&g,nacRing);
        return y;
      }
      else if (pNext(h)==NULL)
        break;
      x = y;
      y = h;
    }
    p_Delete(&y,nacRing);
    p_LmDelete(&h,nacRing);
    napSetExp(g,1, napExp(a, b));
    p_Setm(g,nacRing);
    return g;
  }
  else
  { // we have ntNumbOfPar transcendental variables 
    if (!ntIsChar0) x = p_ISet(1,nacRing);
    else            x = napGcd0(a,b);
    for (i=(ntNumbOfPar-1); i>=0; i--)
    {
      napSetExp(x,i+1, napExpi(i,a,b));
      p_Setm(x,nacRing);
    }
    return x;
  }
}

/* returns the lcm of all denominators in the coefficients of a;
   if char != 0, then the constant poly 1 is returned;
   if a = 0, then the constant poly 1 is returned;
   keeps a */
number napLcm(napoly a)
{
  number h = nacInit(1,nacRing);
  if (ntIsChar0)
  {
    number d;
    napoly b = a;
    while (b!=NULL)
    {
      d = nacLcm(h, pGetCoeff(b), nacRing); // uses denominator of pGetCoeff(b)
      n_Delete(&h,nacRing);
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
BOOLEAN napDivPoly (napoly p, napoly q)
{
  int j=1;   /* evtl. von naNumber.. -1 abwaerts zaehlen */

  while (p_GetExp(p,j,nacRing) <= p_GetExp(q,j,nacRing))
  {
    j++;
    if (j > ntNumbOfPar)
      return 1;
  }
  return 0;
}


/*
 * only used for reduction in algebraic extensions when naI != NULL;
 * reduces the tail of poly q which is required to be != NULL;
 * modifies q and returns it
 */
napoly napRedp (napoly q)
{
  napoly h = (napoly)p_Init(nacRing);
  int i=0,j;

  loop
  {
    if (napDivPoly (naI->liste[i], q))
    {
      /* h = lt(q)/lt(naI->liste[i])*/
      pGetCoeff(h) = nacCopy(pGetCoeff(q));
      for (j=ntNumbOfPar; j>0; j--)
        napSetExp(h,j, p_GetExp(q,j,nacRing) - p_GetExp(naI->liste[i],
                                                        j,nacRing));
      p_Setm(h,nacRing);
      h = p_Mult_q(h, napCopy(naI->liste[i]),nacRing);
      h = napNeg (h);
      q = napAdd (q, napCopy(h));
      p_Delete (&pNext(h),nacRing);
      if (q == NULL)
      {
        p_Delete(&h,nacRing);
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
        p_Delete(&h,nacRing);
        return q;
      }
    }
  }
}


/*
 * only used for reduction in algebraic extensions when naI != NULL;
 * reduces the tail of poly q which is required to be != NULL;
 * modifies q and returns it
 */
napoly napTailred (napoly q)
{
  napoly h;

  h = pNext(q);
  while (h != NULL)
  {
     h = napRedp (h);
     if (h == NULL)
        return q;
     pIter(h);
  }
  return q;
}

napoly napMap(napoly p)
{
  napoly w, a;

  if (p==NULL) return NULL;
  a = w = (napoly)p_Init(nacRing);
  int i;
  for(i=1;i<=ntParsToCopy;i++)
    napSetExp(a,i,napGetExpFrom(p,i,ntMapRing));
  p_Setm(a,nacRing);
  pGetCoeff(w) = nacMap(pGetCoeff(p));
  loop
  {
    pIter(p);
    if (p==NULL) break;
    pNext(a) = (napoly)p_Init(nacRing);
    pIter(a);
    for(i=1;i<=ntParsToCopy;i++)
      napSetExp(a,i,napGetExpFrom(p,i,ntMapRing));
    p_Setm(a,nacRing);
    pGetCoeff(a) = nacMap(pGetCoeff(p));
  }
  pNext(a) = NULL;
  return w;
}

napoly napPerm(napoly p,const int *par_perm,const ring src_ring,const nMapFunc nMap)
{
  napoly w;

  if (p==NULL) return NULL;
  w = (napoly)p_Init(nacRing);
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
    p_Setm(w,nacRing);
    pIter(p);
    if (!not_null)
    {
      if (p==NULL)
      {
        p_Delete(&w,nacRing);
        return NULL;
      }
      /* else continue*/
      n_Delete(&(pGetCoeff(w)),nacRing);
    }
    else
    {
      if (p==NULL) return w;
      else
      {
        pNext(w)=napPerm(p,par_perm,src_ring,nMap);
        return w;
      }
    }
  }
}

/*2
* convert a napoly number into a poly
*/
poly napPermNumber(number z, int * par_perm, int P, ring oldRing)
{
  if (z==NULL) return NULL;
  poly res=NULL;
  poly p;
  napoly za=((lnumber)z)->z;
  napoly zb=((lnumber)z)->n;
  nMapFunc nMap=naSetMap(oldRing,currRing); /* todo: check naSetMap
                                                     vs. ntSetMap */
  if (currRing->parameter!=NULL)
    nMap=currRing->algring->cf->cfSetMap(oldRing->algring, nacRing);
  else
    nMap=currRing->cf->cfSetMap(oldRing->algring, currRing);
  if (nMap==NULL) return NULL; /* emergency exit only */
  while(za!=NULL)
  {
    p = pInit();
    pNext(p)=NULL;
    //nNew(&pGetCoeff(p));
    int i;
    //for(i=pVariables;i;i--) pSetExp(p,i, 0); // done by pInit
    //if (rRing_has_Comp(currRing)) pSetComp(p, 0); // done by pInit
    napoly pa=NULL;
    lnumber pan;
    if (currRing->parameter!=NULL)
    {
      assume(oldRing->algring!=NULL);
      pGetCoeff(p)=(number)ALLOC0_LNUMBER();
      pan=(lnumber)pGetCoeff(p);
      pan->s=2;
      pan->z=napInitz(nMap(pGetCoeff(za)));
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
            p_Setm(pa,nacRing);
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
          p_Setm(pa,nacRing);
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
          pan->n=napPerm(zb,par_perm,oldRing,nMap);
          if(pan->n==NULL) /* error in mapping or mapping to variable */
            pDelete(&p);
        }
        else
          pDelete(&p);
      }
      nNormalize(pGetCoeff(p));
      if (nIsZero(pGetCoeff(p)))
        pDelete(&p);
      else
      {
        pTest(p);
        res=pAdd(res,p);
      }
    }
    pIter(za);
  }
  pTest(res);
  return res;
}

number   napGetDenom(number &n, const ring r)
{
  lnumber x=(lnumber)n;
  if (x->n!=NULL)
  {
    lnumber rr=ALLOC0_LNUMBER();
    rr->z=p_Copy(x->n,r->algring);
    rr->s = 2;
    return (number)rr;
  }
  return n_Init(1,r);
}

number   napGetNumerator(number &n, const ring r)
{
  lnumber x=(lnumber)n;
  lnumber rr=ALLOC0_LNUMBER();
  rr->z=p_Copy(x->z,r->algring);
  rr->s = 2;
  return (number)rr;
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
      lnumber l = (lnumber)ALLOC_LNUMBER();
      l->z = z;
      l->s = 2;
      l->n = NULL;
      return (number)l;
    }
  }
  /*else*/
  return NULL;
}

/*3
*  division with remainder: f = g*q + r,
*  returns r and destroys f
*/
napoly ntRemainder(napoly f, const napoly g)
{
  napoly a, h, qq;

  qq = (napoly)p_Init(nacRing);
  pNext(qq) = NULL;
  p_Normalize(g, nacRing);
  p_Normalize(f, nacRing);
  a = f;
  do
  {
    napSetExp(qq,1, p_GetExp(a,1,nacRing) - p_GetExp(g,1,nacRing));
    napSetm(qq);
    pGetCoeff(qq) = nacDiv(pGetCoeff(a), pGetCoeff(g));
    pGetCoeff(qq) = nacNeg(pGetCoeff(qq));
    nacNormalize(pGetCoeff(qq));
    h = napCopy(g);
    napMultT(h, qq);
    p_Normalize(h,nacRing);
    n_Delete(&pGetCoeff(qq),nacRing);
    a = napAdd(a, h);
  }
  while ((a!=NULL) && (p_GetExp(a,1,nacRing) >= p_GetExp(g,1,nacRing)));
  omFreeBinAddr(qq);
  return a;
}

number  ntPar(int i)
{
  lnumber l = ALLOC_LNUMBER();
  l->s = 2;
  l->z = p_ISet(1,nacRing);
  napSetExp(l->z,i,1);
  p_Setm(l->z,nacRing);
  l->n = NULL;
  return (number)l;
}

int     ntParDeg(number n)     /* i := deg(n) */
{
  lnumber l = (lnumber)n;
  if (l==NULL) return -1;
  return napDeg(l->z);
}

//int     ntParDeg(number n)     /* i := deg(n) */
//{
//  lnumber l = (lnumber)n;
//  if (l==NULL) return -1;
//  return napMaxDeg(l->z)+napMaxDeg(l->n);
//}

int     ntSize(number n)     /* size desc. */
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
int ntInt(number &n, const ring r)
{
  lnumber l=(lnumber)n;
  if ((l!=NULL)&&(l->n==NULL)&&(p_IsConstant(l->z,r->algring)))
  {
    return nacInt(pGetCoeff(l->z),r->algring);
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
    FREE_LNUMBER(l);
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
  erg = ALLOC_LNUMBER();
  erg->z = p_Copy(src->z, nacRing);
  erg->n = p_Copy(src->n, nacRing);
  erg->s = src->s;
  return (number)erg;
}
number nt_Copy(number p, const ring r)
{
  if (p==NULL) return NULL;
  lnumber erg;
  lnumber src = (lnumber)p;
  erg = ALLOC_LNUMBER();
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
  omCheckAddrSize(a,sizeof(slnumber));
  omCheckAddrSize(b,sizeof(slnumber));
  #endif
  if (b->n!=NULL) x = pp_Mult_qq(a->z, b->n,nacRing);
  else            x = napCopy(a->z);
  if (a->n!=NULL) y = pp_Mult_qq(b->z, a->n,nacRing);
  else            y = napCopy(b->z);
  napoly res = napAdd(x, y);
  if (res==NULL)
  {
    return (number)NULL;
  }
  lu = ALLOC_LNUMBER();
  lu->z=res;
  if (a->n!=NULL)
  {
    if (b->n!=NULL) x = pp_Mult_qq(a->n, b->n,nacRing);
    else            x = napCopy(a->n);
  }
  else
  {
    if (b->n!=NULL) x = napCopy(b->n);
    else            x = NULL;
  }
  //if (x!=NULL)
  //{
  //  if (p_LmIsConstant(x,nacRing))
  //  {
  //    number inv=nacInvers(pGetCoeff(x));
  //    napMultN(lu->z,inv);
  //    n_Delete(&inv,nacRing);
  //    napDelete(&x);
  //  }
  //}
  lu->n = x;
  lu->s = FALSE;
  if (/*lu->n*/ x!=NULL)
  {
     number luu=(number)lu;
     //if (p_IsConstant(lu->n,nacRing)) ntCoefNormalize(luu);
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
  omCheckAddrSize(a,sizeof(slnumber));
  omCheckAddrSize(b,sizeof(slnumber));
  #endif

  napoly x, y;
  if (b->n!=NULL) x = pp_Mult_qq(a->z, b->n,nacRing);
  else            x = napCopy(a->z);
  if (a->n!=NULL) y = p_Mult_q(napCopy(b->z), napCopyNeg(a->n),nacRing);
  else            y = napCopyNeg(b->z);
  napoly res = napAdd(x, y);
  if (res==NULL)
  {
    return (number)NULL;
  }
  lu = ALLOC_LNUMBER();
  lu->z=res;
  if (a->n!=NULL)
  {
    if (b->n!=NULL) x = pp_Mult_qq(a->n, b->n,nacRing);
    else            x = napCopy(a->n);
  }
  else
  {
    if (b->n!=NULL) x = napCopy(b->n);
    else            x = NULL;
  }
  lu->n = x;
  lu->s = FALSE;
  if (/*lu->n*/ x!=NULL)
  {
     number luu=(number)lu;
     //if (p_IsConstant(lu->n,nacRing)) ntCoefNormalize(luu);
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
  omCheckAddrSize(a,sizeof(slnumber));
  omCheckAddrSize(b,sizeof(slnumber));
  #endif
  ntTest(la);
  ntTest(lb);

  lo = ALLOC_LNUMBER();
  lo->z = pp_Mult_qq(a->z, b->z,nacRing);

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
      x = pp_Mult_qq(b->n, a->n, nacRing);
    }
  }
  if ((x!=NULL) && (p_LmIsConstant(x,nacRing)) && nacIsOne(pGetCoeff(x)))
    p_Delete(&x,nacRing);
  lo->n = x;
  lo->s = 0;
  number luu=(number)lo;
  if(lo->z==NULL)
  {
    FREE_LNUMBER(lo);
    lo=NULL;
  }
  else if (lo->n!=NULL)
  {
    // if (p_IsConstant(lo->n,nacRing)) ntCoefNormalize(luu);
    // else
                      ntNormalize(luu);
    lo=(lnumber)luu;
  }
  luu=(number)lo;
  if ((lo!=NULL) 
  && (naMinimalPoly!=NULL)
  &&(p_GetExp(lo->z,1,nacRing)>=p_GetExp(naMinimalPoly,1,nacRing)))
    lo->z=napRemainder(lo->z,naMinimalPoly);
  //if (naMinimalPoly==NULL) lo->s=2;
  ntTest((number)lo);
  return (number)lo;
}

number ntIntDiv(number la, number lb)
{
  ntTest(la);
  ntTest(lb);
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
#ifdef LDEBUG
  omCheckAddrSize(a,sizeof(slnumber));
  omCheckAddrSize(b,sizeof(slnumber));
#endif
  assume(a->z!=NULL && b->z!=NULL);
  assume(a->n==NULL && b->n==NULL);
  res = ALLOC_LNUMBER();
  res->z = napCopy(a->z);
  res->n = napCopy(b->z);
  res->s = 0;
  number nres=(number)res;
  ntNormalize(nres);

  //napDelete(&res->n);
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
  omCheckAddrSize(a,sizeof(slnumber));
  omCheckAddrSize(b,sizeof(slnumber));
  #endif
  lo = ALLOC_LNUMBER();
  if (b->n!=NULL)
    lo->z = pp_Mult_qq(a->z, b->n,nacRing);
  else
    lo->z = napCopy(a->z);
  if (a->n!=NULL)
    x = pp_Mult_qq(b->z, a->n, nacRing);
  else
    x = napCopy(b->z);
  if ((p_LmIsConstant(x,nacRing)) && nacIsOne(pGetCoeff(x)))
    p_Delete(&x,nacRing);
  lo->n = x;
  lo->s = 0;
  if (lo->n!=NULL)
  {
    number luu=(number)lo;
     //if (p_IsConstant(lo->n,nacRing)) ntCoefNormalize(luu);
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
  omCheckAddrSize(b,sizeof(slnumber));
  #endif
  lo = ALLOC0_LNUMBER();
  lo->s = b->s;
  if (b->n!=NULL)
    lo->z = napCopy(b->n);
  else
    lo->z = p_ISet(1,nacRing);
  x = b->z;
  if ((!p_LmIsConstant(x,nacRing)) || !nacIsOne(pGetCoeff(x)))
    x = napCopy(x);
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
     //if (p_IsConstant(lo->n,nacRing)) ntCoefNormalize(luu);
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
    return (nacGreaterZero(pGetCoeff(zb->z))||(!p_LmIsConstant(zb->z,nacRing)));
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
    an_deg=napDeg(aa->n);
  int bn_deg=0;
  if(bb->n!=NULL)
    bn_deg=napDeg(bb->n);
  if(an_deg+napDeg(bb->z)!=bn_deg+napDeg(aa->z))
    return FALSE;
#if 0
  ntNormalize(a);
  aa=(lnumber)a;
  ntNormalize(b);
  bb=(lnumber)b;
  if((aa->n==NULL)&&(bb->n!=NULL)) return FALSE;
  if((bb->n==NULL)&&(aa->n!=NULL)) return FALSE;
  if(napComp(aa->z,bb->z)!=0) return FALSE;
  if((aa->n!=NULL) && (napComp(aa->n,bb->n))) return FALSE;
#endif
  number h = ntSub(a, b);
  BOOLEAN bo = ntIsZero(h);
  ntDelete(&h,currRing);
  return bo;
}

/* This method will only consider the numerators of a and b.
   Moreover it may return TRUE only if one or both numerators
   are zero or if their degrees are equal. Then TRUE is returned iff
   coeff(numerator(a)) > coeff(numerator(b));
   In all other cases, FALSE will be returned. */
BOOLEAN ntGreater (number a, number b)
{
  int az = 0; int ad = 0;
  if (ntIsZero(a)) az = 1;
  else ad = napDeg(((lnumber)a)->z);
  int bz = 0; int bd = 0;
  if (ntIsZero(b)) bz = 1;
  else bd = napDeg(((lnumber)b)->z);
  
  if ((az == 1) && (bz == 1)) /* a = b = 0 */ return FALSE;
  if (az == 1) /* a = 0, b != 0 */
  {
    return (!nacGreaterZero(pGetCoeff(((lnumber)b)->z)));
  }
  if (bz == 1) /* a != 0, b = 0 */
  {
    return (nacGreaterZero(pGetCoeff(((lnumber)a)->z)));
  }
  if (ad == bd)  
    return nacGreater(pGetCoeff(((lnumber)a)->z),
                      pGetCoeff(((lnumber)b)->z));
  return FALSE;
}

/*2
* reads a number
*/
const char  *ntRead(const char *s, number *p)
{
  napoly x;
  lnumber a;
  s = napRead(s, &x);
  if (x==NULL)
  {
    *p = NULL;
    return s;
  }
  *p = (number)ALLOC0_LNUMBER();
  a = (lnumber)*p;
  a->z = x;
  if(a->z==NULL)
  {
    FREE_LNUMBER(a);
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
  char *s=(char *)omAlloc(4* ntNumbOfPar);
  char *t=(char *)omAlloc(8);
  s[0]='\0';
  for (i = 0; i <= ntNumbOfPar - 1; i++)
  {
    int e=p_GetExp(ph->z,i+1,nacRing);
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
    napWrite(ph->z,has_denom/*(ph->n!=NULL)*/,r);
    if (has_denom/*(ph->n!=NULL)*/)
    {
      StringAppendS("/");
      napWrite(ph->n,TRUE,r);
    }
  }
}

/*2
* za == 1 ?
*/
BOOLEAN ntIsOne(number za)
{
  lnumber a = (lnumber)za;
  if (a==NULL) return FALSE;
#ifdef LDEBUG
  omCheckAddrSize(a,sizeof(slnumber));
  if (a->z==NULL)
  {
    WerrorS("internal zero error(4)");
    return FALSE;
  }
#endif
  if (a->n==NULL)
  {
    if (p_LmIsConstant(a->z,nacRing))
    {
      return nacIsOne(pGetCoeff(a->z));
    }
    else                 return FALSE;
  }
#if 0
  number t;
  x = a->z;
  y = a->n;
  do
  {
    if (napComp(x, y))
      return FALSE;
    else
    {
      t = nacSub(pGetCoeff(x), pGetCoeff(y));
      if (!nacIsZero(t))
      {
        n_Delete(&t,nacRing);
        return FALSE;
      }
      else
        n_Delete(&t,nacRing);
    }
    pIter(x);
    pIter(y);
  }
  while ((x!=NULL) && (y!=NULL));
  if ((x!=NULL) || (y!=NULL)) return FALSE;
  p_Delete(&a->z,nacRing);
  p_Delete(&a->n,nacRing);
  a->z = p_ISet(1,nacRing);
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
  if (a==NULL) return FALSE;
#ifdef LDEBUG
  omCheckAddrSize(a,sizeof(slnumber));
  if (a->z==NULL)
  {
    WerrorS("internal zero error(5)");
    return FALSE;
  }
#endif
  if (a->n==NULL)
  {
    if (p_LmIsConstant(a->z,nacRing)) return n_IsMOne(pGetCoeff(a->z),nacRing);
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
  lnumber result = ALLOC0_LNUMBER();

  x = (lnumber)a;
  y = (lnumber)b;
#ifndef HAVE_FACTORY
  result->z = napGcd(x->z, y->z); // change from napGcd0
#else
  int c=ABS(nGetChar());
  if (c==1) c=0;
  setCharacteristic( c );

  napoly rz=napGcd(x->z, y->z);
  CanonicalForm F, G, R;
  R=convSingPFactoryP(rz,r->algring);
  p_Normalize(x->z,nacRing);
  F=convSingPFactoryP(x->z,r->algring)/R;
  p_Normalize(y->z,nacRing);
  G=convSingPFactoryP(y->z,r->algring)/R;
  F = gcd( F, G );
  if (F.isOne())
    result->z= rz;
  else
  {
    p_Delete(&rz,r->algring);
    result->z=convFactoryPSingP( F*R,r->algring );
    p_Normalize(result->z,nacRing);
  }
#endif
  ntTest((number)result);
  return (number)result;
}


/*2
* ntNumbOfPar = 1:
* clears denominator         algebraic case;
* tries to simplify ratio    transcendental case;
*
* cancels monomials
* occuring in denominator
* and enumerator  ?          ntNumbOfPar != 1;
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
  nz=p_GetAllDenom(p->z,nacRing);
  BOOLEAN norm=FALSE;
  if (!n_IsOne(nz,nacRing))
  {
    norm=TRUE;
    p->z=p_Mult_nn(p->z,nz,nacRing);
    if (p->n==NULL)
    {
      p->n=p_NSet(nz,nacRing);
    }
    else
    {
      p->n=p_Mult_nn(p->n,nz,nacRing);
      n_Delete(&nz, nacRing);
    }
  }
  else
  {
    n_Delete(&nz, nacRing);
  }
  if (norm)
  {
    norm=FALSE;
    p_Normalize(p->z,nacRing);
    p_Normalize(p->n,nacRing);
  }
  number nn;
  nn=p_GetAllDenom(p->n,nacRing);
  if (!n_IsOne(nn,nacRing))
  {
    norm=TRUE;
    p->n=p_Mult_nn(p->n,nn,nacRing);
    p->z=p_Mult_nn(p->z,nn,nacRing);
    n_Delete(&nn, nacRing);
  }
  else
  {
    n_Delete(&nn, nacRing);
  }
  if (norm)
  {
    p_Normalize(p->z,nacRing);
    p_Normalize(p->n,nacRing);
  }
  // remove common factors in n, z:
  if (p->n!=NULL)
  {
    poly pp=p->z;
    nz=n_Copy(pGetCoeff(pp),nacRing);
    pIter(pp);
    while(pp!=NULL)
    {
      if (n_IsOne(nz,nacRing)) break;
      number d=n_Gcd(nz,pGetCoeff(pp),nacRing);
      n_Delete(&nz,nacRing); nz=d;
      pIter(pp);
    }
    if (!n_IsOne(nz,nacRing))
    {
      pp=p->n;
      nn=n_Copy(pGetCoeff(pp),nacRing);
      pIter(pp);
      while(pp!=NULL)
      {
        if (n_IsOne(nn,nacRing)) break;
        number d=n_Gcd(nn,pGetCoeff(pp),nacRing);
        n_Delete(&nn,nacRing); nn=d;
        pIter(pp);
      }
      number ng=n_Gcd(nz,nn,nacRing);
      n_Delete(&nn,nacRing);
      if (!n_IsOne(ng,nacRing))
      {
        number ni=n_Invers(ng,nacRing);
	p->z=p_Mult_nn(p->z,ni,nacRing);
	p->n=p_Mult_nn(p->n,ni,nacRing);
        p_Normalize(p->z,nacRing);
        p_Normalize(p->n,nacRing);
	n_Delete(&ni,nacRing);
      }
      n_Delete(&ng,nacRing);
    }
    n_Delete(&nz,nacRing);
  }
  if (p->n!=NULL)
  {
    if(!nacGreaterZero(pGetCoeff(p->n)))
    {
      p->z=napNeg(p->z);
      p->n=napNeg(p->n);
    }

    if (/*(p->n!=NULL) && */
    (p_IsConstant(p->n,nacRing))
    && (n_IsOne(pGetCoeff(p->n),nacRing)))
    {
      p_Delete(&(p->n), nacRing);
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

  if (y==NULL) return;

  if ((x!=NULL) && (y!=NULL))
  {
    int i;
    for (i=ntNumbOfPar-1; i>=0; i--)
    {
      napoly xx=x;
      napoly yy=y;
      int m = napExpi(i, yy, xx);
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
  if (p_LmIsConstant(y,nacRing)) /* i.e. => simplify to (1/c)*z / monom */
  {
    if (nacIsOne(pGetCoeff(y)))
    {
      p_LmDelete(&y,nacRing);
      p->n = NULL;
      ntTest(pp);
      return;
    }
    number h1 = nacInvers(pGetCoeff(y));
    nacNormalize(h1);
    napMultN(x, h1);
    n_Delete(&h1,nacRing);
    p_LmDelete(&y,nacRing);
    p->n = NULL;
    ntTest(pp);
    return;
  }
#ifndef FACTORY_GCD_TEST
  if (ntNumbOfPar == 1) /* apply built-in gcd */
  {
    napoly x1,y1;
    if (p_GetExp(x,1,nacRing) >= p_GetExp(y,1,nacRing))
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
      r = ntRemainder(x1, y1);
      if ((r==NULL) || (pNext(r)==NULL)) break;
      x1 = y1;
      y1 = r;
    }
    if (r!=NULL)
    {
      p_Delete(&r,nacRing);
      p_Delete(&y1,nacRing);
    }
    else
    {
      napDivMod(x, y1, &(p->z), &r);
      napDivMod(y, y1, &(p->n), &r);
      p_Delete(&y1,nacRing);
    }
    x = p->z;
    y = p->n;
    /* collect all denoms from y and multiply x and y by it */
    if (ntIsChar0)
    {
      number n=napLcm(y);
      napMultN(x,n);
      napMultN(y,n);
      n_Delete(&n,nacRing);
      while(x!=NULL)
      {
        nacNormalize(pGetCoeff(x));
        pIter(x);
      }
      x = p->z;
      while(y!=NULL)
      {
        nacNormalize(pGetCoeff(y));
        pIter(y);
      }
      y = p->n;
    }
    if (pNext(y)==NULL)
    {
      if (nacIsOne(pGetCoeff(y)))
      {
        if (p_GetExp(y,1,nacRing)==0)
        {
          p_LmDelete(&y,nacRing);
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
      p_Delete(&x,nacRing);
      p_Delete(&y,nacRing);
    }
  }
#endif
  /* remove common factors from z and n */
  x=p->z;
  y=p->n;
  if(!nacGreaterZero(pGetCoeff(y)))
  {
    x=napNeg(x);
    y=napNeg(y);
  }
  number g=nacCopy(pGetCoeff(x));
  pIter(x);
  while (x!=NULL)
  {
    number d=nacGcd(g,pGetCoeff(x), nacRing);
    if(nacIsOne(d))
    {
      n_Delete(&g,nacRing);
      n_Delete(&d,nacRing);
      ntTest(pp);
      return;
    }
    n_Delete(&g,nacRing);
    g = d;
    pIter(x);
  }
  while (y!=NULL)
  {
    number d=nacGcd(g,pGetCoeff(y), nacRing);
    if(nacIsOne(d))
    {
      n_Delete(&g,nacRing);
      n_Delete(&d,nacRing);
      ntTest(pp);
      return;
    }
    n_Delete(&g,nacRing);
    g = d;
    pIter(y);
  }
  x=p->z;
  y=p->n;
  while (x!=NULL)
  {
    number d = nacIntDiv(pGetCoeff(x),g);
    napSetCoeff(x,d);
    pIter(x);
  }
  while (y!=NULL)
  {
    number d = nacIntDiv(pGetCoeff(y),g);
    napSetCoeff(y,d);
    pIter(y);
  }
  n_Delete(&g,nacRing);
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
  result = ALLOC0_LNUMBER();
  ntTest(la);
  ntTest(lb);
  napoly x = p_Copy(a->z, r->algring);
  number t = napLcm(b->z); // get all denom of b->z
  if (!nacIsOne(t))
  {
    number bt, rr;
    napoly xx=x;
    while (xx!=NULL)
    {
      bt = nacGcd(t, pGetCoeff(xx), r->algring);
      rr = nacMult(t, pGetCoeff(xx));
      n_Delete(&pGetCoeff(xx),r->algring);
      pGetCoeff(xx) = nacDiv(rr, bt);
      nacNormalize(pGetCoeff(xx));
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
* map Z/p -> Q(a)
*/
number ntMapP0(number c)
{
  if (npIsZero(c)) return NULL;
  lnumber l=ALLOC_LNUMBER();
  l->s=2;
  l->z=(napoly)p_Init(nacRing);
  int i=(int)((long)c);
  if (i>((long)ntMapRing->ch>>2)) i-=(long)ntMapRing->ch;
  pGetCoeff(l->z)=nlInit(i, nacRing);
  l->n=NULL;
  return (number)l;
}

/*2
* map Q -> Q(a)
*/
number ntMap00(number c)
{
  if (nlIsZero(c)) return NULL;
  lnumber l=ALLOC_LNUMBER();
  l->s=0;
  l->z=(napoly)p_Init(nacRing);
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
  lnumber l=ALLOC_LNUMBER();
  l->s=2;
  l->z=(napoly)p_Init(nacRing);
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
  lnumber l=ALLOC_LNUMBER();
  l->s=2;
  l->z=(napoly)p_Init(nacRing);
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
  number n=npInit(nlModP(c,npPrimeM),nacRing);
  if (npIsZero(n)) return NULL;
  npTest(n);
  lnumber l=ALLOC_LNUMBER();
  l->s=2;
  l->z=(napoly)p_Init(nacRing);
  pGetCoeff(l->z)=n;
  l->n=NULL;
  return (number)l;
}

/*2
* map _(a) -> _(b)
*/
number ntMapQaQb(number c)
{
  if (c==NULL) return NULL;
  lnumber erg= ALLOC0_LNUMBER();
  lnumber src =(lnumber)c;
  erg->s=src->s;
  erg->z=napMap(src->z);
  erg->n=napMap(src->n);
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
      nacMap=nacCopy;
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
        nacMap=nacCopy;
      }
      else
      {
        nacMap = npMapP;
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
      && (nacMap==nacCopy))
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
    nMap=currRing->algring->cf->cfSetMap(oldRing->algring, nacRing);
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
      pGetCoeff(p)=(number)ALLOC0_LNUMBER();
      pan=(lnumber)pGetCoeff(p);
      pan->s=2;
      pan->z=napInitz(nMap(pGetCoeff(za)));
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
            p_Setm(pa,nacRing);
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
          p_Setm(pa,nacRing);
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
          pan->n=napPerm(zb,par_perm,oldRing,nMap);
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
    lnumber rr=ALLOC0_LNUMBER();
    rr->z=p_Copy(x->n,r->algring);
    rr->s = 2;
    return (number)rr;
  }
  return n_Init(1,r);
}

number   ntGetNumerator(number &n, const ring r)
{
  lnumber x=(lnumber)n;
  lnumber rr=ALLOC0_LNUMBER();
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
  omCheckAddrSize(a, sizeof(slnumber));
  #endif
  napoly p = x->z;
  if (p==NULL)
  {
    Print("0/* in %s:%d\n",f,l);
    return FALSE;
  }
  while(p!=NULL)
  {
    if (( ntIsChar0  && nlIsZero(pGetCoeff(p)))
    || ((!ntIsChar0) && npIsZero(pGetCoeff(p))))
    {
      Print("coeff 0 in %s:%d\n",f,l);
      return FALSE;
    }
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
#endif
