/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_gcd.cc,v 1.25 2004-10-14 11:36:15 Singular Exp $ */

#include <config.h>

#include "assert.h"
#include "debug.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_reval.h"
#include "cf_primes.h"
#include "cf_algorithm.h"
#include "cf_map.h"
#include "sm_sparsemod.h"
#include "fac_util.h"
#include "ftmpl_functions.h"

#ifdef HAVE_NTL
#include <NTL/ZZX.h>
#include "NTLconvert.h"
bool isPurePoly(const CanonicalForm & f);
#endif

static CanonicalForm gcd_poly( const CanonicalForm & f, const CanonicalForm& g, bool modularflag );

bool
gcd_test_one ( const CanonicalForm & f, const CanonicalForm & g, bool swap )
{
    int count = 0;
    // assume polys have same level;
    CFRandom * sample = CFRandomFactory::generate();
    REvaluation e( 2, tmax( f.level(), g.level() ), *sample );
    delete sample;
    CanonicalForm lcf, lcg;
    if ( swap ) {
        lcf = swapvar( LC( f ), Variable(1), f.mvar() );
        lcg = swapvar( LC( g ), Variable(1), f.mvar() );
    }
    else {
        lcf = LC( f, Variable(1) );
        lcg = LC( g, Variable(1) );
    }
    while ( ( e( lcf ).isZero() || e( lcg ).isZero() ) && count < 100 ) {
        e.nextpoint();
        count++;
    }
    if ( count == 100 )
        return false;
    CanonicalForm F, G;
    if ( swap ) {
        F=swapvar( f, Variable(1), f.mvar() );
        G=swapvar( g, Variable(1), g.mvar() );
    }
    else {
        F = f;
        G = g;
    }
    if ( e(F).taildegree() > 0 && e(G).taildegree() > 0 )
        return false;
    return gcd( e( F ), e( G ) ).degree() < 1;
}

//{{{ static CanonicalForm balance ( const CanonicalForm & f, const CanonicalForm & q )
//{{{ docu
//
// balance() - map f from positive to symmetric representation
//   mod q.
//
// This makes sense for univariate polynomials over Z only.
// q should be an integer.
//
// Used by gcd_poly_univar0().
//
//}}}
static CanonicalForm
balance ( const CanonicalForm & f, const CanonicalForm & q )
{
    Variable x = f.mvar();
    CanonicalForm result = 0, qh = q / 2;
    CanonicalForm c;
    CFIterator i;
    for ( i = f; i.hasTerms(); i++ ) {
        c = mod( i.coeff(), q );
        if ( c > qh )
            result += power( x, i.exp() ) * (c - q);
        else
            result += power( x, i.exp() ) * c;
    }
    return result;
}
//}}}

//{{{ static CanonicalForm icontent ( const CanonicalForm & f, const CanonicalForm & c )
//{{{ docu
//
// icontent() - return gcd of c and all coefficients of f which
//   are in a coefficient domain.
//
// Used by icontent().
//
//}}}
static CanonicalForm
icontent ( const CanonicalForm & f, const CanonicalForm & c )
{
    if ( f.inCoeffDomain() )
        return gcd( f, c );
    else {
        CanonicalForm g = c;
        for ( CFIterator i = f; i.hasTerms() && ! g.isOne(); i++ )
            g = icontent( i.coeff(), g );
        return g;
    }
}
//}}}

//{{{ CanonicalForm icontent ( const CanonicalForm & f )
//{{{ docu
//
// icontent() - return gcd over all coefficients of f which are
//   in a coefficient domain.
//
//}}}
CanonicalForm
icontent ( const CanonicalForm & f )
{
    return icontent( f, 0 );
}
//}}}

