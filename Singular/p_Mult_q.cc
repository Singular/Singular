/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Mult_q.cc
 *  Purpose: multiplication of polynomials
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Mult_q.cc,v 1.2 2000-09-04 13:39:01 obachman Exp $
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
#include "p_Numbers.h"

poly _p_Mult_q(poly p, poly q, const int copy, const ring r)
{
  assume (p != NULL && pNext(p) != NULL && q != NULL && pNext(q) != NULL);
  pTest(p);
  pTest(q);
  
  poly res = pp_Mult_mm(p,q,r);  // holds initially q1*p
  poly qq = pNext(q);           // we iter of this
  poly qn = pp_Mult_mm(qq, p,r); // holds p1*qi
  poly pp = pNext(p);           // used for Lm(qq)*pp
  poly rr = res;                // last monom which is surely not NULL
  poly rn = pNext(res);         // pNext(rr)
  number n, n1, n2;
  
  // now the main loop
  Top:
  if (rn == NULL) goto Smaller;
  p_LmCmpAction(rn, qn, r, goto Equal, goto Greater, goto Smaller);
  
  Greater:
  // rn > qn, so iter
  rr = rn;
  pIter(rn);
  goto Top;
  
  // rn < qn, append qn to rr, and compute next Lm(qq)*pp
  Smaller:
  pNext(rr) = qn;
  rr = qn;
  pIter(qn);
  Work: // now we compute Lm(qq)*pp
  if (rn == NULL)
    pNext(rr) = pp_Mult_mm(pp, qq, r);
  else
    pNext(rr) = p_Plus_mm_Mult_qq(rn, qq, pp, r);
  
  pIter(qq);
  if (qq == NULL) goto Finish;
  rn = pNext(rr);
  goto Top;
  
  Equal:
  n1 = pGetCoeff(rn);
  n = p_nAdd(n1, pGetCoeff(qn), r);
  p_nDelete(&n1, r);
  if (p_nIsZero(n, r))
  {
    p_nDelete(&n, r);
    pFreeAndNext(rn);
  }
  else
  {
    pSetCoeff0(rn, n);
    rr = rn;
    pIter(rn);
  }
  p_nDelete(&pGetCoeff(qn),r);
  pFreeAndNext(qn);
  goto Work;
  
  Finish:
  pTest(res);
  if (!copy)
  {
    p_Delete(&p, r);
    p_Delete(&q, r);
  }

  pTest(res);
  return res;
}

#if 0
poly _p_Mult_q(poly p, poly q, const int copy, const ring r)
{
  assume (p != NULL && pNext(p) != NULL && q != NULL && pNext(q) != NULL);
  
  // to minimize the number of polynomial comparisons
  // we reverse p and should arrange p and q such that
  // pLast(p)*pHead(q) > pHead(p)*pLast(q)
  // as a first approximation, we just compare the head terms
#if 0  
  if (p_LmCmp(p, q, r) == -1)
  {
    poly tmp = q;
    q = p;
    p = tmp;
  }
  p = r->p_Procs->p_ReverseNeg(p, r);
#endif

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

  
  
#endif
