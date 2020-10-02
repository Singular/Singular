/* emacs edit mode for this file is -*- C++ -*- */

/**
 *
 *
 * cf_algorithm.cc - simple mathematical algorithms.
 *
 * Hierarchy: mathematical algorithms on canonical forms
 *
 * Developers note:
 * ----------------
 * A "mathematical" algorithm is an algorithm which calculates
 * some mathematical function in contrast to a "structural"
 * algorithm which gives structural information on polynomials.
 *
 * Compare these functions to the functions in `cf_ops.cc', which
 * are structural algorithms.
 *
**/


#include "config.h"


#include "cf_assert.h"

#include "cf_factory.h"
#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_algorithm.h"
#include "variable.h"
#include "cf_iter.h"
#include "templates/ftmpl_functions.h"
#include "cfGcdAlgExt.h"

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

/** CanonicalForm psr ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
 *
 *
 * psr() - return pseudo remainder of `f' and `g' with respect
 *   to `x'.
 *
 * `g' must not equal zero.
 *
 * For f and g in R[x], R an arbitrary ring, g != 0, there is a
 * representation
 *
 *   LC(g)^s*f = g*q + r
 *
 * with r = 0 or deg(r) < deg(g) and s = 0 if f = 0 or
 * s = max( 0, deg(f)-deg(g)+1 ) otherwise.
 * r = psr(f, g) and q = psq(f, g) are called "pseudo remainder"
 * and "pseudo quotient", resp.  They are uniquely determined if
 * LC(g) is not a zero divisor in R.
 *
 * See H.-J. Reiffen/G. Scheja/U. Vetter - "Algebra", 2nd ed.,
 * par. 15, for a reference.
 *
 * Type info:
 * ----------
 * f, g: Current
 * x: Polynomial
 *
 * Polynomials over prime power domains are admissible if
 * lc(LC(`g',`x')) is not a zero divisor.  This is a slightly
 * stronger precondition than mathematically necessary since
 * pseudo remainder and quotient are well-defined if LC(`g',`x')
 * is not a zero divisor.
 *
 * For example, psr(y^2, (13*x+1)*y) is well-defined in
 * (Z/13^2[x])[y] since (13*x+1) is not a zero divisor.  But
 * calculating it with Factory would fail since 13 is a zero
 * divisor in Z/13^2.
 *
 * Due to this inconsistency with mathematical notion, we decided
 * not to declare type `CurrentPP' for `f' and `g'.
 *
 * Developers note:
 * ----------------
 * This is not an optimal implementation.  Better would have been
 * an implementation in `InternalPoly' avoiding the
 * exponentiation of the leading coefficient of `g'.  In contrast
 * to `psq()' and `psqr()' it definitely seems worth to implement
 * the pseudo remainder on the internal level.
 *
 * @sa psq(), psqr()
**/
CanonicalForm
#if 0
psr ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
{

    ASSERT( x.level() > 0, "type error: polynomial variable expected" );
    ASSERT( ! g.isZero(), "math error: division by zero" );

    // swap variables such that x's level is larger or equal
    // than both f's and g's levels.
    Variable X = tmax( tmax( f.mvar(), g.mvar() ), x );
    CanonicalForm F = swapvar( f, x, X );
    CanonicalForm G = swapvar( g, x, X );

    // now, we have to calculate the pseudo remainder of F and G
    // w.r.t. X
    int fDegree = degree( F, X );
    int gDegree = degree( G, X );
    if ( (fDegree < 0) || (fDegree < gDegree) )
        return f;
    else
    {
        CanonicalForm xresult = (power( LC( G, X ), fDegree-gDegree+1 ) * F) ;
        CanonicalForm result = xresult -(xresult/G)*G;
        return swapvar( result, x, X );
    }
}
#else
psr ( const CanonicalForm &rr, const CanonicalForm &vv, const Variable & x )
{
  CanonicalForm r=rr, v=vv, l, test, lu, lv, t, retvalue;
  int dr, dv, d,n=0;


  dr = degree( r, x );
  if (dr>0)
  {
    dv = degree( v, x );
    if (dv <= dr) {l=LC(v,x); v = v -l*power(x,dv);}
    else { l = 1; }
    d= dr-dv+1;
    //out_cf("psr(",rr," ");
    //out_cf("",vv," ");
    //printf(" var=%d\n",x.level());
    while ( ( dv <= dr  ) && ( !r.isZero()) )
    {
      test = power(x,dr-dv)*v*LC(r,x);
      if ( dr == 0 ) { r= CanonicalForm(0); }
      else { r= r - LC(r,x)*power(x,dr); }
      r= l*r -test;
      dr= degree(r,x);
      n+=1;
    }
    r= power(l, d-n)*r;
  }
  return r;
}
#endif

