/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pp_Mult_Coeff_mm_DivSelect__Template.cc
 *  Purpose: template for pp_Mult_Coeff_mm__DivSelect
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*Coeff(m) for such monomials pm of p, for which
 *             m is divisible by pm
 *   Const:    p, m
 *
 ***************************************************************/
LINKAGE poly pp_Mult_Coeff_mm_DivSelect__T(poly p, const poly m, int &shorter,
                                        const ring r)
{
  number n = pGetCoeff(m);
  pAssume(!n_IsZero__T(n,r->cf));
  p_Test(p, r);
  if (p == NULL) return NULL;
  spolyrec rp;
  DECLARE_LENGTH(const unsigned long length = r->ExpL_Size);
  int Shorter = 0;
  poly q = &rp;
  omBin bin = r->PolyBin;

  do
  {
    if (p_LmDivisibleByNoComp(m, p, r))
    {
      p_AllocBin(pNext(q), bin, r);
      q = pNext(q);
      number nc = pGetCoeff(p);
      pSetCoeff0(q, n_Mult__T(n, nc, r->cf));
      p_MemCopy__T(q->exp, p->exp, length);
    }
    else
    {
      Shorter++;
    }
    pIter(p);
  }
  while (p != NULL);
  pNext(q) = NULL;

  shorter = Shorter;
  p_Test(rp.next, r);
  return rp.next;
}


