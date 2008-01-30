/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Minus_mm_Mult_qq__Template.cc
 *  Purpose: template for p_Minus_m_Mult_q
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Minus_mm_Mult_qq__T.cc,v 1.7 2008-01-30 18:49:43 wienand Exp $
 *******************************************************************/

/***************************************************************
 *
 * Returns:  p - m*q
 *           Shorter, where Shorter == Length(p) + Length(q) - Length(p - m*q);
 * Destroys: p
 * Const:    m, q
 *
 ***************************************************************/
LINKAGE poly p_Minus_mm_Mult_qq(poly p, poly m, poly q, int& Shorter, const poly spNoether, const ring r, poly &last)
{
  p_Test(p, r);
  p_Test(q, r);
  p_LmTest(m, r);

#if PDEBUG > 0
  int l_debug = pLength(p) + pLength(q);
#endif

  Shorter = 0;
  // we are done if q == NULL || m == NULL
  if (q == NULL || m == NULL) return p;

  spolyrec rp;
  poly a = &rp,                    // collects the result
    qm = NULL,                     // stores q*m
    c;                             // used for temporary storage


  number tm = pGetCoeff(m),           // coefficient of m
    tneg = n_Neg(n_Copy(tm, r), r),    // - (coefficient of m)
    tb,                            // used for tm*coeff(a1)
    tc;                            // used as intermediate number


  int shorter = 0;
  DECLARE_LENGTH(const unsigned long length = r->ExpL_Size);
  DECLARE_ORDSGN(const long* ordsgn = r->ordsgn);

  const unsigned long* m_e = m->exp;
  omBin bin = r->PolyBin;

  if (p == NULL) goto Finish;           // return tneg*q if (p == NULL)

  pAssume(p_GetComp(q, r) == 0 || p_GetComp(m, r) == 0);

  AllocTop:
  p_AllocBin(qm, bin, r);
  SumTop:
  p_MemSum(qm->exp, q->exp, m_e, length);
  p_MemAddAdjust(qm, r);

  CmpTop:
  // compare qm = m*q and p w.r.t. monomial ordering
  p_MemCmp(qm->exp, p->exp, length, ordsgn, goto Equal, goto Greater, goto Smaller );

  Equal:   // qm equals p
  tb = n_Mult(pGetCoeff(q), tm, r);
#ifdef HAVE_ZERODIVISORS
  if (!nIsZero(tb)) {
#endif
  tc = pGetCoeff(p);
  if (!n_Equal(tc, tb, r))
  {
    shorter++;
    tc = n_Sub(tc, tb, r);
    n_Delete(&(pGetCoeff(p)), r);
    pSetCoeff0(p,tc); // adjust coeff of p
    a = pNext(a) = p; // append p to result and advance p
    pIter(p);
  }
  else
  { // coeffs are equal, so their difference is 0:
    shorter += 2;
    n_Delete(&tc, r);
    p = p_LmFreeAndNext(p, r);
  }
#ifdef HAVE_ZERODIVISORS
  }
#endif
  n_Delete(&tb, r);
  pIter(q);
  if (q == NULL || p == NULL) goto Finish; // are we done ?
  // no, so update qm
  goto SumTop;


  Greater:
#ifdef HAVE_ZERODIVISORS
  tb = n_Mult(pGetCoeff(q), tneg, r);
  if (!nIsZero(tb))
  {
#endif
    pSetCoeff0(qm, n_Mult(pGetCoeff(q), tneg, r));
    a = pNext(a) = qm;       // append qm to result and advance q
#ifdef HAVE_ZERODIVISORS
  }
  else
  {
    shorter++;
  }
  n_Delete(&tb, r);
#endif
  pIter(q);
  if (q == NULL) // are we done?
  {
    qm = NULL;
    goto Finish;
  }
  // construct new qm
  goto AllocTop;

  Smaller:
  a = pNext(a) = p;// append p to result and advance p
  pIter(p);
  if (p == NULL) goto Finish;
  goto CmpTop;


  Finish: // q or p is NULL: Clean-up time
  if (q == NULL) // append rest of p to result
  {
    pNext(a) = p;
    if (p == NULL) last = a;
  }
  else  // append (- m*q) to result
  {
    pSetCoeff0(m, tneg);
    last = a;
    if (spNoether != NULL)
    {
      int ll = 0;
      pNext(a) = r->p_Procs->pp_Mult_mm_Noether(q, m, spNoether, ll, r, last);
      shorter += ll;
    }
    else
      pNext(a) = r->p_Procs->pp_Mult_mm(q, m, r, last);
    pSetCoeff0(m, tm);
  }

  n_Delete(&tneg, r);
  if (qm != NULL) p_FreeBinAddr(qm, r);
  Shorter = shorter;
  p_Test(pNext(&rp), r);
  return pNext(&rp);
}
