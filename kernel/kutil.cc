/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kutil.cc,v 1.8 2005-04-25 18:15:24 Singular Exp $ */
/*
* ABSTRACT: kernel: utils for kStd
*/

#ifndef KUTIL_CC
#define KUTIL_CC

// #define PDEBUG 2
// #define PDIV_DEBUG
#include <stdlib.h>
#include <string.h>
#include "mod2.h"
#include <mylimits.h>
#include "structs.h"
#ifdef HAVE_PLURAL
#include "gring.h"
#endif
#ifdef KDEBUG
#undef KDEBUG
#define KDEBUG 2
#endif

// define if enterL, enterT should use memmove instead of doing it manually
// on topgun, this is slightly faster (see monodromy_l.tst, homog_gonnet.sing)
#ifndef SunOS_4
#define ENTER_USE_MEMMOVE
#endif

// define, if the my_memmove inlines should be used instead of
// system memmove -- it does not seem to pay off, though
// #define ENTER_USE_MYMEMMOVE

#include "kutil.h"
#include "febase.h"
#include "omalloc.h"
#include "numbers.h"
#include "polys.h"
#include "ring.h"
#include "ideals.h"
#include "timer.h"
//#include "cntrlc.h"
#include "stairc.h"
#include "kstd1.h"
#include "pShallowCopyDelete.h"

#ifdef KDEBUG
#undef KDEBUG
#define KDEBUG 2
#endif

#ifdef ENTER_USE_MYMEMMOVE
inline void _my_memmove_d_gt_s(unsigned long* d, unsigned long* s, long l)
{
  register unsigned long* _dl = (unsigned long*) d;
  register unsigned long* _sl = (unsigned long*) s;
  register long _i = l - 1;

  do
  {
    _dl[_i] = _sl[_i];
    _i--;
  }
  while (_i >= 0);
}

inline void _my_memmove_d_lt_s(unsigned long* d, unsigned long* s, long l)
{
  register long _ll = l;
  register unsigned long* _dl = (unsigned long*) d;
  register unsigned long* _sl = (unsigned long*) s;
  register long _i = 0;

  do
  {
    _dl[_i] = _sl[_i];
    _i++;
  }
  while (_i < _ll);
}

inline void _my_memmove(void* d, void* s, long l)
{
  unsigned long _d = (unsigned long) d;
  unsigned long _s = (unsigned long) s;
  unsigned long _l = ((l) + SIZEOF_LONG - 1) >> LOG_SIZEOF_LONG;

  if (_d > _s) _my_memmove_d_gt_s(_d, _s, _l);
  else _my_memmove_d_lt_s(_d, _s, _l);
}

#undef memmove
#define memmove(d,s,l) _my_memmove(d, s, l)
#endif


static poly redMora (poly h,int maxIndex,kStrategy strat);
static poly redBba (poly h,int maxIndex,kStrategy strat);

static inline int pDivComp(poly p, poly q)
{
  if (pGetComp(p) == pGetComp(q))
  {
    BOOLEAN a=FALSE, b=FALSE;
    int i;
    unsigned long la, lb;
    unsigned long divmask = currRing->divmask;
    for (i=0; i<currRing->VarL_Size; i++)
    {
      la = p->exp[currRing->VarL_Offset[i]];
      lb = q->exp[currRing->VarL_Offset[i]];
      if (la != lb)
      {
        if (la < lb)
        {
          if (b) return 0;
          if (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask))
            return 0;
          a = TRUE;
        }
        else
        {
          if (a) return 0;
          if (((la & divmask) ^ (lb & divmask)) != ((la - lb) & divmask))
            return 0;
          b = TRUE;
        }
      }
    }
    if (a) return 1;
    if (b) return -1;
  }
  return 0;
}


BITSET  test=(BITSET)0;
int     HCord;
int     Kstd1_deg;
int     mu=32000;

/*2
*deletes higher monomial of p, re-compute ecart and length
*works only for orderings with ecart =pFDeg(end)-pFDeg(start)
*/
void deleteHC(LObject *L, kStrategy strat, BOOLEAN fromNext)
{
  if (strat->kHEdgeFound)
  {
    kTest_L(L);
    poly p1;
    poly p = L->GetLmTailRing();
    int l = 1;
    kBucket_pt bucket = NULL;
    if (L->bucket != NULL)
    {
      kBucketClear(L->bucket, &pNext(p), &L->pLength);
      L->pLength++;
      bucket = L->bucket;
      L->bucket = NULL;
      L->last = NULL;
    }

    if (!fromNext && p_Cmp(p,strat->kNoetherTail(), L->tailRing) == -1)
    {
      L->Delete();
      L->Clear();
      L->ecart = -1;
      if (bucket != NULL) kBucketDestroy(&bucket);
      return;
    }
    p1 = p;
    while (pNext(p1)!=NULL)
    {
      if (p_LmCmp(pNext(p1), strat->kNoetherTail(), L->tailRing) == -1)
      {
        L->last = p1;
        p_Delete(&pNext(p1), L->tailRing);
        if (p1 == p)
        {
          if (L->t_p != NULL)
          {
            assume(L->p != NULL && p == L->t_p);
            pNext(L->p) = NULL;
          }
          L->max  = NULL;
        }
        else if (fromNext)
          L->max  = p_GetMaxExpP(pNext(L->p), L->tailRing ); // p1;
        if (L->pLength != 0) L->pLength = l;
        // Hmmm when called from updateT, then only
        // reset ecart when cut
        if (fromNext)
          L->ecart = L->pLDeg() - L->GetpFDeg();
        break;
      }
      l++;
      pIter(p1);
    }
    if (! fromNext)
    {
      L->SetpFDeg();
      L->ecart = L->pLDeg(strat->LDegLast) - L->GetpFDeg();
    }
    if (bucket != NULL)
    {
      if (L->pLength > 1)
      {
        kBucketInit(bucket, pNext(p), L->pLength - 1);
        pNext(p) = NULL;
        if (L->t_p != NULL) pNext(L->t_p) = NULL;
        L->pLength = 0;
        L->bucket = bucket;
        L->last = NULL;
      }
      else
        kBucketDestroy(&bucket);
    }
    kTest_L(L);
  }
}

void deleteHC(poly* p, int* e, int* l,kStrategy strat)
{
  LObject L(*p, currRing, strat->tailRing);

  deleteHC(&L, strat);
  *p = L.p;
  *e = L.ecart;
  *l = L.length;
  if (L.t_p != NULL) p_LmFree(L.t_p, strat->tailRing);
}

/*2
*tests if p.p=monomial*unit and cancels the unit
*/
void cancelunit (LObject* L)
{
  int  i;
  poly h;

  if(currRing->OrdSgn != -1) return;
  if(TEST_OPT_CANCELUNIT) return;

  ring r = L->tailRing;
  poly p = L->GetLmTailRing();

  if(p_GetComp(p, r) != 0 && !p_OneComp(p, r)) return;

  if (L->ecart != 0)
  {
//    for(i=r->N;i>0;i--)
//    {
//      if ((p_GetExp(p,i,r)>0) && (rIsPolyVar(i, r)==TRUE)) return;
//    }
    h = pNext(p);
    loop
    {
      if (h==NULL)
      {
        p_Delete(&pNext(p), r);
        number eins=nInit(1);
        if (L->p != NULL)  pSetCoeff(L->p,eins);
        else if (L->t_p != NULL) nDelete(&pGetCoeff(L->t_p));
        if (L->t_p != NULL) pSetCoeff0(L->t_p,eins);
        L->ecart = 0;
        L->length = 1;
        if (L->pLength > 0) L->pLength = 1;
        if (L->last != NULL) L->last = p;

        if (L->t_p != NULL && pNext(L->t_p) != NULL)
          pNext(L->t_p) = NULL;
        if (L->p != NULL && pNext(L->p) != NULL)
          pNext(L->p) = NULL;
        return;
      }
      i = 0;
      loop
      {
        i++;
        if (p_GetExp(p,i,r) > p_GetExp(h,i,r)) return ; // does not divide
        if (i == r->N) break; // does divide, try next monom
      }
      pIter(h);
    }
  }
}

/*2
*pp is the new element in s
*returns TRUE (in strat->kHEdgeFound) if
*-HEcke is allowed
*-we are in the last componente of the vector
*-on all axis are monomials (all elements in NotUsedAxis are FALSE)
*returns FALSE for pLexOrderings,
*assumes in module case an ordering of type c* !!
* HEckeTest is only called with strat->kHEdgeFound==FALSE !
*/
void HEckeTest (poly pp,kStrategy strat)
{
  int   j,k,p;

  strat->kHEdgeFound=FALSE;
  if (pLexOrder || currRing->MixedOrder)
  {
    return;
  }
  if (strat->ak > 1)           /*we are in the module case*/
  {
    return; // until ....
    //if (!pVectorOut)     /*pVectorOut <=> order = c,* */
    //  return FALSE;
    //if (pGetComp(pp) < strat->ak) /* ak is the number of the last component */
    //  return FALSE;
  }
  k = 0;
  p=pIsPurePower(pp);
  if (p!=0) strat->NotUsedAxis[p] = FALSE;
  /*- the leading term of pp is a power of the p-th variable -*/
  for (j=pVariables;j>0; j--)
  {
    if (strat->NotUsedAxis[j])
    {
      return;
    }
  }
  strat->kHEdgeFound=TRUE;
}

/*2
*utilities for TSet, LSet
*/
inline static intset initec (const int maxnr)
{
  return (intset)omAlloc(maxnr*sizeof(int));
}

inline static unsigned long* initsevS (const int maxnr)
{
  return (unsigned long*)omAlloc0(maxnr*sizeof(unsigned long));
}
inline static int* initS_2_R (const int maxnr)
{
  return (int*)omAlloc0(maxnr*sizeof(int));
}

static inline void enlargeT (TSet &T, TObject** &R, unsigned long* &sevT,
                             int &length, const int incr)
{
  assume(T!=NULL);
  assume(sevT!=NULL);
  assume(R!=NULL);
  assume((length+incr) > 0);

  int i;
  T = (TSet)omRealloc0Size(T, length*sizeof(TObject),
                           (length+incr)*sizeof(TObject));

  sevT = (unsigned long*) omReallocSize(sevT, length*sizeof(long*),
                           (length+incr)*sizeof(long*));

  R = (TObject**)omRealloc0Size(R,length*sizeof(TObject*),
                                (length+incr)*sizeof(TObject*));
  for (i=length-1;i>=0;i--) R[T[i].i_r] = &(T[i]);
  length += incr;
}

void cleanT (kStrategy strat)
{
  int i,j;
  poly  p;
  assume(currRing == strat->tailRing || strat->tailRing != NULL);

  pShallowCopyDeleteProc p_shallow_copy_delete =
    (strat->tailRing != currRing ?
     pGetShallowCopyDeleteProc(strat->tailRing, currRing) :
     NULL);

  for (j=0; j<=strat->tl; j++)
  {
    p = strat->T[j].p;
    strat->T[j].p=NULL;
    if (strat->T[j].max != NULL)
      p_LmFree(strat->T[j].max, strat->tailRing);
    i = -1;
    loop
    {
      i++;
      if (i>strat->sl)
      {
        if (strat->T[j].t_p != NULL)
        {
          p_Delete(&(strat->T[j].t_p), strat->tailRing);
          p_LmFree(p, currRing);
        }
        else
          pDelete(&p);
        break;
      }
      if (p == strat->S[i])
      {
        if (strat->T[j].t_p != NULL)
        {
          assume(p_shallow_copy_delete != NULL);
          pNext(p) = p_shallow_copy_delete(pNext(p),strat->tailRing,currRing,
                                           currRing->PolyBin);
          p_LmFree(strat->T[j].t_p, strat->tailRing);
        }
        break;
      }
    }
  }
  strat->tl=-1;
}

LSet initL ()
{
  int i;
  LSet l = (LSet)omAlloc(setmaxL*sizeof(LObject));
  for (i=setmaxL-1;i>=0;i--)
  {
    l[i].tailRing = currRing;
    l[i].i_r1 = -1;
    l[i].i_r2 = -1;
    l[i].i_r = -1;
  }
  return l;
}

static inline void enlargeL (LSet* L,int* length,const int incr)
{
  assume((*L)!=NULL);
  assume((length+incr)>0);

  *L = (LSet)omReallocSize((*L),(*length)*sizeof(LObject),
                                   ((*length)+incr)*sizeof(LObject));
  (*length) += incr;
}

void initPairtest(kStrategy strat)
{
  strat->pairtest = (BOOLEAN *)omAlloc0((strat->sl+2)*sizeof(BOOLEAN));
}

/*2
*test whether (p1,p2) or (p2,p1) is in L up position length
*it returns TRUE if yes and the position k
*/
BOOLEAN isInPairsetL(int length,poly p1,poly p2,int*  k,kStrategy strat)
{
  LObject *p=&(strat->L[length]);

  *k = length;
  loop
  {
    if ((*k) < 0) return FALSE;
    if (((p1 == (*p).p1) && (p2 == (*p).p2))
    ||  ((p1 == (*p).p2) && (p2 == (*p).p1)))
      return TRUE;
    (*k)--;
    p--;
  }
}

/*2
*in B all pairs have the same element p on the right
*it tests whether (q,p) is in B and returns TRUE if yes
*and the position k
*/
BOOLEAN isInPairsetB(poly q,int*  k,kStrategy strat)
{
  LObject *p=&(strat->B[strat->Bl]);

  *k = strat->Bl;
  loop
  {
    if ((*k) < 0) return FALSE;
    if (q == (*p).p1)
      return TRUE;
    (*k)--;
    p--;
  }
}

int kFindInT(poly p, TSet T, int tlength)
{
  int i;

  for (i=0; i<=tlength; i++)
  {
    if (T[i].p == p) return i;
  }
  return -1;
}

int kFindInT(poly p, kStrategy strat)
{
  int i;
  do
  {
    i = kFindInT(p, strat->T, strat->tl);
    if (i >= 0) return i;
    strat = strat->next;
  }
  while (strat != NULL);
  return -1;
}

#ifdef KDEBUG

void sTObject::wrp()
{
  if (t_p != NULL) p_wrp(t_p, tailRing);
  else if (p != NULL) p_wrp(p, currRing, tailRing);
  else ::wrp(NULL);
}

#define kFalseReturn(x) do { if (!x) return FALSE;} while (0)

// check that Lm's of a poly from T are "equal"
static const char* kTest_LmEqual(poly p, poly t_p, ring tailRing)
{
  int i;
  for (i=1; i<=tailRing->N; i++)
  {
    if (p_GetExp(p, i, currRing) != p_GetExp(t_p, i, tailRing))
      return "Lm[i] different";
  }
  if (p_GetComp(p, currRing) != p_GetComp(t_p, tailRing))
    return "Lm[0] different";
  if (pNext(p) != pNext(t_p))
    return "Lm.next different";
  if (pGetCoeff(p) != pGetCoeff(t_p))
    return "Lm.coeff different";
  return NULL;
}

static BOOLEAN sloppy_max = FALSE;
BOOLEAN kTest_T(TObject * T, ring strat_tailRing, int i, char TN)
{
  ring tailRing = T->tailRing;
  if (strat_tailRing == NULL) strat_tailRing = tailRing;
  r_assume(strat_tailRing == tailRing);

  poly p = T->p;
  ring r = currRing;

  if (T->p == NULL && T->t_p == NULL && i >= 0)
    return dReportError("%c[%d].poly is NULL", TN, i);

  if (T->tailRing != currRing)
  {
    if (T->t_p == NULL && i > 0)
      return dReportError("%c[%d].t_p is NULL", TN, i);
    pFalseReturn(p_Test(T->t_p, T->tailRing));
    if (T->p != NULL) pFalseReturn(p_LmTest(T->p, currRing));
    if (T->p != NULL && T->t_p != NULL)
    {
      const char* msg = kTest_LmEqual(T->p, T->t_p, T->tailRing);
      if (msg != NULL)
        return dReportError("%c[%d] %s", TN, i, msg);
      r = T->tailRing;
      p = T->t_p;
    }
    if (T->p == NULL)
    {
      p = T->t_p;
      r = T->tailRing;
    }
    if (T->t_p != NULL && i >= 0 && TN == 'T')
    {
      if (pNext(T->t_p) == NULL)
      {
        if (T->max != NULL)
          return dReportError("%c[%d].max is not NULL as it should be", TN, i);
      }
      else
      {
        if (T->max == NULL)
          return dReportError("%c[%d].max is NULL", TN, i);
        if (pNext(T->max) != NULL)
          return dReportError("pNext(%c[%d].max) != NULL", TN, i);

        pFalseReturn(p_CheckPolyRing(T->max, tailRing));
        omCheckBinAddrSize(T->max, (tailRing->PolyBin->sizeW)*SIZEOF_LONG);
#if KDEBUG > 0
        if (! sloppy_max)
        {
          poly test_max = p_GetMaxExpP(pNext(T->t_p), tailRing);
          p_Setm(T->max, tailRing);
          p_Setm(test_max, tailRing);
          BOOLEAN equal = p_ExpVectorEqual(T->max, test_max, tailRing);
          if (! equal)
            return dReportError("%c[%d].max out of sync", TN, i);
          p_LmFree(test_max, tailRing);
        }
#endif
      }
    }
  }
  else
  {
    if (T->max != NULL)
      return dReportError("%c[%d].max != NULL but tailRing == currRing",TN,i);
    if (T->t_p != NULL)
      return dReportError("%c[%d].t_p != NULL but tailRing == currRing",TN,i);
    if (T->p == NULL && i > 0)
      return dReportError("%c[%d].p is NULL", TN, i);
    pFalseReturn(p_Test(T->p, currRing));
  }

  if (i >= 0 && T->pLength != 0 && T->pLength != pLength(p))
  {
    return dReportError("%c[%d] pLength error: has %d, specified to have %d",
                        TN, i , pLength(p), T->pLength);
  }

  // check FDeg,  for elements in L and T
  if (i >= 0 && (TN == 'T' || TN == 'L'))
  {
    // FDeg has ir element from T of L set
    if (T->FDeg  != T->pFDeg())
      return dReportError("%c[%d] FDeg error: has %d, specified to have %d",
                          TN, i , T->pFDeg(), T->FDeg);
  }

  // check is_normalized for elements in T
  if (i >= 0 && TN == 'T')
  {
    if (T->is_normalized && ! nIsOne(pGetCoeff(p)))
      return dReportError("T[%d] is_normalized error", i);

  }
  return TRUE;
}

