/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Mult_m__Template.cc
 *  Purpose: template for p_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Mult_m__Template.cc,v 1.1 2000-08-24 14:42:43 obachman Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*m
 *   Const:    p, m
 *   If spNoether != NULL, then monoms which are smaller
 *   then spNoether are cut
 *
 ***************************************************************/
poly p_Mult_m(poly p, const poly m, const poly spNoether, const ring ri)
{
  if (p == NULL) return NULL;
  spolyrec rp;
  poly q = &rp, r;
  number ln = pGetCoeff(m);
  omBin bin = ri->PolyBin;
  DECLARE_LENGTH(const unsigned long length = ri->ExpLSize);
  const unsigned long* m_e = m->exp.l;

  if (spNoether == NULL)
  {
    while (p != NULL)
    {
      omTypeAllocBin(poly, pNext(q), bin);
      q = pNext(q);
      pSetCoeff0(q, p_nMult(ln, pGetCoeff(p), ri));
      p_MemAdd(q->exp.l, p->exp.l, m_e, length);
      p = pNext(p);
    }
  }
  else
  {
    poly r;
    while (p != NULL)
    {
      omTypeAllocBin(poly, r, bin);
      p_MemAdd(r->exp.l, p->exp.l, m_e, length);

      if (prComp0(r, spNoether, ri) == -1)
      {
        omFreeBinAddr(r);
        break;
      }
      q = pNext(q) = r;
      pSetCoeff0(q, p_nMult(ln, pGetCoeff(p), ri));
      pIter(p);
    }
  }
  pNext(q) = NULL;

  return pNext(&rp);
}