//{{{ CanonicalForm extgcd ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & a, CanonicalForm & b )
//{{{ docu
//
// extgcd() - returns polynomial extended gcd of f and g.
//
// Returns gcd(f, g) and a and b sucht that f*a+g*b=gcd(f, g).
// The gcd is calculated using an extended euclidean polynomial
// remainder sequence, so f and g should be polynomials over an
// euclidean domain.  Normalizes result.
//
// Note: be sure that f and g have the same level!
//
//}}}
CanonicalForm
extgcd ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & a, CanonicalForm & b )
{
#ifdef HAVE_NTL
  if (isOn(SW_USE_NTL_GCD) && ( getCharacteristic() > 0 )
  && isPurePoly(f) && isPurePoly(g))
  {
    zz_pContext ccc(getCharacteristic());
    ccc.restore();
    zz_p::init(getCharacteristic());
    zz_pX F1=convertFacCF2NTLzzpX(f);
    zz_pX G1=convertFacCF2NTLzzpX(g);
    zz_pX R;
    zz_pX A,B;
    XGCD(R,A,B,F1,G1);
    a=convertNTLzzpX2CF(A,f.mvar());
    b=convertNTLzzpX2CF(B,f.mvar());
    return convertNTLzzpX2CF(R,f.mvar());
  }
#endif
  CanonicalForm contf = content( f );
  CanonicalForm contg = content( g );

  CanonicalForm p0 = f / contf, p1 = g / contg;
  CanonicalForm f0 = 1, f1 = 0, g0 = 0, g1 = 1, q, r;

  while ( ! p1.isZero() ) {
      divrem( p0, p1, q, r );
      p0 = p1; p1 = r;
      r = g0 - g1 * q;
      g0 = g1; g1 = r;
      r = f0 - f1 * q;
      f0 = f1; f1 = r;
  }
  CanonicalForm contp0 = content( p0 );
  a = f0 / ( contf * contp0 );
  b = g0 / ( contg * contp0 );
  p0 /= contp0;
  if ( p0.sign() < 0 ) {
      p0 = -p0;
      a = -a;
      b = -b;
  }
  return p0;
}
//}}}

static CanonicalForm
gcd_poly_univar0( const CanonicalForm & F, const CanonicalForm & G, bool primitive )
{
#ifdef HAVE_NTL
  if (isOn(SW_USE_NTL_GCD) && isPurePoly(F) && isPurePoly(G))
  {
    if ( getCharacteristic() > 0 )
    {
      zz_pContext ccc(getCharacteristic());
      ccc.restore();
      zz_p::init(getCharacteristic());
      zz_pX F1=convertFacCF2NTLzzpX(F);
      zz_pX G1=convertFacCF2NTLzzpX(G);
      zz_pX R=GCD(F1,G1);
      return convertNTLzzpX2CF(R,F.mvar());
    }
    else
    {
      ZZX F1=convertFacCF2NTLZZX(F);
      ZZX G1=convertFacCF2NTLZZX(G);
      ZZX R=GCD(F1,G1);
      return convertNTLZZX2CF(R,F.mvar());
    }
  }
#endif
  CanonicalForm f, g, c, cg, cl, BB, B, M, q, Dp, newD, D, newq;
  int p, i, n;

  if ( primitive )
  {
    f = F;
    g = G;
    c = 1;
  }
  else
  {
    CanonicalForm cF = content( F ), cG = content( G );
    f = F / cF;
    g = G / cG;
    c = bgcd( cF, cG );
  }
  cg = gcd( f.lc(), g.lc() );
  cl = ( f.lc() / cg ) * g.lc();
//     B = 2 * cg * tmin(
//         maxnorm(f)*power(CanonicalForm(2),f.degree())*isqrt(f.degree()+1),
//         maxnorm(g)*power(CanonicalForm(2),g.degree())*isqrt(g.degree()+1)
//         )+1;
  M = tmin( maxNorm(f), maxNorm(g) );
  BB = power(CanonicalForm(2),tmin(f.degree(),g.degree()))*M;
  q = 0;
  i = cf_getNumSmallPrimes() - 1;
  while ( true )
  {
    B = BB;
    while ( i >= 0 && q < B )
    {
      p = cf_getSmallPrime( i );
      i--;
      while ( i >= 0 && mod( cl, p ) == 0 )
      {
        p = cf_getSmallPrime( i );
        i--;
      }
      setCharacteristic( p );
      Dp = gcd( mapinto( f ), mapinto( g ) );
      Dp = ( Dp / Dp.lc() ) * mapinto( cg );
      setCharacteristic( 0 );
      if ( Dp.degree() == 0 )
        return c;
      if ( q.isZero() )
      {
        D = mapinto( Dp );
        q = p;
        B = power(CanonicalForm(2),D.degree())*M+1;
      }
      else
      {
        if ( Dp.degree() == D.degree() )
        {
          chineseRemainder( D, q, mapinto( Dp ), p, newD, newq );
          q = newq;
          D = newD;
        }
        else if ( Dp.degree() < D.degree() )
        {
          // all previous p's are bad primes
          q = p;
          D = mapinto( Dp );
          B = power(CanonicalForm(2),D.degree())*M+1;
        }
        // else p is a bad prime
      }
    }
    if ( i >= 0 )
    {
      // now balance D mod q
      D = pp( balance( D, q ) );
      if ( divides( D, f ) && divides( D, g ) )
        return D * c;
      else
        q = 0;
    }
    else
      return gcd_poly( F, G, false );
    DEBOUTLN( cerr, "another try ..." );
  }
}

