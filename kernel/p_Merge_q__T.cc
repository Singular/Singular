/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Merge_q__Template.cc
 *  Purpose: template for p_Add_q
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Merge_q__T.cc,v 1.1.1.1 2003-10-06 12:15:59 Singular Exp $
 *******************************************************************/

/***************************************************************
 *
 * Returns:  p merged with q
 * Destroys: p, q
 *
 ***************************************************************/
LINKAGE poly p_Merge_q(poly p, poly q, const ring r)
{
  assume(p != NULL && q != NULL);
  p_Test(p, r);
  p_Test(q, r);
#if PDEBUG > 0
  int l = pLength(p) + pLength(q);
#endif

  spolyrec rp;
  poly a = &rp;
  DECLARE_LENGTH(const unsigned long length = r->CmpL_Size);
  DECLARE_ORDSGN(const long* ordsgn = r->ordsgn);

  Top:     // compare p and q w.r.t. monomial ordering
  p_MemCmp(p->exp, q->exp, length, ordsgn, goto Equal, goto Greater , goto Smaller);

  Equal:
  // should never get here
  dReportError("Equal monomials in p_Merge_q");
  return NULL;

  Greater:
  a = pNext(a) = p;
  pIter(p);
  if (p==NULL) { pNext(a) = q; goto Finish;}
  goto Top;

  Smaller:
  a = pNext(a) = q;
  pIter(q);
  if (q==NULL) { pNext(a) = p; goto Finish;}
  goto Top;

  Finish:

  p_Test(pNext(&rp), r);
#if PDEBUG > 0
  pAssume1(l - pLength(pNext(&rp)) == 0);
#endif
  return pNext(&rp);
}

