/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-impl.cc,v 1.50 2000-09-12 16:01:09 obachman Exp $ */

/***************************************************************
 *
 * File:       polys-impl.cc
 * Purpose:    low-level procuders for polys.
 *
 * If you touch anything here, you better know what you are doing.
 * What is here should not be used directly from other routines -- the
 * encapsulations in polys.[h,cc] should be used, instead.
 *
 ***************************************************************/
#ifndef POLYS_IMPL_CC
#define POLYS_IMPL_CC

#include <stdio.h>
#include <string.h>
#include "mod2.h"

#include "omalloc.h"
#ifdef PDEBUG
#undef NO_INLINE3
#define NO_INLINE3
#endif
#include "polys-impl.h"

#include "tok.h"
#include "structs.h"
#include "febase.h"
#include "numbers.h"
#include "polys.h"
#include "ring.h"
#include "p_Procs.h"
#include "dError.h"

#ifdef PDEBUG
int pDBsyzComp=0;
#endif

/***************************************************************
 *
 * Storage Managament Routines
 *
 ***************************************************************/
#ifdef PDEBUG
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
#endif


void ppDelete(poly* p, ring rg)
{
  ring origRing = currRing;
  rChangeCurrRing(rg, FALSE);
  pDelete(p);
  rChangeCurrRing(origRing, FALSE);
}

poly _pShallowCopyDelete(omBin d_h, poly *p, omBin s_h)
{
  spolyrec dp;
  poly d_p = &dp, tmp;
  poly s_p = *p;

  assume(d_h != NULL && s_h != NULL &&
         d_h->sizeW == s_h->sizeW);

  if (currRing->ExpLSize <= 2)
  {
    if (currRing->ExpLSize == 1)
    {
      while (s_p != NULL)
      {
        omTypeAllocBin( poly,d_p->next, d_h);
        d_p = d_p->next;

        d_p->coef = s_p->coef;
        d_p->exp[0] = s_p->exp[0];

        tmp = pNext(s_p);
        omFreeBin(s_p, s_h);
        s_p = tmp;
      }
    }
    else
    {
      while (s_p != NULL)
      {
        omTypeAllocBin( poly,d_p->next, d_h);
        d_p = d_p->next;

        d_p->coef = s_p->coef;
        d_p->exp[0] = s_p->exp[0];
        d_p->exp[1] = s_p->exp[1];

        tmp = pNext(s_p);
        omFreeBin(s_p, s_h);
        s_p = tmp;
      }
    }
  }
  else
  {
    if (currRing->ExpLSize & 1)
    {
      while (s_p != NULL)
      {

        omTypeAllocBin( poly,d_p->next, d_h);
        d_p = d_p->next;

        d_p->coef = s_p->coef;
        omMemcpy_nwODD(&(d_p->exp[0]), &(s_p->exp[1]), currRing->ExpLSize);

        tmp = pNext(s_p);
        omFreeBin(s_p, s_h);
        s_p = tmp;
      }
    }
    else
    {
      while (s_p != NULL)
      {

        omTypeAllocBin(poly,d_p->next, d_h);
        d_p = d_p->next;

        d_p->coef = s_p->coef;
        omMemcpy_nwEVEN(&(d_p->exp[0]), &(s_p->exp[1]), currRing->ExpLSize);

        tmp = pNext(s_p);
        omFreeBin(s_p, s_h);
        s_p = tmp;
      }
    }
  }
  pNext(d_p) = NULL;
  pHeapTest(dp.next, d_h);
  *p = NULL;
  return pNext(dp.next);
}

poly pHeadProc(poly p)
{
  return pHead(p);
}

#ifdef PDEBUG
BOOLEAN prDBTest(poly p, ring r, char* f, int l)
{
  ring old_ring = NULL;
  BOOLEAN res;

  if (r != currRing)
  {
    old_ring = currRing;
    rChangeCurrRing(r);
  }
  res = pDBTest(p, currRing->PolyBin, f, l);
  if (old_ring != NULL)
  {
    rChangeCurrRing(old_ring);
  }
  return res;
}


BOOLEAN pDBTest(poly p, char *f, int l)
{
  return pDBTest(p, currPolyBin, f,l);
}

