/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT:
*/

#include <string.h>
#include <math.h>
#include <kernel/mod2.h>
#include <omalloc/mylimits.h>
#include <kernel/structs.h>
#include <kernel/febase.h>
#include <kernel/numbers.h>
#include <kernel/longrat.h>
#include <kernel/mpr_complex.h>
#include <kernel/ring.h>
#include <kernel/shortfl.h>

static float nrEps = 1.0e-3;
union nf
{
  float _f;
  number _n;
  nf(float f) {_f = f;}
  nf(number n) {_n = n;}
  float F() const {return _f;}
  number N() const {return _n;}
};

float nrFloat(number n)
{
  return nf(n).F();
}

BOOLEAN nrGreaterZero (number k)
{
  return nf(k).F() >= 0.0;
}

number nrMult (number a,number b)
{
  return nf(nf(a).F() * nf(b).F()).N();
}

/*2
* create a number from int
*/
number nrInit (long i, const ring R)
{
  float r = (float)i;
  return nf(nf(r).F()).N();
}

/*2
* convert a number to int
*/
int nrInt(number &n, const ring R)
{
  int i;
  float r = nf(n).F();
  if (((float)INT_MIN <= r) || ((float)MAX_INT_VAL >= r))
    i = (int)r;
  else
    i = 0;
  return i;
}

int nrSize(number n)
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

number nrAdd (number a, number b)
{
  float x = nf(a).F();
  float y = nf(b).F();
  float r = x + y;
  if (x > 0.0)
  {
    if (y < 0.0)
    {
      x = r / (x - y);
      if (x < 0.0)
        x = -x;
      if (x < nrEps)
        r = 0.0;
    }
  }
  else
  {
    if (y > 0.0)
    {
      x = r / (y - x);
      if (x < 0.0)
        x = -x;
      if (x < nrEps)
        r = 0.0;
    }
  }
  return nf(r).N();
}

number nrSub (number a, number b)
{
  float x = nf(a).F();
  float y = nf(b).F();
  float r = x - y;
  if (x > 0.0)
  {
    if (y > 0.0)
    {
      x = r / (x + y);
      if (x < 0.0)
        x = -x;
      if (x < nrEps)
        r = 0.0;
    }
  }
  else
  {
    if (y < 0.0)
    {
      x = r / (x + y);
      if (x < 0.0)
        x = -x;
      if (x < nrEps)
        r = 0.0;
    }
  }
  return nf(r).N();
}

BOOLEAN nrIsZero (number  a)
{
  return (0.0 == nf(a).F());
}

BOOLEAN nrIsOne (number a)
{
  float aa=nf(a).F()-1.0;
  if (aa<0.0) aa=-aa;
  return (aa<nrEps);
}

BOOLEAN nrIsMOne (number a)
{
  float aa=nf(a).F()+1.0;
  if (aa<0.0) aa=-aa;
  return (aa<nrEps);
}

number nrDiv (number a,number b)
{
  float n = nf(b).F();
  if (n == 0.0)
  {
    WerrorS(nDivBy0);
    return nf((float)0.0).N();
  }
  else
    return nf(nf(a).F() / n).N();
}

number  nrInvers (number c)
{
  float n = nf(c).F();
  if (n == 0.0)
  {
    WerrorS(nDivBy0);
    return nf((float)0.0).N();
  }
  return nf(1.0 / n).N();
}

number nrNeg (number c)
{
  return nf(-nf(c).F()).N();
}

BOOLEAN nrGreater (number a,number b)
{
  return nf(a).F() > nf(b).F();
}

BOOLEAN nrEqual (number a,number b)
{
  number x = nrSub(a,b);
  return nf(x).F() == nf((float)0.0).F();
}

void nrWrite (number &a, const ring r)
{
  StringAppend("%9.3e", nf(a).F());
}

void nrPower (number a, int i, number * result)
{
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
  nrPower(a,i-1,result);
  *result = nf(nf(a).F() * nf(*result).F()).N();
}

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

const char *nIllegalChar="illegal character in number";

const char * nrRead (const char *s, number *a)
{
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

/*2
* the last used charcteristic
*/
int nrGetChar()
{
  return 0;
}

#ifdef LDEBUG
/*2
* test valid numbers: not implemented yet
*/
//BOOLEAN nrDBTest(number a, const char *f, const int l)
//{
//  return TRUE;
//}
#endif

/* in longrat.h
#define SR_INT    1
#define mpz_size1(A) (ABS((A)->_mp_size))
*/
#define SR_HDL(A) ((long)(A))
#define IS_INT(A) ((A)->s==3)
#define IS_IMM(A) (SR_HDL(A)&SR_INT)
#define GET_NOM(A) ((A)->z)
#define GET_DENOM(A) ((A)->n)

number nrMapQ(number from)
{
  if (IS_IMM(from))
    return nf((float)nlInt(from,NULL /* dummy for nlInt*/)).N();

  /* read out the enumerator */
  mpz_ptr z=GET_NOM(from);
  int i = mpz_size1(z);
  mpf_t e;
  mpf_init(e);
  mpf_set_z(e,z);

  /* if number was an integer, we are done*/
  if(IS_INT(from))
  {
    if(i>4)
    {
      WerrorS("float overflow");
      return nf(0.0).N();
    }
    double basis;
    signed long int exp;
    basis = mpf_get_d_2exp(&exp, e);
    float f= mpf_sgn(e)*ldexp(basis,exp);
    mpf_clear(e);
    return nf(f).N();
  }

  /* else read out the denominator */
  mpz_ptr n = GET_DENOM(from);
  int j = mpz_size1(n);
  if(j-i>4)
  {
    WerrorS("float overflow");
    mpf_clear(e);
    return nf(0.0).N();
  }
  mpf_t d;
  mpf_init(d);
  mpf_set_z(d,n);

  /* and compute the quotient */
  mpf_t q;
  mpf_init(q);
  mpf_div(q,e,d);

  double basis;
  signed long int exp;
  basis = mpf_get_d_2exp(&exp, q);
  float f = mpf_sgn(e)*ldexp(basis,exp);
  mpf_clear(e);
  mpf_clear(d);
  mpf_clear(q);
  return nf(f).N();
}

static number nrMapP(number from)
{
  int i = (int)((long)from);
  float r = (float)i;
  return nf(r).N();
}

static number nrMapLongR(number from)
{
  float t =(float)mpf_get_d((mpf_srcptr)from);
  return nf(t).N();
}
static number nrMapC(number from)
{
  gmp_float h = ((gmp_complex*)from)->real();
  float t =(float)mpf_get_d((mpf_srcptr)&h);
  return nf(t).N();
}

nMapFunc nrSetMap(const ring src, const ring dst)
{
  if (rField_is_Q(src))
  {
    return nrMapQ;
  }
  if (rField_is_long_R(src))
  {
    return nrMapLongR;
  }
  if (rField_is_R(src))
  {
    return ndCopy;
  }
  if(rField_is_Zp(src))
  {
    return nrMapP;
  }
  if (rField_is_long_C(src))
  {
    return nrMapC;
  }
  return NULL;
}
