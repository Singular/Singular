/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Copy__Template.cc
 *  Purpose: template for p_Copy
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Copy__T.cc,v 1.1.1.1 2003-10-06 12:15:59 Singular Exp $
 *******************************************************************/

LINKAGE poly p_Copy(poly s_p, const ring r)
{
  // let's not do tests here -- but instead allow
  // to be sloppy
  spolyrec dp;
  poly d_p = &dp;
  omBin bin = r->PolyBin;
  poly h;

  DECLARE_LENGTH(const unsigned long length = r->ExpL_Size);

  while (s_p != NULL)
  {
    p_AllocBin(pNext(d_p), bin, r);
    pIter(d_p);
    pSetCoeff0(d_p, n_Copy(pGetCoeff(s_p), r));
    // it is better to iter here,
    // for MemCopy advances goes from low to high addresses
    h = s_p;
    s_p = pNext(s_p);
    p_MemCopy(d_p->exp, h->exp, length);
  }
  pNext(d_p) = NULL;

  return dp.next;
}

