#ifndef RAT_FUNCT_FIELD_H
#define RAT_FUNCT_FIELD_H

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: RatFunctField.h 12469 2010-05-14 10:39:49Z frank $ */
/*
* ABSTRACT: elements of a rational function field
*
* This field has the form K(t_1, t_2, ..., t_m), where K is
* itself a field, and the t_i are transcendental over K.
* The elements in K(t_1, t_2, ..., t_m) can be treated as a
* specific type of number, see numbers.h.
*/
#include "structs.h"
#include "longrat.h"
#include "polys-impl.h"

struct ratFraction
{
  poly n;     /* the numerator polynomial */
  poly d;     /* the denominator polynomial */
  BOOLEAN c;  /* true if and only if z/n is canceled */
};

typedef struct ratFraction* pRatFraction;

/* ring initialisation and setting of function pointers: */
void          nratSetChar(int p, ring r);
/* for deleting the represented element of K(t_1, t_2, ..., t_m): */
void          nratDelete (number *p, const ring r);
/* for mapping the integer i into K(t_1, t_2, ..., t_m): */
number        nratInit(int i, const ring r);
/* i-th extension variable, that is, t_i: */
number        nratPar(int i);
/* degree of the numerator of n, seen as the degree of the numerator
   polynomial in K(t_1, t_2, ..., t_m): */
int           nratParDeg(number n);
/* measure of complexity for n; takes coeffs and degree into account: */
int           nratSize(number n);
/* for (sensibly) mapping n into the integers, Z: */
int           nratInt(number &n, const ring r);
/* for checking whether n is 0: */
BOOLEAN       nratIsZero(number n);
/* for checking whether n is 1: */
BOOLEAN       nratIsOne(number  n);
/* for checking whether n is -1: */
BOOLEAN       nratIsMOne(number  n);
/* for determining whether n1 = n2: */
BOOLEAN       nratEqual(number n1, number n2);
/* for determining whether n1 > n2; so far, this is true iff n1 != n2: */
BOOLEAN       nratGreater(number n1, number n2);
/* for computing -n; use according to the pattern 'n := - n;' only: */
number        nratNeg(number n);
/* for computing 1/n as an element of K(t_1, t_2, ..., t_m): */
number        nratInvers(number n);
/* for computing n^exp inside K(t_1, t_2, ..., t_m); result will be stored
   in 'result': */
void          nratPower(number n, int exp, number* result);
/* for determining whether n > 0; so far, this is true iff n != 0: */
BOOLEAN       nratGreaterZero(number n);
/* for producing a copy of n: */
number        nratCopy(number n);
/* for computing n1 + n2: */
number        nratAdd(number n1, number n2);
/* for computing n1 * n2: */
number        nratMult(number n1, number n2);
/* for computing n1 / n2: */
number        nratDiv(number n1, number n2);
/* for performing polynomial division; assumes that n1 and n2 are
   polynomials; then this returns f, where n1 = n2 * f + r, and
   LM(n2) > LM(r): */
number        nratIntDiv(number n1, number n2);
/* for performing polynomial division; assumes that n1 and n2 are
   polynomials; then this returns r, where n1 = n2 * f + r, and
   LM(n2) > LM(r): */
//number      nratIntMod(number n1, number n2);
/* for computing n1 - n2: */
number        nratSub(number la, number li);
/* for simplifying coefficients (elements of K) and cancelling common
   factors in numerator and denominator: */
void          nratNormalize(number &p);
/* for computing gcd(n1, n2); assumes that n1 and n2 are
   polynomials: */
number        nratGcd(number n1, number n2, const ring r);
/* for computing lcm(num1, denom2), where num1 is the numerator of n1,
   and denom2 is the denominator of n2: */
number        nratLcm(number n1, number n2, const ring r);
/* next method tries to read a number (which represents an element
   of K(t_1, t_2, ..., t_m)) from s; in case of success, the number
   will sit inside n, and the returned pointer points to the
   remainder of s (starting after the successfully parsed substring) : */