BOOLEAN kTest_L(LObject *L, ring strat_tailRing,
                BOOLEAN testp, int lpos, TSet T, int tlength)
{
  if (testp)
  {
    poly pn = NULL;
    if (L->bucket != NULL)
    {
      kFalseReturn(kbTest(L->bucket));
      r_assume(L->bucket->bucket_ring == L->tailRing);
      if (L->p != NULL && pNext(L->p) != NULL)
      {
        pn = pNext(L->p);
        pNext(L->p) = NULL;
      }
    }
    kFalseReturn(kTest_T(L, strat_tailRing, lpos, 'L'));
    if (pn != NULL)
      pNext(L->p) = pn;

    ring r;
    poly p;
    L->GetLm(p, r);
    if (L->sev != 0 && p_GetShortExpVector(p, r) != L->sev)
    {
      return dReportError("L[%d] wrong sev: has %o, specified to have %o",
                          lpos, p_GetShortExpVector(p, r), L->sev);
    }
    if (lpos > 0 && L->last != NULL && pLast(p) != L->last)
    {
      return dReportError("L[%d] last wrong: has %p specified to have %p",
                          lpos, pLast(p), L->last);
    }
  }
  if (L->p1 == NULL)
  {
    // L->p2 either NULL or "normal" poly
    pFalseReturn(pp_Test(L->p2, currRing, L->tailRing));
  }
  else if (tlength > 0 && T != NULL && (lpos >=0))
  {
    // now p1 and p2 must be != NULL and must be contained in T
    int i;
    i = kFindInT(L->p1, T, tlength);
    if (i < 0)
      return dReportError("L[%d].p1 not in T",lpos);
    i = kFindInT(L->p2, T, tlength);
    if (i < 0)
      return dReportError("L[%d].p2 not in T",lpos);
  }
  return TRUE;
}

BOOLEAN kTest (kStrategy strat)
{
  int i;

  // test P
  kFalseReturn(kTest_L(&(strat->P), strat->tailRing,
                       (strat->P.p != NULL && pNext(strat->P.p)!=strat->tail),
                       -1, strat->T, strat->tl));

  // test T
  if (strat->T != NULL)
  {
    for (i=0; i<=strat->tl; i++)
    {
      kFalseReturn(kTest_T(&(strat->T[i]), strat->tailRing, i, 'T'));
      if (strat->sevT[i] != pGetShortExpVector(strat->T[i].p))
        return dReportError("strat->sevT[%d] out of sync", i);
    }
  }

  // test L
  if (strat->L != NULL)
  {
    for (i=0; i<=strat->Ll; i++)
    {
      kFalseReturn(kTest_L(&(strat->L[i]), strat->tailRing,
                           strat->L[i].Next() != strat->tail, i,
                           strat->T, strat->tl));
      if (strat->use_buckets && strat->L[i].Next() != strat->tail &&
          strat->L[i].Next() != NULL && strat->L[i].p1 != NULL)
      {
        assume(strat->L[i].bucket != NULL);
      }
    }
  }

  // test S
  if (strat->S != NULL)
    kFalseReturn(kTest_S(strat));

  return TRUE;
}

BOOLEAN kTest_S(kStrategy strat)
{
  int i;
  BOOLEAN ret = TRUE;
  for (i=0; i<=strat->sl; i++)
  {
    if (strat->S[i] != NULL &&
        strat->sevS[i] != pGetShortExpVector(strat->S[i]))
    {
      return dReportError("S[%d] wrong sev: has %o, specified to have %o",
                          i , pGetShortExpVector(strat->S[i]), strat->sevS[i]);
    }
  }
  return ret;
}



BOOLEAN kTest_TS(kStrategy strat)
{
  int i, j;
  BOOLEAN ret = TRUE;
  kFalseReturn(kTest(strat));

  // test strat->R, strat->T[i].i_r
  for (i=0; i<=strat->tl; i++)
  {
    if (strat->T[i].i_r < 0 || strat->T[i].i_r > strat->tl)
      return dReportError("strat->T[%d].i_r == %d out of bounds", i,
                          strat->T[i].i_r);
    if (strat->R[strat->T[i].i_r] != &(strat->T[i]))
      return dReportError("T[%d].i_r with R out of sync", i);
  }
  // test containment of S inT
  if (strat->S != NULL)
  {
    for (i=0; i<=strat->sl; i++)
    {
      j = kFindInT(strat->S[i], strat->T, strat->tl);
      if (j < 0)
        return dReportError("S[%d] not in T", i);
      if (strat->S_2_R[i] != strat->T[j].i_r)
        return dReportError("S_2_R[%d]=%d != T[%d].i_r=%d\n",
                            i, strat->S_2_R[i], j, strat->T[j].i_r);
    }
  }
  // test strat->L[i].i_r1
  for (i=0; i<=strat->Ll; i++)
  {
    if (strat->L[i].p1 != NULL && strat->L[i].p2)
    {
      if (strat->L[i].i_r1 < 0 ||
          strat->L[i].i_r1 > strat->tl ||
          strat->L[i].T_1(strat)->p != strat->L[i].p1)
        return dReportError("L[%d].i_r1 out of sync", i);
      if (strat->L[i].i_r2 < 0 ||
          strat->L[i].i_r2 > strat->tl ||
          strat->L[i].T_2(strat)->p != strat->L[i].p2);
    }
    else
    {
      if (strat->L[i].i_r1 != -1)
        return dReportError("L[%d].i_r1 out of sync", i);
      if (strat->L[i].i_r2 != -1)
        return dReportError("L[%d].i_r2 out of sync", i);
    }
    if (strat->L[i].i_r != -1)
      return dReportError("L[%d].i_r out of sync", i);
  }
  return TRUE;
}

#endif // KDEBUG

/*2
*cancels the i-th polynomial in the standardbase s
*/
void deleteInS (int i,kStrategy strat)
{
#ifdef ENTER_USE_MEMMOVE
  memmove(&(strat->S[i]), &(strat->S[i+1]), (strat->sl - i)*sizeof(poly));
  memmove(&(strat->ecartS[i]),&(strat->ecartS[i+1]),(strat->sl - i)*sizeof(int));
  memmove(&(strat->sevS[i]),&(strat->sevS[i+1]),(strat->sl - i)*sizeof(long));
  memmove(&(strat->S_2_R[i]),&(strat->S_2_R[i+1]),(strat->sl - i)*sizeof(int));
#else
  int j;
  for (j=i; j<strat->sl; j++)
  {
    strat->S[j] = strat->S[j+1];
    strat->ecartS[j] = strat->ecartS[j+1];
    strat->sevS[j] = strat->sevS[j+1];
    strat->S_2_R[j] = strat->S_2_R[j+1];
  }
#endif
  if (strat->lenS!=NULL)
  {
#ifdef ENTER_USE_MEMMOVE
    memmove(&(strat->lenS[i]),&(strat->lenS[i+1]),(strat->sl - i)*sizeof(int));
#else
    for (j=i; j<strat->sl; j++) strat->lenS[j] = strat->lenS[j+1];
#endif
  }
  if (strat->lenSw!=NULL)
  {
#ifdef ENTER_USE_MEMMOVE
    memmove(&(strat->lenSw[i]),&(strat->lenSw[i+1]),(strat->sl - i)*sizeof(int));
#else
    for (j=i; j<strat->sl; j++) strat->lenSw[j] = strat->lenSw[j+1];
#endif
  }
  if (strat->fromQ!=NULL)
  {
#ifdef ENTER_USE_MEMMOVE
    memmove(&(strat->fromQ[i]),&(strat->fromQ[i+1]),(strat->sl - i)*sizeof(int));
#else
    for (j=i; j<strat->sl; j++)
    {
      strat->fromQ[j] = strat->fromQ[j+1];
    }
#endif
  }
  strat->S[strat->sl] = NULL;
  strat->sl--;
}

/*2
*cancels the j-th polynomial in the set
*/
void deleteInL (LSet set, int *length, int j,kStrategy strat)
{
  if (set[j].lcm!=NULL)
    pLmFree(set[j].lcm);
  if (set[j].p!=NULL)
  {
    if (pNext(set[j].p) == strat->tail)
    {
      pLmFree(set[j].p);
      /*- tail belongs to several int spolys -*/
    }
    else
    {
      // search p in T, if it is there, do not delete it
      if (pOrdSgn != -1 || kFindInT(set[j].p, strat) < 0)
      {
        // assure that for global ordereings kFindInT fails
        assume(pOrdSgn == -1 || kFindInT(set[j].p, strat) < 0);
        set[j].Delete();
      }
    }
  }
  if (*length > 0 && j < *length)
  {
#ifdef ENTER_USE_MEMMOVE
    memmove(&(set[j]), &(set[j+1]), (*length - j)*sizeof(LObject));
#else
    int i;
    for (i=j; i < (*length); i++)
      set[i] = set[i+1];
#endif
  }
#ifdef KDEBUG
  memset(&(set[*length]),0,sizeof(LObject));
#endif
  (*length)--;
}

/*2
*is used after updating the pairset,if the leading term of p
*divides the leading term of some S[i] it will be canceled
*/
static inline void clearS (poly p, unsigned long p_sev, int* at, int* k,
                    kStrategy strat)
{
  assume(p_sev == pGetShortExpVector(p));
  if (!pLmShortDivisibleBy(p,p_sev, strat->S[*at], ~ strat->sevS[*at])) return;
  deleteInS((*at),strat);
  (*at)--;
  (*k)--;
}

/*2
*enters p at position at in L
*/
#ifdef PDEBUG
/* int zaehler=0; */
/* for counting number of pairs in Plural */
#endif PDEBUG

void enterL (LSet *set,int *length, int *LSetmax, LObject p,int at)
{
#ifdef PDEBUG
  /*  zaehler++; */
#endif PDEBUG
  int i;
  // this should be corrected
  assume(p.FDeg == p.pFDeg());
  if ((*length)>=0)
  {
    if ((*length) == (*LSetmax)-1) enlargeL(set,LSetmax,setmaxLinc);
    if (at <= (*length))
#ifdef ENTER_USE_MEMMOVE
      memmove(&((*set)[at+1]), &((*set)[at]), ((*length)-at+1)*sizeof(LObject));
#else
    for (i=(*length)+1; i>=at+1; i--) (*set)[i] = (*set)[i-1];
#endif
  }
  else at = 0;
  (*set)[at] = p;
  (*length)++;
}

/*2
* computes the normal ecart;
* used in mora case and if pLexOrder & sugar in bba case
*/
void initEcartNormal (LObject* h)
{
  h->FDeg = h->pFDeg();
  h->ecart = h->pLDeg() - h->FDeg;
}

void initEcartBBA (LObject* h)
{
  h->FDeg = h->pFDeg();
  (*h).ecart = 0;
  (*h).length = 0;
}

void initEcartPairBba (LObject* Lp,poly f,poly g,int ecartF,int ecartG)
{
  Lp->FDeg = Lp->pFDeg();
  (*Lp).ecart = 0;
  (*Lp).length = 0;
}

void initEcartPairMora (LObject* Lp,poly f,poly g,int ecartF,int ecartG)
{
  Lp->FDeg = Lp->pFDeg();
  (*Lp).ecart = si_max(ecartF,ecartG);
  (*Lp).ecart = (*Lp).ecart- (Lp->FDeg -pFDeg((*Lp).lcm,currRing));
  (*Lp).length = 0;
}

/*2
*if ecart1<=ecart2 it returns TRUE
*/
BOOLEAN sugarDivisibleBy(int ecart1, int ecart2)
{
  return (ecart1 <= ecart2);
}

/*2
* put the pair (s[i],p)  into the set B, ecart=ecart(p)
*/

void enterOnePair (int i,poly p,int ecart, int isFromQ,kStrategy strat, int atR = -1)
{
  assume(i<=strat->sl);
  int      l,j,compare;
  LObject  Lp;

  if (strat->interred_flag) return;
#ifdef KDEBUG
  Lp.ecart=0; Lp.length=0;
#endif
  /*- computes the lcm(s[i],p) -*/
  Lp.lcm = pInit();

  pLcm(p,strat->S[i],Lp.lcm);
  pSetm(Lp.lcm);
  if (strat->sugarCrit)
  {
    if(
    (!((strat->ecartS[i]>0)&&(ecart>0)))
    && pHasNotCF(p,strat->S[i]))
    {
    /*
    *the product criterion has applied for (s,p),
    *i.e. lcm(s,p)=product of the leading terms of s and p.
    *Suppose (s,r) is in L and the leading term
    *of p divides lcm(s,r)
    *(==> the leading term of p divides the leading term of r)
    *but the leading term of s does not divide the leading term of r
    *(notice that tis condition is automatically satisfied if r is still
    *in S), then (s,r) can be cancelled.
    *This should be done here because the
    *case lcm(s,r)=lcm(s,p) is not covered by chainCrit.
    *
    *Moreover, skipping (s,r) holds also for the noncommutative case.
    */
      strat->cp++;
      pLmFree(Lp.lcm);
      Lp.lcm=NULL;
      return;
    }
    else
      Lp.ecart = si_max(ecart,strat->ecartS[i]);
    if (strat->fromT && (strat->ecartS[i]>ecart))
    {
      pLmFree(Lp.lcm);
      Lp.lcm=NULL;
      return;
      /*the pair is (s[i],t[.]), discard it if the ecart is too big*/
    }
    /*
    *the set B collects the pairs of type (S[j],p)
    *suppose (r,p) is in B and (s,p) is the new pair and lcm(s,p)#lcm(r,p)
    *if the leading term of s devides lcm(r,p) then (r,p) will be canceled
    *if the leading term of r devides lcm(s,p) then (s,p) will not enter B
    */
    {
      j = strat->Bl;
      loop
      {
        if (j < 0)  break;
        compare=pDivComp(strat->B[j].lcm,Lp.lcm);
        if ((compare==1)
        &&(sugarDivisibleBy(strat->B[j].ecart,Lp.ecart)))
        {
          strat->c3++;
          if ((strat->fromQ==NULL) || (isFromQ==0) || (strat->fromQ[i]==0))
          {
            pLmFree(Lp.lcm);
            return;
          }
          break;
        }
        else
        if ((compare ==-1)
        && sugarDivisibleBy(Lp.ecart,strat->B[j].ecart))
        {
          deleteInL(strat->B,&strat->Bl,j,strat);
          strat->c3++;
        }
        j--;
      }
    }
  }
  else /*sugarcrit*/
  {
#ifdef HAVE_PLURAL
    if (!rIsPluralRing(currRing))
    {
    // if currRing->nc_type!=quasi (or skew)
#endif

    if(/*(strat->ak==0) && productCrit(p,strat->S[i])*/
    pHasNotCF(p,strat->S[i]))
    {
    /*
    *the product criterion has applied for (s,p),
    *i.e. lcm(s,p)=product of the leading terms of s and p.
    *Suppose (s,r) is in L and the leading term
    *of p devides lcm(s,r)
    *(==> the leading term of p devides the leading term of r)
    *but the leading term of s does not devide the leading term of r
    *(notice that tis condition is automatically satisfied if r is still
    *in S), then (s,r) can be canceled.
    *This should be done here because the
    *case lcm(s,r)=lcm(s,p) is not covered by chainCrit.
    */
      if (!rIsPluralRing(currRing))
      {
        strat->cp++;
        pLmFree(Lp.lcm);
        Lp.lcm=NULL;
        return;
      }
    }
    if (strat->fromT && (strat->ecartS[i]>ecart))
    {
      pLmFree(Lp.lcm);
      Lp.lcm=NULL;
      return;
      /*the pair is (s[i],t[.]), discard it if the ecart is too big*/
    }
    /*
    *the set B collects the pairs of type (S[j],p)
    *suppose (r,p) is in B and (s,p) is the new pair and lcm(s,p)#lcm(r,p)
    *if the leading term of s devides lcm(r,p) then (r,p) will be canceled
    *if the leading term of r devides lcm(s,p) then (s,p) will not enter B
    */
    for(j = strat->Bl;j>=0;j--)
    {
      compare=pDivComp(strat->B[j].lcm,Lp.lcm);
      if (compare==1)
      {
        strat->c3++;
        if ((strat->fromQ==NULL) || (isFromQ==0) || (strat->fromQ[i]==0))
        {
          pLmFree(Lp.lcm);
          return;
        }
        break;
      }
      else
      if (compare ==-1)
      {
        deleteInL(strat->B,&strat->Bl,j,strat);
        strat->c3++;
      }
    }
  }
#ifdef HAVE_PLURAL
  }
#endif
  /*
  *the pair (S[i],p) enters B if the spoly != 0
  */
  /*-  compute the short s-polynomial -*/
  if (strat->fromT && !TEST_OPT_INTSTRATEGY)
    pNorm(p);
  if ((strat->S[i]==NULL) || (p==NULL))
    return;
  if ((strat->fromQ!=NULL) && (isFromQ!=0) && (strat->fromQ[i]!=0))
    Lp.p=NULL;
  else
  {
#ifdef HAVE_PLURAL
    if (currRing->nc!=NULL)
    {
      if ((currRing->nc->type==nc_lie) && (pHasNotCF(p,strat->S[i])))
        /* generalized prod-crit for lie-type */
      {
          strat->cp++;
          Lp.p = nc_p_Bracket_qq(pCopy(p),strat->S[i]);
      }
      else  Lp.p = nc_CreateSpoly(strat->S[i],p,NULL,currRing);
    }
    else
    {
#endif
    Lp.p = ksCreateShortSpoly(strat->S[i],p, strat->tailRing);
#ifdef HAVE_PLURAL
    }
#endif
  }
  if (Lp.p == NULL)
  {
    /*- the case that the s-poly is 0 -*/
    if (strat->pairtest==NULL) initPairtest(strat);
    strat->pairtest[i] = TRUE;/*- hint for spoly(S^[i],p)=0 -*/
    strat->pairtest[strat->sl+1] = TRUE;
    /*hint for spoly(S[i],p) == 0 for some i,0 <= i <= sl*/
    /*
    *suppose we have (s,r),(r,p),(s,p) and spoly(s,p) == 0 and (r,p) is
    *still in B (i.e. lcm(r,p) == lcm(s,p) or the leading term of s does not
    *devide lcm(r,p)). In the last case (s,r) can be canceled if the leading
    *term of p devides the lcm(s,r)
    *(this canceling should be done here because
    *the case lcm(s,p) == lcm(s,r) is not covered in chainCrit)
    *the first case is handeled in chainCrit
    */
    if (Lp.lcm!=NULL) pLmFree(Lp.lcm);
  }
  else
  {
    /*- the pair (S[i],p) enters B -*/
    Lp.p1 = strat->S[i];
    Lp.p2 = p;

#ifdef HAVE_PLURAL
    if (currRing->nc==NULL)
    {
#endif

     pNext(Lp.p) = strat->tail;

#ifdef HAVE_PLURAL
    }
#endif

    if (atR >= 0)
    {
      Lp.i_r2 = atR;
      Lp.i_r1 = strat->S_2_R[i];
    }
    strat->initEcartPair(&Lp,strat->S[i],p,strat->ecartS[i],ecart);
    if (TEST_OPT_INTSTRATEGY)
    {

#ifdef HAVE_PLURAL
      if (currRing->nc==NULL)
      {
#endif

      nDelete(&(Lp.p->coef));

#ifdef HAVE_PLURAL
      }
#endif

    }
    l = strat->posInL(strat->B,strat->Bl,&Lp,strat);
    enterL(&strat->B,&strat->Bl,&strat->Bmax,Lp,l);
  }
}

