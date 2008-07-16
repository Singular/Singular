#ifndef RMODULO2M_H
#define RMODULO2M_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: rmodulo2m.h,v 1.10 2008-07-16 12:41:33 wienand Exp $ */
/*
* ABSTRACT: numbers modulo 2^m
*/
#ifdef HAVE_RING2TOM
#include "structs.h"

extern int nr2mExp;
extern NATNUMBER nr2mModul;

BOOLEAN nr2mGreaterZero (number k);
number  nr2mMult        (number a, number b);
number  nr2mInit        (int i);
int     nr2mInt         (number &n);
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
number  nr2mNeg         (number c);
number  nr2mInvers      (number c);
BOOLEAN nr2mGreater     (number a, number b);
BOOLEAN nr2mDivBy       (number a, number b);
int     nr2mDivComp     (number a, number b);
BOOLEAN nr2mEqual       (number a, number b);
number  nr2mLcm         (number a,number b, ring r);
number  nr2mGcd         (number a,number b,ring r);
number  nr2mExtGcd      (number a, number b, number *s, number *t);
nMapFunc nr2mSetMap     (ring src, ring dst);
void    nr2mWrite       (number &a);
const char *  nr2mRead  (const char *s, number *a);
char *  nr2mName        (number n);
#ifdef LDEBUG
BOOLEAN nr2mDBTest      (number a, const char *f, const int l);
#endif
void    nr2mSetExp(int c, ring r);
void    nr2mInitExp(int c, ring r);


static inline number nr2mMultM(number a, number b)
{
  return (number) 
    ((((NATNUMBER) a)*((NATNUMBER) b)) % ((NATNUMBER) nr2mModul));
}

static inline number nr2mAddM(number a, number b)
{
  NATNUMBER r = (NATNUMBER)a + (NATNUMBER)b;
  return (number) (r >= nr2mModul ? r - nr2mModul : r);
}

static inline number nr2mSubM(number a, number b)
{
  return (number)((NATNUMBER)a<(NATNUMBER)b ?
                       nr2mModul-(NATNUMBER)b+(NATNUMBER)a : (NATNUMBER)a-(NATNUMBER)b);
}

#define nr2mNegM(A) (number)(nr2mModul-(NATNUMBER)(A))
#define nr2mEqualM(A,B)  ((A)==(B))

number nr2mMapQ(number from);
#endif
#endif
