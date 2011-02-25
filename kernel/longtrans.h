#ifndef LONGTRANS_H
#define LONGTRANS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longtrans.cc 12469 2011-02-25 13:38:49Z seelisch $ */
/*
* ABSTRACT:   numbers in transcendental field extensions, i.e.,
              in rational function fields
*/
#include <kernel/structs.h>
#include <kernel/longrat.h>
#include <kernel/polys-impl.h>
#include <kernel/longalg.h>

extern ring ntcRing;


void ntSetChar(int p, ring r);
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

void    ntSetIdeal(ideal I);

#endif

