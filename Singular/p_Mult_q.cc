/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Mult_q.cc
 *  Purpose: multiplication of polynomials
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Mult_q.cc,v 1.1 2000-08-29 14:10:27 obachman Exp $
 *******************************************************************/
#include "mod2.h"
/***************************************************************
 *
 * Returns:  p * q, 
 * Destroys: if !copy then p, q
 * Assumes: pLength(p) >= 2 pLength(q) >=2
 ***************************************************************/
#include "polys.h"
#include "p_Procs.h"

poly _p_Mult_q(poly p, poly q, const int copy, const ring r)
{
  assume (p != NULL && pNext(p) != NULL && q != NULL && pNext(q) != NULL);
  
  // to minimize the number of polynomial comparisons
  // we reverse p and should arrange p and q such that
  // pLast(p)*pHead(q) > pHead(p)*pLast(q)
  // as a first approximation, we just compare the head terms
  if (p_LmCmp(p, q, r) == -1)
  {
    poly tmp = q;
    q = p;
    p = tmp;
  }
  p = r->p_Procs->p_ReverseNeg(p, r);

  poly pp = p;
  int shorter;
  p_Minus_mm_Mult_qq_Proc_Ptr p_minus_mm_mult_qq 
    =  r->p_Procs->p_Minus_mm_Mult_qq;
  poly pr = NULL;

  while (pp != NULL)
  {
    pr = p_minus_mm_mult_qq(pr, pp, q, shorter, NULL, r);
    pIter(pp);
  }
  
  if (copy)
  {
    r->p_Procs->p_ReverseNeg(p, r);
  }
  else
  {
    r->p_Procs->p_Delete(&q, r);
    r->p_Procs->p_Delete(&p, r);
  }
  return pr;
}

  
  
