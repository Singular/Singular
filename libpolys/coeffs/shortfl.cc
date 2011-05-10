/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/*
* ABSTRACT:
*/

#include <coeffs/shortfl.h>

#include <string.h>
#include <coeffs/coeffs.h>
#include <reporter/reporter.h>
#include <coeffs/numbers.h>
#include <coeffs/longrat.h>
#include <coeffs/mpr_complex.h>

#include <misc/mylimits.h>

/// Our Type!
static const n_coeffType ID = n_R;

static const float nrEps = 1.0e-3;

union nf
{
  float _f;
  number _n;

  nf(float f): _f(f){};

  nf(number n): _n(n){};

  inline float F() const {return _f;}
  inline number N() const {return _n;}
};




float nrFloat(number n)
{
  return nf(n).F();
}


void    nrCoeffWrite  (const coeffs r)
{
  assume( getCoeffType(r) == ID );
  PrintS("//   characteristic : 0 (real)\n");  /* R */
}


BOOLEAN nrGreaterZero (number k, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return nf(k).F() >= 0.0;
}

number nrMult (number a,number b, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return nf(nf(a).F() * nf(b).F()).N();
}

/*2
* create a number from int
*/
number nrInit (int i, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  float f = (float)i;
  return nf(nf(f).F()).N();
}

/*2
* convert a number to int
*/
int nrInt(number &n, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  int i;
  float f = nf(n).F();
  if (((float)INT_MIN <= f) || ((float)INT_MAX >= f))
    i = (int)f;
  else
    i = 0;
  return i;
}

int nrSize(number n, const coeffs)
{
  float f = nf(n).F();
  int i = (int)f;
  /* basically return the largest integer in n;
     only if this happens to be zero although n != 0,
     return 1;
     (this code ensures that zero has the size zero) */
  if ((f != 0.0) & (i == 0)) i = 1;
  return i;
}

number nrAdd (number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  float x = nf(a).F();
  float y = nf(b).F();
  float f = x + y;
  if (x > 0.0)
  {
    if (y < 0.0)
    {
      x = f / (x - y);
      if (x < 0.0)
        x = -x;
      if (x < nrEps)
        f = 0.0;
    }
  }
  else
  {
    if (y > 0.0)
    {
      x = f / (y - x);
      if (x < 0.0)
        x = -x;
      if (x < nrEps)
        f = 0.0;
    }
  }
  return nf(f).N();
}

number nrSub (number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  float x = nf(a).F();
  float y = nf(b).F();
  float f = x - y;
  if (x > 0.0)
  {
    if (y > 0.0)
    {
      x = f / (x + y);
      if (x < 0.0)
        x = -x;
      if (x < nrEps)
        f = 0.0;
    }
  }
  else
  {
    if (y < 0.0)
    {
      x = f / (x + y);
      if (x < 0.0)
        x = -x;
      if (x < nrEps)
        f = 0.0;
    }
  }
  return nf(f).N();
}

BOOLEAN nrIsZero (number  a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return (0.0 == nf(a).F());
}

BOOLEAN nrIsOne (number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  float aa=nf(a).F()-1.0;
  if (aa<0.0) aa=-aa;
  return (aa<nrEps);
}

BOOLEAN nrIsMOne (number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  float aa=nf(a).F()+1.0;
  if (aa<0.0) aa=-aa;
  return (aa<nrEps);
}

number nrDiv (number a,number b, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  float n = nf(b).F();
  if (n == 0.0)
  {
    WerrorS(nDivBy0);
    return nf((float)0.0).N();
  }
  else
    return nf(nf(a).F() / n).N();
}

number  nrInvers (number c, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  float n = nf(c).F();
  if (n == 0.0)
  {
    WerrorS(nDivBy0);
    return nf((float)0.0).N();
  }
  return nf(1.0 / n).N();
}

number nrNeg (number c, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return nf(-nf(c).F()).N();
}

BOOLEAN nrGreater (number a,number b, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return nf(a).F() > nf(b).F();
}

BOOLEAN nrEqual (number a,number b, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  number x = nrSub(a,b,r);
  return nf(x).F() == nf((float)0.0).F();
}

void nrWrite (number &a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  StringAppend("%9.3e", nf(a).F());
}

