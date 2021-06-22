/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: computations with GMP floating-point numbers
*
* ngf == number gnu floats
*/

#include "misc/auxiliary.h"

#include "reporter/reporter.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"
#include "coeffs/mpr_complex.h"

#include "coeffs/longrat.h"
#include "coeffs/shortfl.h"
#include "coeffs/gnumpfl.h"
#include "coeffs/gnumpc.h"
#include "coeffs/modulop.h"

const char *   ngfRead (const char *s, number *a, const coeffs r);

union nf
{
  SI_FLOAT _f;
  number _n;
  nf(SI_FLOAT f) {_f = f;}
  nf(number n) {_n = n;}
  SI_FLOAT F() const {return _f;}
  number N() const {return _n;}
};

/*2
* n := i
*/
static number ngfInit (long i, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  gmp_float* n= new gmp_float( (double)i );
  return (number)n;
}

/*2
* convert number to int
*/
static long ngfInt(number &i, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  double d=(double)*(gmp_float*)i;
  if (d<0.0)
    return (long)(d-0.5);
  else
    return (long)(d+0.5);
}

static BOOLEAN ngfIsZero (number a, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  return ( ((gmp_float*)a)->isZero() );
}

static int ngfSize(number n, const coeffs r)
{
  long i = ngfInt(n, r);
  /* basically return the largest integer in n;
     only if this happens to be zero although n != 0,
     return 1;
     (this code ensures that zero has the size zero) */
  if ((i == 0) && (ngfIsZero(n,r) == FALSE)) i = 1;
  return ABS(i);
}

/*2
* delete a
*/
static void ngfDelete (number * a, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  if ( *a != NULL )
  {
    delete *(gmp_float**)a;
    *a=NULL;
  }
}

/*2
* copy a to b
*/
static number ngfCopy(number a, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  gmp_float* b= new gmp_float( *(gmp_float*)a );
  return (number)b;
}

#if 0
static number ngfCopyMap(number a, const coeffs r1, const coeffs r2)
{
  assume( getCoeffType(r1) == n_long_R );
  assume( getCoeffType(r2) == n_long_R );

  gmp_float* b= NULL;
  if ( a !=  NULL )
  {
    b= new gmp_float( *(gmp_float*)a );
  }
  return (number)b;
}
#endif

/*2
* za:= - za
*/
static number ngfNeg (number a, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  *(gmp_float*)a= -(*(gmp_float*)a);
  return (number)a;
}

/*
* 1/a
*/
static number ngfInvers(number a, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  gmp_float* f= NULL;
  if (((gmp_float*)a)->isZero() )
  {
    WerrorS(nDivBy0);
    f= new gmp_float( 0 );
  }
  else
  {
    f= new gmp_float( gmp_float(1) / (*(gmp_float*)a) );
  }
  return (number)f;
}

/*2
* u:= a + b
*/
static number ngfAdd (number a, number b, const coeffs R)
{
  assume( getCoeffType(R) == n_long_R );

  gmp_float* r= new gmp_float( (*(gmp_float*)a) + (*(gmp_float*)b) );
  return (number)r;
}

/*2
* u:= a - b
*/
static number ngfSub (number a, number b, const coeffs R)
{
  assume( getCoeffType(R) == n_long_R );

  gmp_float* r= new gmp_float( (*(gmp_float*)a) - (*(gmp_float*)b) );
  return (number)r;
}

/*2
* u := a * b
*/
static number ngfMult (number a, number b, const coeffs R)
{
  assume( getCoeffType(R) == n_long_R );

  gmp_float* r= new gmp_float( (*(gmp_float*)a) * (*(gmp_float*)b) );
  return (number)r;
}

/*2
* u := a / b
*/
static number ngfDiv (number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );
  
  gmp_float* f;
  if ( ((gmp_float*)b)->isZero() )
  {
    // a/0 = error
    WerrorS(nDivBy0);
    f= new gmp_float( 0 );
  }
  else
  {
    f= new gmp_float( (*(gmp_float*)a) / (*(gmp_float*)b) );
  }
  return (number)f;
}

/*2
* u:= x ^ exp
*/
static number ngfPower (number x, int exp, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  if ( exp == 0 )
  {
    gmp_float* n = new gmp_float(1);
    return (number)n;
  }
  else if ( ngfIsZero(x, r) ) // 0^e, e>0
  {
    return ngfInit(0, r);
  }
  else if ( exp == 1 )
  {
    return ngfCopy(x,r);
  }
  return (number) ( new gmp_float( (*(gmp_float*)x)^exp ) );
}