/** CanonicalForm psq ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
 *
 *
 * psq() - return pseudo quotient of `f' and `g' with respect
 *   to `x'.
 *
 * `g' must not equal zero.
 *
 * Type info:
 * ----------
 * f, g: Current
 * x: Polynomial
 *
 * Developers note:
 * ----------------
 * This is not an optimal implementation.  Better would have been
 * an implementation in `InternalPoly' avoiding the
 * exponentiation of the leading coefficient of `g'.  It seemed
 * not worth to do so.
 *
 * @sa psr(), psqr()
 *
**/
CanonicalForm
psq ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x )
{
    ASSERT( x.level() > 0, "type error: polynomial variable expected" );
    ASSERT( ! g.isZero(), "math error: division by zero" );

    // swap variables such that x's level is larger or equal
    // than both f's and g's levels.
    Variable X = tmax( tmax( f.mvar(), g.mvar() ), x );
    CanonicalForm F = swapvar( f, x, X );
    CanonicalForm G = swapvar( g, x, X );

    // now, we have to calculate the pseudo remainder of F and G
    // w.r.t. X
    int fDegree = degree( F, X );
    int gDegree = degree( G, X );
    if ( fDegree < 0 || fDegree < gDegree )
        return 0;
    else {
        CanonicalForm result = (power( LC( G, X ), fDegree-gDegree+1 ) * F) / G;
        return swapvar( result, x, X );
    }
}

/** void psqr ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q, CanonicalForm & r, const Variable & x )
 *
 *
 * psqr() - calculate pseudo quotient and remainder of `f' and
 *   `g' with respect to `x'.
 *
 * Returns the pseudo quotient of `f' and `g' in `q', the pseudo
 * remainder in `r'.  `g' must not equal zero.
 *
 * See `psr()' for more detailed information.
 *
 * Type info:
 * ----------
 * f, g: Current
 * q, r: Anything
 * x: Polynomial
 *
 * Developers note:
 * ----------------
 * This is not an optimal implementation.  Better would have been
 * an implementation in `InternalPoly' avoiding the
 * exponentiation of the leading coefficient of `g'.  It seemed
 * not worth to do so.
 *
 * @sa psr(), psq()
 *
**/
void
psqr ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q, CanonicalForm & r, const Variable& x )
{
    ASSERT( x.level() > 0, "type error: polynomial variable expected" );
    ASSERT( ! g.isZero(), "math error: division by zero" );

    // swap variables such that x's level is larger or equal
    // than both f's and g's levels.
    Variable X = tmax( tmax( f.mvar(), g.mvar() ), x );
    CanonicalForm F = swapvar( f, x, X );
    CanonicalForm G = swapvar( g, x, X );

    // now, we have to calculate the pseudo remainder of F and G
    // w.r.t. X
    int fDegree = degree( F, X );
    int gDegree = degree( G, X );
    if ( fDegree < 0 || fDegree < gDegree ) {
        q = 0; r = f;
    } else {
        divrem( power( LC( G, X ), fDegree-gDegree+1 ) * F, G, q, r );
        q = swapvar( q, x, X );
        r = swapvar( r, x, X );
    }
}

/** static CanonicalForm internalBCommonDen ( const CanonicalForm & f )
 *
 *
 * internalBCommonDen() - recursively calculate multivariate
 *   common denominator of coefficients of `f'.
 *
 * Used by: bCommonDen()
 *
 * Type info:
 * ----------
 * f: Poly( Q )
 * Switches: isOff( SW_RATIONAL )
 *
**/
static CanonicalForm
internalBCommonDen ( const CanonicalForm & f )
{
    if ( f.inBaseDomain() )
        return f.den();
    else {
        CanonicalForm result = 1;
        for ( CFIterator i = f; i.hasTerms(); i++ )
            result = blcm( result, internalBCommonDen( i.coeff() ) );
        return result;
    }
}

