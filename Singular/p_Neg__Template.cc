/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Neg__Template.cc
 *  Purpose: template for p_Neg
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Neg__Template.cc,v 1.1 2000-08-29 14:10:27 obachman Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  -p
 *   Destroys: p
 *
 ***************************************************************/
poly p_Neg(poly p, const ring r)
{
  poly q = p;
  while (p != NULL)
  {
    pSetCoeff0(p, p_nNeg(pGetCoeff(p), r));
    pIter(p);
  }
  return q;
}


