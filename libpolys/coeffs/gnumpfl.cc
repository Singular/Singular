/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: computations with GMP floating-point numbers
*
* ngf == number gnu floats
*/

#include <misc/auxiliary.h>
#include <omalloc/omalloc.h>

#include <reporter/reporter.h>

#include "coeffs.h"
#include "numbers.h"
#include "mpr_complex.h"

#include "longrat.h"
#include "shortfl.h"
#include "gnumpfl.h"
#include "modulop.h"

//ring ngfMapRing; // to be used also in gnumpc.cc

/// Our Type!
static const n_coeffType ID = n_long_R;

/// Get a mapping function from src into the domain of this type:
nMapFunc  ngfSetMap(const coeffs src, const coeffs dst);

const char *   ngfRead (const char *s, number *a, const coeffs r);

 // Private interface should be hidden!!!
/// Note: MAY NOT WORK AS EXPECTED!
BOOLEAN  ngfGreaterZero(number za, const coeffs r);
BOOLEAN  ngfGreater(number a, number b, const coeffs r);
BOOLEAN  ngfEqual(number a, number b, const coeffs r);
BOOLEAN  ngfIsOne(number a, const coeffs r);
BOOLEAN  ngfIsMOne(number a, const coeffs r);
BOOLEAN  ngfIsZero(number za, const coeffs r);
number   ngfInit(long i, const coeffs r);
long     ngfInt(number &n, const coeffs r);
number   ngfNeg(number za, const coeffs r);
number   ngfInvers(number a, const coeffs r);
number   ngfAdd(number la, number li, const coeffs r);
number   ngfSub(number la, number li, const coeffs r);
number   ngfMult(number a, number b, const coeffs r);
number   ngfDiv(number a, number b, const coeffs r);
void     ngfPower(number x, int exp, number *lu, const coeffs r);
number   ngfCopy(number a, const coeffs r);
number   ngf_Copy(number a, coeffs r);
void     ngfWrite(number &a, const coeffs r);
void     ngfCoeffWrite(const coeffs r, BOOLEAN details);

void     ngfDelete(number *a, const coeffs r);

number ngfMapQ(number from, const coeffs src, const coeffs r);

union nf
{
  float _f;
  number _n;
  nf(float f) {_f = f;}
  nf(number n) {_n = n;}
  float F() const {return _f;}
  number N() const {return _n;}
};

/*2
* n := i
*/
number ngfInit (long i, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  gmp_float* n= new gmp_float( (double)i );
  return (number)n;
}

/*2
* convert number to int
*/
long ngfInt(number &i, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  double d=(double)*(gmp_float*)i;
  if (d<0.0)
    return (long)(d-0.5);
  else
    return (long)(d+0.5);
}

int ngfSize(number n, const coeffs r)
{
  long i = ngfInt(n, r);
  /* basically return the largest integer in n;
     only if this happens to be zero although n != 0,
     return 1;
     (this code ensures that zero has the size zero) */
  if ((i == 0) && (ngfIsZero(n,r) == FALSE)) i = 1;
  return i;
}

/*2
* delete a
*/
void ngfDelete (number * a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  if ( *a != NULL )
  {
    delete *(gmp_float**)a;
    *a=NULL;
  }
}

/*2
* copy a to b
*/
number ngfCopy(number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  gmp_float* b= new gmp_float( *(gmp_float*)a );
  return (number)b;
}

