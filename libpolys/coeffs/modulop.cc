/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo p (<=32003)
*/




#include <misc/auxiliary.h>

#include <factory/factory.h>

#include <string.h>
#include <omalloc/omalloc.h>
#include <coeffs/coeffs.h>
#include <reporter/reporter.h>
#include <coeffs/numbers.h>
#include <coeffs/longrat.h>
#include <coeffs/mpr_complex.h>
#include <misc/mylimits.h>
#include <misc/sirandom.h>
#include <coeffs/modulop.h>

// int npGen=0;

/// Our Type!
static const n_coeffType ID = n_Zp;

#ifdef NV_OPS
#pragma GCC diagnostic ignored "-Wlong-long"
static inline number nvMultM(number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == ID );

#if SIZEOF_LONG == 4
#define ULONG64 (unsigned long long)(unsigned long)
#else
#define ULONG64 (unsigned long)
#endif
  return (number)
      (unsigned long)((ULONG64 a)*(ULONG64 b) % (ULONG64 r->ch));
}
number  nvMult        (number a, number b, const coeffs r);
number  nvDiv         (number a, number b, const coeffs r);
number  nvInvers      (number c, const coeffs r);
//void    nvPower       (number a, int i, number * result, const coeffs r);
#endif




BOOLEAN npGreaterZero (number k, const coeffs r)
{
  assume( n_Test(k, r) );

  int h = (int)((long) k);
  return ((int)h !=0) && (h <= (r->ch>>1));
}

//unsigned long npMultMod(unsigned long a, unsigned long b, int npPrimeM)
//{
//  unsigned long c = a*b;
//  c = c % npPrimeM;
//  assume(c == (unsigned long) npMultM((number) a, (number) b, npPrimeM));
//  return c;
//}

number npMult (number a,number b, const coeffs r)
{
  assume( n_Test(a, r) );
  assume( n_Test(b, r) );

  if (((long)a == 0) || ((long)b == 0))
    return (number)0;
  number c = npMultM(a,b, r);
  assume( n_Test(c, r) );
  return c;
}

/*2
* create a number from int
*/
number npInit (long i, const coeffs r)
{
  long ii=i % (long)r->ch;
  if (ii <  0L)                         ii += (long)r->ch;

  number c = (number)ii;
  assume( n_Test(c, r) );
  return c;

}


/*2
 * convert a number to an int in (-p/2 .. p/2]
 */
int npInt(number &n, const coeffs r)
{
  assume( n_Test(n, r) );

  if ((long)n > (((long)r->ch) >>1)) return (int)((long)n -((long)r->ch));
  else                               return (int)((long)n);
}

number npAdd (number a, number b, const coeffs r)
{
  assume( n_Test(a, r) );
  assume( n_Test(b, r) );

  number c = npAddM(a,b, r);

  assume( n_Test(c, r) );

  return c;
}

number npSub (number a, number b, const coeffs r)
{
  assume( n_Test(a, r) );
  assume( n_Test(b, r) );

  number c = npSubM(a,b,r);

  assume( n_Test(c, r) );

  return c;
}

BOOLEAN npIsZero (number  a, const coeffs r)
{
  assume( n_Test(a, r) );

  return 0 == (long)a;
}

BOOLEAN npIsOne (number a, const coeffs r)
{
  assume( n_Test(a, r) );

  return 1 == (long)a;
}