/*2
* put the pair (s[i],p) into the set L, ecart=ecart(p)
* in the case that s forms a SB of (s)
*/
void enterOnePairSpecial (int i,poly p,int ecart,kStrategy strat, int atR = -1)
{
  //PrintS("try ");wrp(strat->S[i]);PrintS(" and ");wrp(p);PrintLn();
  if(pHasNotCF(p,strat->S[i]))
  {
    //PrintS("prod-crit\n");
      if (!rIsPluralRing(currRing))
      {
        strat->cp++;
        return;
      }
  }

  int      l,j,compare;
  LObject  Lp;

  Lp.lcm = pInit();
  pLcm(p,strat->S[i],Lp.lcm);
  pSetm(Lp.lcm);
  for(j = strat->Ll;j>=0;j--)
  {
    compare=pDivComp(strat->L[j].lcm,Lp.lcm);
    if ((compare==1) || (pLmEqual(strat->L[j].lcm,Lp.lcm)))
    {
      //PrintS("c3-crit\n");
      strat->c3++;
      pLmFree(Lp.lcm);
      return;
    }
    else if (compare ==-1)
    {
      //Print("c3-crit with L[%d]\n",j);
      deleteInL(strat->L,&strat->Ll,j,strat);
      strat->c3++;
    }
  }
  /*-  compute the short s-polynomial -*/

  Lp.p = ksCreateShortSpoly(strat->S[i],p,strat->tailRing);
  if (Lp.p == NULL)
  {
     pLmFree(Lp.lcm);
  }
  else
  {
    /*- the pair (S[i],p) enters L -*/
    Lp.p1 = strat->S[i];
    Lp.p2 = p;
    if (atR >= 0)
    {
      Lp.i_r1 = strat->S_2_R[i];
      Lp.i_r2 = atR;
    }
    pNext(Lp.p) = strat->tail;
    strat->initEcartPair(&Lp,strat->S[i],p,strat->ecartS[i],ecart);
    if (TEST_OPT_INTSTRATEGY)
    {
      nDelete(&(Lp.p->coef));
    }
    l = strat->posInL(strat->L,strat->Ll,&Lp,strat);
    //Print("-> L[%d]\n",l);
    enterL(&strat->L,&strat->Ll,&strat->Lmax,Lp,l);
  }
}

/*2
*the pairset B of pairs of type (s[i],p) is complete now. It will be updated
*using the chain-criterion in B and L and enters B to L
*/
void chainCrit (poly p,int ecart,kStrategy strat)
{
  int i,j,l;

  /*
  *pairtest[i] is TRUE if spoly(S[i],p) == 0.
  *In this case all elements in B such
  *that their lcm is divisible by the leading term of S[i] can be canceled
  */
  if (strat->pairtest!=NULL)
  {
    {
      /*- i.e. there is an i with pairtest[i]==TRUE -*/
      for (j=0; j<=strat->sl; j++)
      {
        if (strat->pairtest[j])
        {
          for (i=strat->Bl; i>=0; i--)
          {
            if (pDivisibleBy(strat->S[j],strat->B[i].lcm))
            {
              deleteInL(strat->B,&strat->Bl,i,strat);
              strat->c3++;
            }
          }
        }
      }
    }
    omFreeSize(strat->pairtest,(strat->sl+2)*sizeof(BOOLEAN));
    strat->pairtest=NULL;
  }
  if (strat->Gebauer || strat->fromT)
  {
    if (strat->sugarCrit)
    {
    /*
    *suppose L[j] == (s,r) and p/lcm(s,r)
    *and lcm(s,r)#lcm(s,p) and lcm(s,r)#lcm(r,p)
    *and in case the sugar is o.k. then L[j] can be canceled
    */
      for (j=strat->Ll; j>=0; j--)
      {
        if (sugarDivisibleBy(ecart,strat->L[j].ecart)
        && ((pNext(strat->L[j].p) == strat->tail) || (pOrdSgn==1))
        && pCompareChain(p,strat->L[j].p1,strat->L[j].p2,strat->L[j].lcm))
        {
          if (strat->L[j].p == strat->tail)
          {
            deleteInL(strat->L,&strat->Ll,j,strat);
            strat->c3++;
          }
        }
      }
      /*
      *this is GEBAUER-MOELLER:
      *in B all elements with the same lcm except the "best"
      *(i.e. the last one in B with this property) will be canceled
      */
      j = strat->Bl;
      loop /*cannot be changed into a for !!! */
      {
        if (j <= 0) break;
        i = j-1;
        loop
        {
          if (i <  0) break;
          if (pLmEqual(strat->B[j].lcm,strat->B[i].lcm))
          {
            strat->c3++;
            if (sugarDivisibleBy(strat->B[j].ecart,strat->B[i].ecart))
            {
              deleteInL(strat->B,&strat->Bl,i,strat);
              j--;
            }
            else
            {
              deleteInL(strat->B,&strat->Bl,j,strat);
              break;
            }
          }
          i--;
        }
        j--;
      }
    }
    else /*sugarCrit*/
    {
      /*
      *suppose L[j] == (s,r) and p/lcm(s,r)
      *and lcm(s,r)#lcm(s,p) and lcm(s,r)#lcm(r,p)
      *and in case the sugar is o.k. then L[j] can be canceled
      */
      for (j=strat->Ll; j>=0; j--)
      {
        if (pCompareChain(p,strat->L[j].p1,strat->L[j].p2,strat->L[j].lcm))
        {
          if ((pNext(strat->L[j].p) == strat->tail)||(pOrdSgn==1))
          {
            deleteInL(strat->L,&strat->Ll,j,strat);
            strat->c3++;
          }
        }
      }
      /*
      *this is GEBAUER-MOELLER:
      *in B all elements with the same lcm except the "best"
      *(i.e. the last one in B with this property) will be canceled
      */
      j = strat->Bl;
      loop   /*cannot be changed into a for !!! */
      {
        if (j <= 0) break;
        for(i=j-1; i>=0; i--)
        {
          if (pLmEqual(strat->B[j].lcm,strat->B[i].lcm))
          {
            strat->c3++;
            deleteInL(strat->B,&strat->Bl,i,strat);
            j--;
          }
        }
        j--;
      }
    }
    /*
    *the elements of B enter L/their order with respect to B is kept
    *j = posInL(L,j,B[i]) would permutate the order
    *if once B is ordered different from L
    *then one should use j = posInL(L,Ll,B[i])
    */
    j = strat->Ll+1;
    for (i=strat->Bl; i>=0; i--)
    {
      j = strat->posInL(strat->L,j-1,&(strat->B[i]),strat);
      enterL(&strat->L,&strat->Ll,&strat->Lmax,strat->B[i],j);
    }
    strat->Bl = -1;
  }
  else
  {
    for (j=strat->Ll; j>=0; j--)
    {
      if (pCompareChain(p,strat->L[j].p1,strat->L[j].p2,strat->L[j].lcm))
      {
        if ((pNext(strat->L[j].p) == strat->tail)||(pOrdSgn==1))
        {
          deleteInL(strat->L,&strat->Ll,j,strat);
          strat->c3++;
        }
      }
    }
    /*
    *this is our MODIFICATION of GEBAUER-MOELLER:
    *First the elements of B enter L,
    *then we fix a lcm and the "best" element in L
    *(i.e the last in L with this lcm and of type (s,p))
    *and cancel all the other elements of type (r,p) with this lcm
    *except the case the element (s,r) has also the same lcm
    *and is on the worst position with respect to (s,p) and (r,p)
    */
    /*
    *B enters to L/their order with respect to B is permutated for elements
    *B[i].p with the same leading term
    */
    j = strat->Ll;
    for (i=strat->Bl; i>=0; i--)
    {
      j = strat->posInL(strat->L,j,&(strat->B[i]),strat);
      enterL(&strat->L,&strat->Ll,&strat->Lmax,strat->B[i],j);
    }
    strat->Bl = -1;
    j = strat->Ll;
    loop  /*cannot be changed into a for !!! */
    {
      if (j <= 0)
      {
        /*now L[0] cannot be canceled any more and the tail can be removed*/
        if (strat->L[0].p2 == strat->tail) strat->L[0].p2 = p;
        break;
      }
      if (strat->L[j].p2 == p)
      {
        i = j-1;
        loop
        {
          if (i < 0)  break;
          if ((strat->L[i].p2 == p) && pLmEqual(strat->L[j].lcm,strat->L[i].lcm))
          {
            /*L[i] could be canceled but we search for a better one to cancel*/
            strat->c3++;
            if (isInPairsetL(i-1,strat->L[j].p1,strat->L[i].p1,&l,strat)
            && (pNext(strat->L[l].p) == strat->tail)
            && (!pLmEqual(strat->L[i].p,strat->L[l].p))
            && pDivisibleBy(p,strat->L[l].lcm))
            {
              /*
              *"NOT equal(...)" because in case of "equal" the element L[l]
              *is "older" and has to be from theoretical point of view behind
              *L[i], but we do not want to reorder L
              */
              strat->L[i].p2 = strat->tail;
              /*
              *L[l] will be canceled, we cannot cancel L[i] later on,
              *so we mark it with "tail"
              */
              deleteInL(strat->L,&strat->Ll,l,strat);
              i--;
            }
            else
            {
              deleteInL(strat->L,&strat->Ll,i,strat);
            }
            j--;
          }
          i--;
        }
      }
      else if (strat->L[j].p2 == strat->tail)
      {
        /*now L[j] cannot be canceled any more and the tail can be removed*/
        strat->L[j].p2 = p;
      }
      j--;
    }
  }
}

/*2
*(s[0],h),...,(s[k],h) will be put to the pairset L
*/
void initenterpairs (poly h,int k,int ecart,int isFromQ,kStrategy strat, int atR = -1)
{

  if ((strat->syzComp==0)
  || (pGetComp(h)<=strat->syzComp))
  {
    int j;
    BOOLEAN new_pair=FALSE;

    if (pGetComp(h)==0)
    {
      /* for Q!=NULL: build pairs (f,q),(f1,f2), but not (q1,q2)*/
      if ((isFromQ)&&(strat->fromQ!=NULL))
      {
        for (j=0; j<=k; j++)
        {
          if (!strat->fromQ[j])
          {
            new_pair=TRUE;
            enterOnePair(j,h,ecart,isFromQ,strat, atR);
          //Print("j:%d, Ll:%d\n",j,strat->Ll);
          }
        }
      }
      else
      {
        new_pair=TRUE;
        for (j=0; j<=k; j++)
        {
          enterOnePair(j,h,ecart,isFromQ,strat, atR);
          //Print("j:%d, Ll:%d\n",j,strat->Ll);
        }
      }
    }
    else
    {
      for (j=0; j<=k; j++)
      {
        if ((pGetComp(h)==pGetComp(strat->S[j]))
        || (pGetComp(strat->S[j])==0))
        {
          new_pair=TRUE;
          enterOnePair(j,h,ecart,isFromQ,strat, atR);
        //Print("j:%d, Ll:%d\n",j,strat->Ll);
        }
      }
    }

    if (new_pair) chainCrit(h,ecart,strat);

  }
}

/*2
*(s[0],h),...,(s[k],h) will be put to the pairset L(via initenterpairs)
*superfluous elements in S will be deleted
*/
void enterpairs (poly h,int k,int ecart,int pos,kStrategy strat, int atR)
{
  int j=pos;

  initenterpairs(h,k,ecart,0,strat, atR);
  if ( (!strat->fromT)
  && ((strat->syzComp==0)
    ||(pGetComp(h)<=strat->syzComp)))
  {
    //Print("start clearS k=%d, pos=%d, sl=%d\n",k,pos,strat->sl);
    unsigned long h_sev = pGetShortExpVector(h);
    loop
    {
      if (j > k) break;
      clearS(h,h_sev, &j,&k,strat);
      j++;
    }
    //Print("end clearS sl=%d\n",strat->sl);
  }
 // PrintS("end enterpairs\n");
}

/*2
*(s[0],h),...,(s[k],h) will be put to the pairset L(via initenterpairs)
*superfluous elements in S will be deleted
*/
void enterpairsSpecial (poly h,int k,int ecart,int pos,kStrategy strat, int atR = -1)
{
  int j;

  for (j=0; j<=k; j++)
  {
    if ((pGetComp(h)==pGetComp(strat->S[j]))
    || (0==pGetComp(strat->S[j])))
    {
      enterOnePairSpecial(j,h,ecart,strat, atR);
    }
  }
  j=pos;
  loop
  {
    unsigned long h_sev = pGetShortExpVector(h);
    if (j > k) break;
    clearS(h,h_sev,&j,&k,strat);
    j++;
  }
}

/*2
*constructs the pairset at the beginning
*of the buchberger/mora algorithm
*/
void pairs (kStrategy strat)
{
  int j,i;
//  Print("pairs:sl=%d\n",strat->sl);
//  for (i=0; i<=strat->sl; i++)
//  {
//    Print("s%d:",i);pWrite(strat->S[i]);
//  }
  if (strat->fromQ!=NULL)
  {
    for (i=1; i<=strat->sl; i++)
    {
      initenterpairs(strat->S[i],i-1,strat->ecartS[i],strat->fromQ[i],strat);
    }
  }
  else
  {
    for (i=1; i<=strat->sl; i++)
    {
      initenterpairs(strat->S[i],i-1,strat->ecartS[i],0,strat);
    }
  }
  /*deletes superfluous elements in S*/
  i = -1;
  loop
  {
    i++;
    if (i >= strat->sl) break;
    if ((strat->syzComp==0) || (pGetComp(strat->S[i])<=strat->syzComp))
    {
      j=i;
      loop
      {
        j++;
        if (j > strat->sl) break;
        if (pLmShortDivisibleBy(strat->S[i], strat->sevS[i],
                              strat->S[j], ~ strat->sevS[j]))
        {
        //  Print("delete %d=",j);
        //  wrp(strat->S[j]);
        //  Print(" wegen %d=",i);
        //  wrp(strat->S[i]);
        //  Print("( fromQ=%d)\n", (strat->fromQ) ? strat->fromQ[j]:0);
          if ((strat->fromQ==NULL) || (strat->fromQ[j]==0))
          {
            deleteInS(j,strat);
            j--;
          }
        }
      }
    }
  }
}

