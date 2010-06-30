/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Mult_n__Template.cc
 *  Purpose: template for p_Mult_n
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
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
#ifdef HAVE_ZERODIVISORS
  poly old = NULL;
#endif
  while (p != NULL)
  {
#ifndef HAVE_ZERODIVISORS
    n_InpMult(pGetCoeff(p), n, r);
    pIter(p);
#else
    number tmp = n_Mult(n, pGetCoeff(p), r);
    if (!nIsZero(tmp))
    {
       number nc = pGetCoeff(p);
       p_SetCoeff0(p, tmp, r);
       n_Delete(&nc, r);
       old = p;
       pIter(p);
    }
    else
    {
      n_Delete(&tmp, r);
      if (old == NULL)
      {
        pIter(p);
        p_LmDelete(&q, r);
      }
      else
      { 
        p_LmDelete(&p, r); 
        pNext(old) = p;
      }
    }
#endif
  }
  p_Test(q, r);
  return q;
}
