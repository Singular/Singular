#ifndef LONGTRANS_H
#define LONGTRANS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longtrans.h 12469 2011-02-25 13:38:49Z seelisch $ */
/*
* ABSTRACT:   numbers in transcendental field extensions,
              i.e., in rational function fields
*/
#include <coeffs/longrat.h>
#include <polys/monomials/polys-impl.h>

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

struct slnumber;
typedef struct slnumber * lnumber;
struct slnumber
{
  poly z;
  poly n;
  BOOLEAN s;
};

extern int  ntNumbOfPar;
#define     ntParNames (currRing->parameter)
extern int  ntIsChar0;                                  /* == 1 iff char = 0,
                                                           == 0 otherwise */ 
extern ring ntMapRing;
extern int  ntParsToCopy;

/* specific methods / macros for transcendental field extensions */
void        ntSetChar(int p, ring r);
void        ntDelete (number *p, const ring r);
number      ntInit(int i, const ring r);                /* z := i      */
number      ntPar(int i);                               /* z := par(i) */
int         ntParDeg(number n);                         /* i := deg(n) */
int         ntSize(number n);                           /* size desc.  */
int         ntInt(number &n, const ring r);
BOOLEAN     ntIsZero(number za);                        /* za = 0 ?    */
BOOLEAN     ntIsOne(number  za);                        /* za = 1 ?    */
BOOLEAN     ntIsMOne(number  za);                       /* za = -1 ?   */
BOOLEAN     ntEqual(number a, number b);                /* a = b ?     */
BOOLEAN     ntGreater(number a, number b);              /* dummy       */
number      ntNeg(number za);                           /* za := - za  */
number      ntInvers(number a);
void        ntPower(number x, int exp, number *lo);
BOOLEAN     ntGreaterZero(number a);
number      ntCopy(number p);                           /* erg:= p     */
number      nt_Copy(number p, const ring r);            /* erg:= p     */
number      ntAdd(number la, number li);                /* lu := la+li */
number      ntMult(number la, number li);               /* lo := la*li */
number      ntDiv(number la, number li);                /* lo := la/li */
number      ntIntDiv(number la, number li);             /* lo := la/li */
//number      ntIntMod(number la, number li);           /* lo := la/li */
number      ntSub(number la, number li);                /* lu := la-li */
void        ntNormalize(number &p);
number      ntGcd(number a, number b, const ring r);
number      ntLcm(number a, number b, const ring r);
const char* ntRead(const char * s, number * p);
void        ntWrite(number &p, const ring r);
char*       ntName(number n);
nMapFunc    ntSetMap(const ring src, const ring dst);
number      ntMap0P(number c);
number      ntMap00(number c);
#ifdef LDEBUG
BOOLEAN     ntDBTest(number a, const char *f,const int l);
#endif
poly      ntRemainder(poly f, const poly  g);
void        ntSetIdeal(ideal I);
void        ntCoefNormalize(number pp);
extern number (*ntMap)(number from);

/* procedure variables for computing with the coefficients of
   polynomials living in nacRing */
extern ring        nacRing;
extern numberfunc  nacMult, nacSub, nacAdd, nacDiv, nacIntDiv;
extern number      (*nacGcd)(number a, number b, const ring r);
extern number      (*nacLcm)(number a, number b, const ring r);
extern number      (*nacInit)(int i, const ring r);
extern int         (*nacInt)(number &n, const ring r);
extern void        (*nacNormalize)(number &a);
extern number      (*nacNeg)(number a);
extern number      (*nacCopy)(number a);
extern number      (*nacInvers)(number a);
extern BOOLEAN     (*nacIsZero)(number a);
extern BOOLEAN     (*nacIsOne)(number a);
extern BOOLEAN     (*nacGreaterZero)(number a);
extern BOOLEAN     (*nacGreater)(number a, number b);
extern number      (*nacMap)(number);

/* for computing with polynomials living in nacRing */
poly    napPermNumber(number z, int * par_perm, int P, ring r);
#define napAddExp(p,i,e)       (p_AddExp(p,i,e,currRing->extRing))
#define napLength(p)           pLength(p)
#define napNeg(p)              (p_Neg(p,currRing->extRing))
#define napVariables           naNumbOfPar
#define napGetCoeff(p)         pGetCoeff(p)
#define napGetExpFrom(p,i,r)   (p_GetExp(p,i,r->extRing))
#define napSetExp(p,i,e)       (p_SetExp(p,i,e,currRing->extRing))
#define napNew()               (p_Init(currRing->extRing))
#define napAdd(p1,p2)          (p_Add_q(p1,p2,currRing->extRing))
#define napSetm(p)             p_Setm(p,currRing->extRing)
#define napCopy(p)             p_Copy(p,nacRing)
#define napSetCoeff(p,n)       {n_Delete(&pGetCoeff(p),nacRing);pGetCoeff(p)=n;}
#define napComp(p,q)           p_LmCmp((poly)p,(poly)q, nacRing)
#define napMultT(A,E)          A=(poly)p_Mult_mm((poly)A,(poly)E,nacRing)
#define napDeg(p)              (int)p_Totaldegree(p, nacRing)
number  napGetDenom(number &n, const ring r);
number  napGetNumerator(number &n, const ring r);
void    napTest(poly p);
poly  napInitz(number z);
poly  napCopyNeg(const poly p);
void    napMultN(poly p, const number z);
void    napDivMod(poly f, const poly g, poly *q, poly *r);
poly  napRemainder(poly f, const poly g);
poly  napInvers(poly x, const poly c);
int     napMaxDeg(poly p);
int     napMaxDegLen(poly p, int &l);
void    napWrite(poly p,const BOOLEAN has_denom, const ring r);
int     napExp(poly a, poly b);
int     napExpi(int i, poly a, poly b);
void    napContent(poly ph);
void    napCleardenom(poly ph);
poly  napGcd0(poly a, poly b);
poly  napGcd(poly a, poly b);
number  napLcm(poly a);
BOOLEAN napDivPoly (poly p, poly q);
poly  napRedp (poly q);
poly  napTailred (poly q);
poly  napMap(poly p);
poly  napPerm(poly p, const int *par_perm, const ring src_ring,
                const nMapFunc nMap);
const char* napHandleMons(const char *s, int i, poly ex);
const char* napHandlePars(const char *s, int i, poly ex);
const char* napRead(const char *s, poly *b);

#endif