/*2
*reorders  s with respect to posInS,
*suc is the first changed index or zero
*/
void reorderS (int* suc,kStrategy strat)
{
  int i,j,at,ecart, s2r;
  int fq=0;
  unsigned long sev;
  poly  p;

  *suc = -1;
  for (i=1; i<=strat->sl; i++)
  {
    at = posInS(strat,i-1,strat->S[i],strat->ecartS[i]);
    if (at != i)
    {
      if ((*suc > at) || (*suc == -1)) *suc = at;
      p = strat->S[i];
      ecart = strat->ecartS[i];
      sev = strat->sevS[i];
      s2r = strat->S_2_R[i];
      if (strat->fromQ!=NULL) fq=strat->fromQ[i];
      for (j=i; j>=at+1; j--)
      {
        strat->S[j] = strat->S[j-1];
        strat->ecartS[j] = strat->ecartS[j-1];
        strat->sevS[j] = strat->sevS[j-1];
        strat->S_2_R[j] = strat->S_2_R[j-1];
      }
      strat->S[at] = p;
      strat->ecartS[at] = ecart;
      strat->sevS[at] = sev;
      strat->S_2_R[at] = s2r;
      if (strat->fromQ!=NULL)
      {
        for (j=i; j>=at+1; j--)
        {
          strat->fromQ[j] = strat->fromQ[j-1];
        }
        strat->fromQ[at]=fq;
      }
    }
  }
}


/*2
*looks up the position of p in set
*set[0] is the smallest with respect to the ordering-procedure
*pComp
* Assumption: posInS only depends on the leading term
*             otherwise, bba has to be changed
*/
int posInS (kStrategy strat, int length,poly p, int ecart_p)
{
  if(length==-1) return 0;
  polyset set=strat->S;
  int i;
  int an = 0;
  int en= length;
  int cmp_int=pOrdSgn;
  if (currRing->MixedOrder)
  {
    int o=pWTotaldegree(p);
    int oo=pWTotaldegree(set[length]);

    if ((oo<o)
    || ((o==oo) && (pLmCmp(set[length],p)!= cmp_int)))
      return length+1;

    loop
    {
      if (an >= en-1)
      {
        if ((pWTotaldegree(set[an])>=o) && (pLmCmp(set[an],p) == cmp_int))
        {
          return an;
        }
        return en;
      }
      i=(an+en) / 2;
      if ((pWTotaldegree(set[an])>=o)
      && (pLmCmp(set[i],p) == cmp_int)) en=i;
      else                              an=i;
    }
  }
  else
  {
    if (pLmCmp(set[length],p)== -cmp_int)
      return length+1;

    loop
    {
      if (an >= en-1)
      {
        if (pLmCmp(set[an],p) == cmp_int) return an;
        if (pLmCmp(set[an],p) == -cmp_int) return en;
        if ((cmp_int!=1)
        && ((strat->ecartS[an])>ecart_p))
          return an;
        return en;
      }
      i=(an+en) / 2;
      if (pLmCmp(set[i],p) == cmp_int) en=i;
      else if (pLmCmp(set[i],p) == -cmp_int) an=i;
      else
      {
        if ((cmp_int!=1)
        &&((strat->ecartS[i])<ecart_p))
          en=i;
        else
          an=i;
      }
    }
  }
}


/*2
* looks up the position of p in set
* the position is the last one
*/
int posInT0 (const TSet set,const int length,LObject &p)
{
  return (length+1);
}


/*2
* looks up the position of p in T
* set[0] is the smallest with respect to the ordering-procedure
* pComp
*/
int posInT1 (const TSet set,const int length,LObject &p)
{
  if (length==-1) return 0;

  if (pLmCmp(set[length].p,p.p)!= pOrdSgn) return length+1;

  int i;
  int an = 0;
  int en= length;

  loop
  {
    if (an >= en-1)
    {
      if (pLmCmp(set[an].p,p.p) == pOrdSgn) return an;
      return en;
    }
    i=(an+en) / 2;
    if (pLmCmp(set[i].p,p.p) == pOrdSgn) en=i;
    else                                 an=i;
  }
}

/*2
* looks up the position of p in T
* set[0] is the smallest with respect to the ordering-procedure
* length
*/
int posInT2 (const TSet set,const int length,LObject &p)
{
  if (length==-1)
    return 0;
  if (set[length].length<p.length)
    return length+1;

  int i;
  int an = 0;
  int en= length;

  loop
  {
    if (an >= en-1)
    {
      if (set[an].length>p.length) return an;
      return en;
    }
    i=(an+en) / 2;
    if (set[i].length>p.length) en=i;
    else                        an=i;
  }
}

/*2
* looks up the position of p in T
* set[0] is the smallest with respect to the ordering-procedure
* totaldegree,pComp
*/
int posInT11 (const TSet set,const int length,LObject &p)
/*{
 * int j=0;
 * int o;
 *
 * o = p.GetpFDeg();
 * loop
 * {
 *   if ((pFDeg(set[j].p) > o)
 *   || ((pFDeg(set[j].p) == o) && (pLmCmp(set[j].p,p.p) == pOrdSgn)))
 *   {
 *     return j;
 *   }
 *   j++;
 *   if (j > length) return j;
 * }
 *}
 */
{
  if (length==-1) return 0;

  int o = p.GetpFDeg();
  int op = set[length].GetpFDeg();

  if ((op < o)
  || ((op == o) && (pLmCmp(set[length].p,p.p) != pOrdSgn)))
    return length+1;

  int i;
  int an = 0;
  int en= length;

  loop
  {
    if (an >= en-1)
    {
      op= set[an].GetpFDeg();
      if ((op > o)
      || (( op == o) && (pLmCmp(set[an].p,p.p) == pOrdSgn)))
        return an;
      return en;
    }
    i=(an+en) / 2;
    op = set[i].GetpFDeg();
    if (( op > o)
    || (( op == o) && (pLmCmp(set[i].p,p.p) == pOrdSgn)))
      en=i;
    else
      an=i;
  }
}

/*2
* looks up the position of p in T
* set[0] is the smallest with respect to the ordering-procedure
* totaldegree,pComp
*/
int posInT110 (const TSet set,const int length,LObject &p)
{
  if (length==-1) return 0;

  int o = p.GetpFDeg();
  int op = set[length].GetpFDeg();

  if (( op < o)
  || (( op == o) && (set[length].length<p.length))
  || (( op == o) && (set[length].length == p.length)
     && (pLmCmp(set[length].p,p.p) != pOrdSgn)))
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op = set[an].GetpFDeg();
      if (( op > o)
      || (( op == o) && (set[an].length > p.length))
      || (( op == o) && (set[an].length == p.length)
         && (pLmCmp(set[an].p,p.p) == pOrdSgn)))
        return an;
      return en;
    }
    i=(an+en) / 2;
    op = set[i].GetpFDeg();
    if (( op > o)
    || (( op == o) && (set[i].length > p.length))
    || (( op == o) && (set[i].length == p.length)
       && (pLmCmp(set[i].p,p.p) == pOrdSgn)))
      en=i;
    else
      an=i;
  }
}

/*2
* looks up the position of p in set
* set[0] is the smallest with respect to the ordering-procedure
* pFDeg
*/
int posInT13 (const TSet set,const int length,LObject &p)
{
  if (length==-1) return 0;

  int o = p.GetpFDeg();

  if (set[length].GetpFDeg() <= o)
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      if (set[an].GetpFDeg() > o)
        return an;
      return en;
    }
    i=(an+en) / 2;
    if (set[i].GetpFDeg() > o)
      en=i;
    else
      an=i;
  }
}

// determines the position based on: 1.) Ecart 2.) pLength
int posInT_EcartpLength(const TSet set,const int length,LObject &p)
{
  if (length==-1) return 0;

  int op=p.ecart;
  int ol = p.GetpLength();

  int oo=set[length].ecart;
  if ((oo < op) || ((oo==op) && (set[length].length < ol)))
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
    {
      if (an >= en-1)
      {
        int oo=set[an].ecart;
        if((oo > op)
           || ((oo==op) && (set[an].pLength > ol)))
          return an;
        return en;
      }
      i=(an+en) / 2;
      int oo=set[i].ecart;
      if ((oo > op)
          || ((oo == op) && (set[i].pLength > ol)))
        en=i;
      else
        an=i;
    }
}

/*2
* looks up the position of p in set
* set[0] is the smallest with respect to the ordering-procedure
* maximaldegree, pComp
*/
int posInT15 (const TSet set,const int length,LObject &p)
/*{
 *int j=0;
 * int o;
 *
 * o = p.GetpFDeg()+p.ecart;
 * loop
 * {
 *   if ((set[j].GetpFDeg()+set[j].ecart > o)
 *   || ((set[j].GetpFDeg()+set[j].ecart == o)
 *     && (pLmCmp(set[j].p,p.p) == pOrdSgn)))
 *   {
 *     return j;
 *   }
 *   j++;
 *   if (j > length) return j;
 * }
 *}
 */
{
  if (length==-1) return 0;

  int o = p.GetpFDeg() + p.ecart;
  int op = set[length].GetpFDeg()+set[length].ecart;

  if ((op < o)
  || ((op == o)
     && (pLmCmp(set[length].p,p.p) != pOrdSgn)))
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op = set[an].GetpFDeg()+set[an].ecart;
      if (( op > o)
      || (( op  == o) && (pLmCmp(set[an].p,p.p) == pOrdSgn)))
        return an;
      return en;
    }
    i=(an+en) / 2;
    op = set[i].GetpFDeg()+set[i].ecart;
    if (( op > o)
    || (( op == o) && (pLmCmp(set[i].p,p.p) == pOrdSgn)))
      en=i;
    else
      an=i;
  }
}

/*2
* looks up the position of p in set
* set[0] is the smallest with respect to the ordering-procedure
* pFDeg+ecart, ecart, pComp
*/
int posInT17 (const TSet set,const int length,LObject &p)
/*
*{
* int j=0;
* int  o;
*
*  o = p.GetpFDeg()+p.ecart;
*  loop
*  {
*    if ((pFDeg(set[j].p)+set[j].ecart > o)
*    || (((pFDeg(set[j].p)+set[j].ecart == o)
*      && (set[j].ecart < p.ecart)))
*    || ((pFDeg(set[j].p)+set[j].ecart == o)
*      && (set[j].ecart==p.ecart)
*      && (pLmCmp(set[j].p,p.p)==pOrdSgn)))
*      return j;
*    j++;
*    if (j > length) return j;
*  }
* }
*/
{
  if (length==-1) return 0;

  int o = p.GetpFDeg() + p.ecart;
  int op = set[length].GetpFDeg()+set[length].ecart;

  if ((op < o)
  || (( op == o) && (set[length].ecart > p.ecart))
  || (( op == o) && (set[length].ecart==p.ecart)
     && (pLmCmp(set[length].p,p.p) != pOrdSgn)))
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op = set[an].GetpFDeg()+set[an].ecart;
      if (( op > o)
      || (( op == o) && (set[an].ecart < p.ecart))
      || (( op  == o) && (set[an].ecart==p.ecart)
         && (pLmCmp(set[an].p,p.p) == pOrdSgn)))
        return an;
      return en;
    }
    i=(an+en) / 2;
    op = set[i].GetpFDeg()+set[i].ecart;
    if ((op > o)
    || (( op == o) && (set[i].ecart < p.ecart))
    || (( op == o) && (set[i].ecart == p.ecart)
       && (pLmCmp(set[i].p,p.p) == pOrdSgn)))
      en=i;
    else
      an=i;
  }
}
/*2
* looks up the position of p in set
* set[0] is the smallest with respect to the ordering-procedure
* pGetComp, pFDeg+ecart, ecart, pComp
*/
int posInT17_c (const TSet set,const int length,LObject &p)
{
  if (length==-1) return 0;

  int cc = (-1+2*currRing->order[0]==ringorder_c);
  /* cc==1 for (c,..), cc==-1 for (C,..) */
  int o = p.GetpFDeg() + p.ecart;
  int c = pGetComp(p.p)*cc;

  if (pGetComp(set[length].p)*cc < c)
    return length+1;
  if (pGetComp(set[length].p)*cc == c)
  {
    int op = set[length].GetpFDeg()+set[length].ecart;
    if ((op < o)
    || ((op == o) && (set[length].ecart > p.ecart))
    || ((op == o) && (set[length].ecart==p.ecart)
       && (pLmCmp(set[length].p,p.p) != pOrdSgn)))
      return length+1;
  }

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      if (pGetComp(set[an].p)*cc < c)
        return en;
      if (pGetComp(set[an].p)*cc == c)
      {
        int op = set[an].GetpFDeg()+set[an].ecart;
        if ((op > o)
        || ((op == o) && (set[an].ecart < p.ecart))
        || ((op == o) && (set[an].ecart==p.ecart)
           && (pLmCmp(set[an].p,p.p) == pOrdSgn)))
          return an;
      }
      return en;
    }
    i=(an+en) / 2;
    if (pGetComp(set[i].p)*cc > c)
      en=i;
    else if (pGetComp(set[i].p)*cc == c)
    {
      int op = set[i].GetpFDeg()+set[i].ecart;
      if ((op > o)
      || ((op == o) && (set[i].ecart < p.ecart))
      || ((op == o) && (set[i].ecart == p.ecart)
         && (pLmCmp(set[i].p,p.p) == pOrdSgn)))
        en=i;
      else
        an=i;
    }
    else
      an=i;
  }
}

/*2
* looks up the position of p in set
* set[0] is the smallest with respect to
* ecart, pFDeg, length
*/
int posInT19 (const TSet set,const int length,LObject &p)
{
  if (length==-1) return 0;

  int o = p.ecart;
  int op=p.GetpFDeg();

  if (set[length].ecart < o)
    return length+1;
  if (set[length].ecart == o)
  {
     int oo=set[length].GetpFDeg();
     if ((oo < op) || ((oo==op) && (set[length].length < p.length)))
       return length+1;
  }

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      if (set[an].ecart > o)
        return an;
      if (set[an].ecart == o)
      {
         int oo=set[an].GetpFDeg();
         if((oo > op)
         || ((oo==op) && (set[an].length > p.length)))
           return an;
      }
      return en;
    }
    i=(an+en) / 2;
    if (set[i].ecart > o)
      en=i;
    else if (set[i].ecart == o)
    {
       int oo=set[i].GetpFDeg();
       if ((oo > op)
       || ((oo == op) && (set[i].length > p.length)))
         en=i;
       else
        an=i;
    }
    else
      an=i;
  }
}

/*2
*looks up the position of polynomial p in set
*set[length] is the smallest element in set with respect
*to the ordering-procedure pComp
*/
int posInLSpecial (const LSet set, const int length,
                   LObject *p,const kStrategy strat)
{
  if (length<0) return 0;

  int d=p->GetpFDeg();
  int op=set[length].GetpFDeg();

  if ((op > d)
  || ((op == d) && (p->p1!=NULL)&&(set[length].p1==NULL))
  || (pLmCmp(set[length].p,p->p)== pOrdSgn))
     return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op=set[an].GetpFDeg();
      if ((op > d)
      || ((op == d) && (p->p1!=NULL) && (set[an].p1==NULL))
      || (pLmCmp(set[an].p,p->p)== pOrdSgn))
         return en;
      return an;
    }
    i=(an+en) / 2;
    op=set[i].GetpFDeg();
    if ((op>d)
    || ((op==d) && (p->p1!=NULL) && (set[i].p1==NULL))
    || (pLmCmp(set[i].p,p->p) == pOrdSgn))
      an=i;
    else
      en=i;
  }
}

/*2
*looks up the position of polynomial p in set
*set[length] is the smallest element in set with respect
*to the ordering-procedure pComp
*/
int posInL0 (const LSet set, const int length,
             LObject* p,const kStrategy strat)
{
  if (length<0) return 0;

  if (pLmCmp(set[length].p,p->p)== pOrdSgn)
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      if (pLmCmp(set[an].p,p->p) == pOrdSgn) return en;
      return an;
    }
    i=(an+en) / 2;
    if (pLmCmp(set[i].p,p->p) == pOrdSgn) an=i;
    else                                 en=i;
    /*aend. fuer lazy == in !=- machen */
  }
}

/*2
* looks up the position of polynomial p in set
* e is the ecart of p
* set[length] is the smallest element in set with respect
* to the ordering-procedure totaldegree,pComp
*/
int posInL11 (const LSet set, const int length,
              LObject* p,const kStrategy strat)
