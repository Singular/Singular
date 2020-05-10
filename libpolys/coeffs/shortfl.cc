/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT:
*/



#include "misc/auxiliary.h"
#include "misc/mylimits.h"

#include "reporter/reporter.h"

#include "coeffs/numbers.h"
#include "coeffs/coeffs.h"
#include "coeffs/mpr_complex.h"

#include "coeffs/shortfl.h"
#include "coeffs/longrat.h"

//#include <string.h>
#include <cmath>

// Private interface should be hidden!!!

#ifdef LDEBUG
static BOOLEAN nrDBTest(number a, const coeffs r, const char *f, const int l);
#endif

/// Get a mapping function from src into the domain of this type: n_R
static nMapFunc nrSetMap(const coeffs src, const coeffs dst);

// Where are the following used?
static number nrMapQ(number from, const coeffs r, const coeffs aRing);

static const SI_FLOAT nrEps = 1.0e-3;

union nf
{
  SI_FLOAT _f;
  number _n;

  nf(SI_FLOAT f): _f(f){};

  nf(number n): _n(n){};

  inline SI_FLOAT F() const {return _f;}
  inline number N() const {return _n;}
};




SI_FLOAT nrFloat(number n)
{
  return nf(n).F();
}

static BOOLEAN nrGreaterZero (number k, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  return nf(k).F() >= 0.0;
}

static number nrMult (number a,number b, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  return nf(nf(a).F() * nf(b).F()).N();
}

/*2
* create a number from int
*/
static number nrInit (long i, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  SI_FLOAT f = (SI_FLOAT)i;
  return nf(nf(f).F()).N();
}

/*2
* convert a number to int
*/
static long nrInt(number &n, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  long i;
  SI_FLOAT f = nf(n).F();
  if (((SI_FLOAT)(-MAX_INT_VAL-1) <= f) || ((SI_FLOAT)MAX_INT_VAL >= f))
    i = (long)f;
  else
    i = 0;
  return i;
}

