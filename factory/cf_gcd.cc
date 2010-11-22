/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#include <config.h>

#define OM_NO_MALLOC_MACROS


#include "assert.h"
#include "debug.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_reval.h"
#include "cf_primes.h"
#include "cf_algorithm.h"
#include "fac_util.h"
#include "ftmpl_functions.h"
#include "ffreval.h"
#include "algext.h"
#include "fieldGCD.h"
#include "cf_gcd_smallp.h"


#ifdef HAVE_NTL
#include <NTL/ZZX.h>
#include "NTLconvert.h"
bool isPurePoly(const CanonicalForm & );
static CanonicalForm gcd_univar_ntl0( const CanonicalForm &, const CanonicalForm & );
static CanonicalForm gcd_univar_ntlp( const CanonicalForm &, const CanonicalForm & );
#endif

static CanonicalForm cf_content ( const CanonicalForm &, const CanonicalForm & );
static bool gcd_avoid_mtaildegree ( CanonicalForm &, CanonicalForm &, CanonicalForm & );
static void cf_prepgcd( const CanonicalForm &, const CanonicalForm &, int &, int &, int & );

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

CanonicalForm chinrem_gcd(const CanonicalForm & FF,const CanonicalForm & GG);
CanonicalForm newGCD(CanonicalForm A, CanonicalForm B);

bool
gcd_test_one ( const CanonicalForm & f, const CanonicalForm & g, bool swap )
{
    int count = 0;
    // assume polys have same level;
    CFRandom * sample = CFRandomFactory::generate();
    REvaluation e( 2, tmax( f.level(), g.level() ), *sample );
    delete sample;
    CanonicalForm lcf, lcg;
    if ( swap )
    {
        lcf = swapvar( LC( f ), Variable(1), f.mvar() );
        lcg = swapvar( LC( g ), Variable(1), f.mvar() );
    }
    else
    {
        lcf = LC( f, Variable(1) );
        lcg = LC( g, Variable(1) );
    }
    #define TEST_ONE_MAX 50
    while ( ( e( lcf ).isZero() || e( lcg ).isZero() ) && count < TEST_ONE_MAX )
    {
        e.nextpoint();
        count++;
    }
    if ( count == TEST_ONE_MAX )
        return false;
    CanonicalForm F, G;
    if ( swap )
    {
        F=swapvar( f, Variable(1), f.mvar() );
        G=swapvar( g, Variable(1), g.mvar() );
    }
    else
    {
        F = f;
        G = g;
    }
    if ( e(F).taildegree() > 0 && e(G).taildegree() > 0 )
        return false;
    return gcd( e( F ), e( G ) ).degree() < 1;
}

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
  if (isOn(SW_USE_NTL_GCD_P) && ( getCharacteristic() > 0 ) && (CFFactory::gettype() != GaloisFieldDomain)
  &&  (f.level()==g.level()) && isPurePoly(f) && isPurePoly(g))
  {
    if (fac_NTL_char!=getCharacteristic())
    {
      fac_NTL_char=getCharacteristic();
      #ifdef NTL_ZZ
      ZZ r;
      r=getCharacteristic();
      ZZ_pContext ccc(r);
      #else
      zz_pContext ccc(getCharacteristic());
      #endif
      ccc.restore();
      #ifdef NTL_ZZ
      ZZ_p::init(r);
      #else
      zz_p::init(getCharacteristic());
      #endif
    }
    #ifdef NTL_ZZ
    ZZ_pX F1=convertFacCF2NTLZZpX(f);
    ZZ_pX G1=convertFacCF2NTLZZpX(g);
    ZZ_pX R;
    ZZ_pX A,B;
    XGCD(R,A,B,F1,G1);
    a=convertNTLZZpX2CF(A,f.mvar());
    b=convertNTLZZpX2CF(B,f.mvar());
    return convertNTLZZpX2CF(R,f.mvar());
    #else
    zz_pX F1=convertFacCF2NTLzzpX(f);
    zz_pX G1=convertFacCF2NTLzzpX(g);
    zz_pX R;
    zz_pX A,B;
    XGCD(R,A,B,F1,G1);
    a=convertNTLzzpX2CF(A,f.mvar());
    b=convertNTLzzpX2CF(B,f.mvar());
    return convertNTLzzpX2CF(R,f.mvar());
    #endif
  }
  if (isOn(SW_USE_NTL_GCD_0) && ( getCharacteristic() ==0)
  && (f.level()==g.level()) && isPurePoly(f) && isPurePoly(g))
  {
    CanonicalForm fc=bCommonDen(f);
    CanonicalForm gc=bCommonDen(g);
    ZZX F1=convertFacCF2NTLZZX(f*fc);
    ZZX G1=convertFacCF2NTLZZX(g*gc);
    ZZX R=GCD(F1,G1);
    CanonicalForm r=convertNTLZZX2CF(R,f.mvar());
    ZZ RR;
    ZZX A,B;
    if (r.inCoeffDomain())
    {
      XGCD(RR,A,B,F1,G1,1);
      CanonicalForm rr=convertZZ2CF(RR);
      ASSERT (!rr.isZero(), "NTL:XGCD failed");
      a=convertNTLZZX2CF(A,f.mvar())*fc/rr;
      b=convertNTLZZX2CF(B,f.mvar())*gc/rr;
      return CanonicalForm(1);
    }
    else
    {
      fc=bCommonDen(f);
      gc=bCommonDen(g);
      F1=convertFacCF2NTLZZX(f*fc/r);
      G1=convertFacCF2NTLZZX(g*gc/r);
      XGCD(RR,A,B,F1,G1,1);
      a=convertNTLZZX2CF(A,f.mvar())*fc;
      b=convertNTLZZX2CF(B,f.mvar())*gc;
      CanonicalForm rr=convertZZ2CF(RR);
      ASSERT (!rr.isZero(), "NTL:XGCD failed");
      r*=rr;
      if ( r.sign() < 0 ) { r= -r; a= -a; b= -b; }
      return r;
    }
  }