BOOLEAN pDBTest(poly p, omBin heap, char *f, int l)
{
  poly old=NULL;
  BOOLEAN ismod=FALSE;
  if (heap == NULL) heap = currPolyBin;

  while (p!=NULL)
  {
    omCheckIf(omCheckAddrBin(p, heap), return FALSE);
#ifdef LDEBUG
    if (!nDBTest(p->coef,f,l))
      return FALSE;
#endif
    if ((p->coef==NULL)&&(nGetChar()<2))
    {
      dReportError("NULL coef in poly in %s:%d",f,l);
      return FALSE;
    }
    if (nIsZero(p->coef))
    {
      dReportError("zero coef in poly in %s:%d",f,l);
      return FALSE;
    }
    int i=pVariables;
    if (ismod==0)
    {
      if (pGetComp(p)>0) ismod=1;
      else               ismod=2;
    }
    else if (ismod==1)
    {
      if (pGetComp(p)==0)
      {
        dReportError("mix vec./poly in %s:%d",f,l);
        return FALSE;
      }
    }
    else if (ismod==2)
    {
      if (pGetComp(p)!=0)
      {
        dReportError("mix poly/vec. in %s:%d",f,l);
        return FALSE;
      }
    }
    if (currRing->order[1] == ringorder_S)
    {
      long c1, cc1, ccc1, ec1;
      sro_ord* o = &(currRing->typ[1]);

      c1 = pGetComp(p);
      cc1 = o->data.syzcomp.Components[c1];
      ccc1 = o->data.syzcomp.ShiftedComponents[cc1];
      if (! (c1 == 0 || cc1 != 0))
      {
        dReportError("Component <-> TrueComponent zero mismatch", f, l);
        return FALSE;
      }
      if (! (c1 == 0 || ccc1 != 0))
      {
        dReportError("Component <-> ShiftedComponent zero mismatch", f, l);
        return FALSE;
      }
      ec1 = p->exp[currRing->typ[1].data.syzcomp.place];
      if (ec1 != ccc1)
      {
        dReportError("Shifted comp out of sync. should %d, is %d", ccc1, ec1);
        return FALSE;
      }
    }
    if (currRing->order[0] == ringorder_s)
    {
      unsigned long syzindex = p->exp[currRing->typ[0].data.syz.place];
      pSetm(p);
      if (p->exp[currRing->typ[0].data.syz.place] != syzindex)
      {
        dReportError("Syzindex wrong: Was %dl but should be %d in %s:%d",
             syzindex, p->exp[currRing->typ[0].data.syz.place], f, l);
      }
    }
    old=p;
    pIter(p);
    if (pCmp(old,p)!=1)
    {
      dReportError("wrong order (");
      wrp(old);
      Print(") in %s:%d (pComp=%d)\n",f,l,pCmp(old,p));
      return FALSE;
    }
    if (p != NULL)
    {
      if (pGetComp(old) == pGetComp(p))
      {
        i = pVariables;
        for (i=pVariables;i; i--)
        {
          if (pGetExp(old, i) != pGetExp(p, i)) break;
        }
        if (i == 0)
        {
          dReportError("different Compare, but same exponent vector for");
          wrp(old);
          Print(" in %s%d\n", f, l);
          return FALSE;
        }
      }
    }
  }
  return TRUE;
}

#endif // PDEBUG

static inline unsigned long GetBitFields(Exponent_t e,
                                         unsigned int s, unsigned int n)
{
  unsigned int i = 0, ev = 0;
  assume(n > 0 && s < BIT_SIZEOF_LONG);
  do
  {
    assume(s+i < BIT_SIZEOF_LONG);
    if (e > (Exponent_t) i) ev |= Sy_bit(s+i);
    else break;
    i++;
  }
  while (i < n);
  return ev;
}

