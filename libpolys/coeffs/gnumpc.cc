/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: computations with GMP complex floating-point numbers
*
* ngc == number gnu complex
*/

#include "config.h"

#include <coeffs/coeffs.h>
#include <coeffs/numbers.h>
#include <coeffs/longrat.h>
#include <coeffs/modulop.h>
#include <coeffs/gnumpc.h>
#include <coeffs/gnumpfl.h>
#include <coeffs/mpr_complex.h>
#include <reporter/reporter.h>
#include <omalloc/omalloc.h>


#include <coeffs/shortfl.h>

/// Our Type!
static const n_coeffType ID = n_long_C;


#ifdef LDEBUG
// not yet implemented
BOOLEAN ngcDBTest(number a, const char *f, const int l, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return TRUE;
}
#endif


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

int ngcSize(number n, const coeffs R)
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
  if ((oneNorm == 0.0) & (ngcIsZero(n,R) == FALSE)) oneNorm = 1;
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
  (*r).neg();
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
    r = new gmp_complex( (gmp_complex)1 / (*(gmp_complex*)a) );
  }
  return (number)r;
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
  if ( (exp & 1) == 1 )
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
  assume( r->complex_parameter != NULL );

  if ((*s >= '0') && (*s <= '9'))
  {
    gmp_float *re=NULL;
    s=ngfRead(s,(number *)&re, r);
    gmp_complex *aa=new gmp_complex(*re);
    *a=(number)aa;
    delete re;
  }
  else if (strncmp(s, r->complex_parameter,strlen(r->complex_parameter))==0)
  {
    s+=strlen(r->complex_parameter);
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
    out= complexToStr(*(gmp_complex*)a, gmp_output_digits, r);
    StringAppendS(out);
    //    omFreeSize((void *)out, (strlen(out)+1)* sizeof(char) );
    omFree( (void *)out );
  }
}



/// test, whether r is an instance of nInitCoeffs(n, parameter)
static BOOLEAN ngcCoeffsEqual(const coeffs r, n_coeffType n, void*)
{
  assume( getCoeffType(r) == ID );
  
  return (n == ID);
}

BOOLEAN ngcInitChar(coeffs n, void* p)
{
  assume( getCoeffType(n) == ID );

  n->cfDelete  = ngcDelete;
  n->cfNormalize=ndNormalize;
  n->cfInit   = ngcInit;
  n->cfInt    = ngcInt;
  n->cfAdd     = ngcAdd;
  n->cfSub     = ngcSub;
  n->cfMult    = ngcMult;
  n->cfDiv     = ngcDiv;
  n->cfExactDiv= ngcDiv;
  n->cfNeg     = ngcNeg;
  n->cfInvers  = ngcInvers;
  n->cfCopy   = ngcCopy;
  n->cfGreater = ngcGreater;
  n->cfEqual   = ngcEqual;
  n->cfIsZero  = ngcIsZero;
  n->cfIsOne   = ngcIsOne;
  n->cfIsMOne  = ngcIsMOne;
  n->cfGreaterZero = ngcGreaterZero;
  n->cfWrite  = ngcWrite;
  n->cfRead    = ngcRead;
  n->cfPower   = ngcPower;
  n->cfSetMap = ngcSetMap;
  n->cfPar     = ngcPar;
  n->cfRePart  = ngcRePart;
  n->cfImPart  = ngcImPart;
  n->cfCoeffWrite = ngcCoeffWrite;
    // cfSize  = ndSize;
#ifdef LDEBUG
  n->cfDBTest  = ndDBTest; // not yet implemented: ngcDBTest
#endif

  n->nCoeffIsEqual = ngcCoeffsEqual;


  
/*  
  //r->cfInitChar=nlInitChar;
  r->cfKillChar=NULL;
  r->cfSetChar=NULL;
  r->nCoeffIsEqual=nlCoeffsEqual;

  r->cfMult  = nlMult;
  r->cfSub   = nlSub;
  r->cfAdd   = nlAdd;
  r->cfDiv   = nlDiv;
  r->cfIntDiv= nlIntDiv;
  r->cfIntMod= nlIntMod;
  r->cfExactDiv= nlExactDiv;
  r->cfInit = nlInit;
  r->cfPar = ndPar;
  r->cfParDeg = ndParDeg;
  r->cfSize  = nlSize;
  r->cfInt  = nlInt;
#ifdef HAVE_RINGS
  r->cfDivComp = NULL; // only for ring stuff
  r->cfIsUnit = NULL; // only for ring stuff
  r->cfGetUnit = NULL; // only for ring stuff
  r->cfExtGcd = NULL; // only for ring stuff
#endif
  r->cfNeg   = nlNeg;
  r->cfInvers= nlInvers;
  r->cfCopy  = nl_Copy;
  r->cfRePart = nl_Copy;
  r->cfImPart = ndReturn0;
  r->cfWrite = nlWrite;
  r->cfRead = nlRead;
  r->cfNormalize=nlNormalize;
  r->cfGreater = nlGreater;
#ifdef HAVE_RINGS
  r->cfDivBy = NULL; // only for ring stuff
#endif
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
  r->cfName = ndName;
  r->cfInpMult=nlInpMult;
  r->cfInit_bigint=nlCopyMap;
#ifdef LDEBUG
  // debug stuff
  r->cfDBTest=nlDBTest;
#endif

  // the variables:
  r->nNULL = INT_TO_SR(0);
  r->type = n_Q;
  r->ch = 0;
  r->has_simple_Alloc=FALSE;
  r->has_simple_Inverse=FALSE;
*/

/// TODO: Any variables?
  if( p == NULL )
    n->complex_parameter = "i"; //??
  else
    n->complex_parameter = omStrDup( (char*) p );
    
  return FALSE;
}





number ngcMapQ(number from, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == ID );
  assume( getCoeffType(aRing) == n_Q );

  if ( from != NULL )
  {
    gmp_complex *res=new gmp_complex(numberFieldToFloat(from,QTOF,aRing));
    return (number)res;
  }
  else
    return NULL;
}

static number ngcMapLongR(number from, const coeffs aRing, const coeffs r)
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

static number ngcMapR(number from, const coeffs aRing, const coeffs r)
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

static number ngcMapP(number from, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == ID );
  assume( getCoeffType(aRing) ==  n_Zp );

  if ( from != NULL )
    return ngcInit(npInt(from, aRing), r);
  else
    return NULL;
}

static number ngcCopyMap(number from, const coeffs aRing, const coeffs r)
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

  if (nCoeff_is_Q(src))
  {
    return ngcMapQ;
  }
  if (nCoeff_is_long_R(src))
  {
    return ngcMapLongR;
  }
  if (nCoeff_is_long_C(src))
  {
    return ngcCopyMap;
  }
  if (nCoeff_is_R(src))
  {
    return ngcMapR;
  }
  if (nCoeff_is_Zp(src))
  {
    return ngcMapP;
  }
  return NULL;
}

void    ngcCoeffWrite  (const coeffs r)
{
  Print("//   characteristic : 0 (complex:%d digits, additional %d digits)\n",
               r->float_len, r->float_len2);  /* long C */
}
