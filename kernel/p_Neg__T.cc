/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Neg__Template.cc
 *  Purpose: template for p_Neg
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Neg__T.cc,v 1.1.1.1 2003-10-06 12:16:00 Singular Exp $
 *******************************************************************/

/***************************************************************
 *
 *   Returns:  -p
 *   Destroys: p
 *
 ***************************************************************/
LINKAGE poly p_Neg(poly p, const ring r)
{
  poly q = p;
  while (p != NULL)
  {
    pSetCoeff0(p, n_Neg(pGetCoeff(p), r));
    pIter(p);
  }
  return q;
}


