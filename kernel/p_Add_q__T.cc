/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Add_q__Template.cc
 *  Purpose: template for p_Add_q
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Add_q__T.cc,v 1.1.1.1 2003-10-06 12:15:58 Singular Exp $
 *******************************************************************/

/***************************************************************
 *
 * Returns:  p + q,
 *           Shorter, where Shorter == Length(p) + Length(q) - Length(p+q);
 * Destroys: p, q
 *
 ***************************************************************/
LINKAGE poly p_Add_q(poly p, poly q, int &Shorter, const ring r)
{
  p_Test(p, r);
  p_Test(q, r);
#if PDEBUG > 0
  int l = pLength(p) + pLength(q);
#endif

  // test for trivial cases
  Shorter = 0;
  if (q == NULL) return p;
  if (p == NULL) return q;

  number t, n1, n2;
  int shorter = 0;
  spolyrec rp;
  poly a = &rp;
  DECLARE_LENGTH(const unsigned long length = r->CmpL_Size);
  DECLARE_ORDSGN(const long* ordsgn = r->ordsgn);

  Top:     // compare p and q w.r.t. monomial ordering
  p_MemCmp(p->exp, q->exp, length, ordsgn, goto Equal, goto Greater , goto Smaller);

  Equal:
  n1 = pGetCoeff(p);
  n2 = pGetCoeff(q);
  t = n_Add(n1,n2, r);
  n_Delete(&n1, r);
  n_Delete(&n2, r);
  q = p_LmFreeAndNext(q, r);

  if (n_IsZero(t, r))
  {
    shorter += 2;
    n_Delete(&t, r);
    p = p_LmFreeAndNext(p, r);
  }
  else
  {
    shorter++;
    pSetCoeff0(p,t);
    a = pNext(a) = p;
    pIter(p);
  }
  if (p==NULL) { pNext(a) = q; goto Finish;}
  if (q==NULL) { pNext(a) = p; goto Finish;}
  goto Top;

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
  Shorter = shorter;

  p_Test(pNext(&rp), r);
#if PDEBUG > 0
  pAssume1(l - pLength(pNext(&rp)) == Shorter);
#endif
  return pNext(&rp);
}

