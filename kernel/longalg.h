#ifndef LONGALG_H
#define LONGALG_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longalg.h,v 1.4 2005-02-24 17:45:46 Singular Exp $ */
/*
* ABSTRACT:   algebraic numbers
*/
#include "structs.h"
#include "longrat.h"
#include "polys-impl.h"

struct slnumber;
typedef struct slnumber * lnumber;

struct slnumber
{
  napoly z;
  napoly n;
  BOOLEAN s;
};

extern int naNumbOfPar;             /* maximal number of parameters */

extern napoly naMinimalPoly;

void naSetChar(int p, ring r);
void    naDelete (number *p, const ring r);
number  naInit(int i);                              /* z := i */
number  naPar(int i);                               /* z := par(i) */
int     naParDeg(number n);                         /* i := deg(n) */
int     naSize(number n);                           /* size desc. */
int     naInt(number &n);
void    naNumberToInt(number n, int *i);
void    naNew(number *z);
BOOLEAN naIsZero(number za);                         /* za = 0 ? */
BOOLEAN naIsOne(number  za);                         /* za = 1 ? */
BOOLEAN naIsMOne(number  za);                        /* za = -1 ? */
BOOLEAN naEqual(number a, number b);                  /* a = b ? */
BOOLEAN naGreater(number a, number b);                  /* dummy */
number  naNeg(number za);                           /* za := - za */
number  naInvers(number a);
BOOLEAN naGreaterZero(number a);
void    naPower(number x, int exp, number *lo);
BOOLEAN naGreaterZero(number a);
number  naCopy(number p);                              /* erg:= p */
number  na_Copy(number p, const ring r);               /* erg:= p */
number  naAdd(number la, number li);               /* lu := la+li */
number  naMult(number la, number li);              /* lo := la*li */
number  naDiv(number la, number li);               /* lo := la/li */
number  naIntDiv(number la, number li);            /* lo := la/li */
//number  naIntMod(number la, number li);            /* lo := la/li */
number  naSub(number la, number li);               /* lu := la-li */
void    naNormalize(number &p);
number  naGcd(number a, number b, const ring r);
number  naLcm(number a, number b, const ring r);
char *  naRead(char * s, number * p);
void    naWrite(number &p);
char *  naName(number n);
nMapFunc naSetMap(ring src, ring dst);
#ifdef LDEBUG
BOOLEAN naDBTest(number a, char *f,int l);
#endif

void    naSetIdeal(ideal I);

// external access to the interna
poly naPermNumber(number z, int * par_perm, int P, ring r);
#define napAddExp(p,i,e)       (p_AddExp(p,i,e,currRing->algring))
#define napLength(p)           pLength(p)
#define napNeg(p)              (p_Neg(p,currRing->algring))
#define napVariables           naNumbOfPar
#define napNext(p)             pNext(p)
#define napIter(p)             pIter(p)
#define napGetCoeff(p)         pGetCoeff(p)
#define napGetExp(p,i)         (p_GetExp(p,i,currRing->algring))
#define napGetExpFrom(p,i,r)   (p_GetExp(p,i,r->algring))
#define napSetExp(p,i,e)       (p_SetExp(p,i,e,currRing->algring))
#define napDelete(p)           p_Delete(p, currRing->algring)
#define nap_Delete(p,r)        p_Delete(p, (r)->algring)
#define napNew()               (p_Init(currRing->algring))
#define napAdd(p1,p2)          (p_Add_q(p1,p2,currRing->algring))
#define napSetm(p)             p_Setm(p,currRing->algring)
#define nanumber               lnumber
#define naGetNom0(na)          (((nanumber)(na))->z)
#define naGetDenom0(na)        (((nanumber)(na))->n)
napoly napRemainder(napoly f, const napoly  g);
extern number   (*nacCopy)(number a);
extern BOOLEAN  (*nacIsZero)(number a);
extern number   (*nacInit)(int i);
extern void     (*nacNormalize)(number &a);
extern void napWrite(napoly l);
extern number   naGetDenom(number &n, const ring r);
#endif

