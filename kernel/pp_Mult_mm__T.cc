/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pp_Mult_mm__Template.cc
 *  Purpose: template for p_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pp_Mult_mm__T.cc,v 1.2 2005-11-27 15:28:46 wienand Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*m
 *   Const:    p, m
 *
 ***************************************************************/
LINKAGE poly pp_Mult_mm(poly p, const poly m, const ring ri, poly &last)
{
  p_Test(p, ri);
  p_LmTest(m, ri);
  if (p == NULL)
  {
    last = NULL;
    return NULL;
  }
  spolyrec rp;
  poly q = &rp, r;
  number ln = pGetCoeff(m);
  omBin bin = ri->PolyBin;
  DECLARE_LENGTH(const unsigned long length = ri->ExpL_Size);
  const unsigned long* m_e = m->exp;
  pAssume(!n_IsZero(ln,ri));
  pAssume1(p_GetComp(m, ri) == 0 || p_MaxComp(p, ri) == 0);
  number tmp;

  do
  {
    tmp = n_Mult(ln, pGetCoeff(p), ri);
#ifdef HAVE_RING2TOM
    if (ri->cring==0 || (ri->cring ==1 && (long) tmp != 0)){
#endif
      p_AllocBin( pNext(q), bin, ri);
      q = pNext(q);
      pSetCoeff0(q, tmp);
      p_MemSum(q->exp, p->exp, m_e, length);
      p_MemAddAdjust(q, ri);
#ifdef HAVE_RING2TOM
    }
#endif
    p = pNext(p);
  }
  while (p != NULL);
  last = q;
  pNext(q) = NULL;

  p_Test(pNext(&rp), ri);
  return pNext(&rp);
}


