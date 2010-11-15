#include "factoryconf.h"

#ifdef HAVE_CSTDIO
#include <cstdio>
#else
#include <stdio.h>
#endif
#ifdef HAVE_IOSTREAM_H
#include <iostream.h>
#else
#include <iostream>
#endif

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_primes.h"
#include "cf_algorithm.h"
#include "algext.h"
#include "fieldGCD.h"
#include "cf_map.h"
#include "cf_generator.h"

CanonicalForm reduce(const CanonicalForm & f, const CanonicalForm & M)
{ // polynomials in M.mvar() are considered coefficients
  // M univariate monic polynomial
  // the coefficients of f are reduced modulo M
  if(f.inBaseDomain() || f.level() < M.level())
    return f;
  if(f.level() == M.level())
  {
    if(f.degree() < M.degree())
      return f;
    CanonicalForm tmp = f;
    do
      tmp -= lc(tmp)*M*power(M.mvar(),tmp.degree()-M.degree());
    while(tmp.degree() >= M.degree());
    return tmp;
  }
  // here: f.level() > M.level()
  CanonicalForm result = 0;
  for(CFIterator i=f; i.hasTerms(); i++)
    result += reduce(i.coeff(),M) * power(f.mvar(),i.exp());
  return result;
}

void tryInvert( const CanonicalForm & F, const CanonicalForm & M, CanonicalForm & inv, bool & fail )
{ // F, M are required to be "univariate" polynomials in an algebraic variable
  // we try to invert F modulo M
  if(F.inBaseDomain())
  {
    if(F.isZero())
    {
      fail = true;
      return;
    }
    inv = 1/F;
    return;
  }
  CanonicalForm b;
  Variable a = M.mvar();
  Variable x = Variable(1);
  if(!extgcd( replacevar( F, a, x ), replacevar( M, a, x ), inv, b ).isOne())
    fail = true;
  else
    inv = replacevar( inv, x, a ); // change back to alg var
}

void tryEuclid( const CanonicalForm & A, const CanonicalForm & B, const CanonicalForm & M, CanonicalForm & result, bool & fail )
{
  CanonicalForm P;
  if(A.inCoeffDomain())
  {
    tryInvert( A, M, P, fail );
    if(fail)
      return;
    result = 1;
    return;
  }
  if(B.inCoeffDomain())
  {
    tryInvert( B, M, P, fail );
    if(fail)
      return;
    result = 1;
    return;
  }
  // here: both not inCoeffDomain
  if( A.degree() > B.degree() )
  {
    P = A; result = B;
  }
  else
  {
    P = B; result = A;
  }
  CanonicalForm inv;
  if( result.isZero() )
  {
    tryInvert( Lc(P), M, inv, fail );
    if(fail)
      return;
    result = inv*P; // monify result (not reduced, yet)
    return;
  }
  Variable x = P.mvar();
  CanonicalForm rem;
  // here: degree(P) >= degree(result)
  while(true)
  {
    tryInvert( Lc(result), M, inv, fail );
    if(fail)
      return;
    // here: Lc(result) is invertible modulo M
    rem = reduce( P - Lc(P)*inv*result*power( x, P.degree(x)-result.degree(x) ), M );
    if( rem.isZero() )
    {
      result *= inv;
      return;
    }
    if(result.degree(x) >= rem.degree(x))
    {
      P = result;
      result = rem;
    }
    else
      P = rem;
  }
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



CanonicalForm QGCD( const CanonicalForm & F, const CanonicalForm & G );
int * leadDeg(const CanonicalForm & f, int *degs);
bool isLess(int *a, int *b, int lower, int upper);
bool isEqual(int *a, int *b, int lower, int upper);
CanonicalForm firstLC(const CanonicalForm & f);
void tryCRA( const CanonicalForm & x1, const CanonicalForm & q1, const CanonicalForm & x2, const CanonicalForm & q2, const CanonicalForm & M, CanonicalForm & xnew, CanonicalForm & qnew, bool & fail );
void tryExtgcd( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm & M, CanonicalForm & result, CanonicalForm & s, CanonicalForm & t, bool & fail );
static CanonicalForm trycontent ( const CanonicalForm & f, const Variable & x, const CanonicalForm & M, bool & fail );
static CanonicalForm tryvcontent ( const CanonicalForm & f, const Variable & x, const CanonicalForm & M, bool & fail );
static CanonicalForm trycf_content ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & M, bool & fail );
static void tryDivide( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & M, CanonicalForm & result, bool & divides, bool & fail );


