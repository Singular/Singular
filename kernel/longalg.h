#ifndef LONGALG_H
#define LONGALG_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longalg.h,v 1.1.1.1 2003-10-06 12:15:57 Singular Exp $ */
/*
* ABSTRACT:   algebraic numbers
*/
#include "structs.h"
#include "longrat.h"
#include "polys-impl.h"

struct slnumber;
typedef struct slnumber * lnumber;

#ifndef LONGALGNEW
//make parameter type same as exponent type
#define PARAMETER_TYPE short
#define SIZEOF_PARAMETER SIZEOF_SHORT

struct reca
{
  napoly ne;
  number ko;
  PARAMETER_TYPE e[1];
};
#endif /* not LONGALGNEW */

struct slnumber
{
  napoly z;
  napoly n;
  BOOLEAN s;
};

extern int naNumbOfPar;             /* maximal number of parameters */
extern napoly naMinimalPoly;
#ifndef LONGALGNEW
extern char **naParNames;
extern int napMonomSize;
#endif /* LONGALGNEW */

void naSetChar(int p, ring r);
#ifndef LONGALGNEW
#define napAddExp(P,I,E)  ((P)->e[I-1]+=(E))
#define napLength(p)      pLength((poly)p)
napoly napNeg(napoly a);
#endif /* LONGALGNEW */
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
#ifndef LONGALGNEW
#define RECA_SIZE (sizeof(napoly)+sizeof(number))
napoly napAdd(napoly p1, napoly p2);
void napDelete(napoly *p);
void nap_Delete(napoly *p, ring r);
#endif /* not LONGALGNEW */
poly naPermNumber(number z, int * par_perm, int P, ring r);
#ifndef LONGALGNEW
#define napVariables naNumbOfPar
#define napNext(p) (p->ne)
#define napIter(p) ((p) = (p)->ne)
#define napGetCoeff(p) (p->ko)
#define napGetExp(p,i) ((p)->e[(i)-1])
#define napGetExpFrom(p,i,r) ((p)->e[(i)-1])
#define napSetExp(p,i,ee) ((p)->e[(i)-1]=ee)
#define napNew() ((napoly)omAlloc0(napMonomSize))
#define nanumber lnumber
#define naGetNom0(na)  (((nanumber)(na))->z)
#define naGetDenom0(na)  (((nanumber)(na))->n)
#else /* LONGALGNEW */
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
#define nanumber               lnumber
#define naGetNom0(na)          (((nanumber)(na))->z)
#define naGetDenom0(na)        (((nanumber)(na))->n)
#endif /* LONGALGNEW */
extern number   (*nacCopy)(number a);
extern BOOLEAN  (*nacIsZero)(number a);
extern number   (*nacInit)(int i);
extern void     (*nacNormalize)(number &a);
extern void napWrite(napoly l);
extern number   naGetDenom(number &n, const ring r);
#endif

