#ifndef CF_MOD_GCD_H
#define CF_MOD_GCD_H
// -*- c++ -*-
//*****************************************************************************
/** @file cfModGcd.h
 *
 * modular and sparse modular GCD algorithms over finite fields and Z.
 *
 * @author Martin Lee
 * @date   22.10.2009
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
**/
//*****************************************************************************

// #include "config.h"
#include "cf_assert.h"

#include "cf_factory.h"

CanonicalForm modGCDFq (const CanonicalForm& F, const CanonicalForm& G,
                        Variable & alpha, CFList& l, bool& top_level);

/// GCD of A and B over \f$ F_{p}(\alpha ) \f$
static inline
CanonicalForm modGCDFq (const CanonicalForm& A, ///<[in] poly over F_q
                        const CanonicalForm& B, ///<[in] poly over F_q
                        Variable & alpha        ///<[in] algebraic variable
                       )
{
  CFList list;
  bool top_level= true;
  return modGCDFq (A, B, alpha, list, top_level);
}


CanonicalForm
modGCDFp (const CanonicalForm& F, const CanonicalForm& G, bool& top_level,
          CFList& l);

CanonicalForm
modGCDFp (const CanonicalForm& F, const CanonicalForm& G,
          CanonicalForm& coF, CanonicalForm& coG,
          bool& topLevel, CFList& l);

///GCD of A and B over \f$ F_{p} \f$
static inline
CanonicalForm modGCDFp (const CanonicalForm& A, ///<[in] poly over F_p
                        const CanonicalForm& B  ///<[in] poly over F_p
                       )
{
  CFList list;
  bool top_level= true;
  return modGCDFp (A, B, top_level, list);
}

static inline
CanonicalForm modGCDFp (const CanonicalForm& A, const CanonicalForm& B,
                        CanonicalForm& coA, CanonicalForm& coB)
{
  CFList list;
  bool top_level= true;
  return modGCDFp (A, B, coA, coB, top_level, list);
}

CanonicalForm
modGCDGF (const CanonicalForm& F, const CanonicalForm& G, CFList& l,
          bool& top_level);

/// GCD of A and B over GF
static inline
CanonicalForm modGCDGF (const CanonicalForm& A, ///<[in] poly over GF
                        const CanonicalForm& B  ///<[in] poly over GF
                       )
{
  ASSERT (CFFactory::gettype() == GaloisFieldDomain,
          "GF as base field expected");
  CFList list;
  bool top_level= true;
  return modGCDGF (A, B, list, top_level);
}

CanonicalForm sparseGCDFp (const CanonicalForm& F, const CanonicalForm& G,
                           bool& topLevel, CFList& l);

/// Zippel's sparse GCD over Fp
static inline
CanonicalForm sparseGCDFp (const CanonicalForm& A, ///<[in] poly over F_p
                           const CanonicalForm& B  ///<[in] poly over F_p
                          )
{
  ASSERT (CFFactory::gettype() == FiniteFieldDomain,
          "Fp as base field expected");
  CFList list;
  bool topLevel= true;
  return sparseGCDFp (A, B, topLevel, list);
}


CanonicalForm
sparseGCDFq (const CanonicalForm& F, const CanonicalForm& G,
             const Variable& alpha, CFList& l, bool& topLevel);

/// Zippel's sparse GCD over Fq
static inline
CanonicalForm sparseGCDFq (const CanonicalForm& A, ///<[in] poly over F_q
                           const CanonicalForm& B, ///<[in] poly over F_q
                           const Variable& alpha   ///<[in] algebraic variable
                          )
{
  CFList list;
  bool topLevel= true;
  return sparseGCDFq (A, B, alpha, list, topLevel);
}

/// extract monomials of F, parts in algebraic variable are considered
/// coefficients
CFArray
getMonoms (const CanonicalForm& F ///<[in] some poly
          );

bool
terminationTest (const CanonicalForm& F, const CanonicalForm& G,
                 const CanonicalForm& coF, const CanonicalForm& coG,
                 const CanonicalForm& cand);

/// modular GCD over Z
CanonicalForm modGCDZ (const CanonicalForm & FF, ///<[in] poly over Z
                       const CanonicalForm & GG  ///<[in] poly over Z
                      );
#endif
