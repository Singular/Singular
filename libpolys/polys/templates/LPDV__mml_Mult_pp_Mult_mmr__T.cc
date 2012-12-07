/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    LPDV__mml_Mult_pp_Mult_mmr__T.cc
 *  Purpose: template for LPDV__mml_Mult_pp_Mult_mmr__T
 *  Author:  obachman (Olaf Bachmann) (small Additions from
 *           gribo for the Letterplace DVec case)
 *  Created: 10/2012
 *  TODO:    See LPDV__p_Minus_mm_Mult_qq__Template.cc
 *******************************************************************/

#define HAVE_SHIFTBBADVEC

/***************************************************************
 *
 *   Returns:  ml*p*mr
 *   Const:    ml, p, mr
 *
 *   BOCO:
 *   ml has to have the right coefficient; we do not consider
 *   the coefficient of mr (if it has one, it will not be used)
 *
 ***************************************************************/
LINKAGE poly LPDV__mml_Mult_pp_Mult_mmr__T
 ( const poly ml, poly p, const poly mr, const ring ri )
{
  p_Test(p, ri);
  p_LmTest(ml, ri);
  p_LmTest(mr, ri);
  if (p == NULL)
  {
    return NULL;
  }
  spolyrec rp;
#ifdef HAVE_ZERODIVISORS
  rp.next = NULL;
#endif
  poly q = &rp;
  number lnl = pGetCoeff(ml);
  omBin bin = ri->PolyBin;
  DECLARE_LENGTH(const unsigned long length = ri->ExpL_Size);
  //const unsigned long* m_e = m->exp; //BOCO: unused
  pAssume(!n_IsZero__T(lnl,ri));
  pAssume1(p_GetComp(m, ri) == 0 || p_MaxComp(p, ri) == 0);
  number tmp;

  do
  {
    tmp = n_Mult__T(lnl, pGetCoeff(p), ri);
#ifdef HAVE_ZERODIVISORS
    if (! n_IsZero__T(tmp, ri))
    {
#endif
      p_AllocBin( pNext(q), bin, ri);
      q = pNext(q);
      pSetCoeff0(q, tmp);

      p_MemCopy__T(q->exp, ml->exp, length);
      ri->p_ExpSum(q, ml, p, ri);
      ri->p_ExpSum(q, q, mr, ri);
      p_MemAddAdjust__T(q, ri);

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


