#include <stdio.h>
#include <iostream.h>
#include <config.h>

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_primes.h"
#include "cf_algorithm.h"
#include "algext.h"


void tryEuclid( const CanonicalForm & A, const CanonicalForm & B, const CanonicalForm M, CanonicalForm & result, bool & fail )
{
  CanonicalForm P;
  if( degree(A) > degree(B) )
  {
    P = A; result = B;
  }
  else
  {
    P = B; result = A;
  }
  if( P.isZero() ) // then result is zero, too
    return;
  CanonicalForm inv;
  if( result.isZero() )
  {
    tryInvert( Lc(P), M, inv, fail );
    if(fail)
      return;
    result = inv*P; // monify result
    return;
  }
  CanonicalForm rem;
  // here: degree(P) >= degree(result)
  while(true)
  {
    tryInvert( Lc(result), M, inv, fail );
    if(fail)
      return;
    // here: Lc(result) is invertible modulo M
    rem = P - Lc(P)*inv*result * power( P.mvar(), degree(P)-degree(result) );
    if( rem.isZero() )
    {
      result *= inv; // monify result
      return;
    }
    P = result;
    result = rem;
  }
}


void tryInvert( const CanonicalForm & F, const CanonicalForm & M, CanonicalForm & inv, bool & fail )
{
  // F, M are required to be "univariate" polynomials in an algebraic variable
  // we try to invert F modulo M
  CanonicalForm b;
  Variable a = M.mvar();
  Variable x = Variable(1);
  if(!extgcd( replacevar( F, a, x ), replacevar( M, a, x ), inv, b ).isOne())
    fail = true;
  else
    inv = replacevar( inv, a, x); // change back to alg var
}


bool hasFirstAlgVar( const CanonicalForm & f, Variable & a )
{
  if( f.inBaseDomain() ) // f has NO alg. variable
    return false;

  if( f.level()<0 ) // f has only alg. vars, so take the first one
  {
    a = f.mvar();
    return true;
  }
  for(CFIterator i=f; i.hasTerms(); i++)
    if( hasFirstAlgVar( i.coeff(), a ))
      return true; // 'a' is already set

  return false;
}


CanonicalForm QGCD( const CanonicalForm & F, const CanonicalForm & G )
{
  CanonicalForm f, g, tmp, M, q, D, Dp, cl, newD, newq;
  int p, dp_deg, bound, i;
  bool fail;
  On(SW_RATIONAL);
  f = F * bCommonDen(F);
  g = G * bCommonDen(G);
  Variable a,b;
  if( !hasFirstAlgVar( f, a ) && !hasFirstAlgVar( g, b ))
  {
    // F and G are in Q[x], call...
#ifdef HAVE_NTL
    if ( isOn( SW_USE_NTL_GCD_0 ))
      return gcd_univar_ntl0( f, g );
#endif
    return gcd_poly_univar0( f, g, false );
  }
  if( b.level() > a.level() )
    a = b;
  // here: a is the biggest alg. var in f and g
  tmp = getMipo(a);
  M = tmp * bCommonDen(tmp);
  Off(SW_RATIONAL);
  // calculate upper bound for degree of gcd
  bound = degree(f);
  i = degree(g);
  if( i < bound )
    bound = i;

  cl = lc(M) * lc(f) * lc(g);
  q = 1;
  D = 0;
  for(i=cf_getNumBigPrimes()-1; i>-1; i--)
  {
    p = cf_getBigPrime(i);
    if( mod( cl, p ).isZero() ) // skip lc-bad primes
      continue;

    fail = false;
    setCharacteristic(p);
    tryEuclid( mapinto(f), mapinto(g), mapinto(M), Dp, fail );
    setCharacteristic(0);
    if( fail ) // M splits in char p
      continue;

    dp_deg = degree(Dp);

    if( !dp_deg ) // early termination
      return CanonicalForm(1);

    if( dp_deg > bound ) // current prime unlucky
      continue;

    if( dp_deg < bound ) // all previous primes unlucky
    {
      q = p;
      D = mapinto(Dp); // shortcut CRA
      bound = dp_deg; // tighten bound
      continue;
    }
    chineseRemainder( D, q, mapinto(Dp), p, newD, newq );
    // newD = Dp mod p
    // newD = D mod q
    // newq = p*q
    q = newq;
    if( D != newD )
    {
      D = newD;
      continue;
    }
    On( SW_RATIONAL );
    tmp = Farey( D, q );
    if( fdivides( tmp, f ) && fdivides( tmp, g )) // trial division
    {
      Off( SW_RATIONAL );
      return tmp;
    }
    Off( SW_RATIONAL );
  }
  // hopefully, we never reach this point
  Off( SW_USE_QGCD );
  D = gcd( f, g );
  On( SW_USE_QGCD );
  return D;
}
