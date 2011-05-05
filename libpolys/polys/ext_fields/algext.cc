/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers in an algebraic extension field K(a)
*           Assuming that we have a coeffs object cf, then these numbers
*           are polynomials in the polynomial ring K[a] represented by
*           cf->algring.
*           IMPORTANT ASSUMPTIONS:
*           1.) So far we assume that cf->algring is a valid polynomial
*               ring in exactly one variable, i.e., K[a], where K is allowed
*               to be any field (representable in SINGULAR and which may
*               itself be some extension field, thus allowing for extension
*               towers).
*           2.) Moreover, this implementation assumes that
*               cf->algring->minideal is not NULL but an ideal with at
*               least one non-zero generator which may be accessed by
*               cf->algring->minideal->m[0] and which represents the minimal
*               polynomial of the extension variable 'a' in K[a].
*/

#include "config.h"
#include <misc/auxiliary.h>

#include <omalloc/omalloc.h>

#include <reporter/reporter.h>

#include <coeffs/coeffs.h>
#include <coeffs/numbers.h>

#include <polys/monomials/ring.h>
#include <polys/monomials/p_polys.h>
#include <polys/simpleideals.h>

#include <polys/ext_fields/algext.h>

/// our own type
static const n_coeffType naID = n_Ext;

/// polynomial ring in which our numbers live; to be set by naInitChar
static ring naRing = NULL;

/* coeffs object in which the coefficients of our numbers live;
 * to be set by naInitChar;
 * methods attached to naCoeffs may be used to compute with the
 * coefficients of our numbers, e.g., use naCoeffs->nAdd to add
 * coefficients of our numbers */
static coeffs naCoeffs = NULL;

/// minimal polynomial; to be set by naInitChar
static poly naMinpoly = NULL;

/// forward declarations
BOOLEAN  naGreaterZero(number a, const coeffs cf); 
BOOLEAN  naGreater(number a, number b, const coeffs cf);
BOOLEAN  naEqual(number a, number b, const coeffs cf);
BOOLEAN  naIsOne(number a, const coeffs cf);
BOOLEAN  naIsMOne(number a, const coeffs cf);
BOOLEAN  naIsZero(number a, const coeffs cf);
number   naInit(int i, const coeffs cf);
int      naInt(number &a, const coeffs cf);
number   naNeg(number a, const coeffs cf);
number   naInvers(number a, const coeffs cf);
number   naPar(int i, const coeffs cf);
number   naAdd(number a, number b, const coeffs cf);
number   naSub(number a, number b, const coeffs cf);
number   naMult(number a, number b, const coeffs cf);
number   naDiv(number a, number b, const coeffs cf);
void     naPower(number a, int exp, number *b, const coeffs cf);
number   naCopy(number a, const coeffs cf);
void     naWrite(number &a, const coeffs cf);
number   naRePart(number a, const coeffs cf);
number   naImPart(number a, const coeffs cf);
number   naGetDenom(number a, const coeffs cf);
number   naGetNumerator(number a, const coeffs cf);
number   naGcd(number a, const coeffs cf);
number   naLcm(number a, const coeffs cf);
number   naSize(number a, const coeffs cf);
void     naDelete(number *a, const coeffs cf);
void     naCoeffWrite(const coeffs cf);
number   naIntDiv(number a, number b, const coeffs r);
number   naPar(int i, const coeffs r);
number   naInit_bigint(number a, const coeffs src, const coeffs dst);
const char * naRead(const char *s, number *a, const coeffs cf);
static BOOLEAN naCoeffIsEqual(const coeffs cf, n_coeffType n, void * param);

#ifdef LDEBUG
BOOLEAN naDBTest(number a, const char *f, const int l, const coeffs r)
{
  assume(getCoeffType(r) == naID);
  #ifdef LDEBUG
  omCheckAddr(&a);
  #endif
  /// What else is there to test here?
  return TRUE;
}
#endif

BOOLEAN naIsZero(number a, const coeffs cf)
{
  naTest(a);
  return (a == NULL);
}

void naDelete(number *a, const coeffs cf)
{
  if (a == NULL) return;
  p_Delete((poly *)a, naRing);
  *a = NULL;
}

BOOLEAN naEqual (number a, number b, const coeffs cf)
{
  naTest(a); naTest(b);
  
  /// simple tests
  if (a == b) return TRUE;
  if ((a == NULL) && (b != NULL)) return FALSE;
  if ((b == NULL) && (a != NULL)) return FALSE;

  /// deg test
  int aDeg = 0;
  if (a != NULL) aDeg = p_Deg((poly)a, naRing);
  int bDeg = 0;
  if (b != NULL) bDeg = p_Deg((poly)b, naRing);
  if (aDeg != bDeg) return FALSE;
  
  /// subtraction test
  number c = naSub(a, b, cf);
  BOOLEAN result = naIsZero(c, cf);
  naDelete(&c, naCoeffs);
  return result;
}

number naGetNumerator(number &a, const coeffs cf)
{
  naTest(a);
  return (number)p_Copy((poly)a, naRing);
}

number naGetDenom(number &a, const coeffs cf)
{
  naTest(a);
  return naInit(1, cf);
}

BOOLEAN naIsOne(number a, const coeffs cf)
{
  naTest(a);
  if (p_GetExp((poly)a, 1, naRing) != 0) return FALSE;
  return n_IsOne(p_GetCoeff((poly)a, naRing), naCoeffs);
}

