/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers modulo p (<=32003)
*/

#include <string.h>
#include <kernel/mod2.h>
#include <omalloc/mylimits.h>
#include <kernel/structs.h>
#include <kernel/febase.h>
#include <omalloc/omalloc.h>
#include <kernel/numbers.h>
#include <kernel/longrat.h>
#include <kernel/mpr_complex.h>
#include <kernel/ring.h>
#ifdef HAVE_RINGS
#include <si_gmp.h>
#endif
#include <kernel/modulop.h>

long npPrimeM=0;
int npGen=0;
long npPminus1M=0;
long npMapPrime;

#ifdef HAVE_DIV_MOD
unsigned short *npInvTable=NULL;
#endif

#if !defined(HAVE_DIV_MOD) || !defined(HAVE_MULT_MOD)
unsigned short *npExpTable=NULL;
unsigned short *npLogTable=NULL;
#endif


BOOLEAN npGreaterZero (number k)
{
  int h = (int)((long) k);
  return ((int)h !=0) && (h <= (npPrimeM>>1));
}

//unsigned long npMultMod(unsigned long a, unsigned long b)
//{
//  unsigned long c = a*b;
//  c = c % npPrimeM;
//  assume(c == (unsigned long) npMultM((number) a, (number) b));
//  return c;
//}

number npMult (number a,number b)
{
  if (((long)a == 0) || ((long)b == 0))
    return (number)0;
  else
    return npMultM(a,b);
}

/*2
* create a number from int
*/
number npInit (int i, const ring r)
{
  long ii=i;
  while (ii <  0L)                         ii += (long)r->ch;
  while ((ii>1L) && (ii >= ((long)r->ch))) ii -= (long)r->ch;
  return (number)ii;
}

/*2
* convert a number to int (-p/2 .. p/2)
*/
int npInt(number &n, const ring r)
{
  if ((long)n > (((long)r->ch) >>1)) return (int)((long)n -((long)r->ch));
  else                               return (int)((long)n);
}

number npAdd (number a, number b)
{
  return npAddM(a,b);
}

number npSub (number a, number b)
{
  return npSubM(a,b);
}

BOOLEAN npIsZero (number  a)
{
  return 0 == (long)a;
}

BOOLEAN npIsOne (number a)
{
  return 1 == (long)a;
}

BOOLEAN npIsMOne (number a)
{
  return ((npPminus1M == (long)a)&&((long)1!=(long)a));
}

#ifdef HAVE_DIV_MOD
#ifdef USE_NTL_XGCD
//ifdef HAVE_NTL // in ntl.a
//extern void XGCD(long& d, long& s, long& t, long a, long b);
#include <NTL/ZZ.h>
#ifdef NTL_CLIENT
NTL_CLIENT
#endif
#endif

long InvMod(long a)
{
   long d, s, t;

#ifdef USE_NTL_XGCD
   XGCD(d, s, t, a, npPrimeM);
   assume (d == 1);
#else
   long  u, v, u0, v0, u1, v1, u2, v2, q, r;

   assume(a>0);
   u1=1; u2=0;
   u = a; v = npPrimeM;

   while (v != 0)
   {
      q = u / v;
      r = u % v;
      u = v;
      v = r;
      u0 = u2;
      u2 = u1 - q*u2;
      u1 = u0;
   }

   assume(u==1);
   s = u1;
#endif
   if (s < 0)
      return s + npPrimeM;
   else
      return s;
}
#endif

static inline number npInversM (number c)
{
#ifndef HAVE_DIV_MOD
  return (number)(long)npExpTable[npPminus1M - npLogTable[(long)c]];
#else
  long inv=(long)npInvTable[(long)c];
  if (inv==0)
  {
    inv=InvMod((long)c);
    npInvTable[(long)c]=inv;
  }
  return (number)inv;
#endif
}

number npDiv (number a,number b)
{
//#ifdef NV_OPS
//  if (npPrimeM>NV_MAX_PRIME)
//    return nvDiv(a,b);
//#endif
  if ((long)a==0)
    return (number)0;
#ifndef HAVE_DIV_MOD
  if ((long)b==0)
  {
    WerrorS(nDivBy0);
    return (number)0;
  }
  else
  {
    int s = npLogTable[(long)a] - npLogTable[(long)b];
    if (s < 0)
      s += npPminus1M;
    return (number)(long)npExpTable[s];
  }
#else
  number inv=npInversM(b);
  return npMultM(a,inv);
#endif
}
number  npInvers (number c)
{
  if ((long)c==0)
  {
    WerrorS("1/0");
    return (number)0;
  }
  return npInversM(c);
}

number npNeg (number c)
{
  if ((long)c==0) return c;
  return npNegM(c);
}

BOOLEAN npGreater (number a,number b)
{
  //return (long)a != (long)b;
  return (long)a > (long)b;
}

BOOLEAN npEqual (number a,number b)
{
//  return (long)a == (long)b;
  return npEqualM(a,b);
}

void npWrite (number &a, const ring r)
{
  if ((long)a>(((long)r->ch) >>1)) StringAppend("-%d",(int)(((long)r->ch)-((long)a)));
  else                             StringAppend("%d",(int)((long)a));
}

