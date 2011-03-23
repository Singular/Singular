/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pInline.cc
 *  Purpose: implementation of poly Level 0 functions
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/
#ifndef PINLINE0_H
#define PINLINE0_H

#if defined(DO_PINLINE0)
#define PINLINE0 static inline
#else
#define PINLINE0
#endif

#include <polys/monomials/p_polys.h>
#include <polys/monomials/ring.h>

// returns minimal column number in the modul element a (or 0)
BOOLEAN   p_IsConstantPoly(const poly p, const ring r)
{
  poly pp=p;
  while(pp!=NULL)
  {
    if (! p_LmIsConstantComp(pp, r))
      return FALSE;
    pIter(pp);
  }
  return TRUE;
}

/***************************************************************
 *
 * poly things which are independent of ring
 *
 ***************************************************************/
PINLINE0 poly pReverse(poly p)
{
  if (p == NULL || pNext(p) == NULL) return p;

  poly q = pNext(p), // == pNext(p)
    qn;
  pNext(p) = NULL;
  do
  {
    qn = pNext(q);
    pNext(q) = p;
    p = q;
    q = qn;
  }
  while (qn != NULL);
  return p;
}


/*2
* returns the length of a (numbers of monomials)
* respect syzComp
*/
poly pLast(poly a, int &l);

#endif // PINLINE_CC

