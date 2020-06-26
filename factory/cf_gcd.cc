/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file cf_gcd.cc
 *
 * gcd/content/lcm of polynomials
 *
 * To compute the GCD different variants are chosen automatically
**/

#include "config.h"


#include "timing.h"
#include "cf_assert.h"
#include "debug.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_reval.h"
#include "cf_primes.h"
#include "cf_algorithm.h"
#include "cfEzgcd.h"
#include "cfGcdAlgExt.h"
#include "cfSubResGcd.h"
#include "cfModGcd.h"
#include "FLINTconvert.h"
#include "facAlgFuncUtil.h"

#ifdef HAVE_NTL
#include <NTL/ZZX.h>
#include "NTLconvert.h"
bool isPurePoly(const CanonicalForm & );
#endif

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

/** static CanonicalForm icontent ( const CanonicalForm & f, const CanonicalForm & c )
 *
 * icontent() - return gcd of c and all coefficients of f which
 *   are in a coefficient domain.
 *
 * @sa icontent().
 *
**/
static CanonicalForm
icontent ( const CanonicalForm & f, const CanonicalForm & c )
{
    if ( f.inBaseDomain() )
    {
      if (c.isZero()) return abs(f);
      return bgcd( f, c );
    }
    //else if ( f.inCoeffDomain() )
    //   return gcd(f,c);
    else
    {
        CanonicalForm g = c;
        for ( CFIterator i = f; i.hasTerms() && ! g.isOne(); i++ )
            g = icontent( i.coeff(), g );
        return g;
    }
}

/** CanonicalForm icontent ( const CanonicalForm & f )
 *
 * icontent() - return gcd over all coefficients of f which are
 *   in a coefficient domain.
 *
**/
CanonicalForm
icontent ( const CanonicalForm & f )
{
    return icontent( f, 0 );
}

/** CanonicalForm gcd_poly ( const CanonicalForm & f, const CanonicalForm & g )
 *
 * gcd_poly() - calculate polynomial gcd.
 *
 * This is the dispatcher for polynomial gcd calculation.
 * Different gcd variants get called depending the input, characteristic, and
 * on switches (cf_defs.h)
 *
 * With the current settings from Singular (i.e. SW_USE_EZGCD= on,
 * SW_USE_EZGCD_P= on, SW_USE_CHINREM_GCD= on, the EZ GCD variants are the
 * default algorithms for multivariate polynomial GCD computations)
 *
 * @sa gcd(), cf_defs.h
 *
**/
CanonicalForm gcd_poly ( const CanonicalForm & f, const CanonicalForm & g )
{
  CanonicalForm fc, gc;
  bool fc_isUnivariate=f.isUnivariate();
  bool gc_isUnivariate=g.isUnivariate();
  bool fc_and_gc_Univariate=fc_isUnivariate && gc_isUnivariate;
  fc = f;
  gc = g;
  if ( getCharacteristic() != 0 )
  {
    #if defined(HAVE_FLINT) && ( __FLINT_RELEASE >= 20503)
    if ( isOn( SW_USE_FL_GCD_P)
    && (CFFactory::gettype() != GaloisFieldDomain)
    && (getCharacteristic()>10)
    &&(!hasAlgVar(fc)) && (!hasAlgVar(gc)))
    {
      return gcdFlintMP_Zp(fc,gc);
    }
    #endif
    #ifdef HAVE_NTL
    if ((!fc_and_gc_Univariate) && (isOn( SW_USE_EZGCD_P )))
    {
      fc= EZGCD_P (fc, gc);
    }
    #endif
    #ifdef HAVE_NTL
    else if (isOn(SW_USE_FF_MOD_GCD) && !fc_and_gc_Univariate)
    {
      Variable a;
      if (hasFirstAlgVar (fc, a) || hasFirstAlgVar (gc, a))
        fc=modGCDFq (fc, gc, a);
      else if (CFFactory::gettype() == GaloisFieldDomain)
        fc=modGCDGF (fc, gc);
      else
        fc=modGCDFp (fc, gc);
    }
    else
    #endif
    fc = subResGCD_p( fc, gc );
  }
  else if (!fc_and_gc_Univariate) /* && char==0*/
  {
    #if defined(HAVE_FLINT) && ( __FLINT_RELEASE >= 20503)
    if (( isOn( SW_USE_FL_GCD_0) )
    &&(!hasAlgVar(fc)) && (!hasAlgVar(gc)))
    {
      return gcdFlintMP_QQ(fc,gc);
    }
    else
    #endif
    #ifdef HAVE_NTL
    if ( isOn( SW_USE_EZGCD ) )
      fc= ezgcd (fc, gc);
    #endif  
    #ifdef HAVE_NTL
    else if (isOn(SW_USE_CHINREM_GCD))
      fc = modGCDZ( fc, gc);
    else
    #endif
    {
       fc = subResGCD_0( fc, gc );
    }
  }
  else
  {
    fc = subResGCD_0( fc, gc );
  }
  if ((getCharacteristic()>0)&&(!hasAlgVar(fc))) fc/=fc.lc();
  return fc;
}