static CanonicalForm
gcd_poly1( const CanonicalForm & f, const CanonicalForm & g, bool modularflag )
{
    CanonicalForm C, Ci, Ci1, Hi, bi, pi, pi1, pi2;
    int delta;
    Variable v = f.mvar();

    if ( f.degree( v ) >= g.degree( v ) ) {
        pi = f; pi1 = g;
    }
    else {
        pi = g; pi1 = f;
    }
    Ci = content( pi ); Ci1 = content( pi1 );
    C = gcd( Ci, Ci1 );
    pi1 = pi1 / Ci1; pi = pi / Ci;
    if ( pi.isUnivariate() && pi1.isUnivariate() )
    {
      if ( modularflag
#ifdef HAVE_NTL
      || (isOn(SW_USE_NTL_GCD) && isPurePoly(pi) && isPurePoly(pi1))
#endif
      )
        return gcd_poly_univar0( pi, pi1, true ) * C;
    }
    else if ( gcd_test_one( pi1, pi, true ) )
      return C;
    delta = degree( pi, v ) - degree( pi1, v );
    Hi = power( LC( pi1, v ), delta );
    if ( (delta+1) % 2 )
        bi = 1;
    else
        bi = -1;
    while ( degree( pi1, v ) > 0 ) {
        pi2 = psr( pi, pi1, v );
        pi2 = pi2 / bi;
        pi = pi1; pi1 = pi2;
        if ( degree( pi1, v ) > 0 ) {
            delta = degree( pi, v ) - degree( pi1, v );
            if ( (delta+1) % 2 )
                bi = LC( pi, v ) * power( Hi, delta );
            else
                bi = -LC( pi, v ) * power( Hi, delta );
            Hi = power( LC( pi1, v ), delta ) / power( Hi, delta-1 );
        }
    }
    if ( degree( pi1, v ) == 0 )
        return C;
    else {
        return C * pp( pi );
    }
}

//{{{ static CanonicalForm gcd_poly ( const CanonicalForm & f, const CanonicalForm & g, bool modularflag )
//{{{ docu
//
// gcd_poly() - calculate polynomial gcd.
//
// This is the dispatcher for polynomial gcd calculation.  We call either
// ezgcd(), sparsemod() or gcd_poly1() in dependecy on the current
// characteristic and settings of SW_USE_EZGCD and SW_USE_SPARSEMOD, resp.
//
// modularflag is reached down to gcd_poly1() without change in case of
// zero characteristic.
//
// Used by gcd() and gcd_poly_univar0().
//
//}}}
static CanonicalForm
gcd_poly ( const CanonicalForm & f, const CanonicalForm & g, bool modularflag )
{
    if ( getCharacteristic() != 0 ) {
        return gcd_poly1( f, g, false );
    }
    else if ( isOn( SW_USE_EZGCD ) && ! ( f.isUnivariate() && g.isUnivariate() ) ) {
        CFMap M, N;
        compress( f, g, M, N );
        CanonicalForm r=N( ezgcd( M(f), M(g) ) );
        if ((f%r!=0) || (g % r !=0)) return gcd_poly1( f, g, modularflag);
        else return r;
    }
    else if ( isOn( SW_USE_SPARSEMOD )
    && ! ( f.isUnivariate() && g.isUnivariate() ) )
    {
        CanonicalForm r=sparsemod( f, g );
        if ((f%r!=0) || (g % r !=0)) return gcd_poly1( f, g, modularflag);
        else return r;
    }
    else
    {
        return gcd_poly1( f, g, modularflag );
    }
}
//}}}

//{{{ static CanonicalForm cf_content ( const CanonicalForm & f, const CanonicalForm & g )
//{{{ docu
//
// cf_content() - return gcd(g, content(f)).
//
// content(f) is calculated with respect to f's main variable.
//
// Used by gcd(), content(), content( CF, Variable ).
//
//}}}
static CanonicalForm
cf_content ( const CanonicalForm & f, const CanonicalForm & g )
{
    if ( f.inPolyDomain() || ( f.inExtension() && ! getReduce( f.mvar() ) ) ) {
        CFIterator i = f;
        CanonicalForm result = g;
        while ( i.hasTerms() && ! result.isOne() ) {
            result = gcd( result, i.coeff() );
            i++;
        }
        return result;
    }
    else
        if ( f.sign() < 0 )
            return -f;
        else
            return f;
}
//}}}

