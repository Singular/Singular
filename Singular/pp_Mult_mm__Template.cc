/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pp_Mult_mm__Template.cc
 *  Purpose: template for p_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pp_Mult_mm__Template.cc,v 1.6 2000-10-30 16:54:56 obachman Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*m
 *   Const:    p, m
 *   If spNoether != NULL, then monoms which are smaller
 *   then spNoether are cut
 *
 ***************************************************************/
poly pp_Mult_mm(poly p, const poly m, const poly spNoether, const ring ri)
{
  p_Test(p, ri);
  p_LmTest(m, ri);
  if (p == NULL) return NULL;
  spolyrec rp;
  poly q = &rp, r;
  number ln = pGetCoeff(m);
  omBin bin = ri->PolyBin;
  DECLARE_LENGTH(const unsigned long length = ri->ExpL_Size);
  const unsigned long* m_e = m->exp;
  pAssume(!n_IsZero(ln,ri));
  pAssume1(p_GetComp(m, ri) == 0 || p_MaxComp(p, ri) == 0);


  if (spNoether == NULL)
  {
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
  }
  else
  {
    poly r;
    while (p != NULL)
    {
      p_AllocBin( r, bin, ri);
      p_MemSum(r->exp, p->exp, m_e, length);
      p_MemAddAdjust(r, ri);
      if (p_LmCmp(r, spNoether, ri) == -1)
      {
        p_FreeBinAddr(r, ri);
        break;
      }
      q = pNext(q) = r;
      pSetCoeff0(q, n_Mult(ln, pGetCoeff(p), ri));
      pIter(p);
    }
  }
  pNext(q) = NULL;
  
  p_Test(pNext(&rp), ri);
  return pNext(&rp);
}


