#include <stdio.h>
#include <iostream.h>
#include <config.h>

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_primes.h"
#include "cf_algorithm.h"
#include "algext.h"
#include "fieldGCD.h"
#include "cf_map.h"
#include "cf_generator.h"

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
  {
    printf("failed ");
    fail = true;
  }
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

CanonicalForm univarQGCD( const CanonicalForm & F, const CanonicalForm & G )
{ // F,G in Q(a)[x]
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

    if( dp_deg == 0 ) // early termination
    {
      CanonicalForm inv;
      tryInvert(Dp, M, inv, fail);
      if(fail)
        continue;
      return CanonicalForm(1);
    }

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


CanonicalForm QGCD( const CanonicalForm & F, const CanonicalForm & G )
{ // F,G in Q(a)[x1,...,xn]
  if(F.isZero())
  {
    if(G.isZero())
      return G; // G is zero
    if(G.inCoeffDomain())
      return CanonicalForm(1);
    return G/Lc(G); // return monic G
  }
  if(G.isZero()) // F is non-zero
  {
    if(F.inCoeffDomain())
      return CanonicalForm(1);
    return F/Lc(F); // return monic F
  }
  if(F.inCoeffDomain() || G.inCoeffDomain())
    return CanonicalForm(1);

  CanonicalForm D;
  if (getCharacteristic()==0)
  {
    CanonicalForm f,g,tmp, M, q, Dp, cl, newD, newq;
    int p, i;
    int *bound, *other; // degree vectors
    bool fail;
    On(SW_RATIONAL);
    f = F * bCommonDen(F);
    g = G * bCommonDen(G);
    Variable a,b;
    if( !hasFirstAlgVar( f, a ) && !hasFirstAlgVar( g, b ))
    {
      // F and G are in Q[x1,...,xn], call...
      return gcd_poly_0( f, g );
    }
    if( b.level() > a.level() )
      a = b;
    // here: a is the biggest alg. var in f and g
    tmp = getMipo(a);
    M = tmp * bCommonDen(tmp);
    Off( SW_RATIONAL );
    // here: f, g in Z[y][x1,...,xn], M in Z[y] not necessarily monic
    // calculate upper bound for degree of gcd
    int mv = f.level();
    if(g.level() > mv)
      mv = g.level();
    // here: mv is level of the largest variable in f, g
    bound = new int[mv+1];
    other = new int[mv+1];
    for(int i=1; i<=mv; i++)
      bound[i] = other[i] = 0;
    bound = leadDeg(f,bound);
    other = leadDeg(g,other);
    for(int i=1; i<=mv; i++) // entry at i=0 is not visited
      if(other[i] < bound[i])
        bound[i] = other[i];
    // now bound points on the smaller vector
    cl = lc(M) * lc(f) * lc(g);
    q = 1;
    D = 0;
    for(int i=cf_getNumBigPrimes()-1; i>-1; i--)
    {
      p = cf_getBigPrime(i);
      if( mod( cl, p ).isZero() ) // skip lc-bad primes
        continue;

      fail = false;
      setCharacteristic(p);
      tryBrownGCD( mapinto(f), mapinto(g), mapinto(M), Dp, fail );
      setCharacteristic(0);
      if( fail ) // M splits in char p
        continue;

      for(int i=1; i<=mv; i++)
        other[i] = 0; // reset (this is necessary, because some entries may not be updated by call to leadDeg)
      other = leadDeg(Dp,other);

      if( other==0 ) // early termination
      {
        // Dp is in coeff domain
        CanonicalForm inv;
        tryInvert(Dp,M,inv,fail); // check if zero-divisor
        if(fail)
          continue;
        return CanonicalForm(1);
      }

      if( isLess(bound, other, 1, mv) ) // current prime unlucky
        continue;

      if( isLess(other, bound, 1, mv) ) // all previous primes unlucky
      {
        q = p;
        D = mapinto(Dp); // shortcut CRA
        for(int i=1; i<=mv; i++) // tighten bound
          bound[i] = other[i];
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
  }
  // hopefully, we never reach this point
  Off( SW_USE_QGCD );
  D = gcd( F, G );
  On( SW_USE_QGCD );
  return D;
}

void tryBrownGCD( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm & M, CanonicalForm & result, bool & fail )
{// assume F,G are multivariate polys over Z/p(a) for big prime p, M "univariate" polynomial in an algebraic variable
  printf("Brown ");
  if(F.isZero())
  {
    if(G.isZero())
    {
      result = G; // G is zero
      return;
    }
    if(G.inCoeffDomain())
    {
      tryInvert(G,M,result,fail);
      return;
    }
    // try to make G monic modulo M
    CanonicalForm inv;
    tryInvert(Lc(G),M,inv,fail);
    if(fail)
      return;
    result = inv*G;
    return;
  }
  if(G.isZero()) // F is non-zero
  {
    if(F.inCoeffDomain())
    {
      tryInvert(F,M,result,fail);
      return;
    }
    // try to make F monic modulo M
    CanonicalForm inv;
    tryInvert(Lc(F),M,inv,fail);
    if(fail)
      return;
    result = inv*F;
    return;
  }
  if(F.inCoeffDomain())
  {
    tryInvert(F,M,result,fail);
    return;
  }
  if(G.inCoeffDomain())
  {
    tryInvert(G,M,result,fail);
    return;
  }
  CFMap MM,NN;
  CFArray ps(1,2);
  ps[1] = F;
  ps[2] = G;
  compress(ps,MM,NN); // maps MM, NN are created
  CanonicalForm f=MM(F);
  CanonicalForm g=MM(G);
  // here: f, g are compressed
  // compute largest variable in f or g (least one is Variable(1))
  int mv = f.level();
  if(g.level() > mv)
    mv = g.level();
  // here: mv is level of the largest variable in f, g
  if(mv == 1) // f,g univariate
  {
    tryEuclid(f,g,M,result,fail);
    if(fail)
      return;
    result = NN(result); // do not forget to map back
    return;
  }
  // here: mv > 1
  CanonicalForm cf = vcontent(f, Variable(2)); // cf is univariate poly in var(1)
  CanonicalForm cg = vcontent(g, Variable(2));
  CanonicalForm c;
  tryEuclid(cf,cg,M,c,fail);
  if(fail)
    return;
  f/=cf;
  g/=cg;
  if(f.inCoeffDomain())
  {
    tryInvert(f,M,result,fail);
    if(fail)
      return;
    result = NN(result);
    return;
  }
  if(g.inCoeffDomain())
  {
    tryInvert(g,M,result,fail);
    if(fail)
      return;
    result = NN(result);
    return;
  }
  int *L = new int[mv+1]; // L is addressed by i from 2 to mv
  int *N = new int[mv+1];
  for(int i=2; i<=mv; i++)
    L[i] = N[i] = 0;
  L = leadDeg(f, L);
  N = leadDeg(g, N);
  CanonicalForm gamma;
  tryEuclid( firstLC(f), firstLC(g), M, gamma, fail );
  if(fail)
    return;
  for(int i=2; i<=mv; i++) // entry at i=1 is not visited
    if(N[i] < L[i])
      L[i] = N[i];
  // L is now upper bound for degrees of gcd
  int *dg_im = new int[mv+1]; // for the degree vector of the image we don't need any entry at i=1
  for(int i=2; i<=mv; i++)
    dg_im[i] = 0; // initialize
  CanonicalForm gamma_image, m=1;
  CanonicalForm gm=0;
  CanonicalForm g_image, alpha, gnew, mnew;
  FFGenerator gen = FFGenerator();
  for(FFGenerator gen = FFGenerator(); gen.hasItems(); gen.next())
  {
    alpha = gen.item();
    gamma_image = gamma(alpha, Variable(1)); // plug in alpha for var(1)
    if(gamma_image.isZero()) // skip lc-bad points var(1)-alpha
      continue;
    tryBrownGCD( f(alpha, Variable(1)), g(alpha, Variable(1)), M, g_image, fail ); // recursive call with one var less
    if(fail)
      return;
    if(g_image.inCoeffDomain()) // early termination
    {
      tryInvert(g_image,M,result,fail);
      if(fail)
        return;
      result = NN(c);
      return;
    }
    for(int i=2; i<=mv; i++)
      dg_im[i] = 0; // reset (this is necessary, because some entries may not be updated by call to leadDeg)
    dg_im = leadDeg(g_image, dg_im); // dg_im cannot be NIL-pointer
    if(isEqual(dg_im, L, 2, mv))
    {
      g_image /= lc(g_image); // make g_image monic over Z/p
      g_image *= gamma_image; // multiply by multiple of image lc(gcd)
      tryCRA( g_image, Variable(1)-alpha, gm, m, gnew, mnew, fail );
      // gnew = gm mod m
      // gnew = g_image mod var(1)-alpha
      // mnew = m * (var(1)-alpha)
      if(fail)
        return;
      m = mnew;
      if(gnew == gm) // gnew did not change
      {
        g_image = gm / vcontent(gm, Variable(2));
        if(fdivides(g_image,f) && fdivides(g_image,g)) // trial division
        {
          result = NN(c*g_image);
          return;
        }
      }
      gm = gnew;
      continue;
    }

    if(isLess(L, dg_im, 2, mv)) // dg_im > L --> current point unlucky
      continue;

    if(isLess(dg_im, L, 2, mv)) // dg_im < L --> all previous points were unlucky
    {
      m = CanonicalForm(1); // reset
      gm = 0; // reset
      for(int i=2; i<=mv; i++) // tighten bound
        L[i] = dg_im[i];
    }
  }
  // we are out of evaluation points
  fail = true;
}


void tryCRA( const CanonicalForm & x1, const CanonicalForm & q1, const CanonicalForm & x2, const CanonicalForm & q2, CanonicalForm & xnew, CanonicalForm & qnew, bool & fail )
{ // polys of level <= 1 are considered coefficients. q1,q2 are assumed to be coprime
  // xnew = x1 mod q1 (coefficientwise in the above sense)
  // xnew = x2 mod q2
  // qnew = q1*q2
  CanonicalForm tmp;
  if(x1.level() <= 1 && x2.level() <= 1) // base case
  {
    tryExtgcd(q1,q2,tmp,xnew,qnew,fail);
    if(fail)
      return;
    xnew = x1 + (x2-x1) * xnew * q1;
    qnew = q1*q2;
    xnew = mod(xnew,qnew);
    return;
  }
  CanonicalForm tmp2;
  xnew = 0;
  qnew = q1 * q2;
  // here: x1.level() > 1 || x2.level() > 1
  if(x1.level() > x2.level())
  {
    for(CFIterator i=x1; i.hasTerms(); i++)
    {
      if(i.exp() == 0) // const. term
      {
        tryCRA(i.coeff(),q1,x2,q2,tmp,tmp2,fail);
        if(fail)
          return;
        xnew += tmp;
      }
      else
      {
        tryCRA(i.coeff(),q1,0,q2,tmp,tmp2,fail);
        if(fail)
          return;
        xnew += tmp * power(x1.mvar(),i.exp());
      }
    }
    return;
  }
  // here: x1.level() <= x2.level() && ( x1.level() > 1 || x2.level() > 1 )
  if(x2.level() > x1.level())
  {
    for(CFIterator j=x2; j.hasTerms(); j++)
    {
      if(j.exp() == 0) // const. term
      {
        tryCRA(x1,q1,j.coeff(),q2,tmp,tmp2,fail);
        if(fail)
          return;
        xnew += tmp;
      }
      else
      {
        tryCRA(0,q1,j.coeff(),q2,tmp,tmp2,fail);
        if(fail)
          return;
        xnew += tmp * power(x2.mvar(),j.exp());
      }
    }
    return;
  }
  // here: x1.level() == x2.level() && x1.level() > 1 && x2.level() > 1
  CFIterator i = x1;
  CFIterator j = x2;
  while(i.hasTerms() || j.hasTerms())
  {
    if(i.hasTerms())
    {
      if(j.hasTerms())
      {
        if(i.exp() == j.exp())
        {
          tryCRA(i.coeff(),q1,j.coeff(),q2,tmp,tmp2,fail);
          if(fail)
            return;
          xnew += tmp * power(x1.mvar(),i.exp());
          i++; j++;
        }
        else
        {
          if(i.exp() < j.exp())
          {
            tryCRA(i.coeff(),q1,0,q2,tmp,tmp2,fail);
            if(fail)
              return;
            xnew += tmp * power(x1.mvar(),i.exp());
            i++;
          }
          else // i.exp() > j.exp()
          {
            tryCRA(0,q1,j.coeff(),q2,tmp,tmp2,fail);
            if(fail)
              return;
            xnew += tmp * power(x1.mvar(),j.exp());
            j++;
          }
        }
      }
      else // j is out of terms
      {
        tryCRA(i.coeff(),q1,0,q2,tmp,tmp2,fail);
        if(fail)
          return;
        xnew += tmp * power(x1.mvar(),i.exp());
        i++;
      }
    }
    else // i is out of terms
    {
      tryCRA(0,q1,j.coeff(),q2,tmp,tmp2,fail);
      if(fail)
        return;
      xnew += tmp * power(x1.mvar(),j.exp());
      j++;
    }
  }
}


void tryExtgcd( const CanonicalForm & F, const CanonicalForm & G, CanonicalForm & result, CanonicalForm & s, CanonicalForm & t, bool & fail )
{
  // F, G are univariate polynomials (i.e. they have exactly one polynomial variable)
  // F and G must have the same level AND level > 0
  // we try to calculate gcd(f,g) = s*F + t*G
  // if a zero divisor is encontered, 'fail' is set to one
  Variable a, b;
  if( !hasFirstAlgVar(F,a) && !hasFirstAlgVar(G,b) ) // note lazy evaluation
  {
    result = extgcd( F, G, s, t ); // no zero divisors possible
    return;
  }
  if( b.level() > a.level() )
    a = b;
  // here: a is the biggest alg. var in F and G
  CanonicalForm M = getMipo(a);
  CanonicalForm P;
  if( degree(F) > degree(G) )
  {
    P=F; result=G; s=0; t=1;
  }
  else
  {
    P=G; result=F; s=1; t=0;
  }
  CanonicalForm inv, rem, q, u, v;
  // here: degree(P) >= degree(result)
  while(true)
  {
    tryInvert( Lc(result), M, inv, fail );
    if(fail)
      return;
    // here: Lc(result) is invertible modulo M
    q = Lc(P)*inv * power( P.mvar(), degree(P)-degree(result) );
    rem = P - q*result;
    // here: s*F + t*G = result
    if( rem.isZero() )
    {
      s*=inv;
      t*=inv;
      result *= inv; // monify result
      return;
    }
    P=result;
    result=rem;
    rem=u-q*s;
    u=s;
    s=rem;
    rem=v-q*t;
    v=t;
    t=rem;
  }
}
