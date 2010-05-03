/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: computations with GMP complex floating-point numbers
*
* ngc == number gnu complex
*/

#include "coeffs.h"
#include "numbers.h"
#include "longrat.h"
#include "modulop.h"
#include "gnumpc.h"
#include "gnumpfl.h"
#include "mpr_complex.h"
#include "output.h"
#include "omalloc.h"


#include "shortfl.h"


#ifdef LDEBUG
// not yet implemented
BOOLEAN ngcDBTest(number a, const char *f, const int l, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return TRUE;
}
#endif

#ifndef assume
#  define assume(a) if(!(a)){ Werror( "Assumption: is wrong: %s\n", #a ); };
#endif

static const n_coeffType ID = n_long_C;

number ngcMapQ(number from, const coeffs r, const coeffs aRing)
{
  assume( getCoeffType(r) == ID );
  assume( getCoeffType(aRing) == n_Q );

  if ( from != NULL )
  {
    gmp_complex *res=new gmp_complex(numberFieldToFloat(from,QTOF));
    return (number)res;
  }
  else
    return NULL;
}

static number ngcMapLongR(number from, const coeffs r, const coeffs aRing)
{
  assume( getCoeffType(r) == ID );
  assume( getCoeffType(aRing) == n_long_R );
  
  if ( from != NULL )
  {
    gmp_complex *res=new gmp_complex(*((gmp_float *)from));
    return (number)res;
  }
  else
    return NULL;
}

static number ngcMapR(number from, const coeffs r, const coeffs aRing)
{
  assume( getCoeffType(r) == ID );
  assume( getCoeffType(aRing) == n_R );

  if ( from != NULL )
  {
    gmp_complex *res=new gmp_complex((double)nrFloat(from));
    return (number)res;
  }
  else
    return NULL;
}

static number ngcMapP(number from, const coeffs r, const coeffs aRing)
{
  assume( getCoeffType(r) == ID );
  assume( getCoeffType(aRing) ==  n_Zp );
  
  if ( from != NULL)
    return ngcInit(npInt(from, aRing), r);
  else
    return NULL;
}



static number ngcCopyMap(number from, const coeffs r, const coeffs aRing)
{
  assume( getCoeffType(r) == ID );
  assume( getCoeffType(aRing) ==  ID );

  gmp_complex* b = NULL;
  
  if ( from !=  NULL )
  { 
    b = new gmp_complex( *(gmp_complex*)from );
  }
  return (number)b;  
}

nMapFunc ngcSetMap(const coeffs src, const coeffs dst)
{
  assume( getCoeffType(dst) == ID );
  
  if(nField_is_Q(src))
  {
    return ngcMapQ;
  }
  if (nField_is_long_R(src))
  {
    return ngcMapLongR;
  }
  if (nField_is_long_C(src))
  {
    return ngcCopyMap;
  }
  if(nField_is_R(src))
  {
    return ngcMapR;
  }
  if (nField_is_Zp(src))
  {
    return ngcMapP;
  }
  return NULL;
}



number   ngcPar(int i, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  gmp_complex* n= new gmp_complex( (long)0, (long)1 );
  return (number)n;
}

/*2
* n := i
*/
number ngcInit (int i, const coeffs r)
{
  assume( getCoeffType(r) == ID );
  
  gmp_complex* n= new gmp_complex( (long)i, (long)0 );
  
  return (number)n;
}

/*2
* convert number to int
*/
int ngcInt(number &i, const coeffs r)
{
  assume( getCoeffType(r) == ID );
  
  return (int)((gmp_complex*)i)->real();
}

int ngcSize(number n)
{
  int r = (int)((gmp_complex*)n)->real();
  if (r < 0) r = -r;
  int i = (int)((gmp_complex*)n)->imag();
  if (i < 0) i = -i;
  int oneNorm = r + i;
  /* basically return the 1-norm of n;
     only if this happens to be zero although n != 0,
     return 1;
     (this code ensures that zero has the size zero) */
  if ((oneNorm == 0.0) & (ngcIsZero(n) == FALSE)) oneNorm = 1;
  return oneNorm;
}

/*2
* delete a
*/
void ngcDelete (number * a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  if ( *a != NULL )
  {
    delete *(gmp_complex**)a;
    *a=NULL;
  }
}

/*2
 * copy a to b
*/
number ngcCopy(number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );
  
  gmp_complex* b= new gmp_complex( *(gmp_complex*)a );
  return (number)b;
}


/*2
* za:= - za
*/
number ngcNeg (number a, const coeffs R)
{
  assume( getCoeffType(R) == ID );

  gmp_complex* r=(gmp_complex*)a;
  (*r) *= ngc_m1;
  return (number)a;
}

/*
* 1/a
*/
number ngcInvers(number a, const coeffs R)
{
  assume( getCoeffType(R) == ID );

  gmp_complex* r = NULL;
  if (((gmp_complex*)a)->isZero())
  {
    WerrorS(nDivBy0);
  }
  else
  {
    b = new gmp_complex( (gmp_complex)1 / (*(gmp_complex*)a) );
  }
  return (number)b;
}

/*2
* u:= a + b
*/
number ngcAdd (number a, number b, const coeffs R)
{
  assume( getCoeffType(R) == ID );

  gmp_complex* r= new gmp_complex( (*(gmp_complex*)a) + (*(gmp_complex*)b) );
  return (number)r;
}

