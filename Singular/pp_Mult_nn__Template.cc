/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pp_Mult_n__Template.cc
 *  Purpose: template for pp_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pp_Mult_nn__Template.cc,v 1.1 2000-08-29 14:10:30 obachman Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*n
 *   Const:    p, n
 *
 ***************************************************************/
poly pp_Mult_nn(poly p, const number n, const ring r)
{
  assume(!p_nIsZero(n,r));
  pTest(p);
  if (p == NULL) return NULL;
  spolyrec rp;
  poly q = &rp;
  omBin bin = r->PolyBin;
  DECLARE_LENGTH(const unsigned long length = r->ExpLSize);

  do
  {
    omTypeAllocBin(poly, pNext(q), bin);
    q = pNext(q);
    number nc = pGetCoeff(p);
    pSetCoeff0(q, p_nMult(n, nc, r));
    p_MemCopy(q->exp.l, p->exp.l, length);
    pIter(p);
  }
  while (p != NULL);
  pNext(q) = NULL;

  pTest(rp.next);
  return rp.next;
}


