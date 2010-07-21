#ifndef RMODULO2M_H
#define RMODULO2M_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers modulo 2^m
*/
#ifdef HAVE_RINGS
#include <kernel/structs.h>

extern int nr2mExp;
extern NATNUMBER nr2mModul; /* for storing 2^m - 1, i.e., the
                               bit pattern '11..1' of length m */

BOOLEAN nr2mGreaterZero (number k);
number  nr2mMult        (number a, number b);
number  nr2mInit        (int i, const ring r);
int     nr2mInt         (number &n, const ring r);
number  nr2mAdd         (number a, number b);
number  nr2mSub         (number a, number b);
void    nr2mPower       (number a, int i, number * result);
BOOLEAN nr2mIsZero      (number a);
BOOLEAN nr2mIsOne       (number a);
BOOLEAN nr2mIsMOne      (number a);
BOOLEAN nr2mIsUnit      (number a);
number  nr2mGetUnit     (number a);
number  nr2mDiv         (number a, number b);
number  nr2mIntDiv      (number a,number b);
number  nr2mMod         (number a,number b);
number  nr2mNeg         (number c);
number  nr2mInvers      (number c);
BOOLEAN nr2mGreater     (number a, number b);
BOOLEAN nr2mDivBy       (number a, number b);
int     nr2mDivComp     (number a, number b);
BOOLEAN nr2mEqual       (number a, number b);
number  nr2mLcm         (number a,number b, ring r);
number  nr2mGcd         (number a,number b,ring r);
number  nr2mExtGcd      (number a, number b, number *s, number *t);
nMapFunc nr2mSetMap     (const ring src, const ring dst);
void    nr2mWrite       (number &a, const ring r);
const char *  nr2mRead  (const char *s, number *a);
char *  nr2mName        (number n);
#ifdef LDEBUG
BOOLEAN nr2mDBTest      (number a, const char *f, const int l);
#endif
void    nr2mSetExp(int c, const ring r);
void    nr2mInitExp(int c, const ring r);


static inline number nr2mMultM(number a, number b)
{
  return (number)
    ((((NATNUMBER) a) * ((NATNUMBER) b)) & ((NATNUMBER) currRing->nr2mModul));
}

static inline number nr2mAddM(number a, number b)
{
  return (number)
    ((((NATNUMBER) a) + ((NATNUMBER) b)) & ((NATNUMBER) currRing->nr2mModul));
}

static inline number nr2mSubM(number a, number b)
{
  return (number)((NATNUMBER)a < (NATNUMBER)b ?
                     currRing->nr2mModul - (NATNUMBER)b + (NATNUMBER)a + 1 :
                     (NATNUMBER)a - (NATNUMBER)b);
}

#define nr2mNegM(A) (number)(currRing->nr2mModul - (NATNUMBER)(A) + 1)
#define nr2mEqualM(A,B)  ((A)==(B))

number nr2mMapQ(number from);
#endif
#endif
