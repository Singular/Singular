/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    LPDV__pp_Mult_mm__Template.cc
 *  Purpose: template for LPDV__pp_Mult_mm
 *  Author:  obachman (Olaf Bachmann) (small Additions from
 *           gribo for the Letterplace DVec case)
 *  Created: 10/2012
 *  TODO:    See LPDV__p_Minus_mm_Mult_qq__Template.cc
 *******************************************************************/

#define HAVE_SHIFTBBADVEC

/***************************************************************
 *
 *   Returns:  p*m
 *   Const:    p, m
 *
 ***************************************************************/
LINKAGE poly LPDV__pp_Mult_mm__T
 ( poly p, const poly m, const ring ri )
{
  p_Test(p, ri);
  p_LmTest(m, ri);
  if (p == NULL)
  {
    return NULL;
  }
  spolyrec rp;
#ifdef HAVE_ZERODIVISORS
  rp.next = NULL;
#endif
  poly q = &rp;
  number ln = pGetCoeff(m);
  omBin bin = ri->PolyBin;
  DECLARE_LENGTH(const unsigned long length = ri->ExpL_Size);
  //const unsigned long* m_e = m->exp; //BOCO: unused
  pAssume(!n_IsZero__T(ln,ri));
  pAssume1(p_GetComp(m, ri) == 0 || p_MaxComp(p, ri) == 0);
  number tmp;

  do
  {
    tmp = n_Mult__T(ln, pGetCoeff(p), ri);
#ifdef HAVE_ZERODIVISORS
    if (! n_IsZero__T(tmp, ri))
    {
#endif
      p_AllocBin( pNext(q), bin, ri);
      q = pNext(q);
      pSetCoeff0(q, tmp);

#ifdef HAVE_SHIFTBBADVEC //BOCO: only change for LPDV in this file
      p_MemCopy__T(q->exp, p->exp, length);
      ri->p_ExpSum(q, p, m, ri);
#else
      p_MemSum__T(q->exp, p->exp, m_e, length);
      p_MemAddAdjust__T(q, ri);
#endif

#ifdef HAVE_ZERODIVISORS
    }
    else n_Delete__T(&tmp, ri);
#endif
    p = pNext(p);
  }
  while (p != NULL);
  pNext(q) = NULL;

  p_Test(pNext(&rp), ri);
  return pNext(&rp);
}