void tryBrownGCD( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm & M, CanonicalForm & result, bool & fail )
{ // assume F,G are multivariate polys over Z/p(a) for big prime p, M "univariate" polynomial in an algebraic variable
  // M is assumed to be monic
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
      if(fail)
        return;
      result = 1;
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
      if(fail)
        return;
      result = 1;
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
  // here: F,G both nonzero
  if(F.inCoeffDomain())
  {
    tryInvert(F,M,result,fail);
    if(fail)
      return;
    result = 1;
    return;
  }
  if(G.inCoeffDomain())
  {
    tryInvert(G,M,result,fail);
    if(fail)
      return;
    result = 1;
    return;
  }
  CFMap MM,NN;
  CFArray ps(1,2);
  ps[1] = F;
  ps[2] = G;
  compress(ps,MM,NN); // maps MM, NN are created
  CanonicalForm f=MM(F);
  CanonicalForm g=MM(G);
  // here: f,g are compressed
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
  CanonicalForm cf = tryvcontent(f, Variable(2), M, fail); // cf is univariate poly in var(1)
  if(fail)
    return;
  CanonicalForm cg = tryvcontent(g, Variable(2), M, fail);
  if(fail)
    return;
  CanonicalForm c;
  tryEuclid(cf,cg,M,c,fail);
  if(fail)
    return;
  bool divides = true;
  CanonicalForm tmp;
  // f /= cf
  tryDivide(f,cf,M,tmp,divides,fail); // 'divides' is not checked
  if(fail)
    return;
  f = tmp;
  // g /= cg
  tryDivide(g,cg,M,tmp,divides,fail); // 'divides' is not checked
  if(fail)
    return;
  g = tmp;
  if(f.inCoeffDomain())
  {
    tryInvert(f,M,result,fail);
    if(fail)
      return;
    result = NN(c);
    return;
  }
  if(g.inCoeffDomain())
  {
    tryInvert(g,M,result,fail);
    if(fail)
      return;
    result = NN(c);
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
  for(int i=2; i<=mv; i++) // entries at i=0,1 not visited
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
    gamma_image = reduce(gamma(alpha, Variable(1)),M); // plug in alpha for var(1)
    if(gamma_image.isZero()) // skip lc-bad points var(1)-alpha
      continue;
    tryBrownGCD( f(alpha, Variable(1)), g(alpha, Variable(1)), M, g_image, fail ); // recursive call with one var less
    if(fail)
      return;
    g_image = reduce(g_image, M);
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
      tryCRA( g_image, Variable(1)-alpha, gm, m, M, gnew, mnew, fail );
      // gnew = gm mod m
      // gnew = g_image mod var(1)-alpha
      // mnew = m * (var(1)-alpha)
      if(fail)
        return;
      m = mnew;
      if(gnew == gm) // gnew did not change
      {
        cf = tryvcontent(gm, Variable(2), M, fail);
        if(fail)
          return;
        divides = true;
        tryDivide(gm,cf,M,g_image,divides,fail); // 'divides' is ignored
        if(fail)
          return;
        tryDivide(f,g_image,M,tmp,divides,fail); // trial division (f)
        if(fail)
          return;
        if(divides)
        {
          tryDivide(g,g_image,M,tmp,divides,fail); // trial division (g)
          if(fail)
            return;
          if(divides)
          {
            result = NN(c*g_image);
            return;
          }
        }
      }
      gm = gnew;
      continue;
    }

    if(isLess(L, dg_im, 2, mv)) // dg_im > L --> current point unlucky
      continue;

    // here: isLess(dg_im, L, 2, mv) --> all previous points were unlucky
    m = CanonicalForm(1); // reset
    gm = 0; // reset
    for(int i=2; i<=mv; i++) // tighten bound
      L[i] = dg_im[i];
  }
  // we are out of evaluation points
  fail = true;
}

