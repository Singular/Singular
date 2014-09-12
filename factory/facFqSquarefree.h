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
 **/
/*****************************************************************************/

#ifndef FAC_FQ_SQUAREFREE_H
#define FAC_FQ_SQUAREFREE_H

#include "cf_assert.h"
#include "cf_factory.h"
#include "fac_sqrfree.h"
#include "cf_factory.h"

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
CFFList FpSqrf (const CanonicalForm& F, ///< [in] a poly
                bool sort= true         ///< [in] sort factors by exponent?
               )
{
  Variable a= 1;
  int n= F.level();
  CanonicalForm cont, bufF= F;
  CFFList bufResult;

  CFFList result;
  for (int i= n; i >= 1; i++)
  {
    cont= content (bufF, i);
    bufResult= squarefreeFactorization (cont, a);
    if (bufResult.getFirst().factor().inCoeffDomain())
      bufResult.removeFirst();
    result= Union (result, bufResult);
    bufF /= cont;
    if (bufF.inCoeffDomain())
      break;
  }
  if (!bufF.inCoeffDomain())
  {
    bufResult= squarefreeFactorization (bufF, a);
    if (bufResult.getFirst().factor().inCoeffDomain())
      bufResult.removeFirst();
    result= Union (result, bufResult);
  }
  if (sort)
    result= sortCFFList (result);
  result.insert (CFFactor (Lc(F), 1));
  return result;
}

/// squarefree factorization over \f$ F_{p}(\alpha ) \f$.
/// If input is not monic, the leading coefficient is dropped
///
/// @return a list of squarefree factors with multiplicity
inline
CFFList FqSqrf (const CanonicalForm& F, ///< [in] a poly
                const Variable& alpha,  ///< [in] algebraic variable
                bool sort= true         ///< [in] sort factors by exponent?
               )
{
  int n= F.level();
  CanonicalForm cont, bufF= F;
  CFFList bufResult;

  CFFList result;
  for (int i= n; i >= 1; i++)
  {
    cont= content (bufF, i);
    bufResult= squarefreeFactorization (cont, alpha);
    if (bufResult.getFirst().factor().inCoeffDomain())
      bufResult.removeFirst();
    result= Union (result, bufResult);
    bufF /= cont;
    if (bufF.inCoeffDomain())
      break;
  }
  if (!bufF.inCoeffDomain())
  {
    bufResult= squarefreeFactorization (bufF, alpha);
    if (bufResult.getFirst().factor().inCoeffDomain())
      bufResult.removeFirst();
    result= Union (result, bufResult);
  }
  if (sort)
    result= sortCFFList (result);
  result.insert (CFFactor (Lc(F), 1));
  return result;
}

/// squarefree factorization over GF.
/// If input is not monic, the leading coefficient is dropped
///
/// @return a list of squarefree factors with multiplicity
inline
CFFList GFSqrf (const CanonicalForm& F, ///< [in] a poly
                bool sort= true         ///< [in] sort factors by exponent?
               )
{
  ASSERT (CFFactory::gettype() == GaloisFieldDomain,
          "GF as base field expected");
  return FpSqrf (F, sort);
}

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

/// p^l-th root extraction, where l is maximal
///
/// @return @a maxpthRoot returns a p^l-th root of @a F, where @a l is maximal
/// @sa pthRoot()
CanonicalForm
maxpthRoot (const CanonicalForm & F, ///< [in] a poly which is a pth power
            int q,                   ///< [in] size of the field
            int& l                   ///< [in,out] @a l maximal, s.t. @a F is
                                     ///< a p^l-th power
           );

#endif
/* FAC_FQ_SQUAREFREE_H */

