/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - implementation of functions for Copy/Move/Delete for Polys
*/


#include "misc/auxiliary.h"

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"
#include "coeffs/numbers.h"
#include "polys/monomials/ring.h"
#include "polys/simpleideals.h"

static inline void
prCopyEvector(poly dest, ring dest_r, poly src, ring src_r,int max)
{
  //memset(dest->exp,0,dest_r->ExpL_Size*sizeof(long));
  int i;
  for (i=max; i>0; i--)
  {
    p_SetExp(dest, i, p_GetExp( src, i,src_r), dest_r);
  }
  if (rRing_has_Comp(dest_r) && rRing_has_Comp(src_r))
    p_SetComp(dest, p_GetComp( src,src_r), dest_r);
  p_Setm(dest, dest_r);
}

#include "polys/prCopy.inc"

/////////////////////////////////////////////////////////////////////////
poly prCopyR(poly p, ring src_r, ring dest_r)
{
  poly res;
  if (rField_has_simple_Alloc(dest_r))
    res =  pr_Copy_NoREqual_NSimple_Sort(p, src_r, dest_r);
  else
    res =  pr_Copy_NoREqual_NoNSimple_Sort(p, src_r, dest_r);
  p_Test(res, dest_r);
  return res;
}

poly prMapR(poly src, nMapFunc nMap, ring src_r, ring dest_r)
{
  if (src==NULL) return NULL;
  int _min = si_min(dest_r->N, src_r->N);

  spolyrec dest_s;

  poly dest = &dest_s;

  poly prev;
  while (src != NULL)
  {
    pNext(dest) = (poly) p_Init(dest_r);
    prev = dest;
    pIter(dest);

    pSetCoeff0(dest, nMap(pGetCoeff(src),src_r->cf,dest_r->cf));
    prCopyEvector(dest, dest_r, src, src_r, _min);
    if (n_IsZero(pGetCoeff(dest),dest_r->cf))
    {
      p_LmDelete(&pNext(prev),dest_r);
      dest=prev;
    }
    pIter(src);
  }
  pNext(dest) = NULL;
  dest = pNext(&dest_s);
  dest=p_SortAdd(dest, dest_r);
  p_Test(dest, dest_r);
  return dest;
}

poly prCopyR_NoSort(poly p, ring src_r, ring dest_r)
{
  poly res;
  if (rField_has_simple_Alloc(dest_r))
    res =  pr_Copy_NoREqual_NSimple_NoSort(p, src_r, dest_r);
  else
    res =  pr_Copy_NoREqual_NoNSimple_NoSort(p, src_r, dest_r);
  p_Test(res, dest_r);
  return res;
}

/////////////////////////////////////////////////////////////////////////
// prMove
poly prMoveR(poly &p, ring src_r, ring dest_r)
{
  poly res;
  if (rField_has_simple_Alloc(dest_r))
    res =  pr_Move_NoREqual_NSimple_Sort(p, src_r, dest_r);
  else
    res =  pr_Move_NoREqual_NoNSimple_Sort(p, src_r, dest_r);
  p_Test(res, dest_r);
  return res;
}

poly prMoveR_NoSort(poly &p, ring src_r, ring dest_r)
{
  poly res;
  if (rField_has_simple_Alloc(dest_r))
    res =  pr_Move_NoREqual_NSimple_NoSort(p, src_r, dest_r);
  else
    res =  pr_Move_NoREqual_NoNSimple_NoSort(p, src_r, dest_r);
  p_Test(res, dest_r);
  return res;
}

poly prShallowCopyR_NoSort(poly p, ring r, ring dest_r)
{
  return pr_Copy_NoREqual_NSimple_NoSort(p, r, dest_r);
}

poly prShallowCopyR(poly p, ring r, ring dest_r)
{
  return pr_Copy_NoREqual_NSimple_Sort(p, r, dest_r);
}

/////////////////////////////////////////////////////////////////////////
// prHead
typedef poly (*prCopyProc_t)(poly &src_p, ring src_r, ring dest_r);

poly prHeadR(poly p, ring src_r, ring dest_r, prCopyProc_t prproc)
{
  if (p == NULL) return NULL;
  poly tail, head, q = p;
  tail = pNext(p);
  pNext(p) = NULL;
  head = prproc(q, src_r, dest_r);
  pNext(p) = tail;

  p_Test(p, src_r);
  p_Test(head, dest_r);

  return head;
}

