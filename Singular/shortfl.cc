/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
static char rcsid[] = "$Header: /exports/cvsroot-2/cvsroot/Singular/shortfl.cc,v 1.1.1.1 1997-03-19 13:18:50 obachman Exp $";
/* $Log: not supported by cvs2svn $
*/

/*
* ABSTRACT:
*/

#include <limits.h>
#include <string.h>
#include "mod2.h"
#include "febase.h"
#include "numbers.h"
#include "longrat.h"
//#include "modulop.h"
#include "shortfl.h"

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
number nrInit (int i)
{
  float r = (float)i;
  return nf(nf(r).F()).N();
}

/*2
* convert a number to int
*/
int nrInt(number &n)
{
  int i;
  float r = nf(n).F();
  if (((float)INT_MIN <= r) || ((float)INT_MAX >= r))
    i = (int)r;
  else
    i = 0;
  return i;
}

number nrCopy (number  k1)
{
  return k1;
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
  return (1.0 == nf(a).F());
}

BOOLEAN nrIsMOne (number a)
{
  return ((-1.0) == nf(a).F());
}

number nrDiv (number a,number b)
{
  float n = nf(b).F();
  if (n == 0.0)
  {
    Werror("a/0");
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
    Werror("1/0");
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

void nrWrite (number &a)
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

char* nrEatr(char *s, float *r)
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

char * nrRead (char *s, number *a)
{
  char *t;
  float z1,z2;
  float n=1.0;

  s = nrEatr(s, &z1);
  if (*s == '/')
  {
    s++;
    s = nrEatr(s, &z2);
    z1 /= z2;
  }
  else if (*s =='.')
  {
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
* modulus for reals: 0.0
*/
number nrIntMod (number a, number b)
{
  return nf((float)0.0).N();
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
BOOLEAN nrDBTest(number a, char *f, int l)
{
  return TRUE;
}
#endif

static number nrMap0(number from)
{
  return nrInit(nlInt(from));
}

static number nrMapP(number from)
{
  int i = (int)from;
  float r = (float)i;
  return nf(r).N();
}

BOOLEAN nrSetMap(int c, char ** par, int nop, number minpol)
{
  if (c == 0)
    nMap = nrMap0;   /*Q -> R*/
  else if (c>1)
  {
    if (par==NULL)
      nMap = nrMapP; /* Z/p' -> R */
    else
     return FALSE;   /* GF(q) ->R */
  }
  else if (c<0)
     return FALSE;   /* Z/p'(a) -> R*/
  else if (c==1)
     return FALSE;   /* Q(a) -> R */
  return TRUE;
}
