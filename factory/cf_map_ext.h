// -*- c++ -*-
//*****************************************************************************
/** @file cf_map_ext.h
 *
 * This file implements functions to map between extensions of finite fields
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @author Martin Lee
 * @date   16.11.2009
**/
//*****************************************************************************

#ifndef CF_MAP_EXT_H
#define CF_MAP_EXT_H

// #include "config.h"

int findItem (const CFList& list, const CanonicalForm& item);

CanonicalForm getItem (const CFList& list, const int& pos);

/// maps a polynomial over \f$ GF(p^{k}) \f$ to a polynomial over
/// \f$ GF(p^{d}) \f$ , d needs to be a multiple of k
CanonicalForm GFMapUp (const CanonicalForm & F, int k);

/// maps a polynomial over \f$ GF(p^{d}) \f$ to a polynomial over
/// \f$ GF(p^{k})\f$ , d needs to be a multiple of k
CanonicalForm GFMapDown (const CanonicalForm & F, int k);

/// map F from \f$ F_{p} (\alpha ) \f$ to \f$ F_{p}(\beta ) \f$.
/// We assume \f$ F_{p} (\alpha ) \subset F_{p}(\beta ) \f$.
CanonicalForm
mapUp (const CanonicalForm& F,           ///<[in] poly over \f$ F_{p} (\alpha ) \f$
       const Variable& alpha,            ///<[in] alg. variable
       const Variable& beta,             ///<[in] alg. variable
       const CanonicalForm& prim_elem,   ///<[in] primitive element of \f$ F_{p} (\alpha ) \f$
       const CanonicalForm& im_prim_elem,///<[in] image of prim_elem in \f$ F_{p} (\beta ) \f$
       CFList& source,                   ///<[in,out] look up lists
       CFList& dest                      ///<[in,out] look up lists
      );

/// map F from \f$ F_{p} (\beta ) \f$ to \f$ F_{p}(\alpha ) \f$.
/// We assume \f$ F_{p} (\alpha ) \subset F_{p}(\beta ) \f$ and F in
/// \f$ F_{p}(\alpha ) \f$.
CanonicalForm
mapDown (const CanonicalForm& F,           ///<[in] poly over \f$ F_{p} (\beta ) \f$
         const CanonicalForm& prim_elem,   ///<[in] primitive element of \f$ F_{p} (\alpha ) \f$
         const CanonicalForm& im_prim_elem,///<[in] image of prim_elem in \f$ F_{p} (\beta ) \f$
         const Variable& alpha,            ///<[in] alg. variable
         CFList& source,                   ///<[in,out] look up lists
         CFList& dest                      ///<[in,out] look up lists
        );

/// determine a primitive element of \f$ F_{p} (\alpha ) \f$,
/// \f$ \beta \f$ is a primitive element of a field which is isomorphic to
/// \f$ F_{p}(\alpha ) \f$
CanonicalForm
primitiveElement (const Variable& alpha, ///< [in] some algebraic variable
                  Variable& beta,        ///< [in,out] s.a.
                  bool& fail             ///< [in,out] failure due to integer
                                         ///< factorization failure?
                 );

/// compute the image of a primitive element of \f$ F_{p} (\alpha ) \f$
/// in \f$ F_{p}(\beta ) \f$.
/// We assume \f$ F_{p} (\alpha ) \subset F_{p}(\beta ) \f$.
CanonicalForm
mapPrimElem (const CanonicalForm& prim_elem, ///< [in] primitive element
             const Variable& alpha,          ///< [in] algebraic variable
             const Variable& beta            ///< [in] algebraic variable
            );

/// changes representation by primitive element to representation by residue
/// classes modulo a Conway polynomial
CanonicalForm GF2FalphaRep (const CanonicalForm& F, ///< [in] some poly over GF
                            const Variable& alpha   ///< [in] root of a Conway
                                                    ///< poly
                           );

/// change representation by residue classes modulo a Conway polynomial
/// to representation by primitive element
CanonicalForm Falpha2GFRep (const CanonicalForm& F ///<[in] some poly over
                                                   ///< F_p(alpha) where alpha
                                                   ///< is a root of a Conway
                                                   ///< poly
                           );

/// map from \f$ F_p(\alpha) \f$ to \f$ F_p(\beta) \f$ such that
/// \f$ F\in F_p(\alpha) \f$ is mapped onto \f$ \beta \f$
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
                                   ///< \f$ F_p (\alpha):F_p \f$
     const Variable& beta          ///< [in] algebraic variable, root of \a F's
                                   ///< minimal polynomial
    );

/// compute minimal polynomial of \f$ F\in F_p(\alpha)\backslash F_p \f$ via NTL
///
/// @return @a findMinPoly computes the minimal polynomial of F
CanonicalForm
findMinPoly (const CanonicalForm& F, ///< [in] an element of
                                     ///< \f$ F_p(\alpha)\backslash F_p \f$
             const Variable& alpha   ///< [in] algebraic variable
            );

#endif