/*{
 * int j=0;
 * int o;
 *
 * o = p->GetpFDeg();
 * loop
 * {
 *   if (j > length)            return j;
 *   if ((set[j].GetpFDeg() < o)) return j;
 *   if ((set[j].GetpFDeg() == o) && (pLmCmp(set[j].p,p->p) == -pOrdSgn))
 *   {
 *     return j;
 *   }
 *   j++;
 * }
 *}
 */
{
  if (length<0) return 0;

  int o = p->GetpFDeg();
  int op = set[length].GetpFDeg();

  if ((op > o)
  || ((op == o) && (pLmCmp(set[length].p,p->p) != -pOrdSgn)))
    return length+1;
  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op = set[an].GetpFDeg();
      if ((op > o)
      || ((op == o) && (pLmCmp(set[an].p,p->p) != -pOrdSgn)))
        return en;
      return an;
    }
    i=(an+en) / 2;
    op = set[i].GetpFDeg();
    if ((op > o)
    || ((op == o) && (pLmCmp(set[i].p,p->p) != -pOrdSgn)))
      an=i;
    else
      en=i;
  }
}

/*2
* looks up the position of polynomial p in set
* set[length] is the smallest element in set with respect
* to the ordering-procedure totaldegree,pLength0
*/
int posInL110 (const LSet set, const int length,
               LObject* p,const kStrategy strat)
{
  if (length<0) return 0;

  int o = p->GetpFDeg();
  int op = set[length].GetpFDeg();

  if ((op > o)
  || ((op == o) && (set[length].length >2*p->length))
  || ((op == o) && (set[length].length <= 2*p->length)
     && (pLmCmp(set[length].p,p->p) != -pOrdSgn)))
    return length+1;
  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op = set[an].GetpFDeg();
      if ((op > o)
      || ((op == o) && (set[an].length >2*p->length))
      || ((op == o) && (set[an].length <=2*p->length)
         && (pLmCmp(set[an].p,p->p) != -pOrdSgn)))
        return en;
      return an;
    }
    i=(an+en) / 2;
    op = set[i].GetpFDeg();
    if ((op > o)
    || ((op == o) && (set[i].length > 2*p->length))
    || ((op == o) && (set[i].length <= 2*p->length)
       && (pLmCmp(set[i].p,p->p) != -pOrdSgn)))
      an=i;
    else
      en=i;
  }
}

/*2
* looks up the position of polynomial p in set
* e is the ecart of p
* set[length] is the smallest element in set with respect
* to the ordering-procedure totaldegree
*/
int posInL13 (const LSet set, const int length,
              LObject* p,const kStrategy strat)
{
  if (length<0) return 0;

  int o = p->GetpFDeg();

  if (set[length].GetpFDeg() > o)
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      if (set[an].GetpFDeg() >= o)
        return en;
      return an;
    }
    i=(an+en) / 2;
    if (set[i].GetpFDeg() >= o)
      an=i;
    else
      en=i;
  }
}

/*2
* looks up the position of polynomial p in set
* e is the ecart of p
* set[length] is the smallest element in set with respect
* to the ordering-procedure maximaldegree,pComp
*/
int posInL15 (const LSet set, const int length,
              LObject* p,const kStrategy strat)
/*{
 * int j=0;
 * int o;
 *
 * o = p->ecart+p->GetpFDeg();
 * loop
 * {
 *   if (j > length)                       return j;
 *   if (set[j].GetpFDeg()+set[j].ecart < o) return j;
 *   if ((set[j].GetpFDeg()+set[j].ecart == o)
 *   && (pLmCmp(set[j].p,p->p) == -pOrdSgn))
 *   {
 *     return j;
 *   }
 *   j++;
 * }
 *}
 */
{
  if (length<0) return 0;

  int o = p->GetpFDeg() + p->ecart;
  int op = set[length].GetpFDeg() + set[length].ecart;

  if ((op > o)
  || ((op == o) && (pLmCmp(set[length].p,p->p) != -pOrdSgn)))
    return length+1;
  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op = set[an].GetpFDeg() + set[an].ecart;
      if ((op > o)
      || ((op == o) && (pLmCmp(set[an].p,p->p) != -pOrdSgn)))
        return en;
      return an;
    }
    i=(an+en) / 2;
    op = set[i].GetpFDeg() + set[i].ecart;
    if ((op > o)
    || ((op == o) && (pLmCmp(set[i].p,p->p) != -pOrdSgn)))
      an=i;
    else
      en=i;
  }
}

/*2
* looks up the position of polynomial p in set
* e is the ecart of p
* set[length] is the smallest element in set with respect
* to the ordering-procedure totaldegree
*/
int posInL17 (const LSet set, const int length,
              LObject* p,const kStrategy strat)
{
  if (length<0) return 0;

  int o = p->GetpFDeg() + p->ecart;

  if ((set[length].GetpFDeg() + set[length].ecart > o)
  || ((set[length].GetpFDeg() + set[length].ecart == o)
     && (set[length].ecart > p->ecart))
  || ((set[length].GetpFDeg() + set[length].ecart == o)
     && (set[length].ecart == p->ecart)
     && (pLmCmp(set[length].p,p->p) != -pOrdSgn)))
    return length+1;
  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      if ((set[an].GetpFDeg() + set[an].ecart > o)
      || ((set[an].GetpFDeg() + set[an].ecart == o)
         && (set[an].ecart > p->ecart))
      || ((set[an].GetpFDeg() + set[an].ecart == o)
         && (set[an].ecart == p->ecart)
         && (pLmCmp(set[an].p,p->p) != -pOrdSgn)))
        return en;
      return an;
    }
    i=(an+en) / 2;
    if ((set[i].GetpFDeg() + set[i].ecart > o)
    || ((set[i].GetpFDeg() + set[i].ecart == o)
       && (set[i].ecart > p->ecart))
    || ((set[i].GetpFDeg() +set[i].ecart == o)
       && (set[i].ecart == p->ecart)
       && (pLmCmp(set[i].p,p->p) != -pOrdSgn)))
      an=i;
    else
      en=i;
  }
}
/*2
* looks up the position of polynomial p in set
* e is the ecart of p
* set[length] is the smallest element in set with respect
* to the ordering-procedure pComp
*/
int posInL17_c (const LSet set, const int length,
                LObject* p,const kStrategy strat)
{
  if (length<0) return 0;

  int cc = (-1+2*currRing->order[0]==ringorder_c);
  /* cc==1 for (c,..), cc==-1 for (C,..) */
  int c = pGetComp(p->p)*cc;
  int o = p->GetpFDeg() + p->ecart;

  if (pGetComp(set[length].p)*cc > c)
    return length+1;
  if (pGetComp(set[length].p)*cc == c)
  {
    if ((set[length].GetpFDeg() + set[length].ecart > o)
    || ((set[length].GetpFDeg() + set[length].ecart == o)
       && (set[length].ecart > p->ecart))
    || ((set[length].GetpFDeg() + set[length].ecart == o)
       && (set[length].ecart == p->ecart)
       && (pLmCmp(set[length].p,p->p) != -pOrdSgn)))
      return length+1;
  }
  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      if (pGetComp(set[an].p)*cc > c)
        return en;
      if (pGetComp(set[an].p)*cc == c)
      {
        if ((set[an].GetpFDeg() + set[an].ecart > o)
        || ((set[an].GetpFDeg() + set[an].ecart == o)
           && (set[an].ecart > p->ecart))
        || ((set[an].GetpFDeg() + set[an].ecart == o)
           && (set[an].ecart == p->ecart)
           && (pLmCmp(set[an].p,p->p) != -pOrdSgn)))
          return en;
      }
      return an;
    }
    i=(an+en) / 2;
    if (pGetComp(set[i].p)*cc > c)
      an=i;
    else if (pGetComp(set[i].p)*cc == c)
    {
      if ((set[i].GetpFDeg() + set[i].ecart > o)
      || ((set[i].GetpFDeg() + set[i].ecart == o)
         && (set[i].ecart > p->ecart))
      || ((set[i].GetpFDeg() +set[i].ecart == o)
         && (set[i].ecart == p->ecart)
         && (pLmCmp(set[i].p,p->p) != -pOrdSgn)))
        an=i;
      else
        en=i;
    }
    else
      en=i;
  }
}

/***************************************************************
 *
 * Tail reductions
 *
 ***************************************************************/
TObject*
kFindDivisibleByInS(kStrategy strat, int pos, LObject* L, TObject *T,
                    long ecart)
{
  int j = 0;
  const unsigned long not_sev = ~L->sev;
  const unsigned long* sev = strat->sevS;
  poly p;
  ring r;
  L->GetLm(p, r);

  assume(~not_sev == p_GetShortExpVector(p, r));

  if (r == currRing)
  {
    loop
    {
      if (j > pos) return NULL;
#if defined(PDEBUG) || defined(PDIV_DEBUG)
      if (p_LmShortDivisibleBy(strat->S[j], sev[j], p, not_sev, r) &&
          (ecart== LONG_MAX || ecart>= strat->ecartS[j]))
        break;
#else
      if (!(sev[j] & not_sev) &&
          (ecart== LONG_MAX || ecart>= strat->ecartS[j]) &&
          p_LmDivisibleBy(strat->S[j], p, r))
        break;

#endif
      j++;
    }
    // if called from NF, T objects do not exist:
    if (strat->tl < 0 || strat->S_2_R[j] == -1)
    {
      T->Set(strat->S[j], r, strat->tailRing);
      return T;
    }
    else
    {
      assume (j >= 0 && j <= strat->tl && strat->S_2_T(j) != NULL &&
              strat->S_2_T(j)->p == strat->S[j]);
      return strat->S_2_T(j);
    }
  }
  else
  {
    TObject* t;
    loop
    {
      if (j > pos) return NULL;
      assume(strat->S_2_R[j] != -1);
#if defined(PDEBUG) || defined(PDIV_DEBUG)
      t = strat->S_2_T(j);
      assume(t != NULL && t->t_p != NULL && t->tailRing == r);
      if (p_LmShortDivisibleBy(t->t_p, sev[j], p, not_sev, r) &&
          (ecart== LONG_MAX || ecart>= strat->ecartS[j]))
        return t;
#else
      if (! (sev[j] & not_sev) && (ecart== LONG_MAX || ecart>= strat->ecartS[j]))
      {
        t = strat->S_2_T(j);
        assume(t != NULL && t->t_p != NULL && t->tailRing == r && t->p == strat->S[j]);
        if (p_LmDivisibleBy(t->t_p, p, r)) return t;
      }
#endif
      j++;
    }
  }
}


poly redtail (LObject* L, int pos, kStrategy strat)
{
  poly h, hn;
  int j;
  unsigned long not_sev;
  strat->redTailChange=FALSE;

  poly p = L->p;
  if (strat->noTailReduction || pNext(p) == NULL)
    return p;

  LObject Ln(strat->tailRing);
  TObject* With;
  // placeholder in case strat->tl < 0
  TObject  With_s(strat->tailRing);
  h = p;
  hn = pNext(h);
  long op = strat->tailRing->pFDeg(hn, strat->tailRing);
  long e;
  int l;
  BOOLEAN save_HE=strat->kHEdgeFound;
  strat->kHEdgeFound |=
    ((Kstd1_deg>0) && (op<=Kstd1_deg)) || TEST_OPT_INFREDTAIL;

  while(hn != NULL)
  {
    op = strat->tailRing->pFDeg(hn, strat->tailRing);
    if ((Kstd1_deg>0)&&(op>Kstd1_deg)) goto all_done;
    e = strat->tailRing->pLDeg(hn, &l, strat->tailRing) - op;
    loop
    {
      Ln.Set(hn, strat->tailRing);
      Ln.sev = p_GetShortExpVector(hn, strat->tailRing);
      if (strat->kHEdgeFound)
        With = kFindDivisibleByInS(strat, pos, &Ln, &With_s);
      else
        With = kFindDivisibleByInS(strat, pos, &Ln, &With_s, e);
      if (With == NULL) break;
      strat->redTailChange=TRUE;
      if (ksReducePolyTail(L, With, h, strat->kNoetherTail()))
      {
        // reducing the tail would violate the exp bound
        if (kStratChangeTailRing(strat, L))
        {
          strat->kHEdgeFound = save_HE;
          return redtail(L, pos, strat);
        }
        else
          return NULL;
      }
      hn = pNext(h);
      if (hn == NULL) goto all_done;
      op = strat->tailRing->pFDeg(hn, strat->tailRing);
      if ((Kstd1_deg>0)&&(op>Kstd1_deg)) goto all_done;
      e = strat->tailRing->pLDeg(hn, &l, strat->tailRing) - op;
    }
    h = hn;
    hn = pNext(h);
  }

  all_done:
  if (strat->redTailChange)
  {
    L->last = 0;
    L->pLength = 0;
  }
  strat->kHEdgeFound = save_HE;
  return p;
}

poly redtail (poly p, int pos, kStrategy strat)
{
  LObject L(p, currRing);
  return redtail(&L, pos, strat);
}

poly redtailBba (LObject* L, int pos, kStrategy strat, BOOLEAN withT)
{
  strat->redTailChange=FALSE;
  if (strat->noTailReduction) return L->GetLmCurrRing();
  poly h, p;
  p = h = L->GetLmTailRing();
  if ((h==NULL) || (pNext(h)==NULL))
    return L->GetLmCurrRing();

  TObject* With;
  // placeholder in case strat->tl < 0
  TObject  With_s(strat->tailRing);

  LObject Ln(pNext(h), strat->tailRing);
  Ln.pLength = L->GetpLength() - 1;

  pNext(h) = NULL;
  if (L->p != NULL) pNext(L->p) = NULL;
  L->pLength = 1;

  Ln.PrepareRed(strat->use_buckets);

  while(!Ln.IsNull())
  {
    loop
    {
      Ln.SetShortExpVector();
      if (! withT)
      {
        With = kFindDivisibleByInS(strat, pos, &Ln, &With_s);
        if (With == NULL) break;
      }
      else
      {
        int j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, &Ln);
        if (j < 0) break;
        With = &(strat->T[j]);
      }
      if (ksReducePolyTail(L, With, &Ln))
      {
        // reducing the tail would violate the exp bound
        pNext(h) = Ln.GetTP();
        L->pLength += Ln.GetpLength();
        if (L->p != NULL) pNext(L->p) = pNext(p);
        if (kStratChangeTailRing(strat, L))
          return redtailBba(L, pos, strat, withT);
        else
        { // should never get here -- need to fix this
          assume(0);
          return NULL;
        }
      }
      strat->redTailChange=TRUE;
      if (Ln.IsNull()) goto all_done;
      if (! withT) With_s.Init(currRing);
    }
    pNext(h) = Ln.LmExtractAndIter();
    pIter(h);
    L->pLength++;
  }

  all_done:
  if (L->p != NULL) pNext(L->p) = pNext(p);
  assume(pLength(L->p != NULL ? L->p : L->t_p) == L->pLength);

  if (strat->redTailChange)
  {
    L->last = NULL;
    L->length = 0;
  }
  L->Normalize(); // HANNES: should have a test
  kTest_L(L);
  return L->GetLmCurrRing();
}

/*2
*checks the change degree and write progress report
*/
void message (int i,int* reduc,int* olddeg,kStrategy strat, int red_result)
{
  if (i != *olddeg)
  {
    Print("%d",i);
    *olddeg = i;
  }
  if (K_TEST_OPT_OLDSTD)
  {
    if (strat->Ll != *reduc)
    {
      if (strat->Ll != *reduc-1)
        Print("(%d)",strat->Ll+1);
      else
        PrintS("-");
      *reduc = strat->Ll;
    }
    else
      PrintS(".");
    mflush();
  }
  else
  {
    if (red_result == 0)
      PrintS("-");
    else if (red_result < 0)
      PrintS(".");
    if ((red_result > 0) || ((strat->Ll % 100)==99))
    {
      if (strat->Ll != *reduc && strat->Ll > 0)
      {
        Print("(%d)",strat->Ll+1);
        *reduc = strat->Ll;
      }
    }
  }
}

/*2
*statistics
*/
void messageStat (int srmax,int lrmax,int hilbcount,kStrategy strat)
{
  //PrintS("\nUsage/Allocation of temporary storage:\n");
  //Print("%d/%d polynomials in standard base\n",srmax,IDELEMS(Shdl));
  //Print("%d/%d polynomials in set L (for lazy alg.)",lrmax+1,strat->Lmax);
  Print("\nproduct criterion:%d chain criterion:%d\n",strat->cp,strat->c3);
  if (hilbcount!=0) Print("hilbert series criterion:%d\n",hilbcount);
  /*mflush();*/
}