BOOLEAN naIsMOne(number a, const coeffs cf)
{
  naTest(a);
  if (p_GetExp((poly)a, 1, naRing) != 0) return FALSE;
  return n_IsMOne(p_GetCoeff((poly)a, naRing), naCoeffs);
}

/// this is in-place, modifies a
number naNeg(number a, const coeffs cf)
{
  naTest(a);
  if (a != NULL) a = (number)p_Neg((poly)a, naRing);
  return a;
}

number naRePart(number a, const coeffs cf)
{
  naTest(a);
  return (number)p_Copy((poly)a, naRing);
}

number naImPart(number a, const coeffs cf)
{
  naTest(a);
  return NULL;
}

number naInit(int i, const coeffs cf)
{
  if (i == 0) return NULL;
  else        return (number)p_ISet(i, naRing);
}

int naInt(number &a, const coeffs cf)
{
  naTest(a);
  if (p_GetExp((poly)a, 1, naRing) != 0) return 0;
  return n_Int(p_GetCoeff((poly)a, naRing), naCoeffs);
}

/* TRUE iff (a != 0 and (b == 0 or deg(a) > deg(b))) */
BOOLEAN naGreater(number a, number b, const coeffs cf)
{
  if (naIsZero(a, cf)) return FALSE;
  if (naIsZero(b, cf)) return TRUE;
  int aDeg = 0;
  if (a != NULL) aDeg = p_Deg((poly)a, naRing);
  int bDeg = 0;
  if (b != NULL) bDeg = p_Deg((poly)b, naRing);
  return (aDeg > bDeg);
}

/* TRUE iff a != 0 and (LC(a) > 0 or deg(a) > 0) */
BOOLEAN naGreaterZero(number a, const coeffs cf)
{
  naTest(a);
  if (a == NULL)                                            return FALSE;
  if (n_GreaterZero(p_GetCoeff((poly)a, naRing), naCoeffs)) return TRUE;
  if (p_Deg((poly)a, naRing) > 0)                           return TRUE;
  return FALSE;
}

void naCoeffWrite(const coeffs cf)
{
  char *x = rRingVar(0, naRing);
  Print("//   Coefficients live in the extension field K[%s]/<f(%s)>\n", x, x);
  Print("//   with the minimal polynomial f(%s) = %s\n", x,
        p_String(naMinpoly, naRing));
  PrintS("//   and K: "); n_CoeffWrite(cf->algring->cf);
}

/////////////////////////////////////////////////////////////////////
/////// to be corrected --> /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

nMapFunc naSetMap(const coeffs src, const coeffs dst)
{
  assume(dst->type == naID);
  return NULL;
}

number naSub(number a, number b, const coeffs cf)
{
  return naInit(17, cf);
}

/////////////////////////////////////////////////////////////////////
/////// <-- to be corrected /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

BOOLEAN naInitChar(coeffs cf, void* infoStruct)
{
  ExtInfo *e = (ExtInfo *)infoStruct;
  /// first check whether cf->algring != NULL and delete old ring???
  cf->algring           = e->r;
  cf->algring->minideal = e->i;

  assume(cf->algring                     != NULL);      // algring;
  assume((cf->algring->minideal          != NULL) &&    // minideal has one
         (IDELEMS(cf->algring->minideal) != 0)    &&    // non-zero generator
         (cf->algring->minideal->m[0]    != NULL)    ); // at m[0];
  assume(cf->algring->cf                 != NULL);      // algring->cf;
  assume(getCoeffType(cf) == naID);                     // coeff type;
  
  naRing = cf->algring;
  naCoeffs = cf->algring->cf;
  naMinpoly = naRing->minideal->m[0];
  #ifdef LDEBUG
  omCheckAddr(naMinpoly);
  #endif
  
  cf->cfGreaterZero  = naGreaterZero;
  cf->cfGreater      = naGreater;
  cf->cfEqual        = naEqual;
  cf->cfIsZero       = naIsZero;
  cf->cfIsOne        = naIsOne;
  cf->cfIsMOne       = naIsMOne;
  cf->cfInit         = naInit;
  cf->cfInt          = naInt;
  cf->cfNeg          = naNeg;
  cf->cfInvers       = NULL; //naInvers;
  cf->cfPar          = NULL; //naPar;
  cf->cfAdd          = NULL; //naAdd;
  cf->cfSub          = naSub;
  cf->cfMult         = NULL; //naMult;
  cf->cfDiv          = NULL; //naDiv;
  cf->cfExactDiv     = NULL; //just write here 'naDiv' when naDiv is available;
  cf->cfPower        = NULL; //naPower;
  cf->cfCopy         = NULL; //naCopy;
  cf->cfWrite        = NULL; //naWrite;
  cf->cfRead         = NULL; //naRead;
  cf->cfDelete       = naDelete;
  cf->cfSetMap       = naSetMap;
  cf->cfGetDenom     = naGetDenom;
  cf->cfGetNumerator = naGetNumerator;
  cf->cfRePart       = naRePart;
  cf->cfImPart       = naImPart;
  cf->cfGcd          = NULL; //naGcd;
  cf->cfLcm          = NULL; //naLcm;
  cf->cfCoeffWrite   = NULL; //naCoeffWrite;
  cf->cfSize         = NULL; //naSize;
  cf->cfCoeffWrite   = naCoeffWrite;
  cf->nCoeffIsEqual  = NULL; //naCoeffIsEqual;
  cf->cfIntDiv       = NULL; //naCoeffIsEqual;
  cf->cfPar          = NULL; //naPar;
  cf->cfDBTest       = naDBTest;
  cf->cfInit_bigint  = NULL; //naInit_bigint;
  
  return FALSE;
}
