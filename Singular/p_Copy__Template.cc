/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Copy__Template.cc
 *  Purpose: template for p_Copy
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Copy__Template.cc,v 1.1 2000-08-24 14:42:42 obachman Exp $
 *******************************************************************/

poly p_Copy(poly s_p, const ring r)
{
  pTest(s_p);
  spolyrec dp;
  poly d_p = &dp;
  omBin bin = r->PolyBin;
  poly h;
  
  DECLARE_LENGTH(const unsigned long length = r->ExpLSize);

  while (s_p != NULL)
  {
    omTypeAllocBin(poly, pNext(d_p), bin);
    pIter(d_p);
    pSetCoeff0(d_p, p_nCopy(pGetCoeff(s_p), r));
    // it is better to iter here,
    // for MemCopy advances goes from low to high addresses
    h = s_p;
    s_p = pNext(s_p);
    p_MemCopy(d_p->exp.l, h->exp.l, length);
  }
  pNext(d_p) = NULL;

  pTest(dp.next);
#if PDEBUG
  assume(pEqual(s_p, dp.next));
#endif
  return dp.next;
}

