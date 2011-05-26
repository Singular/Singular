#ifndef TRANSEXT_H
#define TRANSEXT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers in a transcendental extension field K(t_1, .., t_s)
*           Assuming that we have a coeffs object cf, then these numbers
*           are quotients of polynomials in the polynomial ring
*           K[t_1, .., t_s] represented by cf->algring.
*/

#include <coeffs/coeffs.h>

struct ip_sring;
typedef struct ip_sring * ring;

struct sip_sideal;
typedef struct sip_sideal * ideal;

/// struct for passing initialization parameters to naInitChar
typedef struct { ring r; } transExtInfo;

/* a number in K(t_1, .., t_s) is represented by either NULL
   (representing the zero number), or a pointer to a fraction which contains
   the numerator polynomial and the denominator polynomial in K[t_1, .., t_s];
   if the denominator is 1, the member 'denominator' is NULL;
   as a consequence of the above we get: if some number n is not NULL, then
   n->numerator cannot be NULL */
struct { poly numerator; poly denominator; } ip_fraction;
typedef struct ip_fraction * fraction;
/* some useful accessors: */
#define is0(n) (n == NULL) /**< TRUE iff n represents 0 in K(t_1, .., t_s) */
#define num(n) n->numerator
#define den(n) n->denominator
#define denIs1(n) (n->denominator == NULL) /**< TRUE iff the denom. is 1 */

/// Get a mapping function from src into the domain of this type (n_transExt)
nMapFunc ntSetMap(const coeffs src, const coeffs dst);

/// Initialize the coeffs object
BOOLEAN  ntInitChar(coeffs cf, void* infoStruct);

/* Private hidden interface
BOOLEAN  ntGreaterZero(number a, const coeffs cf); 
BOOLEAN  ntGreater(number a, number b, const coeffs cf);
BOOLEAN  ntEqual(number a, number b, const coeffs cf);
BOOLEAN  ntIsOne(number a, const coeffs cf);
BOOLEAN  ntIsMOne(number a, const coeffs cf);
BOOLEAN  ntIsZero(number a, const coeffs cf);
number   ntInit(int i, const coeffs cf);
int      ntInt(number &a, const coeffs cf);
number   ntNeg(number a, const coeffs cf);
number   ntInvers(number a, const coeffs cf);
number   ntPar(int i, const coeffs cf);
number   ntAdd(number a, number b, const coeffs cf);
number   ntSub(number a, number b, const coeffs cf);
number   ntMult(number a, number b, const coeffs cf);
number   ntDiv(number a, number b, const coeffs cf);
void     ntPower(number a, int exp, number *b, const coeffs cf);
number   ntCopy(number a, const coeffs cf);
void     ntWrite(number &a, const coeffs cf);
number   ntRePart(number a, const coeffs cf);
number   ntImPart(number a, const coeffs cf);
number   ntGetDenom(number &a, const coeffs cf);
number   ntGetNumerator(number &a, const coeffs cf);
number   ntGcd(number a, number b, const coeffs cf);
number   ntLcm(number a, number b, const coeffs cf);
number   ntSize(number a, const coeffs cf);
void     ntDelete(number * a, const coeffs cf);
void     ntCoeffWrite(const coeffs cf);
number   ntIntDiv(number a, number b, const coeffs cf);
const char * ntRead(const char *s, number *a, const coeffs cf);
static BOOLEAN ntCoeffIsEqual(const coeffs cf, n_coeffType n, void * param);
*/

#ifdef LDEBUG
#define ntTest(a) ntDBTest(a,__FILE__,__LINE__,cf)
BOOLEAN  ntDBTest(number a, const char *f, const int l, const coeffs r);
#else
#define ntTest(a)
#endif

/* our own type */
#define ntID n_transExt

/* polynomial ring in which the numerators and denominators of our
   numbers live */
#define ntRing cf->extRing

/* coeffs object in which the coefficients of our numbers live;
 * methods attached to ntCoeffs may be used to compute with the
 * coefficients of our numbers, e.g., use ntCoeffs->nAdd to add
 * coefficients of our numbers */
#define ntCoeffs cf->extRing->cf

#endif
/* TRANSEXT_H */
