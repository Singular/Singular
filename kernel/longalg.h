#ifndef LONGALG_H
#define LONGALG_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longalg.h 12749 2010-04-27 14:17:56Z hannes $ */
/*
* ABSTRACT:   algebraic numbers
*/
#include <kernel/structs.h>
#include <kernel/longrat.h>
#include <kernel/polys-impl.h>

typedef polyrec *   napoly;

struct slnumber;
typedef struct slnumber * lnumber;

struct slnumber
{
  napoly z;
  napoly n;
  BOOLEAN s;
};

extern napoly naMinimalPoly;
extern ring nacRing;


void naSetChar(int p, ring r);
void    naDelete (number *p, const ring r);
number  naInit(int i, const ring r);                /* z := i */
number  naPar(int i);                               /* z := par(i) */
int     naParDeg(number n);                         /* i := deg(n) */
int     naSize(number n);                           /* size desc. */
int     naInt(number &n, const ring r);
BOOLEAN naIsZero(number za);                         /* za = 0 ? */
BOOLEAN naIsOne(number  za);                         /* za = 1 ? */
BOOLEAN naIsMOne(number  za);                        /* za = -1 ? */
BOOLEAN naEqual(number a, number b);                  /* a = b ? */
BOOLEAN naGreater(number a, number b);                  /* dummy */
number  naNeg(number za);                           /* za := - za */
number  naInvers(number a);
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
const char *  naRead(const char * s, number * p);
void    naWrite(number &p, const ring r);
char *  naName(number n);
nMapFunc naSetMap(const ring src, const ring dst);
number naMap0P(number c);
number naMap00(number c);
#ifdef LDEBUG
BOOLEAN naDBTest(number a, const char *f,const int l);
#endif

void    naSetIdeal(ideal I);

// external access to the interna
poly naPermNumber(number z, int * par_perm, int P, ring r);
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
#define nanumber               lnumber
napoly napRemainder(napoly f, const napoly  g);
number   naGetDenom(number &n, const ring r);
number   naGetNumerator(number &n, const ring r);
#endif