//{{{ CanonicalForm content ( const CanonicalForm & f )
//{{{ docu
//
// content() - return content(f) with respect to main variable.
//
// Normalizes result.
//
//}}}
CanonicalForm
content ( const CanonicalForm & f )
{
    return cf_content( f, 0 );
}
//}}}

//{{{ CanonicalForm content ( const CanonicalForm & f, const Variable & x )
//{{{ docu
//
// content() - return content(f) with respect to x.
//
// x should be a polynomial variable.
//
//}}}
CanonicalForm
content ( const CanonicalForm & f, const Variable & x )
{
    ASSERT( x.level() > 0, "cannot calculate content with respect to algebraic variable" );
    Variable y = f.mvar();

    if ( y == x )
        return cf_content( f, 0 );
    else  if ( y < x )
        return f;
    else
        return swapvar( content( swapvar( f, y, x ), y ), y, x );
}
//}}}

//{{{ CanonicalForm vcontent ( const CanonicalForm & f, const Variable & x )
//{{{ docu
//
// vcontent() - return content of f with repect to variables >= x.
//
// The content is recursively calculated over all coefficients in
// f having level less than x.  x should be a polynomial
// variable.
//
//}}}
CanonicalForm
vcontent ( const CanonicalForm & f, const Variable & x )
{
    ASSERT( x.level() > 0, "cannot calculate vcontent with respect to algebraic variable" );

    if ( f.mvar() <= x )
        return content( f, x );
    else {
        CFIterator i;
        CanonicalForm d = 0;
        for ( i = f; i.hasTerms() && ! d.isOne(); i++ )
            d = gcd( d, vcontent( i.coeff(), x ) );
        return d;
    }
}
//}}}

//{{{ CanonicalForm pp ( const CanonicalForm & f )
//{{{ docu
//
// pp() - return primitive part of f.
//
// Returns zero if f equals zero, otherwise f / content(f).
//
//}}}
CanonicalForm
pp ( const CanonicalForm & f )
{
    if ( f.isZero() )
        return f;
    else
        return f / content( f );
}
//}}}

CanonicalForm
gcd ( const CanonicalForm & f, const CanonicalForm & g )
{
    if ( f.isZero() )
        if ( g.lc().sign() < 0 )
            return -g;
        else
            return g;
    else  if ( g.isZero() )
        if ( f.lc().sign() < 0 )
            return -f;
        else
            return f;
    else  if ( f.inBaseDomain() )
        if ( g.inBaseDomain() )
            return bgcd( f, g );
        else
            return cf_content( g, f );
    else  if ( g.inBaseDomain() )
        return cf_content( f, g );
    else  if ( f.mvar() == g.mvar() )
        if ( f.inExtension() && getReduce( f.mvar() ) )
            return 1;
        else {
            if ( divides( f, g ) )
                if ( f.lc().sign() < 0 )
                    return -f;
                else
                    return f;
            else  if ( divides( g, f ) )
                if ( g.lc().sign() < 0 )
                    return -g;
                else
                    return g;
            if ( getCharacteristic() == 0 && isOn( SW_RATIONAL ) ) {
                CanonicalForm cdF = bCommonDen( f );
                CanonicalForm cdG = bCommonDen( g );
                Off( SW_RATIONAL );
                CanonicalForm l = lcm( cdF, cdG );
                On( SW_RATIONAL );
                CanonicalForm F = f * l, G = g * l;
                Off( SW_RATIONAL );
                l = gcd_poly( F, G, true );
                //if ((F%l!=0) || (G % l !=0))
                //  l = gcd_poly( F, G, true );
                On( SW_RATIONAL );
                if ( l.lc().sign() < 0 )
                    return -l;
                else
                    return l;
            }
            else {
                CanonicalForm d = gcd_poly( f, g, getCharacteristic()==0 );
                //if ((f%d!=0) || (g % d !=0))
                //  d = gcd_poly( f, g, getCharacteristic()==0  );
                if ( d.lc().sign() < 0 )
                    return -d;
                else
                    return d;
            }
        }
    else  if ( f.mvar() > g.mvar() )
        return cf_content( f, g );
    else
        return cf_content( g, f );
}

//{{{ CanonicalForm lcm ( const CanonicalForm & f, const CanonicalForm & g )
//{{{ docu
//
// lcm() - return least common multiple of f and g.
//
// The lcm is calculated using the formula lcm(f, g) = f * g / gcd(f, g).
//
// Returns zero if one of f or g equals zero.
//
//}}}
CanonicalForm
lcm ( const CanonicalForm & f, const CanonicalForm & g )
{
    if ( f.isZero() || g.isZero() )
        return f;
    else
        return ( f / gcd( f, g ) ) * g;
}
//}}}
