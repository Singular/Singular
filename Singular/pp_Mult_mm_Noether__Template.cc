/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pp_Mult_mm__Template.cc
 *  Purpose: template for p_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pp_Mult_mm_Noether__Template.cc,v 1.1 2000-11-25 11:59:57 Singular Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*m
 *   Const:    p, m
 *   monoms which are smaller than spNoether are cut
 *
 ***************************************************************/
poly pp_Mult_mm_Noether(poly p, const poly m, const poly spNoether, int &ll, const ring ri, poly &last)
{
  p_Test(p, ri);
  p_LmTest(m, ri);
  if (p == NULL) 
  {
    last = NULL;
    ll = 0;
    return NULL;
  }
  spolyrec rp;
  poly q = &rp, r;
  number ln = pGetCoeff(m);
  omBin bin = ri->PolyBin;
  assume(spNoether != NULL);
  unsigned long *spNoether_exp = spNoether->exp;
  DECLARE_LENGTH(const unsigned long length = ri->ExpL_Size);
  DECLARE_ORDSGN(const long* ordsgn = ri->ordsgn);
  const unsigned long* m_e = m->exp;
  pAssume(!n_IsZero(ln,ri));
  pAssume1(p_GetComp(m, ri) == 0 || p_MaxComp(p, ri) == 0);
  int l = 0;
  
  while (p != NULL)
  {
    p_AllocBin( r, bin, ri);
    p_MemSum(r->exp, p->exp, m_e, length);
    p_MemAddAdjust(r, ri);
    p_MemCmp(r->exp, spNoether->exp, length, ordsgn, goto Continue, goto Continue, goto Break);
    
    Break:
    p_FreeBinAddr(r, ri);
    break;
    
    Continue:
    q = pNext(q) = r;
    pSetCoeff0(q, n_Mult(ln, pGetCoeff(p), ri));
    l++;
    pIter(p);
  }
  if (q  != &rp) last = q;
  pNext(q) = NULL;
  ll = l;

  p_Test(pNext(&rp), ri);
  return pNext(&rp);
}


