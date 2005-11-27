//#ifndef RMODULO2M_H
//#define RMODULO2M_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: rmodulo2m.h,v 1.1 2005-11-27 16:30:09 wienand Exp $ */
/*
* ABSTRACT: numbers modulo 2^m
*/
#include "structs.h"

extern int nr2mExp;
extern long nr2mModul;

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
number  nr2mDiv         (number a, number b);
number  nr2mNeg         (number c);
number  nr2mInvers      (number c);
BOOLEAN nr2mGreater     (number a, number b);
BOOLEAN nr2mEqual       (number a, number b);
nMapFunc nr2mSetMap(ring src, ring dst);
void    nr2mWrite       (number &a);
char *  nr2mRead        (char *s, number *a);
char *  nr2mName        (number n);
#ifdef LDEBUG
// BOOLEAN nr2mDBTest      (number a, char *f, int l);
#endif
void    nr2mSetExp(int c, ring r);
void    nr2mInitExp(int c, ring r);


static inline number nr2mMultM(number a, number b)
{
  return (number) 
    ((((unsigned long) a)*((unsigned long) b)) % ((unsigned long) nr2mModul));
}

static inline number nr2mAddM(number a, number b)
{
  long r = (long)a + (long)b;
  return (number) (r >= nr2mModul ? r - nr2mModul : r);
}

static inline number nr2mSubM(number a, number b)
{
  return (number)((long)a<(long)b ?
                       nr2mModul-(long)b+(long)a : (long)a-(long)b);
}

#define nr2mNegM(A) (number)(nr2mModul-(long)(A))
#define nr2mEqualM(A,B)  ((A)==(B))