/*2
* u:= a - b
*/
number ngcSub (number a, number b, const coeffs R)
{
  assume( getCoeffType(R) == ID );

  gmp_complex* r= new gmp_complex( (*(gmp_complex*)a) - (*(gmp_complex*)b) );
  return (number)r;
}

/*2
* u := a * b
*/
number ngcMult (number a, number b, const coeffs R)
{
  assume( getCoeffType(R) == ID );
  
  gmp_complex* r= new gmp_complex( (*(gmp_complex*)a) * (*(gmp_complex*)b) );
  return (number)r;
}

/*2
* u := a / b
*/
number ngcDiv (number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  if (((gmp_complex*)b)->isZero())
  {
    // a/0 = error
    WerrorS(nDivBy0);
    return NULL;
  }
  gmp_complex* res = new gmp_complex( (*(gmp_complex*)a) / (*(gmp_complex*)b) );
  return (number)res;
}

/*2
* u:= x ^ exp
*/
void ngcPower ( number x, int exp, number * u, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  if ( exp == 0 )
  {
    gmp_complex* n = new gmp_complex(1);
    *u=(number)n;
    return;
  }
  else if ( exp == 1 )
  {
    n_New(u, r);
    gmp_complex* n = new gmp_complex();
    *n= *(gmp_complex*)x;
    *u=(number)n;
    return;
  }
  else if (exp == 2)
  {
    n_New(u, r);
    gmp_complex* n = new gmp_complex();
    *n= *(gmp_complex*)x;
    *u=(number)n;
    *(gmp_complex*)(*u) *= *(gmp_complex*)n;
    return;
  }
  if (exp&1==1)
  {
    ngcPower(x,exp-1,u, r);
    gmp_complex *n = new gmp_complex();
    *n=*(gmp_complex*)x;
    *(gmp_complex*)(*u) *= *(gmp_complex*)n;
    delete n;
  }
  else
  {
    number w;
    n_New(&w, r);
    ngcPower(x,exp/2,&w, r);
    ngcPower(w,2,u, r);
    n_Delete(&w, r);
  }
}

BOOLEAN ngcIsZero (number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return ( ((gmp_complex*)a)->real().isZero() && ((gmp_complex*)a)->imag().isZero());
}

number ngcRePart(number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  gmp_complex* n = new gmp_complex(((gmp_complex*)a)->real());
  return (number)n;
}

number ngcImPart(number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  gmp_complex* n = new gmp_complex(((gmp_complex*)a)->imag());
  return (number)n;
}

/*2
* za >= 0 ?
*/
BOOLEAN ngcGreaterZero (number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  if ( ! ((gmp_complex*)a)->imag().isZero() )
    return ( abs( *(gmp_complex*)a).sign() >= 0 );
  else
    return ( ((gmp_complex*)a)->real().sign() >= 0 );
}

/*2
* a > b ?
*/
BOOLEAN ngcGreater (number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  gmp_complex *aa=(gmp_complex*)a;
  gmp_complex *bb=(gmp_complex*)b;
  return (*aa) > (*bb);
}

/*2
* a = b ?
*/
BOOLEAN ngcEqual (number a, number b, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  gmp_complex *aa=(gmp_complex*)a;
  gmp_complex *bb=(gmp_complex*)b;
  return (*aa) == (*bb);
}

/*2
* a == 1 ?
*/
BOOLEAN ngcIsOne (number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return (((gmp_complex*)a)->real().isOne() && ((gmp_complex*)a)->imag().isZero());
  //return (((gmp_complex*)a)->real().isOne());
}

/*2
* a == -1 ?
*/
BOOLEAN ngcIsMOne (number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return (((gmp_complex*)a)->real().isMOne() && ((gmp_complex*)a)->imag().isZero());
  //return (((gmp_complex*)a)->real().isMOne());
}

/*2
* extracts the number a from s, returns the rest
*/
const char * ngcRead (const char * s, number * a, const coeffs r)
{
  assume( getCoeffType(r) == ID );
  
  if ((*s >= '0') && (*s <= '9'))
  {
    gmp_float *re=NULL;
    s=ngfRead(s,(number *)&re, r);
    gmp_complex *aa=new gmp_complex(*re);
    *a=(number)aa;
    delete re;
  }
  else if (strncmp(s, r->parameter[0],strlen(r->parameter[0]))==0)
  {
    s+=strlen(r->parameter[0]);
    gmp_complex *aa=new gmp_complex((long)0,(long)1);
    *a=(number)aa;
  }
  else
  {
    *a=(number) new gmp_complex((long)1);
  }
  return s;
}



/*2
* write a floating point number
*/
void ngcWrite (number &a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  extern size_t gmp_output_digits; /// comes from mpr_complex.cc

  if (a==NULL)
    StringAppendS("0");
  else
  {
    char *out;
    out= complexToStr(*(gmp_complex*)a, gmp_output_digits);
    StringAppendS(out);
    //    omFreeSize((void *)out, (strlen(out)+1)* sizeof(char) );
    omFree( (void *)out );
  }
}

// local Variables: ***
// folded-file: t ***
// compile-command: "make installg" ***
// End: ***
