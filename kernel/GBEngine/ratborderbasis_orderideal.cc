/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: order-independent border bases in rational Weyl algebras
*/

#include "kernel/mod2.h"

#include "kernel/GBEngine/ratborderbasis_orderideal.h"

#ifdef HAVE_RATGRING

#include <algorithm>
#include <climits>
#include <set>
#include <vector>

#include "kernel/GBEngine/borderbasis_staircase.h"
#include "kernel/GBEngine/ratgring.h"
#include "kernel/ideals.h"
#include "kernel/polys.h"
#include "polys/clapsing.h"
#include "polys/monomials/p_polys.h"
#include "polys/monomials/ring.h"
#include "polys/nc/nc.h"
#include "reporter/reporter.h"

namespace
{
using borderbasis_kernel::ExponentVector;
using borderbasis_kernel::ExponentVectors;

static int exponentDegree(const ExponentVector &a)
{
  int degree = 0;
  for (size_t i = 0; i < a.size(); ++i) degree += a[i];
  return degree;
}

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

static bool sameOperatorExponent(const poly term, const ExponentVector &a,
                                 const int firstOperator, const ring r)
{
  for (size_t i = 0; i < a.size(); ++i)
    if (p_GetExp(term, firstOperator + static_cast<int>(i), r) != a[i])
      return false;
  return true;
}

static ExponentVector operatorExponent(const poly term,
                                       const int firstOperator,
                                       const int operators, const ring r)
{
  ExponentVector a(operators, 0);
  for (int i = 0; i < operators; ++i)
    a[i] = p_GetExp(term, firstOperator + i, r);
  return a;
}

static bool hasOperatorCoefficient(const poly p, const ExponentVector &a,
                                   const int firstOperator, const ring r)
{
  for (poly term = p; term != NULL; term = pNext(term))
    if (sameOperatorExponent(term, a, firstOperator, r)) return true;
  return false;
}

// Extract the K[x]-coefficient of an arbitrary differential monomial.  This
// deliberately does not use the leading term of p or the ring ordering.
static poly coefficientAt(const poly p, const ExponentVector &a,
                          const int firstOperator, const ring r)
{
  poly coefficient = NULL;
  for (poly term = p; term != NULL; term = pNext(term))
  {
    if (!sameOperatorExponent(term, a, firstOperator, r)) continue;

    poly summand = p_Head(term, r);
    for (size_t i = 0; i < a.size(); ++i)
      p_SetExp(summand, firstOperator + static_cast<int>(i), 0, r);
    p_SetCompP(summand, 0, r);
    p_Setm(summand, r);
    coefficient = p_Add_q(coefficient, summand, r);
  }
  return coefficient;
}

static void operatorSupport(const poly p, const int firstOperator,
                            const int operators,
                            std::set<ExponentVector> &support, const ring r)
{
  for (poly term = p; term != NULL; term = pNext(term))
    support.insert(operatorExponent(term, firstOperator, operators, r));
}

// Replace target by a fraction-free row operation which cancels column a.
static BOOLEAN eliminateAt(poly &target, const poly pivot,
                           const ExponentVector &a,
                           const int firstOperator, const ring r)
{
  poly targetCoefficient = coefficientAt(target, a, firstOperator, r);
  poly pivotCoefficient = coefficientAt(pivot, a, firstOperator, r);
  if ((targetCoefficient == NULL) || (pivotCoefficient == NULL))
  {
    p_Delete(&targetCoefficient, r);
    p_Delete(&pivotCoefficient, r);
    WerrorS("order-ideal border basis: internal zero pivot");
    return TRUE;
  }

  // ncGCD returns (u,v) with u*targetCoefficient+v*pivotCoefficient=0.
  ideal multipliers = ncGCD(targetCoefficient, pivotCoefficient, r);
  poly targetMultiplier = multipliers->m[0];
  multipliers->m[0] = NULL;
  poly pivotMultiplier = multipliers->m[1];
  multipliers->m[1] = NULL;
  id_Delete(&multipliers, r);

  poly left = p_Mult_q(targetMultiplier, target, r);
  poly right = p_Mult_q(pivotMultiplier, p_Copy(pivot, r), r);
  target = p_Add_q(left, right, r);
  return FALSE;
}

struct EliminationOrder
{
  bool operator()(const ExponentVector &a, const ExponentVector &b) const
  {
    const int da = exponentDegree(a);
    const int db = exponentDegree(b);
    if (da != db) return da > db;
    return b < a;
  }
};

static void appendMonomialsOfDegree(const int variables, const int position,
                                    const int remaining, ExponentVector &a,
                                    ExponentVectors &result)
{
  if (position + 1 == variables)
  {
    a[position] = remaining;
    result.push_back(a);
    return;
  }

  for (int exponent = 0; exponent <= remaining; ++exponent)
  {
    a[position] = exponent;
    appendMonomialsOfDegree(variables, position + 1,
                            remaining - exponent, a, result);
  }
}

static void monomialsThroughDegree(const int variables, const int degree,
                                   ExponentVectors &result)
{
  ExponentVector a(variables, 0);
  for (int d = 0; d <= degree; ++d)
    appendMonomialsOfDegree(variables, 0, d, a, result);
}

static poly leftProlongation(const ExponentVector &a, const poly p,
                             const int firstOperator, const ring r)
{
  if (isZero(a)) return p_Copy(p, r);
  poly multiplier =
      borderbasis_kernel::makeMonomial(a, firstOperator, r);
  poly result = nc_mm_Mult_p(multiplier, p_Copy(p, r), r);
  p_Delete(&multiplier, r);
  return result;
}

static void deleteRows(std::vector<poly> &rows, const ring r)
{
  for (size_t i = 0; i < rows.size(); ++i) p_Delete(&rows[i], r);
  rows.clear();
}

static void macaulayRows(const ideal I, const int prolongationDegree,
                         const int firstOperator, const int operators,
                         std::vector<poly> &rows, const ring r)
{
  ExponentVectors multipliers;
  monomialsThroughDegree(operators, prolongationDegree, multipliers);
  for (size_t i = 0; i < multipliers.size(); ++i)
  {
    for (int j = 0; j < IDELEMS(I); ++j)
    {
      if (I->m[j] == NULL) continue;
      poly row = leftProlongation(multipliers[i], I->m[j], firstOperator, r);
      if (row != NULL) rows.push_back(row);
    }
  }
}

// Fraction-free Gauss-Jordan elimination over K(x).  Only differential
// monomials outside O are pivot columns.  The column ordering is merely a
// deterministic elimination strategy; it does not define an initial ideal.
static BOOLEAN reduceMacaulayRows(std::vector<poly> &rows,
                                  const std::set<ExponentVector> &orderSet,
                                  const int firstOperator,
                                  const int operators, const ring r)
{
  std::set<ExponentVector, EliminationOrder> columnSet;
  for (size_t i = 0; i < rows.size(); ++i)
  {
    std::set<ExponentVector> support;
    operatorSupport(rows[i], firstOperator, operators, support, r);
    for (std::set<ExponentVector>::const_iterator it = support.begin();
         it != support.end(); ++it)
      if (orderSet.find(*it) == orderSet.end()) columnSet.insert(*it);
  }

  size_t pivotRow = 0;
  for (std::set<ExponentVector, EliminationOrder>::const_iterator column =
           columnSet.begin();
       (column != columnSet.end()) && (pivotRow < rows.size()); ++column)
  {
    size_t selected = pivotRow;
    while ((selected < rows.size()) &&
           !hasOperatorCoefficient(rows[selected], *column, firstOperator, r))
      ++selected;
    if (selected == rows.size()) continue;

    if (selected != pivotRow) std::swap(rows[selected], rows[pivotRow]);
    for (size_t row = 0; row < rows.size(); ++row)
    {
      if ((row == pivotRow) || (rows[row] == NULL) ||
          !hasOperatorCoefficient(rows[row], *column, firstOperator, r))
        continue;
      if (eliminateAt(rows[row], rows[pivotRow], *column, firstOperator, r))
        return TRUE;
    }
    ++pivotRow;
  }
  return FALSE;
}

static BOOLEAN readOrderIdeal(const ideal input, const int coefficientVariables,
                              ExponentVectors &order,
                              std::set<ExponentVector> &orderSet, const ring r)
{
  const int operators = r->N - coefficientVariables;
  if ((input == NULL) || (IDELEMS(input) <= 0))
  {
    WerrorS("order-ideal border basis: expected a nonempty order ideal");
    return TRUE;
  }

  bool containsOne = false;
  for (int i = 0; i < IDELEMS(input); ++i)
  {
    const poly p = input->m[i];
    if ((p == NULL) || (pNext(p) != NULL) || (p_GetComp(p, r) != 0))
    {
      WerrorS("order-ideal border basis: entries must be differential monomials");
      return TRUE;
    }
    for (int variable = 1; variable <= coefficientVariables; ++variable)
    {
      if (p_GetExp(p, variable, r) != 0)
      {
        WerrorS("order-ideal border basis: entries must not contain coefficient variables");
        return TRUE;
      }
    }

    ExponentVector a =
        operatorExponent(p, coefficientVariables + 1, operators, r);
    if (!orderSet.insert(a).second)
    {
      WerrorS("order-ideal border basis: duplicate differential monomial");
      return TRUE;
    }
    if (isZero(a)) containsOne = true;
    order.push_back(a);
  }

  if (!containsOne)
  {
    WerrorS("order-ideal border basis: the order ideal must contain 1");
    return TRUE;
  }

  for (size_t i = 0; i < order.size(); ++i)
  {
    for (int variable = 0; variable < operators; ++variable)
    {
      if (order[i][variable] == 0) continue;
      ExponentVector predecessor = order[i];
      --predecessor[variable];
      if (orderSet.find(predecessor) == orderSet.end())
      {
        WerrorS("order-ideal border basis: the monomials are not divisor closed");
        return TRUE;
      }
    }
  }
  return FALSE;
}

static void outsideSupport(const poly p,
                           const std::set<ExponentVector> &orderSet,
                           const int firstOperator, const int operators,
                           ExponentVectors &outside, const ring r)
{
  std::set<ExponentVector> support;
  operatorSupport(p, firstOperator, operators, support, r);
  for (std::set<ExponentVector>::const_iterator it = support.begin();
       it != support.end(); ++it)
    if (orderSet.find(*it) == orderSet.end()) outside.push_back(*it);
}

// Remove a common polynomial factor from all K[x]-coefficients without using
// the ring's leading-term grouping (which is intentionally irrelevant here).
static void makePrimitive(poly &p, const int firstOperator,
                          const int operators, const ring r)
{
  if (p == NULL) return;
  std::set<ExponentVector> support;
  operatorSupport(p, firstOperator, operators, support, r);

  poly content = NULL;
  for (std::set<ExponentVector>::const_iterator it = support.begin();
       it != support.end(); ++it)
  {
    poly coefficient = coefficientAt(p, *it, firstOperator, r);
    if (content == NULL)
      content = coefficient;
    else
      content = singclap_gcd_r(content, coefficient, r);
    if (p_IsConstant(content, r)) break;
  }

  if ((content == NULL) || p_IsConstant(content, r))
  {
    p_Delete(&content, r);
    p_ContentForGB(p, r);
    return;
  }

  poly primitive = NULL;
  for (std::set<ExponentVector>::const_iterator it = support.begin();
       it != support.end(); ++it)
  {
    poly coefficient = coefficientAt(p, *it, firstOperator, r);
    poly quotient = singclap_pdivide(coefficient, content, r);
    poly monomial = borderbasis_kernel::makeMonomial(*it, firstOperator, r);
    primitive = p_Add_q(primitive, p_Mult_q(quotient, monomial, r), r);
  }
  p_Delete(&content, r);
  p_Delete(&p, r);
  p = primitive;
  p_ContentForGB(p, r);
}

static BOOLEAN extractCandidates(const std::vector<poly> &rows,
                                 const ExponentVectors &border,
                                 const std::set<ExponentVector> &orderSet,
                                 const int firstOperator, const int operators,
                                 ideal &candidate, bool &dependent,
                                 const ring r)
{
  dependent = false;
  candidate = idInit(static_cast<int>(border.size()), 1);
  for (size_t i = 0; i < rows.size(); ++i)
  {
    if (rows[i] == NULL) continue;
    ExponentVectors outside;
    outsideSupport(rows[i], orderSet, firstOperator, operators, outside, r);
    if (outside.empty())
    {
      dependent = true;
      return FALSE;
    }
    if (outside.size() != 1) continue;

    for (size_t j = 0; j < border.size(); ++j)
    {
      if ((candidate->m[j] == NULL) && (outside[0] == border[j]))
      {
        candidate->m[j] = p_Copy(rows[i], r);
        makePrimitive(candidate->m[j], firstOperator, operators, r);
        break;
      }
    }
  }
  return FALSE;
}

static bool completeCandidate(const ideal candidate)
{
  for (int i = 0; i < IDELEMS(candidate); ++i)
    if (candidate->m[i] == NULL) return false;
  return true;
}

static int orderIndex(const ExponentVector &a,
                      const ExponentVectors &order)
{
  int result = INT_MAX;
  for (size_t i = 0; i < order.size(); ++i)
  {
    if (!divides(order[i], a)) continue;
    int difference = 0;
    for (size_t j = 0; j < a.size(); ++j)
      difference += a[j] - order[i][j];
    if (difference < result) result = difference;
  }
  return result;
}

static int borderDivisor(const ExponentVector &a,
                         const ExponentVectors &order,
                         const ExponentVectors &border)
{
  const int index = orderIndex(a, order);
  for (size_t i = 0; i < border.size(); ++i)
  {
    if (divides(border[i], a) &&
        (exponentDegree(a) - exponentDegree(border[i]) == index - 1))
      return static_cast<int>(i);
  }
  return -1;
}

static ExponentVector difference(const ExponentVector &a,
                                 const ExponentVector &b)
{
  ExponentVector result(a.size(), 0);
  for (size_t i = 0; i < a.size(); ++i) result[i] = a[i] - b[i];
  return result;
}

static bool selectReductionColumn(const poly p,
                                  const ExponentVectors &order,
                                  const std::set<ExponentVector> &orderSet,
                                  const int firstOperator,
                                  const int operators, ExponentVector &selected,
                                  const ring r)
{
  std::set<ExponentVector> support;
  operatorSupport(p, firstOperator, operators, support, r);
  bool found = false;
  int selectedIndex = -1;
  int selectedDegree = -1;
  for (std::set<ExponentVector>::const_iterator it = support.begin();
       it != support.end(); ++it)
  {
    if (orderSet.find(*it) != orderSet.end()) continue;
    const int index = orderIndex(*it, order);
    const int degree = exponentDegree(*it);
    if (!found || (index > selectedIndex) ||
        ((index == selectedIndex) && (degree > selectedDegree)) ||
        ((index == selectedIndex) && (degree == selectedDegree) &&
         (selected < *it)))
    {
      selected = *it;
      selectedIndex = index;
      selectedDegree = degree;
      found = true;
    }
  }
  return found;
}

// Algorithm 2.9 of Rodriguez--Sattelberger, specialized to testing whether
// the O-remainder is zero.  The marked border term, rather than a leading
// term selected by a monomial ordering, chooses every reduction.
static BOOLEAN borderRemainder(poly input, const ideal borderBasis,
                               const ExponentVectors &order,
                               const std::set<ExponentVector> &orderSet,
                               const ExponentVectors &border,
                               const int firstOperator, const int operators,
                               poly &remainder, const ring r)
{
  remainder = input;
  size_t steps = 0;
  ExponentVector column;
  while ((remainder != NULL) &&
         selectReductionColumn(remainder, order, orderSet, firstOperator,
                               operators, column, r))
  {
    if (++steps > 100000)
    {
      WerrorS("order-ideal border basis: border division did not terminate");
      p_Delete(&remainder, r);
      return TRUE;
    }

    const int reducer = borderDivisor(column, order, border);
    if (reducer < 0)
    {
      WerrorS("order-ideal border basis: no marked border divisor found");
      p_Delete(&remainder, r);
      return TRUE;
    }
    const ExponentVector multiplierExponent =
        difference(column, border[reducer]);
    poly reducerPolynomial = leftProlongation(multiplierExponent,
                                              borderBasis->m[reducer],
                                              firstOperator, r);
    if (eliminateAt(remainder, reducerPolynomial, column, firstOperator, r))
    {
      p_Delete(&reducerPolynomial, r);
      p_Delete(&remainder, r);
      return TRUE;
    }
    p_Delete(&reducerPolynomial, r);
  }
  return FALSE;
}

static BOOLEAN checkIntegrability(const ideal borderBasis,
                                  const ExponentVectors &order,
                                  const std::set<ExponentVector> &orderSet,
                                  const ExponentVectors &border,
                                  const int firstOperator,
                                  const int operators, bool &integrable,
                                  const ring r)
{
  integrable = true;
  for (size_t j = 0; j < border.size(); ++j)
  {
    for (int variable = 0; variable < operators; ++variable)
    {
      ExponentVector unit(operators, 0);
      unit[variable] = 1;
      ExponentVector overlap = border[j];
      ++overlap[variable];
      const int canonical = borderDivisor(overlap, order, border);
      if (canonical < 0)
      {
        WerrorS("order-ideal border basis: invalid second-border overlap");
        return TRUE;
      }
      const ExponentVector canonicalMultiplier =
          difference(overlap, border[canonical]);
      if ((canonical == static_cast<int>(j)) &&
          (canonicalMultiplier == unit))
        continue;

      poly first = leftProlongation(unit, borderBasis->m[j],
                                    firstOperator, r);
      poly second = leftProlongation(canonicalMultiplier,
                                     borderBasis->m[canonical],
                                     firstOperator, r);
      if (eliminateAt(first, second, overlap, firstOperator, r))
      {
        p_Delete(&first, r);
        p_Delete(&second, r);
        return TRUE;
      }
      p_Delete(&second, r);

      poly reduced = NULL;
      if (borderRemainder(first, borderBasis, order, orderSet, border,
                          firstOperator, operators, reduced, r))
        return TRUE;
      if (reduced != NULL)
      {
        integrable = false;
        p_Delete(&reduced, r);
        return FALSE;
      }
    }
  }
  return FALSE;
}

static BOOLEAN generatorsReduceToZero(const ideal I, const ideal borderBasis,
                                      const ExponentVectors &order,
                                      const std::set<ExponentVector> &orderSet,
                                      const ExponentVectors &border,
                                      const int firstOperator,
                                      const int operators, bool &allZero,
                                      const ring r)
{
  allZero = true;
  for (int i = 0; i < IDELEMS(I); ++i)
  {
    if (I->m[i] == NULL) continue;
    poly reduced = NULL;
    if (borderRemainder(p_Copy(I->m[i], r), borderBasis, order, orderSet,
                        border, firstOperator, operators, reduced, r))
      return TRUE;
    if (reduced != NULL)
    {
      allZero = false;
      p_Delete(&reduced, r);
      return FALSE;
    }
  }
  return FALSE;
}
}

