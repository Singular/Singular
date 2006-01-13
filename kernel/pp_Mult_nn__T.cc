/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pp_Mult_nn__Template.cc
 *  Purpose: template for pp_Mult_nn
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pp_Mult_nn__T.cc,v 1.2 2006-01-13 18:10:05 wienand Exp $
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
  poly q = &rp;
  omBin bin = r->PolyBin;
  DECLARE_LENGTH(const unsigned long length = r->ExpL_Size);

  do
  {
    number nc = pGetCoeff(p);
    number tmp = n_Mult(n, nc, r);
#ifdef HAVE_RING2TOM
    if (r->cring==0 || (r->cring ==1 && (long) tmp != 0)){
#endif
      p_AllocBin(pNext(q), bin, r);
      q = pNext(q);
      pSetCoeff0(q, tmp);
      p_MemCopy(q->exp, p->exp, length);
#ifdef HAVE_RING2TOM
    }
#endif
    pIter(p);
  }
  while (p != NULL);
  pNext(q) = NULL;

  p_Test(rp.next, r);
  return rp.next;
}


