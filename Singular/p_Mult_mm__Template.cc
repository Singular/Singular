/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Mult_mm__Template.cc
 *  Purpose: template for p_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Mult_mm__Template.cc,v 1.3 2000-09-12 16:01:07 obachman Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*m
 *   Const:    m
 *   Destroys: p  
 *
 ***************************************************************/
poly p_Mult_mm(poly p, const poly m, const ring ri)
{
  pTest(p);
  if (p == NULL) return NULL;
  poly q = p;
  number ln = pGetCoeff(m);
  DECLARE_LENGTH(const unsigned long length = ri->ExpLSize);
  const unsigned long* m_e = m->exp;
  assume(!p_nIsZero(ln,r));

  while (p != NULL)
  {
    pSetCoeff0(p, p_nMult(ln, pGetCoeff(p), ri));
    p_MemAdd(p->exp, m_e, length);
    p = pNext(p);
  }
  pTest(q);
  return q;
}


