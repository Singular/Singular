/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: rmodulon.cc,v 1.4 2007-05-19 13:22:23 wienand Exp $ */
/*
* ABSTRACT: numbers modulo n
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
#include "rmodulon.h"

#ifdef HAVE_RINGMODN

NATNUMBER nrnModul;

NATNUMBER XSGCD3(NATNUMBER a, NATNUMBER b, NATNUMBER c)
{
  while ((a != 0 && b != 0) || (a != 0 && c != 0) || (b != 0 && c != 0))
  {
    if (a > b)
    {
      if (b > c) a = a % b;       // a > b > c
      else
      {
        if (a > c) a = a % c;     // a > b, c > b, a > c ==> a > c > b
        else c = c % a;           // c > a > b
      }
    }
    else
    {
      if (a > c) b = b % a;       // a > b > c
      else
      {
        if (b > c) b = b % c;     // a > b, c > b, a > c ==> a > c > b
        else c = c % b;           // c > a > b
      }
    }
  }
  return a + b + c;
}

NATNUMBER XSGCD2(NATNUMBER a, NATNUMBER b)
{
  NATNUMBER TMP;
  while (a != 0 && b != 0)
  {
     TMP = a % b;
     a = b;
     b = TMP;
  }
  return a;
}

/*
 * Multiply two numbers
 */
number nrnMult (number a,number b)
{
  if (((NATNUMBER)a == 0) || ((NATNUMBER)b == 0))
    return (number)0;
  else
    return nrnMultM(a,b);
}

/*
 * Give the smallest non unit k, such that a * x = k = b * y has a solution
 */
