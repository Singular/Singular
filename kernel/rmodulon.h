#ifndef RMODULON_H
#define RMODULON_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: rmodulon.h,v 1.2 2007-05-10 08:12:44 wienand Exp $ */
/*
* ABSTRACT: numbers modulo n
*/
#ifdef HAVE_RINGMODN
#include "structs.h"

#ifndef NATNUMBER
#define NATNUMBER unsigned long
#endif

extern int nrnExp;
extern NATNUMBER nrnModul;

BOOLEAN nrnGreaterZero (number k);
number  nrnMult        (number a, number b);
number  nrnInit        (int i);
int     nrnInt         (number &n);
number  nrnAdd         (number a, number b);
number  nrnSub         (number a, number b);
void    nrnPower       (number a, int i, number * result);
BOOLEAN nrnIsZero      (number a);
BOOLEAN nrnIsOne       (number a);
BOOLEAN nrnIsMOne      (number a);
number  nrnDiv         (number a, number b);
number   nrnIntDiv      (number a,number b);
number  nrnNeg         (number c);
number  nrnInvers      (number c);
BOOLEAN nrnGreater     (number a, number b);
BOOLEAN nrnDivBy       (number a, number b);
BOOLEAN nrnEqual       (number a, number b);
number   nrnLcm         (number a,number b, ring r);
number   nrnGcd         (number a,number b,ring r);
nMapFunc nrnSetMap(ring src, ring dst);
void    nrnWrite       (number &a);
char *  nrnRead        (char *s, number *a);
char *  nrnName        (number n);
#ifdef LDEBUG
BOOLEAN nrnDBTest      (number a, char *f, int l);
#endif
void    nrnSetExp(int c, ring r);
void    nrnInitExp(int c, ring r);


static inline number nrnMultM(number a, number b)
{
  return (number) 
    ((((NATNUMBER) a)*((NATNUMBER) b)) % ((NATNUMBER) nrnModul));
}

static inline number nrnAddM(number a, number b)
{
  NATNUMBER r = (NATNUMBER)a + (NATNUMBER)b;
  return (number) (r >= nrnModul ? r - nrnModul : r);
}

static inline number nrnSubM(number a, number b)
{
  return (number)((NATNUMBER)a<(NATNUMBER)b ?
                       nrnModul-(NATNUMBER)b+(NATNUMBER)a : (NATNUMBER)a-(NATNUMBER)b);
}

#define nrnNegM(A) (number)(nrnModul-(NATNUMBER)(A))
#define nrnEqualM(A,B)  ((A)==(B))

#endif
#endif
