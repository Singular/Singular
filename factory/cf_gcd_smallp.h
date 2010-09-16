#ifndef CF_GCD_SMALL_H
#define CF_GCD_SMALL_H
// -*- c++ -*-
//*****************************************************************************
/** @file cf_gcd_smallp.h
 *
 * @author Martin Lee	
 * @date   22.10.2009
 *
 * This file defines the functions GCD_Fp_extension which computes the GCD of 
 * two polynomials over \f$ F_{p}(\alpha ) \f$ , GCD_small_p which computes the 
 * GCD of two polynomials over  \f$ F_{p} \f$ , and GCD_GF which computes the 
 * GCD of two polynomials over GF. Algorithms are based on "On the Genericity of 
 * the Modular Polynomial GCD Algorithm" by E. Kaltofen & M. Monagan
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @internal 
 * @version \$Id$
 *
**/
//*****************************************************************************

#include <config.h>
#include "assert.h" 

CanonicalForm GCD_Fp_extension (const CanonicalForm& F, const CanonicalForm& G,
                  Variable & alpha, CFList& l, bool& top_level);

/// GCD of A and B over \f$ F_{p}(\alpha ) \f$ 
static inline CanonicalForm GCD_Fp_extension (const CanonicalForm& A, const CanonicalForm& B, 
                                Variable & alpha) 
{
  CFList list;
  bool top_level= true;
  return GCD_Fp_extension (A, B, alpha, list, top_level);
}


CanonicalForm GCD_small_p (const CanonicalForm& F, const CanonicalForm&  G,
                           bool& top_level, CFList& l);

///GCD of A and B over \f$ F_{p} \f$
static inline CanonicalForm GCD_small_p (const CanonicalForm& A, const CanonicalForm& B)
{
  CFList list;
  bool top_level= true;
  return GCD_small_p (A, B, top_level, list);
}

CanonicalForm GCD_GF (const CanonicalForm& F, const CanonicalForm& G, CFList& l,
        bool& top_level);

/// GCD of A and B over GF
static inline CanonicalForm GCD_GF (const CanonicalForm& A, const CanonicalForm& B) 
{
  ASSERT (CFFactory::gettype() == GaloisFieldDomain, 
          "GF as base field expected");
  CFList list;
  bool top_level= true;
  return GCD_GF (A, B, list, top_level);
}

CanonicalForm 
randomIrredpoly (int i, const Variable & x) ;
#endif
