/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Mult_n__Template.cc
 *  Purpose: template for p_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Mult_nn__T.cc,v 1.2 2006-01-13 18:10:05 wienand Exp $
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
    number tmp = n_Mult(n, pGetCoeff(p), r);
#ifdef HAVE_RING2TOM
    if (r->cring==0 || (r->cring ==1 && (long) tmp != 0)){
#endif
       pSetCoeff0(p, tmp);
       pIter(p);
#ifdef HAVE_RING2TOM
    }
    else {
      p = pNext(p);    // TODO Free Monom OLIVER
    }
#endif
    n_Delete(&tmp, r);
  }
  p_Test(q, r);
  return q;
}