BOOLEAN npIsMOne (number a, const coeffs r)
{
  assume( n_Test(a, r) );

  return ((r->npPminus1M == (long)a)&&((long)1!=(long)a));
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

long InvMod(long a, const coeffs R)
{
   long d, s, t;

#ifdef USE_NTL_XGCD
   XGCD(d, s, t, a, R->ch);
   assume (d == 1);
#else
   long  u, v, u0, v0, u1, v1, u2, v2, q, r;

   assume(a>0);
   u1=1; u2=0;
   u = a; v = R->ch;

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
      return s + R->ch;
   else
      return s;
}
#endif

inline number npInversM (number c, const coeffs r)
{
  assume( n_Test(c, r) );
#ifndef HAVE_DIV_MOD
  number d = (number)(long)r->npExpTable[r->npPminus1M - r->npLogTable[(long)c]];
#else
  long inv=(long)r->npInvTable[(long)c];
  if (inv==0)
  {
    inv=InvMod((long)c,r);
    r->npInvTable[(long)c]=inv;
  }
  number d = (number)inv;
#endif
  assume( n_Test(d, r) );
  return d;

}

number npDiv (number a,number b, const coeffs r)
{
  assume( n_Test(a, r) );
  assume( n_Test(b, r) );

//#ifdef NV_OPS
//  if (r->ch>NV_MAX_PRIME)
//    return nvDiv(a,b);
//#endif
  if ((long)a==0)
    return (number)0;
  number d;

#ifndef HAVE_DIV_MOD
  if ((long)b==0)
  {
    WerrorS(nDivBy0);
    return (number)0;
  }

  int s = r->npLogTable[(long)a] - r->npLogTable[(long)b];
  if (s < 0)
    s += r->npPminus1M;
  d = (number)(long)r->npExpTable[s];
#else
  number inv=npInversM(b,r);
  d = npMultM(a,inv,r);
#endif

  assume( n_Test(d, r) );
  return d;

}
number  npInvers (number c, const coeffs r)
{
  assume( n_Test(c, r) );

  if ((long)c==0)
  {
    WerrorS("1/0");
    return (number)0;
  }
  number d = npInversM(c,r);

  assume( n_Test(d, r) );
  return d;

}

number npNeg (number c, const coeffs r)
{
  assume( n_Test(c, r) );

  if ((long)c==0) return c;

#if 0
  number d = npNegM(c,r);
  assume( n_Test(d, r) );
  return d;
#else
  c = npNegM(c,r);
  assume( n_Test(c, r) );
  return c;
#endif
}

BOOLEAN npGreater (number a,number b, const coeffs r)
{
  assume( n_Test(a, r) );
  assume( n_Test(b, r) );

  //return (long)a != (long)b;
  return (long)a > (long)b;
}

BOOLEAN npEqual (number a,number b, const coeffs r)
{
  assume( n_Test(a, r) );
  assume( n_Test(b, r) );

//  return (long)a == (long)b;

  return npEqualM(a,b,r);
}

void npWrite (number &a, const coeffs r)
{
  assume( n_Test(a, r) );

  if ((long)a>(((long)r->ch) >>1)) StringAppend("-%d",(int)(((long)r->ch)-((long)a)));
  else                             StringAppend("%d",(int)((long)a));
}

#if 0
void npPower (number a, int i, number * result, const coeffs r)
{
  assume( n_Test(a, r) );

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
    npPower(a,i-1,result,r);
    *result = npMultM(a,*result,r);
  }
}
#endif

static const char* npEati(const char *s, int *i, const coeffs r)
{

  if (((*s) >= '0') && ((*s) <= '9'))
  {
    unsigned long ii=0L;
    do
    {
      ii *= 10;
      ii += *s++ - '0';
      if (ii >= (MAX_INT_VAL / 10)) ii = ii % r->ch;
    }
    while (((*s) >= '0') && ((*s) <= '9'));
    if (ii >= (unsigned long)r->ch) ii = ii % r->ch;
    *i=(int)ii;
  }
  else (*i) = 1;
  return s;
}

const char * npRead (const char *s, number *a, const coeffs r)
{
  int z;
  int n=1;

  s = npEati(s, &z, r);
  if ((*s) == '/')
  {
    s++;
    s = npEati(s, &n, r);
  }
  if (n == 1)
    *a = (number)(long)z;
  else
  {
    if ((z==0)&&(n==0)) WerrorS(nDivBy0);
    else
    {
#ifdef NV_OPS
      if (r->ch>NV_MAX_PRIME)
        *a = nvDiv((number)(long)z,(number)(long)n,r);
      else
#endif
        *a = npDiv((number)(long)z,(number)(long)n,r);
    }
  }
  assume( n_Test(*a, r) );
  return s;
}

/*2
* set the charcteristic (allocate and init tables)
*/

