/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pp_Mult_mm__Template.cc
 *  Purpose: template for p_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pp_Mult_mm__T.cc,v 1.1.1.1 2003-10-06 12:16:02 Singular Exp $
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


  do
  {
    p_AllocBin( pNext(q), bin, ri);
    q = pNext(q);
    pSetCoeff0(q, n_Mult(ln, pGetCoeff(p), ri));
    p_MemSum(q->exp, p->exp, m_e, length);
    p_MemAddAdjust(q, ri);
    p = pNext(p);
  }
  while (p != NULL);
  last = q;
  pNext(q) = NULL;

  p_Test(pNext(&rp), ri);
  return pNext(&rp);
}


