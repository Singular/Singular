/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo p (<=32749)
*/

#include "misc/auxiliary.h"

#include "factory/factory.h"

#include "misc/mylimits.h"
#include "misc/sirandom.h"

#include "reporter/reporter.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"
#include "coeffs/mpr_complex.h"

#include "coeffs/longrat.h"
#include "coeffs/modulop.h"

#include <string.h>

static BOOLEAN npGreaterZero (number k, const coeffs r);
static BOOLEAN npIsMOne       (number a,const coeffs r);
static number  npDiv         (number a, number b,const coeffs r);
static number  npNeg         (number c,const coeffs r);
static number  npInvers      (number c,const coeffs r);
static BOOLEAN npGreater     (number a, number b,const coeffs r);
static BOOLEAN npEqual       (number a, number b,const coeffs r);
static void    npWrite       (number a, const coeffs r);
static const char *  npRead  (const char *s, number *a,const coeffs r);
static void nvInpMult(number &a, number b, const coeffs r);

#ifdef LDEBUG
static BOOLEAN npDBTest      (number a, const char *f, const int l, const coeffs r);
#endif

static nMapFunc npSetMap(const coeffs src, const coeffs dst);

#include "coeffs/modulop_inl.h" // npMult, npInit

#ifdef NV_OPS
static number  nvDiv         (number a, number b, const coeffs r);
number  nvInvers      (number c, const coeffs r);
//void    nvPower       (number a, int i, number * result, const coeffs r);
#endif