void npKillChar(coeffs r)
{
  #ifdef HAVE_DIV_MOD
  if (r->npInvTable!=NULL)
  omFreeSize( (void *)r->npInvTable, r->ch*sizeof(unsigned short) );
  r->npInvTable=NULL;
  #else
  if (r->npExpTable!=NULL)
  {
    omFreeSize( (void *)r->npExpTable, r->ch*sizeof(unsigned short) );
    omFreeSize( (void *)r->npLogTable, r->ch*sizeof(unsigned short) );
    r->npExpTable=NULL; r->npLogTable=NULL;
  }
  #endif
}

static BOOLEAN npCoeffsEqual(const coeffs r, n_coeffType n, void * parameter)
{
  /* test, if r is an instance of nInitCoeffs(n,parameter) */
  return (n==n_Zp) && (r->ch==(int)(long)parameter);
}
CanonicalForm npConvSingNFactoryN( number n, BOOLEAN setChar, const coeffs r )
{
  if (setChar) setCharacteristic( r->ch );
  CanonicalForm term(npInt( n,r ));
  return term;
}

number npConvFactoryNSingN( const CanonicalForm n, const coeffs r)
{
  if (n.isImm())
  {
    return npInit(n.intval(),r);
  }
  else
  {
    assume(0);
    return NULL;
  }
}

static char* npCoeffString(const coeffs r)
{
  char *s=(char*)omAlloc(11);
  snprintf(s,11,"%d",r->ch);
  return s;
}

static void npWriteFd(number n, FILE* f, const coeffs r)
{
  fprintf(f,"%d ",(int)(long)n);
}

static number npReadFd(s_buff f, const coeffs r)
{
  // read int
  int dd;
  dd=s_readint(f);
  return (number)(long)dd;
}

BOOLEAN npInitChar(coeffs r, void* p)
{
  assume( getCoeffType(r) == ID );
  const int c = (int) (long) p;

  assume( c > 0 );

  int i, w;

  r->is_field=TRUE;
  r->is_domain=TRUE;
  r->rep=n_rep_int;

  r->ch = c;
  r->npPminus1M = c /*r->ch*/ - 1;

  //r->cfInitChar=npInitChar;
  r->cfKillChar=npKillChar;
  r->nCoeffIsEqual=npCoeffsEqual;
  r->cfCoeffString=npCoeffString;

  r->cfMult  = npMult;
  r->cfSub   = npSub;
  r->cfAdd   = npAdd;
  r->cfDiv   = npDiv;
  r->cfInit = npInit;
  //r->cfSize  = ndSize;
  r->cfInt  = npInt;
  #ifdef HAVE_RINGS
  //r->cfDivComp = NULL; // only for ring stuff
  //r->cfIsUnit = NULL; // only for ring stuff
  //r->cfGetUnit = NULL; // only for ring stuff
  //r->cfExtGcd = NULL; // only for ring stuff
  // r->cfDivBy = NULL; // only for ring stuff
  #endif
  r->cfInpNeg   = npNeg;
  r->cfInvers= npInvers;
  //r->cfCopy  = ndCopy;
  //r->cfRePart = ndCopy;
  //r->cfImPart = ndReturn0;
  r->cfWriteLong = npWrite;
  r->cfRead = npRead;
  //r->cfNormalize=ndNormalize;
  r->cfGreater = npGreater;
  r->cfEqual = npEqual;
  r->cfIsZero = npIsZero;
  r->cfIsOne = npIsOne;
  r->cfIsMOne = npIsMOne;
  r->cfGreaterZero = npGreaterZero;
  //r->cfPower = npPower;
  r->cfGetDenom = ndGetDenom;
  r->cfGetNumerator = ndGetNumerator;
  //r->cfGcd  = ndGcd;
  //r->cfLcm  = ndGcd;
  //r->cfDelete= ndDelete;
  r->cfSetMap = npSetMap;
  //r->cfName = ndName;
  r->cfInpMult=ndInpMult;
#ifdef NV_OPS
  if (c>NV_MAX_PRIME)
  {
    r->cfMult  = nvMult;
    r->cfDiv   = nvDiv;
    r->cfExactDiv= nvDiv;
    r->cfInvers= nvInvers;
    //r->cfPower= nvPower;
  }
#endif
  r->cfCoeffWrite=npCoeffWrite;
#ifdef LDEBUG
  // debug stuff
  r->cfDBTest=npDBTest;
#endif

  r->convSingNFactoryN=npConvSingNFactoryN;
  r->convFactoryNSingN=npConvFactoryNSingN;

  // io via ssi
  r->cfWriteFd=npWriteFd;
  r->cfReadFd=npReadFd;

  // the variables:
  r->nNULL = (number)0;
  r->type = n_Zp;
  r->ch = c;
  r->has_simple_Alloc=TRUE;
  r->has_simple_Inverse=TRUE;

  // the tables
#ifdef NV_OPS
  if (r->ch <=NV_MAX_PRIME)
#endif
  {
#if !defined(HAVE_DIV_MOD) || !defined(HAVE_MULT_MOD)
    r->npExpTable=(unsigned short *)omAlloc( r->ch*sizeof(unsigned short) );
    r->npLogTable=(unsigned short *)omAlloc( r->ch*sizeof(unsigned short) );
    r->npExpTable[0] = 1;
    r->npLogTable[0] = 0;
    if (r->ch > 2)
    {
      w = 1;
      loop
      {
        r->npLogTable[1] = 0;
        w++;
        i = 0;
        loop
        {
          i++;
          r->npExpTable[i] =(int)(((long)w * (long)r->npExpTable[i-1]) % r->ch);
          r->npLogTable[r->npExpTable[i]] = i;
          if /*(i == r->ch - 1 ) ||*/ (/*(*/ r->npExpTable[i] == 1 /*)*/)
            break;
        }
        if (i == r->ch - 1)
          break;
      }
    }
    else
    {
      r->npExpTable[1] = 1;
      r->npLogTable[1] = 0;
    }
#endif
#ifdef HAVE_DIV_MOD
    r->npInvTable=(unsigned short*)omAlloc0( r->ch*sizeof(unsigned short) );
#endif
  }
  return FALSE;
}

