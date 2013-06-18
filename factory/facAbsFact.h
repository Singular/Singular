/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAbsFact.h
 *
 * bivariate absolute factorization over Q described in "Modular Las Vegas
 * Algorithms for Polynomial Absolute Factorization" by Bertone, Chèze, Galligo
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_ABS_FACT_H
#define FAC_ABS_FACT_H

#include "cf_assert.h"

#include "cf_algorithm.h"
#include "cf_map.h"

#ifdef HAVE_NTL
/// main absolute factorization routine, expects bivariate poly which is
/// primitive wrt. any of its variables and irreducible over Q
///
/// @return absFactorizeMain returns a list whose entries contain three entities:
///         an absolute irreducible factor, an irreducible univariate polynomial
///         that defines the minimal field extension over which the irreducible
///         factor is defined and the multiplicity of the absolute irreducible
///         factor
CFAFList absFactorizeMain (const CanonicalForm& F ///<[in] s.a.
                          );
#endif

/// normalize factors, i.e. make factors monic
static inline
void normalize (CFAFList & L)
{
  for (CFAFListIterator i= L; i.hasItem(); i++)
    i.getItem()= CFAFactor (i.getItem().factor()/Lc (i.getItem().factor()),
                            i.getItem().minpoly(), i.getItem().exp());
}

/// univariate absolute factorization over Q
///
/// @return uniAbsFactorize returns a list whose entries contain three entities:
///         an absolute irreducible factor, an irreducible univariate polynomial
///         that defines the minimal field extension over which the irreducible
///         factor is defined and the multiplicity of the absolute irreducible
///         factor
static inline
CFAFList uniAbsFactorize (const CanonicalForm& F ///<[in] univariate poly over Q
                         )
{
  CFFList rationalFactors= factorize (F);
  CFFListIterator i= rationalFactors;
  i++;
  Variable alpha;
  CFAFList result;
  CFFList QaFactors;
  CFFListIterator iter;
  for (; i.hasItem(); i++)
  {
    if (degree (i.getItem().factor()) == 1)
    {
      result.append (CFAFactor (i.getItem().factor(), 1, i.getItem().exp()));
      continue;
    }
    alpha= rootOf (i.getItem().factor());
    QaFactors= factorize (i.getItem().factor(), alpha);
    iter= QaFactors;
    if (iter.getItem().factor().inCoeffDomain())
      iter++;
    for (;iter.hasItem(); iter++)
    {
      if (degree (iter.getItem().factor()) == 1)
      {
        result.append (CFAFactor (iter.getItem().factor(), getMipo (alpha),
                                  i.getItem().exp()));
        break;
      }
    }
  }
  result.insert (CFAFactor (rationalFactors.getFirst().factor(), 1, 1));
  return result;
}

/*BEGINPUBLIC*/

#ifdef HAVE_NTL
CFAFList absFactorize (const CanonicalForm& G);
#endif

/*ENDPUBLIC*/

#endif
