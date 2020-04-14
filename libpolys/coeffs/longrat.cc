/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: computation with long rational numbers (Hubert Grassmann)
*/

#include "misc/auxiliary.h"

#include "factory/factory.h"

#include "misc/sirandom.h"
#include "misc/prime.h"
#include "reporter/reporter.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"
#include "coeffs/rmodulon.h" // ZnmInfo
#include "coeffs/longrat.h"
#include "coeffs/shortfl.h"
#include "coeffs/modulop.h"
#include "coeffs/mpr_complex.h"

#include <string.h>
#include <float.h>

// allow inlining only from p_Numbers.h and if ! LDEBUG
#if defined(DO_LINLINE) && defined(P_NUMBERS_H) && !defined(LDEBUG)
#define LINLINE static FORCE_INLINE
#else
#define LINLINE
#undef DO_LINLINE
#endif // DO_LINLINE

LINLINE BOOLEAN  nlEqual(number a, number b, const coeffs r);
LINLINE number   nlInit(long i, const coeffs r);
LINLINE BOOLEAN  nlIsOne(number a, const coeffs r);
LINLINE BOOLEAN  nlIsZero(number za, const coeffs r);
LINLINE number   nlCopy(number a, const coeffs r);
LINLINE number   nl_Copy(number a, const coeffs r);
LINLINE void     nlDelete(number *a, const coeffs r);
LINLINE number   nlNeg(number za, const coeffs r);
LINLINE number   nlAdd(number la, number li, const coeffs r);
LINLINE number   nlSub(number la, number li, const coeffs r);
LINLINE number   nlMult(number a, number b, const coeffs r);
LINLINE void nlInpAdd(number &a, number b, const coeffs r);
LINLINE void nlInpMult(number &a, number b, const coeffs r);

number nlRInit (long i);


// number nlInitMPZ(mpz_t m, const coeffs r);
// void nlMPZ(mpz_t m, number &n, const coeffs r);

void     nlNormalize(number &x, const coeffs r);

number   nlGcd(number a, number b, const coeffs r);
number nlExtGcd(number a, number b, number *s, number *t, const coeffs);
number   nlNormalizeHelper(number a, number b, const coeffs r);   /*special routine !*/
BOOLEAN  nlGreater(number a, number b, const coeffs r);
BOOLEAN  nlIsMOne(number a, const coeffs r);
long     nlInt(number &n, const coeffs r);
number   nlBigInt(number &n);

#ifdef HAVE_RINGS
number nlMapGMP(number from, const coeffs src, const coeffs dst);
#endif

BOOLEAN  nlGreaterZero(number za, const coeffs r);
number   nlInvers(number a, const coeffs r);
number   nlDiv(number a, number b, const coeffs r);
number   nlExactDiv(number a, number b, const coeffs r);
number   nlIntDiv(number a, number b, const coeffs r);
number   nlIntMod(number a, number b, const coeffs r);
void     nlPower(number x, int exp, number *lu, const coeffs r);
const char *   nlRead (const char *s, number *a, const coeffs r);
void     nlWrite(number a, const coeffs r);

void     nlCoeffWrite(const coeffs r, BOOLEAN details);
number   nlFarey(number nN, number nP, const coeffs CF);

#ifdef LDEBUG
BOOLEAN  nlDBTest(number a, const char *f, const int l);
#endif

nMapFunc nlSetMap(const coeffs src, const coeffs dst);

// in-place operations
void nlInpIntDiv(number &a, number b, const coeffs r);

#ifdef LDEBUG
#define nlTest(a, r) nlDBTest(a,__FILE__,__LINE__, r)
BOOLEAN nlDBTest(number a, const char *f,int l, const coeffs r);
#else
#define nlTest(a, r) do {} while (0)
#endif


// 64 bit version:
//#if SIZEOF_LONG == 8
#if 0
#define MAX_NUM_SIZE 60
#define POW_2_28 (1L<<60)
#define POW_2_28_32 (1L<<28)
#define LONG long
#else
#define MAX_NUM_SIZE 28
#define POW_2_28 (1L<<28)
#define POW_2_28_32 (1L<<28)
#define LONG int
#endif


static inline number nlShort3(number x) // assume x->s==3
{
  assume(x->s==3);
  if (mpz_sgn1(x->z)==0)
  {
    mpz_clear(x->z);
    FREE_RNUMBER(x);
    return INT_TO_SR(0);
  }
  if (mpz_size1(x->z)<=MP_SMALL)
  {
    LONG ui=mpz_get_si(x->z);
    if ((((ui<<3)>>3)==ui)
    && (mpz_cmp_si(x->z,(long)ui)==0))
    {
      mpz_clear(x->z);
      FREE_RNUMBER(x);
      return INT_TO_SR(ui);
    }
  }
  return x;
}

#ifndef LONGRAT_CC
#define LONGRAT_CC

#ifndef BYTES_PER_MP_LIMB
#define BYTES_PER_MP_LIMB sizeof(mp_limb_t)
#endif

//#define SR_HDL(A) ((long)(A))
/*#define SR_INT    1L*/
/*#define INT_TO_SR(INT)  ((number) (((long)INT << 2) + SR_INT))*/
// #define SR_TO_INT(SR)   (((long)SR) >> 2)

#define MP_SMALL 1
//#define mpz_isNeg(A) (mpz_sgn1(A)<0)
#define mpz_isNeg(A) ((A)->_mp_size<0)
#define mpz_limb_size(A) ((A)->_mp_size)
#define mpz_limb_d(A) ((A)->_mp_d)

void    _nlDelete_NoImm(number *a);

/***************************************************************
 *
 * Routines which are never inlined by p_Numbers.h
 *
 *******************************************************************/
#ifndef P_NUMBERS_H

number nlShort3_noinline(number x) // assume x->s==3
{
  return nlShort3(x);
}


#if (__GNU_MP_VERSION*10+__GNU_MP_VERSION_MINOR < 31)
void mpz_mul_si (mpz_ptr r, mpz_srcptr s, long int si)
{
  if (si>=0)
    mpz_mul_ui(r,s,si);
  else
  {
    mpz_mul_ui(r,s,-si);
    mpz_neg(r,r);
  }
}
#endif

static number nlMapP(number from, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(src) == n_Zp );

  number to = nlInit(npInt(from,src), dst); // FIXME? TODO? // extern long     npInt         (number &n, const coeffs r);

  return to;
}

static number nlMapLongR(number from, const coeffs src, const coeffs dst);
static number nlMapR(number from, const coeffs src, const coeffs dst);


#ifdef HAVE_RINGS
/*2
* convert from a GMP integer
*/
number nlMapGMP(number from, const coeffs /*src*/, const coeffs /*dst*/)
{
  number z=ALLOC_RNUMBER();
#if defined(LDEBUG)
  z->debug=123456;
#endif
  mpz_init_set(z->z,(mpz_ptr) from);
  z->s = 3;
  z=nlShort3(z);
  return z;
}

number nlMapZ(number from, const coeffs src, const coeffs dst)
{
  if (SR_HDL(from) & SR_INT)
  {
    return from;
  }
  return nlMapGMP(from,src,dst);
}

/*2
* convert from an machine long
*/
number nlMapMachineInt(number from, const coeffs /*src*/, const coeffs /*dst*/)
{
  number z=ALLOC_RNUMBER();
#if defined(LDEBUG)
  z->debug=123456;
#endif
  mpz_init_set_ui(z->z,(unsigned long) from);
  z->s = 3;
  z=nlShort3(z);
  return z;
}
#endif


#ifdef LDEBUG
BOOLEAN nlDBTest(number a, const char *f,const int l, const coeffs /*r*/)
{
  if (a==NULL)
  {
    Print("!!longrat: NULL in %s:%d\n",f,l);
    return FALSE;
  }
  //if ((int)a==1) Print("!! 0x1 as number ? %s %d\n",f,l);
  if ((((long)a)&3L)==3L)
  {
    Print(" !!longrat:ptr(3) in %s:%d\n",f,l);
    return FALSE;
  }
  if ((((long)a)&3L)==1L)
  {
    if (((((LONG)(long)a)<<1)>>1)!=((LONG)(long)a))
    {
      Print(" !!longrat:arith:%lx in %s:%d\n",(long)a, f,l);
      return FALSE;
    }
    return TRUE;
  }
  /* TODO: If next line is active, then computations in algebraic field
           extensions over Q will throw a lot of assume violations although
           everything is computed correctly and no seg fault appears.
           Maybe the test is not appropriate in this case. */
  omCheckIf(omCheckAddrSize(a,sizeof(*a)), return FALSE);
  if (a->debug!=123456)
  {
    Print("!!longrat:debug:%d in %s:%d\n",a->debug,f,l);
    a->debug=123456;
    return FALSE;
  }
  if ((a->s<0)||(a->s>4))
  {
    Print("!!longrat:s=%d in %s:%d\n",a->s,f,l);
    return FALSE;
  }
  /* TODO: If next line is active, then computations in algebraic field
           extensions over Q will throw a lot of assume violations although
           everything is computed correctly and no seg fault appears.
           Maybe the test is not appropriate in this case. */
  //omCheckAddrSize(a->z[0]._mp_d,a->z[0]._mp_alloc*BYTES_PER_MP_LIMB);
  if (a->z[0]._mp_alloc==0)
    Print("!!longrat:z->alloc=0 in %s:%d\n",f,l);

  if (a->s<2)
  {
    if ((a->n[0]._mp_d[0]==0)&&(a->n[0]._mp_alloc<=1))
    {
      Print("!!longrat: n==0 in %s:%d\n",f,l);
      return FALSE;
    }
    /* TODO: If next line is active, then computations in algebraic field
             extensions over Q will throw a lot of assume violations although
             everything is computed correctly and no seg fault appears.
             Maybe the test is not appropriate in this case. */
    //omCheckIf(omCheckAddrSize(a->n[0]._mp_d,a->n[0]._mp_alloc*BYTES_PER_MP_LIMB), return FALSE);
    if (a->z[0]._mp_alloc==0)
      Print("!!longrat:n->alloc=0 in %s:%d\n",f,l);
    if ((mpz_size1(a->n) ==1) && (mpz_cmp_si(a->n,1L)==0))
    {
      Print("!!longrat:integer as rational in %s:%d\n",f,l);
      mpz_clear(a->n); a->s=3;
      return FALSE;
    }
    else if (mpz_isNeg(a->n))
    {
      Print("!!longrat:div. by negative in %s:%d\n",f,l);
      mpz_neg(a->z,a->z);
      mpz_neg(a->n,a->n);
      return FALSE;
    }
    return TRUE;
  }
  //if (a->s==2)
  //{
  //  Print("!!longrat:s=2 in %s:%d\n",f,l);
  //  return FALSE;
  //}
  if (mpz_size1(a->z)>MP_SMALL) return TRUE;
  LONG ui=(LONG)mpz_get_si(a->z);
  if ((((ui<<3)>>3)==ui)
  && (mpz_cmp_si(a->z,(long)ui)==0))
  {
    Print("!!longrat:im int %d in %s:%d\n",ui,f,l);
    return FALSE;
  }
  return TRUE;
}
#endif

static CanonicalForm nlConvSingNFactoryN( number n, const BOOLEAN setChar, const coeffs /*r*/ )
{
  if (setChar) setCharacteristic( 0 );

  CanonicalForm term;
  if ( SR_HDL(n) & SR_INT )
  {
    long nn=SR_TO_INT(n);
    term = nn;
  }
  else
  {
    if ( n->s == 3 )
    {
      mpz_t dummy;
      long lz=mpz_get_si(n->z);
      if (mpz_cmp_si(n->z,lz)==0) term=lz;
      else
      {
        mpz_init_set( dummy,n->z );
        term = make_cf( dummy );
      }
    }
    else
    {
      // assume s==0 or s==1
      mpz_t num, den;
      On(SW_RATIONAL);
      mpz_init_set( num, n->z );
      mpz_init_set( den, n->n );
      term = make_cf( num, den, ( n->s != 1 ));
    }
  }
  return term;
}

number nlRInit (long i);

static number nlConvFactoryNSingN( const CanonicalForm f, const coeffs r)
{
  if (f.isImm())
  {
    return nlInit(f.intval(),r);
  }
  else
  {
    number z = ALLOC_RNUMBER();
#if defined(LDEBUG)
    z->debug=123456;
#endif
    gmp_numerator( f, z->z );
    if ( f.den().isOne() )
    {
      z->s = 3;
      z=nlShort3(z);
    }
    else
    {
      gmp_denominator( f, z->n );
      z->s = 1;
    }
    return z;
  }
}

