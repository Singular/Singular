#ifndef BORDERBASIS_STAIRCASE_H
#define BORDERBASIS_STAIRCASE_H

#include <vector>

#include "kernel/structs.h"

// Shared monomial-combinatorics layer for the commutative and rational-Weyl
// border-basis front ends.  This header is private to libGBEngine.
namespace borderbasis_kernel
{
typedef std::vector<int> ExponentVector;
typedef std::vector<ExponentVector> ExponentVectors;

BOOLEAN staircase(const ideal G, int firstVariable, int numberOfVariables,
                  ExponentVectors &standard, ExponentVectors &border,
                  const ring r);

poly makeMonomial(const ExponentVector &a, int firstVariable, const ring r);

ideal makeMonomialIdeal(const ExponentVectors &monomials, int firstVariable,
                        const ring r);
}

#endif