#endif
  // may contain bug in the co-factors, see track 107
  CanonicalForm contf = content( f );
  CanonicalForm contg = content( g );

  CanonicalForm p0 = f / contf, p1 = g / contg;
  CanonicalForm f0 = 1, f1 = 0, g0 = 0, g1 = 1, q, r;

  while ( ! p1.isZero() )
  {
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
  if ( p0.sign() < 0 )
  {
      p0 = -p0;
      a = -a;
      b = -b;
  }
  return p0;
}
//}}}

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
      D = pp( balance( D, q ) );
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
    CanonicalForm pi, pi1;
    CanonicalForm C, Ci, Ci1, Hi, bi, pi2;
    bool bpure;
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
    if ( !( pi.isUnivariate() && pi1.isUnivariate() ) )
    {
        //out_cf("F:",f,"\n");
        //out_cf("G:",g,"\n");
        //out_cf("newGCD:",newGCD(f,g),"\n");
        if (isOn(SW_USE_GCD_P) && (getCharacteristic()>0))
        {
          return newGCD(f,g);
        }
        if ( gcd_test_one( pi1, pi, true ) )
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
#ifdef HAVE_NTL
        if ( isOn(SW_USE_NTL_GCD_P) && bpure && (CFFactory::gettype() != GaloisFieldDomain))
            return gcd_univar_ntlp(pi, pi1 ) * C;
#endif
    }
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
    {
      C=abs(C);
      //out_cf("GCD:",C,"\n");
      return C;
    }
    pi /= content( pi );
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
    if ( pi.isUnivariate() && pi1.isUnivariate() )
    {
#ifdef HAVE_NTL
        if ( isOn(SW_USE_NTL_GCD_0) && isPurePoly(pi) && isPurePoly(pi1) )
            return gcd_univar_ntl0(pi, pi1 ) * C;
#endif
        return gcd_poly_univar0( pi, pi1, true ) * C;
    }
    else if ( gcd_test_one( pi1, pi, true ) )
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