static number nlMapR(number from, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(src) == n_R );

  double f=nrFloat(from); // FIXME? TODO? // extern float   nrFloat(number n);
  if (f==0.0) return INT_TO_SR(0);
  int f_sign=1;
  if (f<0.0)
  {
    f_sign=-1;
    f=-f;
  }
  int i=0;
  mpz_t h1;
  mpz_init_set_ui(h1,1);
  while((FLT_RADIX*f) < DBL_MAX && i<DBL_MANT_DIG)
  {
    f*=FLT_RADIX;
    mpz_mul_ui(h1,h1,FLT_RADIX);
    i++;
  }
  number re=nlRInit(1);
  mpz_set_d(re->z,f);
  memcpy(&(re->n),&h1,sizeof(h1));
  re->s=0; /* not normalized */
  if(f_sign==-1) re=nlNeg(re,dst);
  nlNormalize(re,dst);
  return re;
}

static number nlMapLongR(number from, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(src) == n_long_R );

  gmp_float *ff=(gmp_float*)from;
  mpf_t *f=ff->_mpfp();
  number res;
  mpz_ptr dest,ndest;
  int size, i,negative;
  int e,al,bl;
  mp_ptr qp,dd,nn;

  size = (*f)[0]._mp_size;
  if (size == 0)
    return INT_TO_SR(0);
  if(size<0)
  {
    negative = 1;
    size = -size;
  }
  else
    negative = 0;

  qp = (*f)[0]._mp_d;
  while(qp[0]==0)
  {
    qp++;
    size--;
  }

  e=(*f)[0]._mp_exp-size;
  res = ALLOC_RNUMBER();
#if defined(LDEBUG)
  res->debug=123456;
#endif
  dest = res->z;

  void* (*allocfunc) (size_t);
  mp_get_memory_functions (&allocfunc,NULL, NULL);
  if (e<0)
  {
    al = dest->_mp_size = size;
    if (al<2) al = 2;
    dd = (mp_ptr)allocfunc(sizeof(mp_limb_t)*al);
    for (i=0;i<size;i++) dd[i] = qp[i];
    bl = 1-e;
    nn = (mp_ptr)allocfunc(sizeof(mp_limb_t)*bl);
    memset(nn,0,sizeof(mp_limb_t)*bl);
    nn[bl-1] = 1;
    ndest = res->n;
    ndest->_mp_d = nn;
    ndest->_mp_alloc = ndest->_mp_size = bl;
    res->s = 0;
  }
  else
  {
    al = dest->_mp_size = size+e;
    if (al<2) al = 2;
    dd = (mp_ptr)allocfunc(sizeof(mp_limb_t)*al);
    memset(dd,0,sizeof(mp_limb_t)*al);
    for (i=0;i<size;i++) dd[i+e] = qp[i];
    for (i=0;i<e;i++) dd[i] = 0;
    res->s = 3;
  }

  dest->_mp_d = dd;
  dest->_mp_alloc = al;
  if (negative) mpz_neg(dest,dest);

  if (res->s==0)
    nlNormalize(res,dst);
  else if (mpz_size1(res->z)<=MP_SMALL)
  {
    // res is new, res->ref is 1
    res=nlShort3(res);
  }
  nlTest(res, dst);
  return res;
}

#ifndef P_NUMBERS_H

static number nlInitMPZ(mpz_t m, const coeffs)
{
  number z = ALLOC_RNUMBER();
  z->s = 3;
  #ifdef LDEBUG
  z->debug=123456;
  #endif
  mpz_init_set(z->z, m);
  z=nlShort3(z);
  return z;
}
#endif

static number nlMapC(number from, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(src) == n_long_C );
  if ( ! ((gmp_complex*)from)->imag().isZero() )
    return INT_TO_SR(0);

  if (dst->is_field==FALSE) /* ->ZZ */
  {
    char *s=floatToStr(((gmp_complex*)from)->real(),src->float_len);
    mpz_t z;
    mpz_init(z);
    char *ss=nEatLong(s,z);
    if (*ss=='\0')
    {
      omFree(s);
      number n=nlInitMPZ(z,dst);
      mpz_clear(z);
      return n;
    }
    omFree(s);
    mpz_clear(z);
    WarnS("conversion problem in CC -> ZZ mapping");
    return INT_TO_SR(0);
  }
      
  mpf_t *f = ((gmp_complex*)from)->real()._mpfp();

  number res;
  mpz_ptr dest,ndest;
  int size, i,negative;
  int e,al,bl;
  mp_ptr qp,dd,nn;

  size = (*f)[0]._mp_size;
  if (size == 0)
    return INT_TO_SR(0);
  if(size<0)
  {
    negative = 1;
    size = -size;
  }
  else
    negative = 0;

  qp = (*f)[0]._mp_d;
  while(qp[0]==0)
  {
    qp++;
    size--;
  }

  e=(*f)[0]._mp_exp-size;
  res = ALLOC_RNUMBER();
#if defined(LDEBUG)
  res->debug=123456;
#endif
  dest = res->z;

  void* (*allocfunc) (size_t);
  mp_get_memory_functions (&allocfunc,NULL, NULL);
  if (e<0)
  {
    al = dest->_mp_size = size;
    if (al<2) al = 2;
    dd = (mp_ptr)allocfunc(sizeof(mp_limb_t)*al);
    for (i=0;i<size;i++) dd[i] = qp[i];
    bl = 1-e;
    nn = (mp_ptr)allocfunc(sizeof(mp_limb_t)*bl);
    memset(nn,0,sizeof(mp_limb_t)*bl);
    nn[bl-1] = 1;
    ndest = res->n;
    ndest->_mp_d = nn;
    ndest->_mp_alloc = ndest->_mp_size = bl;
    res->s = 0;
  }
  else
  {
    al = dest->_mp_size = size+e;
    if (al<2) al = 2;
    dd = (mp_ptr)allocfunc(sizeof(mp_limb_t)*al);
    memset(dd,0,sizeof(mp_limb_t)*al);
    for (i=0;i<size;i++) dd[i+e] = qp[i];
    for (i=0;i<e;i++) dd[i] = 0;
    res->s = 3;
  }

  dest->_mp_d = dd;
  dest->_mp_alloc = al;
  if (negative) mpz_neg(dest,dest);

  if (res->s==0)
    nlNormalize(res,dst);
  else if (mpz_size1(res->z)<=MP_SMALL)
  {
    // res is new, res->ref is 1
    res=nlShort3(res);
  }
  nlTest(res, dst);
  return res;
}

//static number nlMapLongR(number from)
//{
//  gmp_float *ff=(gmp_float*)from;
//  const mpf_t *f=ff->mpfp();
//  int f_size=ABS((*f)[0]._mp_size);
//  if (f_size==0)
//    return nlInit(0);
//  int f_sign=1;
//  number work=ngcCopy(from);
//  if (!ngcGreaterZero(work))
//  {
//    f_sign=-1;
//    work=ngcNeg(work);
//  }
//  int i=0;
//  mpz_t h1;
//  mpz_init_set_ui(h1,1);
//  while((FLT_RADIX*f) < DBL_MAX && i<DBL_MANT_DIG)
//  {
//    f*=FLT_RADIX;
//    mpz_mul_ui(h1,h1,FLT_RADIX);
//    i++;
//  }
//  number r=nlRInit(1);
//  mpz_set_d(&(r->z),f);
//  memcpy(&(r->n),&h1,sizeof(h1));
//  r->s=0; /* not normalized */
//  nlNormalize(r);
//  return r;
//
//
//  number r=nlRInit(1);
//  int f_shift=f_size+(*f)[0]._mp_exp;
//  if ( f_shift > 0)
//  {
//    r->s=0;
//    mpz_init(&r->n);
//    mpz_setbit(&r->n,f_shift*BYTES_PER_MP_LIMB*8);
//    mpz_setbit(&r->z,f_size*BYTES_PER_MP_LIMB*8-1);
//    // now r->z has enough space
//    memcpy(mpz_limb_d(&r->z),((*f)[0]._mp_d),f_size*BYTES_PER_MP_LIMB);
//    nlNormalize(r);
//  }
//  else
//  {
//    r->s=3;
//    if (f_shift==0)
//    {
//      mpz_setbit(&r->z,f_size*BYTES_PER_MP_LIMB*8-1);
//      // now r->z has enough space
//      memcpy(mpz_limb_d(&r->z),((*f)[0]._mp_d),f_size*BYTES_PER_MP_LIMB);
//    }
//    else /* f_shift < 0 */
//    {
//      mpz_setbit(&r->z,(f_size-f_shift)*BYTES_PER_MP_LIMB*8-1);
//      // now r->z has enough space
//      memcpy(mpz_limb_d(&r->z)-f_shift,((*f)[0]._mp_d),
//        f_size*BYTES_PER_MP_LIMB);
//    }
//  }
//  if ((*f)[0]._mp_size<0);
//    r=nlNeg(r);
//  return r;
//}

int nlSize(number a, const coeffs)
{
  if (a==INT_TO_SR(0))
     return 0; /* rational 0*/
  if (SR_HDL(a) & SR_INT)
     return 1; /* immediate int */
  int s=a->z[0]._mp_alloc;
//  while ((s>0) &&(a->z._mp_d[s]==0L)) s--;
//#if SIZEOF_LONG == 8
//  if (a->z._mp_d[s] < (unsigned long)0x100000000L) s=s*2-1;
//  else s *=2;
//#endif
//  s++;
  if (a->s<2)
  {
    int d=a->n[0]._mp_alloc;
//    while ((d>0) && (a->n._mp_d[d]==0L)) d--;
//#if SIZEOF_LONG == 8
//    if (a->n._mp_d[d] < (unsigned long)0x100000000L) d=d*2-1;
//    else d *=2;
//#endif
    s+=d;
  }
  return s;
}

/*2
* convert number to int
*/
long nlInt(number &i, const coeffs r)
{
  nlTest(i, r);
  nlNormalize(i,r);
  if (SR_HDL(i) & SR_INT)
  {
    return SR_TO_INT(i);
  }
  if (i->s==3)
  {
    if(mpz_size1(i->z)>MP_SMALL) return 0;
    long ul=mpz_get_si(i->z);
    if (mpz_cmp_si(i->z,ul)!=0) return 0;
    return ul;
  }
  mpz_t tmp;
  long ul;
  mpz_init(tmp);
  mpz_tdiv_q(tmp,i->z,i->n);
  if(mpz_size1(tmp)>MP_SMALL) ul=0;
  else
  {
    ul=mpz_get_si(tmp);
    if (mpz_cmp_si(tmp,ul)!=0) ul=0;
  }
  mpz_clear(tmp);
  return ul;
}

/*2
* convert number to bigint
*/
number nlBigInt(number &i, const coeffs r)
{
  nlTest(i, r);
  nlNormalize(i,r);
  if (SR_HDL(i) & SR_INT) return (i);
  if (i->s==3)
  {
    return nlCopy(i,r);
  }
  number tmp=nlRInit(1);
  mpz_tdiv_q(tmp->z,i->z,i->n);
  tmp=nlShort3(tmp);
  return tmp;
}

/*
* 1/a
*/
number nlInvers(number a, const coeffs r)
{
  nlTest(a, r);
  number n;
  if (SR_HDL(a) & SR_INT)
  {
    if ((a==INT_TO_SR(1L)) || (a==INT_TO_SR(-1L)))
    {
      return a;
    }
    if (nlIsZero(a,r))
    {
      WerrorS(nDivBy0);
      return INT_TO_SR(0);
    }
    n=ALLOC_RNUMBER();
#if defined(LDEBUG)
    n->debug=123456;
#endif
    n->s=1;
    if (((long)a)>0L)
    {
      mpz_init_set_ui(n->z,1L);
      mpz_init_set_si(n->n,(long)SR_TO_INT(a));
    }
    else
    {
      mpz_init_set_si(n->z,-1L);
      mpz_init_set_si(n->n,(long)-SR_TO_INT(a));
    }
    nlTest(n, r);
    return n;
  }
  n=ALLOC_RNUMBER();
#if defined(LDEBUG)
  n->debug=123456;
#endif
  {
    mpz_init_set(n->n,a->z);
    switch (a->s)
    {
      case 0:
      case 1:
              n->s=a->s;
              mpz_init_set(n->z,a->n);
              if (mpz_isNeg(n->n)) /* && n->s<2*/
              {
                mpz_neg(n->z,n->z);
                mpz_neg(n->n,n->n);
              }
              if (mpz_cmp_ui(n->n,1L)==0)
              {
                mpz_clear(n->n);
                n->s=3;
                n=nlShort3(n);
              }
              break;
      case 3:
              // i.e. |a| > 2^...
              n->s=1;
              if (mpz_isNeg(n->n)) /* && n->s<2*/
              {
                mpz_neg(n->n,n->n);
                mpz_init_set_si(n->z,-1L);
              }
              else
              {
                mpz_init_set_ui(n->z,1L);
              }
              break;
    }
  }
  nlTest(n, r);
  return n;
}