/* kept for compatibility reasons, to be deleted */
static void ngfPower ( number x, int exp, number * u, const coeffs r )
{
  *u = ngfPower(x, exp, r);
}

/*2
* za > 0 ?
*/
static BOOLEAN ngfGreaterZero (number a, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  return (((gmp_float*)a)->sign() > 0);
}

/*2
* a > b ?
*/
static BOOLEAN ngfGreater (number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  return ( (*(gmp_float*)a) > (*(gmp_float*)b) );
}

/*2
* a = b ?
*/
static BOOLEAN ngfEqual (number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  return ( (*(gmp_float*)a) == (*(gmp_float*)b) );
}

/*2
* a == 1 ?
*/
static BOOLEAN ngfIsOne (number a, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  return ((gmp_float*)a)->isOne();
}

/*2
* a == -1 ?
*/
static BOOLEAN ngfIsMOne (number a, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  return ((gmp_float*)a)->isMOne();
}

static char * ngfEatFloatNExp(char * s )
{
  char *start= s;

  // eat floats (mantissa) like:
  //   0.394394993, 102.203003008,  .300303032, pssibly starting with -
  if (*s == '-') s++;
  while ((*s >= '0' && *s <= '9')||(*s == '.')) s++;

  // eat the exponent, starts with 'e' followed by '+', '-'
  // and digits, like:
  //   e-202, e+393, accept also E7
  if ( (s != start) && ((*s == 'e')||(*s=='E')))
  {
    if (*s=='E') *s='e';
    s++; // skip 'e'/'E'
    if ((*s == '+') || (*s == '-')) s++;
    while ((*s >= '0' && *s <= '9')) s++;
  }

  return s;
}

/*2
* extracts the number a from s, returns the rest
*
* This is also called to print components of complex coefficients.
* Handle with care!
*/
const char * ngfRead (const char * start, number * a, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R or getCoeffType(r) == n_long_C);

  char *s= (char *)start;

  //Print("%s\n",s);

  s= ngfEatFloatNExp( s );

  if (*s=='\0')  // 0
  {
    if ( *(gmp_float**)a == NULL ) (*(gmp_float**)a)= new gmp_float();
    (*(gmp_float**)a)->setFromStr(start);
  }
  else if (s==start)  // 1
  {
    if ( *(gmp_float**)a != NULL )  delete (*(gmp_float**)a);
    (*(gmp_float**)a)= new gmp_float(1);
  }
  else
  {
    gmp_float divisor(1.0);
    char *start2=s;
    if ( *s == '/' )
    {
      s++;
      s= ngfEatFloatNExp( (char *)s );
      if (s!= start2+1)
      {
        char tmp_c=*s;
        *s='\0';
        divisor.setFromStr(start2+1);
        *s=tmp_c;
      }
      else
      {
        Werror("wrong long real format: %s",start2);
      }
    }
    char c=*start2;
    *start2='\0';
    if ( *(gmp_float**)a == NULL ) (*(gmp_float**)a)= new gmp_float();
    (*(gmp_float**)a)->setFromStr(start);
    *start2=c;
    if (divisor.isZero())
    {
      WerrorS(nDivBy0);
    }
    else
      (**(gmp_float**)a) /= divisor;
  }

  return s;
}

/*2
* write a floating point number
*/
static void ngfWrite (number a, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );

  char *out;
  if ( a != NULL )
  {
    out= floatToStr(*(gmp_float*)a, r->float_len);
    StringAppendS(out);
    //omFreeSize((void *)out, (strlen(out)+1)* sizeof(char) );
    omFree( (void *)out );
  }
  else
  {
    StringAppendS("0");
  }
}

static BOOLEAN ngfCoeffIsEqual (const coeffs r, n_coeffType n, void * parameter)
{
  if (n==n_long_R)
  {
    LongComplexInfo* p = (LongComplexInfo *)(parameter);
    if ((p!=NULL)
    && (p->float_len == r->float_len)
    && (p->float_len2 == r->float_len2))
      return TRUE;
  }
  return FALSE;
}

static void ngfSetChar(const coeffs r)
{
  setGMPFloatDigits(r->float_len, r->float_len2);
}

static char* ngfCoeffName(const coeffs r)
{
  STATIC_VAR char ngfCoeffName_buf[30];
  snprintf(ngfCoeffName_buf,30,"Float(%d,%d)",r->float_len,r->float_len2);
  return ngfCoeffName_buf;
}

