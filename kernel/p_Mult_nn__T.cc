/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Mult_n__Template.cc
 *  Purpose: template for p_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Mult_nn__T.cc,v 1.1.1.1 2003-10-06 12:16:00 Singular Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  p*n
 *   Destroys: p
 *   Const:    n
 *
 ***************************************************************/
LINKAGE poly p_Mult_nn(poly p, const number n, const ring r)
{
  pAssume(!n_IsZero(n,r));
  p_Test(p, r);

  poly q = p;
  while (p != NULL)
  {
    number nc = pGetCoeff(p);
    pSetCoeff0(p, n_Mult(n, nc, r));
    n_Delete(&nc, r);
    pIter(p);
  }
  p_Test(q, r);
  return q;
}


