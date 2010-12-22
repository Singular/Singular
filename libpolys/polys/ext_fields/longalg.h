#ifndef LONGALG_H
#define LONGALG_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longalg.h 12749 2010-04-27 14:17:56Z hannes $ */
/*
* ABSTRACT:   algebraic numbers
*/
#include <coeffs/coeffs.h>
#include <coeffs/longrat.h>
#include <poly/monomials/polys-impl.h>
#include <polys/ext_fields/longtrans.h>

/* 
   IMPORTANT INFORMATION:
   Instantiation of an algebraic field extension in SINGULAR 
   works by first creating a transcendental field extension
   and then providing a minimal polynomial / minimal ideal.
   Consequently, first the code for transcendental field
   extensions will be activated; see longtrans.*.
   When providing a minimal polynomial / minimal ideal, all
   function pointers will be re-defined, using the below method
   redefineFunctionPointers(). After that, the code for algebraic
   field extensions is active; see longalg.*.
   
   NOTE:
   Elements of algebraic and transcendental field extensions
   are polynomials or quotients of two polynomials, respectively.
   All these polynomials, as well as the minimal polynomial (in
   the case of an algebraic extension) live in the globally
   accessible ring 'nacRing', defined in longtrans.*.
   
   METHOD NAMING CONVENTIONS
   (not true for types / structs / global variables):
   nap*   macros and methods operating on polynomials living in
          nacRing (defined in longtrans.*),
   na*    (but not nap*) methods and macros for algebraic field
          extensions (defined in longalg.*),
   nt*    methods and macros for transcendental field extensions,
          (defined in longtrans.*)
   nac*   function pointers for computing with the coefficients of
          polynomials living in nacRing (defined in longtrans.*)
*/

struct snaIdeal
{
  int anz;
  napoly *liste;
};
typedef struct snaIdeal * naIdeal;
extern omBin snaIdeal_bin;
extern naIdeal naI;
extern napoly naMinimalPoly;

/* for re-defining function pointers when switching from
   transcendental to algebraic extension (by providing
   a minpoly) */
void          redefineFunctionPointers();

/* specific methods / macros for algebraic field extensions */
void          naSetChar(int p, ring r);
void          naDelete (number *p, const ring r);
number        naInit(int i, const ring r);                /* z := i      */
number        naPar(int i);                               /* z := par(i) */
int           naParDeg(number n);                         /* i := deg(n) */
int           naSize(number n);                           /* size desc.  */
int           naInt(number &n, const ring r);
BOOLEAN       naIsZero(number za);                        /* za = 0 ?    */
BOOLEAN       naIsOne(number  za);                        /* za = 1 ?    */
BOOLEAN       naIsMOne(number  za);                       /* za = -1 ?   */
BOOLEAN       naEqual(number a, number b);                /* a = b ?     */
BOOLEAN       naGreater(number a, number b);              /* dummy       */
number        naNeg(number za);                           /* za := - za  */
number        naInvers(number a);
void          naPower(number x, int exp, number *lo);
BOOLEAN       naGreaterZero(number a);
number        naCopy(number p);                           /* erg:= p     */
number        na_Copy(number p, const ring r);            /* erg:= p     */
number        naAdd(number la, number li);                /* lu := la+li */
number        naMult(number la, number li);               /* lo := la*li */
number        naDiv(number la, number li);                /* lo := la/li */
number        naIntDiv(number la, number li);             /* lo := la/li */
//number        naIntMod(number la, number li);           /* lo := la/li */
number        naSub(number la, number li);                /* lu := la-li */
void          naNormalize(number &p);
number        naGcd(number a, number b, const ring r);
number        naLcm(number a, number b, const ring r);
const char *  naRead(const char * s, number * p);
void          naWrite(number &p, const ring r);
char *        naName(number n);
nMapFunc      naSetMap(const ring src, const ring dst);
number        naMap0P(number c);
number        naMap00(number c);
#ifdef LDEBUG
BOOLEAN       naDBTest(number a, const char *f,const int l);
#endif
void          naSetIdeal(ideal I);
void          naCoefNormalize(number pp);
extern number (*naMap)(number from);

//extern omBin lnumber_bin;
//#define ALLOC_LNUMBER() (lnumber)omAllocBin(lnumber_bin)
//#define ALLOC0_LNUMBER() (lnumber)omAlloc0Bin(lnumber_bin)
//#define FREE_LNUMBER(x) omFreeBin((ADDRESS)x, lnumber_bin)
extern omBin rnumber_bin;
#define ALLOC_LNUMBER() (lnumber)omAllocBin(rnumber_bin)
#define ALLOC0_LNUMBER() (lnumber)omAlloc0Bin(rnumber_bin)
#define FREE_LNUMBER(x) omFreeBin((ADDRESS)x, rnumber_bin)
#endif

