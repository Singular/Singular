/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: prCopy.cc,v 1.11 2000-12-31 15:14:43 obachman Exp $ */
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
#include "sbuckets.h"

static inline void 
prCopyEvector(poly dest, ring dest_r, poly src, ring src_r,int max)
{
  assume((dest_r == currRing)||(dest_r== currRing->algring));
  number n = pGetCoeff(dest);
  int i;
  for (i=max; i>0; i--)
  {
    p_SetExp(dest, i, p_GetExp( src, i,src_r), dest_r);
    assume(n == pGetCoeff(dest));
  }
  if (rRing_has_Comp(dest_r))
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

