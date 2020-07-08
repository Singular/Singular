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
#include "templates/ftmpl_functions.h"

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

#ifdef HAVE_FLINT
static CanonicalForm
gcd_univar_flintp (const CanonicalForm& F, const CanonicalForm& G)
{
  nmod_poly_t F1, G1;
  convertFacCF2nmod_poly_t (F1, F);
  convertFacCF2nmod_poly_t (G1, G);
  nmod_poly_gcd (F1, F1, G1);
  CanonicalForm result= convertnmod_poly_t2FacCF (F1, F.mvar());
  nmod_poly_clear (F1);
  nmod_poly_clear (G1);
  return result;
}

static CanonicalForm
gcd_univar_flint0( const CanonicalForm & F, const CanonicalForm & G )
{
  fmpz_poly_t F1, G1;
  convertFacCF2Fmpz_poly_t(F1, F);
  convertFacCF2Fmpz_poly_t(G1, G);
  fmpz_poly_gcd (F1, F1, G1);
  CanonicalForm result= convertFmpz_poly_t2FacCF (F1, F.mvar());
  fmpz_poly_clear (F1);
  fmpz_poly_clear (G1);
  return result;
}
#endif

#ifdef HAVE_NTL
static CanonicalForm
gcd_univar_ntl0( const CanonicalForm & F, const CanonicalForm & G )
{
    ZZX F1=convertFacCF2NTLZZX(F);
    ZZX G1=convertFacCF2NTLZZX(G);
    ZZX R=GCD(F1,G1);
    return convertNTLZZX2CF(R,F.mvar());
}

static CanonicalForm
gcd_univar_ntlp( const CanonicalForm & F, const CanonicalForm & G )
{
  if (fac_NTL_char!=getCharacteristic())
  {
    fac_NTL_char=getCharacteristic();
    zz_p::init(getCharacteristic());
  }
  zz_pX F1=convertFacCF2NTLzzpX(F);
  zz_pX G1=convertFacCF2NTLzzpX(G);
  zz_pX R=GCD(F1,G1);
  return  convertNTLzzpX2CF(R,F.mvar());
}
#endif

//{{{ static CanonicalForm balance_p ( const CanonicalForm & f, const CanonicalForm & q )
//{{{ docu
//
// balance_p() - map f from positive to symmetric representation
//   mod q.
//
// This makes sense for univariate polynomials over Z only.
// q should be an integer.
//
// Used by gcd_poly_univar0().
//
//}}}

static CanonicalForm
balance_p ( const CanonicalForm & f, const CanonicalForm & q, const CanonicalForm & qh )
{
    Variable x = f.mvar();
    CanonicalForm result = 0;
    CanonicalForm c;
    CFIterator i;
    for ( i = f; i.hasTerms(); i++ )
    {
        c = i.coeff();
        if ( c.inCoeffDomain())
        {
          if ( c > qh )
            result += power( x, i.exp() ) * (c - q);
          else
            result += power( x, i.exp() ) * c;
        }
        else
          result += power( x, i.exp() ) * balance_p(c,q,qh);
    }
    return result;
}

static CanonicalForm
balance_p ( const CanonicalForm & f, const CanonicalForm & q )
{
    CanonicalForm qh = q / 2;
    return balance_p (f, q, qh);
}

static CanonicalForm gcd_poly_univar0( const CanonicalForm & F, const CanonicalForm & G, bool primitive )
{
  CanonicalForm f, g, c, cg, cl, BB, B, M, q, Dp, newD, D, newq;
  int p, i;

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
      D = pp( balance_p( D, q ) );
      if ( fdivides( D, f ) && fdivides( D, g ) )
        return D * c;
      else
        q = 0;
    }
    else
      return gcd_poly( F, G );
    DEBOUTLN( cerr, "another try ..." );
  }
}

