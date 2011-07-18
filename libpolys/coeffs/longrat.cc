/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: computation with long rational numbers (Hubert Grassmann)
*/

#include <coeffs/longrat.h>

// 64 bit version:
#if 0
#define MAX_NUM_SIZE 60
#define POW_2_28 0x10000000000000L
#define LONG long
#else
#define MAX_NUM_SIZE 28
#define POW_2_28 (1L<<28)
#define LONG int
#endif

static inline number nlShort3(number x) // assume x->s==3
{
  assume(x->s==3);
  if ((mpz_cmp_ui(x->z,(long)0)==0)
  || (mpz_size1(x->z)<=MP_SMALL))
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

#include <string.h>
#include <float.h>
#include "config.h"
#include <coeffs/coeffs.h>
#include <reporter/reporter.h>
#include <omalloc/omalloc.h>
#include <coeffs/numbers.h>
#include <coeffs/modulop.h>
#include <coeffs/shortfl.h>
#include <coeffs/mpr_complex.h>
#include <coeffs/longrat.h>


#ifndef BYTES_PER_MP_LIMB
#define BYTES_PER_MP_LIMB sizeof(mp_limb_t)
#endif

/*-----------------------------------------------------------------*/
/*3
* parameter s in number:
* 0 (or FALSE): not normalised rational
* 1 (or TRUE):  normalised rational
* 3          :  integer with n==NULL
*/
/*3
**  'SR_INT' is the type of those integers small enough to fit into  29  bits.
**  Therefor the value range of this small integers is: $-2^{28}...2^{28}-1$.
**
**  Small integers are represented by an immediate integer handle, containing
**  the value instead of pointing  to  it,  which  has  the  following  form:
**
**      +-------+-------+-------+-------+- - - -+-------+-------+-------+
**      | guard | sign  | bit   | bit   |       | bit   | tag   | tag   |
**      | bit   | bit   | 27    | 26    |       | 0     | 0     | 1     |
**      +-------+-------+-------+-------+- - - -+-------+-------+-------+
**
**  Immediate integers handles carry the tag 'SR_INT', i.e. the last bit is 1.
**  This distuingishes immediate integers from other handles which  point  to
**  structures aligned on 4 byte boundaries and therefor have last bit  zero.
**  (The second bit is reserved as tag to allow extensions of  this  scheme.)
**  Using immediates as pointers and dereferencing them gives address errors.
**
**  To aid overflow check the most significant two bits must always be equal,
**  that is to say that the sign bit of immediate integers has a  guard  bit.
**
**  The macros 'INT_TO_SR' and 'SR_TO_INT' should be used to convert  between
**  a small integer value and its representation as immediate integer handle.
**
**  Large integers and rationals are represented by z and n
**  where n may be undefined (if s==3)
**  NULL represents only deleted values
*/
#define SR_HDL(A) ((long)(A))
/*#define SR_INT    1L*/
/*#define INT_TO_SR(INT)  ((number) (((long)INT << 2) + SR_INT))*/
// #define SR_TO_INT(SR)   (((long)SR) >> 2)

#define MP_SMALL 1
//#define mpz_isNeg(A) (mpz_cmp_si(A,(long)0)<0)
#define mpz_isNeg(A) ((A)->_mp_size<0)
#define mpz_limb_size(A) ((A)->_mp_size)
#define mpz_limb_d(A) ((A)->_mp_d)
#define MPZ_DIV(A,B,C) mpz_tdiv_q((A),(B),(C))
#define MPZ_EXACTDIV(A,B,C) mpz_divexact((A),(B),(C))

void    _nlDelete_NoImm(number *a);

static const n_coeffType ID = n_long_R;

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

//#ifndef SI_THREADS
omBin rnumber_bin = omGetSpecBin(sizeof(snumber));
//#endif

number nlOne=INT_TO_SR(1);

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
  assume( getCoeffType(dst) == ID );
  assume( getCoeffType(src) == n_Zp );
  
  number to;
  to = nlInit(npInt(from,src), dst);
  return to;
}

