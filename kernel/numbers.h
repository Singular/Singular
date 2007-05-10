#ifndef NUMBERS_H
#define NUMBERS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: numbers.h,v 1.7 2007-05-10 08:12:42 wienand Exp $ */
/*
* ABSTRACT: interface to coefficient aritmetics
*/
#include "structs.h"

#define n_Copy(n, r)          (r)->cf->cfCopy(n,r)
#define n_Delete(n, r)        (r)->cf->cfDelete(n,r)
#define n_Mult(n1, n2, r)     (r)->cf->nMult(n1, n2)
#define n_Add(n1, n2, r)      (r)->cf->nAdd(n1, n2)
#define n_IsZero(n, r)        (r)->cf->nIsZero(n)
#define n_Equal(n1, n2, r)    (r)->cf->nEqual(n1, n2)
#define n_Neg(n, r)           (r)->cf->nNeg(n)
#define n_Sub(n1, n2, r)      (r)->cf->nSub(n1, n2)
//#define n_GetChar(r)          ((r)->cf->nChar)
#define n_GetChar(r)          ((r)->ch)
#define n_Init(i, r)          (r)->cf->nInit(i)
#define n_IsOne(n, r)         (r)->cf->nIsOne(n)
#define n_IsMOne(n, r)        (r)->cf->nIsMOne(n)
#define n_GreaterZero(n, r)   (r)->cf->nGreaterZero(n)
#define n_Write(n, r)         (r)->cf->nWrite(n)
#define n_Normalize(n, r)     (r)->cf->nNormalize(n)
#define n_Gcd(a, b, r)        (r)->cf->nGcd(a,b,r)
#define n_IntDiv(a, b, r)     (r)->cf->nIntDiv(a,b)
#define n_Div(a, b, r)        (r)->cf->nDiv(a,b)
#define n_ExactDiv(a, b, r)   (r)->cf->nExactDiv(a,b)
#define n_Test(a,r)           (r)->cf->nDBTest(a,__FILE__,__LINE__)

#define n_InpMult(a, b, r)    (r)->cf->nInpMult(a,b,r)
#define n_Power(a, b, res, r) (r)->cf->nPower(a,b,res)
#define n_Size(n,r)           (r)->cf->nSize(n)

/* variables */
extern short fftable[];

/* prototypes */
extern numberfunc nMult, nSub ,nAdd ,nDiv, nIntDiv, nIntMod, nExactDiv;
extern void    (*nNew)(number * a);
extern number  (*nInit)(int i);
extern number  (*nPar)(int i);
extern int     (*nParDeg)(number n);
extern int     (*nSize)(number n);
extern int     (*nInt)(number &n);
// always use in the form    n=nNeg(n) !
extern number  (*nNeg)(number a);
extern number  (*nInvers)(number a);
extern number  (*nCopy)(number a);
extern number  (*nRePart)(number a);
extern number  (*nImPart)(number a);
extern void    (*nWrite)(number &a);
extern char *  (*nRead)(char * s, number * a);
extern void    (*nNormalize)(number &a);
extern BOOLEAN (*nGreater)(number a,number b),
#ifdef HAVE_RINGS
               (*nDivBy)(number a,number b),
#endif
               (*nEqual)(number a,number b),
               (*nIsZero)(number a),
               (*nIsOne)(number a),
               (*nIsMOne)(number a),
               (*nGreaterZero)(number a);
extern void    (*nPower)(number a, int i, number * result);
extern number (*nGcd)(number a, number b, const ring r);
extern number (*nLcm)(number a, number b, const ring r);

extern number nNULL; /* the 0 as constant */

extern void    (*n__Delete)(number * a, const ring r);
#define nTest(a) (1)
#define nDelete(A) (currRing)->cf->cfDelete(A,currRing)
#define nGetDenom(N) (currRing->cf->n_GetDenom((N),currRing))

#define nSetMap(R) (currRing->cf->cfSetMap(R,currRing))
extern char *  (*nName)(number n);

void nDummy1(number* d);
void ndDelete(number* d, const ring r);
void nDummy2(number &d);
number ndGcd(number a, number b, const ring);
number ndCopy(number a);
void   ndInpMult(number &a, number b, const ring r);

#ifdef LDEBUG
void nDBDummy1(number* d,char *f, int l);
#endif
#define nGetChar() n_GetChar(currRing)

void nInitChar(ring r);
void nKillChar(ring r);
void nSetChar(ring r);

#endif
