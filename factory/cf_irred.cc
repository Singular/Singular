/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include "cf_assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_algorithm.h"
#include "cf_random.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

static bool
is_irreducible ( const CanonicalForm & f )
{
    CFFList F = factorize( f );
    return F.length() == 1 && F.getFirst().exp() == 1;
}

CanonicalForm
find_irreducible ( int deg, CFRandom & gen, const Variable & x )
{
    CanonicalForm result;
    int i;
    do {
        result = power( x, deg );
        for ( i = deg-1; i >= 0; i-- )
            result += gen.generate() * power( x, i );
    } while ( ! is_irreducible( result ) );
    return result;
}

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
/// computes a random monic irreducible univariate polynomial in x over Fp of
/// degree i via NTL/FLINT
CanonicalForm
randomIrredpoly (int i, const Variable & x)
{
  int p= getCharacteristic();
  #ifdef HAVE_FLINT
  nmod_poly_t Irredpoly;
  nmod_poly_init(Irredpoly,p);
  nmod_poly_randtest_monic_irreducible(Irredpoly, FLINTrandom, i+1);
  CanonicalForm CFirredpoly=convertnmod_poly_t2FacCF(Irredpoly,Variable(1));
  nmod_poly_clear(Irredpoly);
  #elif defined(HAVE_NTL)
  if (fac_NTL_char != p)
  {
    fac_NTL_char= p;
    zz_p::init (p);
  }
  zz_pX NTLirredpoly;
  BuildIrred (NTLirredpoly, i);
  CanonicalForm CFirredpoly= convertNTLzzpX2CF (NTLirredpoly, x);
  #endif
  return CFirredpoly;
}
#endif