/** static CanonicalForm cf_content ( const CanonicalForm & f, const CanonicalForm & g )
 *
 * cf_content() - return gcd(g, content(f)).
 *
 * content(f) is calculated with respect to f's main variable.
 *
 * @sa gcd(), content(), content( CF, Variable ).
 *
**/
static CanonicalForm
cf_content ( const CanonicalForm & f, const CanonicalForm & g )
{
    if ( f.inPolyDomain() || ( f.inExtension() && ! getReduce( f.mvar() ) ) )
    {
        CFIterator i = f;
        CanonicalForm result = g;
        while ( i.hasTerms() && ! result.isOne() )
        {
            result = gcd( i.coeff(), result );
            i++;
        }
        return result;
    }
    else
        return abs( f );
}

/** CanonicalForm content ( const CanonicalForm & f )
 *
 * content() - return content(f) with respect to main variable.
 *
 * Normalizes result.
 *
**/
CanonicalForm
content ( const CanonicalForm & f )
{
    if ( f.inPolyDomain() || ( f.inExtension() && ! getReduce( f.mvar() ) ) )
    {
        CFIterator i = f;
        CanonicalForm result = abs( i.coeff() );
        i++;
        while ( i.hasTerms() && ! result.isOne() )
        {
            result = gcd( i.coeff(), result );
            i++;
        }
        return result;
    }
    else
        return abs( f );
}

/** CanonicalForm content ( const CanonicalForm & f, const Variable & x )
 *
 * content() - return content(f) with respect to x.
 *
 * x should be a polynomial variable.
 *
**/
CanonicalForm
content ( const CanonicalForm & f, const Variable & x )
{
    if (f.inBaseDomain()) return f;
    ASSERT( x.level() > 0, "cannot calculate content with respect to algebraic variable" );
    Variable y = f.mvar();

    if ( y == x )
        return cf_content( f, 0 );
    else  if ( y < x )
        return f;
    else
        return swapvar( content( swapvar( f, y, x ), y ), y, x );
}

/** CanonicalForm vcontent ( const CanonicalForm & f, const Variable & x )
 *
 * vcontent() - return content of f with repect to variables >= x.
 *
 * The content is recursively calculated over all coefficients in
 * f having level less than x.  x should be a polynomial
 * variable.
 *
**/
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

/** CanonicalForm pp ( const CanonicalForm & f )
 *
 * pp() - return primitive part of f.
 *
 * Returns zero if f equals zero, otherwise f / content(f).
 *
**/
CanonicalForm
pp ( const CanonicalForm & f )
{
    if ( f.isZero() )
        return f;
    else
        return f / content( f );
}

CanonicalForm
gcd ( const CanonicalForm & f, const CanonicalForm & g )
{
    bool b = f.isZero();
    if ( b || g.isZero() )
    {
        if ( b )
            return abs( g );
        else
            return abs( f );
    }
    if ( f.inPolyDomain() || g.inPolyDomain() )
    {
        if ( f.mvar() != g.mvar() )
        {
            if ( f.mvar() > g.mvar() )
                return cf_content( f, g );
            else
                return cf_content( g, f );
        }
        if (isOn(SW_USE_QGCD))
        {
          Variable m;
          if (
          (getCharacteristic() == 0) &&
          (hasFirstAlgVar(f,m) || hasFirstAlgVar(g,m))
          )
          {
            bool on_rational = isOn(SW_RATIONAL);
            CanonicalForm r=QGCD(f,g);
            On(SW_RATIONAL);
            CanonicalForm cdF = bCommonDen( r );
            if (!on_rational) Off(SW_RATIONAL);
            return cdF*r;
          }
        }

        if ( f.inExtension() && getReduce( f.mvar() ) )
            return CanonicalForm(1);
        else
        {
            if ( fdivides( f, g ) )
                return abs( f );
            else  if ( fdivides( g, f ) )
                return abs( g );
            if ( !( getCharacteristic() == 0 && isOn( SW_RATIONAL ) ) )
            {
                CanonicalForm d;
                d = gcd_poly( f, g );
                return abs( d );
            }
            else
            {
                CanonicalForm cdF = bCommonDen( f );
                CanonicalForm cdG = bCommonDen( g );
                CanonicalForm F = f * cdF, G = g * cdG;
                Off( SW_RATIONAL );
                CanonicalForm l = gcd_poly( F, G );
                On( SW_RATIONAL );
                return abs( l );
            }
        }
    }
    if ( f.inBaseDomain() && g.inBaseDomain() )
        return bgcd( f, g );
    else
        return 1;
}

/** CanonicalForm lcm ( const CanonicalForm & f, const CanonicalForm & g )
 *
 * lcm() - return least common multiple of f and g.
 *
 * The lcm is calculated using the formula lcm(f, g) = f * g / gcd(f, g).
 *
 * Returns zero if one of f or g equals zero.
 *
**/
CanonicalForm
lcm ( const CanonicalForm & f, const CanonicalForm & g )
{
    if ( f.isZero() || g.isZero() )
        return 0;
    else
        return ( f / gcd( f, g ) ) * g;
}

