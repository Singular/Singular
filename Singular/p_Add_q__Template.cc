/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Add_q__Template.cc
 *  Purpose: template for p_Add_q
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Add_q__Template.cc,v 1.1 2000-08-24 14:42:42 obachman Exp $
 *******************************************************************/

/***************************************************************
 *
 * Returns:  p + q, 
 *           Shorter, where Shorter == Length(p) + Length(q) - Length(p+q);
 * Destroys: p, q
 * 
 ***************************************************************/
poly p_Add_q(poly p, poly q, int &Shorter, const ring r)
{
  pTest(p);
  pTest(q);
#ifdef PDEBUG
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
  DECLARE_LENGTH(const unsigned long length = r->pCompLSize);
  DECLARE_ORDSGN(const long* ordsgn = r->ordsgn);

  Top:     // compare p and q w.r.t. monomial ordering
  p_MemCmp(p->exp.l, q->exp.l, length, ordsgn, goto Equal, goto Greater , goto Smaller );

  Equal:
  assume(rComp0(p, q) == 0);
  n1 = pGetCoeff(p);
  n2 = pGetCoeff(q);
  t = p_nAdd(n1,n2, r);
  p_nDelete(&n1, r);
  p_nDelete(&n2, r);
  FreeAndAdvance(q);
  
  if (p_nIsZero(t, r))
  {
    shorter += 2;
    p_nDelete(&t, r);
    FreeAndAdvance(p);
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
  assume(rComp0(p, q) == 1);
  a = pNext(a) = p;
  pIter(p);
  if (p==NULL) { pNext(a) = q; goto Finish;}
  goto Top;
    
  Smaller:
  assume(rComp0(p, q) == -1);
  a = pNext(a) = q;
  pIter(q);
  if (q==NULL) { pNext(a) = p; goto Finish;}
  goto Top;
  

  Finish:
  Shorter = shorter;

  pTest(pNext(&rp));
#ifdef PDEBUG  
  assume(l - pLength(pNext(&rp)) == Shorter);
#endif  
  return pNext(&rp);
}