static number nrAdd (number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  SI_FLOAT x = nf(a).F();
  SI_FLOAT y = nf(b).F();
  SI_FLOAT f = x + y;
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

static number nrSub (number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  SI_FLOAT x = nf(a).F();
  SI_FLOAT y = nf(b).F();
  SI_FLOAT f = x - y;
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

static BOOLEAN nrIsZero (number  a, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  return (0.0 == nf(a).F());
}

static BOOLEAN nrIsOne (number a, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  SI_FLOAT aa=nf(a).F()-1.0;
  if (aa<0.0) aa=-aa;
  return (aa<nrEps);
}

static BOOLEAN nrIsMOne (number a, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  SI_FLOAT aa=nf(a).F()+1.0;
  if (aa<0.0) aa=-aa;
  return (aa<nrEps);
}

static number nrDiv (number a,number b, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  SI_FLOAT n = nf(b).F();
  if (n == 0.0)
  {
    WerrorS(nDivBy0);
    return nf((SI_FLOAT)0.0).N();
  }
  else
    return nf(nf(a).F() / n).N();
}

static number nrInvers (number c, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  SI_FLOAT n = nf(c).F();
  if (n == 0.0)
  {
    WerrorS(nDivBy0);
    return nf((SI_FLOAT)0.0).N();
  }
  return nf(1.0 / n).N();
}

static number nrNeg (number c, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  return nf(-nf(c).F()).N();
}

static BOOLEAN nrGreater (number a,number b, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  return nf(a).F() > nf(b).F();
}

static BOOLEAN nrEqual (number a,number b, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  number x = nrSub(a,b,r);
  return nf(x).F() == nf((SI_FLOAT)0.0).F();
}

static void nrWrite (number a, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  //#if SIZEOF_DOUBLE == SIZEOF_LONG
  //char ch[16];
  //int n = sprintf(ch,"%12.6e", nf(a).F());
  //#else
  char ch[11];
  int n = sprintf(ch,"%9.3e", nf(a).F());
  //#endif
  if (ch[0] == '-')
  {
    char* chbr = new char[n+3];
    memcpy(&chbr[2],&ch[1],n-1);
    chbr[0] = '-';
    chbr[1] = '(';
    chbr[n+1] = ')';
    chbr[n+2] = '\0';
    StringAppendS(chbr);
    delete[] chbr;
  }
  else
    StringAppend("(%s)",ch);
}

#if 0
static void nrPower (number a, int i, number * result, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

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
#endif

namespace {
  static const char* nrEatr(const char *s, SI_FLOAT *r)
  {
    int i;

    if    (*s >= '0' && *s <= '9')
    {
      *r = 0.0;
      do
      {
        *r *= 10.0;
        i = *s++ - '0';
        *r += (SI_FLOAT)i;
      }
      while (*s >= '0' && *s <= '9');
    }
    else *r = 1.0;
    return s;
  }
}

static const char * nrRead (const char *s, number *a, const coeffs r)
{

  assume( getCoeffType(r) == n_R );

  static const char *nIllegalChar="illegal character in number";

  const char *t;
  const char *start=s;
  SI_FLOAT z1,z2;
  SI_FLOAT n=1.0;

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
static BOOLEAN  nrDBTest(number a, const char *f, const int l, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  return TRUE;
}
#endif

static number nrMapP(number from, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == n_R );
  assume( getCoeffType(aRing) ==  n_Zp );

  int i = (int)((long)from);
  SI_FLOAT f = (SI_FLOAT)i;
  return nf(f).N();
}

static number nrMapLongR(number from, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == n_R );
  assume( getCoeffType(aRing) == n_long_R );

  SI_FLOAT t =(SI_FLOAT)mpf_get_d((mpf_srcptr)from);
  return nf(t).N();
}

static number nrMapC(number from, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == n_R );
  assume( getCoeffType(aRing) == n_long_C );

  gmp_float h = ((gmp_complex*)from)->real();
  SI_FLOAT t =(SI_FLOAT)mpf_get_d((mpf_srcptr)&h);
  return nf(t).N();
}


static number nrMapQ(number from, const coeffs aRing, const coeffs r)
{
/* in longrat.h
#define SR_INT    1
#define mpz_size1(A) (ABS((A)->_mp_size))
*/
#define SR_HDL(A) ((long)(A))
#define IS_INT(A) ((A)->s==3)
#define IS_IMM(A) (SR_HDL(A) & SR_INT)
#define GET_NOM(A) ((A)->z)
#define GET_DENOM(A) ((A)->n)

  assume( getCoeffType(r) == n_R );
  assume( aRing->rep == n_rep_gap_rat );

  if (IS_IMM(from))
  {
    SI_FLOAT f = (SI_FLOAT)SR_TO_INT(from);
    return nf(nf(f).F()).N();
  }
  else
  {
    /* read out the enumerator */
    if (IS_INT(from))
    {
      mpf_t e;
      mpf_init(e);
      mpf_set_z(e,GET_NOM(from));
      SI_FLOAT f = mpf_get_d(e);
      mpf_clear(e);
      return nf(nf(f).F()).N();
    }
    else /*quotient*/
    {
      mpf_t z,n,q;
      mpf_init(z);
      mpf_init(n);
      mpf_init(q);
      mpf_set_z(z,GET_NOM(from));
      mpf_set_z(n,GET_DENOM(from));
      mpf_div(q,z,n);
      mpf_clear(z);
      mpf_clear(n);
      SI_FLOAT f = mpf_get_d(q);
      mpf_clear(q);
      return nf(nf(f).F()).N();
    }
  }
}

static number nrMapZ(number from, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == n_R );
  assume( aRing->rep == n_rep_gap_gmp );

  mpz_ptr z;
  mpz_ptr zz=NULL;
  if (IS_IMM(from))
  {
     zz=(mpz_ptr)omAlloc(sizeof(mpz_t));
     mpz_init_set_si(zz,SR_TO_INT(from));
     z=zz;
  }
  else
  {
    /* read out the enumerator */
    z=(mpz_ptr)from;
  }

  int i = mpz_size1(z);
  mpf_t e;
  mpf_init(e);
  mpf_set_z(e,z);
  int sign= mpf_sgn(e);
  mpf_abs (e, e);

  if (zz!=NULL)
  {
    mpz_clear(zz);
    omFreeSize(zz,sizeof(mpz_t));
  }
  if(i>4)
  {
    WerrorS("float overflow");
    return nf(0.0).N();
  }
  double basis;
  signed long int exp;
  basis = mpf_get_d_2exp(&exp, e);
  SI_FLOAT f= sign*ldexp(basis,exp);
  mpf_clear(e);
  return nf(f).N();
}

// old version:
// number nrMapQ(number from, const coeffs aRing, const coeffs r)
// {
// /* in longrat.h
// #define SR_INT    1
// #define mpz_size1(A) (ABS((A)->_mp_size))
// */
// #define SR_HDL(A) ((long)(A))
// #define mpz_isNeg(A) ((A)->_mp_size<0)
// #define mpz_limb_size(A) ((A)->_mp_size)
// #define mpz_limb_d(A) ((A)->_mp_d)
// #define MPZ_DIV(A,B,C) mpz_tdiv_q((A),(B),(C))
// #define IS_INT(A) ((A)->s==3)
// #define IS_IMM(A) (SR_HDL(A)&SR_INT)
// #define GET_NOM(A) ((A)->z)
// #define GET_DENOM(A) ((A)->n)
// #define MPZ_INIT mpz_init
// #define MPZ_CLEAR mpz_clear

//   assume( getCoeffType(r) == n_R );
//   assume( getCoeffType(aRing) == n_Q );

//   mpz_t h;
//   mpz_ptr g,z,n;
//   int i,j,t,s;
//   SI_FLOAT ba,rr,rn,y;