static number nlMapLongR(number from, const coeffs src, const coeffs dst);
static number nlMapR(number from, const coeffs src, const coeffs dst);

#ifdef HAVE_RINGS
/*2
* convert from a GMP integer
*/
number nlMapGMP(number from, const coeffs src, const coeffs dst)
{
  number z=ALLOC_RNUMBER();
#if defined(LDEBUG)
  z->debug=123456;
#endif
  mpz_init_set(z->z,(mpz_ptr) from);
  //mpz_init_set_ui(&z->n,1);
  z->s = 3;
  z=nlShort3(z);
  return z;
}

/*2
* convert from an machine long
*/
number nlMapMachineInt(number from, const coeffs src, const coeffs dst)
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
BOOLEAN nlDBTest(number a, const char *f,const int l, const coeffs r)
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
  //omCheckIf(omCheckAddrSize(a,sizeof(*a)), return FALSE);
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
    /* TODO: If next line is active, then computations in algebraic field
             extensions over Q will throw a lot of assume violations although
             everything is computed correctly and no seg fault appears.
             Maybe the test is not appropriate in this case. */
    //omCheckIf(omCheckAddrSize(a->n[0]._mp_d,a->n[0]._mp_alloc*BYTES_PER_MP_LIMB), return FALSE);
    if (a->z[0]._mp_alloc==0)
      Print("!!longrat:n->alloc=0 in %s:%d\n",f,l);
    if ((mpz_size1(a->n) ==1) && (mpz_cmp_si(a->n,(long)1)==0))
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
  LONG ui=(int)mpz_get_si(a->z);
  if ((((ui<<3)>>3)==ui)
  && (mpz_cmp_si(a->z,(long)ui)==0))
  {
    Print("!!longrat:im int %d in %s:%d\n",ui,f,l);
    f=NULL;
    return FALSE;
  }
  return TRUE;
}
#endif

#ifdef HAVE_FACTORY
CanonicalForm nlConvSingNFactoryN( number n, BOOLEAN setChar, const coeffs r )
{
  if (setChar) setCharacteristic( 0 );

  CanonicalForm term;
  if ( SR_HDL(n) & SR_INT )
  {
    term = SR_TO_INT(n);
  }
  else
  {
    if ( n->s == 3 )
    {
      MP_INT dummy;
      mpz_init_set( &dummy,n->z );
      term = make_cf( dummy );
    }
    else
    {
      // assume s==0 or s==1
      MP_INT num, den;
      On(SW_RATIONAL);
      mpz_init_set( &num, n->z );
      mpz_init_set( &den, n->n );
      term = make_cf( num, den, ( n->s != 1 ));
    }
  }
  return term;
}

number nlConvFactoryNSingN( const CanonicalForm n, const coeffs r)
{
  if (n.isImm())
  {
    return nlInit(n.intval(),r);
  }
  else
  {
    number z=(number)omAllocBin(rnumber_bin);
#if defined(LDEBUG)
    z->debug=123456;
#endif
    gmp_numerator( n, z->z );
    if ( n.den().isOne() )
      z->s = 3;
    else
    {
      gmp_denominator( n, z->n );
      z->s = 0;
    }
    nlNormalize(z,r);
    return z;
  }
}
#endif
number nlRInit (long i);

