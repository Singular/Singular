/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: rmodulo2m.cc,v 1.5 2006-12-06 16:59:49 wienand Exp $ */
/*
* ABSTRACT: numbers modulo 2^m
*/

#include <string.h>
#include "mod2.h"
#include <mylimits.h>
#include "structs.h"
#include "febase.h"
#include "omalloc.h"
#include "numbers.h"
#include "longrat.h"
#include "mpr_complex.h"
#include "ring.h"
#include "rmodulo2m.h"


int nr2mExp;
long nr2mModul;

/*
 * Multiply two numbers
 */
number nr2mMult (number a,number b)
{
  if (((long)a == 0) || ((long)b == 0))
    return (number)0;
  else
    return nr2mMultM(a,b);
}

/*
 * Give the smallest non unit k, such that a * x = k = b * y has a solution
 */
number nr2mLcm (number a,number b,ring r)
{
  long res = 0;
  if ((long) a == 0) a = (number) 1;
  if ((long) b == 0) b = (number) 1;
  while ((long) a % 2 == 0)
  {
    a = (number) ((long) a / 2);
    if ((long) b % 2 == 0) b = (number) ((long) b / 2);
    res++;
  }
  while ((long) b % 2 == 0)
  {
    b = (number) ((long) b / 2);
    res++;
  }
  return (number) (1L << res);  // (2**res)
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution.
 */
number nr2mGcd (number a,number b,ring r)
{
  long res = 0;
  if ((long) a == 0 && (long) b == 0) return (number) 1;
  while ((long) a % 2 == 0 && (long) b % 2 == 0)
  {
    a = (number) ((long) a / 2);
    b = (number) ((long) b / 2);
    res++;
  }
  if ((long) b % 2 == 0)
  {
    return (number) ((1L << res));// * (long) a);  // (2**res)*a    a ist Einheit
  }
  else
  {
    return (number) ((1L << res));// * (long) b);  // (2**res)*b    b ist Einheit
  }
}

void nr2mPower (number a, int i, number * result)
{
  if (i==0)
  {
    //npInit(1,result);
    *(long *)result = 1;
  }
  else if (i==1)
  {
    *result = a;
  }
  else
  {
    nr2mPower(a,i-1,result);
    *result = nr2mMultM(a,*result);
  }
}

/*
 * create a number from int
 */
number nr2mInit (int i)
{
  long ii = i;
  while (ii < 0) ii += nr2mModul;
  while ((ii>1) && (ii >= nr2mModul)) ii -= nr2mModul;
  return (number) ii;
}

/*
 * convert a number to int (-p/2 .. p/2)
 */
int nr2mInt(number &n)
{
  if ((long)n > (nr2mModul >>1)) return (int)((long)n - nr2mModul);
  else return (int)((long)n);
}

number nr2mAdd (number a, number b)
{
  return nr2mAddM(a,b);
}

number nr2mSub (number a, number b)
{
  return nr2mSubM(a,b);
}

BOOLEAN nr2mIsZero (number  a)
{
  return 0 == (long)a;
}

BOOLEAN nr2mIsOne (number a)
{
  return 1 == (long)a;
}

BOOLEAN nr2mIsMOne (number a)
{
  return nr2mModul == (long)a + 1;
}

BOOLEAN nr2mEqual (number a,number b)
{
  return nr2mEqualM(a,b);
}

BOOLEAN nr2mGreater (number a,number b)
{
  if ((long) a == 0) return TRUE;
  if ((long) b == 0) return FALSE;
  while ((long) a % 2 == 0 && (long) b % 2 == 0)
  {
    a = (number) ((long) a / 2);
    b = (number) ((long) b / 2);
}
  return ((long) b % 2 == 1);
}

BOOLEAN nr2mGreaterZero (number k)
{
  int h = (int)((long) k);
  return ((int)h !=0) && (h <= (nr2mModul>>1));
}

//#ifdef HAVE_DIV_MOD
#if 1 //ifdef HAVE_NTL // in ntl.a
//extern void XGCD(long& d, long& s, long& t, long a, long b);
#include <NTL/ZZ.h>
#ifdef NTL_CLIENT
NTL_CLIENT
#endif
#else
void XGCD(long& d, long& s, long& t, long a, long b)
{
   long  u, v, u0, v0, u1, v1, u2, v2, q, r;

   long aneg = 0, bneg = 0;

   if (a < 0) {
      a = -a;
      aneg = 1;
   }

   if (b < 0) {
      b = -b;
      bneg = 1;
   }

   u1=1; v1=0;
   u2=0; v2=1;
   u = a; v = b;

   while (v != 0) {
      q = u / v;
      r = u % v;
      u = v;
      v = r;
      u0 = u2;
      v0 = v2;
      u2 =  u1 - q*u2;
      v2 = v1- q*v2;
      u1 = u0;
      v1 = v0;
   }

   if (aneg)
      u1 = -u1;

   if (bneg)
      v1 = -v1;

   d = u;
   s = u1;
   t = v1;
}
#endif

long InvMod(long a)
{
   long d, s, t;

   XGCD(d, s, t, a, nr2mModul);
   assume (d == 1);
   if (s < 0)
      return s + nr2mModul;
   else
      return s;
}
//#endif

inline number nr2mInversM (number c)
{
  // Table !!!
  long inv;
  inv = InvMod((long)c);
  return (number) inv;
}

number nr2mDiv (number a,number b)
{
  if ((long)a==0)
    return (number)0;
  else if ((long)b%2==0)
  {
    if ((long)b != 0)
    {
      while ((long) b%2 == 0 && (long) a%2 == 0)
      {
        a = (number) ((long) a / 2);
        b = (number) ((long) b / 2);
      }
    }
    if ((long) b%2 == 0)
    {
      WerrorS("div by zero divisor");
      return (number)0;
    }
  }
  return (number) nr2mMult(a, nr2mInversM(b));
}

number nr2mIntDiv (number a,number b)
{
  if ((long)a==0)
  {
    return (number) 0;
  }
  else
  {
    return (number) ((long) a / (long) b);
  }
}

number  nr2mInvers (number c)
{
  if ((long)c%2==0)
  {
    WerrorS("division by zero divisor");
    return (number)0;
  }
  return nr2mInversM(c);
}

number nr2mNeg (number c)
{
  if ((long)c==0) return c;
  return nr2mNegM(c);
}

nMapFunc nr2mSetMap(ring src, ring dst)
{
  return NULL;      /* default */
}


/*
 * set the exponent (allocate and init tables) (TODO)
 */

void nr2mSetExp(int m, ring r)
{
  if (m>1)
  {
    nr2mExp = m;
    nr2mModul = 2;
    for (int i = 1; i < m; i++) {
      nr2mModul = nr2mModul * 2;
    }
  }
  else
  {
    nr2mExp=0;
    nr2mModul=0;
  }
//  PrintS("Modul: ");
//  Print("%d\n", nr2mModul);
}

void nr2mInitExp(int m, ring r)
{
  int i, w;

  if (m>1)
  {
    nr2mExp = m;
    nr2mModul = 2;
    for (int i = 1; i < m; i++) {
      nr2mModul = nr2mModul * 2;

    }
  }
  else
  {
    WarnS("nInitChar failed");
  }
}

#ifdef LDEBUG
BOOLEAN nr2mDBTest (number a, char *f, int l)
{
  if (((long)a<0) || ((long)a>nr2mModul))
  {
    return FALSE;
  }
  return TRUE;
}
#endif

void nr2mWrite (number &a)
{
  if ((long)a > (nr2mModul >>1)) StringAppend("-%d",(int)(nr2mModul-((long)a)));
  else                          StringAppend("%d",(int)((long)a));
}

char* nr2mEati(char *s, int *i)
{

  if (((*s) >= '0') && ((*s) <= '9'))
  {
    (*i) = 0;
    do
    {
      (*i) *= 10;
      (*i) += *s++ - '0';
      if ((*i) >= (MAX_INT_VAL / 10)) (*i) = (*i) % nr2mModul;
    }
    while (((*s) >= '0') && ((*s) <= '9'));
    if ((*i) >= nr2mModul) (*i) = (*i) % nr2mModul;
  }
  else (*i) = 1;
  return s;
}

char * nr2mRead (char *s, number *a)
{
  int z;
  int n=1;

  s = nr2mEati(s, &z);
  if ((*s) == '/')
  {
    s++;
    s = nr2mEati(s, &n);
  }
  if (n == 1)
    *a = (number)z;
  else 
      *a = nr2mDiv((number)z,(number)n);
  return s;
}