#if 0
static number ngfCopyMap(number a, const coeffs r1, const coeffs r2)
{
  assume( getCoeffType(r1) == ID );
  assume( getCoeffType(r2) == ID );

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
number ngfNeg (number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  *(gmp_float*)a= -(*(gmp_float*)a);
  return (number)a;
}

/*
* 1/a
*/
number ngfInvers(number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  gmp_float* f= NULL;
  if (((gmp_float*)a)->isZero() )
  {
    WerrorS(nDivBy0);
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
number ngfAdd (number a, number b, const coeffs R)
{
  assume( getCoeffType(R) == ID );

  gmp_float* r= new gmp_float( (*(gmp_float*)a) + (*(gmp_float*)b) );
  return (number)r;
}

/*2
* u:= a - b
*/
number ngfSub (number a, number b, const coeffs R)
{
  assume( getCoeffType(R) == ID );

  gmp_float* r= new gmp_float( (*(gmp_float*)a) - (*(gmp_float*)b) );
  return (number)r;
}

/*2
* u := a * b
*/
number ngfMult (number a, number b, const coeffs R)
{
  assume( getCoeffType(R) == ID );

  gmp_float* r= new gmp_float( (*(gmp_float*)a) * (*(gmp_float*)b) );
  return (number)r;
}

/*2
* u := a / b
*/
number ngfDiv (number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  if ( ((gmp_float*)b)->isZero() )
  {
    // a/0 = error
    WerrorS(nDivBy0);
    return NULL;
  }
  gmp_float* f= new gmp_float( (*(gmp_float*)a) / (*(gmp_float*)b) );
  return (number)f;
}

/*2
* u:= x ^ exp
*/
number ngfPower (number x, int exp, const coeffs r)
{
  assume( getCoeffType(r) == ID );

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
void ngfPower ( number x, int exp, number * u, const coeffs r )
{
  *u = ngfPower(x, exp, r);
}

BOOLEAN ngfIsZero (number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return ( ((gmp_float*)a)->isZero() );
}

/*2
* za > 0 ?
*/
BOOLEAN ngfGreaterZero (number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return (((gmp_float*)a)->sign() > 0);
}

/*2
* a > b ?
*/
BOOLEAN ngfGreater (number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return ( (*(gmp_float*)a) > (*(gmp_float*)b) );
}

/*2
* a = b ?
*/
BOOLEAN ngfEqual (number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return ( (*(gmp_float*)a) == (*(gmp_float*)b) );
}

/*2
* a == 1 ?
*/
BOOLEAN ngfIsOne (number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return ((gmp_float*)a)->isOne();
}

/*2
* a == -1 ?
*/
BOOLEAN ngfIsMOne (number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

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
  assume( getCoeffType(r) == ID or getCoeffType(r) == n_long_C);

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
void ngfWrite (number &a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

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

BOOLEAN ngfCoeffIsEqual (const coeffs r, n_coeffType n, void * parameter)
{
  if (n==ID)
  {
    LongComplexInfo* p = (LongComplexInfo *)(parameter);
    if ((p!=NULL)
    && (p->float_len == r->float_len)
    && (p->float_len2 == r->float_len2))
      return TRUE;
  }
  return FALSE;
}

void ngfSetChar(const coeffs r)
{
  setGMPFloatDigits(r->float_len, r->float_len2);
}

static char* ngfCoeffString(const coeffs r)
{
  char *s=(char*)omAlloc(27);
  snprintf(s,27,"real,%d,%d",r->float_len,r->float_len2);
  return s;
}

BOOLEAN ngfInitChar(coeffs n, void *parameter)
{
  assume( getCoeffType(n) == ID );

  n->is_field=TRUE;
  n->is_domain=TRUE;
  n->rep=n_rep_gmp_float;

  //n->cfKillChar = ndKillChar; /* dummy */

  n->cfSetChar = ngfSetChar;
  n->ch = 0;
  n->cfCoeffString=ngfCoeffString;

  n->cfDelete  = ngfDelete;
  //n->cfNormalize=ndNormalize;
  n->cfInit   = ngfInit;
  n->cfInt    = ngfInt;
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
  n->cfCoeffWrite = ngfCoeffWrite;
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

number ngfMapQ(number from, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == ID );
  assume( src->rep == n_rep_gap_rat );

  gmp_float *res=new gmp_float(numberFieldToFloat(from,QTOF,dst));
  return (number)res;
}
number ngfMapZ(number from, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == ID );
  assume( aRing->rep == n_rep_gap_gmp);

  if ( from != NULL )
  {
    if (SR_HDL(from) & SR_INT)
    {
      gmp_float f_i= gmp_float(SR_TO_INT(from));
      gmp_float *res=new gmp_float(f_i);
      return (number)res;
    }
    gmp_float f_i=(mpz_ptr)from;
    gmp_float *res=new gmp_float(f_i);
    return (number)res;
  }
  else
    return NULL;
}


static number ngfMapR(number from, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == ID );
  assume( getCoeffType(src) == n_R );

  gmp_float *res=new gmp_float((double)nf(from).F());
  return (number)res;
}

static number ngfMapP(number from, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == ID );
  assume( getCoeffType(src) ==  n_Zp );

  return ngfInit(npInt(from,src), dst); // FIXME? TODO? // extern int     npInt         (number &n, const coeffs r);
}

static number ngfMapC(number from, const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == ID );
  assume( getCoeffType(src) ==  n_long_C );

  gmp_float *res=new gmp_float(((gmp_complex*)from)->real());
  return (number)res;
}

nMapFunc ngfSetMap(const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == ID );

  if (src->rep==n_rep_gap_rat) /*Q, Z*/
  {
    return ngfMapQ;
  }
  if (src->rep==n_rep_gap_gmp) /*Q, Z*/
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

void    ngfCoeffWrite  (const coeffs r, BOOLEAN /*details*/)
{
  Print("//   characteristic : 0 (real:%d digits, additional %d digits)\n",
               r->float_len,r->float_len2);  /* long R */
}
