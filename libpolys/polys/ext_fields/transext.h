#ifndef TRANSEXT_H
#define TRANSEXT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers in a rational function field K(t_1, .., t_s) with
*           transcendental variables t_1, ..., t_s, where s >= 1.
*           Denoting the implemented coeffs object by cf, then these numbers
*           are represented as quotients of polynomials living in the
*           polynomial ring K[t_1, .., t_s] represented by cf->extring.
*
*           An element of K(t_1, .., t_s) may have numerous representations,
*           due to the possibility of common polynomial factors in the
*           numerator and denominator. This problem is handled by a
*           cancellation heuristic: Each number "knows" its complexity
*           which is 0 if and only if common factors have definitely been
*           cancelled, and some positive integer otherwise.
*           Each arithmetic operation of two numbers with complexities c1
*           and c2 will result in a number of complexity c1 + c2 + some
*           penalty (specific for each arithmetic operation; see constants
*           in the *.h file). Whenever the resulting complexity exceeds a
*           certain threshold (see constant in the *.h file), then the
*           cancellation heuristic will call 'factory' to compute the gcd
*           and cancel it out in the given number. (This definite cancel-
*           lation will also be performed at the beginning of ntWrite,
*           ensuring that any output is free of common factors.
*           For the special case of K = Q (i.e., when computing over the
*           rationals), this definite cancellation procedure will also take
*           care of nested fractions: If there are fractional coefficients
*           in the numerator or denominator of a number, then this number
*           is being replaced by a quotient of two polynomials over Z, or
*           - if the denominator is a constant - by a polynomial over Q.
*/

#include "coeffs/coeffs.h"
#include "polys/monomials/ring.h"

// restrict access to the internal representation as much as possible:
#ifdef TRANSEXT_PRIVATES

/** a number in K(t_1, .., t_s) is represented by either NULL
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
     the gcd of its numerator and denominator) is trivial.
 */
struct fractionObject
{
  poly numerator;
  poly denominator;
  int complexity;
};

typedef struct fractionObject * fraction;


#define NUM(f) ((f)->numerator)
#define DEN(f) ((f)->denominator)

/* some useful accessors for fractions: */
#define IS0(f) (f == NULL)
/**< TRUE iff n represents 0 in K(t_1, .., t_s) */

#define DENIS1(f) (DEN(f) == NULL)
/**< TRUE iff den. represents 1 */

/// takes over p!
number ntInit(poly p, const coeffs cf);

#endif



/// struct for passing initialization parameters to naInitChar
typedef struct { ring r; } TransExtInfo;

/// Get a mapping function from src into the domain of this type (n_transExt)
nMapFunc ntSetMap(const coeffs src, const coeffs dst);

/// Initialize the coeffs object
BOOLEAN  ntInitChar(coeffs cf, void* infoStruct);

number ntDiff(number a, number d, const coeffs cf);

/* Private hidden interface
BOOLEAN  ntGreaterZero(number a, const coeffs cf);
BOOLEAN  ntGreater(number a, number b, const coeffs cf);
BOOLEAN  ntEqual(number a, number b, const coeffs cf);
BOOLEAN  ntIsOne(number a, const coeffs cf);
BOOLEAN  ntIsMOne(number a, const coeffs cf);
BOOLEAN  ntIsZero(number a, const coeffs cf);
number   ntInit(long i, const coeffs cf);
int      ntInt(number &a, const coeffs cf);
number   ntNeg(number a, const coeffs cf);
number   ntInvers(number a, const coeffs cf);
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
void     ntCoeffWrite(const coeffs cf, BOOLEAN details);
const char * ntRead(const char *s, number *a, const coeffs cf);
static BOOLEAN ntCoeffIsEqual(const coeffs cf, n_coeffType n, void * param);
*/

/// if m == var(i)/1 => return i,
int ntIsParam(number, const coeffs);

/// helper routine for calling singclap_gcd_r
poly gcd_over_Q ( poly f, poly g, const ring r);
#endif
/* TRANSEXT_H */