void npPower (number a, int i, number * result)
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
    npPower(a,i-1,result);
    *result = npMultM(a,*result);
  }
}

static const char* npEati(const char *s, int *i)
{

  if (((*s) >= '0') && ((*s) <= '9'))
  {
    (*i) = 0;
    do
    {
      (*i) *= 10;
      (*i) += *s++ - '0';
      if ((*i) >= (MAX_INT_VAL / 10)) (*i) = (*i) % npPrimeM;
    }
    while (((*s) >= '0') && ((*s) <= '9'));
    if ((*i) >= npPrimeM) (*i) = (*i) % npPrimeM;
  }
  else (*i) = 1;
  return s;
}

const char * npRead (const char *s, number *a)
{
  int z;
  int n=1;

  s = npEati(s, &z);
  if ((*s) == '/')
  {
    s++;
    s = npEati(s, &n);
  }
  if (n == 1)
    *a = (number)z;
  else
  {
    if ((z==0)&&(n==0)) WerrorS(nDivBy0);
    else
    {
#ifdef NV_OPS
      if (npPrimeM>NV_MAX_PRIME)
        *a = nvDiv((number)z,(number)n);
      else
#endif
        *a = npDiv((number)z,(number)n);
    }
  }
  return s;
}

/*2
* the last used charcteristic
*/
//int npGetChar()
//{
//  return npPrimeM;
//}

/*2
* set the charcteristic (allocate and init tables)
*/

void npSetChar(int c, ring r)
{

//  if (c==npPrimeM) return;
  if ((c>1) || (c<(-1)))
  {
    if (c>1) npPrimeM = c;
    else     npPrimeM = -c;
    npPminus1M = npPrimeM - 1;
#ifdef NV_OPS
    if (r->cf->npPrimeM >NV_MAX_PRIME) return;
#endif
#ifdef HAVE_DIV_MOD
    npInvTable=r->cf->npInvTable;
#endif
#if !defined(HAVE_DIV_MOD) || !defined(HAVE_MULT_MOD)
    npExpTable=r->cf->npExpTable;
    npLogTable=r->cf->npLogTable;
    npGen = npExpTable[1];
#endif
  }
  else
  {
    npPrimeM=0;
#ifdef HAVE_DIV_MOD
    npInvTable=NULL;
#endif
#if !defined(HAVE_DIV_MOD) || !defined(HAVE_MULT_MOD)
    npExpTable=NULL;
    npLogTable=NULL;
#endif
  }
}

void npInitChar(int c, ring r)
{
  int i, w;

  if ((c>1) || (c<(-1)))
  {
    if (c>1) r->cf->npPrimeM = c;
    else     r->cf->npPrimeM = -c;
    r->cf->npPminus1M = r->cf->npPrimeM - 1;
#ifdef NV_OPS
    if (r->cf->npPrimeM <=NV_MAX_PRIME)
#endif
    {
#if !defined(HAVE_DIV_MOD) || !defined(HAVE_MULT_MOD)
      r->cf->npExpTable=(unsigned short *)omAlloc( r->cf->npPrimeM*sizeof(unsigned short) );
      r->cf->npLogTable=(unsigned short *)omAlloc( r->cf->npPrimeM*sizeof(unsigned short) );
      r->cf->npExpTable[0] = 1;
      r->cf->npLogTable[0] = 0;
      if (r->cf->npPrimeM > 2)
      {
        w = 1;
        loop
        {
          r->cf->npLogTable[1] = 0;
          w++;
          i = 0;
          loop
          {
            i++;
            r->cf->npExpTable[i] =(int)(((long)w * (long)r->cf->npExpTable[i-1])
                                 % r->cf->npPrimeM);
            r->cf->npLogTable[r->cf->npExpTable[i]] = i;
            if (/*(i == npPrimeM - 1 ) ||*/ (r->cf->npExpTable[i] == 1))
              break;
          }
          if (i == r->cf->npPrimeM - 1)
            break;
        }
      }
      else
      {
        r->cf->npExpTable[1] = 1;
        r->cf->npLogTable[1] = 0;
      }
#endif
#ifdef HAVE_DIV_MOD
      r->cf->npInvTable=(unsigned short*)omAlloc0( r->cf->npPrimeM*sizeof(unsigned short) );
#endif
    }
  }
  else
  {
    WarnS("nInitChar failed");
  }
}

#ifdef LDEBUG
BOOLEAN npDBTest (number a, const char *f, const int l)
{
  if (((long)a<0) || ((long)a>npPrimeM))
  {
    Print("wrong mod p number %ld at %s,%d\n",(long)a,f,l);
    return FALSE;
  }
  return TRUE;
}
#endif

number npMap0(number from)
{
  return npInit(nlModP(from,npPrimeM),currRing);
}

number npMapP(number from)
{
  long i = (long)from;
  if (i>npMapPrime/2)
  {
    i-=npMapPrime;
    while (i < 0) i+=npPrimeM;
  }
  i%=npPrimeM;
  return (number)i;
}

