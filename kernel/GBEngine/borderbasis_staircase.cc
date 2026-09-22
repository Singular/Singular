/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: monomial staircase construction shared by border-basis engines
*/

#include "kernel/mod2.h"

#include <climits>
#include <set>

#include "kernel/GBEngine/borderbasis_staircase.h"
#include "kernel/ideals.h"
#include "polys/monomials/p_polys.h"
#include "reporter/reporter.h"

namespace
{
using borderbasis_kernel::ExponentVector;
using borderbasis_kernel::ExponentVectors;

static bool divides(const ExponentVector &a, const ExponentVector &b)
{
  for (size_t i = 0; i < a.size(); ++i)
    if (a[i] > b[i]) return false;
  return true;
}

static bool isZero(const ExponentVector &a)
{
  for (size_t i = 0; i < a.size(); ++i)
    if (a[i] != 0) return false;
  return true;
}

static bool isStandard(const ExponentVector &a,
                       const ExponentVectors &corners)
{
  for (size_t i = 0; i < corners.size(); ++i)
    if (divides(corners[i], a)) return false;
  return true;
}

static BOOLEAN leadingExponentVectors(const ideal G, const int firstVariable,
                                      const int numberOfVariables,
                                      ExponentVectors &corners, const ring r)
{
  if ((G == NULL) || (numberOfVariables <= 0))
  {
    WerrorS("border basis: expected a nonzero standard basis");
    return TRUE;
  }

  for (int i = 0; i < IDELEMS(G); ++i)
  {
    if (G->m[i] == NULL) continue;

    ExponentVector a(numberOfVariables, 0);
    for (int j = 0; j < numberOfVariables; ++j)
      a[j] = p_GetExp(G->m[i], firstVariable + j, r);

    if (isZero(a))
    {
      WerrorS("border basis: the localized ideal is the unit ideal");
      return TRUE;
    }
    corners.push_back(a);
  }

  if (corners.empty())
  {
    WerrorS("border basis: expected a nonzero standard basis");
    return TRUE;
  }
  return FALSE;
}

static BOOLEAN standardMonomials(const ExponentVectors &corners,
                                 ExponentVectors &standard)
{
  const int variables = static_cast<int>(corners[0].size());
  ExponentVector bounds(variables, 0);

  for (int variable = 0; variable < variables; ++variable)
  {
    int bound = INT_MAX;
    for (size_t i = 0; i < corners.size(); ++i)
    {
      bool purePower = corners[i][variable] > 0;
      for (int j = 0; purePower && j < variables; ++j)
        if ((j != variable) && (corners[i][j] != 0)) purePower = false;
      if (purePower && (corners[i][variable] < bound))
        bound = corners[i][variable];
    }

    if (bound == INT_MAX)
    {
      WerrorS("border basis: the quotient does not have finite rank");
      return TRUE;
    }
    bounds[variable] = bound;
  }

  size_t boxSize = 1;
  for (int i = 0; i < variables; ++i)
  {
    if (boxSize > static_cast<size_t>(INT_MAX) /
                      static_cast<size_t>(bounds[i]))
    {
      WerrorS("border basis: the standard monomial box is too large");
      return TRUE;
    }
    boxSize *= static_cast<size_t>(bounds[i]);
  }

  ExponentVector a(variables, 0);
  for (size_t count = 0; count < boxSize; ++count)
  {
    if (isStandard(a, corners)) standard.push_back(a);

    for (int j = variables - 1; j >= 0; --j)
    {
      ++a[j];
      if (a[j] < bounds[j]) break;
      a[j] = 0;
    }
  }

  if (standard.empty())
  {
    WerrorS("border basis: the localized ideal is the unit ideal");
    return TRUE;
  }
  return FALSE;
}

}

namespace borderbasis_kernel
{
void border(const ExponentVectors &standard, ExponentVectors &result)
{
  std::set<ExponentVector> orderSet(standard.begin(), standard.end());
  std::set<ExponentVector> borderSet;

  for (size_t i = 0; i < standard.size(); ++i)
  {
    for (size_t variable = 0; variable < standard[i].size(); ++variable)
    {
      ExponentVector b = standard[i];
      ++b[variable];
      if (orderSet.find(b) == orderSet.end()) borderSet.insert(b);
    }
  }
  result.assign(borderSet.begin(), borderSet.end());
}

poly makeMonomial(const ExponentVector &a, const int firstVariable,
                  const ring r)
{
  poly p = p_One(r);
  for (size_t i = 0; i < a.size(); ++i)
    p_SetExp(p, firstVariable + static_cast<int>(i), a[i], r);
  p_Setm(p, r);
  return p;
}

ideal makeMonomialIdeal(const ExponentVectors &monomials,
                        const int firstVariable, const ring r)
{
  ideal result = idInit(static_cast<int>(monomials.size()), 1);
  for (size_t i = 0; i < monomials.size(); ++i)
    result->m[i] = makeMonomial(monomials[i], firstVariable, r);
  return result;
}

BOOLEAN staircase(const ideal G, const int firstVariable,
                  const int numberOfVariables, ExponentVectors &standard,
                  ExponentVectors &border, const ring r)
{
  ExponentVectors corners;
  if (leadingExponentVectors(G, firstVariable, numberOfVariables, corners, r))
    return TRUE;
  if (standardMonomials(corners, standard)) return TRUE;
  borderbasis_kernel::border(standard, border);
  return FALSE;
}
}