/*2
* u := a / b in Z, if b | a (else undefined)
*/
number   nlExactDiv(number a, number b, const coeffs r)
{
  if (b==INT_TO_SR(0))
  {
    WerrorS(nDivBy0);
    return INT_TO_SR(0);
  }
  if (a==INT_TO_SR(0))
    return INT_TO_SR(0);
  number u;
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    /* the small int -(1<<28) divided by -1 is the large int (1<<28)   */
    if ((a==INT_TO_SR(-(POW_2_28)))&&(b==INT_TO_SR(-1L)))
    {
      return nlRInit(POW_2_28);
    }
    long aa=SR_TO_INT(a);
    long bb=SR_TO_INT(b);
    return INT_TO_SR(aa/bb);
  }
  number aa=NULL;
  number bb=NULL;
  if (SR_HDL(a) & SR_INT)
  {
    aa=nlRInit(SR_TO_INT(a));
    a=aa;
  }
  if (SR_HDL(b) & SR_INT)
  {
    bb=nlRInit(SR_TO_INT(b));
    b=bb;
  }
  u=ALLOC_RNUMBER();
#if defined(LDEBUG)
  u->debug=123456;
#endif
  mpz_init(u->z);
  /* u=a/b */
  u->s = 3;
  assume(a->s==3);
  assume(b->s==3);
  mpz_divexact(u->z,a->z,b->z);
  if (aa!=NULL)
  {
    mpz_clear(aa->z);
#if defined(LDEBUG)
    aa->debug=654324;
#endif
    FREE_RNUMBER(aa); // omFreeBin((void *)aa, rnumber_bin);
  }
  if (bb!=NULL)
  {
    mpz_clear(bb->z);
#if defined(LDEBUG)
    bb->debug=654324;
#endif
    FREE_RNUMBER(bb); // omFreeBin((void *)bb, rnumber_bin);
  }
  u=nlShort3(u);
  nlTest(u, r);
  return u;
}

/*2
* u := a / b in Z
*/
number nlIntDiv (number a, number b, const coeffs r)
{
  if (b==INT_TO_SR(0))
  {
    WerrorS(nDivBy0);
    return INT_TO_SR(0);
  }
  if (a==INT_TO_SR(0))
    return INT_TO_SR(0);
  number u;
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    /* the small int -(1<<28) divided by -1 is the large int (1<<28)   */
    if ((a==INT_TO_SR(-(POW_2_28)))&&(b==INT_TO_SR(-1L)))
    {
      return nlRInit(POW_2_28);
    }
    LONG aa=SR_TO_INT(a);
    LONG bb=SR_TO_INT(b);
    LONG rr=aa%bb;
    if (rr<0) rr+=ABS(bb);
    LONG cc=(aa-rr)/bb;
    return INT_TO_SR(cc);
  }
  number aa=NULL;
  if (SR_HDL(a) & SR_INT)
  {
    /* the small int -(1<<28) divided by 2^28 is 1   */
    if (a==INT_TO_SR(-(POW_2_28)))
    {
      if(mpz_cmp_si(b->z,(POW_2_28))==0)
      {
        return INT_TO_SR(-1);
      }
    }
    aa=nlRInit(SR_TO_INT(a));
    a=aa;
  }
  number bb=NULL;
  if (SR_HDL(b) & SR_INT)
  {
    bb=nlRInit(SR_TO_INT(b));
    b=bb;
  }
  u=ALLOC_RNUMBER();
#if defined(LDEBUG)
  u->debug=123456;
#endif
  assume(a->s==3);
  assume(b->s==3);
  mpz_init_set(u->z,a->z);
  /* u=u/b */
  u->s = 3;
  number rr=nlIntMod(a,b,r);
  if (SR_HDL(rr) &  SR_INT) mpz_sub_ui(u->z,u->z,SR_TO_INT(rr));
  else                      mpz_sub(u->z,u->z,rr->z);
  mpz_divexact(u->z,u->z,b->z);
  if (aa!=NULL)
  {
    mpz_clear(aa->z);
#if defined(LDEBUG)
    aa->debug=654324;
#endif
    FREE_RNUMBER(aa);
  }
  if (bb!=NULL)
  {
    mpz_clear(bb->z);
#if defined(LDEBUG)
    bb->debug=654324;
#endif
    FREE_RNUMBER(bb);
  }
  u=nlShort3(u);
  nlTest(u,r);
  return u;
}

/*2
* u := a mod b in Z, u>=0
*/
number nlIntMod (number a, number b, const coeffs r)
{
  if (b==INT_TO_SR(0))
  {
    WerrorS(nDivBy0);
    return INT_TO_SR(0);
  }
  if (a==INT_TO_SR(0))
    return INT_TO_SR(0);
  number u;
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    LONG aa=SR_TO_INT(a);
    LONG bb=SR_TO_INT(b);
    LONG c=aa % bb;
    if (c<0) c+=ABS(bb);
    return INT_TO_SR(c);
  }
  if (SR_HDL(a) & SR_INT)
  {
    LONG ai=SR_TO_INT(a);
    mpz_t aa;
    mpz_init_set_si(aa, ai);
    u=ALLOC_RNUMBER();
#if defined(LDEBUG)
    u->debug=123456;
#endif
    u->s = 3;
    mpz_init(u->z);
    mpz_mod(u->z, aa, b->z);
    mpz_clear(aa);
    u=nlShort3(u);
    nlTest(u,r);
    return u;
  }
  number bb=NULL;
  if (SR_HDL(b) & SR_INT)
  {
    bb=nlRInit(SR_TO_INT(b));
    b=bb;
  }
  u=ALLOC_RNUMBER();
#if defined(LDEBUG)
  u->debug=123456;
#endif
  mpz_init(u->z);
  u->s = 3;
  mpz_mod(u->z, a->z, b->z);
  if (bb!=NULL)
  {
    mpz_clear(bb->z);
#if defined(LDEBUG)
    bb->debug=654324;
#endif
    FREE_RNUMBER(bb);
  }
  u=nlShort3(u);
  nlTest(u,r);
  return u;
}

BOOLEAN nlDivBy (number a,number b, const coeffs)
{
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    return ((SR_TO_INT(a) % SR_TO_INT(b))==0);
  }
  if (SR_HDL(b) & SR_INT)
  {
    return (mpz_divisible_ui_p(a->z,SR_TO_INT(b))!=0);
  }
  if (SR_HDL(a) & SR_INT) return FALSE;
  return mpz_divisible_p(a->z, b->z) != 0;
}

int nlDivComp(number a, number b, const coeffs r)
{
  if (nlDivBy(a, b, r))
  {
    if (nlDivBy(b, a, r)) return 2;
    return -1;
  }
  if (nlDivBy(b, a, r)) return 1;
  return 0;
}

number  nlGetUnit (number n, const coeffs cf)
{
  if (nlGreaterZero(n,cf)) return INT_TO_SR(1);
  else                     return INT_TO_SR(-1);
}

coeffs nlQuot1(number c, const coeffs r)
{
  long ch = r->cfInt(c, r);
  int p=IsPrime(ch);
  coeffs rr=NULL;
  if (((long)p)==ch)
  {
    rr = nInitChar(n_Zp,(void*)ch);
  }
  #ifdef HAVE_RINGS
  else
  {
    mpz_t dummy;
    mpz_init_set_ui(dummy, ch);
    ZnmInfo info;
    info.base = dummy;
    info.exp = (unsigned long) 1;
    rr = nInitChar(n_Zn, (void*)&info);
    mpz_clear(dummy);
  }
  #endif
  return(rr);
}


BOOLEAN nlIsUnit (number a, const coeffs)
{
  return ((SR_HDL(a) & SR_INT) && (ABS(SR_TO_INT(a))==1));
}


/*2
* u := a / b
*/
number nlDiv (number a, number b, const coeffs r)
{
  if (nlIsZero(b,r))
  {
    WerrorS(nDivBy0);
    return INT_TO_SR(0);
  }
  number u;
// ---------- short / short ------------------------------------
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    LONG i=SR_TO_INT(a);
    LONG j=SR_TO_INT(b);
    if (j==1L) return a;
    if ((i==-POW_2_28) && (j== -1L))
    {
      return nlRInit(POW_2_28);
    }
    LONG r=i%j;
    if (r==0)
    {
      return INT_TO_SR(i/j);
    }
    u=ALLOC_RNUMBER();
    u->s=0;
    #if defined(LDEBUG)
    u->debug=123456;
    #endif
    mpz_init_set_si(u->z,(long)i);
    mpz_init_set_si(u->n,(long)j);
  }
  else
  {
    u=ALLOC_RNUMBER();
    u->s=0;
    #if defined(LDEBUG)
    u->debug=123456;
    #endif
    mpz_init(u->z);
// ---------- short / long ------------------------------------
    if (SR_HDL(a) & SR_INT)
    {
      // short a / (z/n) -> (a*n)/z
      if (b->s<2)
      {
        mpz_mul_si(u->z,b->n,SR_TO_INT(a));
      }
      else
      // short a / long z -> a/z
      {
        mpz_set_si(u->z,SR_TO_INT(a));
      }
      if (mpz_cmp(u->z,b->z)==0)
      {
        mpz_clear(u->z);
        FREE_RNUMBER(u);
        return INT_TO_SR(1);
      }
      mpz_init_set(u->n,b->z);
    }
// ---------- long / short ------------------------------------
    else if (SR_HDL(b) & SR_INT)
    {
      mpz_set(u->z,a->z);
      // (z/n) / b -> z/(n*b)
      if (a->s<2)
      {
        mpz_init_set(u->n,a->n);
        if (((long)b)>0L)
          mpz_mul_ui(u->n,u->n,SR_TO_INT(b));
        else
        {
          mpz_mul_ui(u->n,u->n,-SR_TO_INT(b));
          mpz_neg(u->z,u->z);
        }
      }
      else
      // long z / short b -> z/b
      {
        //mpz_set(u->z,a->z);
        mpz_init_set_si(u->n,SR_TO_INT(b));
      }
    }
// ---------- long / long ------------------------------------
    else
    {
      mpz_set(u->z,a->z);
      mpz_init_set(u->n,b->z);
      if (a->s<2) mpz_mul(u->n,u->n,a->n);
      if (b->s<2) mpz_mul(u->z,u->z,b->n);
    }
  }
  if (mpz_isNeg(u->n))
  {
    mpz_neg(u->z,u->z);
    mpz_neg(u->n,u->n);
  }
  if (mpz_cmp_si(u->n,1L)==0)
  {
    mpz_clear(u->n);
    u->s=3;
    u=nlShort3(u);
  }
  nlTest(u, r);
  return u;
}

/*2
* u:= x ^ exp
*/
void nlPower (number x,int exp,number * u, const coeffs r)
{
  *u = INT_TO_SR(0); // 0^e, e!=0
  if (exp==0)
    *u= INT_TO_SR(1);
  else if (!nlIsZero(x,r))
  {
    nlTest(x, r);
    number aa=NULL;
    if (SR_HDL(x) & SR_INT)
    {
      aa=nlRInit(SR_TO_INT(x));
      x=aa;
    }
    else if (x->s==0)
      nlNormalize(x,r);
    *u=ALLOC_RNUMBER();
#if defined(LDEBUG)
    (*u)->debug=123456;
#endif
    mpz_init((*u)->z);
    mpz_pow_ui((*u)->z,x->z,(unsigned long)exp);
    if (x->s<2)
    {
      if (mpz_cmp_si(x->n,1L)==0)
      {
        x->s=3;
        mpz_clear(x->n);
      }
      else
      {
        mpz_init((*u)->n);
        mpz_pow_ui((*u)->n,x->n,(unsigned long)exp);
      }
    }
    (*u)->s = x->s;
    if ((*u)->s==3) *u=nlShort3(*u);
    if (aa!=NULL)
    {
      mpz_clear(aa->z);
      FREE_RNUMBER(aa);
    }
  }
#ifdef LDEBUG
  if (exp<0) Print("nlPower: neg. exp. %d\n",exp);
  nlTest(*u, r);
#endif
}


/*2
* za >= 0 ?
*/
BOOLEAN nlGreaterZero (number a, const coeffs r)
{
  nlTest(a, r);
  if (SR_HDL(a) & SR_INT) return SR_HDL(a)>1L /* represents number(0) */;
  return (!mpz_isNeg(a->z));
}

/*2
* a > b ?
*/
BOOLEAN nlGreater (number a, number b, const coeffs r)
{
  nlTest(a, r);
  nlTest(b, r);
  number re;
  BOOLEAN rr;
  re=nlSub(a,b,r);
  rr=(!nlIsZero(re,r)) && (nlGreaterZero(re,r));
  nlDelete(&re,r);
  return rr;
}

/*2
* a == -1 ?
*/
BOOLEAN nlIsMOne (number a, const coeffs r)
{
#ifdef LDEBUG
  if (a==NULL) return FALSE;
  nlTest(a, r);
#endif
  return  (a==INT_TO_SR(-1L));
}