static BOOLEAN npGreaterZero (number k, const coeffs r)
{
  n_Test(k, r);

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


void npInpMult (number &a,number b, const coeffs r)
{
  n_Test(a, r);
  n_Test(b, r);

  if (((long)a == 0) || ((long)b == 0))
    a=(number)0;
  else
    a = npMultM(a,b, r);
  n_Test(a, r);
}

/*2
 * convert a number to an int in (-p/2 .. p/2]
 */
long npInt(number &n, const coeffs r)
{
  n_Test(n, r);

  if ((long)n > (((long)r->ch) >>1)) return ((long)n -((long)r->ch));
  else                               return ((long)n);
}

static BOOLEAN npIsMOne (number a, const coeffs r)
{
  n_Test(a, r);

  return ((r->npPminus1M == (long)a) &&(1L!=(long)a))/*for char 2*/;
}

static number npDiv (number a,number b, const coeffs r)
{
  n_Test(a, r);
  n_Test(b, r);

  if ((long)b==0L)
  {
    WerrorS(nDivBy0);
    return (number)0L;
  }
  if ((long)a==0) return (number)0L;

  number d;
#ifndef HAVE_GENERIC_MULT
  int s = r->npLogTable[(long)a] - r->npLogTable[(long)b];
  #ifdef HAVE_GENERIC_ADD
    if (s < 0)
      s += r->npPminus1M;
  #else
    #if SIZEOF_LONG == 8
    s += ((long)s >> 63) & r->npPminus1M;
    #else
    s += ((long)s >> 31) & r->npPminus1M;
    #endif
  #endif
  d = (number)(long)r->npExpTable[s];
#else
  number inv=npInversM(b,r);
  d = npMultM(a,inv,r);
#endif

  n_Test(d, r);
  return d;

}
static number  npInvers (number c, const coeffs r)
{
  n_Test(c, r);

  if ((long)c==0L)
  {
    WerrorS("1/0");
    return (number)0L;
  }
  number d = npInversM(c,r);

  n_Test(d, r);
  return d;
}

static number npNeg (number c, const coeffs r)
{
  n_Test(c, r);

  if ((long)c==0L) return c;

#if 0
  number d = npNegM(c,r);
  n_Test(d, r);
  return d;
#else
  c = npNegM(c,r);
  n_Test(c, r);
  return c;
#endif
}

static BOOLEAN npGreater (number a,number b, const coeffs r)
{
  n_Test(a, r);
  n_Test(b, r);

  //return (long)a != (long)b;
  return ((long)a) > ((long)b);
}

static BOOLEAN npEqual (number a,number b, const coeffs r)
{
  n_Test(a, r);
  n_Test(b, r);

//  return (long)a == (long)b;

  return npEqualM(a,b,r);
}

static void npWrite (number a, const coeffs r)
{
  n_Test(a, r);

  if ((long)a>(((long)r->ch) >>1)) StringAppend("-%d",(int)(((long)r->ch)-((long)a)));
  else                             StringAppend("%d",(int)((long)a));
}

#if 0
static void npPower (number a, int i, number * result, const coeffs r)
{
  n_Test(a, r);

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

static inline const char* npEati(const char *s, int *i, const coeffs r)
{
  return nEati((char *)s,i,(int)r->ch);
}

static const char * npRead (const char *s, number *a, const coeffs r)
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
    if ((z==0)&&(n==0))
    {
      WerrorS(nDivBy0);
      *a=(number)0L;
    }
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
  n_Test(*a, r);
  return s;
}

/*2
* set the charcteristic (allocate and init tables)
*/

void npKillChar(coeffs r)
{
  #ifdef HAVE_INVTABLE
  if (r->npInvTable!=NULL)
  {
    omFreeSize( (void *)r->npInvTable, r->ch*sizeof(unsigned short) );
    r->npInvTable=NULL;
  }
  #endif
  #ifndef HAVE_GENERIC_MULT
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
  return CanonicalForm(npInt( n,r ));
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

static char* npCoeffName(const coeffs cf)
{
  STATIC_VAR char npCoeffName_buf[15];
  snprintf(npCoeffName_buf,14,"ZZ/%d",cf->ch);
  return npCoeffName_buf;
}

static void npWriteFd(number n, const ssiInfo* d, const coeffs)
{
  fprintf(d->f_write,"%d ",(int)(long)n);
}

static number npReadFd(const ssiInfo *d, const coeffs)
{
  // read int
  int dd;
  dd=s_readint(d->f_read);
  return (number)(long)dd;
}

static number npRandom(siRandProc p, number, number, const coeffs cf)
{
  return npInit(p(),cf);
}


#ifndef HAVE_GENERIC_MULT
static number npPar(int, coeffs r)
{
  return (number)(long)r->npExpTable[1];
}
#endif

static number npInitMPZ(mpz_t m, const coeffs r)
{
  return (number)mpz_fdiv_ui(m, r->ch);
}

BOOLEAN npInitChar(coeffs r, void* p)
{
  assume( getCoeffType(r) == n_Zp );
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
  r->cfCoeffName=npCoeffName;

  r->cfMult  = npMult;
  r->cfInpMult  = npInpMult;
  r->cfSub   = npSubM;
  r->cfAdd   = npAddM;
  r->cfInpAdd   = npInpAddM;
  r->cfDiv   = npDiv;
  r->cfInit = npInit;
  //r->cfSize  = ndSize;
  r->cfInt  = npInt;
  r->cfInitMPZ = npInitMPZ;
  //r->cfDivComp = NULL; // only for ring stuff
  //r->cfIsUnit = NULL; // only for ring stuff
  //r->cfGetUnit = NULL; // only for ring stuff
  //r->cfExtGcd = NULL; // only for ring stuff
  // r->cfDivBy = NULL; // only for ring stuff
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
  //r->cfGetDenom = ndGetDenom;
  //r->cfGetNumerator = ndGetNumerator;
  //r->cfGcd  = ndGcd;
  //r->cfLcm  = ndGcd;
  //r->cfDelete= ndDelete;
  r->cfSetMap = npSetMap;
  //r->cfName = ndName;
  //r->cfInpMult=ndInpMult;
  r->convSingNFactoryN=npConvSingNFactoryN;
  r->convFactoryNSingN=npConvFactoryNSingN;
  r->cfRandom=npRandom;
#ifdef LDEBUG
  // debug stuff
  r->cfDBTest=npDBTest;
#endif

  // io via ssi
  r->cfWriteFd=npWriteFd;
  r->cfReadFd=npReadFd;

  // the variables:
  r->type = n_Zp;
  r->has_simple_Alloc=TRUE;
  r->has_simple_Inverse=TRUE;

  // the tables
#ifdef NV_OPS
  if (r->ch <=NV_MAX_PRIME)
#endif
  {
#ifdef HAVE_INVTABLE
    r->npInvTable=(unsigned short*)omAlloc0( r->ch*sizeof(unsigned short) );
#endif
#ifndef HAVE_GENERIC_MULT
    r->cfParameter=npPar; /* Singular.jl */
    r->npExpTable=(unsigned short *)omAlloc0( r->ch*sizeof(unsigned short) );
    r->npLogTable=(unsigned short *)omAlloc0( r->ch*sizeof(unsigned short) );
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
  }
#ifdef NV_OPS
  else /*if (c>NV_MAX_PRIME)*/
  {
    r->cfMult  = nvMult;
    r->cfDiv   = nvDiv;
    r->cfExactDiv = nvDiv;
    r->cfInvers  = nvInvers;
    r->cfInpMult = nvInpMult;
    //r->cfPower= nvPower;
    //if (c>FACTORY_MAX_PRIME) // factory will catch this error
    //{
    //  r->convSingNFactoryN=ndConvSingNFactoryN;
    //}
  }
#endif
  return FALSE;
}

#ifdef LDEBUG
static BOOLEAN npDBTest (number a, const char *f, const int l, const coeffs r)
{
  if (((long)a<0L) || ((long)a>(long)r->ch))
  {
    Print("wrong mod p number %ld at %s,%d\n",(long)a,f,l);
    return FALSE;
  }
  return TRUE;
}
#endif

static number npMapP(number from, const coeffs src, const coeffs dst_r)
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

/*2
* convert from a GMP integer
*/
static number npMapGMP(number from, const coeffs /*src*/, const coeffs dst)
{
  return (number)mpz_fdiv_ui((mpz_ptr) from, dst->ch);
}

static number npMapZ(number from, const coeffs src, const coeffs dst)
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
static number npMapMachineInt(number from, const coeffs /*src*/,const coeffs dst)
{
  long i = (long) (((unsigned long) from) % dst->ch);
  return (number) i;
}

static number npMapCanonicalForm (number a, const coeffs /*src*/, const coeffs dst)
{
  setCharacteristic (dst ->ch);
  CanonicalForm f= CanonicalForm ((InternalCF*)(a));
  return (number) (f.intval());
}

static nMapFunc npSetMap(const coeffs src, const coeffs)
{
  if ((src->rep==n_rep_int) && nCoeff_is_Ring_2toM(src))
  {
    return npMapMachineInt;
  }
  if (src->rep==n_rep_gmp) //nCoeff_is_Z(src) || nCoeff_is_Ring_PtoM(src) || nCoeff_is_Zn(src))
  {
    return npMapGMP;
  }
  if (src->rep==n_rep_gap_gmp) //nCoeff_is_Z(src)
  {
    return npMapZ;
  }
  if (src->rep==n_rep_gap_rat)  /* Q, Z */
  {
    return nlModP; // npMap0; // FIXME? TODO? // extern number nlModP(number q, const coeffs Q, const coeffs Zp); // Map q \in QQ \to Zp // FIXME!
  }
  if ((src->rep==n_rep_int) &&  nCoeff_is_Zp(src) )
  {
    return npMapP;
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
//  operation for very large primes (32749< p < 2^31-1)
// ----------------------------------------------------------
#ifdef NV_OPS
static void nvInpMult(number &a, number b, const coeffs r)
{
  number n=nvMult(a,b,r);
  a=n;
}

static inline number nvInversM (number c, const coeffs r)
{
  long inv=npInvMod((long)c,r);
  return (number)inv;
}

static number nvDiv (number a,number b, const coeffs r)
{
  if ((long)a==0L)
    return (number)0L;
  else if ((long)b==0L)
  {
    WerrorS(nDivBy0);
    return (number)0L;
  }
  else
  {
    number inv=nvInversM(b,r);
    return nvMult(a,inv,r);
  }
}
number  nvInvers (number c, const coeffs r)
{
  if ((long)c==0L)
  {
    WerrorS(nDivBy0);
    return (number)0L;
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
    *result = nvMult(a,*result,r);
  }
}
#endif
#endif