const char*   nratRead(const char* s, number* n);
/* for writing a printable version of n into the global string buffer: */
void          nratWrite(number &n, const ring r);
/* for retrieving a printable version of ?????????: */
char*         nratName(number n);
/* for obtaining a function pointer that may be used to map an element
   of K(t_1, t_2, ..., t_m) to the destination ring */
nMapFunc      nratSetMap(const ring src, const ring dst);
/* for mapping n into Z/p(t_1, t_2, ..., t_m); this method assumes
   that n lives in Q, and that 'nacRing' is Z/p(t_1, t_2, ..., t_m): */
number        nratMap0P(number n);
/* for mapping n into Q(a); this method assumes that n lives in Q,
   and that 'nacRing' is Q(t_1, t_2, ..., t_m): */
number        nratMap00(number n);

#ifdef LDEBUG
  /* for checking whether n is an intact number representing
     an element of K(t_1, t_2, ..., t_m); file is the name of the
     source code file, and line the line of code where the test had
     been invoked: */
  BOOLEAN     nratDBTest(number n, const char *file, const int line);
#endif
/* LDEBUG */

/* ===============================
   external access to the interna:
   =============================== */

/* for mapping the transcendental extension variables 't_i' to other
   such extension variables, or to some ring variable of r, or to zero;
   'mapLength' is the length of 'parMap';
   if 'parMap[i] = 0' then 't_i' is mapped to zero,
   if 'parMap[i] = j' then 't_i' is mapped to 't_j',
   if 'parMap[i] = -j', then 't_i' is mapped to the j-th ring variable
                        of r,
   other values in 'parMap' are not supported: */
poly          nratPermNumber(number n, int* parMap, int mapLength, ring r);
/* for altering the exponent of 't_i' in LT(p) by 'e',
   e.g. 3*t_1^2*t_2+7*t_1-4*t_2+6 becomes 3*t_1^7*t_2+7*t_1-4*t_2+6
   when i = 1 and e = 5; this may modify p */
#define       nratpAddExp(p,i,e)     (p_AddExp(p,i,e,currRing->algring))
/* for retrieving the number of terms in p: */
#define       nratpLength(p)         pLength(p)
/* for negating p; this may modify p: */
#define       nratpNeg(p)            (p_Neg(p,currRing->algring))
/* for retrieving the number of transcendental extension variables: */
#define       nratpVariables         nratNumbOfPar
/* for retrieving the leading coefficient of p: */
#define       nratpGetCoeff(p)       pGetCoeff(p)
/* for retrieving the exponent of t_i in the leading exponent of p: */
#define       nratpGetExpFrom(p,i,r) (p_GetExp(p,i,r->algring))
/* for setting the exponent of t_i in the leading exponent of p;
   careful, no checks will be performed; call nratpSetm(p) afterwards: */
#define       nratpSetExp(p,i,e)     (p_SetExp(p,i,e,currRing->algring))
/* for obtaining a new (but uninitialised) instance of poly */
#define       nratpNew()             (p_Init(currRing->algring))
/* for computing the sum of p1 and p2; this will destroy p1 and p2: */
#define       nratpAdd(p1,p2)        (p_Add_q(p1,p2,currRing->algring))
/* to be called after nratpSetExp(p,e): */
#define       nratpSetm(p)           p_Setm(p,currRing->algring)
/* for computing the remainder when dividing f by g in
   K[t_1, t_2, ..., t_m]; this method will destroy f: */
napoly        nratpRemainder(poly f, const poly g);
/* for writing a printable version of p into the global string buffer;
   the second argument is a hint whether there's a denominator coming
   next (if so, the string for p will be in paranthesis unless the
   denominator is 1): */
//void        nratpWrite(poly p, const BOOLEAN denom = TRUE);
/* for retrieving the numerator of n: */
number        nratGetNumerator(number &n, const ring r);
/* for retrieving the denominator of n: */
number        nratGetDenom(number &n, const ring r);

#endif
/* RAT_FUNCT_FIELD_H */