static number nlMapR(number from, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == ID );
  assume( getCoeffType(src) == n_R );
  
  double f=nrFloat(from);
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
  assume( getCoeffType(dst) == ID );
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
  if (negative) dest->_mp_size = -dest->_mp_size;

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
     return 1; /* immidiate int */
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
int nlInt(number &i, const coeffs r)
{
  nlTest(i, r);
  nlNormalize(i,r);
  if (SR_HDL(i) &SR_INT) return SR_TO_INT(i);
  if (i->s==3)
  {
    if(mpz_size1(i->z)>MP_SMALL) return 0;
    int ul=(int)mpz_get_si(i->z);
    if (mpz_cmp_si(i->z,(long)ul)!=0) return 0;
    return ul;
  }
  mpz_t tmp;
  int ul;
  mpz_init(tmp);
  MPZ_DIV(tmp,i->z,i->n);
  if(mpz_size1(tmp)>MP_SMALL) ul=0;
  else
  {
    ul=(int)mpz_get_si(tmp);
    if (mpz_cmp_si(tmp,(long)ul)!=0) ul=0;
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
  if (SR_HDL(i) &SR_INT) return (i);
  if (i->s==3)
  {
    return nlCopy(i,r);
  }
  number tmp=nlRInit(1);
  MPZ_DIV(tmp->z,i->z,i->n);
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
    if ((long)a>0L)
    {
      mpz_init_set_si(n->z,(long)1);
      mpz_init_set_si(n->n,(long)SR_TO_INT(a));
    }
    else
    {
      mpz_init_set_si(n->z,(long)-1);
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
    n->s=a->s;
    mpz_init_set(n->n,a->z);
    switch (a->s)
    {
      case 0:
      case 1:
              mpz_init_set(n->z,a->n);
              if (mpz_isNeg(n->n)) /* && n->s<2*/
              {
                mpz_neg(n->z,n->z);
                mpz_neg(n->n,n->n);
              }
              if (mpz_cmp_si(n->n,(long)1)==0)
              {
                mpz_clear(n->n);
                n->s=3;
                n=nlShort3(n);
              }
              break;
      case 3:
              n->s=1;
              if (mpz_isNeg(n->n)) /* && n->s<2*/
              {
                mpz_neg(n->n,n->n);
                mpz_init_set_si(n->z,(long)-1);
              }
              else
              {
                mpz_init_set_si(n->z,(long)1);
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
  /* u=a/b */
  u->s = 3;
  MPZ_EXACTDIV(u->z,a->z,b->z);
  if (bb!=NULL)
  {
    mpz_clear(bb->z);
#if defined(LDEBUG)
    bb->debug=654324;
#endif
    omFreeBin((void *)bb, rnumber_bin);
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
    long aa=SR_TO_INT(a);
    long bb=SR_TO_INT(b);
    return INT_TO_SR(aa/bb);
  }
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
    /* a is a small and b is a large int: -> 0 */
    return INT_TO_SR(0);
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
  MPZ_DIV(u->z,u->z,b->z);
  if (bb!=NULL)
  {
    mpz_clear(bb->z);
#if defined(LDEBUG)
    bb->debug=654324;
#endif
    omFreeBin((void *)bb, rnumber_bin);
  }
  u=nlShort3(u);
  nlTest(u, r);
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
    if ((long)a>0L)
    {
      if ((long)b>0L)
        return INT_TO_SR(SR_TO_INT(a)%SR_TO_INT(b));
      else
        return INT_TO_SR(SR_TO_INT(a)%(-SR_TO_INT(b)));
    }
    else
    {
      if ((long)b>0L)
      {
        long i=(-SR_TO_INT(a))%SR_TO_INT(b);
        if ( i != 0L ) i = (SR_TO_INT(b))-i;
        return INT_TO_SR(i);
      }
      else
      {
        long i=(-SR_TO_INT(a))%(-SR_TO_INT(b));
        if ( i != 0L ) i = (-SR_TO_INT(b))-i;
        return INT_TO_SR(i);
      }
    }
  }
  if (SR_HDL(a) & SR_INT)
  {
    /* a is a small and b is a large int: -> a or (a+b) or (a-b) */
    if ((long)a<0L)
    {
      if (mpz_isNeg(b->z))
        return nlSub(a,b,r);
      /*else*/
        return nlAdd(a,b,r);
    }
    /*else*/
      return a;
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
  mpz_mod(u->z,a->z,b->z);
  if (bb!=NULL)
  {
    mpz_clear(bb->z);
#if defined(LDEBUG)
    bb->debug=654324;
#endif
    omFreeBin((void *)bb, rnumber_bin);
  }
  if (mpz_isNeg(u->z))
  {
    if (mpz_isNeg(b->z))
      mpz_sub(u->z,u->z,b->z);
    else
      mpz_add(u->z,u->z,b->z);
  }
  u=nlShort3(u);
  nlTest(u, r);
  return u;
}

/*2
* u := a / b
*/
number nlDiv (number a, number b, const coeffs r)
{
  number u;
  if (nlIsZero(b,r))
  {
    WerrorS(nDivBy0);
    return INT_TO_SR(0);
  }
  u=ALLOC_RNUMBER();
  u->s=0;
#if defined(LDEBUG)
  u->debug=123456;
#endif
// ---------- short / short ------------------------------------
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    long i=SR_TO_INT(a);
    long j=SR_TO_INT(b);
    if ((i==-POW_2_28) && (j== -1L))
    {  
      FREE_RNUMBER(u);
      return nlRInit(POW_2_28);
    }
    long r=i%j;
    if (r==0)
    {
      omFreeBin((void *)u, rnumber_bin);
      return INT_TO_SR(i/j);
    }
    mpz_init_set_si(u->z,(long)i);
    mpz_init_set_si(u->n,(long)j);
  }
  else
  {
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
        if ((long)b>0L)
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
  if (mpz_cmp_si(u->n,(long)1)==0)
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
  if (!nlIsZero(x,r))
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
      if (mpz_cmp_si(x->n,(long)1)==0)
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
  else if (exp==0)
    *u = INT_TO_SR(1); // 0^0
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
  //if (SR_HDL(a) & SR_INT) return (a==INT_TO_SR(-1L));
  //return FALSE;
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
    result=ALLOC_RNUMBER();
    mpz_init(result->z);
    mpz_gcd(result->z,a->z,b->z);
    result->s = 3;
  #ifdef LDEBUG
    result->debug=123456;
  #endif
    result=nlShort3(result);
  }
  nlTest(result, r);
  return result;
}

number nlShort1(number x) // assume x->s==0/1
{
  assume(x->s<2);
  if (mpz_cmp_ui(x->z,(long)0)==0)
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
    if (mpz_cmp_si(x->n,(long)1)==0)
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
      if (mpz_cmp_si(gcd,(long)1)!=0)
      {
        mpz_t r;
        mpz_init(r);
        MPZ_EXACTDIV(r,x->z,gcd);
        mpz_set(x->z,r);
        MPZ_EXACTDIV(r,x->n,gcd);
        mpz_set(x->n,r);
        mpz_clear(r);
        if (mpz_cmp_si(x->n,(long)1)==0)
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
number nlLcm(number a, number b, const coeffs r)
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
  if (mpz_cmp_si(gcd,(long)1)!=0)
  {
    mpz_t bt;
    mpz_init_set(bt,b->n);
    MPZ_EXACTDIV(bt,bt,gcd);
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

int nlModP(number n, int p, const coeffs r)
{
  if (SR_HDL(n) & SR_INT)
  {
    long i=SR_TO_INT(n);
    if (i<0L) return (((long)p)-((-i)%((long)p)));
    return i%((long)p);
  }
  int iz=(int)mpz_fdiv_ui(n->z,(unsigned long)p);
  if (n->s!=3)
  {
    int in=mpz_fdiv_ui(n->n,(unsigned long)p);
    #ifdef NV_OPS
    if (p>NV_MAX_PRIME)
    return (int)((long)nvDiv((number)iz,(number)in,(const coeffs)r));
    #endif
    return (int)((long)npDiv((number)iz,(number)in,(const coeffs)r));
  }
  return iz;
}

#ifdef HAVE_RINGS
/*2
* convert number i (from Q) to GMP and warn if denom != 1
*/
void nlGMP(number &i, number n, const coeffs r)
{
  // Hier brauche ich einfach die GMP Zahl
  nlTest(i, r);
  nlNormalize(i, r);
  if (SR_HDL(i) & SR_INT)
  {
    mpz_set_si((mpz_ptr) n, SR_TO_INT(i));
    return;
  }
  if (i->s!=3)
  {
     WarnS("Omitted denominator during coefficient mapping !");
  }
  mpz_set((mpz_ptr) n, i->z);
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
    if (((long)a > 0L) && (mpz_isNeg(b->z)))
      return FALSE;
    if (((long)a < 0L) && (!mpz_isNeg(b->z)))
      return FALSE;
    mpz_t  bb;
    mpz_init_set(bb,b->n);
    mpz_mul_si(bb,bb,(long)SR_TO_INT(a));
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
    omFreeBin((void *) *a, rnumber_bin);
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
        if (mpz_cmp_ui(u->z,(long)0)==0)
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
        break;
      }
      case 3:
      {
        if ((long)a>0L)
          mpz_add_ui(u->z,b->z,SR_TO_INT(a));
        else
          mpz_sub_ui(u->z,b->z,-SR_TO_INT(a));
        if (mpz_cmp_ui(u->z,(long)0)==0)
        {
          mpz_clear(u->z);
          FREE_RNUMBER(u);
          return INT_TO_SR(0);
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
            mpz_init(x);

            mpz_mul(x,b->z,a->n);
            mpz_mul(u->z,a->z,b->n);
            mpz_add(u->z,u->z,x);
            mpz_clear(x);

            if (mpz_cmp_ui(u->z,(long)0)==0)
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
            break;
          }
          case 3: /* a:1 b:3 */
          {
            mpz_mul(u->z,b->z,a->n);
            mpz_add(u->z,u->z,a->z);
            if (mpz_cmp_ui(u->z,(long)0)==0)
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
            if (mpz_cmp_ui(u->z,(long)0)==0)
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
            break;
          }
          case 3:
          {
            mpz_add(u->z,a->z,b->z);
            if (mpz_cmp_ui(u->z,(long)0)==0)
            {
              mpz_clear(u->z);
              FREE_RNUMBER(u);
              return INT_TO_SR(0);
            }
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
        if (mpz_cmp_ui(u->z,(long)0)==0)
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
        break;
      }
      case 3:
      {
        if ((long)a>0L)
        {
          mpz_sub_ui(u->z,b->z,SR_TO_INT(a));
          mpz_neg(u->z,u->z);
        }
        else
        {
          mpz_add_ui(u->z,b->z,-SR_TO_INT(a));
          mpz_neg(u->z,u->z);
        }
        if (mpz_cmp_ui(u->z,(long)0)==0)
        {
          mpz_clear(u->z);
          FREE_RNUMBER(u);
          return INT_TO_SR(0);
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
        if (mpz_cmp_ui(u->z,(long)0)==0)
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
        break;
      }
      case 3:
      {
        if ((long)b>0L)
        {
          mpz_sub_ui(u->z,a->z,SR_TO_INT(b));
        }
        else
        {
          mpz_add_ui(u->z,a->z,-SR_TO_INT(b));
        }
        if (mpz_cmp_ui(u->z,(long)0)==0)
        {
          mpz_clear(u->z);
          FREE_RNUMBER(u);
          return INT_TO_SR(0);
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
            if (mpz_cmp_ui(u->z,(long)0)==0)
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
            break;
          }
          case 3: /* a:1, b:3 */
          {
            mpz_t x;
            mpz_init(x);
            mpz_mul(x,b->z,a->n);
            mpz_sub(u->z,a->z,x);
            mpz_clear(x);
            if (mpz_cmp_ui(u->z,(long)0)==0)
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
            if (mpz_cmp_ui(u->z,(long)0)==0)
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
            break;
          }
          case 3: /* a:3 , b:3 */
          {
            mpz_sub(u->z,a->z,b->z);
            if (mpz_cmp_ui(u->z,(long)0)==0)
            {
              mpz_clear(u->z);
              FREE_RNUMBER(u);
              return INT_TO_SR(0);
            }
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
    if ((long)a>0L)
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
      }
    }
  }
  return u;
}

/*2
* copy a to b for mapping
*/
number nlCopyMap(number a, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == ID );
  assume( getCoeffType(src) == ID );
  
  if ((SR_HDL(a) & SR_INT)||(a==NULL))
  {
    return a;
  }
  return _nlCopy_NoImm(a);
}

nMapFunc nlSetMap(const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == ID );
//  assume( getCoeffType(src) == ID );
  
  if (nCoeff_is_Q(src))
  {
    return nlCopyMap;
  }
  if (nCoeff_is_Zp(src))
  {
    return nlMapP;
  }
  if (nCoeff_is_R(src))
  {
    return nlMapR;
  }
  if (nCoeff_is_long_R(src))
  {
    return nlMapLongR; /* long R -> Q */
  }
#ifdef HAVE_RINGS
  if (nCoeff_is_Ring_Z(src) || nCoeff_is_Ring_PtoM(src) || nCoeff_is_Ring_ModN(src))
  {
    return nlMapGMP;
  }
  if (nCoeff_is_Ring_2toM(src))
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
number nlRInit (int i);
BOOLEAN _nlEqual_aNoImm_OR_bNoImm(number a, number b);
number  _nlCopy_NoImm(number a);
number  _nlNeg_NoImm(number a);
number  _nlAdd_aNoImm_OR_bNoImm(number a, number b);
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


LINLINE number nlInit (int i, const coeffs r)
{
  number n;
  LONG ii=(LONG)i;
  if ( ((ii << 3) >> 3) == ii ) n=INT_TO_SR(ii);
  else                          n=nlRInit(ii);
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
  return (a==INT_TO_SR(0));
  //return (mpz_cmp_si(a->z,(long)0)==0);
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
    int r=SR_TO_INT(a);
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

LINLINE number nlInpAdd_(number a, number b, const coeffs r)
{
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    LONG r=SR_HDL(a)+SR_HDL(b)-1L;
    if ( ((r << 1) >> 1) == r )
      return (number)(long)r;
    else
      return nlRInit(SR_TO_INT(r));
  }
  // a=a+b
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
        a->s = 0;
        a=nlShort1(a);
        break;
      }
      case 3:
      {
        if ((long)b>0L)
          mpz_add_ui(a->z,a->z,SR_TO_INT(b));
        else
          mpz_sub_ui(a->z,a->z,-SR_TO_INT(b));
        a->s = 3;
        a=nlShort3_noinline(a);
        break;
      }
    }
    return a;
  }
  if (SR_HDL(a) & SR_INT)
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
        u->s = 0;
        u=nlShort1(u);
        break;
      }
      case 3:
      {
        if ((long)a>0L)
          mpz_add_ui(u->z,b->z,SR_TO_INT(a));
        else
          mpz_sub_ui(u->z,b->z,-SR_TO_INT(a));
        // result cannot be 0, if coeffs are normalized
        u->s = 3;
        u=nlShort3_noinline(u);
        break;
      }
    }
    nlTest(u, r);
    return u;
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
            a->s = 0;
            break;
          }
          case 3: /* a:1 b:3 */
          {
            mpz_t x;
            mpz_init(x);
            mpz_mul(x,b->z,a->n);
            mpz_add(a->z,a->z,x);
            mpz_clear(x);
            a->s = 0;
            break;
          }
        } /*switch (b->s) */
        a=nlShort1(a);
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
            a->s = 0;
            a=nlShort1(a);
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
    nlTest(a, r);
    return a;
  }
}