/** CanonicalForm bCommonDen ( const CanonicalForm & f )
 *
 *
 * bCommonDen() - calculate multivariate common denominator of
 *   coefficients of `f'.
 *
 * The common denominator is calculated with respect to all
 * coefficients of `f' which are in a base domain.  In other
 * words, common_den( `f' ) * `f' is guaranteed to have integer
 * coefficients only.  The common denominator of zero is one.
 *
 * Returns something non-trivial iff the current domain is Q.
 *
 * Type info:
 * ----------
 * f: CurrentPP
 *
**/
CanonicalForm
bCommonDen ( const CanonicalForm & f )
{
    if ( getCharacteristic() == 0 && isOn( SW_RATIONAL ) )
    {
        // otherwise `bgcd()' returns one
        Off( SW_RATIONAL );
        CanonicalForm result = internalBCommonDen( f );
        On( SW_RATIONAL );
        return result;
    }
    else
        return CanonicalForm( 1 );
}

/** bool fdivides ( const CanonicalForm & f, const CanonicalForm & g )
 *
 *
 * fdivides() - check whether `f' divides `g'.
 *
 * Returns true iff `f' divides `g'.  Uses some extra heuristic
 * to avoid polynomial division.  Without the heuristic, the test
 * essentialy looks like `divremt(g, f, q, r) && r.isZero()'.
 *
 * Type info:
 * ----------
 * f, g: Current
 *
 * Elements from prime power domains (or polynomials over such
 * domains) are admissible if `f' (or lc(`f'), resp.) is not a
 * zero divisor.  This is a slightly stronger precondition than
 * mathematically necessary since divisibility is a well-defined
 * notion in arbitrary rings.  Hence, we decided not to declare
 * the weaker type `CurrentPP'.
 *
 * Developers note:
 * ----------------
 * One may consider the the test `fdivides( f.LC(), g.LC() )' in
 * the main `if'-test superfluous since `divremt()' in the
 * `if'-body repeats the test.  However, `divremt()' does not use
 * any heuristic to do so.
 *
 * It seems not reasonable to call `fdivides()' from `divremt()'
 * to check divisibility of leading coefficients.  `fdivides()' is
 * on a relatively high level compared to `divremt()'.
 *
**/
bool
fdivides ( const CanonicalForm & f, const CanonicalForm & g )
{
    // trivial cases
    if ( g.isZero() )
        return true;
    else if ( f.isZero() )
        return false;

    if ( (f.inCoeffDomain() || g.inCoeffDomain())
         && ((getCharacteristic() == 0 && isOn( SW_RATIONAL ))
             || (getCharacteristic() > 0) ))
    {
        // if we are in a field all elements not equal to zero are units
        if ( f.inCoeffDomain() )
            return true;
        else
            // g.inCoeffDomain()
            return false;
    }

    // we may assume now that both levels either equal LEVELBASE
    // or are greater zero
    int fLevel = f.level();
    int gLevel = g.level();
    if ( (gLevel > 0) && (fLevel == gLevel) )
        // f and g are polynomials in the same main variable
        if ( degree( f ) <= degree( g )
             && fdivides( f.tailcoeff(), g.tailcoeff() )
             && fdivides( f.LC(), g.LC() ) )
        {
            CanonicalForm q, r;
            return divremt( g, f, q, r ) && r.isZero();
        }
        else
            return false;
    else if ( gLevel < fLevel )
        // g is a coefficient w.r.t. f
        return false;
    else
    {
        // either f is a coefficient w.r.t. polynomial g or both
        // f and g are from a base domain (should be Z or Z/p^n,
        // then)
        CanonicalForm q, r;
        return divremt( g, f, q, r ) && r.isZero();
    }
}

