/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pp_Mult_mm__Template.cc
 *  Purpose: template for p_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pp_Mult_mm_Noether__T.cc,v 1.1.1.1 2003-10-06 12:16:02 Singular Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*m, ll
 *   ll == pLength(p*m) , if on input ll < 0
 *   pLength(p) - pLength(p*m), if on input ll >= 0
 *   Const:    p, m
 *
 ***************************************************************/
LINKAGE poly pp_Mult_mm_Noether(poly p, const poly m, const poly spNoether, int &ll, const ring ri, poly &last)
{
  p_Test(p, ri);
  p_LmTest(m, ri);
  assume(spNoether != NULL);
  if (p == NULL)
  {
    ll = 0;
    last = NULL;
    return NULL;
  }
  spolyrec rp;
  poly q = &rp, r;
  const unsigned long *spNoether_exp = spNoether->exp;
  number ln = pGetCoeff(m);
  omBin bin = ri->PolyBin;
  DECLARE_LENGTH(const unsigned long length = ri->ExpL_Size);
  DECLARE_ORDSGN(const long* ordsgn = ri->ordsgn);
  const unsigned long* m_e = m->exp;
  pAssume(!n_IsZero(ln,ri));
  pAssume1(p_GetComp(m, ri) == 0 || p_MaxComp(p, ri) == 0);
  int l = 0;

  do
  {
    p_AllocBin(r, bin, ri);
    p_MemSum(r->exp, p->exp, m_e, length);
    p_MemAddAdjust(r, ri);

    p_MemCmp(r->exp, spNoether_exp, length, ordsgn, goto Continue, goto Continue, goto Break);

    Break:
    p_FreeBinAddr(r, ri);
    break;

    Continue:
    l++;
    q = pNext(q) = r;
    pSetCoeff0(q, n_Mult(ln, pGetCoeff(p), ri));
    pIter(p);
  } while (p != NULL);

  if (ll < 0)
    ll = l;
  else
    ll = pLength(p);

  if (q != &rp)
    last = q;
  pNext(q) = NULL;

  p_Test(pNext(&rp), ri);
  return pNext(&rp);
}


