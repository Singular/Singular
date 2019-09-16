/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Minus_mm_Mult_qq__Template.cc
 *  Purpose: template for p_Minus_m_Mult_q
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *******************************************************************/

/***************************************************************
 *
 * Returns:  p - m*q
 *           Shorter, where Shorter == Length(p) + Length(q) - Length(p - m*q);
 * Destroys: p
 * Const:    m, q
 *
 ***************************************************************/
LINKAGE poly p_Minus_mm_Mult_qq__T(poly p, poly m, poly q, int& Shorter, const poly spNoether, const ring r)
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
    qm = NULL;                     // stores q*m


  number tm = pGetCoeff(m),           // coefficient of m
    tneg = n_Neg__T(n_Copy__T(tm, r->cf), r->cf),    // - (coefficient of m)
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
  p_MemSum__T(qm->exp, q->exp, m_e, length);
  p_MemAddAdjust__T(qm, r);

  CmpTop:
  // compare qm = m*q and p w.r.t. monomial ordering
  p_MemCmp__T(qm->exp, p->exp, length, ordsgn, goto Equal, goto Greater, goto Smaller );

  Equal:   // qm equals p
  tb = n_Mult__T(pGetCoeff(q), tm, r->cf);
#ifdef HAVE_ZERODIVISORS
  if (!n_IsZero__T(tb,r->cf)) {
#endif
  tc = pGetCoeff(p);
  if (!n_Equal__T(tc, tb, r->cf))
  {
    shorter++;
    tc = n_Sub__T(tc, tb, r->cf);
    n_Delete__T(&(pGetCoeff(p)), r->cf);
    pSetCoeff0(p,tc); // adjust coeff of p
    a = pNext(a) = p; // append p to result and advance p
    pIter(p);
  }
  else
  { // coeffs are equal, so their difference is 0:
    shorter += 2;
    n_Delete__T(&tc, r->cf);
    p = p_LmFreeAndNext(p, r);
  }
#ifdef HAVE_ZERODIVISORS
  }
  else
  { // coeff itself is zero
    shorter += 1;
  }
#endif
  n_Delete__T(&tb, r->cf);
  pIter(q);
  if (q == NULL || p == NULL) goto Finish; // are we done ?
  // no, so update qm
  goto SumTop;


  Greater:
#ifdef HAVE_ZERODIVISORS
  tb = n_Mult__T(pGetCoeff(q), tneg, r->cf);
  if (!n_IsZero__T(tb,r->cf))
  {
#endif
    pSetCoeff0(qm, n_Mult__T(pGetCoeff(q), tneg, r->cf));
    a = pNext(a) = qm;       // append qm to result and advance q
#ifdef HAVE_ZERODIVISORS
  }
  else
  {
    shorter++;
  }
  n_Delete__T(&tb, r->cf);
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
  }
  else  // append (- m*q) to result
  {
    pSetCoeff0(m, tneg);
    if (spNoether != NULL)
    {
      int ll = 0;
      pNext(a) = r->p_Procs->pp_Mult_mm_Noether(q, m, spNoether, ll, r);
      shorter += ll;
    }
    else
    {
      pNext(a) = r->p_Procs->pp_Mult_mm(q, m, r);
#ifdef HAVE_ZERODIVISORS
      if (! rField_is_Domain(r))
      {
        shorter += pLength(q) - pLength(pNext(a));
      }
#endif
    }
    pSetCoeff0(m, tm);
  }

  n_Delete__T(&tneg, r->cf);
  if (qm != NULL) p_FreeBinAddr(qm, r);
  Shorter = shorter;
  p_Test(pNext(&rp), r);
  return pNext(&rp);
}
