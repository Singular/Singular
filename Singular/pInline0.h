/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pInline.cc
 *  Purpose: implementation of poly Level 0 functions
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pInline0.h,v 1.1 2000-09-12 16:01:04 obachman Exp $
 *******************************************************************/
#ifndef PINLINE0_H
#define PINLINE0_H

#if defined(DO_PINLINE0)
#define PINLINE0 static inline
#else
#define PINLINE0 
#endif

#include "mod2.h"
#include "polys.h"
  
PINLINE0 void p_SetCompP(poly p, int i, ring r)
{
  while (p != NULL)
  {
    p_SetComp(p, i, r);
    p_SetmComp(p, r);
    pIter(p);
  }
}

PINLINE0 void p_SetCompP(poly p, int i, ring lmRing, ring tailRing)
{
  if (p != NULL)
  {
    p_SetComp(p, i, lmRing);
    p_SetmComp(p, lmRing);
    p_SetCompP(pNext(p), i, tailRing);
  }
}

PINLINE0 poly pReverse(poly p)
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

#endif // PINLINE_CC

