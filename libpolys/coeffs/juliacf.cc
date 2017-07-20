/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT:
*/



#include "misc/auxiliary.h"
#include "misc/mylimits.h"

#include "reporter/reporter.h"
#include "omalloc/omalloc.h"

#include "coeffs/numbers.h"
#include "coeffs/coeffs.h"
#include "coeffs/mpr_complex.h"

#include "coeffs/juliacf.h"
#include "coeffs/longrat.h"

#ifdef HAVE_JULIA

// Private interface should be hidden!!!

#ifdef LDEBUG
static BOOLEAN jcfDBTest(number a, const coeffs r, const char *f, const int l);
#endif

/// Get a mapping function from src into the domain of this type: n_R
static nMapFunc jcfSetMap(const coeffs src, const coeffs dst);

static void jcfCoeffWrite (const coeffs r, BOOLEAN /*details*/)
{
  assume( getCoeffType(r) == n_R );
  PrintS("Float()");  /* R */
}


static BOOLEAN jcfGreaterZero (number k, const coeffs r)
{
}

static number jcfMult (number a,number b, const coeffs r)
{
}

/*2
* create a number from int
*/
static number jcfInit (long i, const coeffs r)
{
}

/*2
* convert a number to int
*/
static long jcfInt(number &n, const coeffs r)
{
}

static int jcfSize(number n, const coeffs)
{
}

static number jcfAdd (number a, number b, const coeffs r)
{
}

static number jcfSub (number a, number b, const coeffs r)
{
}

static BOOLEAN jcfIsZero (number  a, const coeffs r)
{
}

static BOOLEAN jcfIsOne (number a, const coeffs r)
{
}

static BOOLEAN jcfIsMOne (number a, const coeffs r)
{
}

static number jcfDiv (number a,number b, const coeffs r)
{
}

static number jcfInvers (number c, const coeffs r)
{
}

static number jcfNeg (number c, const coeffs r)
{
}

static BOOLEAN jcfGreater (number a,number b, const coeffs r)
{
}

static BOOLEAN jcfEqual (number a,number b, const coeffs r)
{
}

static void jcfWrite (number a, const coeffs r)
{
}

#if 0
static void jcfPower (number a, int i, number * result, const coeffs r)
{
}
#endif

static const char * jcfRead (const char *s, number *a, const coeffs r)
{

}

static void jcfKillChar(coeffs r)
{
}

static number jcfCopy(number a, const coeffs cf)
{
}

static BOOLEAN jcfCoeffIsEqual(const coeffs r, n_coeffType n, void * parameter)
{
}

#ifdef LDEBUG
/*2
* test valid numbers: not implemented yet
*/
#pragma GCC diagnostic ignored "-Wunused-parameter"
static BOOLEAN  jcfDBTest(number a, const char *f, const int l, const coeffs r)
{
  return TRUE;
}
#endif

static number jcfMapP(number from, const coeffs aRing, const coeffs r)
{
}

static number jcfMapLongR(number from, const coeffs aRing, const coeffs r)
{
}

static number jcfMapC(number from, const coeffs aRing, const coeffs r)
{
}


static number jcfMapQ(number from, const coeffs aRing, const coeffs r)
{
}

static number jcfMapZ(number from, const coeffs aRing, const coeffs r)
{
}

static nMapFunc jcfSetMap(const coeffs src, const coeffs dst)
{
  return NULL;
}

static char* jcfCoeffString(const coeffs r)
{
  return omStrDup("Float()");
}

static char* jcfCoeffName(const coeffs r)
{
  return (char*)"Float()";
}

BOOLEAN jcfInitChar(coeffs n, void* p)
{
  n->is_field=TRUE;
  n->is_domain=TRUE;
  n->rep=n_rep_float;

  n->cfKillChar = jcfKillChar;
  n->ch = 0;
  n->cfCoeffString = jcfCoeffString;
  n->cfCoeffName = jcfCoeffName;

  n->cfInit = jcfInit;
  n->cfInt  = jcfInt;
  n->cfAdd   = jcfAdd;
  n->cfSub   = jcfSub;
  n->cfMult  = jcfMult;
  n->cfDiv   = jcfDiv;
  n->cfExactDiv= jcfDiv;
  n->cfInpNeg   = jcfNeg;
  n->cfInvers= jcfInvers;
  n->cfCopy  = jcfCopy;
  n->cfGreater = jcfGreater;
  n->cfEqual = jcfEqual;
  n->cfIsZero = jcfIsZero;
  n->cfIsOne = jcfIsOne;
  n->cfIsMOne = jcfIsMOne;
  n->cfGreaterZero = jcfGreaterZero;
  n->cfWriteLong = jcfWrite;
  n->cfRead = jcfRead;
  //n->cfPower = jcfPower;
  n->cfSetMap = jcfSetMap;
  n->cfCoeffWrite  = jcfCoeffWrite;

    /*nSize  = ndSize;*/
#ifdef LDEBUG
  //n->cfDBTest=ndDBTest; // not yet implemented: jcfDBTest;
#endif

  n->nCoeffIsEqual = jcfCoeffIsEqual;
  return FALSE;
}
#endif
