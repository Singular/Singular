/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Mult_mm__Template.cc
 *  Purpose: template for p_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Mult_mm__T.cc,v 1.2 2006-01-13 18:10:05 wienand Exp $
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
  poly before = p;
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

  while (p != NULL)
  {
    pn = pGetCoeff(p);
    number tmp = n_Mult(ln, pn, ri);
#ifdef HAVE_RING2TOM
    if (n_IsZero(tmp, ri)) {
      if (before == p) {
        p = pNext(p);
        before = p;
        q = p;
      }
      else {
        pNext(before) = pNext(p);
        p_LmFree(p, ri);
        p = pNext(before);
      }
    }
    else
#endif
    {
      pSetCoeff0(p, tmp);
      n_Delete(&pn, ri);
      p_MemAdd(p->exp, m_e, length);
      p_MemAddAdjust(p, ri);
#ifdef HAVE_RING_2TOM
      before = p;
#endif
      p = pNext(p);
    }
  }
  p_Test(q, ri);
  return q;
}