/*2
* result =gcd(a,b)
*/
number nlGcd(number a, number b, const coeffs r)
{
  number result;
  nlTest(a, r);
  nlTest(b, r);
  //nlNormalize(a);
  //nlNormalize(b);
  if ((a==INT_TO_SR(1L))||(a==INT_TO_SR(-1L))
  ||  (b==INT_TO_SR(1L))||(b==INT_TO_SR(-1L)))
    return INT_TO_SR(1L);
  if (a==INT_TO_SR(0)) /* gcd(0,b) ->b */
    return nlCopy(b,r);
  if (b==INT_TO_SR(0)) /* gcd(a,0) -> a */
    return nlCopy(a,r);
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    long i=SR_TO_INT(a);
    long j=SR_TO_INT(b);
    if((i==0L)||(j==0L))
      return INT_TO_SR(1);
    long l;
    i=ABS(i);
    j=ABS(j);
    do
    {
      l=i%j;
      i=j;
      j=l;
    } while (l!=0L);
    if (i==POW_2_28)
      result=nlRInit(POW_2_28);
    else
     result=INT_TO_SR(i);
    nlTest(result,r);
    return result;
  }
  if (((!(SR_HDL(a) & SR_INT))&&(a->s<2))
  ||  ((!(SR_HDL(b) & SR_INT))&&(b->s<2))) return INT_TO_SR(1);
  if (SR_HDL(a) & SR_INT)
  {
    LONG aa=ABS(SR_TO_INT(a));
    unsigned long t=mpz_gcd_ui(NULL,b->z,(long)aa);
    if (t==POW_2_28)
      result=nlRInit(POW_2_28);
    else
     result=INT_TO_SR(t);
  }
  else
  if (SR_HDL(b) & SR_INT)
  {
    LONG bb=ABS(SR_TO_INT(b));
    unsigned long t=mpz_gcd_ui(NULL,a->z,(long)bb);
    if (t==POW_2_28)
      result=nlRInit(POW_2_28);
    else
     result=INT_TO_SR(t);
  }
  else
  {
    result=ALLOC0_RNUMBER();
    result->s = 3;
  #ifdef LDEBUG
    result->debug=123456;
  #endif
    mpz_init(result->z);
    mpz_gcd(result->z,a->z,b->z);
    result=nlShort3(result);
  }
  nlTest(result, r);
  return result;
}
static int int_extgcd(int a, int b, int * u, int* x, int * v, int* y)
{
  int q, r;
  if (a==0)
  {
    *u = 0;
    *v = 1;
    *x = -1;
    *y = 0;
    return b;
  }
  if (b==0)
  {
    *u = 1;
    *v = 0;
    *x = 0;
    *y = 1;
    return a;
  }
  *u=1;
  *v=0;
  *x=0;
  *y=1;
  do
  {
    q = a/b;
    r = a%b;
    assume (q*b+r == a);
    a = b;
    b = r;

    r = -(*v)*q+(*u);
    (*u) =(*v);
    (*v) = r;

    r = -(*y)*q+(*x);
    (*x) = (*y);
    (*y) = r;
  } while (b);

  return a;
}

//number nlGcd_dummy(number a, number b, const coeffs r)
//{
//  extern char my_yylinebuf[80];
//  Print("nlGcd in >>%s<<\n",my_yylinebuf);
//  return nlGcd(a,b,r);;
//}

number nlShort1(number x) // assume x->s==0/1
{
  assume(x->s<2);
  if (mpz_sgn1(x->z)==0)
  {
    _nlDelete_NoImm(&x);
    return INT_TO_SR(0);
  }
  if (x->s<2)
  {
    if (mpz_cmp(x->z,x->n)==0)
    {
      _nlDelete_NoImm(&x);
      return INT_TO_SR(1);
    }
  }
  return x;
}
/*2
* simplify x
*/
void nlNormalize (number &x, const coeffs r)
{
  if ((SR_HDL(x) & SR_INT) ||(x==NULL))
    return;
  if (x->s==3)
  {
    x=nlShort3_noinline(x);
    nlTest(x,r);
    return;
  }
  else if (x->s==0)
  {
    if (mpz_cmp_si(x->n,1L)==0)
    {
      mpz_clear(x->n);
      x->s=3;
      x=nlShort3(x);
    }
    else
    {
      mpz_t gcd;
      mpz_init(gcd);
      mpz_gcd(gcd,x->z,x->n);
      x->s=1;
      if (mpz_cmp_si(gcd,1L)!=0)
      {
        mpz_divexact(x->z,x->z,gcd);
        mpz_divexact(x->n,x->n,gcd);
        if (mpz_cmp_si(x->n,1L)==0)
        {
          mpz_clear(x->n);
          x->s=3;
          x=nlShort3_noinline(x);
        }
      }
      mpz_clear(gcd);
    }
  }
  nlTest(x, r);
}

/*2
* returns in result->z the lcm(a->z,b->n)
*/
number nlNormalizeHelper(number a, number b, const coeffs r)
{
  number result;
  nlTest(a, r);
  nlTest(b, r);
  if ((SR_HDL(b) & SR_INT)
  || (b->s==3))
  {
    // b is 1/(b->n) => b->n is 1 => result is a
    return nlCopy(a,r);
  }
  result=ALLOC_RNUMBER();
#if defined(LDEBUG)
  result->debug=123456;
#endif
  result->s=3;
  mpz_t gcd;
  mpz_init(gcd);
  mpz_init(result->z);
  if (SR_HDL(a) & SR_INT)
    mpz_gcd_ui(gcd,b->n,ABS(SR_TO_INT(a)));
  else
    mpz_gcd(gcd,a->z,b->n);
  if (mpz_cmp_si(gcd,1L)!=0)
  {
    mpz_t bt;
    mpz_init(bt);
    mpz_divexact(bt,b->n,gcd);
    if (SR_HDL(a) & SR_INT)
      mpz_mul_si(result->z,bt,SR_TO_INT(a));
    else
      mpz_mul(result->z,bt,a->z);
    mpz_clear(bt);
  }
  else
    if (SR_HDL(a) & SR_INT)
      mpz_mul_si(result->z,b->n,SR_TO_INT(a));
    else
      mpz_mul(result->z,b->n,a->z);
  mpz_clear(gcd);
  result=nlShort3(result);
  nlTest(result, r);
  return result;
}

// Map q \in QQ or ZZ \to Zp or an extension of it
// src = Q or Z, dst = Zp (or an extension of Zp)
number nlModP(number q, const coeffs /*Q*/, const coeffs Zp)
{
  const int p = n_GetChar(Zp);
  assume( p > 0 );

  const long P = p;
  assume( P > 0 );

  // embedded long within q => only long numerator has to be converted
  // to int (modulo char.)
  if (SR_HDL(q) & SR_INT)
  {
    long i = SR_TO_INT(q);
    return n_Init( i, Zp );
  }

  const unsigned long PP = p;

  // numerator modulo char. should fit into int
  number z = n_Init( static_cast<long>(mpz_fdiv_ui(q->z, PP)), Zp );

  // denominator != 1?
  if (q->s!=3)
  {
    // denominator modulo char. should fit into int
    number n = n_Init( static_cast<long>(mpz_fdiv_ui(q->n, PP)), Zp );

    number res = n_Div( z, n, Zp );

    n_Delete(&z, Zp);
    n_Delete(&n, Zp);

    return res;
  }

  return z;
}

#ifdef HAVE_RINGS
/*2
* convert number i (from Q) to GMP and warn if denom != 1
*/
void nlGMP(number &i, mpz_t n, const coeffs r)
{
  // Hier brauche ich einfach die GMP Zahl
  nlTest(i, r);
  nlNormalize(i, r);
  if (SR_HDL(i) & SR_INT)
  {
    mpz_set_si(n, SR_TO_INT(i));
    return;
  }
  if (i->s!=3)
  {
     WarnS("Omitted denominator during coefficient mapping !");
  }
  mpz_set(n, i->z);
}
#endif

/*2
* acces to denominator, other 1 for integers
*/
number   nlGetDenom(number &n, const coeffs r)
{
  if (!(SR_HDL(n) & SR_INT))
  {
    if (n->s==0)
    {
      nlNormalize(n,r);
    }
    if (!(SR_HDL(n) & SR_INT))
    {
      if (n->s!=3)
      {
        number u=ALLOC_RNUMBER();
        u->s=3;
#if defined(LDEBUG)
        u->debug=123456;
#endif
        mpz_init_set(u->z,n->n);
        u=nlShort3_noinline(u);
        return u;
      }
    }
  }
  return INT_TO_SR(1);
}

/*2
* acces to Nominator, nlCopy(n) for integers
*/
number   nlGetNumerator(number &n, const coeffs r)
{
  if (!(SR_HDL(n) & SR_INT))
  {
    if (n->s==0)
    {
      nlNormalize(n,r);
    }
    if (!(SR_HDL(n) & SR_INT))
    {
      number u=ALLOC_RNUMBER();
#if defined(LDEBUG)
      u->debug=123456;
#endif
      u->s=3;
      mpz_init_set(u->z,n->z);
      if (n->s!=3)
      {
        u=nlShort3_noinline(u);
      }
      return u;
    }
  }
  return n; // imm. int
}

/***************************************************************
 *
 * routines which are needed by Inline(d) routines
 *
 *******************************************************************/
BOOLEAN _nlEqual_aNoImm_OR_bNoImm(number a, number b)
{
  assume(! (SR_HDL(a) & SR_HDL(b) & SR_INT));
//  long - short
  BOOLEAN bo;
  if (SR_HDL(b) & SR_INT)
  {
    if (a->s!=0) return FALSE;
    number n=b; b=a; a=n;
  }
//  short - long
  if (SR_HDL(a) & SR_INT)
  {
    if (b->s!=0)
      return FALSE;
    if ((((long)a) > 0L) && (mpz_isNeg(b->z)))
      return FALSE;
    if ((((long)a) < 0L) && (!mpz_isNeg(b->z)))
      return FALSE;
    mpz_t  bb;
    mpz_init(bb);
    mpz_mul_si(bb,b->n,(long)SR_TO_INT(a));
    bo=(mpz_cmp(bb,b->z)==0);
    mpz_clear(bb);
    return bo;
  }
// long - long
  if (((a->s==1) && (b->s==3))
  ||  ((b->s==1) && (a->s==3)))
    return FALSE;
  if (mpz_isNeg(a->z)&&(!mpz_isNeg(b->z)))
    return FALSE;
  if (mpz_isNeg(b->z)&&(!mpz_isNeg(a->z)))
    return FALSE;
  mpz_t  aa;
  mpz_t  bb;
  mpz_init_set(aa,a->z);
  mpz_init_set(bb,b->z);
  if (a->s<2) mpz_mul(bb,bb,a->n);
  if (b->s<2) mpz_mul(aa,aa,b->n);
  bo=(mpz_cmp(aa,bb)==0);
  mpz_clear(aa);
  mpz_clear(bb);
  return bo;
}

// copy not immediate number a
number _nlCopy_NoImm(number a)
{
  assume(!((SR_HDL(a) & SR_INT)||(a==NULL)));
  //nlTest(a, r);
  number b=ALLOC_RNUMBER();
#if defined(LDEBUG)
  b->debug=123456;
#endif
  switch (a->s)
  {
    case 0:
    case 1:
            mpz_init_set(b->n,a->n);
    case 3:
            mpz_init_set(b->z,a->z);
            break;
  }
  b->s = a->s;
  return b;
}

void _nlDelete_NoImm(number *a)
{
  {
    switch ((*a)->s)
    {
      case 0:
      case 1:
        mpz_clear((*a)->n);
      case 3:
        mpz_clear((*a)->z);
#ifdef LDEBUG
        (*a)->s=2;
#endif
    }
    #ifdef LDEBUG
    memset(*a,0,sizeof(**a));
    #endif
    FREE_RNUMBER(*a); // omFreeBin((void *) *a, rnumber_bin);
  }
}

number _nlNeg_NoImm(number a)
{
  {
    mpz_neg(a->z,a->z);
    if (a->s==3)
    {
      a=nlShort3(a);
    }
  }
  return a;
}

// conditio to use nlNormalize_Gcd in intermediate computations:
#define GCD_NORM_COND(OLD,NEW) (mpz_size1(NEW->z)>mpz_size1(OLD->z))

static void nlNormalize_Gcd(number &x)
{
  mpz_t gcd;
  mpz_init(gcd);
  mpz_gcd(gcd,x->z,x->n);
  x->s=1;
  if (mpz_cmp_si(gcd,1L)!=0)
  {
    mpz_divexact(x->z,x->z,gcd);
    mpz_divexact(x->n,x->n,gcd);
    if (mpz_cmp_si(x->n,1L)==0)
    {
      mpz_clear(x->n);
      x->s=3;
      x=nlShort3_noinline(x);
    }
  }
  mpz_clear(gcd);
}

