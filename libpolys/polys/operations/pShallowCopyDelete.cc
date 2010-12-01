/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pShallowCopyDelete.cc
 *  Purpose: implementation of pShallowCopyDelete routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/
#include <kernel/mod2.h>
#include <kernel/p_polys.h>
#include <kernel/ring.h>
#include <kernel/pShallowCopyDelete.h>

// a simple implementations
poly pShallowCopyDelete_General(poly s_p, ring s_r, ring d_r, omBin d_bin)
{
  p_CheckPolyRing(s_p, s_r);
  p_CheckRing(d_r);
  assume(d_bin != NULL);
  assume(d_bin == d_r->PolyBin || d_bin->sizeW == d_r->PolyBin->sizeW);
  assume(s_r->N == d_r->N);

  spolyrec dp;
  poly d_p = &dp;
  int N = d_r->N;
  int i;


  while (s_p != NULL)
  {
    d_p->next = p_Init(d_r, d_bin);
    pIter(d_p);

    pSetCoeff0(d_p, pGetCoeff(s_p));
    for (i=1; i<= N; i++)
      p_SetExp(d_p, i, p_GetExp(s_p, i, s_r), d_r);

    if (rRing_has_Comp(d_r))
      p_SetComp(d_p, p_GetComp(s_p, s_r), d_r);
    p_Setm(d_p, d_r);

    s_p = p_LmFreeAndNext(s_p, s_r);
  }
  pNext(d_p) = NULL;

  return dp.next;
}

pShallowCopyDeleteProc pGetShallowCopyDeleteProc(ring source_r, ring dest_r)
{
  return pShallowCopyDelete_General;
}
