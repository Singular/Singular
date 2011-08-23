#ifndef CF_MAP_EXT_H
#define CF_MAP_EXT_H
// -*- c++ -*-
//*****************************************************************************
/** @file cf_map_ext.h
 *
 * @author Martin Lee
 * @date   16.11.2009
 *
 * This file implements functions to map between extensions of finite fields
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @internal
 * @version \$Id$
 *
**/
//*****************************************************************************

// #include "config.h"

int findItem (const CFList& list, const CanonicalForm& item);

CanonicalForm getItem (const CFList& list, const int& pos);

CanonicalForm GFMapUp (const CanonicalForm & F, int k);

CanonicalForm GFMapDown (const CanonicalForm & F, int k);

CanonicalForm
mapUp (const CanonicalForm& F, const Variable& alpha, const Variable& beta,
        const CanonicalForm& prim_elem, const CanonicalForm& im_prim_elem,
        CFList& source, CFList& dest);

CanonicalForm
mapDown (const CanonicalForm& F, const CanonicalForm& prim_elem, const
          CanonicalForm& im_prim_elem, const Variable& alpha, CFList& source,
          CFList& dest);

CanonicalForm
primitiveElement (const Variable& alpha, Variable& beta, bool fail);

CanonicalForm
mapPrimElem (const CanonicalForm& prim_elem, const Variable& alpha,
             const Variable& beta);

CanonicalForm GF2FalphaRep (const CanonicalForm& F, const Variable& alpha);

CanonicalForm Falpha2GFRep (const CanonicalForm& F);

/// map from \f$ F_p(\alpha) \f$ to \f$ F_p(\beta) \f$ such that 
/// \f$ F\in\F_p(\alpha) \f$ is mapped onto \f$ \beta \f$
///
/// @return @a map returns the image of @a primElem such that the above 
/// described properties hold
CanonicalForm
map (const CanonicalForm& primElem,///< [in] primitive element of 
                                   ///< \f$ F_p (\alpha) \f$
     const Variable& alpha,        ///< [in] algebraic variable
     const CanonicalForm& F,       ///< [in] an element of \f$ F_p (\alpha) \f$,
                                   ///< whose minimal polynomial defines a field
                                   ///< extension of \f$ F_p \f$ of degree
                                   ///< \f$ \[F_p (\alpha):F_p\] \f$
     const Variable& beta          ///< [in] algebraic variable, root of \a F's
                                   ///< minimal polynomial
    );

/// compute minimal polynomial of \f$ F\in\F_p(\alpha)\backslash\F_p \f$ via NTL
///
/// @return @a findMinPoly computes the minimal polynomial of F
CanonicalForm
findMinPoly (const CanonicalForm& F, ///< [in] an element of 
                                     ///< \f$ \F_p(\alpha)\backslash\F_p \f$
             const Variable& alpha   ///< [in] algebraic variable
            );

#endif
