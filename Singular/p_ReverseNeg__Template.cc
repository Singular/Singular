/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_ReverseNeg_Template.cc
 *  Purpose: template for p_ReverseNeg
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_ReverseNeg__Template.cc,v 1.1 2000-08-29 14:10:28 obachman Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  -p and monomials of p are reversed
 *   Destroys: p
 *
 ***************************************************************/
poly p_ReverseNeg(poly p, const ring r)
{
  if (p == NULL) return p;
  pSetCoeff0(p, p_nNeg(pGetCoeff(p), r));
  if (pNext(p) == NULL) return p;

  poly q = pNext(p), // == pNext(p)
    qn;
  pNext(p) = NULL;
  do
  {
    qn = pNext(q);
    pNext(q) = p;
    p = q;
    pSetCoeff0(p, p_nNeg(pGetCoeff(p), r));
    q = qn;
  }
  while (qn != NULL);
  return p;
}


