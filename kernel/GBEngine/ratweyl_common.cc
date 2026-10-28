/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: shared fraction-free support for rational Weyl computations
*/

#include "kernel/mod2.h"

#include "kernel/GBEngine/ratweyl_common.h"

#ifdef HAVE_RATWEYL_COMMON

#include "kernel/ideals.h"
#include "polys/clapsing.h"
#include "polys/monomials/p_polys.h"

ideal ncGCD(poly p, poly q, const ring r)
{
  poly pCopy = p_Copy(p, r);
  poly qCopy = p_Copy(q, r);
  poly g = singclap_gcd_r(pCopy, qCopy, r);
  p_Delete(&pCopy, r);
  p_Delete(&qCopy, r);
  poly u = singclap_pdivide(q, g, r);
  poly v = p_Neg(singclap_pdivide(p, g, r), r);
  p_Delete(&g, r);
  p_Delete(&p, r);
  p_Delete(&q, r);

  ideal result = idInit(2, 1);
  result->m[0] = u;
  result->m[1] = v;
  return result;
}

#endif
