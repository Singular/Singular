/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: border bases in commutative polynomial rings
*/

#include "kernel/mod2.h"

#include "kernel/GBEngine/borderbasis.h"
#include "kernel/GBEngine/borderbasis_staircase.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/ideals.h"
#include "kernel/polys.h"
#include "polys/monomials/p_polys.h"
#include "polys/monomials/ring.h"
#include "reporter/reporter.h"

BOOLEAN kBorderBasisFromStandardBasis(const ideal G, ideal &orderIdeal,
                                      ideal &borderBasis, const ring r)
{
  orderIdeal = NULL;
  borderBasis = NULL;
  if (rIsNCRing(r))
  {
    WerrorS("border basis: expected a commutative polynomial ring");
    return TRUE;
  }
  if (!rHasGlobalOrdering(r))
  {
    WerrorS("border basis: the kernel implementation requires a global ordering");
    return TRUE;
  }

  borderbasis_kernel::ExponentVectors standard;
  borderbasis_kernel::ExponentVectors border;
  if (borderbasis_kernel::staircase(G, 1, r->N, standard, border, r))
    return TRUE;

  orderIdeal = borderbasis_kernel::makeMonomialIdeal(standard, 1, r);
  borderBasis = idInit(static_cast<int>(border.size()), 1);
  for (size_t i = 0; i < border.size(); ++i)
  {
    poly b = borderbasis_kernel::makeMonomial(border[i], 1, r);
    poly normalForm = kNF(G, r->qideal, p_Copy(b, r));
    borderBasis->m[i] = p_Sub(b, normalForm, r);
  }
  return FALSE;
}