BOOLEAN nc_RationalWeylBorderBasisForOrderIdeal(
    const ideal I, const ideal inputOrderIdeal, const int coefficientVariables,
    const int maxProlongation, ideal &borderBasis, const ring r)
{
  borderBasis = NULL;
  if (!rIsPluralRing(r))
  {
    WerrorS("order-ideal border basis: expected a noncommutative G-algebra");
    return TRUE;
  }
  if ((coefficientVariables <= 0) ||
      (2 * coefficientVariables != r->N))
  {
    WerrorS("order-ideal border basis: expected variables (x(1..n),D(1..n))");
    return TRUE;
  }
  if ((I == NULL) || (IDELEMS(I) <= 0))
  {
    WerrorS("order-ideal border basis: expected nonzero ideal generators");
    return TRUE;
  }
  if (maxProlongation < 0)
  {
    WerrorS("order-ideal border basis: negative prolongation bound");
    return TRUE;
  }

  const int firstOperator = coefficientVariables + 1;
  const int operators = coefficientVariables;
  ExponentVectors order;
  std::set<ExponentVector> orderSet;
  if (readOrderIdeal(inputOrderIdeal, coefficientVariables, order, orderSet, r))
    return TRUE;

  ExponentVectors border;
  borderbasis_kernel::border(order, border);
  for (int degree = 0; degree <= maxProlongation; ++degree)
  {
    std::vector<poly> rows;
    macaulayRows(I, degree, firstOperator, operators, rows, r);
    if (reduceMacaulayRows(rows, orderSet, firstOperator, operators, r))
    {
      deleteRows(rows, r);
      return TRUE;
    }

    ideal candidate = NULL;
    bool dependent = false;
    if (extractCandidates(rows, border, orderSet, firstOperator, operators,
                          candidate, dependent, r))
    {
      deleteRows(rows, r);
      id_Delete(&candidate, r);
      return TRUE;
    }
    deleteRows(rows, r);

    if (dependent)
    {
      id_Delete(&candidate, r);
      WerrorS("order-ideal border basis: the supplied monomials are dependent modulo the ideal");
      return TRUE;
    }
    if (!completeCandidate(candidate))
    {
      id_Delete(&candidate, r);
      continue;
    }

    bool integrable = false;
    if (checkIntegrability(candidate, order, orderSet, border, firstOperator,
                           operators, integrable, r))
    {
      id_Delete(&candidate, r);
      return TRUE;
    }
    if (!integrable)
    {
      id_Delete(&candidate, r);
      WerrorS("order-ideal border basis: the induced border relations are not integrable");
      return TRUE;
    }

    bool generatorsVanished = false;
    if (generatorsReduceToZero(I, candidate, order, orderSet, border,
                               firstOperator, operators, generatorsVanished,
                               r))
    {
      id_Delete(&candidate, r);
      return TRUE;
    }
    if (!generatorsVanished)
    {
      id_Delete(&candidate, r);
      WerrorS("order-ideal border basis: the supplied order ideal is not a quotient basis");
      return TRUE;
    }

    borderBasis = candidate;
    return FALSE;
  }

  WerrorS("order-ideal border basis: prolongation bound exhausted before all border relations were found");
  return TRUE;
}

#endif
