#ifndef NUMBERS_H
#define NUMBERS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: interface to coefficient aritmetics
*/
#include <kernel/structs.h>

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
#define n_Init(i, r)          (r)->cf->cfInit(i,r)
#define n_IsOne(n, r)         (r)->cf->nIsOne(n)
#define n_IsMOne(n, r)        (r)->cf->nIsMOne(n)
#define n_GreaterZero(n, r)   (r)->cf->nGreaterZero(n)
#define n_Write(n, r)         (r)->cf->cfWrite(n,r)
#define n_Normalize(n, r)     (r)->cf->nNormalize(n)
#define n_Gcd(a, b, r)        (r)->cf->nGcd(a,b,r)
#define n_IntDiv(a, b, r)     (r)->cf->nIntDiv(a,b)
#define n_Div(a, b, r)        (r)->cf->nDiv(a,b)
#define n_Invers(a, r)     (r)->cf->nInvers(a)
#define n_ExactDiv(a, b, r)   (r)->cf->nExactDiv(a,b)
#define n_Test(a,r)           (r)->cf->nDBTest(a,__FILE__,__LINE__)

#define n_InpMult(a, b, r)    (r)->cf->nInpMult(a,b,r)
#define n_Power(a, b, res, r) (r)->cf->nPower(a,b,res)
#define n_Size(n,r)           (r)->cf->nSize(n)
#define n_GetDenom(N,r)       (r)->cf->cfGetDenom((N),r)
#define n_GetNumerator(N,r)   (r)->cf->cfGetNumerator((N),r)

#define n_New(n, r)           nNew(n)

/* variables */
extern unsigned short fftable[];

/* prototypes */
extern numberfunc nMult, nSub ,nAdd ,nDiv, nIntDiv, nIntMod, nExactDiv;
void           nNew(number * a);
extern number  (*nInit_bigint)(number i);
#define        nInit(i) n_Init(i,currRing)
extern number  (*nPar)(int i);
extern int     (*nParDeg)(number n);
/* size: a measure for the complexity of the represented number n;
         zero should have size zero; larger size means more complex */
extern int     (*nSize)(number n);
extern int     (*n_Int)(number &n, const ring r);
#ifdef HAVE_RINGS
extern int     (*nDivComp)(number a,number b);
extern BOOLEAN (*nIsUnit)(number a);
extern number  (*nGetUnit)(number a);
extern number  (*nExtGcd)(number a, number b, number *s, number *t);
#endif
// always use in the form    n=nNeg(n) !
extern number  (*nNeg)(number a);
extern number  (*nInvers)(number a);
extern number  (*nCopy)(number a);
extern number  (*nRePart)(number a);
extern number  (*nImPart)(number a);
#define nWrite(A) n_Write(A,currRing)
extern const char *  (*nRead)(const char * s, number * a);
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

#define nTest(a) (1)
#define nDelete(A) (currRing)->cf->cfDelete(A,currRing)
#define nGetDenom(N) (currRing->cf->cfGetDenom((N),currRing))
#define nGetNumerator(N) (currRing->cf->cfGetNumerator((N),currRing))

#define nSetMap(R) (currRing->cf->cfSetMap(R,currRing))
extern char *  (*nName)(number n);

void nDummy1(number* d);
void ndDelete(number* d, const ring r);
void nDummy2(number &d);
number ndGcd(number a, number b, const ring);
number ndCopy(number a);
void   ndInpMult(number &a, number b, const ring r);
number ndInpAdd(number &a, number b, const ring r);

#ifdef LDEBUG
void nDBDummy1(number* d,char *f, int l);
#endif
#define nGetChar() n_GetChar(currRing)

void nInitChar(ring r);
void nKillChar(ring r);
void nSetChar(ring r);

#define nDivBy0 "div by 0"

// dummy routines
void   nDummy2(number& d); // nNormalize...
#endif
