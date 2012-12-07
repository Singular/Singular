/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    LPDV__p_Minus_mml_Mult_qq_Mult_mmr__T.cc
 *  Purpose: template for LPDV__p_Minus_mm_Mult_qq
 *  Author:  obachman (Olaf Bachmann) (the original, this one
 *           also: Bo)
 *  Created: 11/2012
 *  TODO:    A lot maybe, or not, see other LPDV* templates.
 *******************************************************************/

#define HAVE_SHIFTBBADVEC

/***************************************************************
 *
 * Returns:  p - ml*q*mr
 *           Shorter, where 
 *           Shorter == Length(p) + Length(q) - Length(p - ml*q*mr);
 * Destroys: p
 * Const:    ml, q, mr
 *
 * BOCO:
 * ml has to have the right coefficient; we do not consider the
 * coefficient of mr (if it has one, it will not be used)
 *
 ***************************************************************/
LINKAGE poly LPDV__p_Minus_mml_Mult_qq_Mult_mmr__T
  ( poly p, poly ml, poly q, poly mr,
    int& Shorter, const poly spNoether, const ring r )
{
  p_Test(p, r);
  p_Test(q, r);
  p_LmTest(ml, r);
  p_LmTest(mr, r);

#if PDEBUG > 0
  int l_debug = pLength(p) + pLength(q);
#endif

  Shorter = 0;
  // we are done if q == NULL || (mr == NULL && ml == NULL)
  if (q == NULL || (mr == NULL && ml == NULL) ) return p;

  spolyrec rp;
  poly a = &rp,                    // collects the result
  qm = NULL;                       // stores ml*q*mr


  number tm = pGetCoeff(ml),           // coefficient of ml
  tneg = n_Neg__T(n_Copy__T(tm, r), r),    // - (coefficient of ml)
  tb,                            // used for tm*coeff(a1)
  tc;                            // used as intermediate number


  int shorter = 0;
  DECLARE_LENGTH(const unsigned long length = r->ExpL_Size);
  DECLARE_ORDSGN(const long* ordsgn = r->ordsgn);

  //const unsigned long* m_e = m->exp; //BOCO: unused in LPDV
  omBin bin = r->PolyBin;

  if (p == NULL) goto Finish;           // return tneg*q if (p == NULL)

  pAssume(p_GetComp(q, r) == 0 || p_GetComp(ml, r) == 0);

  AllocTop:
  p_AllocBin(qm, bin, r);
  SumTop:
#ifdef HAVE_SHIFTBBADVEC 
  /*BOCO: Notes for SHIFTBBADVEC case
   *NOTE 1: We need other Parameters
   *NOTE 2: ShiftDVec::p_ExpSum is a pointer which has to be 
   *      choosen appropriately according to the term ordering 
   *      used. At the moment I'm writing this, there are two 
   *      possibilities:
   *      ShiftDVec::p_ExpSum_slow and
   *      ShiftDVec::p_ExpSum_dp
   *TODO: How can we merge that? For the Letterplace case we
   *      don't need the original p_Minus_mm_Mult_qq !
   *      But we need a mm_Mult_qq, see below!
   */
  p_MemCopy__T(qm->exp, ml->exp, length); //BOCO: will that be defined correctly?
  r->p_ExpSum(qm, ml, q, r);
  r->p_ExpSum(qm, qm, mr, r);
#else
  p_MemSum__T(qm->exp, q->exp, m_e, length);
  p_MemAddAdjust__T(qm, r);
#endif

  CmpTop:
  // compare qm = m*q and p w.r.t. monomial ordering
  p_MemCmp__T(qm->exp, p->exp, length, ordsgn, goto Equal, goto Greater, goto Smaller );

  Equal:   // qm equals p
  tb = n_Mult__T(pGetCoeff(q), tm, r);
#ifdef HAVE_ZERODIVISORS
  if (!n_IsZero__T(tb,r)) {
#endif
  tc = pGetCoeff(p);
  if (!n_Equal__T(tc, tb, r))
  {
    shorter++;
    tc = n_Sub__T(tc, tb, r);
    n_Delete__T(&(pGetCoeff(p)), r);
    pSetCoeff0(p,tc); // adjust coeff of p
    a = pNext(a) = p; // append p to result and advance p
    pIter(p);
  }
  else
  { // coeffs are equal, so their difference is 0:
    shorter += 2;
    n_Delete__T(&tc, r);
    p = p_LmFreeAndNext(p, r);
  }
#ifdef HAVE_ZERODIVISORS
  }
  else
  { // coeff itself is zero
    shorter += 1;
  }
#endif
  n_Delete__T(&tb, r);
  pIter(q);
  if (q == NULL || p == NULL) goto Finish; // are we done ?
  // no, so update qm
  goto SumTop;


  Greater:
#ifdef HAVE_ZERODIVISORS
  tb = n_Mult__T(pGetCoeff(q), tneg, r);
  if (!n_IsZero__T(tb,r))
  {
#endif
    pSetCoeff0(qm, n_Mult__T(pGetCoeff(q), tneg, r));
    a = pNext(a) = qm;       // append qm to result and advance q
#ifdef HAVE_ZERODIVISORS
  }
  else
  {
    shorter++;
  }
  n_Delete__T(&tb, r);
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
  else  // append (-ml*q*mr) to result
  {
    pSetCoeff0(ml, tneg);
    if (spNoether != NULL)
    {
#ifdef HAVE_SHIFTBBADVEC 
      assume(0); 
      //Didn't consider that case yet - what does this Noether
      //case do anyway?
#endif
    }
    else
    {
      pNext(a) = 
        r->p_Procs->LPDV__mml_Mult_pp_Mult_mmr(ml, q, mr, r);
#ifdef HAVE_RINGS
      if (! rField_is_Domain(r))
      {
        shorter += pLength(q) - pLength(pNext(a));
      }
#endif
    }
    pSetCoeff0(ml, tm);
  }

  n_Delete__T(&tneg, r);
  if (qm != NULL) p_FreeBinAddr(qm, r);
  Shorter = shorter;
  p_Test(pNext(&rp), r);
  return pNext(&rp);
}
