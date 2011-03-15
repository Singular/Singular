// -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file DebugPrint.cc
 * 
 * Here we implement dPrint-s.
 *
 * ABSTRACT: debug-detailed-printing
 *
 * @author Oleksandr Motsak
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

// include header file
#include <kernel/mod2.h>

#include "DebugPrint.h"

#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/p_polys.h>
#include <kernel/ideals.h>



BEGIN_NAMESPACE()
/// debug-print monomial poly/vector p, assuming that it lives in the ring R
static inline void m_DebugPrint(const poly p, const ring R)
{
  Print("\nexp[0..%d]\n", R->ExpL_Size - 1);
  for(int i = 0; i < R->ExpL_Size; i++)
    Print("%09lx ", p->exp[i]);
  PrintLn();
  Print("v0:%9ld ", p_GetComp(p, R));
  for(int i = 1; i <= R->N; i++) Print(" v%d:%5ld",i, p_GetExp(p, i, R));
  PrintLn();
}
END_NAMESPACE

BEGIN_NAMESPACE_SINGULARXX    BEGIN_NAMESPACE(DEBUG)

// debug-print at most nTerms (2 by default) terms from poly/vector p,
// assuming that lt(p) lives in lmRing and tail(p) lives in tailRing.
void dPrint(const poly p, const ring lmRing, const ring tailRing, const int nTerms)
{
  assume( nTerms >= 0 );
  if( p != NULL )
  {
    assume( p != NULL );

    p_Write(p, lmRing, tailRing);

    if( (p != NULL) && (nTerms > 0) )
    {
      assume( p != NULL );
      assume( nTerms > 0 );

      // debug pring leading term
      m_DebugPrint(p, lmRing);

      poly q = pNext(p); // q = tail(p)

      // debug pring tail (at most nTerms-1 terms from it)
      for(int j = nTerms - 1; (q !=NULL) && (j > 0); pIter(q), --j)
        m_DebugPrint(q, tailRing);

      if (q != NULL)
        PrintS("...\n");
    }
  }
  else
    PrintS("0\n");
}

// output an ideal
void dPrint(const ideal id, const ring lmRing, const ring tailRing, const int nTerms)
{
  assume( nTerms >= 0 );

  if( id == NULL )
    PrintS("(NULL)");
  else
  {
    Print("Module of rank %ld,real rank %ld and %d generators.\n",
          id->rank,idRankFreeModule(id, lmRing, tailRing),IDELEMS(id));

    int j = (id->ncols*id->nrows) - 1;
    while ((j > 0) && (id->m[j]==NULL)) j--;
    for (int i = 0; i <= j; i++)
    {
      Print("generator %d: ",i); dPrint(id->m[i], lmRing, tailRing, nTerms);
    }
  }
}

END_NAMESPACE               END_NAMESPACE_SINGULARXX


// Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab
