/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pInline.cc
 *  Purpose: implementation of poly related inline routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pInline.cc,v 1.1 2000-08-29 14:10:26 obachman Exp $
 *******************************************************************/
#ifndef PINLINE_CC
#define PINLINE_CC

#if !defined(NO_PINLINE) || defined(POLYS_IMPL_CC)

PINLINE poly pReverse(poly p)
{
  if (p == NULL || pNext(p) == NULL) return p;

  poly q = pNext(p), // == pNext(p)
    qn;
  pNext(p) = NULL;
  do
  {
    qn = pNext(q);
    pNext(q) = p;
    p = q;
    q = qn;
  }
  while (qn != NULL);
  return p;
}


#endif // defined(PINLINE) || defined(POLYS_CC)
#endif // PINLINE_CC

