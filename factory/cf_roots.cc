//*****************************************************************************


#include "config.h"


#include "cf_assert.h"
#include "debug.h"

#include "canonicalform.h"
#include "cf_util.h"
#include "imm.h"
#include "cf_iter.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

#include "cf_roots.h"

int* Zp_roots (const CanonicalForm f)
{
  int p= getCharacteristic ();
  #if defined(HAVE_FLINT) && (__FLINT_RELEASE >= 20503)
    nmod_poly_t FLINT_f;
    convertFacCF2nmod_poly_t(FLINT_f,f);
    nmod_poly_factor_t fac;
    nmod_poly_factor_init(fac);
    nmod_poly_roots(fac,FLINT_f,0);
    int *res=NEW_ARRAY(int,1+fac->num);

    int j=1;
    for(int i=fac->num-1; i>=0;i--)
    {
      // root of (linear) factor: -absolute Term
      if (nmod_poly_length(fac->p+i)==2)
      {
        res[j]=p-nmod_poly_get_coeff_ui(fac->p+i,0);
	j++;
      }
    }
    res[0]=j-1;
    // cleanup
    nmod_poly_clear(FLINT_f);
    nmod_poly_factor_clear(fac);
    return res;
  #elif defined(HAVE_NTL)
  if (fac_NTL_char != p)
  {
    fac_NTL_char= p;
    zz_p::init (p);
  }
  zz_pX NTL_f= convertFacCF2NTLzzpX (f);
  vec_zz_p roots= FindRoots (NTL_f);
  int *res=NEW_ARRAY(int,1+roots.length());
  res[0]=roots.length();
  for(int i=roots.length()-1;i>=0;i--)
  {
    res[i+1]=to_long(rep(roots[i]));
  }
  return res;
  #else
  factoryError("NTL/FLINT missing: Zp_roots");
  return NULL;
  #endif
}