CanonicalForm QGCD( const CanonicalForm & F, const CanonicalForm & G )
{ // f,g in Q(a)[x1,...,xn]
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
  // here: both NOT inCoeffDomain
  CanonicalForm f, g, tmp, M, q, D, Dp, cl, newq, mipo;
  int p, i;
  int *bound, *other; // degree vectors
  bool fail;
  bool off_rational=!isOn(SW_RATIONAL);
  On( SW_RATIONAL ); // needed by bCommonDen
  f = F * bCommonDen(F);
  g = G * bCommonDen(G);
  Variable a, b;
  if(hasFirstAlgVar(f,a))
  {
    if(hasFirstAlgVar(g,b))
    {
      if(b.level() > a.level())
        a = b;
    }
  }
  else
  {
    if(!hasFirstAlgVar(g,a))// both not in extension
    {
      Off( SW_RATIONAL );
      Off( SW_USE_QGCD );
      tmp = gcd( F, G );
      On( SW_USE_QGCD );
      if (off_rational) Off(SW_RATIONAL);
      return tmp;
    }
  }
  // here: a is the biggest alg. var in f and g AND some of f,g is in extension
  // (in the sequel b is used to swap alg/poly vars)
  setReduce(a,false); // do not reduce expressions modulo mipo
  tmp = getMipo(a);
  M = tmp * bCommonDen(tmp);
  // here: f, g in Z[a][x1,...,xn], M in Z[a] not necessarily monic
  Off( SW_RATIONAL ); // needed by mod
  // calculate upper bound for degree vector of gcd
  int mv = f.level(); i = g.level();
  if(i > mv)
    mv = i;
  // here: mv is level of the largest variable in f, g
  b = Variable(mv+1);
  bound = new int[mv+1]; // 'bound' could be indexed from 0 to mv, but we will only use from 1 to mv
  other = new int[mv+1];
  for(int i=1; i<=mv; i++) // initialize 'bound', 'other' with zeros
    bound[i] = other[i] = 0;
  bound = leadDeg(f,bound); // 'bound' is set the leading degree vector of f
  other = leadDeg(g,other);
  for(int i=1; i<=mv; i++) // entry at i=0 not visited
    if(other[i] < bound[i])
      bound[i] = other[i];
  // now 'bound' is the smaller vector
  cl = lc(M) * lc(f) * lc(g);
  q = 1;
  D = 0;
  for( i=cf_getNumBigPrimes()-1; i>-1; i-- )
  {
    p = cf_getBigPrime(i);
    if( mod( cl, p ).isZero() ) // skip lc-bad primes
      continue;
    fail = false;
    setCharacteristic(p);
    mipo = mapinto(M);
    mipo /= mipo.lc();
    // here: mipo is monic
    tryBrownGCD( mapinto(f), mapinto(g), mipo, Dp, fail );
    Dp = reduce( Dp, mipo );
    setCharacteristic(0);
    if( fail ) // mipo splits in char p
      continue;
    if( Dp.inCoeffDomain() ) // early termination
    {
      tryInvert(Dp,mipo,tmp,fail); // check if zero divisor
      if(fail)
        continue;
      setReduce(a,true);
      if (off_rational) Off(SW_RATIONAL); else On(SW_RATIONAL);
      return CanonicalForm(1);
    }
    // here: Dp NOT inCoeffDomain
    for(int i=1; i<=mv; i++)
      other[i] = 0; // reset (this is necessary, because some entries may not be updated by call to leadDeg)
    other = leadDeg(Dp,other);

    if(isEqual(bound, other, 1, mv)) // equal
    {
      chineseRemainder( D, q, replacevar( mapinto(Dp), a, b ), p, tmp, newq );
      // tmp = Dp mod p
      // tmp = D mod q
      // newq = p*q
      q = newq;
      if( D != tmp )
        D = tmp;
      On( SW_RATIONAL );
      tmp = replacevar( Farey( D, q ), b, a ); // Farey and switch back to alg var
      setReduce(a,true); // reduce expressions modulo mipo
      On( SW_RATIONAL ); // needed by fdivides
      if( fdivides( tmp, f ) && fdivides( tmp, g )) // trial division
      {
        Off( SW_RATIONAL );
        setReduce(a,true);
        if (off_rational) Off(SW_RATIONAL); else On(SW_RATIONAL);
        return tmp;
      }
      Off( SW_RATIONAL );
      setReduce(a,false); // do not reduce expressions modulo mipo
      continue;
    }
    if( isLess(bound, other, 1, mv) ) // current prime unlucky
      continue;
    // here: isLess(other, bound, 1, mv) ) ==> all previous primes unlucky
    q = p;
    D = replacevar( mapinto(Dp), a, b ); // shortcut CRA // shortcut CRA
    for(int i=1; i<=mv; i++) // tighten bound
      bound[i] = other[i];
  }
  // hopefully, we never reach this point
  setReduce(a,true);
  Off( SW_USE_QGCD );
  D = gcd( f, g );
  On( SW_USE_QGCD );
  if (off_rational) Off(SW_RATIONAL); else On(SW_RATIONAL);
  return D;
}


