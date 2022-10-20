/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pDebug.h
 *  Purpose: implementation of debug related poly routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *******************************************************************/

#ifndef PDEBUG_CC
#define PDEBUG_CC

#include <stdarg.h>
#include <stdio.h>





#include "misc/auxiliary.h"


#ifdef PDEBUG

// do the following to always enforce checking of pSetm
// #undef PDEBUG
// #define PDEBUG 2

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"

#include "coeffs/coeffs.h"

/***************************************************************
 *
 * Error reporting
 *
 ***************************************************************/
// avoid recursive calls
STATIC_VAR BOOLEAN d_poly_error_reporting = FALSE;
BOOLEAN dPolyReportError(poly p, ring r, const char* fmt, ...)
{
  if (d_poly_error_reporting) return FALSE;
  d_poly_error_reporting = TRUE;
  va_list ap;
  va_start(ap, fmt);

  fprintf(stderr, "\n// ***dPolyReportError: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n occurred at\n");
  #ifdef HAVE_OMALLOC
  omPrintCurrentBackTraceMax(stderr, 8);
  #endif
  if (p != NULL)
  {
    fprintf(stderr, " occurred for poly: ");
    p_wrp(p, r);
    omPrintAddrInfo(stderr, p, " ");
  }
  dErrorBreak();
  d_poly_error_reporting = FALSE;
  return FALSE;
}

/***************************************************************
 *
 * checking for ring stuff
 *
 ***************************************************************/
BOOLEAN p_LmCheckIsFromRing(poly p, ring r)
{
  if (p != NULL)
  {
    #if (OM_TRACK > 0) && defined(OM_TRACK_CUSTOM)
    void* custom = omGetCustomOfAddr(p);
    if (custom != NULL)
    {
      pPolyAssumeReturnMsg(custom == r ||
                           // be more sloppy for qrings
                           (r->qideal != NULL &&
                            omIsBinPageAddr(p) &&
                            omSizeWOfAddr(p)==omSizeWOfBin(r->PolyBin)) ||
                           rSamePolyRep((ring) custom, r),
                           "monomial not from specified ring",p,r);
      return TRUE;
    }
    else
    #endif
    #ifndef X_OMALLOC
    {
      _pPolyAssumeReturn(omIsBinPageAddr(p),p,r);
      _pPolyAssumeReturn(omSizeWOfAddr(p)==omSizeWOfBin(r->PolyBin),p,r);
      return TRUE;
    }
    return FALSE;
    #endif
  }
  return TRUE;
}

BOOLEAN p_CheckIsFromRing(poly p, ring r)
{
  while (p!=NULL)
  {
    pFalseReturn(p_LmCheckIsFromRing(p, r));
    pIter(p);
  }
  return TRUE;
}

BOOLEAN p_CheckPolyRing(poly p, ring r)
{
  #ifndef X_OMALLOC
  pAssumeReturn(r != NULL && r->PolyBin != NULL);
  #endif
  return p_CheckIsFromRing(p, r);
}

BOOLEAN p_LmCheckPolyRing(poly p, ring r)
{
  #ifndef X_OMALLOC
  pAssumeReturn(r != NULL && r->PolyBin != NULL);
  #endif
  pAssumeReturn(p != NULL);
  return p_LmCheckIsFromRing(p, r);
}
BOOLEAN p_CheckRing(ring r)
{
  #ifndef X_OMALLOC
  pAssumeReturn(r != NULL && r->PolyBin != NULL);
  #endif
  return TRUE;
}

/***************************************************************
 *
 * Debugging/statistics of pDivisibleBy
 *
 ***************************************************************/
BOOLEAN p_DebugLmDivisibleByNoComp(poly a, poly b, ring r)
{
  int i=r->N;

  do
  {
    if (p_GetExp(a,i,r) > p_GetExp(b,i,r))
      return FALSE;
    i--;
  }
  while (i);
#ifdef HAVE_RINGS
  return n_DivBy(pGetCoeff(b), pGetCoeff(a), r->cf);
#else
  return TRUE;
#endif
     }


/***************************************************************
 *
 * Misc things helpful for debugging
 *
 ***************************************************************/
BOOLEAN pIsMonomOf(poly p, poly m)
{
  if (m == NULL) return TRUE;
  while (p != NULL)
  {
    if (p == m) return TRUE;
    pIter(p);
  }
  return FALSE;
}
BOOLEAN pHaveCommonMonoms(poly p, poly q)
{
  while (p != NULL)
  {
    if (pIsMonomOf(q, p))
    {
      return TRUE;
    }
    pIter(p);
  }
  return FALSE;
}

/***************************************************************
 *
 * Testing of polys
 *
 ***************************************************************/
extern void p_Setm_General(poly p, ring r);

static poly p_DebugInit(poly p, ring src_ring, ring dest_ring)
{
  poly d_p = p_Init(dest_ring);
  int i;
  assume(dest_ring->N == src_ring->N);

  for (i=1; i<= src_ring->N; i++)
  {
    p_SetExp(d_p, i, p_GetExp(p, i, src_ring), dest_ring);
  }
  if (rRing_has_Comp(dest_ring))
    p_SetComp(d_p, p_GetComp(p, src_ring), dest_ring);

  p_Setm_General(d_p, dest_ring);
  return d_p;
}