#ifdef KDEBUG
/*2
*debugging output: all internal sets, if changed
*for testing purpuse only/has to be changed for later use
*/
void messageSets (kStrategy strat)
{
  int i;
  if (strat->news)
  {
    PrintS("set S");
    for (i=0; i<=strat->sl; i++)
    {
      Print("\n  %d:",i);
      p_wrp(strat->S[i], currRing, strat->tailRing);
    }
    strat->news = FALSE;
  }
  if (strat->newt)
  {
    PrintS("\nset T");
    for (i=0; i<=strat->tl; i++)
    {
      Print("\n  %d:",i);
      strat->T[i].wrp();
      Print(" o:%d e:%d l:%d",
        strat->T[i].pFDeg(),strat->T[i].ecart,strat->T[i].length);
    }
    strat->newt = FALSE;
  }
  PrintS("\nset L");
  for (i=strat->Ll; i>=0; i--)
  {
    Print("\n%d:",i);
    p_wrp(strat->L[i].p1, currRing, strat->tailRing);
    PrintS("  ");
    p_wrp(strat->L[i].p2, currRing, strat->tailRing);
    PrintS(" lcm: ");p_wrp(strat->L[i].lcm, currRing);
    PrintS("\n  p : ");
    strat->L[i].wrp();
    Print("  o:%d e:%d l:%d",
          strat->L[i].pFDeg(),strat->L[i].ecart,strat->L[i].length);
  }
  PrintLn();
}

#endif


/*2
*construct the set s from F
*/
void initS (ideal F, ideal Q,kStrategy strat)
{
  int   i,pos;

  if (Q!=NULL) i=((IDELEMS(Q)+(setmaxTinc-1))/setmaxTinc)*setmaxTinc;
  else i=setmaxT;
  strat->ecartS=initec(i);
  strat->sevS=initsevS(i);
  strat->S_2_R=initS_2_R(i);
  strat->fromQ=NULL;
  strat->Shdl=idInit(i,F->rank);
  strat->S=strat->Shdl->m;
  /*- put polys into S -*/
  if (Q!=NULL)
  {
    strat->fromQ=initec(i);
    memset(strat->fromQ,0,i*sizeof(int));
    for (i=0; i<IDELEMS(Q); i++)
    {
      if (Q->m[i]!=NULL)
      {
        LObject h;
        h.p = pCopy(Q->m[i]);
        if (TEST_OPT_INTSTRATEGY)
        {
          //pContent(h.p);
          h.pCleardenom(); // also does a pContent
        }
        else
        {
          h.pNorm();
        }
        strat->initEcart(&h);
        if (pOrdSgn==-1)
        {
          deleteHC(&h, strat);
        }
        if (h.p!=NULL)
        {
          if (strat->sl==-1)
            pos =0;
          else
          {
            pos = posInS(strat,strat->sl,h.p,h.ecart);
          }
          h.sev = pGetShortExpVector(h.p);
          strat->enterS(h,pos,strat,-1);
          strat->fromQ[pos]=1;
        }
      }
    }
  }
  for (i=0; i<IDELEMS(F); i++)
  {
    if (F->m[i]!=NULL)
    {
      LObject h;
      h.p = pCopy(F->m[i]);
      if (TEST_OPT_INTSTRATEGY)
      {
        //pContent(h.p);
        h.pCleardenom(); // also does a pContent
      }
      else
      {
        h.pNorm();
      }
      strat->initEcart(&h);
      if (pOrdSgn==-1)
      {
        cancelunit(&h);  /*- tries to cancel a unit -*/
        deleteHC(&h, strat);
      }
      if (h.p!=NULL)
      {
        if (strat->sl==-1)
          pos =0;
        else
          pos = posInS(strat,strat->sl,h.p,h.ecart);
        h.sev = pGetShortExpVector(h.p);
        strat->enterS(h,pos,strat,-1);
      }
    }
  }
  /*- test, if a unit is in F -*/
  if ((strat->sl>=0) && pIsConstant(strat->S[0]))
  {
    while (strat->sl>0) deleteInS(strat->sl,strat);
  }
}

void initSL (ideal F, ideal Q,kStrategy strat)
{
  int   i,pos;

  if (Q!=NULL) i=((IDELEMS(Q)+(setmaxTinc-1))/setmaxTinc)*setmaxTinc;
  else i=setmaxT;
  strat->ecartS=initec(i);
  strat->sevS=initsevS(i);
  strat->S_2_R=initS_2_R(i);
  strat->fromQ=NULL;
  strat->Shdl=idInit(i,F->rank);
  strat->S=strat->Shdl->m;
  /*- put polys into S -*/
  if (Q!=NULL)
  {
    strat->fromQ=initec(i);
    memset(strat->fromQ,0,i*sizeof(int));
    for (i=0; i<IDELEMS(Q); i++)
    {
      if (Q->m[i]!=NULL)
      {
        LObject h;
        h.p = pCopy(Q->m[i]);
        if (TEST_OPT_INTSTRATEGY)
        {
          //pContent(h.p);
          h.pCleardenom(); // also does a pContent
        }
        else
        {
          h.pNorm();
        }
        strat->initEcart(&h);
        if (pOrdSgn==-1)
        {
          deleteHC(&h,strat);
        }
        if (h.p!=NULL)
        {
          if (strat->sl==-1)
            pos =0;
          else
          {
            pos = posInS(strat,strat->sl,h.p,h.ecart);
          }
          h.sev = pGetShortExpVector(h.p);
          strat->enterS(h,pos,strat,-1);
          strat->fromQ[pos]=1;
        }
      }
    }
  }
  for (i=0; i<IDELEMS(F); i++)
  {
    if (F->m[i]!=NULL)
    {
      LObject h;
      h.p = pCopy(F->m[i]);
      if (TEST_OPT_INTSTRATEGY)
      {
        //pContent(h.p);
        h.pCleardenom(); // also does a pContent
      }
      else
      {
        h.pNorm();
      }
      strat->initEcart(&h);
      if (pOrdSgn==-1)
      {
        cancelunit(&h);  /*- tries to cancel a unit -*/
        deleteHC(&h, strat);
      }
      if (h.p!=NULL)
      {
        if (strat->Ll==-1)
          pos =0;
        else
            pos = strat->posInL(strat->L,strat->Ll,&h,strat);
        h.sev = pGetShortExpVector(h.p);
        enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);
      }
    }
  }
  /*- test, if a unit is in F -*/
  if ((strat->Ll>=0) && pIsConstant(strat->L[strat->Ll].p))
  {
    while (strat->Ll>0) deleteInL(strat->L,&strat->Ll,strat->Ll-1,strat);
  }
}


/*2
*construct the set s from F and {P}
*/
void initSSpecial (ideal F, ideal Q, ideal P,kStrategy strat)
{
  int   i,pos;

  if (Q!=NULL) i=((IDELEMS(Q)+(setmaxTinc-1))/setmaxTinc)*setmaxTinc;
  else i=setmaxT;
  i=((i+IDELEMS(F)+15)/16)*16;
  strat->ecartS=initec(i);
  strat->sevS=initsevS(i);
  strat->S_2_R=initS_2_R(i);
  strat->fromQ=NULL;
  strat->Shdl=idInit(i,F->rank);
  strat->S=strat->Shdl->m;

  /*- put polys into S -*/
  if (Q!=NULL)
  {
    strat->fromQ=initec(i);
    memset(strat->fromQ,0,i*sizeof(int));
    for (i=0; i<IDELEMS(Q); i++)
    {
      if (Q->m[i]!=NULL)
      {
        LObject h;
        h.p = pCopy(Q->m[i]);
        //if (TEST_OPT_INTSTRATEGY)
        //{
        //  //pContent(h.p);
        //  h.pCleardenom(); // also does a pContent
        //}
        //else
        //{
        //  h.pNorm();
        //}
        strat->initEcart(&h);
        if (pOrdSgn==-1)
        {
          deleteHC(&h,strat);
        }
        if (h.p!=NULL)
        {
          if (strat->sl==-1)
            pos =0;
          else
          {
            pos = posInS(strat,strat->sl,h.p,h.ecart);
          }
          h.sev = pGetShortExpVector(h.p);
          h.SetpFDeg();
          strat->enterS(h,pos,strat, strat->tl+1);
          enterT(h, strat);
          strat->fromQ[pos]=1;
        }
      }
    }
  }
  /*- put polys into S -*/
  for (i=0; i<IDELEMS(F); i++)
  {
    if (F->m[i]!=NULL)
    {
      LObject h;
      h.p = pCopy(F->m[i]);
      if (pOrdSgn==1)
      {
        h.p=redtailBba(h.p,strat->sl,strat);
      }
      strat->initEcart(&h);
      if (pOrdSgn==-1)
      {
        deleteHC(&h,strat);
      }
      if (h.p!=NULL)
      {
        if (strat->sl==-1)
          pos =0;
        else
          pos = posInS(strat,strat->sl,h.p,h.ecart);
        h.sev = pGetShortExpVector(h.p);
        strat->enterS(h,pos,strat, strat->tl+1);
        h.length = pLength(h.p);
        h.SetpFDeg();
        enterT(h,strat);
      }
    }
  }
  for (i=0; i<IDELEMS(P); i++)
  {
    if (P->m[i]!=NULL)
    {
      LObject h;
      h.p=pCopy(P->m[i]);
      strat->initEcart(&h);
      h.length = pLength(h.p);
      if (TEST_OPT_INTSTRATEGY)
      {
        h.pCleardenom();
      }
      else
      {
        h.pNorm();
      }
      if(strat->sl>=0)
      {
        if (pOrdSgn==1)
        {
          h.p=redBba(h.p,strat->sl,strat);
          if (h.p!=NULL)
            h.p=redtailBba(h.p,strat->sl,strat);
        }
        else
        {
          h.p=redMora(h.p,strat->sl,strat);
          strat->initEcart(&h);
        }
        if(h.p!=NULL)
        {
          if (TEST_OPT_INTSTRATEGY)
          {
            h.pCleardenom();
          }
          else
          {
            h.is_normalized = 0;
            h.pNorm();
          }
          h.sev = pGetShortExpVector(h.p);
          h.SetpFDeg();
          pos = posInS(strat,strat->sl,h.p,h.ecart);
          enterpairsSpecial(h.p,strat->sl,h.ecart,pos,strat,strat->tl+1);
          strat->enterS(h,pos,strat, strat->tl+1);
          enterT(h,strat);
        }
      }
      else
      {
        h.sev = pGetShortExpVector(h.p);
        h.SetpFDeg();
        strat->enterS(h,0,strat, strat->tl+1);
        enterT(h,strat);
      }
    }
  }
}
/*2
* reduces h using the set S
* procedure used in cancelunit1
*/
static poly redBba1 (poly h,int maxIndex,kStrategy strat)
{
  int j = 0;
  unsigned long not_sev = ~ pGetShortExpVector(h);

  while (j <= maxIndex)
  {
    if (pLmShortDivisibleBy(strat->S[j],strat->sevS[j],h, not_sev))
       return ksOldSpolyRedNew(strat->S[j],h,strat->kNoetherTail());
    else j++;
  }
  return h;
}

/*2
*tests if p.p=monomial*unit and cancels the unit
*/
void cancelunit1 (LObject* p,int *suc, int index,kStrategy strat )
{
  int k;
  poly r,h,h1,q;

  if (!pIsVector((*p).p) && ((*p).ecart != 0))
  {
    k = 0;
    h1 = r = pCopy((*p).p);
    h =pNext(r);
    loop
    {
      if (h==NULL)
      {
        pDelete(&r);
        pDelete(&(pNext((*p).p)));
        (*p).ecart = 0;
        (*p).length = 1;
        (*suc)=0;
        return;
      }
      if (!pDivisibleBy(r,h))
      {
        q=redBba1(h,index ,strat);
        if (q != h)
        {
          k++;
          pDelete(&h);
          pNext(h1) = h = q;
        }
        else
        {
          pDelete(&r);
          return;
        }
      }
      else
      {
        h1 = h;
        pIter(h);
      }
      if (k > 10)
      {
        pDelete(&r);
        return;
      }
    }
  }
}

/*2
* reduces h using the elements from Q in the set S
* procedure used in updateS
* must not be used for elements of Q or elements of an ideal !
*/
static poly redQ (poly h, int j, kStrategy strat)
{
  int start;
  unsigned long not_sev = ~ pGetShortExpVector(h);
  while ((j <= strat->sl) && (pGetComp(strat->S[j])!=0)) j++;
  start=j;
  while (j<=strat->sl)
  {
    if (pLmShortDivisibleBy(strat->S[j],strat->sevS[j], h, not_sev))
    {
      h = ksOldSpolyRed(strat->S[j],h,strat->kNoetherTail());
      if (h==NULL) return NULL;
      j = start;
      not_sev = ~ pGetShortExpVector(h);
    }
    else j++;
  }
  return h;
}

/*2
* reduces h using the set S
* procedure used in updateS
*/
static poly redBba (poly h,int maxIndex,kStrategy strat)
{
  int j = 0;
  unsigned long not_sev = ~ pGetShortExpVector(h);

  while (j <= maxIndex)
  {
    if (pLmShortDivisibleBy(strat->S[j],strat->sevS[j], h, not_sev))
    {
      h = ksOldSpolyRed(strat->S[j],h,strat->kNoetherTail());
      if (h==NULL) return NULL;
      j = 0;
      not_sev = ~ pGetShortExpVector(h);    }
    else j++;
  }
  return h;
}

/*2
* reduces h using the set S
*e is the ecart of h
*procedure used in updateS
*/
static poly redMora (poly h,int maxIndex,kStrategy strat)
{
  int  j=0;
  int  e,l;
  unsigned long not_sev = ~ pGetShortExpVector(h);

  if (maxIndex >= 0)
  {
    e = pLDeg(h,&l,currRing)-pFDeg(h,currRing);
    do
    {
      if (pLmShortDivisibleBy(strat->S[j],strat->sevS[j], h, not_sev)
      && ((e >= strat->ecartS[j]) || strat->kHEdgeFound))
      {
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
          {PrintS("reduce ");wrp(h);Print(" with S[%d] (",j);wrp(strat->S[j]);}

#endif
        h = ksOldSpolyRed(strat->S[j],h,strat->kNoetherTail());
#ifdef KDEBUG
        if(TEST_OPT_DEBUG)
          {PrintS(")\nto "); wrp(h); PrintLn();}

#endif
        // pDelete(&h);
        if (h == NULL) return NULL;
        e = pLDeg(h,&l,currRing)-pFDeg(h,currRing);
        j = 0;
        not_sev = ~ pGetShortExpVector(h);
      }
      else j++;
    }
    while (j <= maxIndex);
  }
  return h;
}

/*2
*updates S:
*the result is a set of polynomials which are in
*normalform with respect to S
*/
void updateS(BOOLEAN toT,kStrategy strat)
{
  LObject h;
  int i, suc=0;
  poly redSi=NULL;
//Print("nach initS: updateS start mit sl=%d\n",(strat->sl));
//  for (i=0; i<=(strat->sl); i++)
//  {
//    Print("s%d:",i);
//    if (strat->fromQ!=NULL) Print("(Q:%d) ",strat->fromQ[i]);
//    pWrite(strat->S[i]);
//  }
  if (pOrdSgn==1)
  {
    while (suc != -1)
    {
      i=suc+1;
      while (i<=strat->sl)
      {
        if (((strat->syzComp==0) || (pGetComp(strat->S[i])<=strat->syzComp))
        && ((strat->fromQ==NULL) || (strat->fromQ[i]==0)))
        {
          pDelete(&redSi);
          redSi = pHead(strat->S[i]);
          strat->S[i] = redBba(strat->S[i],i-1,strat);
          if ((strat->ak!=0)&&(strat->S[i]!=NULL))
            strat->S[i]=redQ(strat->S[i],i+1,strat); /*reduce S[i] mod Q*/
          if (TEST_OPT_DEBUG && (pCmp(redSi,strat->S[i])!=0))
          {
            PrintS("reduce:");
            wrp(redSi);PrintS(" to ");p_wrp(strat->S[i], currRing, strat->tailRing);PrintLn();
          }
          if (TEST_OPT_PROT && (pCmp(redSi,strat->S[i])!=0))
          {
            if (strat->S[i]==NULL)
              PrintS("V");
            else
              PrintS("v");
            mflush();
          }
          if (strat->S[i]==NULL)
          {
            pDelete(&redSi);
            deleteInS(i,strat);
            i--;
          }
          else
          {
            pDelete(&redSi);
            if (TEST_OPT_INTSTRATEGY)
            {
              //pContent(strat->S[i]);
              pCleardenom(strat->S[i]);// also does a pContent
            }
            else
            {
              pNorm(strat->S[i]);
            }
            strat->sevS[i] = pGetShortExpVector(strat->S[i]);
          }
        }
        i++;
      }
      reorderS(&suc,strat);
    }
    if (toT)
    {
      for (i=0; i<=strat->sl; i++)
      {
        if ((strat->fromQ==NULL) || (strat->fromQ[i]==0))
        {
          h.p = redtailBba(strat->S[i],i-1,strat);
          if (TEST_OPT_INTSTRATEGY)
          {
            pCleardenom(h.p);// also does a pContent
          }
        }
        else
        {
          h.p = strat->S[i];
        }
        if (strat->honey)
        {
          strat->initEcart(&h);
          strat->ecartS[i] = h.ecart;
        }
        if (strat->sevS[i] == 0) {strat->sevS[i] = pGetShortExpVector(h.p);}
        else assume(strat->sevS[i] == pGetShortExpVector(h.p));
        h.sev = strat->sevS[i];
        h.SetpFDeg();
        /*puts the elements of S also to T*/
        enterT(h,strat);
        strat->S_2_R[i] = strat->tl;
      }
    }
  }
  else
  {
    while (suc != -1)
    {
      i=suc;
      while (i<=strat->sl)
      {
        if (((strat->syzComp==0) || (pGetComp(strat->S[i])<=strat->syzComp))
        && ((strat->fromQ==NULL) || (strat->fromQ[i]==0)))
        {
          pDelete(&redSi);
          redSi=pHead((strat->S)[i]);
          (strat->S)[i] = redMora((strat->S)[i],i-1,strat);
          if ((strat->S)[i]==NULL)
          {
            deleteInS(i,strat);
            i--;
          }
          else
          {
            if (TEST_OPT_INTSTRATEGY)
            {
              pDelete(&redSi);
              pCleardenom(strat->S[i]);// also does a pContent
              h.p = strat->S[i];
              strat->initEcart(&h);
              strat->ecartS[i] = h.ecart;
            }
            else
            {
              pDelete(&redSi);
              pNorm(strat->S[i]);
              h.p = strat->S[i];
              strat->initEcart(&h);
              strat->ecartS[i] = h.ecart;
            }
            h.sev =  pGetShortExpVector(h.p);
            strat->sevS[i] = h.sev;
          }
          kTest(strat);
        }
        i++;
      }
#ifdef KDEBUG
      kTest(strat);
#endif
      reorderS(&suc,strat);
      if (h.p!=NULL)
      {
        if (!strat->kHEdgeFound)
        {
          /*strat->kHEdgeFound =*/ HEckeTest(h.p,strat);
        }
        if (strat->kHEdgeFound)
          newHEdge(strat->S,strat);
      }
    }
    for (i=0; i<=strat->sl; i++)
    {
      if (((strat->fromQ==NULL) || (strat->fromQ[i]==0))
      )
      {
        strat->S[i] = h.p = redtail(strat->S[i],strat->sl,strat);
        strat->initEcart(&h);
        strat->ecartS[i] = h.ecart;
        h.sev = pGetShortExpVector(h.p);
        strat->sevS[i] = h.sev;
      }
      else
      {
        h.p = strat->S[i];
        h.ecart=strat->ecartS[i];
        h.sev = strat->sevS[i];
      }
      if ((strat->fromQ==NULL) || (strat->fromQ[i]==0))
        cancelunit1(&h,&suc,strat->sl,strat);
      h.length = pLength(h.p);
      h.SetpFDeg();
      /*puts the elements of S also to T*/
      enterT(h,strat);
      strat->S_2_R[i] = strat->tl;
    }
    if (suc!= -1) updateS(toT,strat);
  }
  if (redSi!=NULL) pDeleteLm(&redSi);
#ifdef KDEBUG
  kTest(strat);
#endif
}


