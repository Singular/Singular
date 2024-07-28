/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pp_Mult_Coeff_mm_DivSelectMult__Template.cc
 *  Purpose: template for pp_Mult_Coeff_mm__DivSelectMult
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*Coeff(m)*a/b for such monomials pm of p, for which
 *             m is divisible by pm, shorter == #of monomials left out
 *   Assumes:  m, a, b are monomials, ordering is (c, dp),
 *            (p*a) is divisible by b for all monomials in question
 *   Const:    p, m, a, b
 *
 ***************************************************************/
LINKAGE poly pp_Mult_Coeff_mm_DivSelectMult__T(poly p,const poly m, const poly a, const poly b, int &shorter,const ring r)
{
  assume(rOrd_is_Comp_dp(r) && r->ExpL_Size > 2);
  p_Test(p, r);
  if (p == NULL) return NULL;
  number n = pGetCoeff(m);
  number nc;
  pAssume(!n_IsZero__T(n,r->cf));

  spolyrec rp;
  omBin bin = r->PolyBin;
  const unsigned long bitmask = r->divmask;
  const unsigned long* m_e = &(m->exp[2]);
  DECLARE_LENGTH(const unsigned long length = r->ExpL_Size);
  DECLARE_LENGTH_2(const unsigned long length_2 = length - 2);

  poly ab;
  p_AllocBin(ab, bin, r);
  unsigned long* ab_e = &(ab->exp[0]);

  p_MemDiff__T(ab_e, ((unsigned long*) &(a->exp[0])), ((unsigned long*) &(b->exp[0])),
            length);

  int Shorter = 0;
  poly q = &rp;

  do
  {
    p_MemCmp_Bitmask_2(m_e, &(p->exp[2]), bitmask, length_2,
                       goto Divisible, goto NotDivisible);

    NotDivisible:
    pAssume(!p_LmDivisibleByNoComp(m, p, r));
    Shorter++;
    goto Iter;

    Divisible:
    pAssume(p_LmDivisibleByNoComp(m, p, r));
    p_AllocBin(pNext(q), bin, r);
    q = pNext(q);
    nc = pGetCoeff(p);
    pSetCoeff0(q, n_Mult__T(n, nc, r->cf));
    p_MemSum__T(q->exp, p->exp, ab_e, length);

    Iter:
    pIter(p);
  }
  while (p != NULL);

  pNext(q) = NULL;
  p_FreeBinAddr(ab, r);

  shorter = Shorter;
  p_Test(rp.next, r);
  return rp.next;
}


