/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: prCopy.cc,v 1.8 2000-09-25 12:26:36 obachman Exp $ */
/*
* ABSTRACT - implementation of functions for Copy/Move/Delete for Polys
*/

#include "mod2.h"
#include "omalloc.h"
#include "polys.h"
#include "numbers.h"
#include "tok.h"
#include "ring.h"
#include "ideals.h"


/////////////////////////////////////////////////////////////////////////
// prSort prMerge

poly prMergeR(poly p1, poly p2, ring r)
{
  if (p1 == NULL) return p2;
  if (p2 == NULL) return p1;
  
  spolyrec rp;
  poly p = &rp;
  p_Test(p1, r);
  p_Test(p2, r);
  
  Top:
  p_LmCmpAction(p1, p2, r, goto Equal, goto Greater, goto Smaller);
  
  Equal:
  // should never get here
  assume(0);

  Greater:
  pNext(p) = p1;
  if (pNext(p1) != NULL)
  {
    pIter(p);
    pIter(p1);
    goto Top;
  }
  pNext(p1) = p2;
  goto Finish;
  
  Smaller:
  pNext(p) = p2;
  if (pNext(p2) != NULL)
  {
    pIter(p);
    pIter(p2);
    goto Top;
  }
  pNext(p2) = p1;
  goto Finish;
  
  Finish:
  p_Test(pNext(&rp), r);
  return pNext(&rp);
}
  
// Sorts poly, reverts it first -- this way, almost sorted polys's
// don't have n^2 sorting property
poly prSortR(poly p, ring r, BOOLEAN revert)
{
  if (p == NULL) return NULL;
  poly qq, result = NULL;
  int *lp = NULL, lq = 0;
  
  if (revert)
  {
    // there must be a bug here: consider m1->m2
    while (p != NULL)
    {
      qq = p;
      pIter(p);
      qq->next = result;
      result = qq;
    }
    p = result;
  }
  
  result = p;
  pIter(p);
  pNext(result) = NULL;
  while (p != NULL)
  {
    qq = p;
    pIter(p);
    qq->next = NULL;
    result = prMergeR(result, qq, r);
  }
  p_Test(result, r);
  return result;
}

static inline void 
prCopyEvector(poly dest, ring dest_r, poly src, ring src_r,int max)
{
  assume(dest_r == currRing);
  number n = pGetCoeff(dest);
  int i=0;
  for (i=max; i; i--)
  {
    p_SetExp(dest, i, p_GetExp( src, i,src_r), dest_r);
    assume(n == pGetCoeff(dest));
  }
  p_SetComp(dest, p_GetComp( src,src_r), dest_r);
    assume(n == pGetCoeff(dest));
  p_Setm(dest, dest_r);
  assume(n == pGetCoeff(dest));
}

#include "prCopy.inc"

/////////////////////////////////////////////////////////////////////////
// prCopy
poly prCopy(poly p)
{
  poly res;
  
  if (rField_has_simple_Alloc(currRing))
    res = pr_Copy_REqual_NSimple_NoSort(p, currRing, currRing);
  else
    res = pr_Copy_REqual_NoNSimple_NoSort(p, currRing, currRing);
  pTest(res);
  return res;
}

poly prCopyR(poly p, ring src_r)
{
  poly res;
  if (rField_has_simple_Alloc(currRing))
    res =  pr_Copy_NoREqual_NSimple_Sort(p, src_r, currRing);
  else
    res =  pr_Copy_NoREqual_NoNSimple_Sort(p, src_r, currRing);
  pTest(res);
  return res;
}

poly prCopyR_NoSort(poly p, ring src_r)
{
  poly res;
  if (rField_has_simple_Alloc(currRing))
    res =  pr_Copy_NoREqual_NSimple_NoSort(p, src_r, currRing);
  else
    res =  pr_Copy_NoREqual_NoNSimple_NoSort(p, src_r, currRing);
  pTest(res);
  return res;
}

/////////////////////////////////////////////////////////////////////////
// prMove
poly prMoveR(poly &p, ring src_r)
{
  poly res;
  if (rField_has_simple_Alloc(currRing))
    res =  pr_Move_NoREqual_NSimple_Sort(p, src_r, currRing);
  else
    res =  pr_Move_NoREqual_NoNSimple_Sort(p, src_r, currRing);
  pTest(res);
  return res;
}

poly prMoveR_NoSort(poly &p, ring src_r)
{
  poly res;
  if (rField_has_simple_Alloc(currRing))
    res =  pr_Move_NoREqual_NSimple_NoSort(p, src_r, currRing);
  else
    res =  pr_Move_NoREqual_NoNSimple_NoSort(p, src_r, currRing);
  pTest(res);  
  return res;
}
  
/////////////////////////////////////////////////////////////////////////
// prHead
typedef poly (*prCopyProc_t)(poly &src_p, ring src_r, ring dest_r);

poly prHeadR(poly p, ring src_r, prCopyProc_t prproc)
{
  if (p == NULL) return NULL;
  poly tail, head, q = p;
  tail = pNext(p);
  pNext(p) = NULL;
  head = prproc(q, src_r, currRing);
  pNext(p) = tail;
  return head;
}