LINLINE void nlInpAdd(number &a, number b, const coeffs r)
{
  a = nlInpAdd_(a, b, r);
}


LINLINE number nlMult (number a, number b, const coeffs R)
{
  nlTest(a, R);
  nlTest(b, R);
  if (a==INT_TO_SR(0)) return INT_TO_SR(0);
  if (b==INT_TO_SR(0)) return INT_TO_SR(0);
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    LONG r=(SR_HDL(a)-1L)*(SR_HDL(b)>>1);
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
    if (mpz_isNeg(a->z))
    {
      if (mpz_isNeg(b->z))
      {
        mpz_add(a->z,a->z,b->z);
      }
      else
      {
        mpz_sub(a->z,a->z,b->z);
      }
      mpz_add_ui(a->z,a->z,1);
    }
    else
    {
      if (mpz_isNeg(b->z))
      {
        mpz_sub(a->z,a->z,b->z);
      }
      else
      {
        mpz_add(a->z,a->z,b->z);
      }
      mpz_sub_ui(a->z,a->z,1);
    }
    MPZ_DIV(a->z,a->z,b->z);
    a=nlShort3_noinline(a);
  }
}

number nlFarey(number nN, number nP, const coeffs r)
{
  mpz_t tmp; mpz_init(tmp);
  mpz_t A,B,C,D,E,N,P;
  if (SR_HDL(nN) & SR_INT) mpz_init_set_si(N,SR_TO_INT(nN));
  else                     mpz_init_set(N,nN->z);
  if (SR_HDL(nP) & SR_INT) mpz_init_set_si(P,SR_TO_INT(nP));
  else                     mpz_init_set(P,nP->z);
  assume(!mpz_isNeg(P));
  if (mpz_isNeg(N))  mpz_add(N,N,P);
  mpz_init_set_si(A,(long)0);
  mpz_init_set_ui(B,(unsigned long)1);
  mpz_init_set_si(C,(long)0);
  mpz_init(D);
  mpz_init_set(E,P);
  number z=INT_TO_SR(0);
  while(mpz_cmp_si(N,(long)0)!=0)
  {
    mpz_mul(tmp,N,N);
    mpz_add(tmp,tmp,tmp);
    if (mpz_cmp(tmp,P)<0)
    {
       // return N/B
       z=ALLOC_RNUMBER();
       #ifdef LDEBUG
       z->debug=123456;
       #endif
       if (mpz_isNeg(B))
       {
         mpz_neg(B,B);
         mpz_neg(N,N);
       }
       mpz_init_set(z->z,N);
       mpz_init_set(z->n,B);
       z->s = 0;
       nlNormalize(z,r);
       break;
    }
    mpz_mod(D,E,N);
    mpz_div(tmp,E,N);
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

void    nlCoeffWrite  (const coeffs r)
{
  PrintS("//   characteristic : 0\n");
}

BOOLEAN nlInitChar(coeffs r, void* p)
{
  const int ch = (int)(long)(p);
  
  r->cfKillChar=NULL;
  r->cfSetChar=NULL;
  r->nCoeffIsEqual=ndCoeffIsEqual;
  r->cfKillChar = ndKillChar; /* dummy */

  r->cfMult  = nlMult;
  r->cfSub   = nlSub;
  r->cfAdd   = nlAdd;
  r->cfDiv   = nlDiv;
  r->cfIntDiv= nlIntDiv;
  r->cfIntMod= nlIntMod;
  r->cfExactDiv= nlExactDiv;
  r->cfInit = nlInit;
  //r->cfPar = ndPar;
  //r->cfParDeg = ndParDeg;
  r->cfSize  = nlSize;
  r->cfInt  = nlInt;
  #ifdef HAVE_RINGS
  //r->cfDivComp = NULL; // only for ring stuff
  //r->cfIsUnit = NULL; // only for ring stuff
  //r->cfGetUnit = NULL; // only for ring stuff
  //r->cfExtGcd = NULL; // only for ring stuff
  //r->cfDivBy = NULL; // only for ring stuff
  #endif
  r->cfNeg   = nlNeg;
  r->cfInvers= nlInvers;
  r->cfCopy  = nlCopy;
  r->cfRePart = nlCopy;
  //r->cfImPart = ndReturn0;
  r->cfWrite = nlWrite;
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
  r->cfGcd  = nlGcd;
  r->cfLcm  = nlLcm;
  r->cfDelete= nlDelete;
  r->cfSetMap = nlSetMap;
  //r->cfName = ndName;
  r->cfInpMult=nlInpMult;
  r->cfInit_bigint=nlCopyMap;
  r->cfCoeffWrite=nlCoeffWrite;
#ifdef LDEBUG
  // debug stuff
  r->cfDBTest=nlDBTest;
#endif

  // the variables: general stuff (required)
  r->nNULL = INT_TO_SR(0);
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
      omFreeBin((void *)r, rnumber_bin);
      r=INT_TO_SR(ui);
    }
  }
  return r;
}
#endif
#endif // not P_NUMBERS_H
#endif // LONGRAT_CC
