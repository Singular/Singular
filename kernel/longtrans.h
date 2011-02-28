#ifndef LONGTRANS_H
#define LONGTRANS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longtrans.h 12469 2011-02-25 13:38:49Z seelisch $ */
/*
* ABSTRACT:   numbers in transcendental field extensions, i.e.,
              in rational function fields
*/
#include <kernel/structs.h>
#include <kernel/longrat.h>
#include <kernel/polys-impl.h>

typedef polyrec * napoly;

struct slnumber;
typedef struct slnumber * lnumber;

struct slnumber
{
  napoly z;
  napoly n;
  BOOLEAN s;
};

extern int ntNumbOfPar;
#define naParNames (currRing->parameter)
extern ring nacRing;
extern int ntIsChar0;
extern ring ntMapRing;
extern int ntParsToCopy;

void    ntSetChar(int p, ring r);
void    ntDelete (number *p, const ring r);
number  ntInit(int i, const ring r);                /* z := i */
number  ntPar(int i);                               /* z := par(i) */
int     ntParDeg(number n);                         /* i := deg(n) */
int     ntSize(number n);                           /* size desc. */
int     ntInt(number &n, const ring r);
BOOLEAN ntIsZero(number za);                         /* za = 0 ? */
BOOLEAN ntIsOne(number  za);                         /* za = 1 ? */
BOOLEAN ntIsMOne(number  za);                        /* za = -1 ? */
BOOLEAN ntEqual(number a, number b);                  /* a = b ? */
BOOLEAN ntGreater(number a, number b);                  /* dummy */
number  ntNeg(number za);                           /* za := - za */
number  ntInvers(number a);
void    ntPower(number x, int exp, number *lo);
BOOLEAN ntGreaterZero(number a);
number  ntCopy(number p);                              /* erg:= p */
number  nt_Copy(number p, const ring r);               /* erg:= p */
number  ntAdd(number la, number li);               /* lu := la+li */
number  ntMult(number la, number li);              /* lo := la*li */
number  ntDiv(number la, number li);               /* lo := la/li */
number  ntIntDiv(number la, number li);            /* lo := la/li */
//number  ntIntMod(number la, number li);            /* lo := la/li */
number  ntSub(number la, number li);               /* lu := la-li */
void    ntNormalize(number &p);
number  ntGcd(number a, number b, const ring r);
number  ntLcm(number a, number b, const ring r);
const char *  ntRead(const char * s, number * p);
void    ntWrite(number &p, const ring r);
char *  ntName(number n);
nMapFunc ntSetMap(const ring src, const ring dst);
number ntMap0P(number c);
number ntMap00(number c);
#ifdef LDEBUG
BOOLEAN ntDBTest(number a, const char *f,const int l);
#endif
napoly ntRemainder(napoly f, const napoly  g);
void    ntSetIdeal(ideal I);
extern number (*ntMap)(number from);
void ntCoefNormalize(number pp);

/* procedure variables for operations in coefficient field/ring */
extern numberfunc ntcMult, ntcSub, ntcAdd, ntcDiv, ntcIntDiv;
extern number   (*ntcGcd)(number a, number b, const ring r);
extern number   (*ntcLcm)(number a, number b, const ring r);
extern number   (*ntcInit)(int i, const ring r);
extern int      (*ntcInt)(number &n, const ring r);
extern void     (*ntcDelete)(number *a, const ring r);
#undef n_Delete
#define n_Delete(A,R) ntcDelete(A,R)
extern void     (*ntcNormalize)(number &a);
extern number   (*ntcNeg)(number a);
extern number   (*ntcCopy)(number a);
extern number   (*ntcInvers)(number a);
extern BOOLEAN  (*ntcIsZero)(number a);
extern BOOLEAN  (*ntcIsOne)(number a);
extern BOOLEAN  (*ntcIsMOne)(number a);
extern BOOLEAN  (*ntcGreaterZero)(number a);
extern const char   * (*ntcRead) (const char *s, number *a);
extern number (*ntcMap)(number);

// external access to the interna
poly napPermNumber(number z, int * par_perm, int P, ring r);
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
number napGetDenom(number &n, const ring r);
number napGetNumerator(number &n, const ring r);
void napTest(napoly p);
napoly napInitz(number z);
napoly napCopyNeg(napoly p);
void napMultN(napoly p, number z);
void napDivMod(napoly f, napoly  g, napoly *q, napoly *r);
napoly napInvers(napoly x, const napoly c);
int  napMaxDeg(napoly p);
int  napMaxDegLen(napoly p, int &l);
void napWrite(napoly p,const BOOLEAN has_denom, const ring r);
const char *napHandleMons(const char *s, int i, napoly ex);
const char *napHandlePars(const char *s, int i, napoly ex);
const char  *napRead(const char *s, napoly *b);
int napExp(napoly a, napoly b);
int napExpi(int i, napoly a, napoly b);
void napContent(napoly ph);
void napCleardenom(napoly ph);
napoly napGcd0(napoly a, napoly b);
napoly napGcd(napoly a, napoly b);
number napLcm(napoly a);
BOOLEAN napDivPoly (napoly p, napoly q);
napoly napRedp (napoly q);
napoly napTailred (napoly q);
napoly napMap(napoly p);
napoly napPerm(napoly p,const int *par_perm,const ring src_ring,const nMapFunc nMap);

#endif