//{{{ CanonicalForm gcd_poly ( const CanonicalForm & f, const CanonicalForm & g )
//{{{ docu
//
// gcd_poly() - calculate polynomial gcd.
//
// This is the dispatcher for polynomial gcd calculation.  We call either
// ezgcd(), sparsemod() or gcd_poly1() in dependecy on the current
// characteristic and settings of SW_USE_EZGCD and SW_USE_SPARSEMOD, resp.
//
// Used by gcd() and gcd_poly_univar0().
//
//}}}
#if 0
int si_factor_reminder=1;
#endif
CanonicalForm gcd_poly ( const CanonicalForm & f, const CanonicalForm & g )
{
  CanonicalForm fc, gc, d1;
  int mp, cc, p1, pe;
  mp = f.level()+1;
  bool fc_isUnivariate=f.isUnivariate();
  bool gc_isUnivariate=g.isUnivariate();
  bool fc_and_gc_Univariate=fc_isUnivariate && gc_isUnivariate;
#if 1
  if (( getCharacteristic() == 0 )
  && (f.level() >4)
  && (g.level() >4)
  && isOn( SW_USE_CHINREM_GCD)
  && (!fc_and_gc_Univariate)
  && (isPurePoly_m(f))
  && (isPurePoly_m(g))
  )
  {
      return chinrem_gcd( f, g );
  }
#endif
  cf_prepgcd( f, g, cc, p1, pe);
  if ( cc != 0 )
  {
    if ( cc > 0 )
    {
      fc = replacevar( f, Variable(cc), Variable(mp) );
      gc = g;
    }
    else
    {
      fc = replacevar( g, Variable(-cc), Variable(mp) );
      gc = f;
    }
    return cf_content( fc, gc );
  }
// now each appearing variable is in f and g
  fc = f;
  gc = g;
  if( gcd_avoid_mtaildegree ( fc, gc, d1 ) )
      return d1;
  if ( getCharacteristic() != 0 )
  {
    if ((!fc_and_gc_Univariate)
    && isOn(SW_USE_fieldGCD)
    && (getCharacteristic() >100))
    {
      return fieldGCD(f,g);
    }
    else if ((!fc_and_gc_Univariate) && (isOn( SW_USE_EZGCD_P )))
    {
      /*if ( pe == 1 )
        fc = fin_ezgcd( fc, gc );
      else if ( pe > 0 )// no variable at position 1
      {
        fc = replacevar( fc, Variable(pe), Variable(1) );
        gc = replacevar( gc, Variable(pe), Variable(1) );
        fc = replacevar( fin_ezgcd( fc, gc ), Variable(1), Variable(pe) );
      }
      else
      {
        pe = -pe;
        fc = swapvar( fc, Variable(pe), Variable(1) );
        gc = swapvar( gc, Variable(pe), Variable(1) );
        fc = swapvar( fin_ezgcd( fc, gc ), Variable(1), Variable(pe) );
      }*/
      fc= EZGCD_P (fc, gc);
    }
    else if (isOn(SW_USE_GCD_P))
    {
      fc=newGCD(fc,gc);
    }
    else if (isOn(SW_USE_FF_MOD_GCD) && !fc_and_gc_Univariate)
    {
      Variable a;
      if (hasFirstAlgVar (fc, a) || hasFirstAlgVar (gc, a))
      {
        fc=GCD_Fp_extension (fc, gc, a);
      }
      if (CFFactory::gettype() == GaloisFieldDomain)
      {
        fc=GCD_GF (fc, gc);
      }
      fc=GCD_small_p (fc, gc);
    }
    else if ( p1 == fc.level() )
      fc = gcd_poly_p( fc, gc );
    else
    {
      fc = replacevar( fc, Variable(p1), Variable(mp) );
      gc = replacevar( gc, Variable(p1), Variable(mp) );
      fc = replacevar( gcd_poly_p( fc, gc ), Variable(mp), Variable(p1) );
    }
  }
  else if (!fc_and_gc_Univariate)
  {
    if (
    isOn(SW_USE_CHINREM_GCD)
    && (gc.level() >5)
    && (fc.level() >5)
    && (isPurePoly_m(fc)) && (isPurePoly_m(gc))
    )
    {
    #if 0
      if ( p1 == fc.level() )
        fc = chinrem_gcd( fc, gc );
      else
      {
        fc = replacevar( fc, Variable(p1), Variable(mp) );
        gc = replacevar( gc, Variable(p1), Variable(mp) );
        fc = replacevar( chinrem_gcd( fc, gc ), Variable(mp), Variable(p1) );
      }
    #else
      fc = chinrem_gcd( fc, gc);
    #endif
    }
    else if ( isOn( SW_USE_EZGCD ) )
    {
      if ( pe == 1 )
        fc = ezgcd( fc, gc );
      else if ( pe > 0 )// no variable at position 1
      {
        fc = replacevar( fc, Variable(pe), Variable(1) );
        gc = replacevar( gc, Variable(pe), Variable(1) );
        fc = replacevar( ezgcd( fc, gc ), Variable(1), Variable(pe) );
      }
      else
      {
        pe = -pe;
        fc = swapvar( fc, Variable(pe), Variable(1) );
        gc = swapvar( gc, Variable(pe), Variable(1) );
        fc = swapvar( ezgcd( fc, gc ), Variable(1), Variable(pe) );
      }
    }
    else if (
    isOn(SW_USE_CHINREM_GCD)
    && (isPurePoly_m(fc)) && (isPurePoly_m(gc))
    )
    {
    #if 0
      if ( p1 == fc.level() )
        fc = chinrem_gcd( fc, gc );
      else
      {
        fc = replacevar( fc, Variable(p1), Variable(mp) );
        gc = replacevar( gc, Variable(p1), Variable(mp) );
        fc = replacevar( chinrem_gcd( fc, gc ), Variable(mp), Variable(p1) );
      }
    #else
      fc = chinrem_gcd( fc, gc);
    #endif
    }
    else
    {
       fc = gcd_poly_0( fc, gc );
    }
  }
  else
  {
    fc = gcd_poly_0( fc, gc );
  }
  if ( d1.degree() > 0 )
    fc *= d1;
  return fc;
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
          //&& f.isUnivariate()
          //&& g.isUnivariate()
          )
          {
            //if ((f.level()==g.level()) && f.isUnivariate() && g.isUnivariate())
            //  return univarQGCD(f,g);
            //else
              //return QGCD(f,g);
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
#if 1
                do{ d = gcd_poly( f, g ); }
                while ((!fdivides(d,f)) || (!fdivides(d,g)));
#else
                while(1)
                {
                  d = gcd_poly( f, g );
                  if ((fdivides(d,f)) && (fdivides(d,g))) break;
                  printf("g"); fflush(stdout);
                }
#endif
                return abs( d );
            }
            else
            {
                CanonicalForm cdF = bCommonDen( f );
                CanonicalForm cdG = bCommonDen( g );
                Off( SW_RATIONAL );
                CanonicalForm l = lcm( cdF, cdG );
                On( SW_RATIONAL );
                CanonicalForm F = f * l, G = g * l;
                Off( SW_RATIONAL );
                do { l = gcd_poly( F, G ); }
                while ((!fdivides(l,F)) || (!fdivides(l,G)));
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
        return 0;
    else
        return ( f / gcd( f, g ) ) * g;
}
//}}}

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
    #ifdef NTL_ZZ
    ZZ r;
    r=getCharacteristic();
    ZZ_pContext ccc(r);
    #else
    zz_pContext ccc(getCharacteristic());
    #endif
    ccc.restore();
    #ifdef NTL_ZZ
    ZZ_p::init(r);
    #else
    zz_p::init(getCharacteristic());
    #endif
  }
  #ifdef NTL_ZZ
  ZZ_pX F1=convertFacCF2NTLZZpX(F);
  ZZ_pX G1=convertFacCF2NTLZZpX(G);
  ZZ_pX R=GCD(F1,G1);
  return  convertNTLZZpX2CF(R,F.mvar());
  #else
  zz_pX F1=convertFacCF2NTLzzpX(F);
  zz_pX G1=convertFacCF2NTLzzpX(G);
  zz_pX R=GCD(F1,G1);
  return  convertNTLzzpX2CF(R,F.mvar());
  #endif
}

