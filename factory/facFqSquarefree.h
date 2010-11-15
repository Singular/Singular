/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqSquarefree.h
 *
 * This file provides functions for squarefrees factorizing over
 * \f$ F_{p} \f$ , \f$ F_{p}(\alpha ) \f$ or GF.
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef FAC_FQ_SQUAREFREE_H
#define FAC_FQ_SQUAREFREE_H

#include "assert.h"


/// squarefree factorization over a finite field
/// @a return a list of squarefree factors with multiplicity
CFFList
squarefreeFactorization
                (const CanonicalForm & F, ///<[in] a poly
                 const Variable & alpha   ///<[in] either an algebraic variable,
                                          ///< i.e. we are over some F_p (alpha)
                                          ///< or a variable of level 1, i.e.
                                          ///< we are F_p or GF
                );

/// squarefree factorization over \f$ F_{p} \f$.
/// If input is not monic, the leading coefficient is dropped
///
/// @return a list of squarefree factors with multiplicity
inline
CFFList FpSqrf (const CanonicalForm& F ///< [in] a poly
               )
{
  Variable a= 1;
  CFFList result= squarefreeFactorization (F, a);
  result.insert (CFFactor (Lc(F), 1));
  return result;
}

/// squarefree factorization over \f$ F_{p}(\alpha ) \f$.
/// If input is not monic, the leading coefficient is dropped
///
/// @return a list of squarefree factors with multiplicity
inline
CFFList FqSqrf (const CanonicalForm& F, ///< [in] a poly
                const Variable& alpha   ///< [in] algebraic variable
               )
{
  CFFList result= squarefreeFactorization (F, alpha);
  result.insert (CFFactor (Lc(F), 1));
  return result;
}

/// squarefree factorization over GF.
/// If input is not monic, the leading coefficient is dropped
///
/// @return a list of squarefree factors with multiplicity
inline
CFFList GFSqrf (const CanonicalForm& F ///< [in] a poly
               )
{
  ASSERT (CFFactory::gettype() == GaloisFieldDomain,
          "GF as base field expected");
  Variable a= 1;
  CFFList result= squarefreeFactorization (F, a);
  result.insert (CFFactor (Lc(F), 1));
  return result;
}

/// squarefree factorization of @a F wrt @x
///
/// @return a list of factors of @a F which are squarefree wrt x
static inline
CFFList
sqrfPosDer (const CanonicalForm & F, ///< [in] some poly
            const Variable & x,      ///< [in] a variable s.t. deriv (F, x) != 0
            const int & k,           ///< [in] GFDegree or 1
            const Variable & alpha,  ///< [in] algebraic variable or Variable(1)
            CanonicalForm & c        ///< [in,out] F divided by the product of
                                     ///< the result
           );

/// squarefree part of @a F/g, where g is the product of those squarefree
/// factors whose multiplicity is 0 mod p, if @a F a pth power pthPower= F.
///
/// @return @a sqrfPart returns 1, if F is a pthPower, else it returns the
///         squarefree part of @a F/g, where g is the product of those
///         squarefree factors whose multiplicity is 0 mod p
CanonicalForm
sqrfPart (const CanonicalForm& F,  ///< [in] a poly
          CanonicalForm& pthPower, ///< [in,out] returns F is F is a pthPower
          const Variable& alpha    ///< [in] algebraic variable
         );

/// pth root extraction
///
/// @return @a pthRoot returns a pth root of @a F
/// @sa maxpthRoot()
static inline
CanonicalForm
pthRoot (const CanonicalForm & F, ///< [in] a poly which is a pth power
         const int & q            ///< [in] size of the field
        );

/// p^l-th root extraction, where l is maximal
///
/// @return @a maxpthRoot returns a p^l-th root of @a F, where @a l is maximal
/// @sa pthRoot()
CanonicalForm
maxpthRoot (const CanonicalForm & F, ///< [in] a poly which is a pth power
            const int & q,           ///< [in] size of the field
            int& l                   ///< [in,out] @a l maximal, s.t. @a F is
                                     ///< a p^l-th power
           );

#endif
/* FAC_FQ_SQUAREFREE_H */

