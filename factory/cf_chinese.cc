/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file cf_chinese.cc
 *
 * algorithms for chinese remaindering and rational reconstruction
 *
 * Used by: cf_gcd.cc, cf_linsys.cc
 *
 * Header file: cf_algorithm.h
 *
**/


#include "config.h"


#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

#include "cf_assert.h"
#include "debug.h"

#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_util.h"


/** void chineseRemainder ( const CanonicalForm & x1, const CanonicalForm & q1, const CanonicalForm & x2, const CanonicalForm & q2, CanonicalForm & xnew, CanonicalForm & qnew )
 *
 * chineseRemainder - integer chinese remaindering.
 *
 * Calculate xnew such that xnew=x1 (mod q1) and xnew=x2 (mod q2)
 * and qnew = q1*q2.  q1 and q2 should be positive integers,
 * pairwise prime, x1 and x2 should be polynomials with integer
 * coefficients.  If x1 and x2 are polynomials with positive
 * coefficients, the result is guaranteed to have positive
 * coefficients, too.
 *
 * Note: This algorithm is optimized for the case q1>>q2.
 *
 * This is a standard algorithm.  See, for example,
 * Geddes/Czapor/Labahn - 'Algorithms for Computer Algebra',
 * par. 5.6 and 5.8, or the article of M. Lauer - 'Computing by
 * Homomorphic Images' in B. Buchberger - 'Computer Algebra -
 * Symbolic and Algebraic Computation'.
 *
 * Note: Be sure you are calculating in Z, and not in Q!
 *
**/
void
chineseRemainder ( const CanonicalForm & x1, const CanonicalForm & q1, const CanonicalForm & x2, const CanonicalForm & q2, CanonicalForm & xnew, CanonicalForm & qnew )
{
    DEBINCLEVEL( cerr, "chineseRemainder" );

    DEBOUTLN( cerr, "log(q1) = " << q1.ilog2() );
    DEBOUTLN( cerr, "log(q2) = " << q2.ilog2() );

    // We calculate xnew as follows:
    //     xnew = v1 + v2 * q1
    // where
    //     v1 = x1 (mod q1)
    //     v2 = (x2-v1)/q1 (mod q2)  (*)
    //
    // We do one extra test to check whether x2-v1 vanishes (mod
    // q2) in (*) since it is not costly and may save us
    // from calculating the inverse of q1 (mod q2).
    //
    // u: v1 (mod q2)
    // d: x2-v1 (mod q2)
    // s: 1/q1 (mod q2)
    //
    CanonicalForm v2, v1;
    CanonicalForm u, d, s, dummy;

    v1 = mod( x1, q1 );
    u = mod( v1, q2 );
    d = mod( x2-u, q2 );
    if ( d.isZero() )
    {
        xnew = v1;
        qnew = q1 * q2;
        DEBDECLEVEL( cerr, "chineseRemainder" );
        return;
    }
    (void)bextgcd( q1, q2, s, dummy );
    v2 = mod( d*s, q2 );
    xnew = v1 + v2*q1;

    // After all, calculate new modulus.  It is important that
    // this is done at the very end of the algorithm, since q1
    // and qnew may refer to the same object (same is true for x1
    // and xnew).
    qnew = q1 * q2;

    DEBDECLEVEL( cerr, "chineseRemainder" );
}
//}}}

/** void chineseRemainder ( const CFArray & x, const CFArray & q, CanonicalForm & xnew, CanonicalForm & qnew )
 *
 * chineseRemainder - integer chinese remaindering.
 *
 * Calculate xnew such that xnew=x[i] (mod q[i]) and qnew is the
 * product of all q[i].  q[i] should be positive integers,
 * pairwise prime.  x[i] should be polynomials with integer
 * coefficients.  If all coefficients of all x[i] are positive
 * integers, the result is guaranteed to have positive
 * coefficients, too.
 *
 * This is a standard algorithm, too, except for the fact that we
 * use a divide-and-conquer method instead of a linear approach
 * to calculate the remainder.
 *
 * Note: Be sure you are calculating in Z, and not in Q!
 *
**/
void
chineseRemainder ( const CFArray & x, const CFArray & q, CanonicalForm & xnew, CanonicalForm & qnew )
{
    DEBINCLEVEL( cerr, "chineseRemainder( ... CFArray ... )" );

    ASSERT( x.min() == q.min() && x.size() == q.size(), "incompatible arrays" );
    CFArray X(x), Q(q);
    int i, j, n = x.size(), start = x.min();

    DEBOUTLN( cerr, "array size = " << n );

    while ( n != 1 )
    {
        i = j = start;
        while ( i < start + n - 1 )
        {
            // This is a little bit dangerous: X[i] and X[j] (and
            // Q[i] and Q[j]) may refer to the same object.  But
            // xnew and qnew in the above function are modified
            // at the very end of the function, so we do not
            // modify x1 and q1, resp., by accident.
            chineseRemainder( X[i], Q[i], X[i+1], Q[i+1], X[j], Q[j] );
            i += 2;
            j++;
        }

        if ( n & 1 )
        {
            X[j] = X[i];
            Q[j] = Q[i];
        }
        // Maybe we would get some memory back at this point if
        // we would set X[j+1, ..., n] and Q[j+1, ..., n] to zero
        // at this point?

        n = ( n + 1) / 2;
    }
    xnew = X[start];
    qnew = Q[q.min()];

    DEBDECLEVEL( cerr, "chineseRemainder( ... CFArray ... )" );
}