/// same as fdivides if true returns quotient quot of g by f otherwise quot == 0
bool
fdivides ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm& quot )
{
    quot= 0;
    // trivial cases
    if ( g.isZero() )
        return true;
    else if ( f.isZero() )
        return false;

    if ( (f.inCoeffDomain() || g.inCoeffDomain())
         && ((getCharacteristic() == 0 && isOn( SW_RATIONAL ))
             || (getCharacteristic() > 0) ))
    {
        // if we are in a field all elements not equal to zero are units
        if ( f.inCoeffDomain() )
        {
            quot= g/f;
            return true;
        }
        else
            // g.inCoeffDomain()
            return false;
    }

    // we may assume now that both levels either equal LEVELBASE
    // or are greater zero
    int fLevel = f.level();
    int gLevel = g.level();
    if ( (gLevel > 0) && (fLevel == gLevel) )
        // f and g are polynomials in the same main variable
        if ( degree( f ) <= degree( g )
             && fdivides( f.tailcoeff(), g.tailcoeff() )
             && fdivides( f.LC(), g.LC() ) )
        {
            CanonicalForm q, r;
            if (divremt( g, f, q, r ) && r.isZero())
            {
              quot= q;
              return true;
            }
            else
              return false;
        }
        else
            return false;
    else if ( gLevel < fLevel )
        // g is a coefficient w.r.t. f
        return false;
    else
    {
        // either f is a coefficient w.r.t. polynomial g or both
        // f and g are from a base domain (should be Z or Z/p^n,
        // then)
        CanonicalForm q, r;
        if (divremt( g, f, q, r ) && r.isZero())
        {
          quot= q;
          return true;
        }
        else
          return false;
    }
}

/// same as fdivides but handles zero divisors in Z_p[t]/(f)[x1,...,xn] for reducible f
bool
tryFdivides ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm& M, bool& fail )
{
    fail= false;
    // trivial cases
    if ( g.isZero() )
        return true;
    else if ( f.isZero() )
        return false;

    if (f.inCoeffDomain() || g.inCoeffDomain())
    {
        // if we are in a field all elements not equal to zero are units
        if ( f.inCoeffDomain() )
        {
            CanonicalForm inv;
            tryInvert (f, M, inv, fail);
            return !fail;
        }
        else
        {
            return false;
        }
    }

    // we may assume now that both levels either equal LEVELBASE
    // or are greater zero
    int fLevel = f.level();
    int gLevel = g.level();
    if ( (gLevel > 0) && (fLevel == gLevel) )
    {
        if (degree( f ) > degree( g ))
          return false;
        bool dividestail= tryFdivides (f.tailcoeff(), g.tailcoeff(), M, fail);

        if (fail || !dividestail)
          return false;
        bool dividesLC= tryFdivides (f.LC(),g.LC(), M, fail);
        if (fail || !dividesLC)
          return false;
        CanonicalForm q,r;
        bool divides= tryDivremt (g, f, q, r, M, fail);
        if (fail || !divides)
          return false;
        return r.isZero();
    }
    else if ( gLevel < fLevel )
    {
        // g is a coefficient w.r.t. f
        return false;
    }
    else
    {
        // either f is a coefficient w.r.t. polynomial g or both
        // f and g are from a base domain (should be Z or Z/p^n,
        // then)
        CanonicalForm q, r;
        bool divides= tryDivremt (g, f, q, r, M, fail);
        if (fail || !divides)
          return false;
        return r.isZero();
    }
}

/** CanonicalForm maxNorm ( const CanonicalForm & f )
 *
 *
 * maxNorm() - return maximum norm of `f'.
 *
 * That is, the base coefficient of `f' with the largest absolute
 * value.
 *
 * Valid for arbitrary polynomials over arbitrary domains, but
 * most useful for multivariate polynomials over Z.
 *
 * Type info:
 * ----------
 * f: CurrentPP
 *
**/
CanonicalForm
maxNorm ( const CanonicalForm & f )
{
    if ( f.inBaseDomain() )
        return abs( f );
    else {
        CanonicalForm result = 0;
        for ( CFIterator i = f; i.hasTerms(); i++ ) {
            CanonicalForm coeffMaxNorm = maxNorm( i.coeff() );
            if ( coeffMaxNorm > result )
                result = coeffMaxNorm;
        }
        return result;
    }
}

/** CanonicalForm euclideanNorm ( const CanonicalForm & f )
 *
 *
 * euclideanNorm() - return Euclidean norm of `f'.
 *
 * Returns the largest integer smaller or equal norm(`f') =
 * sqrt(sum( `f'[i]^2 )).
 *
 * Type info:
 * ----------
 * f: UVPoly( Z )
 *
**/
CanonicalForm
euclideanNorm ( const CanonicalForm & f )
{
    ASSERT( (f.inBaseDomain() || f.isUnivariate()) && f.LC().inZ(),
            "type error: univariate poly over Z expected" );

    CanonicalForm result = 0;
    for ( CFIterator i = f; i.hasTerms(); i++ ) {
        CanonicalForm coeff = i.coeff();
        result += coeff*coeff;
    }
    return sqrt( result );
}
