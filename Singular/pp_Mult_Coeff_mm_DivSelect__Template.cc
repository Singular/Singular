/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pp_Mult_Coeff_mm_DivSelect__Template.cc
 *  Purpose: template for pp_Mult_Coeff_mm__DivSelect
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pp_Mult_Coeff_mm_DivSelect__Template.cc,v 1.3 2000-10-30 13:40:26 obachman Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*Coeff(m) for such monomials pm of p, for which
 *             m is divisble by pm
 *   Const:    p, m
 *
 ***************************************************************/
poly pp_Mult_Coeff_mm_DivSelect(poly p, const poly m, const ring r)
{
  number n = pGetCoeff(m);
  pAssume(!n_IsZero(n,r));
  p_Test(p, r);
  if (p == NULL) return NULL;
  spolyrec rp;
  poly q = &rp;
  omBin bin = r->PolyBin;
  DECLARE_LENGTH(const unsigned long length = r->ExpL_Size);

  do
  {
    if (p_LmDivisibleByNoComp(m, p, r))
    {
      p_AllocBin(pNext(q), bin, r);
      q = pNext(q);
      number nc = pGetCoeff(p);
      pSetCoeff0(q, n_Mult(n, nc, r));
      p_MemCopy(q->exp, p->exp, length);
    }
    pIter(p);
  }
  while (p != NULL);
  pNext(q) = NULL;

  p_Test(rp.next, r);
  return rp.next;
}