#endif

static bool
gcd_avoid_mtaildegree ( CanonicalForm & f1, CanonicalForm & g1, CanonicalForm & d1 )
{
    bool rdy = true;
    int df = f1.taildegree();
    int dg = g1.taildegree();

    d1 = d1.genOne();
    if ( dg == 0 )
    {
        if ( df == 0 )
            return false;
        else
        {
            if ( f1.degree() == df )
                d1 = cf_content( g1, LC( f1 ) );
            else
            {
                f1 /= power( f1.mvar(), df );
                rdy = false;
            }
        }
    }
    else
    {
        if ( df == 0)
        {
            if ( g1.degree() == dg )
                d1 = cf_content( f1, LC( g1 ) );
            else
            {
                g1 /= power( g1.mvar(), dg );
                rdy = false;
            }
        }
        else
        {
            if ( df > dg )
                d1 = power( f1.mvar(), dg );
            else
                d1 = power( f1.mvar(), df );
            if ( f1.degree() == df )
            {
                if (g1.degree() == dg)
                    d1 *= gcd( LC( f1 ), LC( g1 ) );
                else
                {
                    g1 /= power( g1.mvar(), dg);
                    d1 *= cf_content( g1, LC( f1 ) );
                }
            }
            else
            {
                f1 /= power( f1.mvar(), df );
                if ( g1.degree() == dg )
                    d1 *= cf_content( f1, LC( g1 ) );
                else
                {
                    g1 /= power( g1.mvar(), dg );
                    rdy = false;
                }
            }
        }
    }
    return rdy;
}

