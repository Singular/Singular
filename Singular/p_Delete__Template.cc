/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Delete__Template.cc
 *  Purpose: template for p_Delete
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Delete__Template.cc,v 1.3 2000-12-07 15:03:57 obachman Exp $
 *******************************************************************/

LINKAGE void p_Delete(poly* pp, const ring r)
{
  poly p = *pp;
  
  while (p != NULL)
  {
    n_Delete(&(p->coef), r);
    p = p_LmFreeAndNext(p, r);
  }
  *pp = NULL;
}