static number ngfMapQ(number from, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == n_long_R );
  assume( src->rep == n_rep_gap_rat );

  gmp_float *res=new gmp_float(numberFieldToFloat(from,QTOF));
  return (number)res;
}

static number ngfMapZ(number from, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == n_long_R );
  assume( aRing->rep == n_rep_gmp);

  gmp_float *res=new gmp_float((mpz_ptr)from);
  return (number)res;
}

static number ngfMapR(number from, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == n_long_R );
  assume( getCoeffType(src) == n_R );

  gmp_float *res=new gmp_float((double)nf(from).F());
  return (number)res;
}

static number ngfMapP(number from, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == n_long_R );
  assume( getCoeffType(src) ==  n_Zp );

  return ngfInit(npInt(from,src), dst); // FIXME? TODO? // extern int     npInt         (number &n, const coeffs r);
}

static number ngfMapC(number from, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == n_long_R );
  assume( getCoeffType(src) ==  n_long_C );

  gmp_float *res=new gmp_float(((gmp_complex*)from)->real());
  return (number)res;
}

static number ngfInitMPZ(mpz_t m, const coeffs)
{
  gmp_float *res=new gmp_float(m);
  return (number)res;
}

static nMapFunc ngfSetMap(const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == n_long_R );

  if (src->rep==n_rep_gap_rat) /*Q, Z*/
  {
    return ngfMapQ;
  }
  if (src->rep==n_rep_gap_gmp) /*Q, bigint*/
  {
    return ngfMapQ;
  }
  if (src->rep==n_rep_gmp) /* Z*/
  {
    return ngfMapZ;
  }
  if ((src->rep==n_rep_gmp_float) && nCoeff_is_long_R(src))
  {
    return ndCopyMap; //ngfCopyMap;
  }
  if ((src->rep==n_rep_float) && nCoeff_is_R(src))
  {
    return ngfMapR;
  }
  if ((src->rep==n_rep_gmp_complex) && nCoeff_is_long_C(src))
  {
    return ngfMapC;
  }
  if ((src->rep==n_rep_int) && nCoeff_is_Zp(src))
  {
    return ngfMapP;
  }
  return NULL;
}

BOOLEAN ngfInitChar(coeffs n, void *parameter)
{
  assume( getCoeffType(n) == n_long_R );

  n->is_field=TRUE;
  n->is_domain=TRUE;
  n->rep=n_rep_gmp_float;

  //n->cfKillChar = ndKillChar; /* dummy */

  n->cfSetChar = ngfSetChar;
  n->ch = 0;
  n->cfCoeffName=ngfCoeffName;

  n->cfDelete  = ngfDelete;
  //n->cfNormalize=ndNormalize;
  n->cfInit    = ngfInit;
  n->cfInitMPZ = ngfInitMPZ;
  n->cfInt     = ngfInt;
  n->cfAdd     = ngfAdd;
  n->cfSub     = ngfSub;
  n->cfMult    = ngfMult;
  n->cfDiv     = ngfDiv;
  n->cfExactDiv= ngfDiv;
  n->cfInpNeg     = ngfNeg;
  n->cfInvers  = ngfInvers;
  n->cfCopy   = ngfCopy;
  n->cfGreater = ngfGreater;
  n->cfEqual   = ngfEqual;
  n->cfIsZero  = ngfIsZero;
  n->cfIsOne   = ngfIsOne;
  n->cfIsMOne  = ngfIsMOne;
  n->cfGreaterZero = ngfGreaterZero;
  n->cfWriteLong  = ngfWrite;
  n->cfRead    = ngfRead;
  n->cfPower   = ngfPower;
  n->cfSetMap = ngfSetMap;
#ifdef LDEBUG
  //n->cfDBTest  = ndDBTest; // not yet implemented: ngfDBTest
#endif

  n->nCoeffIsEqual = ngfCoeffIsEqual;

  if( parameter != NULL)
  {
    LongComplexInfo* p = (LongComplexInfo*)parameter;

    n->float_len = p->float_len;
    n->float_len2 = p->float_len2;
  } else // default values, just for testing!
  {
    n->float_len = SHORT_REAL_LENGTH;
    n->float_len2 = SHORT_REAL_LENGTH;
  }

  assume( n->float_len2 >= SHORT_REAL_LENGTH );

  assume( n_NumberOfParameters(n) == 0 );
  assume( n_ParameterNames(n) == NULL );

  return FALSE;
}
