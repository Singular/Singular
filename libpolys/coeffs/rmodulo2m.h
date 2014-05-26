#ifndef RMODULO2M_H
#define RMODULO2M_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo 2^m such that 2^m - 1
*           fits in an unsigned long
*/
#ifdef HAVE_RINGS
#include <coeffs/coeffs.h>
#include <coeffs/rintegers.h>

#ifndef NATNUMBER
#define NATNUMBER unsigned long
#endif

BOOLEAN nr2mInitChar    (coeffs r, void*);

number  nr2mCopy        (number a, const coeffs r);
BOOLEAN nr2mGreaterZero (number k, const coeffs r);
number  nr2mMult        (number a, number b, const coeffs r);
number  nr2mInit        (long i, const coeffs r);
int     nr2mInt         (number &n, const coeffs r);
number  nr2mAdd         (number a, number b, const coeffs r);
number  nr2mSub         (number a, number b, const coeffs r);
void    nr2mPower       (number a, int i, number * result, const coeffs r);
BOOLEAN nr2mIsZero      (number a, const coeffs r);
BOOLEAN nr2mIsOne       (number a, const coeffs r);
BOOLEAN nr2mIsMOne      (number a, const coeffs r);
BOOLEAN nr2mIsUnit      (number a, const coeffs r);
number  nr2mGetUnit     (number a, const coeffs r);
number  nr2mDiv         (number a, number b, const coeffs r);
number  nr2mIntDiv      (number a,number b, const coeffs r);
number  nr2mMod         (number a,number b, const coeffs r);
number  nr2mNeg         (number c, const coeffs r);
number  nr2mInvers      (number c, const coeffs r);
BOOLEAN nr2mGreater     (number a, number b, const coeffs r);
BOOLEAN nr2mDivBy       (number a, number b, const coeffs r);
int     nr2mDivComp     (number a, number b, const coeffs r);
BOOLEAN nr2mEqual       (number a, number b, const coeffs r);
number  nr2mLcm         (number a, number b, const coeffs r);
number  nr2mGcd         (number a, number b, const coeffs r);
number  nr2mExtGcd      (number a, number b, number *s, number *t, const coeffs r);
nMapFunc nr2mSetMap     (const coeffs src, const coeffs dst);
void    nr2mWrite       (number &a, const coeffs r);
const char *  nr2mRead  (const char *s, number *a, const coeffs r);
char *  nr2mName        (number n, const coeffs r);
void    nr2mCoeffWrite  (const coeffs r, BOOLEAN details);
coeffs  nr2mQuot1(number c, const coeffs r);
#ifdef LDEBUG
BOOLEAN nr2mDBTest      (number a, const char *f, const int l, const coeffs r);
#endif
void    nr2mSetExp(int c, const coeffs r);
void    nr2mInitExp(int c, const coeffs r);

number nr2mMapQ(number from, const coeffs src, const coeffs dst);

static inline number nr2mMultM(number a, number b, const coeffs r)
{
  return (number)
    ((((NATNUMBER) a) * ((NATNUMBER) b)) & ((NATNUMBER)r->mod2mMask));
}

static inline number nr2mAddM(number a, number b, const coeffs r)
{
  return (number)
    ((((NATNUMBER) a) + ((NATNUMBER) b)) & ((NATNUMBER)r->mod2mMask));
}

static inline number nr2mSubM(number a, number b, const coeffs r)
{
  return (number)((NATNUMBER)a < (NATNUMBER)b ?
                       r->mod2mMask - (NATNUMBER)b + (NATNUMBER)a + 1:
                       (NATNUMBER)a - (NATNUMBER)b);
}

#define nr2mNegM(A,r) (number)((r->mod2mMask - (NATNUMBER)(A) + 1) & r->mod2mMask)
#define nr2mEqualM(A,B)  ((A)==(B))


#endif
#endif