void nrPower (number a, int i, number * result, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  if (i==0)
  {
    *result = nf(nf(1.0).F()).N();
    return;
  }
  if (i==1)
  {
    *result = nf(nf(a).F()).N();
    return;
  }
  nrPower(a,i-1,result,r);
  *result = nf(nf(a).F() * nf(*result).F()).N();
}

namespace {
  static const char* nrEatr(const char *s, float *r)
  {
    int i;

    if    (*s >= '0' && *s <= '9')
    {
      *r = 0.0;
      do
      {
        *r *= 10.0;
        i = *s++ - '0';
        *r += (float)i;
      }
      while (*s >= '0' && *s <= '9');
    }
    else *r = 1.0;
    return s;
  }
}

const char * nrRead (const char *s, number *a, const coeffs r)
{

  assume( getCoeffType(r) == ID );

  static const char *nIllegalChar="illegal character in number";

  const char *t;
  const char *start=s;
  float z1,z2;
  float n=1.0;

  s = nrEatr(s, &z1);
  if (*s == '/')
  {
    if (s==start) { WerrorS(nIllegalChar);return s; }
    s++;
    s = nrEatr(s, &z2);
    if (z2==0.0)
      WerrorS(nDivBy0);
    else
      z1 /= z2;
  }
  else if (*s =='.')
  {
    if (s==start) { WerrorS(nIllegalChar);return s; }
    s++;
    t = s;
    while (*t >= '0' && *t <= '9')
    {
      t++;
      n *= 10.0;
    }
    s = nrEatr(s, &z2);
    z1 = (z1*n + z2) / n;
    if (*s=='e')
    {
      int e=0; /* exponent */
      int si=1;/* sign of exponent */
      s++;
      if (*s=='+') s++;
      else if (*s=='-') {s++; si=-1; }
      while (*s >= '0' && *s <= '9')
      {
        e=e*10+(*s)-'0';
        s++;
      }
      if (si==1)
      {
        while (e>0) {z1*=10.0; e--; }
      }
      else
      {
        while (e>0) {z1/=10.0; e--; }
      }
    }
  }
  *a = nf(z1).N();
  return s;
}


// the last used charcteristic
// int nrGetChar(){ return 0; }


#ifdef LDEBUG
/*2
* test valid numbers: not implemented yet
*/
#pragma GCC diagnostic ignored "-Wunused-parameter"
BOOLEAN  nrDBTest(number a, const char *f, const int l, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return TRUE;
}
#endif

static number nrMapP(number from, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == ID );
  assume( getCoeffType(aRing) ==  n_Zp );
  
  int i = (int)((long)from);
  float f = (float)i;
  return nf(f).N();
}

static number nrMapLongR(number from, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == ID );
  assume( getCoeffType(aRing) == n_long_R );

  float t =(float)mpf_get_d((mpf_srcptr)from);
  return nf(t).N();
}

static number nrMapC(number from, const coeffs aRing, const coeffs r)
{  
  assume( getCoeffType(r) == ID );
  assume( getCoeffType(aRing) == n_long_C );
  
  gmp_float h = ((gmp_complex*)from)->real();
  float t =(float)mpf_get_d((mpf_srcptr)&h);
  return nf(t).N();
}


