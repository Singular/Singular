#ifndef NUMBERS_H
#define NUMBERS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: numbers.h,v 1.5 1997-08-12 17:14:42 Singular Exp $ */
/*
* ABSTRACT: interface to coefficient aritmetics
*/
#include "structs.h"

/* typedefs */
/*typedef void (*numberproc)(number a,number b,number * c);*/
/*typedef number (*numberfunc)(number a,number b);*/

/* variables */
extern short fftable[];
/*extern int characteristic;*/

/* prototypes */
extern numberfunc nMult, nSub ,nAdd ,nDiv, nIntDiv, nIntMod, nExactDiv;
extern void    (*nNew)(number * a);
extern number  (*nInit)(int i);
extern number  (*nPar)(int i);
extern int     (*nParDeg)(number n);
extern int     (*nSize)(number n);
extern int     (*nInt)(number &n);
extern number  (*nNeg)(number a);
extern number  (*nInvers)(number a);
extern number  (*nCopy)(number a);
extern void    (*nWrite)(number &a);
extern char *  (*nRead)(char * s, number * a);
extern void    (*nNormalize)(number &a);
extern BOOLEAN (*nGreater)(number a,number b),
               (*nEqual)(number a,number b),
               (*nIsZero)(number a),
               (*nIsOne)(number a),
               (*nIsMOne)(number a),
               (*nGreaterZero)(number a);
extern void    (*nPower)(number a, int i, number * result);
extern numberfunc nGcd, nLcm;

extern number nNULL; /* the 0 as constant */

#ifdef LDEBUG
extern BOOLEAN (*nDBTest)(number a, char *f, int l);
#define nTest(a) nDBTest(a,__FILE__,__LINE__)
extern void    (*nDBDelete)(number * a,char *f, int l);
#define nDelete(A) nDBDelete(A,__FILE__,__LINE__)
#else
#define nTest(a) (1)
extern void    (*nDelete)(number * a);
#endif

extern BOOLEAN (*nSetMap)(int c, char **par, int nop, number minpol);
extern number  (*nMap)(number from);
extern char *  (*nName)(number n);

void nDummy1(number* d);
void nDummy2(number &d);
number ndGcd(number a, number b);

#ifdef LDEBUG
void nDBDummy1(number* d,char *f, int l);
#endif
int  nGetChar();
void nSetChar(int c, BOOLEAN complete, char **param, int pars);

#ifndef FAST_AND_DIRTY
#define npMultM  nMult
#define npSubM   nSub
#define npNegM   nNeg
#define npEqualM nEqual
#endif

#endif