number _nlAdd_aNoImm_OR_bNoImm(number a, number b)
{
  number u=ALLOC_RNUMBER();
#if defined(LDEBUG)
  u->debug=123456;
#endif
  mpz_init(u->z);
  if (SR_HDL(b) & SR_INT)
  {
    number x=a;
    a=b;
    b=x;
  }
  if (SR_HDL(a) & SR_INT)
  {
    switch (b->s)
    {
      case 0:
      case 1:/* a:short, b:1 */
      {
        mpz_t x;
        mpz_init(x);
        mpz_mul_si(x,b->n,SR_TO_INT(a));
        mpz_add(u->z,b->z,x);
        mpz_clear(x);
        if (mpz_sgn1(u->z)==0)
        {
          mpz_clear(u->z);
          FREE_RNUMBER(u);
          return INT_TO_SR(0);
        }
        if (mpz_cmp(u->z,b->n)==0)
        {
          mpz_clear(u->z);
          FREE_RNUMBER(u);
          return INT_TO_SR(1);
        }
        mpz_init_set(u->n,b->n);
        u->s = 0;
        if (GCD_NORM_COND(b,u)) { nlNormalize_Gcd(u); }
        break;
      }
      case 3:
      {
        if (((long)a)>0L)
          mpz_add_ui(u->z,b->z,SR_TO_INT(a));
        else
          mpz_sub_ui(u->z,b->z,-SR_TO_INT(a));
        u->s = 3;
        u=nlShort3(u);
        break;
      }
    }
  }
  else
  {
    switch (a->s)
    {
      case 0:
      case 1:
      {
        switch(b->s)
        {
          case 0:
          case 1:
          {
            mpz_t x;
            mpz_init(x);

            mpz_mul(x,b->z,a->n);
            mpz_mul(u->z,a->z,b->n);
            mpz_add(u->z,u->z,x);
            mpz_clear(x);

            if (mpz_sgn1(u->z)==0)
            {
              mpz_clear(u->z);
              FREE_RNUMBER(u);
              return INT_TO_SR(0);
            }
            mpz_init(u->n);
            mpz_mul(u->n,a->n,b->n);
            if (mpz_cmp(u->z,u->n)==0)
            {
               mpz_clear(u->z);
               mpz_clear(u->n);
               FREE_RNUMBER(u);
               return INT_TO_SR(1);
            }
            u->s = 0;
            if (GCD_NORM_COND(b,u)) { nlNormalize_Gcd(u); }
            break;
          }
          case 3: /* a:1 b:3 */
          {
            mpz_mul(u->z,b->z,a->n);
            mpz_add(u->z,u->z,a->z);
            if (mpz_sgn1(u->z)==0)
            {
              mpz_clear(u->z);
              FREE_RNUMBER(u);
              return INT_TO_SR(0);
            }
            if (mpz_cmp(u->z,a->n)==0)
            {
              mpz_clear(u->z);
              FREE_RNUMBER(u);
              return INT_TO_SR(1);
            }
            mpz_init_set(u->n,a->n);
            u->s = 0;
            if (GCD_NORM_COND(a,u)) { nlNormalize_Gcd(u); }
            break;
          }
        } /*switch (b->s) */
        break;
      }
      case 3:
      {
        switch(b->s)
        {
          case 0:
          case 1:/* a:3, b:1 */
          {
            mpz_mul(u->z,a->z,b->n);
            mpz_add(u->z,u->z,b->z);
            if (mpz_sgn1(u->z)==0)
            {
              mpz_clear(u->z);
              FREE_RNUMBER(u);
              return INT_TO_SR(0);
            }
            if (mpz_cmp(u->z,b->n)==0)
            {
              mpz_clear(u->z);
              FREE_RNUMBER(u);
              return INT_TO_SR(1);
            }
            mpz_init_set(u->n,b->n);
            u->s = 0;
            if (GCD_NORM_COND(b,u)) { nlNormalize_Gcd(u); }
            break;
          }
          case 3:
          {
            mpz_add(u->z,a->z,b->z);
            u->s = 3;
            u=nlShort3(u);
            break;
          }
        }
        break;
      }
    }
  }
  return u;
}

void _nlInpAdd_aNoImm_OR_bNoImm(number &a, number b)
{
  if (SR_HDL(b) & SR_INT)
  {
    switch (a->s)
    {
      case 0:
      case 1:/* b:short, a:1 */
      {
        mpz_t x;
        mpz_init(x);
        mpz_mul_si(x,a->n,SR_TO_INT(b));
        mpz_add(a->z,a->z,x);
        mpz_clear(x);
        nlNormalize_Gcd(a);
        break;
      }
      case 3:
      {
        if (((long)b)>0L)
          mpz_add_ui(a->z,a->z,SR_TO_INT(b));
        else
          mpz_sub_ui(a->z,a->z,-SR_TO_INT(b));
        a->s = 3;
        a=nlShort3_noinline(a);
        break;
      }
    }
    return;
  }
  else if (SR_HDL(a) & SR_INT)
  {
    number u=ALLOC_RNUMBER();
    #if defined(LDEBUG)
    u->debug=123456;
    #endif
    mpz_init(u->z);
    switch (b->s)
    {
      case 0:
      case 1:/* a:short, b:1 */
      {
        mpz_t x;
        mpz_init(x);

        mpz_mul_si(x,b->n,SR_TO_INT(a));
        mpz_add(u->z,b->z,x);
        mpz_clear(x);
        // result cannot be 0, if coeffs are normalized
        mpz_init_set(u->n,b->n);
        u->s=0;
        if (GCD_NORM_COND(b,u)) { nlNormalize_Gcd(u); }
        else { u=nlShort1(u); }
        break;
      }
      case 3:
      {
        if (((long)a)>0L)
          mpz_add_ui(u->z,b->z,SR_TO_INT(a));
        else
          mpz_sub_ui(u->z,b->z,-SR_TO_INT(a));
        // result cannot be 0, if coeffs are normalized
        u->s = 3;
        u=nlShort3_noinline(u);
        break;
      }
    }
    a=u;
  }
  else
  {
    switch (a->s)
    {
      case 0:
      case 1:
      {
        switch(b->s)
        {
          case 0:
          case 1: /* a:1 b:1 */
          {
            mpz_t x;
            mpz_t y;
            mpz_init(x);
            mpz_init(y);
            mpz_mul(x,b->z,a->n);
            mpz_mul(y,a->z,b->n);
            mpz_add(a->z,x,y);
            mpz_clear(x);
            mpz_clear(y);
            mpz_mul(a->n,a->n,b->n);
            a->s=0;
            if (GCD_NORM_COND(b,a)) { nlNormalize_Gcd(a); }
            else { a=nlShort1(a);}
            break;
          }
          case 3: /* a:1 b:3 */
          {
            mpz_t x;
            mpz_init(x);
            mpz_mul(x,b->z,a->n);
            mpz_add(a->z,a->z,x);
            mpz_clear(x);
            a->s=0;
            if (GCD_NORM_COND(b,a)) { nlNormalize_Gcd(a); }
            else { a=nlShort1(a);}
            break;
          }
        } /*switch (b->s) */
        break;
      }
      case 3:
      {
        switch(b->s)
        {
          case 0:
          case 1:/* a:3, b:1 */
          {
            mpz_t x;
            mpz_init(x);
            mpz_mul(x,a->z,b->n);
            mpz_add(a->z,b->z,x);
            mpz_clear(x);
            mpz_init_set(a->n,b->n);
            a->s=0;
            if (GCD_NORM_COND(b,a)) { nlNormalize_Gcd(a); }
            else { a=nlShort1(a);}
            break;
          }
          case 3:
          {
            mpz_add(a->z,a->z,b->z);
            a->s = 3;
            a=nlShort3_noinline(a);
            break;
          }
        }
        break;
      }
    }
  }
}

number _nlSub_aNoImm_OR_bNoImm(number a, number b)
{
  number u=ALLOC_RNUMBER();
#if defined(LDEBUG)
  u->debug=123456;
#endif
  mpz_init(u->z);
  if (SR_HDL(a) & SR_INT)
  {
    switch (b->s)
    {
      case 0:
      case 1:/* a:short, b:1 */
      {
        mpz_t x;
        mpz_init(x);
        mpz_mul_si(x,b->n,SR_TO_INT(a));
        mpz_sub(u->z,x,b->z);
        mpz_clear(x);
        if (mpz_sgn1(u->z)==0)
        {
          mpz_clear(u->z);
          FREE_RNUMBER(u);
          return INT_TO_SR(0);
        }
        if (mpz_cmp(u->z,b->n)==0)
        {
          mpz_clear(u->z);
          FREE_RNUMBER(u);
          return INT_TO_SR(1);
        }
        mpz_init_set(u->n,b->n);
        u->s=0;
        if (GCD_NORM_COND(b,u)) { nlNormalize_Gcd(u); }
        break;
      }
      case 3:
      {
        if (((long)a)>0L)
        {
          mpz_sub_ui(u->z,b->z,SR_TO_INT(a));
          mpz_neg(u->z,u->z);
        }
        else
        {
          mpz_add_ui(u->z,b->z,-SR_TO_INT(a));
          mpz_neg(u->z,u->z);
        }
        u->s = 3;
        u=nlShort3(u);
        break;
      }
    }
  }
  else if (SR_HDL(b) & SR_INT)
  {
    switch (a->s)
    {
      case 0:
      case 1:/* b:short, a:1 */
      {
        mpz_t x;
        mpz_init(x);
        mpz_mul_si(x,a->n,SR_TO_INT(b));
        mpz_sub(u->z,a->z,x);
        mpz_clear(x);
        if (mpz_sgn1(u->z)==0)
        {
          mpz_clear(u->z);
          FREE_RNUMBER(u);
          return INT_TO_SR(0);
        }
        if (mpz_cmp(u->z,a->n)==0)
        {
          mpz_clear(u->z);
          FREE_RNUMBER(u);
          return INT_TO_SR(1);
        }
        mpz_init_set(u->n,a->n);
        u->s=0;
        if (GCD_NORM_COND(a,u)) { nlNormalize_Gcd(u); }
        break;
      }
      case 3:
      {
        if (((long)b)>0L)
        {
          mpz_sub_ui(u->z,a->z,SR_TO_INT(b));
        }
        else
        {
          mpz_add_ui(u->z,a->z,-SR_TO_INT(b));
        }
        u->s = 3;
        u=nlShort3(u);
        break;
      }
    }
  }
  else
  {
    switch (a->s)
    {
      case 0:
      case 1:
      {
        switch(b->s)
        {
          case 0:
          case 1:
          {
            mpz_t x;
            mpz_t y;
            mpz_init(x);
            mpz_init(y);
            mpz_mul(x,b->z,a->n);
            mpz_mul(y,a->z,b->n);
            mpz_sub(u->z,y,x);
            mpz_clear(x);
            mpz_clear(y);
            if (mpz_sgn1(u->z)==0)
            {
              mpz_clear(u->z);
              FREE_RNUMBER(u);
              return INT_TO_SR(0);
            }
            mpz_init(u->n);
            mpz_mul(u->n,a->n,b->n);
            if (mpz_cmp(u->z,u->n)==0)
            {
              mpz_clear(u->z);
              mpz_clear(u->n);
              FREE_RNUMBER(u);
              return INT_TO_SR(1);
            }
            u->s=0;
            if (GCD_NORM_COND(a,u)) { nlNormalize_Gcd(u); }
            break;
          }
          case 3: /* a:1, b:3 */
          {
            mpz_t x;
            mpz_init(x);
            mpz_mul(x,b->z,a->n);
            mpz_sub(u->z,a->z,x);
            mpz_clear(x);
            if (mpz_sgn1(u->z)==0)
            {
              mpz_clear(u->z);
              FREE_RNUMBER(u);
              return INT_TO_SR(0);
            }
            if (mpz_cmp(u->z,a->n)==0)
            {
              mpz_clear(u->z);
              FREE_RNUMBER(u);
              return INT_TO_SR(1);
            }
            mpz_init_set(u->n,a->n);
            u->s=0;
            if (GCD_NORM_COND(a,u)) { nlNormalize_Gcd(u); }
            break;
          }
        }
        break;
      }
      case 3:
      {
        switch(b->s)
        {
          case 0:
          case 1: /* a:3, b:1 */
          {
            mpz_t x;
            mpz_init(x);
            mpz_mul(x,a->z,b->n);
            mpz_sub(u->z,x,b->z);
            mpz_clear(x);
            if (mpz_sgn1(u->z)==0)
            {
              mpz_clear(u->z);
              FREE_RNUMBER(u);
              return INT_TO_SR(0);
            }
            if (mpz_cmp(u->z,b->n)==0)
            {
              mpz_clear(u->z);
              FREE_RNUMBER(u);
              return INT_TO_SR(1);
            }
            mpz_init_set(u->n,b->n);
            u->s=0;
            if (GCD_NORM_COND(b,u)) { nlNormalize_Gcd(u); }
            break;
          }
          case 3: /* a:3 , b:3 */
          {
            mpz_sub(u->z,a->z,b->z);
            u->s = 3;
            u=nlShort3(u);
            break;
          }
        }
        break;
      }
    }
  }
  return u;
}