BOOLEAN _p_Test(poly p, ring r, int level)
{
  assume(r->cf !=NULL);

  if (PDEBUG > level) level = PDEBUG;
  if (level < 0 || p == NULL) return TRUE;

  poly p_prev = NULL;

  #ifndef OM_NDEBUG
  #ifndef X_OMALLOC
  // check addr with level+1 so as to check bin/page of addr
  _pPolyAssumeReturnMsg(omTestBinAddrSize(p, (omSizeWOfBin(r->PolyBin))*SIZEOF_LONG, level+1)
                        == omError_NoError, "memory error",p,r);
  #endif
  #endif

  pFalseReturn(p_CheckRing(r));

  // this checks that p does not contain a loop: rather expensive O(length^2)
  #ifndef OM_NDEBUG
  if (level > 1)
    pFalseReturn(omTestList(p, level) == omError_NoError);
  #endif

  int ismod = p_GetComp(p, r) != 0;

  while (p != NULL)
  {
    // ring check
    pFalseReturn(p_LmCheckIsFromRing(p, r));
    #ifndef OM_NDEBUG
    #ifndef X_OMALLOC
    // omAddr check
    _pPolyAssumeReturnMsg(omTestBinAddrSize(p, (omSizeWOfBin(r->PolyBin))*SIZEOF_LONG, 1)
                     == omError_NoError, "memory error",p,r);
    #endif
    #endif
    // number/coef check
    _pPolyAssumeReturnMsg(p->coef != NULL || (n_GetChar(r->cf) >= 2), "NULL coef",p,r);

    #ifdef LDEBUG
    _pPolyAssumeReturnMsg(n_Test(p->coef,r->cf),"coeff err",p,r);
    #endif
    _pPolyAssumeReturnMsg(!n_IsZero(p->coef, r->cf), "Zero coef",p,r);

    // check for valid comp
    _pPolyAssumeReturnMsg(p_GetComp(p, r) >= 0 && (p_GetComp(p, r)<65000), "component out of range ?",p,r);
    // check for mix poly/vec representation
    _pPolyAssumeReturnMsg(ismod == (p_GetComp(p, r) != 0), "mixed poly/vector",p,r);

    // special check for ringorder_s/S
    if ((r->typ!=NULL) && (r->typ[0].ord_typ == ro_syzcomp))
    {
      long c1, cc1, ccc1, ec1;
      sro_ord* o = &(r->typ[0]);

      c1 = p_GetComp(p, r);
      if (o->data.syzcomp.Components!=NULL)
      {
        cc1 = o->data.syzcomp.Components[c1];
        ccc1 = o->data.syzcomp.ShiftedComponents[cc1];
      }
      else { cc1=0; ccc1=0; }
      _pPolyAssumeReturnMsg(c1 == 0 || cc1 != 0, "Component <-> TrueComponent zero mismatch",p,r);
      _pPolyAssumeReturnMsg(c1 == 0 || ccc1 != 0,"Component <-> ShiftedComponent zero mismatch",p,r);
      ec1 = p->exp[o->data.syzcomp.place];
      //pPolyAssumeReturnMsg(ec1 == ccc1, "Shifted comp out of sync. should %d, is %d");
      if (ec1 != ccc1)
      {
        dPolyReportError(p,r,"Shifted comp out of sync. should %d, is %d",ccc1,ec1);
        return FALSE;
      }
    }

    // check that p_Setm works ok
    if (level > 0)
    {
      poly p_should_equal = p_DebugInit(p, r, r);
      _pPolyAssumeReturnMsg(p_ExpVectorEqual(p, p_should_equal, r), "p_Setm field(s) out of sync",p,r);
      p_LmFree(p_should_equal, r);
    }

    // check order
    if (p_prev != NULL)
    {
      int cmp = p_LmCmp(p_prev, p, r);
      if (cmp == 0)
      {
        _pPolyAssumeReturnMsg(0, "monoms p and p->next are equal", p_prev, r);
      }
      else
        _pPolyAssumeReturnMsg(p_LmCmp(p_prev, p, r) == 1, "wrong order", p_prev, r);

      // check that compare worked sensibly
      if (level > 1 && p_GetComp(p_prev, r) == p_GetComp(p, r))
      {
        int i;
        for (i=r->N; i>0; i--)
        {
          if (p_GetExp(p_prev, i, r) != p_GetExp(p, i, r)) break;
        }
        _pPolyAssumeReturnMsg(i > 0, "Exponents equal but compare different", p_prev, r);
      }
    }
    p_prev = p;
    pIter(p);
  }
  return TRUE;
}

BOOLEAN _p_LmTest(poly p, ring r, int level)
{
  if (level < 0 || p == NULL) return TRUE;
  poly pnext = pNext(p);
  pNext(p) = NULL;
  BOOLEAN test_res = _p_Test(p, r, level);
  pNext(p) = pnext;
  return test_res;
}

BOOLEAN _pp_Test(poly p, ring lmRing, ring tailRing, int level)
{
  if (PDEBUG > level) level = PDEBUG;
  if (level < 0 || p == NULL) return TRUE;
  if (pNext(p) == NULL || lmRing == tailRing) return _p_Test(p, lmRing, level);

  pFalseReturn(_p_LmTest(p, lmRing, level));
  pFalseReturn(_p_Test(pNext(p), tailRing, level));

  // check that lm > Lm(tail)
  if (level > 1)
  {
    poly lm = p;
    poly tail = p_DebugInit(pNext(p), tailRing, lmRing);
    poly pnext = pNext(lm);
    pNext(lm) = tail;
    BOOLEAN cmp = p_LmCmp(lm, tail, lmRing);
    if (cmp != 1)
      dPolyReportError(lm, lmRing, "wrong order: lm <= Lm(tail)");
    p_LmFree(tail, lmRing);
    pNext(lm) = pnext;
    return (cmp == 1);
  }
  return TRUE;
}

#endif // PDEBUG

#endif // PDEBUG_CC