#ifdef LDEBUG
BOOLEAN npDBTest (number a, const char *f, const int l, const coeffs r)
{
  if (((long)a<0) || ((long)a>r->ch))
  {
    Print("wrong mod p number %ld at %s,%d\n",(long)a,f,l);
    return FALSE;
  }
  return TRUE;
}
#endif

number npMapP(number from, const coeffs src, const coeffs dst_r)
{
  long i = (long)from;
  if (i>src->ch/2)
  {
    i-=src->ch;
    while (i < 0) i+=dst_r->ch;
  }
  i%=dst_r->ch;
  return (number)i;
}

static number npMapLongR(number from, const coeffs /*src*/, const coeffs dst_r)
{
  gmp_float *ff=(gmp_float*)from;
  mpf_t *f=ff->_mpfp();
  number res;
  mpz_ptr dest,ndest;
  int size,i;
  int e,al,bl;
  long iz;
  mp_ptr qp,dd,nn;

  size = (*f)[0]._mp_size;
  if (size == 0)
    return npInit(0,dst_r);
  if(size<0)
    size = -size;

  qp = (*f)[0]._mp_d;
  while(qp[0]==0)
  {
    qp++;
    size--;
  }

  if(dst_r->ch>2)
    e=(*f)[0]._mp_exp-size;
  else
    e=0;
  res = ALLOC_RNUMBER();
#if defined(LDEBUG)
  res->debug=123456;
#endif
  dest = res->z;

  long in=0;
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
    in=mpz_fdiv_ui(ndest,dst_r->ch);
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
  iz=mpz_fdiv_ui(dest,dst_r->ch);
  mpz_clear(dest);
  if(res->s==0)
    iz=(long)npDiv((number)iz,(number)in,dst_r);
  FREE_RNUMBER(res); // Q!?
  return (number)iz;
}

#ifdef HAVE_RINGS
/*2
* convert from a GMP integer
*/
number npMapGMP(number from, const coeffs /*src*/, const coeffs dst)
{
  int_number erg = (int_number) omAlloc(sizeof(mpz_t)); // evtl. spaeter mit bin
  mpz_init(erg);

  mpz_mod_ui(erg, (int_number) from, dst->ch);
  number r = (number) mpz_get_si(erg);

  mpz_clear(erg);
  omFree((void *) erg);
  return (number) r;
}