int * leadDeg(const CanonicalForm & f, int *degs)
{ // leading degree vector w.r.t. lex. monomial order x(i+1) > x(i)
  // if f is in a coeff domain, the zero pointer is returned
  // 'a' should point to an array of sufficient size level(f)+1
  if(f.inCoeffDomain())
    return 0;
  CanonicalForm tmp = f;
  do
  {
    degs[tmp.level()] = tmp.degree();
    tmp = LC(tmp);
  }
  while(!tmp.inCoeffDomain());
  return degs;
}


bool isLess(int *a, int *b, int lower, int upper)
{ // compares the degree vectors a,b on the specified part. Note: x(i+1) > x(i)
  for(int i=upper; i>=lower; i--)
    if(a[i] == b[i])
      continue;
    else
      return a[i] < b[i];
  return true;
}


bool isEqual(int *a, int *b, int lower, int upper)
{ // compares the degree vectors a,b on the specified part. Note: x(i+1) > x(i)
  for(int i=lower; i<=upper; i++)
    if(a[i] != b[i])
      return false;
  return true;
}


CanonicalForm firstLC(const CanonicalForm & f)
{ // returns the leading coefficient (LC) of level <= 1
  CanonicalForm ret = f;
  while(ret.level() > 1)
    ret = LC(ret);
  return ret;
}


void tryCRA( const CanonicalForm & x1, const CanonicalForm & q1, const CanonicalForm & x2, const CanonicalForm & q2, const CanonicalForm & M, CanonicalForm & xnew, CanonicalForm & qnew, bool & fail )
{ // as CRA, but takes care of zero divisors
  CanonicalForm tmp;
  if(x1.level() <= 1 && x2.level() <= 1) // base case
  {
    tryExtgcd(q1,q2,M,tmp,xnew,qnew,fail);
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
        tryCRA(i.coeff(),q1,x2,q2,M,tmp,tmp2,fail);
        if(fail)
          return;
        xnew += tmp;
      }
      else
      {
        tryCRA(i.coeff(),q1,0,q2,M,tmp,tmp2,fail);
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
        tryCRA(x1,q1,j.coeff(),q2,M,tmp,tmp2,fail);
        if(fail)
          return;
        xnew += tmp;
      }
      else
      {
        tryCRA(0,q1,j.coeff(),q2,M,tmp,tmp2,fail);
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
          tryCRA(i.coeff(),q1,j.coeff(),q2,M,tmp,tmp2,fail);
          if(fail)
            return;
          xnew += tmp * power(x1.mvar(),i.exp());
          i++; j++;
        }
        else
        {
          if(i.exp() < j.exp())
          {
            tryCRA(i.coeff(),q1,0,q2,M,tmp,tmp2,fail);
            if(fail)
              return;
            xnew += tmp * power(x1.mvar(),i.exp());
            i++;
          }
          else // i.exp() > j.exp()
          {
            tryCRA(0,q1,j.coeff(),q2,M,tmp,tmp2,fail);
            if(fail)
              return;
            xnew += tmp * power(x1.mvar(),j.exp());
            j++;
          }
        }
      }
      else // j is out of terms
      {
        tryCRA(i.coeff(),q1,0,q2,M,tmp,tmp2,fail);
        if(fail)
          return;
        xnew += tmp * power(x1.mvar(),i.exp());
        i++;
      }
    }
    else // i is out of terms
    {
      tryCRA(0,q1,j.coeff(),q2,M,tmp,tmp2,fail);
      if(fail)
        return;
      xnew += tmp * power(x1.mvar(),j.exp());
      j++;
    }
  }
}


