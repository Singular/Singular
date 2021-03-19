#include "config.h"

#include "debug.h"

#include "cf_algorithm.h"
#include "templates/ftmpl_functions.h"
#include "cf_primes.h"
#include "cfGcdUtil.h"
#include "cfUnivarGcd.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

#ifdef HAVE_NTL
#ifndef HAVE_FLINT
CanonicalForm
gcd_univar_ntl0( const CanonicalForm & F, const CanonicalForm & G )
{
    ZZX F1=convertFacCF2NTLZZX(F);
    ZZX G1=convertFacCF2NTLZZX(G);
    ZZX R=GCD(F1,G1);
    return convertNTLZZX2CF(R,F.mvar());
}

CanonicalForm
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
#endif

#ifdef HAVE_FLINT
CanonicalForm
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

CanonicalForm
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

#ifndef HAVE_NTL
CanonicalForm gcd_poly_univar0( const CanonicalForm & F, const CanonicalForm & G, bool primitive )
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
#endif

/** CanonicalForm extgcd ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & a, CanonicalForm & b )
 *
 * extgcd() - returns polynomial extended gcd of f and g.
 *
 * Returns gcd(f, g) and a and b sucht that f*a+g*b=gcd(f, g).
 * The gcd is calculated using an extended euclidean polynomial
 * remainder sequence, so f and g should be polynomials over an
 * euclidean domain.  Normalizes result.
 *
 * Note: be sure that f and g have the same level!
 *
**/
CanonicalForm
extgcd ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & a, CanonicalForm & b )
{
  if (f.isZero())
  {
    a= 0;
    b= 1;
    return g;
  }
  else if (g.isZero())
  {
    a= 1;
    b= 0;
    return f;
  }
#ifdef HAVE_FLINT
  if (( getCharacteristic() > 0 ) && (CFFactory::gettype() != GaloisFieldDomain)
  &&  (f.level()==g.level()) && isPurePoly(f) && isPurePoly(g))
  {
    nmod_poly_t F1, G1, A, B, R;
    convertFacCF2nmod_poly_t (F1, f);
    convertFacCF2nmod_poly_t (G1, g);
    nmod_poly_init (R, getCharacteristic());
    nmod_poly_init (A, getCharacteristic());
    nmod_poly_init (B, getCharacteristic());
    nmod_poly_xgcd (R, A, B, F1, G1);
    a= convertnmod_poly_t2FacCF (A, f.mvar());
    b= convertnmod_poly_t2FacCF (B, f.mvar());
    CanonicalForm r= convertnmod_poly_t2FacCF (R, f.mvar());
    nmod_poly_clear (F1);
    nmod_poly_clear (G1);
    nmod_poly_clear (A);
    nmod_poly_clear (B);
    nmod_poly_clear (R);
    return r;
  }
#elif defined(HAVE_NTL)
  if (( getCharacteristic() > 0 ) && (CFFactory::gettype() != GaloisFieldDomain)
  &&  (f.level()==g.level()) && isPurePoly(f) && isPurePoly(g))
  {
    if (fac_NTL_char!=getCharacteristic())
    {
      fac_NTL_char=getCharacteristic();
      zz_p::init(getCharacteristic());
    }
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
#ifdef HAVE_FLINT
  if (( getCharacteristic() ==0) && (f.level()==g.level())
       && isPurePoly(f) && isPurePoly(g))
  {
    fmpq_poly_t F1, G1;
    convertFacCF2Fmpq_poly_t (F1, f);
    convertFacCF2Fmpq_poly_t (G1, g);
    fmpq_poly_t R, A, B;
    fmpq_poly_init (R);
    fmpq_poly_init (A);
    fmpq_poly_init (B);
    fmpq_poly_xgcd (R, A, B, F1, G1);
    a= convertFmpq_poly_t2FacCF (A, f.mvar());
    b= convertFmpq_poly_t2FacCF (B, f.mvar());
    CanonicalForm r= convertFmpq_poly_t2FacCF (R, f.mvar());
    fmpq_poly_clear (F1);
    fmpq_poly_clear (G1);
    fmpq_poly_clear (A);
    fmpq_poly_clear (B);
    fmpq_poly_clear (R);
    return r;
  }
#elif defined(HAVE_NTL)
  if (( getCharacteristic() ==0)
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
      if(!rr.isZero())
      {
        a=convertNTLZZX2CF(A,f.mvar())*fc/rr;
        b=convertNTLZZX2CF(B,f.mvar())*gc/rr;
        return CanonicalForm(1);
      }
      else
      {
        F1 /= R;
        G1 /= R;
        XGCD (RR, A,B,F1,G1,1);
        rr=convertZZ2CF(RR);
        a=convertNTLZZX2CF(A,f.mvar())*(fc/rr);
        b=convertNTLZZX2CF(B,f.mvar())*(gc/rr);
      }
    }
    else
    {
      XGCD(RR,A,B,F1,G1,1);
      CanonicalForm rr=convertZZ2CF(RR);
      if (!rr.isZero())
      {
        a=convertNTLZZX2CF(A,f.mvar())*fc;
        b=convertNTLZZX2CF(B,f.mvar())*gc;
      }
      else
      {
        F1 /= R;
        G1 /= R;
        XGCD (RR, A,B,F1,G1,1);
        rr=convertZZ2CF(RR);
        a=convertNTLZZX2CF(A,f.mvar())*(fc/rr);
        b=convertNTLZZX2CF(B,f.mvar())*(gc/rr);
      }
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