/*
*  compute positions p1 and pe of optimal variables:
*    pe is used in "ezgcd" and
*    p1 in "gcd_poly1"
*/
static
void optvalues ( const int * df, const int * dg, const int n, int & p1, int &pe )
{
    int i, o1, oe;
    if ( df[n] > dg[n] )
    {
        o1 = df[n]; oe = dg[n];
    }
    else
    {
        o1 = dg[n]; oe = df[n];
    }
    i = n-1;
    while ( i > 0 )
    {
        if ( df[i] != 0 )
        {
            if ( df[i] > dg[i] )
            {
                if ( o1 > df[i]) { o1 = df[i]; p1 = i; }
                if ( oe <= dg[i]) { oe = dg[i]; pe = i; }
            }
            else
            {
                if ( o1 > dg[i]) { o1 = dg[i]; p1 = i; }
                if ( oe <= df[i]) { oe = df[i]; pe = i; }
            }
        }
        i--;
    }
}

/*
*  make some changes of variables, see optvalues
*/
static void
cf_prepgcd( const CanonicalForm & f, const CanonicalForm & g, int & cc, int & p1, int &pe )
{
    int i, k, n;
    n = f.level();
    cc = 0;
    p1 = pe = n;
    if ( n == 1 )
        return;
    int * degsf = new int[n+1];
    int * degsg = new int[n+1];
    for ( i = n; i > 0; i-- )
    {
        degsf[i] = degsg[i] = 0;
    }
    degsf = degrees( f, degsf );
    degsg = degrees( g, degsg );

    k = 0;
    for ( i = n-1; i > 0; i-- )
    {
        if ( degsf[i] == 0 )
        {
            if ( degsg[i] != 0 )
            {
                cc = -i;
                break;
            }
        }
        else
        {
            if ( degsg[i] == 0 )
            {
                cc = i;
                break;
            }
            else k++;
        }
    }

    if ( ( cc == 0 ) && ( k != 0 ) )
        optvalues( degsf, degsg, n, p1, pe );
    if ( ( pe != 1 ) && ( degsf[1] != 0 ) )
        pe = -pe;

    delete [] degsf;
    delete [] degsg;
}