static CanonicalForm
gcd_poly_p( const CanonicalForm & f, const CanonicalForm & g )
{
    if (f.inCoeffDomain() || g.inCoeffDomain()) //zero case should be caught by gcd
      return 1;
    CanonicalForm pi, pi1;
    CanonicalForm C, Ci, Ci1, Hi, bi, pi2;
    bool bpure, ezgcdon= isOn (SW_USE_EZGCD_P);
    int delta = degree( f ) - degree( g );

    if ( delta >= 0 )
    {
        pi = f; pi1 = g;
    }
    else
    {
        pi = g; pi1 = f; delta = -delta;
    }
    if (pi.isUnivariate())
      Ci= 1;
    else
    {
      if (!ezgcdon)
        On (SW_USE_EZGCD_P);
      Ci = content( pi );
      if (!ezgcdon)
        Off (SW_USE_EZGCD_P);
      pi = pi / Ci;
    }
    if (pi1.isUnivariate())
      Ci1= 1;
    else
    {
      if (!ezgcdon)
        On (SW_USE_EZGCD_P);
      Ci1 = content( pi1 );
      if (!ezgcdon)
        Off (SW_USE_EZGCD_P);
      pi1 = pi1 / Ci1;
    }
    C = gcd( Ci, Ci1 );
    int d= 0;
    if ( !( pi.isUnivariate() && pi1.isUnivariate() ) )
    {
        if ( gcd_test_one( pi1, pi, true, d ) )
        {
          C=abs(C);
          //out_cf("GCD:",C,"\n");
          return C;
        }
        bpure = false;
    }
    else
    {
        bpure = isPurePoly(pi) && isPurePoly(pi1);
#ifdef HAVE_FLINT
        if (bpure && (CFFactory::gettype() != GaloisFieldDomain))
          return gcd_univar_flintp(pi,pi1)*C;
#else
#ifdef HAVE_NTL
        if ( bpure && (CFFactory::gettype() != GaloisFieldDomain))
            return gcd_univar_ntlp(pi, pi1 ) * C;
#endif
#endif
    }
    Variable v = f.mvar();
    Hi = power( LC( pi1, v ), delta );
    int maxNumVars= tmax (getNumVars (pi), getNumVars (pi1));

    if (!(pi.isUnivariate() && pi1.isUnivariate()))
    {
      if (size (Hi)*size (pi)/(maxNumVars*3) > 500) //maybe this needs more tuning
      {
        On (SW_USE_FF_MOD_GCD);
        C *= gcd (pi, pi1);
        Off (SW_USE_FF_MOD_GCD);
        return C;
      }
    }

    if ( (delta+1) % 2 )
        bi = 1;
    else
        bi = -1;
    CanonicalForm oldPi= pi, oldPi1= pi1, powHi;
    while ( degree( pi1, v ) > 0 )
    {
        if (!(pi.isUnivariate() && pi1.isUnivariate()))
        {
          if (size (pi)/maxNumVars > 500 || size (pi1)/maxNumVars > 500)
          {
            On (SW_USE_FF_MOD_GCD);
            C *= gcd (oldPi, oldPi1);
            Off (SW_USE_FF_MOD_GCD);
            return C;
          }
        }
        pi2 = psr( pi, pi1, v );
        pi2 = pi2 / bi;
        pi = pi1; pi1 = pi2;
        maxNumVars= tmax (getNumVars (pi), getNumVars (pi1));
        if (!pi1.isUnivariate() && (size (pi1)/maxNumVars > 500))
        {
            On (SW_USE_FF_MOD_GCD);
            C *= gcd (oldPi, oldPi1);
            Off (SW_USE_FF_MOD_GCD);
            return C;
        }
        if ( degree( pi1, v ) > 0 )
        {
            delta = degree( pi, v ) - degree( pi1, v );
            powHi= power (Hi, delta-1);
            if ( (delta+1) % 2 )
                bi = LC( pi, v ) * powHi*Hi;
            else
                bi = -LC( pi, v ) * powHi*Hi;
            Hi = power( LC( pi1, v ), delta ) / powHi;
            if (!(pi.isUnivariate() && pi1.isUnivariate()))
            {
              if (size (Hi)*size (pi)/(maxNumVars*3) > 1500) //maybe this needs more tuning
              {
                On (SW_USE_FF_MOD_GCD);
                C *= gcd (oldPi, oldPi1);
                Off (SW_USE_FF_MOD_GCD);
                return C;
              }
            }
        }
    }
    if ( degree( pi1, v ) == 0 )
    {
      C=abs(C);
      //out_cf("GCD:",C,"\n");
      return C;
    }
    if (!pi.isUnivariate())
    {
      if (!ezgcdon)
        On (SW_USE_EZGCD_P);
      Ci= gcd (LC (oldPi,v), LC (oldPi1,v));
      pi /= LC (pi,v)/Ci;
      Ci= content (pi);
      pi /= Ci;
      if (!ezgcdon)
        Off (SW_USE_EZGCD_P);
    }
    if ( bpure )
        pi /= pi.lc();
    C=abs(C*pi);
    //out_cf("GCD:",C,"\n");
    return C;
}

static CanonicalForm
gcd_poly_0( const CanonicalForm & f, const CanonicalForm & g )
{
    CanonicalForm pi, pi1;
    CanonicalForm C, Ci, Ci1, Hi, bi, pi2;
    int delta = degree( f ) - degree( g );

    if ( delta >= 0 )
    {
        pi = f; pi1 = g;
    }
    else
    {
        pi = g; pi1 = f; delta = -delta;
    }
    Ci = content( pi ); Ci1 = content( pi1 );
    pi1 = pi1 / Ci1; pi = pi / Ci;
    C = gcd( Ci, Ci1 );
    int d= 0;
    if ( pi.isUnivariate() && pi1.isUnivariate() )
    {
#ifdef HAVE_FLINT
        if (isPurePoly(pi) && isPurePoly(pi1) )
            return gcd_univar_flint0(pi, pi1 ) * C;
#else
#ifdef HAVE_NTL
        if ( isPurePoly(pi) && isPurePoly(pi1) )
            return gcd_univar_ntl0(pi, pi1 ) * C;
#endif
#endif
        return gcd_poly_univar0( pi, pi1, true ) * C;
    }
    else if ( gcd_test_one( pi1, pi, true, d ) )
      return C;
    Variable v = f.mvar();
    Hi = power( LC( pi1, v ), delta );
    if ( (delta+1) % 2 )
        bi = 1;
    else
        bi = -1;
    while ( degree( pi1, v ) > 0 )
    {
        pi2 = psr( pi, pi1, v );
        pi2 = pi2 / bi;
        pi = pi1; pi1 = pi2;
        if ( degree( pi1, v ) > 0 )
        {
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
    else
        return C * pp( pi );
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
    if (0) {} // dummy, to be able to build without NTL and FLINT
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
    #endif
    else
    fc = gcd_poly_p( fc, gc );
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
       fc = gcd_poly_0( fc, gc );
    }
  }
  else
  {
    fc = gcd_poly_0( fc, gc );
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