/*2
* -puts p to the standardbasis s at position at
* -saves the result in S
*/
void enterSBba (LObject p,int atS,kStrategy strat, int atR)
{
  int i;
  strat->news = TRUE;
  /*- puts p to the standardbasis s at position at -*/
  if (strat->sl == IDELEMS(strat->Shdl)-1)
  {
    strat->sevS = (unsigned long*) omRealloc0Size(strat->sevS,
                                    IDELEMS(strat->Shdl)*sizeof(unsigned long),
                                    (IDELEMS(strat->Shdl)+setmaxTinc)
                                                  *sizeof(unsigned long));
    strat->ecartS = (intset)omReallocSize(strat->ecartS,
                                          IDELEMS(strat->Shdl)*sizeof(int),
                                          (IDELEMS(strat->Shdl)+setmaxTinc)
                                                  *sizeof(int));
    strat->S_2_R = (int*) omRealloc0Size(strat->S_2_R,
                                         IDELEMS(strat->Shdl)*sizeof(int),
                                         (IDELEMS(strat->Shdl)+setmaxTinc)
                                                  *sizeof(int));
    if (strat->lenS!=NULL)
      strat->lenS=(int*)omRealloc0Size(strat->lenS,
                                       IDELEMS(strat->Shdl)*sizeof(int),
                                       (IDELEMS(strat->Shdl)+setmaxTinc)
                                                 *sizeof(int));
    if (strat->lenSw!=NULL)
      strat->lenSw=(int*)omRealloc0Size(strat->lenSw,
                                       IDELEMS(strat->Shdl)*sizeof(int),
                                       (IDELEMS(strat->Shdl)+setmaxTinc)
                                                 *sizeof(int));
    if (strat->fromQ!=NULL)
    {
      strat->fromQ = (intset)omReallocSize(strat->fromQ,
                                    IDELEMS(strat->Shdl)*sizeof(int),
                                    (IDELEMS(strat->Shdl)+setmaxTinc)*sizeof(int));
    }
    pEnlargeSet(&strat->S,IDELEMS(strat->Shdl),setmaxTinc);
    IDELEMS(strat->Shdl)+=setmaxTinc;
    strat->Shdl->m=strat->S;
  }
  if (atS <= strat->sl)
  {
#ifdef ENTER_USE_MEMMOVE
// #if 0
    memmove(&(strat->S[atS+1]), &(strat->S[atS]),
            (strat->sl - atS + 1)*sizeof(poly));
    memmove(&(strat->ecartS[atS+1]), &(strat->ecartS[atS]),
            (strat->sl - atS + 1)*sizeof(int));
    memmove(&(strat->sevS[atS+1]), &(strat->sevS[atS]),
            (strat->sl - atS + 1)*sizeof(unsigned long));
    memmove(&(strat->S_2_R[atS+1]), &(strat->S_2_R[atS]),
            (strat->sl - atS + 1)*sizeof(int));
    if (strat->lenS!=NULL)
    memmove(&(strat->lenS[atS+1]), &(strat->lenS[atS]),
            (strat->sl - atS + 1)*sizeof(int));
    if (strat->lenSw!=NULL)
    memmove(&(strat->lenSw[atS+1]), &(strat->lenSw[atS]),
            (strat->sl - atS + 1)*sizeof(int));
#else
    for (i=strat->sl+1; i>=atS+1; i--)
    {
      strat->S[i] = strat->S[i-1];
      strat->ecartS[i] = strat->ecartS[i-1];
      strat->sevS[i] = strat->sevS[i-1];
      strat->S_2_R[i] = strat->S_2_R[i-1];
    }
    if (strat->lenS!=NULL)
    for (i=strat->sl+1; i>=atS+1; i--)
      strat->lenS[i] = strat->lenS[i-1];
    if (strat->lenSw!=NULL)
    for (i=strat->sl+1; i>=atS+1; i--)
      strat->lenSw[i] = strat->lenSw[i-1];
#endif
  }
  if (strat->fromQ!=NULL)
  {
#ifdef ENTER_USE_MEMMOVE
    memmove(&(strat->fromQ[atS+1]), &(strat->fromQ[atS]),
                  (strat->sl - atS + 1)*sizeof(int));
#else
    for (i=strat->sl+1; i>=atS+1; i--)
    {
      strat->fromQ[i] = strat->fromQ[i-1];
    }
#endif
    strat->fromQ[atS]=0;
  }

  /*- save result -*/
  strat->S[atS] = p.p;
  if (strat->honey) strat->ecartS[atS] = p.ecart;
  if (p.sev == 0)
    p.sev = pGetShortExpVector(p.p);
  else
    assume(p.sev == pGetShortExpVector(p.p));
  strat->sevS[atS] = p.sev;
  strat->ecartS[atS] = p.ecart;
  strat->S_2_R[atS] = atR;
  strat->sl++;
}

/*2
* puts p to the set T at position atT
*/
void enterT(LObject p, kStrategy strat, int atT)
{
  int i;

  pp_Test(p.p, currRing, p.tailRing);
  assume(strat->tailRing == p.tailRing);
  // redMoraNF complains about this -- but, we don't really
  // neeed this so far
  // assume(p.pLength == 0 || pLength(p.p) == p.pLength);
  assume(p.FDeg == p.pFDeg());
  assume(!p.is_normalized || nIsOne(pGetCoeff(p.p)));

  strat->newt = TRUE;
  if (atT < 0)
    atT = strat->posInT(strat->T, strat->tl, p);
  if (strat->tl == strat->tmax-1)
    enlargeT(strat->T,strat->R,strat->sevT,strat->tmax,setmaxTinc);
  if (atT <= strat->tl)
  {
#ifdef ENTER_USE_MEMMOVE
    memmove(&(strat->T[atT+1]), &(strat->T[atT]),
            (strat->tl-atT+1)*sizeof(TObject));
    memmove(&(strat->sevT[atT+1]), &(strat->sevT[atT]),
            (strat->tl-atT+1)*sizeof(unsigned long));
#endif
    for (i=strat->tl+1; i>=atT+1; i--)
    {
#ifndef ENTER_USE_MEMMOVE
      strat->T[i] = strat->T[i-1];
      strat->sevT[i] = strat->sevT[i-1];
#endif
      strat->R[strat->T[i].i_r] = &(strat->T[i]);
    }
  }

  if (strat->tailBin != NULL && (pNext(p.p) != NULL))
  {
    pNext(p.p)=p_ShallowCopyDelete(pNext(p.p),
                                   (strat->tailRing != NULL ?
                                    strat->tailRing : currRing),
                                   strat->tailBin);
    if (p.t_p != NULL) pNext(p.t_p) = pNext(p.p);
  }
  strat->T[atT] = (TObject) p;

  if (strat->tailRing != currRing && pNext(p.p) != NULL)
    strat->T[atT].max = p_GetMaxExpP(pNext(p.p), strat->tailRing);
  else
    strat->T[atT].max = NULL;

  strat->tl++;
  strat->R[strat->tl] = &(strat->T[atT]);
  strat->T[atT].i_r = strat->tl;
  assume(p.sev == 0 || pGetShortExpVector(p.p) == p.sev);
  strat->sevT[atT] = (p.sev == 0 ? pGetShortExpVector(p.p) : p.sev);
  kTest_T(&(strat->T[atT]));
}

void initHilbCrit(ideal F, ideal Q, intvec **hilb,kStrategy strat)
{
  if (strat->homog!=isHomog)
  {
    *hilb=NULL;
  }
}

void initBuchMoraCrit(kStrategy strat)
{
  strat->sugarCrit =        TEST_OPT_SUGARCRIT;
  // obachman: Hmm.. I need BTEST1(2) for notBuckets ..
  //  strat->Gebauer =          BTEST1(2) || strat->homog || strat->sugarCrit;
  strat->Gebauer =          strat->homog || strat->sugarCrit;
  strat->honey =            !strat->homog || strat->sugarCrit || TEST_OPT_WEIGHTM;
  if (TEST_OPT_NOT_SUGAR) strat->honey = FALSE;
  strat->pairtest = NULL;
  /* alway use tailreduction, except:
  * - in local rings, - in lex order case, -in ring over extensions */
  strat->noTailReduction = !TEST_OPT_REDTAIL;
#ifdef HAVE_PLURAL
  // and r is plural_ring
  if (currRing->nc!=NULL)
    //or it has non-quasi-comm type... later
  {
    strat->sugarCrit = FALSE;
    strat->Gebauer = FALSE ;
    strat->honey = FALSE;
  }
#endif
  if (TEST_OPT_DEBUG)
  {
    if (strat->homog) PrintS("ideal/module is homogeneous\n");
    else              PrintS("ideal/module is not homogeneous\n");
  }
}

BOOLEAN kPosInLDependsOnLength(int (*pos_in_l)
                               (const LSet set, const int length,
                                LObject* L,const kStrategy strat))
{
  if (pos_in_l == posInL110 ||
      pos_in_l == posInL10)
    return TRUE;

  return FALSE;
}

void initBuchMoraPos (kStrategy strat)
{
  if (pOrdSgn==1)
  {
    if (strat->honey)
    {
      strat->posInL = posInL15;
      // ok -- here is the deal: from my experiments for Singular-2-0
      // I conclude that that posInT_EcartpLength is the best of
      // posInT15, posInT_EcartFDegpLength, posInT_FDegLength, posInT_pLength
      // see the table at the end of this file
      if (K_TEST_OPT_OLDSTD)
        strat->posInT = posInT15;
      else
        strat->posInT = posInT_EcartpLength;
    }
    else if (pLexOrder && !TEST_OPT_INTSTRATEGY)
    {
      strat->posInL = posInL11;
      strat->posInT = posInT11;
    }
    else if (TEST_OPT_INTSTRATEGY)
    {
      strat->posInL = posInL11;
      strat->posInT = posInT11;
    }
    else
    {
      strat->posInL = posInL0;
      strat->posInT = posInT0;
    }
    //if (strat->minim>0) strat->posInL =posInLSpecial;
  }
  else
  {
    if (strat->homog)
    {
      strat->posInL = posInL11;
      strat->posInT = posInT11;
    }
    else
    {
      if ((currRing->order[0]==ringorder_c)
      ||(currRing->order[0]==ringorder_C))
      {
        strat->posInL = posInL17_c;
        strat->posInT = posInT17_c;
      }
      else
      {
        strat->posInL = posInL17;
        strat->posInT = posInT17;
      }
    }
  }
  if (strat->minim>0) strat->posInL =posInLSpecial;
  // for further tests only
  if ((BTEST1(11)) || (BTEST1(12)))
    strat->posInL = posInL11;
  else if ((BTEST1(13)) || (BTEST1(14)))
    strat->posInL = posInL13;
  else if ((BTEST1(15)) || (BTEST1(16)))
    strat->posInL = posInL15;
  else if ((BTEST1(17)) || (BTEST1(18)))
    strat->posInL = posInL17;
  if (BTEST1(11))
    strat->posInT = posInT11;
  else if (BTEST1(13))
    strat->posInT = posInT13;
  else if (BTEST1(15))
    strat->posInT = posInT15;
  else if ((BTEST1(17)))
    strat->posInT = posInT17;
  else if ((BTEST1(19)))
    strat->posInT = posInT19;
  else if (BTEST1(12) || BTEST1(14) || BTEST1(16) || BTEST1(18))
    strat->posInT = posInT1;
  strat->posInLDependsOnLength = kPosInLDependsOnLength(strat->posInL);
}

void initBuchMora (ideal F,ideal Q,kStrategy strat)
{
  strat->interpt = BTEST1(OPT_INTERRUPT);
  strat->kHEdge=NULL;
  if (pOrdSgn==1) strat->kHEdgeFound=FALSE;
  /*- creating temp data structures------------------- -*/
  strat->cp = 0;
  strat->c3 = 0;
  strat->tail = pInit();
  /*- set s -*/
  strat->sl = -1;
  /*- set L -*/
  strat->Lmax = setmaxL;
  strat->Ll = -1;
  strat->L = initL();
  /*- set B -*/
  strat->Bmax = setmaxL;
  strat->Bl = -1;
  strat->B = initL();
  /*- set T -*/
  strat->tl = -1;
  strat->tmax = setmaxT;
  strat->T = initT();
  strat->R = initR();
  strat->sevT = initsevT();
  /*- init local data struct.---------------------------------------- -*/
  strat->P.ecart=0;
  strat->P.length=0;
  if (pOrdSgn==-1)
  {
    if (strat->kHEdge!=NULL) pSetComp(strat->kHEdge, strat->ak);
    if (strat->kNoether!=NULL) pSetComp(strat->kNoetherTail(), strat->ak);
  }
  if(TEST_OPT_SB_1)
  {
    int i;
    ideal P=idInit(IDELEMS(F)-strat->newIdeal,F->rank);
    for (i=strat->newIdeal;i<IDELEMS(F);i++)
    {
      P->m[i-strat->newIdeal] = F->m[i];
      F->m[i] = NULL;
    }
    initSSpecial(F,Q,P,strat);
    for (i=strat->newIdeal;i<IDELEMS(F);i++)
    {
      F->m[i] = P->m[i-strat->newIdeal];
      P->m[i-strat->newIdeal] = NULL;
    }
    idDelete(&P);
  }
  else
  {
    /*Shdl=*/initSL(F, Q,strat); /*sets also S, ecartS, fromQ */
    // /*Shdl=*/initS(F, Q,strat); /*sets also S, ecartS, fromQ */
  }
  strat->kIdeal = NULL;
  strat->fromT = FALSE;
  strat->noTailReduction = !TEST_OPT_REDTAIL;
  if(!TEST_OPT_SB_1)
  {
    updateS(TRUE,strat);
    pairs(strat);
  }
  if (strat->fromQ!=NULL) omFreeSize(strat->fromQ,IDELEMS(strat->Shdl)*sizeof(int));
  strat->fromQ=NULL;
}