void tryExtgcd( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm & M, CanonicalForm & result, CanonicalForm & s, CanonicalForm & t, bool & fail )
{ // F, G are univariate polynomials (i.e. they have exactly one polynomial variable)
  // F and G must have the same level AND level > 0
  // we try to calculate gcd(F,G) = s*F + t*G
  // if a zero divisor is encontered, 'fail' is set to one
  // M is assumed to be monic
  CanonicalForm P;
  if(F.inCoeffDomain())
  {
    tryInvert( F, M, P, fail );
    if(fail)
      return;
    result = 1;
    s = P; t = 0;
    return;
  }
  if(G.inCoeffDomain())
  {
    tryInvert( G, M, P, fail );
    if(fail)
      return;
    result = 1;
    s = 0; t = P;
    return;
  }
  // here: both not inCoeffDomain
  CanonicalForm inv, rem, tmp, u, v, q, sum=0;
  if( F.degree() > G.degree() )
  {
    P = F; result = G;  s=v=0; t=u=1;
  }
  else
  {
    P = G; result = F; s=v=1; t=u=0;
  }
  Variable x = P.mvar();
  // here: degree(P) >= degree(result)
  while(true)
  {
    tryInvert( Lc(result), M, inv, fail );
    if(fail)
      return;
    // here: Lc(result) is invertible modulo M
    q = Lc(P)*inv*power( x, P.degree(x)-result.degree(x) );
    rem = reduce( P - q*result, M );
    if( rem.isZero() )
    {
      s*=inv;
      t*=inv;
      result *= inv; // monify result
      return;
    }
    sum += q;
    if(result.degree(x) >= rem.degree(x))
    {
      P=result;
      result=rem;
      tmp=u-sum*s;
      u=s;
      s=tmp;
      tmp=v-sum*t;
      v=t;
      t=tmp;
      sum = 0; // reset
    }
    else
      P = rem;
  }
}


static CanonicalForm trycontent ( const CanonicalForm & f, const Variable & x, const CanonicalForm & M, bool & fail )
{ // as 'content', but takes care of zero divisors
  ASSERT( x.level() > 0, "cannot calculate content with respect to algebraic variable" );
  Variable y = f.mvar();
  if ( y == x )
    return trycf_content( f, 0, M, fail );
  if ( y < x )
     return f;
  return swapvar( trycontent( swapvar( f, y, x ), y, M, fail ), y, x );
}


static CanonicalForm tryvcontent ( const CanonicalForm & f, const Variable & x, const CanonicalForm & M, bool & fail )
{ // as vcontent, but takes care of zero divisors
  ASSERT( x.level() > 0, "cannot calculate vcontent with respect to algebraic variable" );
  if ( f.mvar() <= x )
    return trycontent( f, x, M, fail );
  CFIterator i;
  CanonicalForm d = 0, e, ret;
  for ( i = f; i.hasTerms() && ! d.isOne() && ! fail; i++ )
  {
    e = tryvcontent( i.coeff(), x, M, fail );
    if(fail)
      break;
    tryBrownGCD( d, e, M, ret, fail );
    d = ret;
  }
  return d;
}


static CanonicalForm trycf_content ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & M, bool & fail )
{ // as cf_content, but takes care of zero divisors
  if ( f.inPolyDomain() || ( f.inExtension() && ! getReduce( f.mvar() ) ) )
  {
    CFIterator i = f;
    CanonicalForm tmp = g, result;
    while ( i.hasTerms() && ! tmp.isOne() && ! fail )
    {
      tryBrownGCD( i.coeff(), tmp, M, result, fail );
      tmp = result;
      i++;
    }
    return result;
  }
  return abs( f );
}


static void tryDivide( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & M, CanonicalForm & result, bool & divides, bool & fail )
{ // M "univariate" monic polynomial
  // f, g polynomials with coeffs modulo M.
  // if f is divisible by g, 'divides' is set to 1 and 'result' == f/g mod M coefficientwise.
  // 'fail' is set to 1, iff a zero divisor is encountered.
  // divides==1 implies fail==0
  // required: getReduce(M.mvar())==0
  if(g.inBaseDomain())
  {
    result = f/g;
    divides = true;
    return;
  }
  if(g.inCoeffDomain())
  {
    tryInvert(g,M,result,fail);
    if(fail)
      return;
    result = reduce(f*result, M);
    divides = true;
    return;
  }
  // here: g NOT inCoeffDomain
  Variable x = g.mvar();
  if(f.degree(x) < g.degree(x))
  {
    divides = false;
    return;
  }
  // here: f.degree(x) > 0 and f.degree(x) >= g.degree(x)
  CanonicalForm F = f;
  CanonicalForm q, leadG = LC(g);
  result = 0;
  while(!F.isZero())
  {
    tryDivide(F.LC(x),leadG,M,q,divides,fail);
    if(fail || !divides)
      return;
    if(F.degree(x)<g.degree(x))
    {
      divides = false;
      return;
    }
    q *= power(x,F.degree(x)-g.degree(x));
    result += q;
    F = reduce(F-q*g, M);
  }
  result = reduce(result, M);
  divides = true;
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
