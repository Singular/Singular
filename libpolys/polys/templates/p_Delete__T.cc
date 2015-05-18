/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Delete__Template.cc
 *  Purpose: template for p_Delete
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *******************************************************************/

LINKAGE void p_Delete__T(poly* pp, const ring r)
{
  poly p = *pp;

  while (p != NULL)
  {
    n_Delete__T(&(p->coef), r->cf);
    p = p_LmFreeAndNext(p, r);
  }
  *pp = NULL;
}