number nrMapQ(number from, const coeffs aRing, const coeffs r)
{
/* in longrat.h
#define SR_INT    1
#define mpz_size1(A) (ABS((A)->_mp_size))
*/
#define SR_HDL(A) ((long)(A))
#define mpz_isNeg(A) ((A)->_mp_size<0)
#define mpz_limb_size(A) ((A)->_mp_size)
#define mpz_limb_d(A) ((A)->_mp_d)
#define MPZ_DIV(A,B,C) mpz_tdiv_q((A),(B),(C))
#define IS_INT(A) ((A)->s==3)
#define IS_IMM(A) (SR_HDL(A)&SR_INT)
#define GET_NOM(A) ((A)->z)
#define GET_DENOM(A) ((A)->n)
#define MPZ_INIT mpz_init
#define MPZ_CLEAR mpz_clear

  assume( getCoeffType(r) == ID );
  assume( getCoeffType(aRing) == n_Q );

  mpz_t h;
  mpz_ptr g,z,n;
  int i,j,t,s;
  float ba,rr,rn,y;

  if (IS_IMM(from))
    return nf((float)nlInt(from,NULL /* dummy for nlInt*/)).N();
  z=GET_NOM(from);
  s=0X10000;
  ba=(float)s;
  ba*=ba;
  rr=0.0;
  i=mpz_size1(z);
  if(IS_INT(from))
  {
    if(i>4)
    {
      WerrorS("float overflow");
      return nf(rr).N();
    }
    i--;
    rr=(float)mpz_limb_d(z)[i];
    while(i>0)
    {
      i--;
      y=(float)mpz_limb_d(z)[i];
      rr=rr*ba+y;
    }
    if(mpz_isNeg(z))
      rr=-rr;
    return nf(rr).N();
  }
  n=GET_DENOM(from);
  j=s=mpz_limb_size(n);
  if(j>i)
  {
    g=n; n=z; z=g;
    t=j; j=i; i=t;
  }
  t=i-j;
  if(t>4)
  {
    if(j==s)
      WerrorS("float overflow");
    return nf(rr).N();
  }
  if(t>1)
  {
    g=h;
    MPZ_INIT(g);
    MPZ_DIV(g,z,n);
    t=mpz_size1(g);
    if(t>4)
    {
      MPZ_CLEAR(g);
      if(j==s)
        WerrorS("float overflow");
      return nf(rr).N();
    }
    t--;
    rr=(float)mpz_limb_d(g)[t];
    while(t)
    {
      t--;
      y=(float)mpz_limb_d(g)[t];
      rr=rr*ba+y;
    }
    MPZ_CLEAR(g);
    if(j!=s)
      rr=1.0/rr;
    if(mpz_isNeg(z))
      rr=-rr;
    return nf(rr).N();
  }
  rn=(float)mpz_limb_d(n)[j-1];
  rr=(float)mpz_limb_d(z)[i-1];
  if(j>1)
  {
    rn=rn*ba+(float)mpz_limb_d(n)[j-2];
    rr=rr*ba+(float)mpz_limb_d(z)[i-2];
    i--;
  }
  if(t!=0)
    rr=rr*ba+(float)mpz_limb_d(z)[i-2];
  if(j==s)
    rr=rr/rn;
  else
    rr=rn/rr;
  if(mpz_isNeg(z))
    rr=-rr;
  return nf(rr).N();
}


nMapFunc nrSetMap(const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == ID );

  if (nCoeff_is_Q(src))
  {
    return nrMapQ;
  }
  if (nCoeff_is_long_R(src))
  {
    return nrMapLongR;
  }
  if (nCoeff_is_R(src))
  {
    return ndCopyMap;
  }
  if(nCoeff_is_Zp(src))
  {
    return nrMapP;
  }
  if (nCoeff_is_long_C(src))
  {
    return nrMapC;
  }
  return NULL;
}





/// test, whether r is an instance of nInitCoeffs(n, parameter)
static BOOLEAN nrCoeffsEqual(const coeffs r, n_coeffType n, void*)
{
  assume( getCoeffType(r) == ID );

  return (n == ID);
}


BOOLEAN nrInitChar(coeffs n, void*)
{
  assume( getCoeffType(n) == ID );
  n->cfKillChar = ndKillChar; /* dummy */

  n->cfInit = nrInit;
  n->cfInt  = nrInt;
  n->cfAdd   = nrAdd;
  n->cfSub   = nrSub;
  n->cfMult  = nrMult;
  n->cfDiv   = nrDiv;
  n->cfExactDiv= nrDiv;
  n->cfNeg   = nrNeg;
  n->cfInvers= nrInvers;
  n->cfCopy  = ndCopy;
  n->cfGreater = nrGreater;
  n->cfEqual = nrEqual;
  n->cfIsZero = nrIsZero;
  n->cfIsOne = nrIsOne;
  n->cfIsMOne = nrIsMOne;
  n->cfGreaterZero = nrGreaterZero;
  n->cfWrite = nrWrite;
  n->cfRead = nrRead;
  n->cfPower = nrPower;
  n->cfSetMap = nrSetMap;
  n->cfCoeffWrite  = nrCoeffWrite;

    /* nName= ndName; */
    /*nSize  = ndSize;*/
#ifdef LDEBUG
  n->cfDBTest=ndDBTest; // not yet implemented: nrDBTest;
#endif
  
  n->nCoeffIsEqual = nrCoeffsEqual;

  // TODO: Any variables?
  return FALSE;
}