// a and b are intermediate, but a*b not
number _nlMult_aImm_bImm_rNoImm(number a, number b)
{
  number u=ALLOC_RNUMBER();
#if defined(LDEBUG)
  u->debug=123456;
#endif
  u->s=3;
  mpz_init_set_si(u->z,SR_TO_INT(a));
  mpz_mul_si(u->z,u->z,SR_TO_INT(b));
  return u;
}

// a or b are not immediate
number _nlMult_aNoImm_OR_bNoImm(number a, number b)
{
  assume(! (SR_HDL(a) & SR_HDL(b) & SR_INT));
  number u=ALLOC_RNUMBER();
#if defined(LDEBUG)
  u->debug=123456;
#endif
  mpz_init(u->z);
  if (SR_HDL(b) & SR_INT)
  {
    number x=a;
    a=b;
    b=x;
  }
  if (SR_HDL(a) & SR_INT)
  {
    u->s=b->s;
    if (u->s==1) u->s=0;
    if (((long)a)>0L)
    {
      mpz_mul_ui(u->z,b->z,(unsigned long)SR_TO_INT(a));
    }
    else
    {
      if (a==INT_TO_SR(-1))
      {
        mpz_set(u->z,b->z);
        mpz_neg(u->z,u->z);
        u->s=b->s;
      }
      else
      {
        mpz_mul_ui(u->z,b->z,(unsigned long)-SR_TO_INT(a));
        mpz_neg(u->z,u->z);
      }
    }
    if (u->s<2)
    {
      if (mpz_cmp(u->z,b->n)==0)
      {
        mpz_clear(u->z);
        FREE_RNUMBER(u);
        return INT_TO_SR(1);
      }
      mpz_init_set(u->n,b->n);
      if (GCD_NORM_COND(b,u)) { nlNormalize_Gcd(u); }
    }
    else //u->s==3
    {
      u=nlShort3(u);
    }
  }
  else
  {
    mpz_mul(u->z,a->z,b->z);
    u->s = 0;
    if(a->s==3)
    {
      if(b->s==3)
      {
        u->s = 3;
      }
      else
      {
        if (mpz_cmp(u->z,b->n)==0)
        {
          mpz_clear(u->z);
          FREE_RNUMBER(u);
          return INT_TO_SR(1);
        }
        mpz_init_set(u->n,b->n);
        if (GCD_NORM_COND(b,u)) { nlNormalize_Gcd(u); }
      }
    }
    else
    {
      if(b->s==3)
      {
        if (mpz_cmp(u->z,a->n)==0)
        {
          mpz_clear(u->z);
          FREE_RNUMBER(u);
          return INT_TO_SR(1);
        }
        mpz_init_set(u->n,a->n);
        if (GCD_NORM_COND(a,u)) { nlNormalize_Gcd(u); }
      }
      else
      {
        mpz_init(u->n);
        mpz_mul(u->n,a->n,b->n);
        if (mpz_cmp(u->z,u->n)==0)
        {
          mpz_clear(u->z);
          mpz_clear(u->n);
          FREE_RNUMBER(u);
          return INT_TO_SR(1);
        }
        if (GCD_NORM_COND(a,u)) { nlNormalize_Gcd(u); }
      }
    }
  }
  return u;
}

/*2
* copy a to b for mapping
*/
number nlCopyMap(number a, const coeffs /*src*/, const coeffs /*dst*/)
{
  if ((SR_HDL(a) & SR_INT)||(a==NULL))
  {
    return a;
  }
  return _nlCopy_NoImm(a);
}

number nlMapQtoZ(number a, const coeffs src, const coeffs dst)
{
  if ((SR_HDL(a) & SR_INT)||(a==NULL))
  {
    return a;
  }
  if (a->s==3) return _nlCopy_NoImm(a);
  number a0=a;
  BOOLEAN a1=FALSE;
  if (a->s==0) { a0=_nlCopy_NoImm(a); a1=TRUE; }
  number b1=nlGetNumerator(a0,src);
  number b2=nlGetDenom(a0,src);
  number b=nlIntDiv(b1,b2,dst);
  nlDelete(&b1,src);
  nlDelete(&b2,src);
  if (a1)  _nlDelete_NoImm(&a0);
  return b;
}

nMapFunc nlSetMap(const coeffs src, const coeffs dst)
{
  if (src->rep==n_rep_gap_rat)  /*Q, coeffs_BIGINT */
  {
    if ((src->is_field==dst->is_field) /* Q->Q, Z->Z*/
    || (src->is_field==FALSE))         /* Z->Q */
      return nlCopyMap;
    return nlMapQtoZ;        /* Q->Z */
  }
  if ((src->rep==n_rep_int) && nCoeff_is_Zp(src))
  {
    return nlMapP;
  }
  if ((src->rep==n_rep_float) && nCoeff_is_R(src))
  {
    return nlMapR;
  }
  if ((src->rep==n_rep_gmp_float) && nCoeff_is_long_R(src))
  {
    return nlMapLongR; /* long R -> Q */
  }
  if (nCoeff_is_long_C(src))
  {
    return nlMapC; /* C -> Q */
  }
#ifdef HAVE_RINGS
  if (src->rep==n_rep_gmp) // nCoeff_is_Z(src) || nCoeff_is_Ring_PtoM(src) || nCoeff_is_Zn(src))
  {
    return nlMapGMP;
  }
  if (src->rep==n_rep_gap_gmp)
  {
    return nlMapZ;
  }
  if ((src->rep==n_rep_int) && nCoeff_is_Ring_2toM(src))
  {
    return nlMapMachineInt;
  }
#endif
  return NULL;
}
/*2
* z := i
*/
number nlRInit (long i)
{
  number z=ALLOC_RNUMBER();
#if defined(LDEBUG)
  z->debug=123456;
#endif
  mpz_init_set_si(z->z,i);
  z->s = 3;
  return z;
}

/*2
* z := i/j
*/
number nlInit2 (int i, int j, const coeffs r)
{
  number z=ALLOC_RNUMBER();
#if defined(LDEBUG)
  z->debug=123456;
#endif
  mpz_init_set_si(z->z,(long)i);
  mpz_init_set_si(z->n,(long)j);
  z->s = 0;
  nlNormalize(z,r);
  return z;
}

number nlInit2gmp (mpz_t i, mpz_t j, const coeffs r)
{
  number z=ALLOC_RNUMBER();
#if defined(LDEBUG)
  z->debug=123456;
#endif
  mpz_init_set(z->z,i);
  mpz_init_set(z->n,j);
  z->s = 0;
  nlNormalize(z,r);
  return z;
}

#else // DO_LINLINE

// declare immedate routines
number nlRInit (long i);
BOOLEAN _nlEqual_aNoImm_OR_bNoImm(number a, number b);
number  _nlCopy_NoImm(number a);
number  _nlNeg_NoImm(number a);
number  _nlAdd_aNoImm_OR_bNoImm(number a, number b);
void    _nlInpAdd_aNoImm_OR_bNoImm(number &a, number b);
number  _nlSub_aNoImm_OR_bNoImm(number a, number b);
number  _nlMult_aNoImm_OR_bNoImm(number a, number b);
number  _nlMult_aImm_bImm_rNoImm(number a, number b);

#endif

/***************************************************************
 *
 * Routines which might be inlined by p_Numbers.h
 *
 *******************************************************************/
#if defined(DO_LINLINE) || !defined(P_NUMBERS_H)

// routines which are always inlined/static

/*2
* a = b ?
*/
LINLINE BOOLEAN nlEqual (number a, number b, const coeffs r)
{
  nlTest(a, r);
  nlTest(b, r);
// short - short
  if (SR_HDL(a) & SR_HDL(b) & SR_INT) return a==b;
  return _nlEqual_aNoImm_OR_bNoImm(a, b);
}

LINLINE number nlInit (long i, const coeffs r)
{
  number n;
  #if MAX_NUM_SIZE == 60
  if (((i << 3) >> 3) == i) n=INT_TO_SR(i);
  else                      n=nlRInit(i);
  #else
  LONG ii=(LONG)i;
  if ( ((((long)ii)==i) && ((ii << 3) >> 3) == ii )) n=INT_TO_SR(ii);
  else                                             n=nlRInit(i);
  #endif
  nlTest(n, r);
  return n;
}

/*2
* a == 1 ?
*/
LINLINE BOOLEAN nlIsOne (number a, const coeffs r)
{
#ifdef LDEBUG
  if (a==NULL) return FALSE;
  nlTest(a, r);
#endif
  return (a==INT_TO_SR(1));
}

LINLINE BOOLEAN nlIsZero (number a, const coeffs)
{
  #if 0
  if (a==INT_TO_SR(0)) return TRUE;
  if ((SR_HDL(a) & SR_INT)||(a==NULL)) return FALSE;
  if (mpz_cmp_si(a->z,0L)==0)
  {
    printf("gmp-0 in nlIsZero\n");
    dErrorBreak();
    return TRUE;
  }
  return FALSE;
  #else
  return (a==NULL)|| (a==INT_TO_SR(0));
  #endif
}

/*2
* copy a to b
*/
LINLINE number nlCopy(number a, const coeffs)
{
  if ((SR_HDL(a) & SR_INT)||(a==NULL))
  {
    return a;
  }
  return _nlCopy_NoImm(a);
}


/*2
* delete a
*/
LINLINE void nlDelete (number * a, const coeffs r)
{
  if (*a!=NULL)
  {
    nlTest(*a, r);
    if ((SR_HDL(*a) & SR_INT)==0)
    {
      _nlDelete_NoImm(a);
    }
    *a=NULL;
  }
}

/*2
* za:= - za
*/
LINLINE number nlNeg (number a, const coeffs R)
{
  nlTest(a, R);
  if(SR_HDL(a) &SR_INT)
  {
    LONG r=SR_TO_INT(a);
    if (r==(-(POW_2_28))) a=nlRInit(POW_2_28);
    else               a=INT_TO_SR(-r);
    return a;
  }
  a = _nlNeg_NoImm(a);
  nlTest(a, R);
  return a;

}

/*2
* u:= a + b
*/
LINLINE number nlAdd (number a, number b, const coeffs R)
{
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    LONG r=SR_HDL(a)+SR_HDL(b)-1L;
    if ( ((r << 1) >> 1) == r )
      return (number)(long)r;
    else
      return nlRInit(SR_TO_INT(r));
  }
  number u =  _nlAdd_aNoImm_OR_bNoImm(a, b);
  nlTest(u, R);
  return u;
}

number nlShort1(number a);
number nlShort3_noinline(number x);

LINLINE void nlInpAdd(number &a, number b, const coeffs r)
{
  // a=a+b
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    LONG r=SR_HDL(a)+SR_HDL(b)-1L;
    if ( ((r << 1) >> 1) == r )
      a=(number)(long)r;
    else
      a=nlRInit(SR_TO_INT(r));
  }
  else
  {
    _nlInpAdd_aNoImm_OR_bNoImm(a,b);
    nlTest(a,r);
  }
}

LINLINE number nlMult (number a, number b, const coeffs R)
{
  nlTest(a, R);
  nlTest(b, R);
  if (a==INT_TO_SR(0)) return INT_TO_SR(0);
  if (b==INT_TO_SR(0)) return INT_TO_SR(0);
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    LONG r=(LONG)((unsigned LONG)(SR_HDL(a)-1L))*((unsigned LONG)(SR_HDL(b)>>1));
    if ((r/(SR_HDL(b)>>1))==(SR_HDL(a)-1L))
    {
      number u=((number) ((r>>1)+SR_INT));
      if (((((LONG)SR_HDL(u))<<1)>>1)==SR_HDL(u)) return (u);
      return nlRInit(SR_HDL(u)>>2);
    }
    number u = _nlMult_aImm_bImm_rNoImm(a, b);
    nlTest(u, R);
    return u;

  }
  number u = _nlMult_aNoImm_OR_bNoImm(a, b);
  nlTest(u, R);
  return u;

}


/*2
* u:= a - b
*/
LINLINE number nlSub (number a, number b, const coeffs r)
{
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    LONG r=SR_HDL(a)-SR_HDL(b)+1;
    if ( ((r << 1) >> 1) == r )
    {
      return (number)(long)r;
    }
    else
      return nlRInit(SR_TO_INT(r));
  }
  number u = _nlSub_aNoImm_OR_bNoImm(a, b);
  nlTest(u, r);
  return u;

}