number npMapZ(number from, const coeffs src, const coeffs dst)
{
  if (SR_HDL(from) & SR_INT)
  {
    long f_i=SR_TO_INT(from);
    return npInit(f_i,dst);
  }
  return npMapGMP(from,src,dst);
}

/*2
* convert from an machine long
*/
number npMapMachineInt(number from, const coeffs /*src*/,const coeffs dst)
{
  long i = (long) (((unsigned long) from) % dst->ch);
  return (number) i;
}
#endif

number npMapCanonicalForm (number a, const coeffs /*src*/, const coeffs dst)
{
  setCharacteristic (dst ->ch);
  CanonicalForm f= CanonicalForm ((InternalCF*)(a));
  return (number) (f.intval());
}

nMapFunc npSetMap(const coeffs src, const coeffs dst)
{
#ifdef HAVE_RINGS
  if ((src->rep==n_rep_int) && nCoeff_is_Ring_2toM(src))
  {
    return npMapMachineInt;
  }
  if (src->rep==n_rep_gmp) //nCoeff_is_Ring_Z(src) || nCoeff_is_Ring_PtoM(src) || nCoeff_is_Ring_ModN(src))
  {
    return npMapGMP;
  }
  if (src->rep==n_rep_gap_gmp) //nCoeff_is_Ring_Z(src)
  {
    return npMapZ;
  }
#endif
  if (src->rep==n_rep_gap_rat)  /* Q, Z */
  {
    return nlModP; // npMap0;
  }
  if ((src->rep==n_rep_int) &&  nCoeff_is_Zp(src) )
  {
    if (n_GetChar(src) == n_GetChar(dst))
    {
      return ndCopyMap;
    }
    else
    {
      return npMapP;
    }
  }
  if ((src->rep==n_rep_gmp_float) && nCoeff_is_long_R(src))
  {
    return npMapLongR;
  }
  if (nCoeff_is_CF (src))
  {
    return npMapCanonicalForm;
  }
  return NULL;      /* default */
}

// -----------------------------------------------------------
//  operation for very large primes (32003< p < 2^31-1)
// ----------------------------------------------------------
#ifdef NV_OPS

number nvMult (number a,number b, const coeffs r)
{
  //if (((long)a == 0) || ((long)b == 0))
  //  return (number)0;
  //else
    return nvMultM(a,b,r);
}

void   nvInpMult(number &a, number b, const coeffs r)
{
  number n=nvMultM(a,b,r);
  a=n;
}


inline long nvInvMod(long a, const coeffs R)
{
#ifdef HAVE_DIV_MOD
  return InvMod(a, R);
#else
/// TODO: use "long InvMod(long a, const coeffs R)"?!

   long  s;

   long  u, u0, u1, u2, q, r; // v0, v1, v2,

   u1=1; // v1=0;
   u2=0; // v2=1;
   u = a;

   long v = R->ch;

   while (v != 0)
   {
      q = u / v;
      r = u % v;
      u = v;
      v = r;
      u0 = u2;
//      v0 = v2;
      u2 = u1 - q*u2;
//      v2 = v1 - q*v2;
      u1 = u0;
//      v1 = v0;
   }

   s = u1;
   //t = v1;
   if (s < 0)
      return s + R->ch;
   else
     return s;
#endif
}

inline number nvInversM (number c, const coeffs r)
{
  long inv=nvInvMod((long)c,r);
  return (number)inv;
}

number nvDiv (number a,number b, const coeffs r)
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
    number inv=nvInversM(b,r);
    return nvMultM(a,inv,r);
  }
}
number  nvInvers (number c, const coeffs r)
{
  if ((long)c==0)
  {
    WerrorS(nDivBy0);
    return (number)0;
  }
  return nvInversM(c,r);
}
#if 0
void nvPower (number a, int i, number * result, const coeffs r)
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
    nvPower(a,i-1,result,r);
    *result = nvMultM(a,*result,r);
  }
}
#endif
#endif

void    npCoeffWrite  (const coeffs r, BOOLEAN /*details*/)
{
  Print("//   characteristic : %d\n",r->ch);
}

