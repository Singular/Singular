/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Delete__Template.cc
 *  Purpose: template for p_Delete
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Delete__Template.cc,v 1.1 2000-08-24 14:42:43 obachman Exp $
 *******************************************************************/

void p_Delete(poly* pp, const ring r)
{
  poly p = *pp;
  
  while (p != NULL)
  {
    p_nDelete(&(p->coef), r);
    FreeAndAdvance(p);
  }
  *pp = NULL;
}