// Short Exponent Vectors are used for fast divisibility tests
// ShortExpVectors "squeeze" an exponent vector into one word as follows:
// Let n = BIT_SIZEOF_LONG / pVariables.
// If n == 0 (i.e. pVariables > BIT_SIZE_OF_LONG), let m == the number
// of non-zero exponents. If (m>BIT_SIZEOF_LONG), then sev = ~0, else
// first m bits of sev are set to 1.
// Otherwise (i.e. pVariables <= BIT_SIZE_OF_LONG)
// represented by a bit-field of length n (resp. n+1 for some
// exponents). If the value of an exponent is greater or equal to n, then
// all of its respective n bits are set to 1. If the value of an exponent
// is smaller than n, say m, then only the first m bits of the respective
// n bits are set to 1, the others are set to 0.
// This way, we have:
// exp1 / exp2 ==> (ev1 & ~ev2) == 0, i.e.,
// if (ev1 & ~ev2) then exp1 does not divide exp2
unsigned long p_GetShortExpVector(poly p, ring r)
{
  assume(p != NULL);
  if (p == NULL) return 0;
  unsigned long ev = 0; // short exponent vector
  unsigned int n = BIT_SIZEOF_LONG / r->N; // number of bits per exp
  unsigned int m1; // highest bit which is filled with (n+1)
  unsigned int i = 0, j=1;

  if (n == 0)
  {
    for (; j<=(unsigned long) r->N; j++)
    {
      if (p_GetExp(p,j,r) > 0) i++;
      if (i == BIT_SIZEOF_LONG) break;
    }
    ev = (unsigned long) ~0 >> ((unsigned long) (BIT_SIZEOF_LONG - i));
    return ev;
  }
  else
  {
    m1 = (n+1)*(BIT_SIZEOF_LONG - n*r->N);
  }

  n++;
  while (i<m1)
  {
    ev |= GetBitFields(p_GetExp(p, j,r), i, n);
    i += n;
    j++;
  }

  n--;
  while (i<BIT_SIZEOF_LONG)
  {
    ev |= GetBitFields(p_GetExp(p, j,r), i, n);
    i += n;
    j++;
  }
  return ev;
}

#ifdef PDIV_DEBUG
static int pDivisibleBy_number = 1;
static int pDivisibleBy_FALSE = 1;
static int pDivisibleBy_ShortFalse = 1;
static int pDivisibleBy_Null = 1;
BOOLEAN pDebugShortDivisibleBy(poly p1, unsigned long sev_1, ring r_1,
                               poly p2, unsigned long not_sev_2, ring r_2)
{
  if (sev_1 != 0 && pGetShortExpVector(p1, r_1) != sev_1)
  {
    dReportError("sev1 is %o but should be %o", sev_1,
          pGetShortExpVector(p1, r_1));
  }
  if (~ pGetShortExpVector(p2, r_2) != not_sev_2)
  {
    dReportError("not_sev2 is %o but should be %o", not_sev_2,
          ~ pGetShortExpVector(p2, r_2));
  }
  if (sev_1 == 0) pDivisibleBy_Null++;
  pDivisibleBy_number++;
  BOOLEAN ret = _p_DivisibleBy1(p1, r1, p2, r2);
  if (! ret) pDivisibleBy_FALSE++;
  if (sev_1 & not_sev_2)
  {
    pDivisibleBy_ShortFalse++;
    if (ret)
    {
      dReportError("p1 divides p2, but sev's are wrong");
      assume(0);
    }
  }
  return ret;
}

void pPrintDivisbleByStat()
{
  Print("#Tests: %d; #FALSE %d(%d); #SHORT %d(%d) #NULL:%d(%d)\n",
        pDivisibleBy_number,
        pDivisibleBy_FALSE, pDivisibleBy_FALSE*100/pDivisibleBy_number,
        pDivisibleBy_ShortFalse, pDivisibleBy_ShortFalse*100/pDivisibleBy_FALSE,
        pDivisibleBy_Null, pDivisibleBy_Null*100/pDivisibleBy_number);

}
#endif

int rComp0_Func(poly p1,poly p2)
{
  int i;
  for(i=0; i<=currRing->pCompHighIndex;i++)
  {
    if (p1->exp[i] != p2->exp[i])
    {
      if (p1->exp[i] > p2->exp[i])
        return currRing->ordsgn[i];
      else
        return -currRing->ordsgn[i];
    }
  }
  return 0;
}

#ifdef PDEBUG
int rComp0(poly p1,poly p2)
{
  int i;
  for(i=0; i<=currRing->pCompHighIndex;i++)
  {
    if (p1->exp[i] != p2->exp[i])
    {
      if (p1->exp[i] > p2->exp[i])
        return currRing->ordsgn[i];
      else
        return -currRing->ordsgn[i];
    }
  }
  return 0;
}
#endif

#endif // POLYS_IMPL_CC
