/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pp_Mult_nn__Template.cc
 *  Purpose: template for pp_Mult_nn
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pp_Mult_nn__T.cc,v 1.8 2008-02-07 08:41:00 wienand Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*n
 *   Const:    p, n
 *
 ***************************************************************/
LINKAGE poly pp_Mult_nn(poly p, const number n, const ring r)
{
  pAssume(!n_IsZero(n,r));
  p_Test(p, r);
  if (p == NULL) return NULL;
  spolyrec rp;
#ifdef HAVE_ZERODIVISORS
  rp.next = NULL;
#endif
  poly q = &rp;
  omBin bin = r->PolyBin;
  DECLARE_LENGTH(const unsigned long length = r->ExpL_Size);

  do
  {
#ifndef HAVE_ZERODIVISORS
    p_AllocBin(pNext(q), bin, r);
    q = pNext(q);
    number nc = pGetCoeff(p);
    pSetCoeff0(q, n_Mult(n, nc, r));
    p_MemCopy(q->exp, p->exp, length);
#else
    number nc = pGetCoeff(p);
    number tmp = n_Mult(n, nc, r);
    if (! nIsZero(tmp))
    {
      p_AllocBin(pNext(q), bin, r);
      q = pNext(q);
      pSetCoeff0(q, tmp);
      p_MemCopy(q->exp, p->exp, length);
    }
    else
      n_Delete(&tmp,r);
#endif
    pIter(p);
  }
  while (p != NULL);
  pNext(q) = NULL;

  p_Test(rp.next, r);
  return rp.next;
}


