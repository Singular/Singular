#ifndef RINTEGERS_H
#define RINTEGERS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: rintegers.h,v 1.1 2007-06-19 10:47:30 wienand Exp $ */
/*
* ABSTRACT: numbers modulo n
*/
#ifdef HAVE_RINGZ
#include "structs.h"

extern int nrzExp;
extern NATNUMBER nrzModul;

BOOLEAN nrzGreaterZero (number k);
number  nrzMult        (number a, number b);
number  nrzInit        (int i);
int     nrzInt         (number &n);
number  nrzAdd         (number a, number b);
number  nrzSub         (number a, number b);
void    nrzPower       (number a, int i, number * result);
BOOLEAN nrzIsZero      (number a);
BOOLEAN nrzIsOne       (number a);
BOOLEAN nrzIsMOne      (number a);
number  nrzDiv         (number a, number b);
number  nrzIntDiv      (number a,number b);
number  nrzNeg         (number c);
number  nrzInvers      (number c);
BOOLEAN nrzGreater     (number a, number b);
BOOLEAN nrzDivBy       (number a, number b);
int     nrzComp        (number a, number b);
BOOLEAN nrzEqual       (number a, number b);
number  nrzLcm         (number a,number b, ring r);
number  nrzGcd         (number a,number b,ring r);
nMapFunc nrzSetMap     (ring src, ring dst);
void    nrzWrite       (number &a);
char *  nrzRead        (char *s, number *a);
char *  nrzName        (number n);
#ifdef LDEBUG
BOOLEAN nrzDBTest      (number a, char *f, int l);
#endif
void    nrzSetExp(int c, ring r);
void    nrzInitExp(int c, ring r);

#endif
#endif