void exitBuchMora (kStrategy strat)
{
  /*- release temp data -*/
  cleanT(strat);
  omFreeSize(strat->T,(strat->tmax)*sizeof(TObject));
  omFreeSize(strat->R,(strat->tmax)*sizeof(TObject*));
  omFreeSize(strat->sevT, (strat->tmax)*sizeof(unsigned long));
  omFreeSize(strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  omFreeSize(strat->sevS,IDELEMS(strat->Shdl)*sizeof(int));
  omFreeSize(strat->S_2_R,IDELEMS(strat->Shdl)*sizeof(int));
  /*- set L: should be empty -*/
  omFreeSize(strat->L,(strat->Lmax)*sizeof(LObject));
  /*- set B: should be empty -*/
  omFreeSize(strat->B,(strat->Bmax)*sizeof(LObject));
  pDeleteLm(&strat->tail);
  strat->syzComp=0;
  if (strat->kIdeal!=NULL)
  {
    omFreeBin(strat->kIdeal, sleftv_bin);
    strat->kIdeal=NULL;
  }
}

/*2
* in the case of a standardbase of a module over a qring:
* replace polynomials in i by ak vectors,
* (the polynomial * unit vectors gen(1)..gen(ak)
* in every case (also for ideals:)
* deletes divisible vectors/polynomials
*/
void updateResult(ideal r,ideal Q, kStrategy strat)
{
  int l;
  if (strat->ak>0)
  {
    for (l=IDELEMS(r)-1;l>=0;l--)
    {
      if ((r->m[l]!=NULL) && (pGetComp(r->m[l])==0))
      {
        pDelete(&r->m[l]); // and set it to NULL
      }
    }
    int q;
    poly p;
    for (l=IDELEMS(r)-1;l>=0;l--)
    {
      if ((r->m[l]!=NULL)
      && (strat->syzComp>0)
      && (pGetComp(r->m[l])<=strat->syzComp))
      {
        for(q=IDELEMS(Q)-1; q>=0;q--)
        {
          if ((Q->m[q]!=NULL)
          &&(pLmDivisibleBy(Q->m[q],r->m[l])))
          {
            if (TEST_OPT_REDSB)
            {
              p=r->m[l];
              r->m[l]=kNF(Q,NULL,p);
              pDelete(&p);
            }
            else
            {
              pDelete(&r->m[l]); // and set it to NULL
            }
            break;
          }
        }
      }
    }
  }
  else
  {
    int q;
    poly p;
    for (l=IDELEMS(r)-1;l>=0;l--)
    {
      if (r->m[l]!=NULL)
      {
        for(q=IDELEMS(Q)-1; q>=0;q--)
        {
          if ((Q->m[q]!=NULL)
          &&(pLmEqual(r->m[l],Q->m[q])))
          {
            if (TEST_OPT_REDSB)
            {
              p=r->m[l];
              r->m[l]=kNF(Q,NULL,p);
              pDelete(&p);
            }
            else
            {
              pDelete(&r->m[l]); // and set it to NULL
            }
            break;
          }
        }
      }
    }
  }
  idSkipZeroes(r);
}

void completeReduce (kStrategy strat)
{
  int i;
  int low = (pOrdSgn == 1 ? 1 : 0);
  LObject L;

#ifdef KDEBUG
  // need to set this: during tailreductions of T[i], T[i].max is out of
  // sync
  sloppy_max = TRUE;
#endif

  strat->noTailReduction = FALSE;
  if (TEST_OPT_PROT)
  {
    PrintLn();
    if (timerv) writeTime("standard base computed:");
  }
  if (TEST_OPT_PROT)
  {
    Print("(S:%d)",strat->sl);mflush();
  }
  for (i=strat->sl; i>=low; i--)
  {
    TObject* T_j = strat->s_2_t(i);
    if (T_j != NULL)
    {
      L = *T_j;
      poly p;
      if (pOrdSgn == 1)
        strat->S[i] = redtailBba(&L, i-1, strat, FALSE);
      else
        strat->S[i] = redtail(&L, strat->sl, strat);

      if (strat->redTailChange && strat->tailRing != currRing)
      {
        if (T_j->max != NULL) p_LmFree(T_j->max, strat->tailRing);
        if (pNext(T_j->p) != NULL)
          T_j->max = p_GetMaxExpP(pNext(T_j->p), strat->tailRing);
        else
          T_j->max = NULL;
      }
      if (TEST_OPT_INTSTRATEGY)
        T_j->pCleardenom();
    }
    else
    {
      assume(currRing == strat->tailRing);
      if (pOrdSgn == 1)
        strat->S[i] = redtailBba(strat->S[i], i-1, strat);
      else
        strat->S[i] = redtail(strat->S[i], strat->sl, strat);
      if (TEST_OPT_INTSTRATEGY)
        pCleardenom(strat->S[i]);
    }
    if (TEST_OPT_PROT)
      PrintS("-");
  }
#ifdef KDEBUG
  sloppy_max = FALSE;
#endif
}


/*2
* computes the new strat->kHEdge and the new pNoether,
* returns TRUE, if pNoether has changed
*/
BOOLEAN newHEdge(polyset S, kStrategy strat)
{
  int i,j;
  poly newNoether;

  scComputeHC(strat->Shdl,NULL,strat->ak,strat->kHEdge, strat->tailRing);
  if (strat->t_kHEdge != NULL) p_LmFree(strat->t_kHEdge, strat->tailRing);
  if (strat->tailRing != currRing)
    strat->t_kHEdge = k_LmInit_currRing_2_tailRing(strat->kHEdge, strat->tailRing);
  /* compare old and new noether*/
  newNoether = pLmInit(strat->kHEdge);
  j = pFDeg(newNoether,currRing);
  for (i=1; i<=pVariables; i++)
  {
    if (pGetExp(newNoether, i) > 0) pDecrExp(newNoether,i);
  }
  pSetm(newNoether);
  if (j < strat->HCord) /*- statistics -*/
  {
    if (TEST_OPT_PROT)
    {
      Print("H(%d)",j);
      mflush();
    }
    strat->HCord=j;
    if (TEST_OPT_DEBUG)
    {
      Print("H(%d):",j);
      wrp(strat->kHEdge);
      PrintLn();
    }
  }
  if (pCmp(strat->kNoether,newNoether)!=1)
  {
    pDelete(&strat->kNoether);
    strat->kNoether=newNoether;
    if (strat->t_kNoether != NULL) p_LmFree(strat->t_kNoether, strat->tailRing);
    if (strat->tailRing != currRing)
      strat->t_kNoether = k_LmInit_currRing_2_tailRing(strat->kNoether, strat->tailRing);

    return TRUE;
  }
  pLmFree(newNoether);
  return FALSE;
}

/***************************************************************
 *
 * Routines related for ring changes during std computations
 *
 ***************************************************************/
BOOLEAN kCheckSpolyCreation(LObject *L, kStrategy strat, poly &m1, poly &m2)
{
  assume(L->p1 != NULL && L->p2 != NULL);
  assume(L->i_r1 >= 0 && L->i_r1 <= strat->tl);
  assume(L->i_r2 >= 0 && L->i_r2 <= strat->tl);
  assume(strat->tailRing != currRing);

  if (! k_GetLeadTerms(L->p1, L->p2, currRing, m1, m2, strat->tailRing))
    return FALSE;
  poly p1_max = (strat->R[L->i_r1])->max;
  poly p2_max = (strat->R[L->i_r2])->max;

  if ((p1_max != NULL && !p_LmExpVectorAddIsOk(m1, p1_max, strat->tailRing)) ||
      (p2_max != NULL && !p_LmExpVectorAddIsOk(m2, p2_max, strat->tailRing)))
  {
    p_LmFree(m1, strat->tailRing);
    p_LmFree(m2, strat->tailRing);
    m1 = NULL;
    m2 = NULL;
    return FALSE;
  }
  return TRUE;
}

BOOLEAN kStratChangeTailRing(kStrategy strat, LObject *L, TObject* T, unsigned long expbound)
{
  if (expbound == 0) expbound = strat->tailRing->bitmask << 1;
  if (expbound >= currRing->bitmask) return FALSE;
  ring new_tailRing = rModifyRing(currRing,
                                  // Hmmm .. the condition pFDeg == pDeg
                                  // might be too strong
                                  (strat->homog && pFDeg == pDeg),
                                  !strat->ak,
                                  expbound);
  if (new_tailRing == currRing) return TRUE;

  strat->pOrigFDeg_TailRing = new_tailRing->pFDeg;
  strat->pOrigLDeg_TailRing = new_tailRing->pLDeg;

  if (currRing->pFDeg != currRing->pFDegOrig)
  {
    new_tailRing->pFDeg = currRing->pFDeg;
    new_tailRing->pLDeg = currRing->pLDeg;
  }

  if (TEST_OPT_PROT)
    Print("[%lu:%d", (unsigned long) new_tailRing->bitmask, new_tailRing->ExpL_Size);
  kTest_TS(strat);
  assume(new_tailRing != strat->tailRing);
  pShallowCopyDeleteProc p_shallow_copy_delete
    = pGetShallowCopyDeleteProc(strat->tailRing, new_tailRing);

  omBin new_tailBin = omGetStickyBinOfBin(new_tailRing->PolyBin);

  int i;
  for (i=0; i<=strat->tl; i++)
  {
    strat->T[i].ShallowCopyDelete(new_tailRing, new_tailBin,
                                  p_shallow_copy_delete);
  }
  for (i=0; i<=strat->Ll; i++)
  {
    assume(strat->L[i].p != NULL);
    if (pNext(strat->L[i].p) != strat->tail)
      strat->L[i].ShallowCopyDelete(new_tailRing, p_shallow_copy_delete);
  }
  if (strat->P.t_p != NULL ||
      (strat->P.p != NULL && pNext(strat->P.p) != strat->tail))
    strat->P.ShallowCopyDelete(new_tailRing, p_shallow_copy_delete);

  if (L != NULL && L->tailRing != new_tailRing)
  {
    if (L->i_r < 0)
      L->ShallowCopyDelete(new_tailRing, p_shallow_copy_delete);
    else
    {
      assume(L->i_r <= strat->tl);
      TObject* t_l = strat->R[L->i_r];
      assume(t_l != NULL);
      L->tailRing = new_tailRing;
      L->p = t_l->p;
      L->t_p = t_l->t_p;
      L->max = t_l->max;
    }
  }

  if (T != NULL && T->tailRing != new_tailRing && T->i_r < 0)
    T->ShallowCopyDelete(new_tailRing, new_tailBin, p_shallow_copy_delete);

  omMergeStickyBinIntoBin(strat->tailBin, strat->tailRing->PolyBin);
  if (strat->tailRing != currRing)
    rKillModifiedRing(strat->tailRing);

  strat->tailRing = new_tailRing;
  strat->tailBin = new_tailBin;
  strat->p_shallow_copy_delete
    = pGetShallowCopyDeleteProc(currRing, new_tailRing);

  if (strat->kHEdge != NULL)
  {
    if (strat->t_kHEdge != NULL)
      p_LmFree(strat->t_kHEdge, strat->tailRing);
    strat->t_kHEdge=k_LmInit_currRing_2_tailRing(strat->kHEdge, new_tailRing);
  }

  if (strat->kNoether != NULL)
  {
    if (strat->t_kNoether != NULL)
      p_LmFree(strat->t_kNoether, strat->tailRing);
    strat->t_kNoether=k_LmInit_currRing_2_tailRing(strat->kNoether,
                                                   new_tailRing);
  }
  kTest_TS(strat);
  if (TEST_OPT_PROT)
    PrintS("]");
  return TRUE;
}

void kStratInitChangeTailRing(kStrategy strat)
{
  unsigned long l = 0;
  int i;
  Exponent_t e;
  ring new_tailRing;

  assume(strat->tailRing == currRing);

  for (i=0; i<= strat->Ll; i++)
  {
    l = p_GetMaxExpL(strat->L[i].p, currRing, l);
  }
  for (i=0; i<=strat->tl; i++)
  {
    // Hmm ... this we could do in one Step
    l = p_GetMaxExpL(strat->T[i].p, currRing, l);
  }
  e = p_GetMaxExp(l, currRing);
  if (e <= 1) e = 2;

  kStratChangeTailRing(strat, NULL, NULL, e);
}

skStrategy::skStrategy()
{
  memset(this, 0, sizeof(skStrategy));
  tailRing = currRing;
  P.tailRing = currRing;
  tl = -1;
  sl = -1;
#ifdef HAVE_LM_BIN
  lmBin = omGetStickyBinOfBin(currRing->PolyBin);
#endif
#ifdef HAVE_TAIL_BIN
  tailBin = omGetStickyBinOfBin(currRing->PolyBin);
#endif
  pOrigFDeg = pFDeg;
  pOrigLDeg = pLDeg;
}


skStrategy::~skStrategy()
{
  if (lmBin != NULL)
    omMergeStickyBinIntoBin(lmBin, currRing->PolyBin);
  if (tailBin != NULL)
    omMergeStickyBinIntoBin(tailBin,
                            (tailRing != NULL ? tailRing->PolyBin:
                             currRing->PolyBin));
  if (t_kHEdge != NULL)
    p_LmFree(t_kHEdge, tailRing);
  if (t_kNoether != NULL)
    p_LmFree(t_kNoether, tailRing);

  if (currRing != tailRing)
    rKillModifiedRing(tailRing);
  pRestoreDegProcs(pOrigFDeg, pOrigLDeg);
}

#if 0
Timings for the different possibilities of posInT:
            T15           EDL         DL          EL            L         1-2-3
Gonnet      43.26       42.30       38.34       41.98       38.40      100.04
Hairer_2_1   1.11        1.15        1.04        1.22        1.08        4.7
Twomat3      1.62        1.69        1.70        1.65        1.54       11.32
ahml         4.48        4.03        4.03        4.38        4.96       26.50
c7          15.02       13.98       15.16       13.24       17.31       47.89
c8         505.09      407.46      852.76      413.21      499.19        n/a
f855        12.65        9.27       14.97        8.78       14.23       33.12
gametwo6    11.47       11.35       14.57       11.20       12.02       35.07
gerhard_3    2.73        2.83        2.93        2.64        3.12        6.24
ilias13     22.89       22.46       24.62       20.60       23.34       53.86
noon8       40.68       37.02       37.99       36.82       35.59      877.16
rcyclic_19  48.22       42.29       43.99       45.35       51.51      204.29
rkat9       82.37       79.46       77.20       77.63       82.54      267.92
schwarz_11  16.46       16.81       16.76       16.81       16.72       35.56
test016     16.39       14.17       14.40       13.50       14.26       34.07
test017     34.70       36.01       33.16       35.48       32.75       71.45
test042     10.76       10.99       10.27       11.57       10.45       23.04
test058      6.78        6.75        6.51        6.95        6.22        9.47
test066     10.71       10.94       10.76       10.61       10.56       19.06
test073     10.75       11.11       10.17       10.79        8.63       58.10
test086     12.23       11.81       12.88       12.24       13.37       66.68
test103      5.05        4.80        5.47        4.64        4.89       11.90
test154     12.96       11.64       13.51       12.46       14.61       36.35
test162     65.27       64.01       67.35       59.79       67.54      196.46
test164      7.50        6.50        7.68        6.70        7.96       17.13
virasoro     3.39        3.50        3.35        3.47        3.70        7.66
#endif


#ifdef HAVE_MORE_POS_IN_T
// determines the position based on: 1.) Ecart 2.) FDeg 3.) pLength
int posInT_EcartFDegpLength(const TSet set,const int length,LObject &p)
{

  if (length==-1) return 0;

  int o = p.ecart;
  int op=p.GetpFDeg();
  int ol = p.GetpLength();

  if (set[length].ecart < o)
    return length+1;
  if (set[length].ecart == o)
  {
     int oo=set[length].GetpFDeg();
     if ((oo < op) || ((oo==op) && (set[length].length < ol)))
       return length+1;
  }

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      if (set[an].ecart > o)
        return an;
      if (set[an].ecart == o)
      {
         int oo=set[an].GetpFDeg();
         if((oo > op)
         || ((oo==op) && (set[an].pLength > ol)))
           return an;
      }
      return en;
    }
    i=(an+en) / 2;
    if (set[i].ecart > o)
      en=i;
    else if (set[i].ecart == o)
    {
       int oo=set[i].GetpFDeg();
       if ((oo > op)
       || ((oo == op) && (set[i].pLength > ol)))
         en=i;
       else
        an=i;
    }
    else
      an=i;
  }
}

// determines the position based on: 1.) FDeg 2.) pLength
int posInT_FDegpLength(const TSet set,const int length,LObject &p)
{

  if (length==-1) return 0;

  int op=p.GetpFDeg();
  int ol = p.GetpLength();

  int oo=set[length].GetpFDeg();
  if ((oo < op) || ((oo==op) && (set[length].length < ol)))
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
    {
      if (an >= en-1)
      {
        int oo=set[an].GetpFDeg();
        if((oo > op)
           || ((oo==op) && (set[an].pLength > ol)))
          return an;
        return en;
      }
      i=(an+en) / 2;
      int oo=set[i].GetpFDeg();
      if ((oo > op)
          || ((oo == op) && (set[i].pLength > ol)))
        en=i;
      else
        an=i;
    }
}


// determines the position based on: 1.) Ecart 2.) FDeg 3.) pLength
int posInT_pLength(const TSet set,const int length,LObject &p)
{
  if (length==-1)
    return 0;
  if (set[length].length<p.length)
    return length+1;

  int i;
  int an = 0;
  int en= length;
  int ol = p.GetpLength();

  loop
  {
    if (an >= en-1)
    {
      if (set[an].pLength>ol) return an;
      return en;
    }
    i=(an+en) / 2;
    if (set[i].pLength>ol) en=i;
    else                        an=i;
  }
}

#endif

#endif // KUTIL_CC
