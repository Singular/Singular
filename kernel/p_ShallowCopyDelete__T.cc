/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_ShallowCopyDelete__Template.cc
 *  Purpose: template for p_ShallowCopyDelete
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_ShallowCopyDelete__T.cc,v 1.1.1.1 2003-10-06 12:16:00 Singular Exp $
 *******************************************************************/

/***************************************************************
 *
 * Destroys: p
 * Assumes:  Monoms of p are from
 * Returns:
 ***************************************************************/
LINKAGE poly p_ShallowCopyDelete(poly s_p, const ring r, omBin d_bin)
{
  spolyrec dp;
  poly d_p = &dp;
  unsigned long* s_e;
  poly h;

  DECLARE_LENGTH(const unsigned long length = r->ExpL_Size);

  while (s_p != NULL)
  {
    p_AllocBin(d_p->next, d_bin, r);
    pIter(d_p);
    pSetCoeff0(d_p, pGetCoeff(s_p));
    h = s_p;
    s_p =  pNext(s_p);
    p_MemCopy(d_p->exp, h->exp, length);
    p_FreeBinAddr(h, r);
  }
  pNext(d_p) = NULL;
  return dp.next;
}