static number npMapLongR(number from)
{
  gmp_float *ff=(gmp_float*)from;
  mpf_t *f=ff->_mpfp();
  number res;
  mpz_ptr dest,ndest;
  int size,i;
  int e,al,bl,in;
  long iz;
  mp_ptr qp,dd,nn;

  size = (*f)[0]._mp_size;
  if (size == 0)
    return npInit(0,currRing);
  if(size<0)
    size = -size;

  qp = (*f)[0]._mp_d;
  while(qp[0]==0)
  {
    qp++;
    size--;
  }

  if(npPrimeM>2)
    e=(*f)[0]._mp_exp-size;
  else
    e=0;
  res = (number)omAllocBin(rnumber_bin);
#if defined(LDEBUG)
  res->debug=123456;
#endif
  dest = res->z;

  if (e<0)
  {
    al = dest->_mp_size = size;
    if (al<2) al = 2;
    dd = (mp_ptr)omAlloc(sizeof(mp_limb_t)*al);
    for (i=0;i<size;i++) dd[i] = qp[i];
    bl = 1-e;
    nn = (mp_ptr)omAlloc(sizeof(mp_limb_t)*bl);
    nn[bl-1] = 1;
    for (i=bl-2;i>=0;i--) nn[i] = 0;
    ndest = res->n;
    ndest->_mp_d = nn;
    ndest->_mp_alloc = ndest->_mp_size = bl;
    res->s = 0;
    in=mpz_fdiv_ui(ndest,npPrimeM);
    mpz_clear(ndest);
  }
  else
  {
    al = dest->_mp_size = size+e;
    if (al<2) al = 2;
    dd = (mp_ptr)omAlloc(sizeof(mp_limb_t)*al);
    for (i=0;i<size;i++) dd[i+e] = qp[i];
    for (i=0;i<e;i++) dd[i] = 0;
    res->s = 3;
  }

  dest->_mp_d = dd;
  dest->_mp_alloc = al;
  iz=mpz_fdiv_ui(dest,npPrimeM);
  mpz_clear(dest);
  omFreeBin((ADDRESS)res, rnumber_bin);
  if(res->s==0)
    iz=(long)npDiv((number)iz,(number)in);
  return (number)iz;
}

#ifdef HAVE_RINGS
/*2
* convert from a GMP integer
*/
number npMapGMP(number from)
{
  int_number erg = (int_number) omAlloc(sizeof(mpz_t)); // evtl. spaeter mit bin
  mpz_init(erg);

  mpz_mod_ui(erg, (int_number) from, npPrimeM);
  number r = (number) mpz_get_si(erg);

  mpz_clear(erg);
  omFree((ADDRESS) erg);
  return (number) r;
}

/*2
* convert from an machine long
*/
number npMapMachineInt(number from)
{
  long i = (long) (((unsigned long) from) % npPrimeM);
  return (number) i;
}
#endif

nMapFunc npSetMap(const ring src, const ring dst)
{
#ifdef HAVE_RINGS
  if (rField_is_Ring_2toM(src))
  {
    return npMapMachineInt;
  }
  if (rField_is_Ring_Z(src) || rField_is_Ring_PtoM(src) || rField_is_Ring_ModN(src))
  {
    return npMapGMP;
  }
#endif
  if (rField_is_Q(src))
  {
    return npMap0;
  }
  if ( rField_is_Zp(src) )
  {
    if (rChar(src) == rChar(dst))
    {
      return ndCopy;
    }
    else
    {
      npMapPrime=rChar(src);
      return npMapP;
    }
  }
  if (rField_is_long_R(src))
  {
    return npMapLongR;
  }
  return NULL;      /* default */
}

// -----------------------------------------------------------
//  operation for very large primes (32003< p < 2^31-1)
// ----------------------------------------------------------
#ifdef NV_OPS

number nvMult (number a,number b)
{
  //if (((long)a == 0) || ((long)b == 0))
  //  return (number)0;
  //else
    return nvMultM(a,b);
}

void   nvInpMult(number &a, number b, const ring r)
{
  number n=nvMultM(a,b);
  a=n;
}


long nvInvMod(long a)
{
   long  s, t;

   long  u, v, u0, v0, u1, v1, u2, v2, q, r;

   u1=1; v1=0;
   u2=0; v2=1;
   u = a; v = npPrimeM;

   while (v != 0)
   {
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

   s = u1;
   //t = v1;
   if (s < 0)
      return s + npPrimeM;
   else
      return s;
}

static inline number nvInversM (number c)
{
  long inv=nvInvMod((long)c);
  return (number)inv;
}

number nvDiv (number a,number b)
{
  if ((long)a==0)
    return (number)0;
  else if ((long)b==0)
  {
    WerrorS(nDivBy0);
    return (number)0;
  }
  else
  {
    number inv=nvInversM(b);
    return nvMultM(a,inv);
  }
}
number  nvInvers (number c)
{
  if ((long)c==0)
  {
    WerrorS(nDivBy0);
    return (number)0;
  }
  return nvInversM(c);
}
void nvPower (number a, int i, number * result)
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
    nvPower(a,i-1,result);
    *result = nvMultM(a,*result);
  }
}
#endif