//   if (IS_IMM(from))
//     return nf((SI_FLOAT)nlInt(from,NULL /* dummy for nlInt*/)).N();
//   z=GET_NOM(from);
//   s=0X10000;
//   ba=(SI_FLOAT)s;
//   ba*=ba;
//   rr=0.0;
//   i=mpz_size1(z);
//   if(IS_INT(from))
//   {
//     if(i>4)
//     {
//       WerrorS("SI_FLOAT overflow");
//       return nf(rr).N();
//     }
//     i--;
//     rr=(SI_FLOAT)mpz_limb_d(z)[i];
//     while(i>0)
//     {
//       i--;
//       y=(SI_FLOAT)mpz_limb_d(z)[i];
//       rr=rr*ba+y;
//     }
//     if(mpz_isNeg(z))
//       rr=-rr;
//     return nf(rr).N();
//   }
//   n=GET_DENOM(from);
//   j=s=mpz_limb_size(n);
//   if(j>i)
//   {
//     g=n; n=z; z=g;
//     t=j; j=i; i=t;
//   }
//   t=i-j;
//   if(t>4)
//   {
//     if(j==s)
//       WerrorS("SI_FLOAT overflow");
//     return nf(rr).N();
//   }
//   if(t>1)
//   {
//     g=h;
//     MPZ_INIT(g);
//     MPZ_DIV(g,z,n);
//     t=mpz_size1(g);
//     if(t>4)
//     {
//       MPZ_CLEAR(g);
//       if(j==s)
//         WerrorS("SI_FLOAT overflow");
//       return nf(rr).N();
//     }
//     t--;
//     rr=(SI_FLOAT)mpz_limb_d(g)[t];
//     while(t)
//     {
//       t--;
//       y=(SI_FLOAT)mpz_limb_d(g)[t];
//       rr=rr*ba+y;
//     }
//     MPZ_CLEAR(g);
//     if(j!=s)
//       rr=1.0/rr;
//     if(mpz_isNeg(z))
//       rr=-rr;
//     return nf(rr).N();
//   }
//   rn=(SI_FLOAT)mpz_limb_d(n)[j-1];
//   rr=(SI_FLOAT)mpz_limb_d(z)[i-1];
//   if(j>1)
//   {
//     rn=rn*ba+(SI_FLOAT)mpz_limb_d(n)[j-2];
//     rr=rr*ba+(SI_FLOAT)mpz_limb_d(z)[i-2];
//     i--;
//   }
//   if(t!=0)
//     rr=rr*ba+(SI_FLOAT)mpz_limb_d(z)[i-2];
//   if(j==s)
//     rr=rr/rn;
//   else
//     rr=rn/rr;
//   if(mpz_isNeg(z))
//     rr=-rr;
//   return nf(rr).N();
// }

static nMapFunc nrSetMap(const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == n_R );

  if (src->rep==n_rep_gap_rat) /*Q, Z */
  {
    return nrMapQ;
  }
  if (src->rep==n_rep_gap_gmp) /*Q, Z */
  {
    return nrMapZ;
  }
  if ((src->rep==n_rep_gmp_float) && nCoeff_is_long_R(src))
  {
    return nrMapLongR;
  }
  if ((src->rep==n_rep_float) && nCoeff_is_R(src))
  {
    return ndCopyMap;
  }
  if ((src->rep==n_rep_int) && nCoeff_is_Zp(src))
  {
    return nrMapP;
  }
  if ((src->rep==n_rep_gmp_complex) && nCoeff_is_long_C(src))
  {
    return nrMapC;
  }
  return NULL;
}

static char* nrCoeffString(const coeffs r)
{
  return omStrDup("Float()");
}

static char* nrCoeffName(const coeffs r)
{
  return (char*)"Float()";
}

BOOLEAN nrInitChar(coeffs n, void* p)
{
  assume( getCoeffType(n) == n_R );

  assume( p == NULL );

  n->is_field=TRUE;
  n->is_domain=TRUE;
  n->rep=n_rep_float;

  //n->cfKillChar = ndKillChar; /* dummy */
  n->ch = 0;
  n->cfCoeffName = nrCoeffName;

  n->cfInit = nrInit;
  n->cfInt  = nrInt;
  n->cfAdd   = nrAdd;
  n->cfSub   = nrSub;
  n->cfMult  = nrMult;
  n->cfDiv   = nrDiv;
  n->cfExactDiv= nrDiv;
  n->cfInpNeg   = nrNeg;
  n->cfInvers= nrInvers;
  //n->cfCopy  = ndCopy;
  n->cfGreater = nrGreater;
  n->cfEqual = nrEqual;
  n->cfIsZero = nrIsZero;
  n->cfIsOne = nrIsOne;
  n->cfIsMOne = nrIsMOne;
  n->cfGreaterZero = nrGreaterZero;
  n->cfWriteLong = nrWrite;
  n->cfRead = nrRead;
  //n->cfPower = nrPower;
  n->cfSetMap = nrSetMap;

    /* nName= ndName; */
    /*nSize  = ndSize;*/
#ifdef LDEBUG
  n->cfDBTest=nrDBTest; // not yet implemented: nrDBTest;
#endif

  //n->nCoeffIsEqual = ndCoeffIsEqual;

  n->float_len = SHORT_REAL_LENGTH;
  n->float_len2 = SHORT_REAL_LENGTH;

  // TODO: Any variables?
  return FALSE;
}
