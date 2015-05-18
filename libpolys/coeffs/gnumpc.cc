/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: computations with GMP complex floating-point numbers
*
* ngc == number gnu complex
*/
#include <misc/auxiliary.h>
#include <omalloc/omalloc.h>

#include <misc/mylimits.h>
#include <reporter/reporter.h>

#include "coeffs.h"
#include "numbers.h"

#include "mpr_complex.h"

#include "gnumpc.h"
#include "longrat.h"
#include "gnumpfl.h"
#include "modulop.h"
#include "shortfl.h"

/// Get a mapping function from src into the domain of this type: long_C!
nMapFunc  ngcSetMap(const coeffs src, const coeffs dst);

number ngcMapQ(number from, const coeffs r, const coeffs aRing);

void ngcSetChar(const coeffs r);

// Private interface should be hidden!!!

/// Note: MAY NOT WORK AS EXPECTED!
BOOLEAN  ngcGreaterZero(number za, const coeffs r);
BOOLEAN  ngcGreater(number a, number b, const coeffs r);
BOOLEAN  ngcEqual(number a, number b, const coeffs r);
BOOLEAN  ngcIsOne(number a, const coeffs r);
BOOLEAN  ngcIsMOne(number a, const coeffs r);
BOOLEAN  ngcIsZero(number za, const coeffs r);
number   ngcInit(long i, const coeffs r);
long     ngcInt(number &n, const coeffs r);
number   ngcNeg(number za, const coeffs r);
number   ngcInvers(number a, const coeffs r);
number   ngcParameter(int i, const coeffs r);
number   ngcAdd(number la, number li, const coeffs r);
number   ngcSub(number la, number li, const coeffs r);
number   ngcMult(number a, number b, const coeffs r);
number   ngcDiv(number a, number b, const coeffs r);
void     ngcPower(number x, int exp, number *lu, const coeffs r);
number   ngcCopy(number a, const coeffs r);
number   ngc_Copy(number a, coeffs r);
const char * ngcRead (const char *s, number *a, const coeffs r);
void     ngcWrite(number a, const coeffs r);
number   ngcRePart(number a, const coeffs r);
number   ngcImPart(number a, const coeffs r);

void     ngcDelete(number *a, const coeffs r);
void     ngcCoeffWrite(const coeffs r, BOOLEAN details);

#ifdef LDEBUG
BOOLEAN  ngcDBTest(number a, const char *f, const int l, const coeffs r);
#endif


// Why is this here? who needs it?
// number ngcMapQ(number from, const coeffs r, const coeffs aRing);

/// Our Type!
static const n_coeffType ID = n_long_C;


#ifdef LDEBUG
// not yet implemented
BOOLEAN ngcDBTest(number, const char *, const int, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return TRUE;
}
#endif

number   ngcParameter(int i, const coeffs r)
{
  assume( getCoeffType(r) == ID );
  assume(i==1);

  if( i == 1 )
    return (number)(new gmp_complex( (long)0, (long)1 ));

  return NULL; // new gmp_complex( )  // 0?
}

/*2
* n := i
*/
number ngcInit (long i, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  gmp_complex* n= new gmp_complex( (long)i, (long)0 );

  return (number)n;
}

