#ifndef ALGEXT_H
#define ALGEXT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
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
*               cf->extRing->qideal is not NULL but an ideal with at
*               least one non-zero generator which may be accessed by
*               cf->extRing->qideal->m[0] and which represents the minimal
*               polynomial f(a) of the extension variable 'a' in K[a].
*           3.) As soon as an std method for polynomial rings becomes
*               available, all reduction steps modulo f(a) should be replaced
*               by a call to std. Moreover, in this situation one can finally
*               move from K[a] / < f(a) > to
*                  K[a_1, ..., a_s] / I, with I some zero-dimensional ideal
*                                        in K[a_1, ..., a_s] given by a lex
*                                        Gröbner basis.
*               The code in algext.h and algext.cc is then capable of
*               computing in K[a_1, ..., a_s] / I.
*/

#include "coeffs/coeffs.h"
#include "polys/monomials/ring.h"

/// struct for passing initialization parameters to naInitChar
typedef struct { ring r; /*ideal i;*/ } AlgExtInfo; // `r.qideal` is supposed to be `i`

/// Get a mapping function from src into the domain of this type (n_algExt)
nMapFunc naSetMap(const coeffs src, const coeffs dst);

/// Initialize the coeffs object
BOOLEAN  naInitChar(coeffs cf, void* infoStruct);
BOOLEAN n2pInitChar(coeffs cf, void * infoStruct);

/// if m == var(i)/1 => return i,
int naIsParam(number, const coeffs);

/// assumes that p and q are univariate polynomials in r,
///   mentioning the same variable;
///   assumes a global monomial ordering in r;
///   assumes that not both p and q are NULL;
///   returns the gcd of p and q;
///   moreover, afterwards pFactor and qFactor contain appropriate
///   factors such that gcd(p, q) = p * pFactor + q * qFactor;
///   leaves p and q unmodified
poly      p_ExtGcd(poly p, poly &pFactor, poly q, poly &qFactor, ring r);

char* naCoeffName(const coeffs r);
#endif
/* ALGEXT_H */