#if !defined(HAVE_NTL) && !defined(HAVE_FLINT)
CanonicalForm Farey_n (CanonicalForm N, const CanonicalForm P)
//"USAGE:  Farey_n (N,P); P, N number;
//RETURN:  a rational number a/b such that a/b=N mod P
//         and |a|,|b|<(P/2)^{1/2}
{
   //assume(P>0);
   // assume !isOn(SW_RATIONAL): mod is a no-op otherwise
   if (N<0) N +=P;
   CanonicalForm A,B,C,D,E;
   E=P;
   B=1;
   while (!N.isZero())
   {
        if (2*N*N<P)
        {
           On(SW_RATIONAL);
           N /=B;
           Off(SW_RATIONAL);
           return(N);
        }
        D=mod(E , N);
        C=A-(E-mod(E , N))/N*B;
        E=N;
        N=D;
        A=B;
        B=C;
   }
   return(0);
}
#endif

/**
 * Farey rational reconstruction. If NTL is available it uses the fast algorithm
 * from NTL, i.e. Encarnacion, Collins.
**/
CanonicalForm Farey ( const CanonicalForm & f, const CanonicalForm & q )
{
    int is_rat=isOn(SW_RATIONAL);
    Off(SW_RATIONAL);
    Variable x = f.mvar();
    CanonicalForm result = 0;
    CanonicalForm c;
    CFIterator i;
#ifdef HAVE_FLINT
   fmpz_t FLINTq;
   fmpz_init(FLINTq);
   convertCF2initFmpz(FLINTq,q);
   fmpz_t FLINTc;
   fmpz_init(FLINTc);
   fmpq_t FLINTres;
   fmpq_init(FLINTres);
#elif defined(HAVE_NTL)
    ZZ NTLq= convertFacCF2NTLZZ (q);
    ZZ bound;
    SqrRoot (bound, NTLq/2);
#else
   factoryError("NTL/FLINT missing:Farey");
#endif
    for ( i = f; i.hasTerms(); i++ )
    {
        c = i.coeff();
        if ( c.inCoeffDomain())
        {
#ifdef HAVE_FLINT
          if (c.inZ())
          {
             convertCF2initFmpz(FLINTc,c);
             fmpq_reconstruct_fmpz(FLINTres,FLINTc,FLINTq);
             result += power (x, i.exp())*(convertFmpq2CF(FLINTres));
          }
#elif defined(HAVE_NTL)
          if (c.inZ())
          {
            ZZ NTLc= convertFacCF2NTLZZ (c);
            bool lessZero= (sign (NTLc) == -1);
            if (lessZero)
              NTL::negate (NTLc, NTLc);
            ZZ NTLnum, NTLden;
            if (ReconstructRational (NTLnum, NTLden, NTLc, NTLq, bound, bound))
            {
              if (lessZero)
                NTL::negate (NTLnum, NTLnum);
              CanonicalForm num= convertNTLZZX2CF (to_ZZX (NTLnum), Variable (1));
              CanonicalForm den= convertNTLZZX2CF (to_ZZX (NTLden), Variable (1));
              On (SW_RATIONAL);
              result += power (x, i.exp())*(num/den);
              Off (SW_RATIONAL);
            }
          }
#else
          if (c.inZ())
            result += power (x, i.exp()) * Farey_n(c,q);
#endif
          else
            result += power( x, i.exp() ) * Farey(c,q);
        }
        else
          result += power( x, i.exp() ) * Farey(c,q);
    }
    if (is_rat) On(SW_RATIONAL);
#ifdef HAVE_FLINT
    fmpq_clear(FLINTres);
    fmpz_clear(FLINTc);
    fmpz_clear(FLINTq);
#endif
    return result;
}

// returns x where (a * x) % b == 1, inv is a cache
static inline CanonicalForm chin_mul_inv(const CanonicalForm a, const CanonicalForm b, int ind, CFArray &inv)
{
  if (inv[ind].isZero())
  {
    CanonicalForm s,dummy;
    (void)bextgcd( a, b, s, dummy );
    inv[ind]=s;
    return s;
  }
  else
    return inv[ind];
}

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);
void chineseRemainderCached(const CFArray &a, const CFArray &n, CanonicalForm &xnew, CanonicalForm &prod, CFArray &inv)
{
  CanonicalForm p, sum=0L; prod=1L;
  int i;
  int len=n.size();

  for (i = 0; i < len; i++) prod *= n[i];

  for (i = 0; i < len; i++)
  {
    p = prod / n[i];
    sum += a[i] * chin_mul_inv(p, n[i], i, inv) * p;
  }

  xnew = mod(sum , prod);
}
// http://rosettacode.org/wiki/Chinese_remainder_theorem#C

void chineseRemainderCached ( const CanonicalForm & a, const CanonicalForm &q1, const CanonicalForm & b, const CanonicalForm & q2, CanonicalForm & xnew, CanonicalForm & qnew,CFArray &inv )
{
  CFArray A(2); A[0]=a; A[1]=b;
  CFArray Q(2); Q[0]=q1; Q[1]=q2;
  chineseRemainderCached(A,Q,xnew,qnew,inv);
}