LINLINE void nlInpMult(number &a, number b, const coeffs r)
{
  number aa=a;
  if (((SR_HDL(b)|SR_HDL(aa))&SR_INT))
  {
    number n=nlMult(aa,b,r);
    nlDelete(&a,r);
    a=n;
  }
  else
  {
    mpz_mul(aa->z,a->z,b->z);
    if (aa->s==3)
    {
      if(b->s!=3)
      {
        mpz_init_set(a->n,b->n);
        a->s=0;
      }
    }
    else
    {
      if(b->s!=3)
      {
        mpz_mul(a->n,a->n,b->n);
      }
      a->s=0;
    }
  }
}
#endif // DO_LINLINE

#ifndef P_NUMBERS_H

void nlMPZ(mpz_t m, number &n, const coeffs r)
{
  nlTest(n, r);
  nlNormalize(n, r);
  if (SR_HDL(n) & SR_INT) mpz_init_set_si(m, SR_TO_INT(n));     /* n fits in an int */
  else             mpz_init_set(m, (mpz_ptr)n->z);
}


number  nlXExtGcd (number a, number b, number *s, number *t, number *u, number *v, const coeffs r)
{
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    int uu, vv, x, y;
    int g = int_extgcd(SR_TO_INT(a), SR_TO_INT(b), &uu, &vv, &x, &y);
    *s = INT_TO_SR(uu);
    *t = INT_TO_SR(vv);
    *u = INT_TO_SR(x);
    *v = INT_TO_SR(y);
    return INT_TO_SR(g);
  }
  else
  {
    mpz_t aa, bb;
    if (SR_HDL(a) & SR_INT)
    {
      mpz_init_set_si(aa, SR_TO_INT(a));
    }
    else
    {
      mpz_init_set(aa, a->z);
    }
    if (SR_HDL(b) & SR_INT)
    {
      mpz_init_set_si(bb, SR_TO_INT(b));
    }
    else
    {
      mpz_init_set(bb, b->z);
    }
    mpz_t erg; mpz_t bs; mpz_t bt;
    mpz_init(erg);
    mpz_init(bs);
    mpz_init(bt);

    mpz_gcdext(erg, bs, bt, aa, bb);

    mpz_div(aa, aa, erg);
    *u=nlInitMPZ(bb,r);
    *u=nlNeg(*u,r);
    *v=nlInitMPZ(aa,r);

    mpz_clear(aa);
    mpz_clear(bb);

    *s = nlInitMPZ(bs,r);
    *t = nlInitMPZ(bt,r);
    return nlInitMPZ(erg,r);
  }
}

number nlQuotRem (number a, number b, number * r, const coeffs R)
{
  assume(SR_TO_INT(b)!=0);
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    if (r!=NULL)
      *r = INT_TO_SR(SR_TO_INT(a) % SR_TO_INT(b));
    return INT_TO_SR(SR_TO_INT(a)/SR_TO_INT(b));
  }
  else if (SR_HDL(a) & SR_INT)
  {
    // -2^xx / 2^xx
    if ((a==INT_TO_SR(-(POW_2_28)))&&(b==INT_TO_SR(-1L)))
    {
      if (r!=NULL) *r=INT_TO_SR(0);
      return nlRInit(POW_2_28);
    }
    //a is small, b is not, so q=0, r=a
    if (r!=NULL)
      *r = a;
    return INT_TO_SR(0);
  }
  else if (SR_HDL(b) & SR_INT)
  {
    unsigned long rr;
    mpz_t qq;
    mpz_init(qq);
    mpz_t rrr;
    mpz_init(rrr);
    rr = mpz_divmod_ui(qq, rrr, a->z, (unsigned long)ABS(SR_TO_INT(b)));
    mpz_clear(rrr);

    if (r!=NULL)
      *r = INT_TO_SR(rr);
    if (SR_TO_INT(b)<0)
    {
      mpz_neg(qq, qq);
    }
    return nlInitMPZ(qq,R);
  }
  mpz_t qq,rr;
  mpz_init(qq);
  mpz_init(rr);
  mpz_divmod(qq, rr, a->z, b->z);
  if (r!=NULL)
    *r = nlInitMPZ(rr,R);
  else
  {
    mpz_clear(rr);
  }
  return nlInitMPZ(qq,R);
}

void nlInpGcd(number &a, number b, const coeffs r)
{
  if ((SR_HDL(b)|SR_HDL(a))&SR_INT)
  {
    number n=nlGcd(a,b,r);
    nlDelete(&a,r);
    a=n;
  }
  else
  {
    mpz_gcd(a->z,a->z,b->z);
    a=nlShort3_noinline(a);
  }
}

void nlInpIntDiv(number &a, number b, const coeffs r)
{
  if ((SR_HDL(b)|SR_HDL(a))&SR_INT)
  {
    number n=nlIntDiv(a,b, r);
    nlDelete(&a,r);
    a=n;
  }
  else
  {
    number rr=nlIntMod(a,b,r);
    if (SR_HDL(rr) &  SR_INT) mpz_sub_ui(a->z,a->z,SR_TO_INT(rr));
    else                      mpz_sub(a->z,a->z,rr->z);
    mpz_divexact(a->z,a->z,b->z);
    a=nlShort3_noinline(a);
  }
}

number nlFarey(number nN, number nP, const coeffs r)
{
  mpz_t A,B,C,D,E,N,P,tmp;
  if (SR_HDL(nP) & SR_INT) mpz_init_set_si(P,SR_TO_INT(nP));
  else                     mpz_init_set(P,nP->z);
  const mp_bitcnt_t bits=2*(mpz_size1(P)+1)*GMP_LIMB_BITS;
  mpz_init2(N,bits);
  if (SR_HDL(nN) & SR_INT) mpz_set_si(N,SR_TO_INT(nN));
  else                     mpz_set(N,nN->z);
  assume(!mpz_isNeg(P));
  if (mpz_isNeg(N))  mpz_add(N,N,P);
  mpz_init2(A,bits); mpz_set_ui(A,0L);
  mpz_init2(B,bits); mpz_set_ui(B,1L);
  mpz_init2(C,bits); mpz_set_ui(C,0L);
  mpz_init2(D,bits);
  mpz_init2(E,bits); mpz_set(E,P);
  mpz_init2(tmp,bits);
  number z=INT_TO_SR(0);
  while(mpz_sgn1(N)!=0)
  {
    mpz_mul(tmp,N,N);
    mpz_add(tmp,tmp,tmp);
    if (mpz_cmp(tmp,P)<0)
    {
       if (mpz_isNeg(B))
       {
         mpz_neg(B,B);
         mpz_neg(N,N);
       }
       // check for gcd(N,B)==1
       mpz_gcd(tmp,N,B);
       if (mpz_cmp_ui(tmp,1)==0)
       {
         // return N/B
         z=ALLOC_RNUMBER();
         #ifdef LDEBUG
         z->debug=123456;
         #endif
         mpz_init_set(z->z,N);
         mpz_init_set(z->n,B);
         z->s = 0;
         nlNormalize(z,r);
       }
       else
       {
         // return nN (the input) instead of "fail"
         z=nlCopy(nN,r);
       }
       break;
    }
    //mpz_mod(D,E,N);
    //mpz_div(tmp,E,N);
    mpz_divmod(tmp,D,E,N);
    mpz_mul(tmp,tmp,B);
    mpz_sub(C,A,tmp);
    mpz_set(E,N);
    mpz_set(N,D);
    mpz_set(A,B);
    mpz_set(B,C);
  }
  mpz_clear(tmp);
  mpz_clear(A);
  mpz_clear(B);
  mpz_clear(C);
  mpz_clear(D);
  mpz_clear(E);
  mpz_clear(N);
  mpz_clear(P);
  return z;
}

number nlExtGcd(number a, number b, number *s, number *t, const coeffs)
{
  mpz_ptr aa,bb;
  *s=ALLOC_RNUMBER();
  mpz_init((*s)->z); (*s)->s=3;
  (*t)=ALLOC_RNUMBER();
  mpz_init((*t)->z); (*t)->s=3;
  number g=ALLOC_RNUMBER();
  mpz_init(g->z); g->s=3;
  #ifdef LDEBUG
  g->debug=123456;
  (*s)->debug=123456;
  (*t)->debug=123456;
  #endif
  if (SR_HDL(a) & SR_INT)
  {
    aa=(mpz_ptr)omAlloc(sizeof(mpz_t));
    mpz_init_set_si(aa,SR_TO_INT(a));
  }
  else
  {
    aa=a->z;
  }
  if (SR_HDL(b) & SR_INT)
  {
    bb=(mpz_ptr)omAlloc(sizeof(mpz_t));
    mpz_init_set_si(bb,SR_TO_INT(b));
  }
  else
  {
    bb=b->z;
  }
  mpz_gcdext(g->z,(*s)->z,(*t)->z,aa,bb);
  g=nlShort3(g);
  (*s)=nlShort3((*s));
  (*t)=nlShort3((*t));
  if (SR_HDL(a) & SR_INT)
  {
    mpz_clear(aa);
    omFreeSize(aa, sizeof(mpz_t));
  }
  if (SR_HDL(b) & SR_INT)
  {
    mpz_clear(bb);
    omFreeSize(bb, sizeof(mpz_t));
  }
  return g;
}

void    nlCoeffWrite  (const coeffs r, BOOLEAN /*details*/)
{
  if (r->is_field)
  PrintS("QQ");
  else
  PrintS("ZZ");
}

VAR int n_SwitchChinRem=0;
number   nlChineseRemainderSym(number *x, number *q,int rl, BOOLEAN sym, CFArray &inv_cache,const coeffs CF)
// elemenst in the array are x[0..(rl-1)], q[0..(rl-1)]
{
  setCharacteristic( 0 ); // only in char 0
  Off(SW_RATIONAL);
  CFArray X(rl), Q(rl);
  int i;
  for(i=rl-1;i>=0;i--)
  {
    X[i]=CF->convSingNFactoryN(x[i],FALSE,CF); // may be larger MAX_INT
    Q[i]=CF->convSingNFactoryN(q[i],FALSE,CF); // may be larger MAX_INT
  }
  CanonicalForm xnew,qnew;
  if (n_SwitchChinRem)
    chineseRemainder(X,Q,xnew,qnew);
  else
    chineseRemainderCached(X,Q,xnew,qnew,inv_cache);
  number n=CF->convFactoryNSingN(xnew,CF);
  if (sym)
  {
    number p=CF->convFactoryNSingN(qnew,CF);
    number p2;
    if (getCoeffType(CF) == n_Q) p2=nlIntDiv(p,nlInit(2, CF),CF);
    else                         p2=CF->cfDiv(p,CF->cfInit(2, CF),CF);
    if (CF->cfGreater(n,p2,CF))
    {
       number n2=CF->cfSub(n,p,CF);
       CF->cfDelete(&n,CF);
       n=n2;
    }
    CF->cfDelete(&p2,CF);
    CF->cfDelete(&p,CF);
  }
  CF->cfNormalize(n,CF);
  return n;
}
#if 0
number nlChineseRemainder(number *x, number *q,int rl, const coeffs C)
{
  CFArray inv(rl);
  return nlChineseRemainderSym(x,q,rl,TRUE,inv,C);
}
#endif

static void nlClearContent(ICoeffsEnumerator& numberCollectionEnumerator, number& c, const coeffs cf)
{
  assume(cf != NULL);

  numberCollectionEnumerator.Reset();

  if( !numberCollectionEnumerator.MoveNext() ) // empty zero polynomial?
  {
    c = nlInit(1, cf);
    return;
  }

  // all coeffs are given by integers!!!

  // part 1, find a small candidate for gcd
  number cand1,cand;
  int s1,s;
  s=2147483647; // max. int

  const BOOLEAN lc_is_pos=nlGreaterZero(numberCollectionEnumerator.Current(),cf);

  int normalcount = 0;
  do
  {
    number& n = numberCollectionEnumerator.Current();
    nlNormalize(n, cf); ++normalcount;
    cand1 = n;

    if (SR_HDL(cand1)&SR_INT) { cand=cand1; break; }
    assume(cand1->s==3); // all coeffs should be integers // ==0?!! after printing
    s1=mpz_size1(cand1->z);
    if (s>s1)
    {
      cand=cand1;
      s=s1;
    }
  } while (numberCollectionEnumerator.MoveNext() );

//  assume( nlGreaterZero(cand,cf) ); // cand may be a negative integer!

  cand=nlCopy(cand,cf);
  // part 2: compute gcd(cand,all coeffs)

  numberCollectionEnumerator.Reset();

  while (numberCollectionEnumerator.MoveNext() )
  {
    number& n = numberCollectionEnumerator.Current();

    if( (--normalcount) <= 0)
      nlNormalize(n, cf);

    nlInpGcd(cand, n, cf);
    assume( nlGreaterZero(cand,cf) );

    if(nlIsOne(cand,cf))
    {
      c = cand;

      if(!lc_is_pos)
      {
        // make the leading coeff positive
        c = nlNeg(c, cf);
        numberCollectionEnumerator.Reset();

        while (numberCollectionEnumerator.MoveNext() )
        {
          number& nn = numberCollectionEnumerator.Current();
          nn = nlNeg(nn, cf);
        }
      }
      return;
    }
  }

  // part3: all coeffs = all coeffs / cand
  if (!lc_is_pos)
    cand = nlNeg(cand,cf);

  c = cand;
  numberCollectionEnumerator.Reset();

  while (numberCollectionEnumerator.MoveNext() )
  {
    number& n = numberCollectionEnumerator.Current();
    number t=nlIntDiv(n, cand, cf); // simple integer exact division, no ratios to remain
    nlDelete(&n, cf);
    n = t;
  }
}

