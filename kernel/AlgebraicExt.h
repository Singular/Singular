#ifndef ALGEBRAIC_EXT_H
#define ALGEBRAIC_EXT_H

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: AlgebraicExt.h 12469 2010-05-14 10:39:49Z frank $ */
/*
* ABSTRACT: elements of an algebraic extension field
*
* This field has the form K(a), where K is itself a field,
* and a is algebraic over K. The elements in K(a) can be
* treated as a specific type of number, see numbers.h.
* This implementation declares a global variable 'nacRing'
* which is a valid SINGULAR ring representing K[a]. Like-
* wise, the minimal polynomial of a over K is stored in
* the global variable 'naMinimalPoly'.
*/
#include "structs.h"
#include "longrat.h"
#include "polys-impl.h"

/* The following is identical to 'poly'. We just use
   'napoly' as an alias to make clear throughtout the
   kernel implementation that this is actually a poly-
   nomial in an algebraic extension field K(a). */
typedef poly  napoly;

/* the minimal polynomial for a over K; this lives in some
   polynomial ring over K(a), i.e., in some K(a)[x_1, x_2, ..., x_n].
   (Note that we cannot let it live inside K(a), because
   there it would be reduced to '0', by definition.) */
extern napoly naMinimalPoly;

/* the SINGULAR-internal ring representing K[a]; herein lives
   the number, i.e., the represented element of K(a) */
extern ring   nacRing;

/* ring initialisation and setting of function pointers: */
void          naSetChar(int p, ring r);
/* for deleting the represented element of K(a): */
void          naDelete (number *p, const ring r);
/* for mapping the integer i into K(a): */
number        naInit(int i, const ring r);
/* i-th extension variable; here i = 1 and result = 'a': */
number        naPar(int i);
/* degree of n, seen as the degree of the polynomial n in K[a]: */
int           naParDeg(number n);
/* measure of complexity for n; takes coeffs and degree into account: */
int           naSize(number n);
/* for (sensibly) mapping n into the integers, Z: */
int           naInt(number &n, const ring r);
/* for checking whether n is 0: */
BOOLEAN       naIsZero(number n);
/* for checking whether n is 1: */
BOOLEAN       naIsOne(number  n);
/* for checking whether n is -1: */
BOOLEAN       naIsMOne(number  n);
/* for determining whether n1 = n2: */
BOOLEAN       naEqual(number n1, number n2);
/* for determining whether n1 > n2; so far, this is true iff n1 != n2: */
BOOLEAN       naGreater(number n1, number n2);
/* for computing -n; use according to the pattern 'n := - n;' only: */
number        naNeg(number n);
/* for computing 1/n as an element of K[a]: */
number        naInvers(number n);
/* for computing n^exp inside K(a); result will be stored in 'result': */
void          naPower(number n, int exp, number* result);
/* for determining whether n > 0; so far, this is true iff n != 0: */
BOOLEAN       naGreaterZero(number n);
/* for producing a copy of n: */
number        naCopy(number n);
/* for computing n1 + n2: */
number        naAdd(number n1, number n2);
/* for computing n1 * n2: */
number        naMult(number n1, number n2);
/* for computing n1 / n2: */
number        naDiv(number n1, number n2);
/* same as naDiv; declared just for interface reasons: */
number        naIntDiv(number n1, number n2);
/* next is always zero in K(a); declared just for interface reasons;
   error when n2 = 0: */
//number      naIntMod(number n1, number n2);
/* for computing n1 - n2: */
number        naSub(number la, number li);
/* for simplifying coefficients (elements of K): */
void          naNormalize(number &p);
/* for computing gcd(n1, n2): */
number        naGcd(number n1, number n2, const ring r);
/* for computing lcm(n1, n2): */
number        naLcm(number n1, number n2, const ring r);
/* next method tries to read a number (which represents an element
   of K(a)) from s; in case of success, the number will sit inside
   n, and the returned pointer points to the remainder of s (starting
   after the successfully parsed substring) : */
const char*   naRead(const char* s, number* n);
/* for writing a printable version of n into the global string buffer: */
void          naWrite(number &n, const ring r);
/* for retrieving a printable version of the leading term of n: */
char*         naName(number n);
/* for obtaining a function pointer that may be used to map an element
   of K(a) to the destination ring */
nMapFunc      naSetMap(const ring src, const ring dst);
/* for mapping n into Z/p(a); this method assumes that n lives in Q,
   and that 'nacRing' is Z/p(a): */
number        naMap0P(number n);
/* for mapping n into Q(a); this method assumes that n lives in Q,
   and that 'nacRing' is Q(a): */
number        naMap00(number n);

#ifdef LDEBUG
  /* for checking whether n is an intact number representing
     an element of K(a); file is the name of the source code file,
     and line the line of code where the test had been invoked: */
  BOOLEAN     naDBTest(number n, const char *file, const int line);
#endif
/* LDEBUG */

/* ===============================
   external access to the interna:
   =============================== */

/* for mapping the algebraic extension variable 'a' to some
   variable, or to zero; 'mapLength' is the length of 'parMap' and
   assumed to be 1;
   if 'parMap[0] = 0' then 'a' is mapped to zero,
   if 'parMap[0] = 1' then 'a' is mapped to 'a',
   if 'parMap[0] = -i', then 'a' is mapped to the i-th ring variable
                        of r,
   other values in 'parMap' are not supported: */
poly          naPermNumber(number n, int* parMap, int mapLength, ring r);
/* for altering the exponent of 'a' in LT(p) by 'e',
   e.g. 3*a^2+7a-4 becomes 3*a^7+7a-4 when e = 5;
   this may modify p */
#define       napAddExp(p,e)         (p_AddExp(p,1,e,currRing->algring))
/* for retrieving the number of terms in p: */
#define       napLength(p)           pLength(p)
/* for negating p; this may modify p: */
#define       napNeg(p)              (p_Neg(p,currRing->algring))
/* for retrieving the number of algebraic extension variables;
   so far this can only be 1: */
#define       napVariables           naNumbOfPar
/* for retrieving the leading coefficient of p: */
#define       napGetCoeff(p)         pGetCoeff(p)
/* for retrieving the leading exponent of p: */
#define       napGetExpFrom(p,r)     (p_GetExp(p,1,r->algring))
/* for setting the leading exponent of p; careful, no checks will be
   performed; call napSetm(p) afterwards: */
#define       napSetExp(p,e)         (p_SetExp(p,1,e,currRing->algring))
/* for obtaining a new (but uninitialised) instance of napoly */
#define       napNew()               (p_Init(currRing->algring))
/* for computing the sum of p1 and p2; this will destroy p1 and p2: */
#define       napAdd(p1,p2)          (p_Add_q(p1,p2,currRing->algring))
/* to be called after napSetExp(p,e): */
#define       napSetm(p)             p_Setm(p,currRing->algring)
/* for computing the remainder when dividing f by g in K[a];
   this method will destroy f: */
napoly        napRemainder(napoly f, const napoly g);
/* for writing a printable version of p into the global string
   buffer: */
//void        napWrite(napoly p);
/* for retrieving the numerator of n, in K(a) this is n itself: */
number        naGetNumerator(number &n, const ring r);
/* for retrieving the denominator of n; in K(a) always 1: */
number        naGetDenom(number &n, const ring r);

#endif
/* ALGEBRAIC_EXT_H */

