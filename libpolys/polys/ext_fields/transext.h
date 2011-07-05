#ifndef TRANSEXT_H
#define TRANSEXT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers in a rational function field K(t_1, .., t_s) with
*           transcendental variables t_1, ..., t_s, where s >= 1.
*           Denoting the implemented coeffs object by cf, then these numbers
*           are represented as quotients of polynomials in the polynomial
*           ring K[t_1, .., t_s] represented by cf->extring.
*/

#include <coeffs/coeffs.h>

struct ip_sring;
typedef struct ip_sring * ring;

struct sip_sideal;
typedef struct sip_sideal * ideal;

struct spolyrec;
typedef struct spolyrec polyrec;
typedef polyrec * poly;

/// struct for passing initialization parameters to naInitChar
typedef struct { ring r; } TransExtInfo;

/* a number in K(t_1, .., t_s) is represented by either NULL
   (representing the zero number), or a pointer to a fraction which contains
   the numerator polynomial and the denominator polynomial in K[t_1, .., t_s];
   if the denominator is 1, the member 'denominator' is NULL;
   as a consequence of the above we get: if some number n is not NULL, then
   n->numerator cannot be NULL;
   The member 'complexity' attempts to capture the complexity of any given
   number n, i.e., starting with a bunch of numbers n_i that have their gcd's
   cancelled out, n may be constructed from the n_i's by using field
   arithmetics (+, -, *, /). If we never cancel out gcd's during this process,
   n will become rather complex. The larger the attribute 'complexity' of n
   is, the more likely it is that n contains some non-trivial gcd. Thus, this
   attribute will be used by a heuristic method to cancel out gcd's from time
   to time. (This heuristic may be set up such that cancellation can be
   enforced after each arithmetic operation, or such that it will never take
   place.) Moreover, the 'complexity' of n is zero iff the gcd in n (that is,
   the gcd of its numerator and denominator) is trivial. */
struct fractionObject
{
  poly numerator;
  poly denominator;
  int complexity;
};
typedef struct fractionObject * fraction;

/* constants for controlling the complexity of numbers */
#define ADD_COMPLEXITY 1   /**< complexity increase due to + and - */
#define MULT_COMPLEXITY 2   /**< complexity increase due to * and / */
#define BOUND_COMPLEXITY 10   /**< maximum complexity of a number */

/* some useful accessors for fractions: */
#define IS0(f) (f == NULL) /**< TRUE iff n represents 0 in K(t_1, .., t_s) */
#define NUM(f) f->numerator
#define DEN(f) f->denominator
#define DENIS1(f) (f->denominator == NULL) /**< TRUE iff den. represents 1 */
#define NUMIS1(f) (p_IsConstant(f->numerator, cf->extRing) && \
                   n_IsOne(p_GetCoeff(f->numerator, cf->extRing), \
                           cf->extRing->cf))
                   /**< TRUE iff num. represents 1 */
#define COM(f) f->complexity

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
int      ntSize(number a, const coeffs cf);
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
