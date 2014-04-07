/* emacs edit mode for this file is -*- C++ -*- */


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
#include "cf_factory.h"
#include "fac_util.h"
#include "templates/ftmpl_functions.h"
#include "algext.h"
#include "cf_gcd_smallp.h"
#include "cf_map_ext.h"
#include "cf_util.h"
#include "gfops.h"

#ifdef HAVE_NTL
#include <NTL/ZZX.h>
#include "NTLconvert.h"
bool isPurePoly(const CanonicalForm & );
#ifndef HAVE_FLINT
static CanonicalForm gcd_univar_ntl0( const CanonicalForm &, const CanonicalForm & );
static CanonicalForm gcd_univar_ntlp( const CanonicalForm &, const CanonicalForm & );
#endif
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
static CanonicalForm gcd_univar_flint0 (const CanonicalForm &, const CanonicalForm &);
static CanonicalForm gcd_univar_flintp (const CanonicalForm &, const CanonicalForm &);
#endif

static CanonicalForm cf_content ( const CanonicalForm &, const CanonicalForm & );

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

CanonicalForm chinrem_gcd(const CanonicalForm & FF,const CanonicalForm & GG);

bool
gcd_test_one ( const CanonicalForm & f, const CanonicalForm & g, bool swap, int & d )
{
    d= 0;
    int count = 0;
    // assume polys have same level;

    Variable v= Variable (1);
    bool algExtension= (hasFirstAlgVar (f, v) || hasFirstAlgVar (g, v));
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

    #define TEST_ONE_MAX 50
    int p= getCharacteristic();
    bool passToGF= false;
    int k= 1;
    if (p > 0 && p < TEST_ONE_MAX && CFFactory::gettype() != GaloisFieldDomain && !algExtension)
    {
      if (p == 2)
        setCharacteristic (2, 6, 'Z');
      else if (p == 3)
        setCharacteristic (3, 4, 'Z');
      else if (p == 5 || p == 7)
        setCharacteristic (p, 3, 'Z');
      else
        setCharacteristic (p, 2, 'Z');
      passToGF= true;
    }
    else if (p > 0 && CFFactory::gettype() == GaloisFieldDomain && ipower (p , getGFDegree()) < TEST_ONE_MAX)
    {
      k= getGFDegree();
      if (ipower (p, 2*k) > TEST_ONE_MAX)
        setCharacteristic (p, 2*k, gf_name);
      else
        setCharacteristic (p, 3*k, gf_name);
      F= GFMapUp (F, k);
      G= GFMapUp (G, k);
      lcf= GFMapUp (lcf, k);
      lcg= GFMapUp (lcg, k);
    }
    else if (p > 0 && p < TEST_ONE_MAX && algExtension)
    {
      bool extOfExt= false;
#ifdef HAVE_NTL
      int d= degree (getMipo (v));
      CFList source, dest;
      Variable v2;
      CanonicalForm primElem, imPrimElem;
      if (p == 2 && d < 6)
      {
        if (fac_NTL_char != 2)
        {
          fac_NTL_char= 2;
          zz_p::init (p);
        }
        bool primFail= false;
        Variable vBuf;
        primElem= primitiveElement (v, vBuf, primFail);
        ASSERT (!primFail, "failure in integer factorizer");
        if (d < 3)
        {
          zz_pX NTLIrredpoly;
          BuildIrred (NTLIrredpoly, d*3);
          CanonicalForm newMipo= convertNTLzzpX2CF (NTLIrredpoly, Variable (1));
          v2= rootOf (newMipo);
        }
        else
        {
          zz_pX NTLIrredpoly;
          BuildIrred (NTLIrredpoly, d*2);
          CanonicalForm newMipo= convertNTLzzpX2CF (NTLIrredpoly, Variable (1));
          v2= rootOf (newMipo);
        }
        imPrimElem= mapPrimElem (primElem, v, v2);
        extOfExt= true;
      }
      else if ((p == 3 && d < 4) || ((p == 5 || p == 7) && d < 3))
      {
        if (fac_NTL_char != p)
        {
          fac_NTL_char= p;
          zz_p::init (p);
        }
        bool primFail= false;
        Variable vBuf;
        primElem= primitiveElement (v, vBuf, primFail);
        ASSERT (!primFail, "failure in integer factorizer");
        zz_pX NTLIrredpoly;
        BuildIrred (NTLIrredpoly, d*2);
        CanonicalForm newMipo= convertNTLzzpX2CF (NTLIrredpoly, Variable (1));
        v2= rootOf (newMipo);
        imPrimElem= mapPrimElem (primElem, v, v2);
        extOfExt= true;
      }
      if (extOfExt)
      {
        F= mapUp (F, v, v2, primElem, imPrimElem, source, dest);
        G= mapUp (G, v, v2, primElem, imPrimElem, source, dest);
        lcf= mapUp (lcf, v, v2, primElem, imPrimElem, source, dest);
        lcg= mapUp (lcg, v, v2, primElem, imPrimElem, source, dest);
        v= v2;
      }
#endif
    }

    CFRandom * sample;
    if ((!algExtension && p > 0) || p == 0)
      sample = CFRandomFactory::generate();
    else
      sample = AlgExtRandomF (v).clone();

    REvaluation e( 2, tmax( f.level(), g.level() ), *sample );
    delete sample;

    if (passToGF)
    {
      lcf= lcf.mapinto();
      lcg= lcg.mapinto();
    }

    CanonicalForm eval1, eval2;
    if (passToGF)
    {
      eval1= e (lcf);
      eval2= e (lcg);
    }
    else
    {
      eval1= e (lcf);
      eval2= e (lcg);
    }

    while ( ( eval1.isZero() || eval2.isZero() ) && count < TEST_ONE_MAX )
    {
        e.nextpoint();
        count++;
        eval1= e (lcf);
        eval2= e (lcg);
    }
    if ( count >= TEST_ONE_MAX )
    {
        if (passToGF)
          setCharacteristic (p);
        if (k > 1)
          setCharacteristic (p, k, gf_name);
        return false;
    }


    if (passToGF)
    {
      F= F.mapinto();
      G= G.mapinto();
      eval1= e (F);
      eval2= e (G);
    }
    else
    {
      eval1= e (F);
      eval2= e (G);
    }

    CanonicalForm c= gcd (eval1, eval2);
    d= c.degree();
    bool result= d < 1;
    if (d < 0)
      d= 0;

    if (passToGF)
      setCharacteristic (p);
    if (k > 1)
      setCharacteristic (p, k, gf_name);
    return result;
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
#ifdef HAVE_NTL
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
#else
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
#else
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

/*static CanonicalForm
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
}*/
//}}}

#ifndef HAVE_NTL
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
#endif

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
        if (bpure && (CFFactory::gettype() != GaloisFieldDomain))
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
        if (isPurePoly(pi) && isPurePoly(pi1) )
            return gcd_univar_ntl0(pi, pi1 ) * C;
#endif
#endif
#ifndef HAVE_NTL
        return gcd_poly_univar0( pi, pi1, true ) * C;
#endif
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

//{{{ CanonicalForm gcd_poly ( const CanonicalForm & f, const CanonicalForm & g )
//{{{ docu
//
// gcd_poly() - calculate polynomial gcd.
//
// This is the dispatcher for polynomial gcd calculation.  We call either
// ezgcd(), sparsemod() or gcd_poly1() in dependecy on the current
// characteristic and settings of SW_USE_EZGCD.
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
  bool fc_isUnivariate=f.isUnivariate();
  bool gc_isUnivariate=g.isUnivariate();
  bool fc_and_gc_Univariate=fc_isUnivariate && gc_isUnivariate;
  fc = f;
  gc = g;
  if ( getCharacteristic() != 0 )
  {
    #ifdef HAVE_NTL
    if ((!fc_and_gc_Univariate) && (isOn( SW_USE_EZGCD_P )))
    {
      fc= EZGCD_P (fc, gc);
    }
    else if (isOn(SW_USE_FF_MOD_GCD) && !fc_and_gc_Univariate)
    {
      Variable a;
      if (hasFirstAlgVar (fc, a) || hasFirstAlgVar (gc, a))
        fc=GCD_Fp_extension (fc, gc, a);
      else if (CFFactory::gettype() == GaloisFieldDomain)
        fc=GCD_GF (fc, gc);
      else
        fc=GCD_small_p (fc, gc);
    }
    else
    #endif
    fc = gcd_poly_p( fc, gc );
  }
  else if (!fc_and_gc_Univariate)
  {
    if ( isOn( SW_USE_EZGCD ) )
      fc= ezgcd (fc, gc);
    else if (isOn(SW_USE_CHINREM_GCD))
      fc = chinrem_gcd( fc, gc);
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
                //printf ("here\n");
                CanonicalForm cdF = bCommonDen( f );
                CanonicalForm cdG = bCommonDen( g );
                Off( SW_RATIONAL );
                CanonicalForm l = lcm( cdF, cdG );
                On( SW_RATIONAL );
                CanonicalForm F = f * l, G = g * l;
                Off( SW_RATIONAL );
                l = gcd_poly( F, G );
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
#ifndef HAVE_FLINT
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
#endif

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


/*
*  compute positions p1 and pe of optimal variables:
*    pe is used in "ezgcd" and
*    p1 in "gcd_poly1"
*/
/*static
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
}*/

/*
*  make some changes of variables, see optvalues
*/
/*static void
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
}*/

TIMING_DEFINE_PRINT(chinrem_termination)
TIMING_DEFINE_PRINT(chinrem_recursion)

CanonicalForm chinrem_gcd ( const CanonicalForm & FF, const CanonicalForm & GG )
{
  CanonicalForm f, g, cl, q(0), Dp, newD, D, newq, newqh;
  int p, i, dp_deg, d_deg=-1;

  CanonicalForm cd ( bCommonDen( FF ));
  f=cd*FF;
  Variable x= Variable (1);
  CanonicalForm cf, cg;
  cf= icontent (f);
  f /= cf;
  //cd = bCommonDen( f ); f *=cd;
  //f /=vcontent(f,Variable(1));

  cd = bCommonDen( GG );
  g=cd*GG;
  cg= icontent (g);
  g /= cg;
  //cd = bCommonDen( g ); g *=cd;
  //g /=vcontent(g,Variable(1));

  CanonicalForm Dn, test= 0;
  CanonicalForm lcf, lcg;
  lcf= f.lc();
  lcg= g.lc();
  cl =  gcd (f.lc(),g.lc());
  CanonicalForm gcdcfcg= gcd (cf, cg);
  CanonicalForm fp, gp;
  CanonicalForm b= 1;
  int minCommonDeg= 0;
  for (i= tmax (f.level(), g.level()); i > 0; i--)
  {
    if (degree (f, i) <= 0 || degree (g, i) <= 0)
      continue;
    else
    {
      minCommonDeg= tmin (degree (g, i), degree (f, i));
      break;
    }
  }
  if (i == 0)
    return gcdcfcg;
  for (; i > 0; i--)
  {
    if (degree (f, i) <= 0 || degree (g, i) <= 0)
      continue;
    else
      minCommonDeg= tmin (minCommonDeg, tmin (degree (g, i), degree (f, i)));
  }
  b= 2*tmin (maxNorm (f), maxNorm (g))*abs (cl)*
     power (CanonicalForm (2), minCommonDeg);
  bool equal= false;
  i = cf_getNumBigPrimes() - 1;

  CanonicalForm cof, cog, cofp, cogp, newCof, newCog, cofn, cogn, cDn;
  int maxNumVars= tmax (getNumVars (f), getNumVars (g));
  //Off (SW_RATIONAL);
  while ( true )
  {
    p = cf_getBigPrime( i );
    i--;
    while ( i >= 0 && mod( cl*(lc(f)/cl)*(lc(g)/cl), p ) == 0 )
    {
      p = cf_getBigPrime( i );
      i--;
    }
    //printf("try p=%d\n",p);
    setCharacteristic( p );
    fp= mapinto (f);
    gp= mapinto (g);
    TIMING_START (chinrem_recursion)
#ifdef HAVE_NTL
    if (size (fp)/maxNumVars > 500 && size (gp)/maxNumVars > 500)
      Dp = GCD_small_p (fp, gp, cofp, cogp);
    else
    {
      Dp= gcd_poly (fp, gp);
      cofp= fp/Dp;
      cogp= gp/Dp;
    }
#else
    Dp= gcd_poly (fp, gp);
    cofp= fp/Dp;
    cogp= gp/Dp;
#endif
    TIMING_END_AND_PRINT (chinrem_recursion,
                          "time for gcd mod p in modular gcd: ");
    Dp /=Dp.lc();
    Dp *= mapinto (cl);
    cofp /= lc (cofp);
    cofp *= mapinto (lcf);
    cogp /= lc (cogp);
    cogp *= mapinto (lcg);
    setCharacteristic( 0 );
    dp_deg=totaldegree(Dp);
    if ( dp_deg == 0 )
    {
      //printf(" -> 1\n");
      return CanonicalForm(gcdcfcg);
    }
    if ( q.isZero() )
    {
      D = mapinto( Dp );
      cof= mapinto (cofp);
      cog= mapinto (cogp);
      d_deg=dp_deg;
      q = p;
      Dn= balance_p (D, p);
      cofn= balance_p (cof, p);
      cogn= balance_p (cog, p);
    }
    else
    {
      if ( dp_deg == d_deg )
      {
        chineseRemainder( D, q, mapinto( Dp ), p, newD, newq );
        chineseRemainder( cof, q, mapinto (cofp), p, newCof, newq);
        chineseRemainder( cog, q, mapinto (cogp), p, newCog, newq);
        cof= newCof;
        cog= newCog;
        newqh= newq/2;
        Dn= balance_p (newD, newq, newqh);
        cofn= balance_p (newCof, newq, newqh);
        cogn= balance_p (newCog, newq, newqh);
        if (test != Dn) //balance_p (newD, newq))
          test= balance_p (newD, newq);
        else
          equal= true;
        q = newq;
        D = newD;
      }
      else if ( dp_deg < d_deg )
      {
        //printf(" were all bad, try more\n");
        // all previous p's are bad primes
        q = p;
        D = mapinto( Dp );
        cof= mapinto (cof);
        cog= mapinto (cog);
        d_deg=dp_deg;
        test= 0;
        equal= false;
        Dn= balance_p (D, p);
        cofn= balance_p (cof, p);
        cogn= balance_p (cog, p);
      }
      else
      {
        //printf(" was bad, try more\n");
      }
      //else dp_deg > d_deg: bad prime
    }
    if ( i >= 0 )
    {
      cDn= icontent (Dn);
      Dn /= cDn;
      cofn /= cl/cDn;
      //cofn /= icontent (cofn);
      cogn /= cl/cDn;
      //cogn /= icontent (cogn);
      TIMING_START (chinrem_termination);
      if ((terminationTest (f,g, cofn, cogn, Dn)) ||
          ((equal || q > b) && fdivides (Dn, f) && fdivides (Dn, g)))
      {
        TIMING_END_AND_PRINT (chinrem_termination,
                            "time for successful termination in modular gcd: ");
        //printf(" -> success\n");
        return Dn*gcdcfcg;
      }
      TIMING_END_AND_PRINT (chinrem_termination,
                          "time for unsuccessful termination in modular gcd: ");
      equal= false;
      //else: try more primes
    }
    else
    { // try other method
      //printf("try other gcd\n");
      Off(SW_USE_CHINREM_GCD);
      D=gcd_poly( f, g );
      On(SW_USE_CHINREM_GCD);
      return D*gcdcfcg;
    }
  }
}