poly prHeadR(poly p, ring src_r, ring dest_r)
{
  prCopyProc_t prproc;
  if (rField_has_simple_Alloc(dest_r))
    prproc = pr_Copy_NoREqual_NSimple_NoSort;
  else
    prproc = pr_Copy_NoREqual_NoNSimple_NoSort;

  const poly res = prHeadR(p, src_r, dest_r, prproc);
  p_Test(res, dest_r);
  return res;
}

/////////////////////////////////////////////////////////////////////////
/// Copy leading terms of id[i] via prHeeadR into dest_r
ideal idrHeadR(ideal id, ring r, ring dest_r)
{
  if (id == NULL) return NULL;

  prCopyProc_t prproc = pr_Copy_NoREqual_NoNSimple_NoSort;
  if (rField_has_simple_Alloc(dest_r))
    prproc = pr_Copy_NoREqual_NSimple_NoSort;

  const int N = IDELEMS(id);
  ideal res = idInit(N, id->rank);

  for (int i = N - 1; i >= 0; i--)
    res->m[i] = prHeadR(id->m[i], r, dest_r, prproc); // !!!

  return res;
}


static inline ideal
idrCopy(ideal id, ring src_r, ring dest_r, prCopyProc_t prproc)
{
  if (id == NULL) return NULL;
  assume(src_r->cf==dest_r->cf);
  poly p;
  ideal res = idInit(IDELEMS(id), id->rank);
  int i;

  for (i=IDELEMS(id)-1; i>=0; i--)
  {
    p = id->m[i];
    res->m[i] = prproc(p, src_r, dest_r);
    p_Test(res->m[i], dest_r);
  }
  return res;
}

ideal idrCopyR(ideal id, ring src_r, ring dest_r)
{
  assume(src_r->cf==dest_r->cf);
  ideal res;
  prCopyProc_t prproc;
  if (rField_has_simple_Alloc(dest_r))
    prproc = pr_Copy_NoREqual_NSimple_Sort;
  else
    prproc = pr_Copy_NoREqual_NoNSimple_Sort;
  res =  idrCopy(id, src_r, dest_r, prproc);
  return res;
}

ideal idrCopyR_NoSort(ideal id, ring src_r, ring dest_r)
{
  assume(src_r->cf==dest_r->cf);
  ideal res;
  prCopyProc_t prproc;
  if (rField_has_simple_Alloc(dest_r))
    prproc = pr_Copy_NoREqual_NSimple_NoSort;
  else
    prproc = pr_Copy_NoREqual_NoNSimple_NoSort;
  res =  idrCopy(id, src_r, dest_r, prproc);
  return res;
}

/////////////////////////////////////////////////////////////////////////
// idrShallowCopy
ideal idrShallowCopyR(ideal id, ring src_r, ring dest_r)
{
  return idrCopy(id, src_r, dest_r, pr_Copy_NoREqual_NSimple_Sort);
}

ideal idrShallowCopyR_NoSort(ideal id, ring src_r, ring dest_r)
{
  return idrCopy(id, src_r, dest_r, pr_Copy_NoREqual_NSimple_NoSort);
}

/////////////////////////////////////////////////////////////////////////
// idrMove
static inline ideal
idrMove(ideal &id, ring src_r, ring dest_r, prCopyProc_t prproc)
{
  assume(src_r->cf==dest_r->cf);
  assume( prproc != NULL );

  if (id == NULL) return NULL;

  ideal res = id; id = NULL;

  for (int i = IDELEMS(res) - 1; i >= 0; i--)
    res->m[i] = prproc(res->m[i], src_r, dest_r);

  return res;
}

ideal idrMoveR(ideal &id, ring src_r, ring dest_r)
{
  assume(src_r->cf==dest_r->cf);
  prCopyProc_t prproc;
  ideal res;
  if (rField_has_simple_Alloc(dest_r))
    prproc = pr_Move_NoREqual_NSimple_Sort;
  else
    prproc = pr_Move_NoREqual_NoNSimple_Sort;
  res =  idrMove(id, src_r, dest_r, prproc);
  return res;
}

ideal idrMoveR_NoSort(ideal &id, ring src_r, ring dest_r)
{
  assume(src_r->cf==dest_r->cf);
  prCopyProc_t prproc;
  ideal res;
  if (rField_has_simple_Alloc(dest_r))
    prproc = pr_Move_NoREqual_NSimple_NoSort;
  else
    prproc = pr_Move_NoREqual_NoNSimple_NoSort;
  res =  idrMove(id, src_r, dest_r, prproc);
  return res;
}