number nrnLcm (number a,number b,ring r)
{
  NATNUMBER erg = XSGCD2(nrnModul, (NATNUMBER) a) * XSGCD2(nrnModul, (NATNUMBER) b) / XSGCD3(nrnModul, (NATNUMBER) a, (NATNUMBER) b);
  if (erg == nrnModul) return NULL;   // Schneller als return erg % nrnModul ?
  return (number) erg;
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution.
 */
number nrnGcd (number a,number b,ring r)
{
  return (number) XSGCD3(nrnModul, (NATNUMBER) a, (NATNUMBER) b);
}

void nrnPower (number a, int i, number * result)
{
  if (i==0)
  {
    //npInit(1,result);
    *(NATNUMBER *)result = 1;
  }
  else if (i==1)
  {
    *result = a;
  }
  else
  {
    nrnPower(a,i-1,result);
    *result = nrnMultM(a,*result);
  }
}

/*
 * create a number from int
 */
number nrnInit (int i)
{
  NATNUMBER ii = i;
  while (ii < 0) ii += nrnModul;
  while ((ii>1) && (ii >= nrnModul)) ii -= nrnModul;
  return (number) ii;
}

/*
 * convert a number to int (-p/2 .. p/2)
 */
int nrnInt(number &n)
{
  if ((NATNUMBER)n > (nrnModul >>1)) return (int)((NATNUMBER)n - nrnModul);
  else return (int)((NATNUMBER)n);
}

number nrnAdd (number a, number b)
{
  return nrnAddM(a,b);
}

number nrnSub (number a, number b)
{
  return nrnSubM(a,b);
}

BOOLEAN nrnIsZero (number  a)
{
  return 0 == (NATNUMBER)a;
}

BOOLEAN nrnIsOne (number a)
{
  return 1 == (NATNUMBER)a;
}

BOOLEAN nrnIsMOne (number a)
{
  return nrnModul == (NATNUMBER)a + 1;
}

BOOLEAN nrnEqual (number a,number b)
{
  return nrnEqualM(a,b);
}

BOOLEAN nrnGreater (number a,number b)
{
  nrnDivBy(a, b);
}

int nrnComp(number a, number b)
{
   NATNUMBER bs = XSGCD2((NATNUMBER) b, nrnModul);
   NATNUMBER as = XSGCD2((NATNUMBER) a, nrnModul);
   if (bs == as) return 0;
   if (as % bs == 0) return -1;
   if (bs % as == 0) return 1;
   return 2;
}

BOOLEAN nrnDivBy (number a,number b)
{
  return (XSGCD2((NATNUMBER) b / XSGCD2((NATNUMBER) a, (NATNUMBER) b), nrnModul) == 1);
}

BOOLEAN nrnGreaterZero (number k)
{
  return (((NATNUMBER) k) != 0) && ((NATNUMBER) k <= (nrnModul>>1));
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
//#endif

NATNUMBER InvModN(NATNUMBER a)
{
   long d, s, t;

   // TODO in chain wird XSGCD2 aufgerufen
   XGCD(d, s, t, a, nrnModul);
   assume (d == 1);
   if (s < 0)
      return s + nrnModul;
   else
      return s;
}

inline number nrnInversM (number c)
{
  // Table !!!
  NATNUMBER inv;
  inv = InvModN((NATNUMBER)c);
  return (number) inv;
}

number nrnDiv (number a,number b)
{
  NATNUMBER tmp = XSGCD3(nrnModul, (NATNUMBER) a, (NATNUMBER) b);
  a = (number) ((NATNUMBER) a / tmp);
  b = (number) ((NATNUMBER) b / tmp);
  if (XSGCD2(nrnModul, (NATNUMBER) b) == 1)
  {
    return (number) nrnMult(a, nrnInversM(b));
  }
  else
  {
    WerrorS("div by zero divisor");
    return (number)0;
  }
}

number nrnIntDiv (number a,number b)
{
  if ((NATNUMBER)a==0)
  {
    if ((NATNUMBER)b==0)
      return (number) 1;
    if ((NATNUMBER)b==1)
      return (number) 0;
    return (number) ( nrnModul / (NATNUMBER) b);
  }
  else
  {
    if ((NATNUMBER)b==0)
      return (number) 0;
    return (number) ((NATNUMBER) a / (NATNUMBER) b);
  }
}

number  nrnInvers (number c)
{
  if (XSGCD2(nrnModul, (NATNUMBER) c) != 1)
  {
    WerrorS("division by zero divisor");
    return (number)0;
  }
  return nrnInversM(c);
}

number nrnNeg (number c)
{
  if ((NATNUMBER)c==0) return c;
  return nrnNegM(c);
}

nMapFunc nrnSetMap(ring src, ring dst)
{
  return NULL;      /* default */
}


/*
 * set the exponent (allocate and init tables) (TODO)
 */

void nrnSetExp(int m, ring r)
{
  nrnModul = m;
//  PrintS("Modul: ");
//  Print("%d\n", nrnModul);
}

void nrnInitExp(int m, ring r)
{
  nrnModul = m;

  if (m < 2)
  {
    WarnS("nInitChar failed");
  }
}

#ifdef LDEBUG
BOOLEAN nrnDBTest (number a, char *f, int l)
{
  if (((NATNUMBER)a<0) || ((NATNUMBER)a>nrnModul))
  {
    return FALSE;
  }
  return TRUE;
}
#endif

void nrnWrite (number &a)
{
  if ((NATNUMBER)a > (nrnModul >>1)) StringAppend("-%d",(int)(nrnModul-((NATNUMBER)a)));
  else                          StringAppend("%d",(int)((NATNUMBER)a));
}

char* nrnEati(char *s, int *i)
{

  if (((*s) >= '0') && ((*s) <= '9'))
  {
    (*i) = 0;
    do
    {
      (*i) *= 10;
      (*i) += *s++ - '0';
      if ((*i) >= (MAX_INT_VAL / 10)) (*i) = (*i) % nrnModul;
    }
    while (((*s) >= '0') && ((*s) <= '9'));
    if ((*i) >= nrnModul) (*i) = (*i) % nrnModul;
  }
  else (*i) = 1;
  return s;
}

char * nrnRead (char *s, number *a)
{
  int z;
  int n=1;

  s = nrnEati(s, &z);
  if ((*s) == '/')
  {
    s++;
    s = nrnEati(s, &n);
  }
  if (n == 1)
    *a = (number)z;
  else
      *a = nrnDiv((number)z,(number)n);
  return s;
}
#endif
