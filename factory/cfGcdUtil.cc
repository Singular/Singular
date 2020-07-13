#include "config.h"

#include "canonicalform.h"
#include "cf_factory.h"
#include "cf_reval.h"
#include "cf_util.h"
#include "cf_iter.h"
#include "gfops.h"
#include "cf_map_ext.h"
#include "templates/ftmpl_functions.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

/// Coprimality Check. f and g are assumed to have the same level. If swap is
/// true, the main variables of f and g are swapped with Variable(1). If the
/// result is false, d is set to the degree of the gcd of f and g evaluated at a
/// random point in K^n-1. This gcd is a gcd of univariate polynomials.
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
    bool extOfExt= false;
    Variable v3;
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
#if defined(HAVE_NTL) || defined(HAVE_FLINT)
      int d= degree (getMipo (v));
      CFList source, dest;
      Variable v2;
      CanonicalForm primElem, imPrimElem;
      #if defined(HAVE_NTL) && !defined(HAVE_FLINT)
      if (fac_NTL_char != p)
      {
        fac_NTL_char= p;
        zz_p::init (p);
      }
      #endif
      if (p == 2 && d < 6)
      {
        bool primFail= false;
        Variable vBuf;
        primElem= primitiveElement (v, vBuf, primFail);
        ASSERT (!primFail, "failure in integer factorizer");
        if (d < 3)
        {
          #ifdef HAVE_FLINT
          nmod_poly_t Irredpoly;
          nmod_poly_init(Irredpoly,p);
          nmod_poly_randtest_monic_irreducible(Irredpoly, FLINTrandom, 3*d+1);
          CanonicalForm newMipo=convertnmod_poly_t2FacCF(Irredpoly,Variable(1));
          nmod_poly_clear(Irredpoly);
          #elif defined(HAVE_NTL)
          zz_pX NTLIrredpoly;
          BuildIrred (NTLIrredpoly, d*3);
          CanonicalForm newMipo= convertNTLzzpX2CF (NTLIrredpoly, Variable (1));
          #else
          factoryError("NTL/FLINT missing:gcd_test_one");
          #endif
          v2= rootOf (newMipo);
        }
        else
        {
          #ifdef HAVE_FLINT
          nmod_poly_t Irredpoly;
          nmod_poly_init(Irredpoly,p);
          nmod_poly_randtest_monic_irreducible(Irredpoly, FLINTrandom, 3*d+1);
          CanonicalForm newMipo=convertnmod_poly_t2FacCF(Irredpoly,Variable(1));
          nmod_poly_clear(Irredpoly);
          #elif defined(HAVE_NTL)
          zz_pX NTLIrredpoly;
          BuildIrred (NTLIrredpoly, d*2);
          CanonicalForm newMipo= convertNTLzzpX2CF (NTLIrredpoly, Variable (1));
          #else
          factoryError("NTL/FLINT missing:gcd_test_one");
          #endif
          v2= rootOf (newMipo);
        }
        imPrimElem= mapPrimElem (primElem, v, v2);
        extOfExt= true;
      }
      else if ((p == 3 && d < 4) || ((p == 5 || p == 7) && d < 3))
      {
        bool primFail= false;
        Variable vBuf;
        primElem= primitiveElement (v, vBuf, primFail);
        ASSERT (!primFail, "failure in integer factorizer");
        #ifdef HAVE_FLINT
        nmod_poly_t Irredpoly;
        nmod_poly_init(Irredpoly,p);
        nmod_poly_randtest_monic_irreducible(Irredpoly, FLINTrandom, 2*d+1);
        CanonicalForm newMipo=convertnmod_poly_t2FacCF(Irredpoly,Variable(1));
        nmod_poly_clear(Irredpoly);
        #elif defined(HAVE_NTL)
        zz_pX NTLIrredpoly;
        BuildIrred (NTLIrredpoly, d*2);
        CanonicalForm newMipo= convertNTLzzpX2CF (NTLIrredpoly, Variable (1));
        #else
        factoryError("NTL/FLINT missing:gcd_test_one");
        #endif
        v2= rootOf (newMipo);
        imPrimElem= mapPrimElem (primElem, v, v2);
        extOfExt= true;
      }
      if (extOfExt)
      {
        v3= v;
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
        if (extOfExt)
          prune1 (v3);
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
    if (extOfExt)
      prune1 (v3);
    return result;
}

/**
 * same as balance_p ( const CanonicalForm & f, const CanonicalForm & q )
 * but qh= q/2 is provided, too.
**/
CanonicalForm
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

/** static CanonicalForm balance_p ( const CanonicalForm & f, const CanonicalForm & q )
 *
 * balance_p() - map f from positive to symmetric representation
 *   mod q.
 *
 * This makes sense for polynomials over Z only.
 * q should be an integer.
 *
**/
CanonicalForm
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
