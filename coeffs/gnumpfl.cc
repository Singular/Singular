/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: computations with GMP floating-point numbers
*
* ngf == number gnu floats
*/

#include "config.h"
#include "coeffs.h"
#include <output.h>
#include <omalloc.h>
#include "numbers.h"
#include "modulop.h"
#include "longrat.h"

#include <gnumpfl.h>
#include <mpr_complex.h>

extern size_t gmp_output_digits;
//ring ngfMapRing; // to be used also in gnumpc.cc

static number ngfMapP(number from, const coeffs src, const coeffs dst)
{
  return ngfInit(npInt(from,src), dst);
}
number ngfMapQ(number from, const coeffs src, const coeffs dst)
{
  gmp_float *res=new gmp_float(numberFieldToFloat(from,QTOF));
  return (number)res;
}
union nf
{
  float _f;
  number _n;
  nf(float f) {_f = f;}
  nf(number n) {_n = n;}
  float F() const {return _f;}
  number N() const {return _n;}
};
static number ngfMapR(number from, const coeffs src, const coeffs dst)
{
  gmp_float *res=new gmp_float((double)nf(from).F());
  return (number)res;
}
static number ngfMapC(number from, const coeffs src, const coeffs dst)
{
  gmp_float *res=new gmp_float(((gmp_complex*)from)->real());
  return (number)res;
}

nMapFunc ngfSetMap(const ring src, const ring dst)
{
  if (rField_is_Q(src))
  {
    return ngfMapQ;
  }
  if (rField_is_long_R(src))
  {
    return ngfCopy;
  }
  if (rField_is_R(src))
  {
    return ngfMapR;
  }
  if (rField_is_Zp(src))
  {
    ngfMapRing=src;
    return ngfMapP;
  }
  if (rField_is_long_C(src))
  {
    return ngfMapC;
  }
  return NULL;
}

/*2
* n := i
*/
number ngfInit (int i, const coeffs r)
{
  gmp_float* n= new gmp_float( (double)i );
  return (number)n;
}

/*2
* convert number to int
*/
int ngfInt(number &i, const coeffs r)
{
  double d=(double)*(gmp_float*)i;
  if (d<0.0)
    return (int)(d-0.5);
  else
    return (int)(d+0.5);
}

int ngfSize(number n, const coeffs r)
{
  int i = ngfInt(n, r);
  /* basically return the largest integer in n;
     only if this happens to be zero although n != 0,
     return 1;
     (this code ensures that zero has the size zero) */
  if ((i == 0) && (ngfIsZero(n) == FALSE)) i = 1;
  return i;
}

/*2
* delete a
*/
void ngfDelete (number * a, const coeffs r)
{
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
  gmp_float* b= new gmp_float( *(gmp_float*)a );
  return (number)b;
}

/*2
* za:= - za
*/
number ngfNeg (number a, const coeffs r)
{
  *(gmp_float*)a= -(*(gmp_float*)a);
  return (number)a;
}

/*
* 1/a
*/
number ngfInvers(number a, const coeffs R)
{
  gmp_float* r= NULL;
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
  gmp_float* r= new gmp_float( (*(gmp_float*)a) + (*(gmp_float*)b) );
  return (number)r;
}

/*2
* u:= a - b
*/
number ngfSub (number a, number b, const coeffs R)
{
  gmp_float* r= new gmp_float( (*(gmp_float*)a) - (*(gmp_float*)b) );
  return (number)r;
}

/*2
* u := a * b
*/
number ngfMult (number a, number b, const coeffs R)
{
  gmp_float* r= new gmp_float( (*(gmp_float*)a) * (*(gmp_float*)b) );
  return (number)r;
}

/*2
* u := a / b
*/
number ngfDiv (number a, number b, const coeffs r)
{
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
  if ( exp == 0 )
  {
    gmp_float* n = new gmp_float(1);
    *u=(number)n;
    return;
  }
  else if ( ngfIsZero(x) ) // 0^e, e>0
  {
    *u=ngfInit(0, r);
    return;
  }
  else if ( exp == 1 )
  {
    n_New(u, r);
    gmp_float* n = new gmp_float();
    *n= *(gmp_float*)x;
    *u=(number)n;
    return;
  }
  f = new gmp_float();
  f = (*(gmp_float*)x)^exp;
  return (number)f;
}

/* kept for compatibility reasons, to be deleted */
void ngfPower ( number x, int exp, number * u, const coeffs r )
{
  *u = ngfPower(x, exp, r);
} 

BOOLEAN ngfIsZero (number a, const coeffs r)
{
  return ( ((gmp_float*)a)->isZero() );
}

/*2
* za > 0 ?
*/
BOOLEAN ngfGreaterZero (number a, const coeffs r)
{
  return (((gmp_float*)a)->sign() > 0);
}

/*2
* a > b ?
*/
BOOLEAN ngfGreater (number a, number b, const coeffs r)
{
  return ( (*(gmp_float*)a) > (*(gmp_float*)b) );
}

/*2
* a = b ?
*/
BOOLEAN ngfEqual (number a, number b, const coeffs r)
{
  return ( (*(gmp_float*)a) == (*(gmp_float*)b) );
}

/*2
* a == 1 ?
*/
BOOLEAN ngfIsOne (number a, const coeffs r)
{
  return ((gmp_float*)a)->isOne();
}

/*2
* a == -1 ?
*/
BOOLEAN ngfIsMOne (number a, const coeffs r)
{
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
*/
const char * ngfRead (const char * start, number * a, const coeffs r)
{
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
  char *out;
  if ( a != NULL )
  {
    out= floatToStr(*(gmp_float*)a,gmp_output_digits);
    StringAppendS(out);
    //omFreeSize((void *)out, (strlen(out)+1)* sizeof(char) );
    omFree( (void *)out );
  }
  else
  {
    StringAppendS("0");
  }
}

