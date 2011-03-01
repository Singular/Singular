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
#include <kernel/structs.h>
#include <kernel/longrat.h>
#include <kernel/polys-impl.h>

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

typedef polyrec * napoly;
struct slnumber;
typedef struct slnumber * lnumber;
struct slnumber
{
  napoly z;
  napoly n;
  BOOLEAN s;
};

extern int  ntNumbOfPar;
#define     ntParNames (currRing->parameter)
extern int  ntIsChar0;
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
napoly      ntRemainder(napoly f, const napoly  g);
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
extern void        (*nacDelete)(number *a, const ring r);
extern void        (*nacNormalize)(number &a);
extern number      (*nacNeg)(number a);
extern number      (*nacCopy)(number a);
extern number      (*nacInvers)(number a);
extern BOOLEAN     (*nacIsZero)(number a);
extern BOOLEAN     (*nacIsOne)(number a);
extern BOOLEAN     (*nacIsMOne)(number a);
extern BOOLEAN     (*nacGreaterZero)(number a);
extern const char* (*nacRead) (const char *s, number *a);
extern number      (*nacMap)(number);

/* for computing with polynomials living in nacRing */
poly    napPermNumber(number z, int * par_perm, int P, ring r);
#define napAddExp(p,i,e)       (p_AddExp(p,i,e,currRing->algring))
#define napLength(p)           pLength(p)
#define napNeg(p)              (p_Neg(p,currRing->algring))
#define napVariables           naNumbOfPar
#define napGetCoeff(p)         pGetCoeff(p)
#define napGetExpFrom(p,i,r)   (p_GetExp(p,i,r->algring))
#define napSetExp(p,i,e)       (p_SetExp(p,i,e,currRing->algring))
#define napNew()               (p_Init(currRing->algring))
#define napAdd(p1,p2)          (p_Add_q(p1,p2,currRing->algring))
#define napSetm(p)             p_Setm(p,currRing->algring)
#define napCopy(p)             p_Copy(p,nacRing)
#define napSetCoeff(p,n)       {n_Delete(&pGetCoeff(p),nacRing);pGetCoeff(p)=n;}
#define napComp(p,q)           p_LmCmp((poly)p,(poly)q, nacRing)
#define napMultT(A,E)          A=(napoly)p_Mult_mm((poly)A,(poly)E,nacRing)
#define napDeg(p)              (int)p_Totaldegree(p, nacRing)
number  napGetDenom(number &n, const ring r);
number  napGetNumerator(number &n, const ring r);
void    napTest(napoly p);
napoly  napInitz(number z);
napoly  napCopyNeg(napoly p);
void    napMultN(napoly p, number z);
void    napDivMod(napoly f, napoly  g, napoly *q, napoly *r);
napoly  napInvers(napoly x, const napoly c);
int     napMaxDeg(napoly p);
int     napMaxDegLen(napoly p, int &l);
void    napWrite(napoly p,const BOOLEAN has_denom, const ring r);
int     napExp(napoly a, napoly b);
int     napExpi(int i, napoly a, napoly b);
void    napContent(napoly ph);
void    napCleardenom(napoly ph);
napoly  napGcd0(napoly a, napoly b);
napoly  napGcd(napoly a, napoly b);
number  napLcm(napoly a);
BOOLEAN napDivPoly (napoly p, napoly q);
napoly  napRedp (napoly q);
napoly  napTailred (napoly q);
napoly  napMap(napoly p);
napoly  napPerm(napoly p, const int *par_perm, const ring src_ring,
                const nMapFunc nMap);
const char *napHandleMons(const char *s, int i, napoly ex);
const char *napHandlePars(const char *s, int i, napoly ex);
const char *napRead(const char *s, napoly *b);

#endif