poly prHeadR(poly p, ring src_r)
{
  prCopyProc_t prproc;
  if (rField_has_simple_Alloc(currRing)) 
    prproc = pr_Copy_NoREqual_NSimple_NoSort;
  else
    prproc = pr_Copy_NoREqual_NoNSimple_NoSort;  
  
  return prHeadR(p, src_r, prproc);
}

/////////////////////////////////////////////////////////////////////////
// idrCopy
static inline ideal 
idrCopy(ideal id, ring src_r, ring dest_r, prCopyProc_t prproc)
{
  if (id == NULL) return NULL;
  poly p;
  ideal res = idInit(IDELEMS(id), id->rank);
  int i;
  
  for (i=IDELEMS(id)-1; i>=0; i--)
  {
    p = id->m[i];
    res->m[i] = prproc(p, src_r, dest_r);
    pTest(res->m[i]);
  }
  return res;
}

ideal idrCopy(ideal id)
{
  ideal res;
  prCopyProc_t prproc;
  if (rField_has_simple_Alloc(currRing)) 
    prproc = pr_Copy_REqual_NSimple_NoSort;
  else
    prproc = pr_Copy_REqual_NoNSimple_NoSort;
  res =  idrCopy(id, currRing, currRing, prproc);
  return res;
}

ideal idrCopyR(ideal id, ring src_r)
{
  ideal res;
  prCopyProc_t prproc;
  if (rField_has_simple_Alloc(currRing)) 
    prproc = pr_Copy_NoREqual_NSimple_Sort;
  else
    prproc = pr_Copy_NoREqual_NoNSimple_Sort;
  res =  idrCopy(id, src_r, currRing, prproc);
  return res;
}
  
ideal idrCopyR_NoSort(ideal id, ring src_r)
{
  ideal res;
  prCopyProc_t prproc;
  if (rField_has_simple_Alloc(currRing)) 
    prproc = pr_Copy_NoREqual_NSimple_NoSort;
  else
    prproc = pr_Copy_NoREqual_NoNSimple_NoSort;
  res =  idrCopy(id, src_r, currRing, prproc);
  return res;
}
  
/////////////////////////////////////////////////////////////////////////
// idrMove
static inline ideal 
idrMove(ideal &id, ring src_r, ring dest_r, prCopyProc_t prproc)
{
  if (id == NULL) return NULL;
  ideal res = id;
  
  int i;
  for (i=IDELEMS(id)-1; i>=0; i--)
    res->m[i] = prproc(id->m[i], src_r, dest_r);
  id = NULL;
  return res;
}

ideal idrMoveR(ideal &id, ring src_r)
{
  prCopyProc_t prproc;
  ideal res;
  
  if (rField_has_simple_Alloc(currRing)) 
    prproc = pr_Move_NoREqual_NSimple_Sort;
  else
    prproc = pr_Move_NoREqual_NoNSimple_Sort;
  res =  idrMove(id, src_r, currRing, prproc);
  return res;
}
  
ideal idrMoveR_NoSort(ideal &id, ring src_r)
{
  prCopyProc_t prproc;
  ideal res;
  
  if (rField_has_simple_Alloc(currRing)) 
    prproc = pr_Move_NoREqual_NSimple_NoSort;
  else
    prproc = pr_Move_NoREqual_NoNSimple_NoSort;
  res =  idrMove(id, src_r, currRing, prproc);
  return res;
}

/////////////////////////////////////////////////////////////////////////
// prDelete
typedef void (*prDeleteProc_t)(poly &src_p, ring src_r);

static inline void prDeleteR_NSimple(poly &p, ring r)
{
  poly next;
  
  while (p != NULL)
  {
    next = pNext(p);
    omFreeBin(p, r->PolyBin);
    p = next;
  }
}

static inline void prDeleteR_NoNSimple(poly &p, ring r)
{
  poly next;
  
  while (p != NULL)
  {
    next = pNext(p);
    nDelete(&pGetCoeff(p));
    omFreeBin(p, r->PolyBin);
    p = next;
  }
}

void prDelete(poly &p)
{
  if (rField_has_simple_Alloc(currRing))
    prDeleteR_NSimple(p, currRing);
  else
    prDeleteR_NoNSimple(p, currRing);
}

void prDeleteR(poly &p, ring r)
{
  if (rField_has_simple_Alloc(r))
    prDeleteR_NSimple(p, r);
  else
    prDeleteR_NoNSimple(p, r);
}


static inline void idrDelete(ideal &id, ring r,  prDeleteProc_t prproc)
{
  if (id == NULL) return;
  int i = IDELEMS(id);

  for (; i>=0; i--)
    prproc(id->m[i], r);
  
  omFreeSize(id->m, IDELEMS(id)*sizeof(poly));
  omFreeBin(id, sip_sideal_bin);
  id = NULL;
}

void idrDelete(ideal &id)
{
  prDeleteProc_t prproc;
  if (rField_has_simple_Alloc(currRing))
    prproc = prDeleteR_NSimple;
  else
    prproc = prDeleteR_NoNSimple;
  
  idrDelete(id, currRing, prproc);
}

void idrDeleteR(ideal &id, ring r)
{
  prDeleteProc_t prproc;
  if (rField_has_simple_Alloc(r))
    prproc = prDeleteR_NSimple;
  else
    prproc = prDeleteR_NoNSimple;
  
  idrDelete(id, r, prproc);
}

  
  
  
