/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pp_Mult_mm__Template.cc
 *  Purpose: template for p_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pp_Mult_mm__Template.cc,v 1.1 2000-08-29 14:10:30 obachman Exp $
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
  if (p == NULL) return NULL;
  spolyrec rp;
  poly q = &rp, r;
  number ln = pGetCoeff(m);
  omBin bin = ri->PolyBin;
  DECLARE_LENGTH(const unsigned long length = ri->ExpLSize);
  const unsigned long* m_e = m->exp.l;
  assume(!p_nIsZero(ln,r));

  if (spNoether == NULL)
  {
    do
    {
      omTypeAllocBin(poly, pNext(q), bin);
      q = pNext(q);
      pSetCoeff0(q, p_nMult(ln, pGetCoeff(p), ri));
      p_MemAdd(q->exp.l, p->exp.l, m_e, length);
      p = pNext(p);
    }
    while (p != NULL);
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


