/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Mult_mm__Template.cc
 *  Purpose: template for p_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Mult_mm__T.cc,v 1.7 2008-02-06 12:51:41 wienand Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*m
 *   Const:    m
 *   Destroys: p
 *
 ***************************************************************/
LINKAGE poly p_Mult_mm(poly p, const poly m, const ring ri)
{
  p_Test(p, ri);
  p_LmTest(m, ri);
  if (p == NULL) return NULL;
  pAssume(m != NULL);
  poly q = p;
  number ln = pGetCoeff(m);
  number pn;
  DECLARE_LENGTH(const unsigned long length = ri->ExpL_Size);
  const unsigned long* m_e = m->exp;
  pAssume(!n_IsZero(ln,ri));

#ifdef HAVE_ZERODIVISORS
  poly before = p;
#endif
  while (p != NULL)
  {
    pn = pGetCoeff(p);
    number tmp = n_Mult(ln, pn, ri);
#ifdef HAVE_ZERODIVISORS
    if (n_IsZero(tmp, ri))
    {
      n_Delete(&tmp, ri);
      if (before == p)
      {
        p = p_LmDeleteAndNext(p, ri);
        before = p;
        q = p;
      }
      else
      {
        p = p_LmDeleteAndNext(p, ri);
        pNext(before) = p;
      }
    }
    else
#endif
    {
      pSetCoeff0(p, tmp);
      n_Delete(&pn, ri);
      p_MemAdd(p->exp, m_e, length);
      p_MemAddAdjust(p, ri);
#ifdef HAVE_ZERODIVISORS
      before = p;
#endif
      p = pNext(p);
    }
  }
  p_Test(q, ri);
  return q;
}


