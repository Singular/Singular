/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pp_Mult_n__Template.cc
 *  Purpose: template for pp_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pp_Mult_nn__Template.cc,v 1.3 2000-09-18 09:19:31 obachman Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*n
 *   Const:    p, n
 *
 ***************************************************************/
poly pp_Mult_nn(poly p, const number n, const ring r)
{
  pAssume(!n_IsZero(n,r));
  p_Test(p, r);
  if (p == NULL) return NULL;
  spolyrec rp;
  poly q = &rp;
  omBin bin = r->PolyBin;
  DECLARE_LENGTH(const unsigned long length = r->ExpLSize);

  do
  {
    p_AllocBin(pNext(q), bin, r);
    q = pNext(q);
    number nc = pGetCoeff(p);
    pSetCoeff0(q, n_Mult(n, nc, r));
    p_MemCopy(q->exp, p->exp, length);
    pIter(p);
  }
  while (p != NULL);
  pNext(q) = NULL;

  p_Test(rp.next, r);
  return rp.next;
}


