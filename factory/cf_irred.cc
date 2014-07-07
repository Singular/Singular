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

#ifdef HAVE_NTL
/// computes a random monic irreducible univariate polynomial in x over Fp of
/// degree i via NTL
CanonicalForm
randomIrredpoly (int i, const Variable & x)
{
  int p= getCharacteristic();
  if (fac_NTL_char != p)
  {
    fac_NTL_char= p;
    zz_p::init (p);
  }
  zz_pX NTLirredpoly;
  CanonicalForm CFirredpoly;
  BuildIrred (NTLirredpoly, i);
  CFirredpoly= convertNTLzzpX2CF (NTLirredpoly, x);
  return CFirredpoly;
}
#endif