static void nlClearDenominators(ICoeffsEnumerator& numberCollectionEnumerator, number& c, const coeffs cf)
{
  assume(cf != NULL);

  numberCollectionEnumerator.Reset();

  if( !numberCollectionEnumerator.MoveNext() ) // empty zero polynomial?
  {
    c = nlInit(1, cf);
//    assume( n_GreaterZero(c, cf) );
    return;
  }

  // all coeffs are given by integers after returning from this routine

  // part 1, collect product of all denominators /gcds
  number cand;
  cand=ALLOC_RNUMBER();
#if defined(LDEBUG)
  cand->debug=123456;
#endif
  cand->s=3;

  int s=0;

  const BOOLEAN lc_is_pos=nlGreaterZero(numberCollectionEnumerator.Current(),cf);

  do
  {
    number& cand1 = numberCollectionEnumerator.Current();

    if (!(SR_HDL(cand1)&SR_INT))
    {
      nlNormalize(cand1, cf);
      if ((!(SR_HDL(cand1)&SR_INT)) // not a short int
      && (cand1->s==1))             // and is a normalised rational
      {
        if (s==0) // first denom, we meet
        {
          mpz_init_set(cand->z, cand1->n); // cand->z = cand1->n
          s=1;
        }
        else // we have already something
        {
          mpz_lcm(cand->z, cand->z, cand1->n);
        }
      }
    }
  }
  while (numberCollectionEnumerator.MoveNext() );


  if (s==0) // nothing to do, all coeffs are already integers
  {
//    mpz_clear(tmp);
    FREE_RNUMBER(cand);
    if (lc_is_pos)
      c=nlInit(1,cf);
    else
    {
      // make the leading coeff positive
      c=nlInit(-1,cf);

      // TODO: incorporate the following into the loop below?
      numberCollectionEnumerator.Reset();
      while (numberCollectionEnumerator.MoveNext() )
      {
        number& n = numberCollectionEnumerator.Current();
        n = nlNeg(n, cf);
      }
    }
//    assume( n_GreaterZero(c, cf) );
    return;
  }

  cand = nlShort3(cand);

  // part2: all coeffs = all coeffs * cand
  // make the lead coeff positive
  numberCollectionEnumerator.Reset();

  if (!lc_is_pos)
    cand = nlNeg(cand, cf);

  c = cand;

  while (numberCollectionEnumerator.MoveNext() )
  {
    number &n = numberCollectionEnumerator.Current();
    nlInpMult(n, cand, cf);
  }

}

char * nlCoeffName(const coeffs r)
{
  if (r->cfDiv==nlDiv) return (char*)"QQ";
  else                 return (char*)"ZZ";
}

static char* nlCoeffString(const coeffs r)
{
  //return omStrDup(nlCoeffName(r));
  if (r->cfDiv==nlDiv) return omStrDup("QQ");
  else                 return omStrDup("ZZ");
}

void nlWriteFd(number n, const ssiInfo* d, const coeffs)
{
  if(SR_HDL(n) & SR_INT)
  {
    #if SIZEOF_LONG == 4
    fprintf(d->f_write,"4 %ld ",SR_TO_INT(n));
    #else
    long nn=SR_TO_INT(n);
    if ((nn<POW_2_28_32)&&(nn>= -POW_2_28_32))
    {
      int nnn=(int)nn;
      fprintf(d->f_write,"4 %d ",nnn);
    }
    else
    {
      mpz_t tmp;
      mpz_init_set_si(tmp,nn);
      fputs("8 ",d->f_write);
      mpz_out_str (d->f_write,SSI_BASE, tmp);
      fputc(' ',d->f_write);
      mpz_clear(tmp);
    }
    #endif
  }
  else if (n->s<2)
  {
    //gmp_fprintf(f,"%d %Zd %Zd ",n->s,n->z,n->n);
    fprintf(d->f_write,"%d ",n->s+5);
    mpz_out_str (d->f_write,SSI_BASE, n->z);
    fputc(' ',d->f_write);
    mpz_out_str (d->f_write,SSI_BASE, n->n);
    fputc(' ',d->f_write);

    //if (d->f_debug!=NULL) gmp_fprintf(d->f_debug,"number: s=%d gmp/gmp \"%Zd %Zd\" ",n->s,n->z,n->n);
  }
  else /*n->s==3*/
  {
    //gmp_fprintf(d->f_write,"3 %Zd ",n->z);
    fputs("8 ",d->f_write);
    mpz_out_str (d->f_write,SSI_BASE, n->z);
    fputc(' ',d->f_write);

    //if (d->f_debug!=NULL) gmp_fprintf(d->f_debug,"number: gmp \"%Zd\" ",n->z);
  }
}

number nlReadFd(const ssiInfo *d, const coeffs)
{
  int sub_type=-1;
  sub_type=s_readint(d->f_read);
  switch(sub_type)
  {
     case 0:
     case 1:
       {// read mpz_t, mpz_t
         number n=nlRInit(0);
         mpz_init(n->n);
         s_readmpz(d->f_read,n->z);
         s_readmpz(d->f_read,n->n);
         n->s=sub_type;
         return n;
       }

     case 3:
       {// read mpz_t
         number n=nlRInit(0);
         s_readmpz(d->f_read,n->z);
         n->s=3; /*sub_type*/
         #if SIZEOF_LONG == 8
         n=nlShort3(n);
         #endif
         return n;
       }
     case 4:
       {
         LONG dd=s_readlong(d->f_read);
         //#if SIZEOF_LONG == 8
         return INT_TO_SR(dd);
         //#else
         //return nlInit(dd,NULL);
         //#endif
       }
     case 5:
     case 6:
       {// read raw mpz_t, mpz_t
         number n=nlRInit(0);
         mpz_init(n->n);
         s_readmpz_base (d->f_read,n->z, SSI_BASE);
         s_readmpz_base (d->f_read,n->n, SSI_BASE);
         n->s=sub_type-5;
         return n;
       }
     case 8:
       {// read raw mpz_t
         number n=nlRInit(0);
         s_readmpz_base (d->f_read,n->z, SSI_BASE);
         n->s=sub_type=3; /*subtype-5*/
         #if SIZEOF_LONG == 8
         n=nlShort3(n);
         #endif
         return n;
       }

     default: Werror("error in reading number: invalid subtype %d",sub_type);
              return NULL;
  }
  return NULL;
}

BOOLEAN nlCoeffIsEqual(const coeffs r, n_coeffType n, void *p)
{
  /* test, if r is an instance of nInitCoeffs(n,parameter) */
  /* if parameter is not needed */
  if (n==r->type)
  {
    if ((p==NULL)&&(r->cfDiv==nlDiv)) return TRUE;
    if ((p!=NULL)&&(r->cfDiv!=nlDiv)) return TRUE;
  }
  return FALSE;
}

static number nlLcm(number a,number b,const coeffs r)
{
  number g=nlGcd(a,b,r);
  number n1=nlMult(a,b,r);
  number n2=nlIntDiv(n1,g,r);
  nlDelete(&g,r);
  nlDelete(&n1,r);
  return n2;
}

static number nlRandom(siRandProc p, number v2, number, const coeffs cf)
{
  number a=nlInit(p(),cf);
  if (v2!=NULL)
  {
    number b=nlInit(p(),cf);
    number c=nlDiv(a,b,cf);
    nlDelete(&b,cf);
    nlDelete(&a,cf);
    a=c;
  }
  return a;
}

BOOLEAN nlInitChar(coeffs r, void*p)
{
  r->is_domain=TRUE;
  r->rep=n_rep_gap_rat;

  r->nCoeffIsEqual=nlCoeffIsEqual;
  //r->cfKillChar = ndKillChar; /* dummy */
  r->cfCoeffString=nlCoeffString;
  r->cfCoeffName=nlCoeffName;

  r->cfInitMPZ = nlInitMPZ;
  r->cfMPZ  = nlMPZ;

  r->cfMult  = nlMult;
  r->cfSub   = nlSub;
  r->cfAdd   = nlAdd;
  r->cfExactDiv= nlExactDiv;
  if (p==NULL) /* Q */
  {
    r->is_field=TRUE;
    r->cfDiv   = nlDiv;
    //r->cfGcd  = ndGcd_dummy;
    r->cfSubringGcd  = nlGcd;
  }
  else /* Z: coeffs_BIGINT */
  {
    r->is_field=FALSE;
    r->cfDiv   = nlIntDiv;
    r->cfIntMod= nlIntMod;
    r->cfGcd  = nlGcd;
    r->cfDivBy=nlDivBy;
    r->cfDivComp = nlDivComp;
    r->cfIsUnit = nlIsUnit;
    r->cfGetUnit = nlGetUnit;
    r->cfQuot1 = nlQuot1;
    r->cfLcm = nlLcm;
    r->cfXExtGcd=nlXExtGcd;
    r->cfQuotRem=nlQuotRem;
  }
  r->cfInit = nlInit;
  r->cfSize  = nlSize;
  r->cfInt  = nlInt;

  r->cfChineseRemainder=nlChineseRemainderSym;
  r->cfFarey=nlFarey;
  r->cfInpNeg   = nlNeg;
  r->cfInvers= nlInvers;
  r->cfCopy  = nlCopy;
  r->cfRePart = nlCopy;
  //r->cfImPart = ndReturn0;
  r->cfWriteLong = nlWrite;
  r->cfRead = nlRead;
  r->cfNormalize=nlNormalize;
  r->cfGreater = nlGreater;
  r->cfEqual = nlEqual;
  r->cfIsZero = nlIsZero;
  r->cfIsOne = nlIsOne;
  r->cfIsMOne = nlIsMOne;
  r->cfGreaterZero = nlGreaterZero;
  r->cfPower = nlPower;
  r->cfGetDenom = nlGetDenom;
  r->cfGetNumerator = nlGetNumerator;
  r->cfExtGcd = nlExtGcd; // only for ring stuff and Z
  r->cfNormalizeHelper  = nlNormalizeHelper;
  r->cfDelete= nlDelete;
  r->cfSetMap = nlSetMap;
  //r->cfName = ndName;
  r->cfInpMult=nlInpMult;
  r->cfInpAdd=nlInpAdd;
  r->cfCoeffWrite=nlCoeffWrite;

  r->cfClearContent = nlClearContent;
  r->cfClearDenominators = nlClearDenominators;

#ifdef LDEBUG
  // debug stuff
  r->cfDBTest=nlDBTest;
#endif
  r->convSingNFactoryN=nlConvSingNFactoryN;
  r->convFactoryNSingN=nlConvFactoryNSingN;

  r->cfRandom=nlRandom;

  // io via ssi
  r->cfWriteFd=nlWriteFd;
  r->cfReadFd=nlReadFd;

  //r->type = n_Q;
  r->ch = 0;
  r->has_simple_Alloc=FALSE;
  r->has_simple_Inverse=FALSE;

  // variables for this type of coeffs:
  // (none)
  return FALSE;
}
#if 0
number nlMod(number a, number b)
{
  if (((SR_HDL(b)&SR_HDL(a))&SR_INT)
  {
    int bi=SR_TO_INT(b);
    int ai=SR_TO_INT(a);
    int bb=ABS(bi);
    int c=ai%bb;
    if (c<0)  c+=bb;
    return (INT_TO_SR(c));
  }
  number al;
  number bl;
  if (SR_HDL(a))&SR_INT)
    al=nlRInit(SR_TO_INT(a));
  else
    al=nlCopy(a);
  if (SR_HDL(b))&SR_INT)
    bl=nlRInit(SR_TO_INT(b));
  else
    bl=nlCopy(b);
  number r=nlRInit(0);
  mpz_mod(r->z,al->z,bl->z);
  nlDelete(&al);
  nlDelete(&bl);
  if (mpz_size1(&r->z)<=MP_SMALL)
  {
    LONG ui=(int)mpz_get_si(&r->z);
    if ((((ui<<3)>>3)==ui)
    && (mpz_cmp_si(x->z,(long)ui)==0))
    {
      mpz_clear(&r->z);
      FREE_RNUMBER(r); //  omFreeBin((void *)r, rnumber_bin);
      r=INT_TO_SR(ui);
    }
  }
  return r;
}
#endif
#endif // not P_NUMBERS_H
#endif // LONGRAT_CC
