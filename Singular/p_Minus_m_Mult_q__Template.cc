/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Minus_m_Mult_q__Template.cc
 *  Purpose: template for p_Minus_m_Mult_q
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Minus_m_Mult_q__Template.cc,v 1.1 2000-08-24 14:42:43 obachman Exp $
 *******************************************************************/

/***************************************************************
 *
 * Returns:  p - m*q
 *           Shorter, where Shorter == Length(p) + Length(q) - Length(p - m*q);
 * Destroys: p
 * Const:    m, q
 * 
 ***************************************************************/
poly p_Minus_m_Mult_q(poly p, poly m, poly q, int& Shorter, const poly spNoether, const ring r)
{
  Shorter = 0;
  // we are done if q == NULL || m == NULL
  if (q == NULL || m == NULL) return p;
  
  spolyrec rp;
  poly a = &rp,                       // collects the result
    qm = NULL,                     // stores q*m
    c;                             // used for temporary storage


  number tm   = pGetCoeff(m),           // coefficient of m
    tneg = p_nNeg(p_nCopy(tm, r), r),    // - (coefficient of m)
    tb,                            // used for tm*coeff(a1)
    tc;                            // used as intermediate number


  int shorter = 0;
  DECLARE_LENGTH(const unsigned long length = r->ExpLSize);
  DECLARE_ORDSGN(const long* ordsgn = r->ordsgn);

  const unsigned long* m_e = m->exp.l;
  omBin bin = r->PolyBin;
  
  if (p == NULL) goto Finish;           // return tneg*q if (p == NULL)
  
  omTypeAllocBin(poly, qm, bin);
  assume(pGetComp(q) == 0 || pGetComp(m) == 0);
  p_MemAdd(qm->exp.l, q->exp.l, m_e, length);
  
  // MAIN LOOP:
  Top:     
  // compare qm = m*q and p w.r.t. monomial ordering
  p_MemCmp(qm->exp.l, p->exp.l, length, ordsgn, goto Equal, goto Greater, goto Smaller );
  
  Equal:   // qm equals p
  
  tb = p_nMult(pGetCoeff(q), tm, r);
  tc = pGetCoeff(p);
  if (!p_nEqual(tc, tb, r))
  {
    shorter++;
    tc = p_nSub(tc, tb, r);
    p_nDelete(&(pGetCoeff(p)), r);
    pSetCoeff0(p,tc); // adjust coeff of p
    a = pNext(a) = p; // append p to result and advance p
    pIter(p);
  }
  else
  { // coeffs are equal, so their difference is 0: 
    shorter += 2;
    p_nDelete(&tc, r);
    FreeAndAdvance(p);
  }
  p_nDelete(&tb, r);
  pIter(q);
  if (q == NULL || p == NULL) goto Finish; // are we done ?
  // no, so update qm
  p_MemAdd(qm->exp.l, q->exp.l, m_e, length);
  goto Top;


  Greater:
  pSetCoeff0(qm, p_nMult(pGetCoeff(q), tneg, r));
  a = pNext(a) = qm;       // append qm to result and advance q
  pIter(q);
  if (q == NULL) // are we done?
  {
    qm = NULL;
    goto Finish; 
  }
  // construct new qm 
  omTypeAllocBin(poly, qm, bin);
  p_MemAdd(qm->exp.l, q->exp.l, m_e, length);
  goto Top;
    
  Smaller:     
  a = pNext(a) = p;// append p to result and advance p
  pIter(p);
  if (p == NULL) goto Finish;;
  goto Top;
 

  Finish: // q or p is NULL: Clean-up time

  if (q == NULL) // append rest of p to result
    pNext(a) = p;
  else  // append (- m*q) to result
  {
    pSetCoeff0(m, tneg);
    pNext(a) = p_Mult_m(q, m, spNoether, r);
    pSetCoeff0(m, tm);
  }
   
  p_nDelete(&tneg, r);
  if (qm != NULL) omFreeBinAddr(qm);
  Shorter = shorter;
  return pNext(&rp);
} 
