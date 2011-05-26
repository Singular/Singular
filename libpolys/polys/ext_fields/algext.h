#ifndef ALGEXT_H
#define ALGEXT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers in an algebraic extension field K[a] / < f(a) >
*           Assuming that we have a coeffs object cf, then these numbers
*           are polynomials in the polynomial ring K[a] represented by
*           cf->extRing.
*           IMPORTANT ASSUMPTIONS:
*           1.) So far we assume that cf->extRing is a valid polynomial
*               ring in exactly one variable, i.e., K[a], where K is allowed
*               to be any field (representable in SINGULAR and which may
*               itself be some extension field, thus allowing for extension
*               towers).
*           2.) Moreover, this implementation assumes that
*               cf->extRing->minideal is not NULL but an ideal with at
*               least one non-zero generator which may be accessed by
*               cf->extRing->minideal->m[0] and which represents the minimal
*               polynomial f(a) of the extension variable 'a' in K[a].
*           3.) As soon as an std method for polynomial rings becomes
*               availabe, all reduction steps modulo f(a) should be replaced
*               by a call to std. Moreover, in this situation one can finally
*               move from K[a] / < f(a) > to
*                  K[a_1, ..., a_s] / I, with I some zero-dimensional ideal
*                                        in K[a_1, ..., a_s] given by a lex
*                                        Gröbner basis.
*               The code in algext.h and algext.cc is then capable of
*               computing in K[a_1, ..., a_s] / I.
*/

#include <coeffs/coeffs.h>

struct ip_sring;
typedef struct ip_sring * ring;

struct sip_sideal;
typedef struct sip_sideal * ideal;

/// struct for passing initialization parameters to naInitChar
typedef struct { ring r; ideal i; } AlgExtInfo;

/// Get a mapping function from src into the domain of this type (n_algExt)
nMapFunc naSetMap(const coeffs src, const coeffs dst);

/// Initialize the coeffs object
BOOLEAN  naInitChar(coeffs cf, void* infoStruct);

/* Private hidden interface
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
number   naGetDenom(number &a, const coeffs cf);
number   naGetNumerator(number &a, const coeffs cf);
number   naGcd(number a, number b, const coeffs cf);
number   naLcm(number a, number b, const coeffs cf);
number   naSize(number a, const coeffs cf);
void     naDelete(number * a, const coeffs cf);
void     naCoeffWrite(const coeffs cf);
number   naIntDiv(number a, number b, const coeffs cf);
const char * naRead(const char *s, number *a, const coeffs cf);
static BOOLEAN naCoeffIsEqual(const coeffs cf, n_coeffType n, void * param);
*/

#ifdef LDEBUG
#define naTest(a) naDBTest(a,__FILE__,__LINE__,cf)
BOOLEAN  naDBTest(number a, const char *f, const int l, const coeffs r);
#else
#define naTest(a)
#endif

/* our own type */
#define naID n_algExt

/* polynomial ring in which our numbers live */
#define naRing cf->extRing

/* coeffs object in which the coefficients of our numbers live;
 * methods attached to naCoeffs may be used to compute with the
 * coefficients of our numbers, e.g., use naCoeffs->nAdd to add
 * coefficients of our numbers */
#define naCoeffs cf->extRing->cf

/* minimal polynomial */
#define naMinpoly naRing->minideal->m[0]

#endif
/* ALGEXT_H */