/*2
* convert number to int
*/
long ngcInt(number &i, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  return ((gmp_complex*)i)->real();
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
  const char * const complex_parameter = n_ParameterNames(r)[0];
  assume( complex_parameter != NULL );
  const int N = strlen(complex_parameter);

  if ((*s >= '0') && (*s <= '9'))
  {
    gmp_float *re=NULL;
    s=ngfRead(s,(number *)&re, r); // FIXME? TODO? // extern const char *   ngfRead (const char *s, number *a, const coeffs r);
    gmp_complex *aa=new gmp_complex(*re);
    *a=(number)aa;
    delete re;
  }
  else if (strncmp(s, complex_parameter, N)==0)
  {
    s += N;
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
void ngcWrite (number a, const coeffs r)
{
  assume( getCoeffType(r) == ID );

  if (a==NULL)
    StringAppendS("0");
  else
  {
    char *out;
    out= complexToStr(*(gmp_complex*)a, r->float_len, r);
    StringAppendS(out);
    //    omFreeSize((void *)out, (strlen(out)+1)* sizeof(char) );
    omFree( (void *)out );
  }
}

BOOLEAN ngcCoeffIsEqual (const coeffs r, n_coeffType n, void * parameter)
{
  if (n==ID)
  {
    LongComplexInfo* p = (LongComplexInfo *)(parameter);

    if ((p==NULL)
      && (6==r->float_len)
      && (6==r->float_len2)
      && (strcmp("i",n_ParameterNames(r)[0]) == 0)
      )
        return TRUE;
    if ((p!=NULL) &&
        (p->float_len == r->float_len) &&
        (p->float_len2 == r->float_len2)
       )
      if (strcmp(p->par_name, n_ParameterNames(r)[0]) == 0)
        return (TRUE);
  }
  return (FALSE);
}

static void ngcKillChar(coeffs r)
{
  char** p = (char**)n_ParameterNames(r);

  const int P = n_NumberOfParameters(r);

  for( int i = 1; i <= P; i++ )
    if (p[i-1] != NULL)
      omFree( (ADDRESS)p[i-1] );

  omFreeSize((ADDRESS)p, P * sizeof(char*));
}

static char* ngcCoeffString(const coeffs r)
{
  const char *p=n_ParameterNames(r)[0];
  char *s=(char*)omAlloc(31+strlen(p));
  sprintf(s,"complex,%d,%d,%s",r->float_len,r->float_len2,p);
  return s;
}

BOOLEAN ngcInitChar(coeffs n, void* parameter)
{
  assume( getCoeffType(n) == ID );
  n->is_field=TRUE;
  n->is_domain=TRUE;
  n->rep=n_rep_gmp_complex;

  n->cfKillChar = ngcKillChar;
  n->ch = 0;
  n->cfCoeffString=ngcCoeffString;

  n->cfDelete  = ngcDelete;
  //n->cfNormalize=ndNormalize;
  n->cfInit   = ngcInit;
  n->cfInt    = ngcInt;
  n->cfAdd     = ngcAdd;
  n->cfSub     = ngcSub;
  n->cfMult    = ngcMult;
  n->cfDiv     = ngcDiv;
  n->cfExactDiv= ngcDiv;
  n->cfInpNeg     = ngcNeg;
  n->cfInvers  = ngcInvers;
  n->cfCopy   = ngcCopy;
  n->cfGreater = ngcGreater;
  n->cfEqual   = ngcEqual;
  n->cfIsZero  = ngcIsZero;
  n->cfIsOne   = ngcIsOne;
  n->cfIsMOne  = ngcIsMOne;
  n->cfGreaterZero = ngcGreaterZero;

  n->cfWriteLong  = ngcWrite;
  n->cfWriteShort = ngcWrite;

  n->cfRead    = ngcRead;
  n->cfPower   = ngcPower;
  n->cfSetMap = ngcSetMap;
  n->cfRePart  = ngcRePart;
  n->cfImPart  = ngcImPart;
  n->cfCoeffWrite = ngcCoeffWrite;
    // cfSize  = ndSize;
#ifdef LDEBUG
  //n->cfDBTest  = ndDBTest; // not yet implemented: ngcDBTest
#endif

  n->nCoeffIsEqual = ngcCoeffIsEqual;

  n->cfSetChar=ngcSetChar;

// we need to initialize n->nNULL at least for minpoly printing
  n->nNULL  = n->cfInit(0,n);

/*
  //r->cfInitChar=nlInitChar;
  r->cfKillChar=NULL;

  r->cfMult  = nlMult;
  r->cfSub   = nlSub;
  r->cfAdd   = nlAdd;
  r->cfDiv   = nlDiv;
  r->cfIntMod= nlIntMod;
  r->cfExactDiv= nlExactDiv;
  r->cfInit = nlInit;
  r->cfSize  = nlSize;
  r->cfInt  = nlInt;
#ifdef HAVE_RINGS
  r->cfDivComp = NULL; // only for ring stuff
  r->cfIsUnit = NULL; // only for ring stuff
  r->cfGetUnit = NULL; // only for ring stuff
  r->cfExtGcd = NULL; // only for ring stuff
#endif
  r->cfInpNeg   = nlNeg;
  r->cfInvers= nlInvers;
  r->cfCopy  = nl_Copy;
  r->cfRePart = nl_Copy;
  r->cfImPart = ndReturn0;
  r->cfWriteLong = nlWrite;
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

  n->iNumberOfParameters = 1;
  n->cfParameter = ngcParameter;

  char ** pParameterNames = (char **) omAlloc0(sizeof(char *));

  if( parameter != NULL)
  {
    LongComplexInfo* p = (LongComplexInfo*)parameter;
    pParameterNames[0] = omStrDup(p->par_name);
    // fix wrong parameters:
    if (p->float_len<SHORT_REAL_LENGTH) p->float_len=SHORT_REAL_LENGTH;
    n->float_len = p->float_len;
    n->float_len2 = p->float_len2;

  } else // default values, just for testing!
  {
    pParameterNames[0] = omStrDup("i");
    n->float_len = SHORT_REAL_LENGTH;
    n->float_len2 = SHORT_REAL_LENGTH;
  }

  assume( pParameterNames != NULL );
  assume( pParameterNames[0] != NULL );

  n->pParameterNames = (const char**)pParameterNames;

  // NOTE: n->complex_parameter was replaced by n_ParameterNames(n)[0]
  // TODO: nfKillChar MUST destroy n->pParameterNames[0] (0-term. string) && n->pParameterNames (array of size 1)

  return FALSE;
}

void ngcSetChar(const coeffs r)
{
  setGMPFloatDigits(r->float_len, r->float_len2);
}



number ngcMapQ(number from, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == ID );
  assume( aRing->rep == n_rep_gap_rat);

  if ( from != NULL )
  {
    gmp_complex *res=new gmp_complex(numberFieldToFloat(from,QTOF,aRing));
    return (number)res;
  }
  else
    return NULL;
}

number ngcMapZ(number from, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == ID );
  assume( aRing->rep == n_rep_gap_gmp);

  if ( from != NULL )
  {
    if (SR_HDL(from) & SR_INT)
    {
      gmp_float f_i= gmp_float(SR_TO_INT(from));
      gmp_complex *res=new gmp_complex(f_i);
      return (number)res;
    }
    gmp_float f_i=(mpz_ptr)from;
    gmp_complex *res=new gmp_complex(f_i);
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
    gmp_complex *res=new gmp_complex((double)nrFloat(from)); // FIXME? TODO? // extern float   nrFloat(number n);
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
    return ngcInit(npInt(from, aRing), r); // FIXME? TODO? // extern int     npInt         (number &n, const coeffs r);
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

  if (src->rep==n_rep_gap_rat) /* Q, Z*/
  {
    return ngcMapQ;
  }
  if (src->rep==n_rep_gap_gmp) /* Z */
  {
    return ngcMapZ;
  }
  if ((src->rep==n_rep_gmp_float) && nCoeff_is_long_R(src))
  {
    return ngcMapLongR;
  }
  if ((src->rep==n_rep_gmp_complex) && nCoeff_is_long_C(src))
  {
    return ngcCopyMap;
  }
  if ((src->rep==n_rep_float) && nCoeff_is_R(src))
  {
    return ngcMapR;
  }
  if ((src->rep==n_rep_int) && nCoeff_is_Zp(src))
  {
    return ngcMapP;
  }
  return NULL;
}

void    ngcCoeffWrite  (const coeffs r, BOOLEAN /*details*/)
{
  Print("//   characteristic : 0 (complex:%d digits, additional %d digits)\n",
        r->float_len, r->float_len2);  /* long C */
  Print("//   1 parameter    : %s \n", n_ParameterNames(r)[0]); // this trailing space is for compatibility with the legacy Singular
  Print("//   minpoly        : (%s^2+1)\n", n_ParameterNames(r)[0]);
}