static CanonicalForm
balance_p ( const CanonicalForm & f, const CanonicalForm & q )
{
    Variable x = f.mvar();
    CanonicalForm result = 0, qh = q / 2;
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
          result += power( x, i.exp() ) * balance_p(c,q);
    }
    return result;
}

CanonicalForm chinrem_gcd ( const CanonicalForm & FF, const CanonicalForm & GG )
{
  CanonicalForm f, g, cg, cl, q(0), Dp, newD, D, newq;
  int p, i, dp_deg, d_deg;

  CanonicalForm cd ( bCommonDen( FF ));
  f=cd*FF;
  f /=vcontent(f,Variable(1));
  //cd = bCommonDen( f ); f *=cd;
  //f /=vcontent(f,Variable(1));

  cd = bCommonDen( GG );
  g=cd*GG;
  g /=vcontent(g,Variable(1));
  //cd = bCommonDen( g ); g *=cd;
  //g /=vcontent(g,Variable(1));

  i = cf_getNumBigPrimes() - 1;
  cl =  f.lc()* g.lc();

  while ( true )
  {
    p = cf_getBigPrime( i );
    i--;
    while ( i >= 0 && mod( cl, p ) == 0 )
    {
      p = cf_getBigPrime( i );
      i--;
    }
    //printf("try p=%d\n",p);
    setCharacteristic( p );
    Dp = gcd_poly( mapinto( f ), mapinto( g ) );
    Dp /=Dp.lc();
    setCharacteristic( 0 );
    dp_deg=totaldegree(Dp);
    if ( dp_deg == 0 )
    {
      //printf(" -> 1\n");
      return CanonicalForm(1);
    }
    if ( q.isZero() )
    {
      D = mapinto( Dp );
      d_deg=dp_deg;
      q = p;
    }
    else
    {
      if ( dp_deg == d_deg )
      {
        chineseRemainder( D, q, mapinto( Dp ), p, newD, newq );
        q = newq;
        D = newD;
      }
      else if ( dp_deg < d_deg )
      {
        //printf(" were all bad, try more\n");
        // all previous p's are bad primes
        q = p;
        D = mapinto( Dp );
        d_deg=dp_deg;
      }
      else
      {
        //printf(" was bad, try more\n");
      }
      //else dp_deg > d_deg: bad prime
    }
    if ( i >= 0 )
    {
      CanonicalForm Dn= Farey(D,q);
      int is_rat=isOn(SW_RATIONAL);
      On(SW_RATIONAL);
      CanonicalForm cd = bCommonDen( Dn ); // we need On(SW_RATIONAL)
      if (!is_rat) Off(SW_RATIONAL);
      Dn *=cd;
      //Dn /=vcontent(Dn,Variable(1));
      if ( fdivides( Dn, f ) && fdivides( Dn, g ) )
      {
        //printf(" -> success\n");
        return Dn;
      }
      //else: try more primes
    }
    else
    { // try other method
      //printf("try other gcd\n");
      Off(SW_USE_CHINREM_GCD);
      D=gcd_poly( f, g );
      On(SW_USE_CHINREM_GCD);
      return D;
    }
  }
}

#include "algext.cc"

