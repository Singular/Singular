/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: kernel: utils for kStd
*/

// #define PDEBUG 2
// #define PDIV_DEBUG
#define KUTIL_CC
#include <stdlib.h>
#include <string.h>
#include <kernel/mod2.h>

#ifndef NDEBUG
# define MYTEST 0
#else /* ifndef NDEBUG */
# define MYTEST 0
#endif /* ifndef NDEBUG */


#include <omalloc/mylimits.h>
#include <kernel/options.h>
#include <kernel/gring.h>
#include <kernel/sca.h>
#ifdef KDEBUG
#undef KDEBUG
#define KDEBUG 2
#endif

#ifdef HAVE_RINGS
#include <kernel/ideals.h>
#endif

// define if enterL, enterT should use memmove instead of doing it manually
// on topgun, this is slightly faster (see monodromy_l.tst, homog_gonnet.sing)
#ifndef SunOS_4
#define ENTER_USE_MEMMOVE
#endif

// define, if the my_memmove inlines should be used instead of
// system memmove -- it does not seem to pay off, though
// #define ENTER_USE_MYMEMMOVE

#include <kernel/kutil.h>
#include <kernel/kbuckets.h>
#include <kernel/febase.h>
#include <omalloc/omalloc.h>
#include <kernel/numbers.h>
#include <kernel/polys.h>
#include <kernel/ring.h>
#include <kernel/ideals.h>
#include <kernel/timer.h>
//#include "cntrlc.h"
#include <kernel/stairc.h>
#include <kernel/kstd1.h>
#include <kernel/pShallowCopyDelete.h>

/* shiftgb stuff */
#include <kernel/shiftgb.h>
#include <kernel/prCopy.h>

#ifdef HAVE_RATGRING
#include <kernel/ratgring.h>
#endif

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

#ifdef HAVE_RINGS
#define pDivComp_EQUAL 2
#define pDivComp_LESS 1
#define pDivComp_GREATER -1
#define pDivComp_INCOMP 0
/* Checks the relation of LM(p) and LM(q)
     LM(p) = LM(q) => return pDivComp_EQUAL
     LM(p) | LM(q) => return pDivComp_LESS
     LM(q) | LM(p) => return pDivComp_GREATER
     else return pDivComp_INCOMP */
static inline int pDivCompRing(poly p, poly q)
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
          if (b) return pDivComp_INCOMP;
          if (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask))
            return pDivComp_INCOMP;
          a = TRUE;
        }
        else
        {
          if (a) return pDivComp_INCOMP;
          if (((la & divmask) ^ (lb & divmask)) != ((la - lb) & divmask))
            return pDivComp_INCOMP;
          b = TRUE;
        }
      }
    }
    if (a) return pDivComp_LESS;
    if (b) return pDivComp_GREATER;
    if (!a & !b) return pDivComp_EQUAL;
  }
  return pDivComp_INCOMP;
}
#endif

static inline int pDivComp(poly p, poly q)
{
  if (pGetComp(p) == pGetComp(q))
  {
#ifdef HAVE_RATGRING
    if (rIsRatGRing(currRing))
    {
      if (_p_LmDivisibleByPart(p,currRing,
                           q,currRing,
                           currRing->real_var_start, currRing->real_var_end))
        return 0;
      return pLmCmp(q,p); // ONLY FOR GLOBAL ORDER!
    }
#endif
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
    if (a) { /*assume(pLmCmp(q,p)==1);*/ return 1; }
    if (b) { /*assume(pLmCmp(q,p)==-1);*/return -1; }
    /*assume(pLmCmp(q,p)==0);*/
  }
  return 0;
}


int     HCord;
int     Kstd1_deg;
int     Kstd1_mu=32000;

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
        //if (L->pLength != 0)
        L->pLength = l;
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
void cancelunit (LObject* L,BOOLEAN inNF)
{
  int  i;
  poly h;

  if(rHasGlobalOrdering_currRing()) return;
  if(TEST_OPT_CANCELUNIT) return;

  ring r = L->tailRing;
  poly p = L->GetLmTailRing();

#ifdef HAVE_RINGS_LOC
  // Leading coef have to be a unit
  if ( !(nIsUnit(p_GetCoeff(p, r))) ) return;
#endif

  if(p_GetComp(p, r) != 0 && !p_OneComp(p, r)) return;

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
      if (!inNF)
      {
        number eins=nInit(1);
        if (L->p != NULL)  pSetCoeff(L->p,eins);
        else if (L->t_p != NULL) nDelete(&pGetCoeff(L->t_p));
        if (L->t_p != NULL) pSetCoeff0(L->t_p,eins);
      }
      L->ecart = 0;
      L->length = 1;
      //if (L->pLength > 0)
      L->pLength = 1;
      if (L->last != NULL) L->last = p;
      L->max = NULL;

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
    {
      p_LmFree(strat->T[j].max, strat->tailRing);
    }
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

//LSet initL ()
//{
//  int i;
//  LSet l = (LSet)omAlloc(setmaxL*sizeof(LObject));
//  return l;
//}

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

  if (i >= 0 && T->pLength != 0 
  && ! rIsSyzIndexRing(currRing) && T->pLength != pLength(p))
  {
    int l=T->pLength;
    T->pLength=pLength(p);
    return dReportError("%c[%d] pLength error: has %d, specified to have %d",
                        TN, i , pLength(p), l);
  }

  // check FDeg,  for elements in L and T
  if (i >= 0 && (TN == 'T' || TN == 'L'))
  {
    // FDeg has ir element from T of L set
    if (T->FDeg  != T->pFDeg())
    {
      int d=T->FDeg;
      T->FDeg=T->pFDeg();
      return dReportError("%c[%d] FDeg error: has %d, specified to have %d",
                          TN, i , T->pFDeg(), d);
    }
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
      // may be unused
      //if (strat->use_buckets && strat->L[i].Next() != strat->tail &&
      //    strat->L[i].Next() != NULL && strat->L[i].p1 != NULL)
      //{
      //  assume(strat->L[i].bucket != NULL);
      //}
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
    memmove(&(strat->lenSw[i]),&(strat->lenSw[i+1]),(strat->sl - i)*sizeof(wlen_type));
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
  {
#ifdef HAVE_RINGS
    if (pGetCoeff(set[j].lcm) != NULL)
      pLmDelete(set[j].lcm);
    else
#endif
      pLmFree(set[j].lcm);
  }
  if (set[j].p!=NULL)
  {
    if (pNext(set[j].p) == strat->tail)
    {
#ifdef HAVE_RINGS
      if (pGetCoeff(set[j].p) != NULL)
        pLmDelete(set[j].p);
      else
#endif
        pLmFree(set[j].p);
      /*- tail belongs to several int spolys -*/
    }
    else
    {
      // search p in T, if it is there, do not delete it
      if (pOrdSgn != -1 || kFindInT(set[j].p, strat) < 0)
      {
        // assure that for global orderings kFindInT fails
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
*enters p at position at in L
*/
void enterL (LSet *set,int *length, int *LSetmax, LObject p,int at)
{
#ifdef PDEBUG
  /*  zaehler++; */
#endif /*PDEBUG*/
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
  // h->length is set by h->pLDeg
  h->length=h->pLength=pLength(h->p);
}

void initEcartBBA (LObject* h)
{
  h->FDeg = h->pFDeg();
  (*h).ecart = 0;
  h->length=h->pLength=pLength(h->p);
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
static inline BOOLEAN sugarDivisibleBy(int ecart1, int ecart2)
{
  return (ecart1 <= ecart2);
}

#ifdef HAVE_RINGS
/*2
* put the pair (s[i],p)  into the set B, ecart=ecart(p) (ring case)
*/
void enterOnePairRing (int i,poly p,int ecart, int isFromQ,kStrategy strat, int atR = -1)
{
  assume(i<=strat->sl);
  int      l,j,compare,compareCoeff;
  LObject  Lp;

  if (strat->interred_flag) return;
#ifdef KDEBUG
  Lp.ecart=0; Lp.length=0;
#endif
  /*- computes the lcm(s[i],p) -*/
  Lp.lcm = pInit();
  pSetCoeff0(Lp.lcm, nLcm(pGetCoeff(p), pGetCoeff(strat->S[i]), currRing));
  // Lp.lcm == 0
  if (nIsZero(pGetCoeff(Lp.lcm)))
  {
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS("--- Lp.lcm == 0\n");
        PrintS("p:");
        wrp(p);
        Print("  strat->S[%d]:", i);
        wrp(strat->S[i]);
        PrintLn();
      }
#endif
      strat->cp++;
      pLmDelete(Lp.lcm);
      return;
  }
  // basic product criterion
  pLcm(p,strat->S[i],Lp.lcm);
  pSetm(Lp.lcm);
  assume(!strat->sugarCrit);
  if (pHasNotCF(p,strat->S[i]) && nIsUnit(pGetCoeff(p)) && nIsUnit(pGetCoeff(strat->S[i])))
  {
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS("--- product criterion func enterOnePairRing type 1\n");
        PrintS("p:");
        wrp(p);
        Print("  strat->S[%d]:", i);
        wrp(strat->S[i]);
        PrintLn();
      }
#endif
      strat->cp++;
      pLmDelete(Lp.lcm);
      return;
  }
  assume(!strat->fromT);
  /*
  *the set B collects the pairs of type (S[j],p)
  *suppose (r,p) is in B and (s,p) is the new pair and lcm(s,p) != lcm(r,p)
  *if the leading term of s devides lcm(r,p) then (r,p) will be canceled
  *if the leading term of r devides lcm(s,p) then (s,p) will not enter B
  */
  for(j = strat->Bl;j>=0;j--)
  {
    compare=pDivCompRing(strat->B[j].lcm,Lp.lcm);
    compareCoeff = nDivComp(pGetCoeff(strat->B[j].lcm), pGetCoeff(Lp.lcm));
    if (compareCoeff == pDivComp_EQUAL || compare == compareCoeff)
    {
      if (compare == 1)
      {
        strat->c3++;
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
        {
          PrintS("--- chain criterion type 1\n");
          PrintS("strat->B[j]:");
          wrp(strat->B[j].lcm);
          PrintS("  Lp.lcm:");
          wrp(Lp.lcm);
          PrintLn();
        }
#endif
        if ((strat->fromQ==NULL) || (isFromQ==0) || (strat->fromQ[i]==0))
        {
          pLmDelete(Lp.lcm);
          return;
        }
        break;
      }
      else
      if (compare == -1)
      {
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
        {
          PrintS("--- chain criterion type 2\n");
          Print("strat->B[%d].lcm:",j);
          wrp(strat->B[j].lcm);
          PrintS("  Lp.lcm:");
          wrp(Lp.lcm);
          PrintLn();
        }
#endif
        deleteInL(strat->B,&strat->Bl,j,strat);
        strat->c3++;
      }
    }
    if ((compare == pDivComp_EQUAL) && (compareCoeff != 2))
    {
      if (compareCoeff == pDivComp_LESS)
      {
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
        {
          PrintS("--- chain criterion type 3\n");
          Print("strat->B[%d].lcm:", j);
          wrp(strat->B[j].lcm);
          PrintS("  Lp.lcm:");
          wrp(Lp.lcm);
          PrintLn();
        }
#endif
        strat->c3++;
        if ((strat->fromQ==NULL) || (isFromQ==0) || (strat->fromQ[i]==0))
        {
          pLmDelete(Lp.lcm);
          return;
        }
        break;
      }
      else
      // Add hint for same LM and LC (later) (TODO Oliver)
      // if (compareCoeff == pDivComp_GREATER)
      {
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
        {
          PrintS("--- chain criterion type 4\n");
          Print("strat->B[%d].lcm:", j);
          wrp(strat->B[j].lcm);
          PrintS("  Lp.lcm:");
          wrp(Lp.lcm);
          PrintLn();
        }
#endif
        deleteInL(strat->B,&strat->Bl,j,strat);
        strat->c3++;
      }
    }
  }
  /*
  *the pair (S[i],p) enters B if the spoly != 0
  */
  /*-  compute the short s-polynomial -*/
  if ((strat->S[i]==NULL) || (p==NULL)) {
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("--- spoly = NULL\n");
    }
#endif
    pLmDelete(Lp.lcm);
    return;
  }
  if ((strat->fromQ!=NULL) && (isFromQ!=0) && (strat->fromQ[i]!=0))
  {
    // Is from a previous computed GB, therefore we know that spoly will
    // reduce to zero. Oliver.
    WarnS("Could we come here? 8738947389");
    Lp.p=NULL;
  }
  else
  {
    Lp.p = ksCreateShortSpoly(strat->S[i], p, strat->tailRing);
  }
  if (Lp.p == NULL)
  {
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("--- spoly = NULL\n");
    }
#endif
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
    pLmDelete(Lp.lcm);
  }
  else
  {
    /*- the pair (S[i],p) enters B -*/
    Lp.p1 = strat->S[i];
    Lp.p2 = p;

    pNext(Lp.p) = strat->tail;

    if (atR >= 0)
    {
      Lp.i_r2 = atR;
      Lp.i_r1 = strat->S_2_R[i];
    }
    strat->initEcartPair(&Lp,strat->S[i],p,strat->ecartS[i],ecart);
    l = strat->posInL(strat->B,strat->Bl,&Lp,strat);
    enterL(&strat->B,&strat->Bl,&strat->Bmax,Lp,l);
  }
}


/*2
* put the  lcm(s[i],p)  into the set B
*/

BOOLEAN enterOneStrongPoly (int i,poly p,int ecart, int isFromQ,kStrategy strat, int atR = -1)
{
  number d, s, t;
  assume(i<=strat->sl);
  assume(atR >= 0);
  poly m1, m2, gcd;

  d = nExtGcd(pGetCoeff(p), pGetCoeff(strat->S[i]), &s, &t);

  if (nIsZero(s) || nIsZero(t))  // evtl. durch divBy tests ersetzen
  {
    nDelete(&d);
    nDelete(&s);
    nDelete(&t);
    return FALSE;
  }

  k_GetStrongLeadTerms(p, strat->S[i], currRing, m1, m2, gcd, strat->tailRing);
  //p_Test(m1,strat->tailRing);
  //p_Test(m2,strat->tailRing);
  while (! kCheckStrongCreation(atR, m1, i, m2, strat) )
  {
    memset(&(strat->P), 0, sizeof(strat->P));
    kStratChangeTailRing(strat);
    strat->P = *(strat->R[atR]);
    p_LmFree(m1, strat->tailRing);
    p_LmFree(m2, strat->tailRing);
    p_LmFree(gcd, currRing);
    k_GetStrongLeadTerms(p, strat->S[i], currRing, m1, m2, gcd, strat->tailRing);
  }
  pSetCoeff0(m1, s);
  pSetCoeff0(m2, t);
  pSetCoeff0(gcd, d);
  p_Test(m1,strat->tailRing);
  p_Test(m2,strat->tailRing);

#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    // Print("t = %d; s = %d; d = %d\n", nInt(t), nInt(s), nInt(d));
    PrintS("m1 = ");
    p_wrp(m1, strat->tailRing);
    PrintS(" ; m2 = ");
    p_wrp(m2, strat->tailRing);
    PrintS(" ; gcd = ");
    wrp(gcd);
    PrintS("\n--- create strong gcd poly: ");
    Print("\n p: ", i);
    wrp(p);
    Print("\n strat->S[%d]: ", i);
    wrp(strat->S[i]);
    PrintS(" ---> ");
  }
#endif

  pNext(gcd) = p_Add_q(pp_Mult_mm(pNext(p), m1, strat->tailRing), pp_Mult_mm(pNext(strat->S[i]), m2, strat->tailRing), strat->tailRing);
  p_LmDelete(m1, strat->tailRing);
  p_LmDelete(m2, strat->tailRing);

#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    wrp(gcd);
    PrintLn();
  }
#endif

  LObject h;
  h.p = gcd;
  h.tailRing = strat->tailRing;
  int posx;
  h.pCleardenom();
  strat->initEcart(&h);
  if (strat->Ll==-1)
    posx =0;
  else
    posx = strat->posInL(strat->L,strat->Ll,&h,strat);
  h.sev = pGetShortExpVector(h.p);
  if (currRing!=strat->tailRing)
    h.t_p = k_LmInit_currRing_2_tailRing(h.p, strat->tailRing);
  enterL(&strat->L,&strat->Ll,&strat->Lmax,h,posx);
  return TRUE;
}
#endif

/*2
* put the pair (s[i],p)  into the set B, ecart=ecart(p)
*/

void enterOnePairNormal (int i,poly p,int ecart, int isFromQ,kStrategy strat, int atR = -1)
{
  assume(i<=strat->sl);
  if (strat->interred_flag) return;

  int      l,j,compare;
  LObject  Lp;
  Lp.i_r = -1;

#ifdef KDEBUG
  Lp.ecart=0; Lp.length=0;
#endif
  /*- computes the lcm(s[i],p) -*/
  Lp.lcm = pInit();

#ifndef HAVE_RATGRING
  pLcm(p,strat->S[i],Lp.lcm);
#elif defined(HAVE_RATGRING)
  //  if (rIsRatGRing(currRing))
  pLcmRat(p,strat->S[i],Lp.lcm, currRing->real_var_start); // int rat_shift
#endif
  pSetm(Lp.lcm);


  if (strat->sugarCrit && ALLOW_PROD_CRIT(strat))
  {
    if((!((strat->ecartS[i]>0)&&(ecart>0)))
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
    if (ALLOW_PROD_CRIT(strat))
    {
      // if currRing->nc_type!=quasi (or skew)
      // TODO: enable productCrit for super commutative algebras...
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
          strat->cp++;
          pLmFree(Lp.lcm);
          Lp.lcm=NULL;
          return;
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
  }
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
    if ( rIsPluralRing(currRing) )
    {
      if(pHasNotCF(p, strat->S[i]))
      {
         if(ncRingType(currRing) == nc_lie)
         {
             // generalized prod-crit for lie-type
             strat->cp++;
             Lp.p = nc_p_Bracket_qq(pCopy(p),strat->S[i]);
         }
         else
        if( ALLOW_PROD_CRIT(strat) )
        {
            // product criterion for homogeneous case in SCA
            strat->cp++;
            Lp.p = NULL;
        }
        else
        {
          Lp.p = // nc_CreateSpoly(strat->S[i],p,currRing);
                nc_CreateShortSpoly(strat->S[i], p, currRing);

          assume(pNext(Lp.p)==NULL); // TODO: this may be violated whenever ext.prod.crit. for Lie alg. is used
          pNext(Lp.p) = strat->tail; // !!!
        }
      }
      else
      {
        Lp.p = // nc_CreateSpoly(strat->S[i],p,currRing);
              nc_CreateShortSpoly(strat->S[i], p, currRing);

        assume(pNext(Lp.p)==NULL); // TODO: this may be violated whenever ext.prod.crit. for Lie alg. is used
        pNext(Lp.p) = strat->tail; // !!!

      }


#if MYTEST
      if (TEST_OPT_DEBUG)
      {
        PrintS("enterOnePairNormal::\n strat->S[i]: "); pWrite(strat->S[i]);
        PrintS("p: "); pWrite(p);
        PrintS("SPoly: "); pWrite(Lp.p);
      }
#endif

    }
    else
    #endif
    {
      assume(!rIsPluralRing(currRing));
      Lp.p = ksCreateShortSpoly(strat->S[i], p, strat->tailRing);
#if MYTEST
      if (TEST_OPT_DEBUG)
      {
        PrintS("enterOnePairNormal::\n strat->S[i]: "); pWrite(strat->S[i]);
        PrintS("p: "); pWrite(p);
        PrintS("commutative SPoly: "); pWrite(Lp.p);
      }
#endif

      }
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

    if (
        (!rIsPluralRing(currRing))
//      ||  (rIsPluralRing(currRing) && (ncRingType(currRing) != nc_lie))
       )
    {
      assume(pNext(Lp.p)==NULL); // TODO: this may be violated whenever ext.prod.crit. for Lie alg. is used
      pNext(Lp.p) = strat->tail; // !!!
    }

    if (atR >= 0)
    {
      Lp.i_r1 = strat->S_2_R[i];
      Lp.i_r2 = atR;
    }
    else
    {
      Lp.i_r1 = -1;
      Lp.i_r2 = -1;
    }
    strat->initEcartPair(&Lp,strat->S[i],p,strat->ecartS[i],ecart);

    if (TEST_OPT_INTSTRATEGY)
    {
      if (!rIsPluralRing(currRing))
        nDelete(&(Lp.p->coef));
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
    if(ALLOW_PROD_CRIT(strat))
    {
      //PrintS("prod-crit\n");
      strat->cp++;
      return;
    }
  }

  int      l,j,compare;
  LObject  Lp;
  Lp.i_r = -1;

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

  #ifdef HAVE_PLURAL
  if (rIsPluralRing(currRing))
  {
    Lp.p = nc_CreateShortSpoly(strat->S[i],p); // ??? strat->tailRing?
  }
  else
  #endif
    Lp.p = ksCreateShortSpoly(strat->S[i],p,strat->tailRing);

  if (Lp.p == NULL)
  {
     //PrintS("short spoly==NULL\n");
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
    else
    {
      Lp.i_r1 = -1;
      Lp.i_r2 = -1;
    }
    assume(pNext(Lp.p) == NULL);
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
* merge set B into L
*/
void kMergeBintoL(kStrategy strat)
{
  int j=strat->Ll+strat->Bl+1;
  if (j>strat->Lmax)
  {
    j=((j+setmaxLinc-1)/setmaxLinc)*setmaxLinc;
    strat->L = (LSet)omReallocSize(strat->L,strat->Lmax*sizeof(LObject),
                                 j*sizeof(LObject));
    strat->Lmax=j;
  }
  j = strat->Ll;
  int i;
  for (i=strat->Bl; i>=0; i--)
  {
    j = strat->posInL(strat->L,j,&(strat->B[i]),strat);
    enterL(&strat->L,&strat->Ll,&strat->Lmax,strat->B[i],j);
  }
  strat->Bl = -1;
}
/*2
*the pairset B of pairs of type (s[i],p) is complete now. It will be updated
*using the chain-criterion in B and L and enters B to L
*/
void chainCritNormal (poly p,int ecart,kStrategy strat)
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
    *the elements of B enter L
    */
    kMergeBintoL(strat);
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
    kMergeBintoL(strat);
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
#ifdef HAVE_RATGRING
void chainCritPart (poly p,int ecart,kStrategy strat)
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
            if (_p_LmDivisibleByPart(strat->S[j],currRing,
               strat->B[i].lcm,currRing,
               currRing->real_var_start,currRing->real_var_end))
            {
              if(TEST_OPT_DEBUG)
              {
                 Print("chain-crit-part: S[%d]=",j);
                 p_wrp(strat->S[j],currRing);
                 Print(" divide B[%d].lcm=",i);
                 p_wrp(strat->B[i].lcm,currRing);
                 PrintLn();
              }
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
        && pCompareChainPart(p,strat->L[j].p1,strat->L[j].p2,strat->L[j].lcm))
        {
          if (strat->L[j].p == strat->tail)
          {
              if(TEST_OPT_DEBUG)
              {
                 PrintS("chain-crit-part: pCompareChainPart p=");
                 p_wrp(p,currRing);
                 Print(" delete L[%d]",j);
                 p_wrp(strat->L[j].lcm,currRing);
                 PrintLn();
              }
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
              if(TEST_OPT_DEBUG)
              {
                 Print("chain-crit-part: sugar B[%d].lcm=",j);
                 p_wrp(strat->B[j].lcm,currRing);
                 Print(" delete B[%d]",i);
                 p_wrp(strat->B[i].lcm,currRing);
                 PrintLn();
              }
              deleteInL(strat->B,&strat->Bl,i,strat);
              j--;
            }
            else
            {
              if(TEST_OPT_DEBUG)
              {
                 Print("chain-crit-part: sugar B[%d].lcm=",i);
                 p_wrp(strat->B[i].lcm,currRing);
                 Print(" delete B[%d]",j);
                 p_wrp(strat->B[j].lcm,currRing);
                 PrintLn();
              }
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
        if (pCompareChainPart(p,strat->L[j].p1,strat->L[j].p2,strat->L[j].lcm))
        {
          if ((pNext(strat->L[j].p) == strat->tail)||(pOrdSgn==1))
          {
              if(TEST_OPT_DEBUG)
              {
                 PrintS("chain-crit-part: sugar:pCompareChainPart p=");
                 p_wrp(p,currRing);
                 Print(" delete L[%d]",j);
                 p_wrp(strat->L[j].lcm,currRing);
                 PrintLn();
              }
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
              if(TEST_OPT_DEBUG)
              {
                 Print("chain-crit-part: equal lcm B[%d].lcm=",j);
                 p_wrp(strat->B[j].lcm,currRing);
                 Print(" delete B[%d]\n",i);
              }
            strat->c3++;
            deleteInL(strat->B,&strat->Bl,i,strat);
            j--;
          }
        }
        j--;
      }
    }
    /*
    *the elements of B enter L
    */
    kMergeBintoL(strat);
  }
  else
  {
    for (j=strat->Ll; j>=0; j--)
    {
      if (pCompareChainPart(p,strat->L[j].p1,strat->L[j].p2,strat->L[j].lcm))
      {
        if ((pNext(strat->L[j].p) == strat->tail)||(pOrdSgn==1))
        {
              if(TEST_OPT_DEBUG)
              {
                 PrintS("chain-crit-part: pCompareChainPart p=");
                 p_wrp(p,currRing);
                 Print(" delete L[%d]",j);
                 p_wrp(strat->L[j].lcm,currRing);
                 PrintLn();
              }
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
    kMergeBintoL(strat);
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
            && _p_LmDivisibleByPart(p,currRing,
                           strat->L[l].lcm,currRing,
                           currRing->real_var_start, currRing->real_var_end))

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
              if(TEST_OPT_DEBUG)
              {
                 PrintS("chain-crit-part: divisible_by p=");
                 p_wrp(p,currRing);
                 Print(" delete L[%d]",l);
                 p_wrp(strat->L[l].lcm,currRing);
                 PrintLn();
              }
              deleteInL(strat->L,&strat->Ll,l,strat);
              i--;
            }
            else
            {
              if(TEST_OPT_DEBUG)
              {
                 PrintS("chain-crit-part: divisible_by(2) p=");
                 p_wrp(p,currRing);
                 Print(" delete L[%d]",i);
                 p_wrp(strat->L[i].lcm,currRing);
                 PrintLn();
              }
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
#endif

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
            strat->enterOnePair(j,h,ecart,isFromQ,strat, atR);
          //Print("j:%d, Ll:%d\n",j,strat->Ll);
          }
        }
      }
      else
      {
        new_pair=TRUE;
        for (j=0; j<=k; j++)
        {
          strat->enterOnePair(j,h,ecart,isFromQ,strat, atR);
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
          strat->enterOnePair(j,h,ecart,isFromQ,strat, atR);
        //Print("j:%d, Ll:%d\n",j,strat->Ll);
        }
      }
    }

    if (new_pair)
    {
#ifdef HAVE_RATGRING
      if (currRing->real_var_start>0)
        chainCritPart(h,ecart,strat);
      else
#endif
      strat->chainCrit(h,ecart,strat);
    }
  }
}

#ifdef HAVE_RINGS
/*2
*the pairset B of pairs of type (s[i],p) is complete now. It will be updated
*using the chain-criterion in B and L and enters B to L
*/
void chainCritRing (poly p,int ecart,kStrategy strat)
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
#ifdef KDEBUG
              if (TEST_OPT_DEBUG)
              {
                PrintS("--- chain criterion func chainCritRing type 1\n");
                PrintS("strat->S[j]:");
                wrp(strat->S[j]);
                PrintS("  strat->B[i].lcm:");
                wrp(strat->B[i].lcm);
                PrintLn();
              }
#endif
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
  assume(!(strat->Gebauer || strat->fromT));
  for (j=strat->Ll; j>=0; j--)
  {
    if (strat->L[j].lcm != NULL && nDivBy(pGetCoeff(strat->L[j].lcm), pGetCoeff(p)))
    {
      if (pCompareChain(p,strat->L[j].p1,strat->L[j].p2,strat->L[j].lcm))
      {
        if ((pNext(strat->L[j].p) == strat->tail) || (pOrdSgn==1))
        {
          deleteInL(strat->L,&strat->Ll,j,strat);
          strat->c3++;
#ifdef KDEBUG
              if (TEST_OPT_DEBUG)
              {
                PrintS("--- chain criterion func chainCritRing type 2\n");
                PrintS("strat->L[j].p:");
                wrp(strat->L[j].p);
                PrintS("  p:");
                wrp(p);
                PrintLn();
              }
#endif
        }
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
  kMergeBintoL(strat);
  j = strat->Ll;
  loop  /*cannot be changed into a for !!! */
  {
    if (j <= 0)
    {
      /*now L[0] cannot be canceled any more and the tail can be removed*/
      if (strat->L[0].p2 == strat->tail) strat->L[0].p2 = p;
      break;
    }
    if (strat->L[j].p2 == p) // Was the element added from B?
    {
      i = j-1;
      loop
      {
        if (i < 0)  break;
        // Element is from B and has the same lcm as L[j]
        if ((strat->L[i].p2 == p) && nDivBy(pGetCoeff(strat->L[j].lcm), pGetCoeff(strat->L[i].lcm))
             && pLmEqual(strat->L[j].lcm,strat->L[i].lcm))
        {
          /*L[i] could be canceled but we search for a better one to cancel*/
          strat->c3++;
#ifdef KDEBUG
          if (TEST_OPT_DEBUG)
          {
            PrintS("--- chain criterion func chainCritRing type 3\n");
            PrintS("strat->L[j].lcm:");
            wrp(strat->L[j].lcm);
            PrintS("  strat->L[i].lcm:");
            wrp(strat->L[i].lcm);
            PrintLn();
          }
#endif
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
#endif

#ifdef HAVE_RINGS
long ind2(long arg)
{
  long ind = 0;
  if (arg <= 0) return 0;
  while (arg%2 == 0)
  {
    arg = arg / 2;
    ind++;
  }
  return ind;
}

long ind_fact_2(long arg)
{
  long ind = 0;
  if (arg <= 0) return 0;
  if (arg%2 == 1) { arg--; }
  while (arg > 0)
  {
    ind += ind2(arg);
    arg = arg - 2;
  }
  return ind;
}
#endif

#ifdef HAVE_VANIDEAL
long twoPow(long arg)
{
  return 1L << arg;
}

/*2
* put the pair (p, f) in B and f in T
*/
void enterOneZeroPairRing (poly f, poly t_p, poly p, int ecart, kStrategy strat, int atR = -1)
{
  int      l,j,compare,compareCoeff;
  LObject  Lp;

  if (strat->interred_flag) return;
#ifdef KDEBUG
  Lp.ecart=0; Lp.length=0;
#endif
  /*- computes the lcm(s[i],p) -*/
  Lp.lcm = pInit();

  pLcm(p,f,Lp.lcm);
  pSetm(Lp.lcm);
  pSetCoeff(Lp.lcm, nLcm(pGetCoeff(p), pGetCoeff(f), currRing));
  assume(!strat->sugarCrit);
  assume(!strat->fromT);
  /*
  *the set B collects the pairs of type (S[j],p)
  *suppose (r,p) is in B and (s,p) is the new pair and lcm(s,p) != lcm(r,p)
  *if the leading term of s devides lcm(r,p) then (r,p) will be canceled
  *if the leading term of r devides lcm(s,p) then (s,p) will not enter B
  */
  for(j = strat->Bl;j>=0;j--)
  {
    compare=pDivCompRing(strat->B[j].lcm,Lp.lcm);
    compareCoeff = nDivComp(pGetCoeff(strat->B[j].lcm), pGetCoeff(Lp.lcm));
    if (compareCoeff == 0 || compare == compareCoeff)
    {
      if (compare == 1)
      {
        strat->c3++;
        pLmDelete(Lp.lcm);
        return;
      }
      else
      if (compare == -1)
      {
        deleteInL(strat->B,&strat->Bl,j,strat);
        strat->c3++;
      }
    }
    if (compare == pDivComp_EQUAL)
    {
      // Add hint for same LM and direction of LC (later) (TODO Oliver)
      if (compareCoeff == 1)
      {
        strat->c3++;
        pLmDelete(Lp.lcm);
        return;
      }
      else
      if (compareCoeff == -1)
      {
        deleteInL(strat->B,&strat->Bl,j,strat);
        strat->c3++;
      }
    }
  }
  /*
  *the pair (S[i],p) enters B if the spoly != 0
  */
  /*-  compute the short s-polynomial -*/
  if ((f==NULL) || (p==NULL)) return;
  pNorm(p);
  {
    Lp.p = ksCreateShortSpoly(f, p, strat->tailRing);
  }
  if (Lp.p == NULL) //deactivated, as we are adding pairs with zeropoly and not from S
  {
    /*- the case that the s-poly is 0 -*/
//    if (strat->pairtest==NULL) initPairtest(strat);
//    strat->pairtest[i] = TRUE;/*- hint for spoly(S^[i],p)=0 -*/
//    strat->pairtest[strat->sl+1] = TRUE;
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
    if (Lp.lcm!=NULL) pLmDelete(Lp.lcm);
  }
  else
  {
    /*- the pair (S[i],p) enters B -*/
    Lp.p1 = f;
    Lp.p2 = p;

    pNext(Lp.p) = strat->tail;

    LObject tmp_h(f, currRing, strat->tailRing);
    tmp_h.SetShortExpVector();
    strat->initEcart(&tmp_h);
    tmp_h.sev = pGetShortExpVector(tmp_h.p);
    tmp_h.t_p = t_p;

    enterT(tmp_h, strat, strat->tl + 1);

    if (atR >= 0)
    {
      Lp.i_r2 = atR;
      Lp.i_r1 = strat->tl;
    }

    strat->initEcartPair(&Lp,f,p,0/*strat->ecartS[i]*/,ecart);     // Attention: TODO: break ecart
    l = strat->posInL(strat->B,strat->Bl,&Lp,strat);
    enterL(&strat->B, &strat->Bl, &strat->Bmax, Lp, l);
  }
}

/* Helper for kCreateZeroPoly
 * enumerating the exponents
ring r = 2^2, (a, b, c), lp; ideal G0 = system("createG0"); ideal G = interred(G0); ncols(G0); ncols(G);
 */

int nextZeroSimplexExponent (long exp[], long ind[], long cexp[], long cind[], long* cabsind, long step[], long bound, long N)
/* gives the next exponent from the set H_1 */
{
  long add = ind2(cexp[1] + 2);
  if ((*cabsind < bound) && (*cabsind - step[1] + add < bound))
  {
    cexp[1] += 2;
    cind[1] += add;
    *cabsind += add;
  }
  else
  {
    // cabsind >= habsind
    if (N == 1) return 0;
    int i = 1;
    while (exp[i] == cexp[i] && i <= N) i++;
    cexp[i] = exp[i];
    *cabsind -= cind[i];
    cind[i] = ind[i];
    step[i] = 500000;
    *cabsind += cind[i];
    // Print("in: %d\n", *cabsind);
    i += 1;
    if (i > N) return 0;
    do
    {
      step[1] = 500000;
      for (int j = i + 1; j <= N; j++)
      {
        if (step[1] > step[j]) step[1] = step[j];
      }
      add = ind2(cexp[i] + 2);
      if (*cabsind - step[1] + add >= bound)
      {
        cexp[i] = exp[i];
        *cabsind -= cind[i];
        cind[i] = ind[i];
        *cabsind += cind[i];
        step[i] = 500000;
        i += 1;
        if (i > N) return 0;
      }
      else step[1] = -1;
    } while (step[1] != -1);
    step[1] = 500000;
    cexp[i] += 2;
    cind[i] += add;
    *cabsind += add;
    if (add < step[i]) step[i] = add;
    for (i = 2; i <= N; i++)
    {
      if (step[1] > step[i]) step[1] = step[i];
    }
  }
  return 1;
}

/*
 * Creates the zero Polynomial on position exp
 * long exp[] : exponent of leading term
 * cabsind    : total 2-ind of exp (if -1 will be computed)
 * poly* t_p  : will hold the LT in tailRing
 * leadRing   : ring for the LT
 * tailRing   : ring for the tail
 */

poly kCreateZeroPoly(long exp[], long cabsind, poly* t_p, ring leadRing, ring tailRing)
{

  poly zeroPoly = NULL;

  number tmp1;
  poly tmp2, tmp3;

  if (cabsind == -1)
  {
    cabsind = 0;
    for (int i = 1; i <= leadRing->N; i++)
    {
      cabsind += ind_fact_2(exp[i]);
    }
//    Print("cabsind: %d\n", cabsind);
  }
  if (cabsind < leadRing->ch)
  {
    zeroPoly = p_ISet(twoPow(leadRing->ch - cabsind), tailRing);
  }
  else
  {
    zeroPoly = p_ISet(1, tailRing);
  }
  for (int i = 1; i <= leadRing->N; i++)
  {
    for (long j = 1; j <= exp[i]; j++)
    {
      tmp1 = nInit(j);
      tmp2 = p_ISet(1, tailRing);
      p_SetExp(tmp2, i, 1, tailRing);
      p_Setm(tmp2, tailRing);
      if (nIsZero(tmp1))
      { // should nowbe obsolet, test ! TODO OLIVER
        zeroPoly = p_Mult_q(zeroPoly, tmp2, tailRing);
      }
      else
      {
        tmp3 = p_NSet(nCopy(tmp1), tailRing);
        zeroPoly = p_Mult_q(zeroPoly, p_Add_q(tmp3, tmp2, tailRing), tailRing);
      }
    }
  }
  tmp2 = p_NSet(nCopy(pGetCoeff(zeroPoly)), leadRing);
  for (int i = 1; i <= leadRing->N; i++)
  {
    pSetExp(tmp2, i, p_GetExp(zeroPoly, i, tailRing));
  }
  p_Setm(tmp2, leadRing);
  *t_p = zeroPoly;
  zeroPoly = pNext(zeroPoly);
  pNext(*t_p) = NULL;
  pNext(tmp2) = zeroPoly;
  return tmp2;
}

// #define OLI_DEBUG

/*
 * Generate the s-polynomial for the virtual set of zero-polynomials
 */

void initenterzeropairsRing (poly p, int ecart, kStrategy strat, int atR)
{
  // Initialize
  long exp[50];            // The exponent of \hat{X} (basepoint)
  long cexp[50];           // The current exponent for iterating over all
  long ind[50];            // The power of 2 in the i-th component of exp
  long cind[50];           // analog for cexp
  long mult[50];           // How to multiply the elements of G
  long cabsind = 0;        // The abs. index of cexp, i.e. the sum of cind
  long habsind = 0;        // The abs. index of the coefficient of h
  long step[50];           // The last increases
  for (int i = 1; i <= currRing->N; i++)
  {
    exp[i] = p_GetExp(p, i, currRing);
    if (exp[i] & 1 != 0)
    {
      exp[i] = exp[i] - 1;
      mult[i] = 1;
    }
    cexp[i] = exp[i];
    ind[i] = ind_fact_2(exp[i]);
    cabsind += ind[i];
    cind[i] = ind[i];
    step[i] = 500000;
  }
  step[1] = 500000;
  habsind = ind2((long) p_GetCoeff(p, currRing));
  long bound = currRing->ch - habsind;
#ifdef OLI_DEBUG
  PrintS("-------------\npoly  :");
  wrp(p);
  Print("\nexp   : (%d, %d)\n", exp[1] + mult[1], exp[2] + mult[1]);
  Print("cexp  : (%d, %d)\n", cexp[1], cexp[2]);
  Print("cind  : (%d, %d)\n", cind[1], cind[2]);
  Print("bound : %d\n", bound);
  Print("cind  : %d\n", cabsind);
#endif
  if (cabsind == 0)
  {
    if (!(nextZeroSimplexExponent(exp, ind, cexp, cind, &cabsind, step, bound, currRing->N)))
    {
      return;
    }
  }
  // Now the whole simplex
  do
  {
    // Build s-polynomial
    // 2**ind-def * mult * g - exp-def * h
    poly t_p;
    poly zeroPoly = kCreateZeroPoly(cexp, cabsind, &t_p, currRing, strat->tailRing);
#ifdef OLI_DEBUG
    Print("%d, (%d, %d), ind = (%d, %d)\n", cabsind, cexp[1], cexp[2], cind[1], cind[2]);
    Print("zPoly : ");
    wrp(zeroPoly);
    Print("\n");
#endif
    enterOneZeroPairRing(zeroPoly, t_p, p, ecart, strat, atR);
  }
  while ( nextZeroSimplexExponent(exp, ind, cexp, cind, &cabsind, step, bound, currRing->N) );
}

/*
 * Create the Groebner basis of the vanishing polynomials.
 */

ideal createG0()
{
  // Initialize
  long exp[50];            // The exponent of \hat{X} (basepoint)
  long cexp[50];           // The current exponent for iterating over all
  long ind[50];            // The power of 2 in the i-th component of exp
  long cind[50];           // analog for cexp
  long mult[50];           // How to multiply the elements of G
  long cabsind = 0;        // The abs. index of cexp, i.e. the sum of cind
  long habsind = 0;        // The abs. index of the coefficient of h
  long step[50];           // The last increases
  for (int i = 1; i <= currRing->N; i++)
  {
    exp[i] = 0;
    cexp[i] = exp[i];
    ind[i] = 0;
    step[i] = 500000;
    cind[i] = ind[i];
  }
  long bound = currRing->ch;
  step[1] = 500000;
#ifdef OLI_DEBUG
  PrintS("-------------\npoly  :");
//  wrp(p);
  Print("\nexp   : (%d, %d)\n", exp[1] + mult[1], exp[2] + mult[1]);
  Print("cexp  : (%d, %d)\n", cexp[1], cexp[2]);
  Print("cind  : (%d, %d)\n", cind[1], cind[2]);
  Print("bound : %d\n", bound);
  Print("cind  : %d\n", cabsind);
#endif
  if (cabsind == 0)
  {
    if (!(nextZeroSimplexExponent(exp, ind, cexp, cind, &cabsind, step, bound, currRing->N)))
    {
      return idInit(1, 1);
    }
  }
  ideal G0 = idInit(1, 1);
  // Now the whole simplex
  do
  {
    // Build s-polynomial
    // 2**ind-def * mult * g - exp-def * h
    poly t_p;
    poly zeroPoly = kCreateZeroPoly(cexp, cabsind, &t_p, currRing, currRing);
#ifdef OLI_DEBUG
    Print("%d, (%d, %d), ind = (%d, %d)\n", cabsind, cexp[1], cexp[2], cind[1], cind[2]);
    Print("zPoly : ");
    wrp(zeroPoly);
    Print("\n");
#endif
    // Add to ideal
    pEnlargeSet(&(G0->m), IDELEMS(G0), 1);
    IDELEMS(G0) += 1;
    G0->m[IDELEMS(G0) - 1] = zeroPoly;
  }
  while ( nextZeroSimplexExponent(exp, ind, cexp, cind, &cabsind, step, bound, currRing->N) );
  idSkipZeroes(G0);
  return G0;
}
#endif

#ifdef HAVE_RINGS
/*2
*(s[0],h),...,(s[k],h) will be put to the pairset L
*/
void initenterstrongPairs (poly h,int k,int ecart,int isFromQ,kStrategy strat, int atR = -1)
{
  const int iCompH = pGetComp(h);
  if (!nIsOne(pGetCoeff(h)))
  {
    int j;
    BOOLEAN new_pair=FALSE;

    for (j=0; j<=k; j++)
    {
      // Print("j:%d, Ll:%d\n",j,strat->Ll);
//      if (((unsigned long) pGetCoeff(h) % (unsigned long) pGetCoeff(strat->S[j]) != 0) &&
//         ((unsigned long) pGetCoeff(strat->S[j]) % (unsigned long) pGetCoeff(h) != 0))
      if ( iCompH == pGetComp(strat->S[j]) )
      {
        {
          if (enterOneStrongPoly(j,h,ecart,isFromQ,strat, atR))
            new_pair=TRUE;
        }
      }
    }
  }
/*
ring r=256,(x,y,z),dp;
ideal I=12xz-133y, 2xy-z;
*/

}

/*2
* Generates spoly(0, h) if applicable. Assumes ring in Z/2^n.
*/
void enterExtendedSpoly(poly h,kStrategy strat)
{
  if (nIsOne(pGetCoeff(h))) return;
  number gcd;
  bool go = false;
  if (nDivBy((number) 0, pGetCoeff(h)))
  {
    gcd = nIntDiv((number) 0, pGetCoeff(h));
    go = true;
  }
  else
    gcd = nGcd((number) 0, pGetCoeff(h), strat->tailRing);
  if (go || !nIsOne(gcd))
  {
    poly p = h->next;
    if (!go)
    {
      number tmp = gcd;
      gcd = nIntDiv(0, gcd);
      nDelete(&tmp);
    }
    p_Test(p,strat->tailRing);
    p = pp_Mult_nn(p, gcd, strat->tailRing);
    nDelete(&gcd);

    if (p != NULL)
    {
      if (TEST_OPT_PROT)
      {
        PrintS("Z");
      }
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS("--- create zero spoly: ");
        p_wrp(h,currRing,strat->tailRing);
        PrintS(" ---> ");
      }
#endif
      poly tmp = pInit();
      pSetCoeff0(tmp, pGetCoeff(p));
      for (int i = 1; i <= rVar(currRing); i++)
      {
        pSetExp(tmp, i, p_GetExp(p, i, strat->tailRing));
      }
      if (rRing_has_Comp(currRing) && rRing_has_Comp(strat->tailRing))
      {
        p_SetComp(tmp, p_GetComp(p, strat->tailRing), currRing);
      }
      p_Setm(tmp, currRing);
      p = p_LmFreeAndNext(p, strat->tailRing);
      pNext(tmp) = p;
      LObject h;
      h.Init();
      h.p = tmp;
      h.tailRing = strat->tailRing;
      int posx;
      if (h.p!=NULL)
      {
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
        if (strat->Ll==-1)
          posx =0;
        else
          posx = strat->posInL(strat->L,strat->Ll,&h,strat);
        h.sev = pGetShortExpVector(h.p);
        if (strat->tailRing != currRing)
        {
          h.t_p = k_LmInit_currRing_2_tailRing(h.p, strat->tailRing);
        }
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
        {
          p_wrp(tmp,currRing,strat->tailRing);
          PrintLn();
        }
#endif
        enterL(&strat->L,&strat->Ll,&strat->Lmax,h,posx);
      }
    }
  }
  nDelete(&gcd);
}

void clearSbatch (poly h,int k,int pos,kStrategy strat)
{
  int j = pos;
  if ( (!strat->fromT)
  && (1//(strat->syzComp==0)
    //||(pGetComp(h)<=strat->syzComp)))
  ))
  {
    // Print("start clearS k=%d, pos=%d, sl=%d\n",k,pos,strat->sl);
    unsigned long h_sev = pGetShortExpVector(h);
    loop
    {
      if (j > k) break;
      clearS(h,h_sev, &j,&k,strat);
      j++;
    }
    // Print("end clearS sl=%d\n",strat->sl);
  }
}

/*2
* Generates a sufficient set of spolys (maybe just a finite generating
* set of the syzygys)
*/
void superenterpairs (poly h,int k,int ecart,int pos,kStrategy strat, int atR)
{
    assume (rField_is_Ring(currRing));
    // enter also zero divisor * poly, if this is non zero and of smaller degree
    if (!(rField_is_Domain(currRing))) enterExtendedSpoly(h, strat);
    initenterpairs(h, k, ecart, 0, strat, atR);
    initenterstrongPairs(h, k, ecart, 0, strat, atR);
    clearSbatch(h, k, pos, strat);
}
#endif

/*2
*(s[0],h),...,(s[k],h) will be put to the pairset L(via initenterpairs)
*superfluous elements in S will be deleted
*/
void enterpairs (poly h,int k,int ecart,int pos,kStrategy strat, int atR)
{
  int j=pos;

#ifdef HAVE_RINGS
  assume (!rField_is_Ring(currRing));
#endif

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
  const int iCompH = pGetComp(h);

  for (j=0; j<=k; j++)
  {
    const int iCompSj = pGetComp(strat->S[j]);
    if ((iCompH==iCompSj)
        || (0==iCompH) // TODO: what about this case???
        || (0==iCompSj))
    {
      enterOnePairSpecial(j,h,ecart,strat, atR);
    }
  }

  if (strat->noClearS) return;

//   #ifdef HAVE_PLURAL
/*
  if (rIsPluralRing(currRing))
  {
    j=pos;
    loop
    {
      if (j > k) break;

      if (pLmDivisibleBy(h, strat->S[j]))
      {
        deleteInS(j, strat);
        j--;
        k--;
      }

      j++;
    }
  }
  else
*/
//   #endif // ??? Why was the following cancelation disabled for non-commutative rings?
  {
    j=pos;
    loop
    {
      unsigned long h_sev = pGetShortExpVector(h);
      if (j > k) break;
      clearS(h,h_sev,&j,&k,strat);
      j++;
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
  int new_suc=strat->sl+1;
  i= *suc;
  if (i<0) i=0;

  for (; i<=strat->sl; i++)
  {
    at = posInS(strat,i-1,strat->S[i],strat->ecartS[i]);
    if (at != i)
    {
      if (new_suc > at) new_suc = at;
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
  if (new_suc <= strat->sl) *suc=new_suc;
  else                      *suc=-1;
}


/*2
*looks up the position of p in set
*set[0] is the smallest with respect to the ordering-procedure deg/pComp
* Assumption: posInS only depends on the leading term
*             otherwise, bba has to be changed
*/
int posInS (const kStrategy strat, const int length,const poly p,
            const int ecart_p)
{
  if(length==-1) return 0;
  polyset set=strat->S;
  int i;
  int an = 0;
  int en = length;
  int cmp_int = pOrdSgn;
  int pc=pGetComp(p);
  if ((currRing->MixedOrder)
#ifdef HAVE_PLURAL
  && (currRing->real_var_start==0)
#endif
#if 0
  || ((strat->ak>0) && ((currRing->order[0]==ringorder_c)||((currRing->order[0]==ringorder_C))))
#endif
  )
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
      if ((pWTotaldegree(set[i])>=o) && (pLmCmp(set[i],p) == cmp_int)) en=i;
      else                              an=i;
    }
  }
  else
  {
#ifdef HAVE_RINGS
    if (rField_is_Ring(currRing))
    {
      if (pLmCmp(set[length],p)== -cmp_int)
        return length+1;
      int cmp;
      loop
      {
        if (an >= en-1)
        {
          cmp = pLmCmp(set[an],p);
          if (cmp == cmp_int)  return an;
          if (cmp == -cmp_int) return en;
          if (nDivBy(pGetCoeff(p), pGetCoeff(set[an]))) return en;
          return an;
        }
        i = (an+en) / 2;
        cmp = pLmCmp(set[i],p);
        if (cmp == cmp_int)         en = i;
        else if (cmp == -cmp_int)   an = i;
        else
        {
          if (nDivBy(pGetCoeff(p), pGetCoeff(set[i]))) an = i;
          else en = i;
        }
      }
    }
    else
#endif
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
  p.GetpLength();
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

/*2 Pos for rings T: Here I am
* looks up the position of p in T
* set[0] is the smallest with respect to the ordering-procedure
* totaldegree,pComp
*/
int posInTrg0 (const TSet set,const int length,LObject &p)
{
  if (length==-1) return 0;
  int o = p.GetpFDeg();
  int op = set[length].GetpFDeg();
  int i;
  int an = 0;
  int en = length;
  int cmp_int = pOrdSgn;
  if ((op < o) || (pLmCmp(set[length].p,p.p)== -cmp_int))
    return length+1;
  int cmp;
  loop
  {
    if (an >= en-1)
    {
      op = set[an].GetpFDeg();
      if (op > o) return an;
      if (op < 0) return en;
      cmp = pLmCmp(set[an].p,p.p);
      if (cmp == cmp_int)  return an;
      if (cmp == -cmp_int) return en;
      if (nGreater(pGetCoeff(p.p), pGetCoeff(set[an].p))) return en;
      return an;
    }
    i = (an + en) / 2;
    op = set[i].GetpFDeg();
    if (op > o)       en = i;
    else if (op < o)  an = i;
    else
    {
      cmp = pLmCmp(set[i].p,p.p);
      if (cmp == cmp_int)                                     en = i;
      else if (cmp == -cmp_int)                               an = i;
      else if (nGreater(pGetCoeff(p.p), pGetCoeff(set[i].p))) an = i;
      else                                                    en = i;
    }
  }
}
/*
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
  */
/*2
* looks up the position of p in T
* set[0] is the smallest with respect to the ordering-procedure
* totaldegree,pComp
*/
int posInT110 (const TSet set,const int length,LObject &p)
{
  p.GetpLength();
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
  int ol = p.GetpLength();
  if (length==-1) return 0;

  int op=p.ecart;

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
  p.GetpLength();
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

/*2 Position for rings L: Here I am
* looks up the position of polynomial p in set
* e is the ecart of p
* set[length] is the smallest element in set with respect
* to the ordering-procedure totaldegree,pComp
*/
inline int getIndexRng(long coeff)
{
  if (coeff == 0) return -1;
  long tmp = coeff;
  int ind = 0;
  while (tmp % 2 == 0)
  {
    tmp = tmp / 2;
    ind++;
  }
  return ind;
}

int posInLrg0 (const LSet set, const int length,
              LObject* p,const kStrategy strat)
/*          if (nGreater(pGetCoeff(p), pGetCoeff(set[an]))) return en;
        if (pLmCmp(set[i],p) == cmp_int)         en = i;
        else if (pLmCmp(set[i],p) == -cmp_int)   an = i;
        else
        {
          if (nGreater(pGetCoeff(p), pGetCoeff(set[i]))) an = i;
          else en = i;
        }*/
{
  if (length < 0) return 0;

  int o = p->GetpFDeg();
  int op = set[length].GetpFDeg();

  if ((op > o) || ((op == o) && (pLmCmp(set[length].p,p->p) != -pOrdSgn)))
    return length + 1;
  int i;
  int an = 0;
  int en = length;
  loop
  {
    if (an >= en - 1)
    {
      op = set[an].GetpFDeg();
      if ((op > o) || ((op == o) && (pLmCmp(set[an].p,p->p) != -pOrdSgn)))
        return en;
      return an;
    }
    i = (an+en) / 2;
    op = set[i].GetpFDeg();
    if ((op > o) || ((op == o) && (pLmCmp(set[i].p,p->p) != -pOrdSgn)))
      an = i;
    else
      en = i;
  }
}

/*{
  if (length < 0) return 0;

  int o = p->GetpFDeg();
  int op = set[length].GetpFDeg();

  int inde = getIndexRng((unsigned long) pGetCoeff(set[length].p));
  int indp = getIndexRng((unsigned long) pGetCoeff(p->p));
  int inda;
  int indi;

  if ((inda > indp) || ((inda == inde) && ((op > o) || ((op == o) && (pLmCmp(set[length].p,p->p) != -pOrdSgn)))))
    return length + 1;
  int i;
  int an = 0;
  inda = getIndexRng((unsigned long) pGetCoeff(set[an].p));
  int en = length;
  loop
  {
    if (an >= en-1)
    {
      op = set[an].GetpFDeg();
      if ((indp > inda) || ((indp == inda) && ((op > o) || ((op == o) && (pLmCmp(set[an].p,p->p) != -pOrdSgn)))))
        return en;
      return an;
    }
    i = (an + en) / 2;
    indi = getIndexRng((unsigned long) pGetCoeff(set[i].p));
    op = set[i].GetpFDeg();
    if ((indi > indp) || ((indi == indp) && ((op > o) || ((op == o) && (pLmCmp(set[i].p,p->p) != -pOrdSgn)))))
    // if ((op > o) || ((op == o) && (pLmCmp(set[i].p,p->p) != -pOrdSgn)))
    {
      an = i;
      inda = getIndexRng((unsigned long) pGetCoeff(set[an].p));
    }
    else
      en = i;
  }
} */

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
  || ((op == o) && (set[length].length >p->length))
  || ((op == o) && (set[length].length <= p->length)
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
      || ((op == o) && (set[an].length >p->length))
      || ((op == o) && (set[an].length <=p->length)
         && (pLmCmp(set[an].p,p->p) != -pOrdSgn)))
        return en;
      return an;
    }
    i=(an+en) / 2;
    op = set[i].GetpFDeg();
    if ((op > o)
    || ((op == o) && (set[i].length > p->length))
    || ((op == o) && (set[i].length <= p->length)
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
/////      assume (j >= 0 && j <= strat->tl && strat->S_2_T(j) != NULL
/////      && strat->S_2_T(j)->p == strat->S[j]); // wrong?
//      assume (j >= 0 && j <= strat->sl && strat->S_2_T(j) != NULL && strat->S_2_T(j)->p == strat->S[j]);
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
      With->length=0;
      With->pLength=0;
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

poly redtailBba (LObject* L, int pos, kStrategy strat, BOOLEAN withT, BOOLEAN normalize)
{
#define REDTAIL_CANONICALIZE 100
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

  int cnt=REDTAIL_CANONICALIZE;
  while(!Ln.IsNull())
  {
    loop
    {
      Ln.SetShortExpVector();
      if (withT)
      {
        int j;
        j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, &Ln);
        if (j < 0) break;
        With = &(strat->T[j]);
      }
      else
      {
        With = kFindDivisibleByInS(strat, pos, &Ln, &With_s);
        if (With == NULL) break;
      }
      cnt--;
      if (cnt==0)
      {
        cnt=REDTAIL_CANONICALIZE;
        poly tmp=Ln.CanonicalizeP();
        if (normalize)
        {
          Ln.Normalize();
          //pNormalize(tmp);
          //if (TEST_OPT_PROT) { PrintS("n"); mflush(); }
        }
      }
      if (normalize && (!TEST_OPT_INTSTRATEGY) && (!nIsOne(pGetCoeff(With->p))))
      {
        With->pNorm();
      }
      strat->redTailChange=TRUE;
      if (ksReducePolyTail(L, With, &Ln))
      {
        // reducing the tail would violate the exp bound
        //  set a flag and hope for a retry (in bba)
        strat->completeReduce_retry=TRUE;
        if ((Ln.p != NULL) && (Ln.t_p != NULL)) Ln.p=NULL;
        do
        {
          pNext(h) = Ln.LmExtractAndIter();
          pIter(h);
          L->pLength++;
        } while (!Ln.IsNull());
        goto all_done;
      }
      if (Ln.IsNull()) goto all_done;
      if (! withT) With_s.Init(currRing);
    }
    pNext(h) = Ln.LmExtractAndIter();
    pIter(h);
    pNormalize(h);
    L->pLength++;
  }

  all_done:
  Ln.Delete();
  if (L->p != NULL) pNext(L->p) = pNext(p);

  if (strat->redTailChange)
  {
    L->last = NULL;
    L->length = 0;
  }

  //if (TEST_OPT_PROT) { PrintS("N"); mflush(); }
  //L->Normalize(); // HANNES: should have a test
  kTest_L(L);
  return L->GetLmCurrRing();
}

#ifdef HAVE_RINGS
poly redtailBba_Z (LObject* L, int pos, kStrategy strat )
// normalize=FALSE, withT=FALSE, coeff=Z
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

  int cnt=REDTAIL_CANONICALIZE;
  while(!Ln.IsNull())
  {
    loop
    {
      Ln.SetShortExpVector();
      With = kFindDivisibleByInS(strat, pos, &Ln, &With_s);
      if (With == NULL) break;
      cnt--;
      if (cnt==0)
      {
        cnt=REDTAIL_CANONICALIZE;
        poly tmp=Ln.CanonicalizeP();
      }
      // we are in Z, do not Ccall pNorm
      strat->redTailChange=TRUE;
      // test divisibility of coefs:
      poly p_Ln=Ln.GetLmCurrRing();
      poly p_With=With->GetLmCurrRing();
      number z=nIntMod(pGetCoeff(p_Ln),pGetCoeff(p_With));
      if (!nIsZero(z))
      {
        // subtract z*Ln, add z.Ln to L
        poly m=pHead(p_Ln);
        pSetCoeff(m,z);
        poly mm=pHead(m);
        pNext(h) = m;
        pIter(h);
        L->pLength++;
        mm=pNeg(mm);
        if (Ln.bucket!=NULL)
        {
          int dummy=1;
          kBucket_Add_q(Ln.bucket,mm,&dummy);
        }
        else
        {
          if (Ln.p!=NULL) Ln.p=pAdd(Ln.p,mm);
          else if (Ln.t_p!=NULL)  Ln.t_p=p_Add_q(Ln.t_p,mm,strat->tailRing);
        }
      }
      else
        nDelete(&z);

      if (ksReducePolyTail(L, With, &Ln))
      {
        // reducing the tail would violate the exp bound
        //  set a flag and hope for a retry (in bba)
        strat->completeReduce_retry=TRUE;
        if ((Ln.p != NULL) && (Ln.t_p != NULL)) Ln.p=NULL;
        do
        {
          pNext(h) = Ln.LmExtractAndIter();
          pIter(h);
          L->pLength++;
        } while (!Ln.IsNull());
        goto all_done;
      }
      if (Ln.IsNull()) goto all_done;
      With_s.Init(currRing);
    }
    pNext(h) = Ln.LmExtractAndIter();
    pIter(h);
    pNormalize(h);
    L->pLength++;
  }

  all_done:
  Ln.Delete();
  if (L->p != NULL) pNext(L->p) = pNext(p);

  if (strat->redTailChange)
  {
    L->last = NULL;
    L->length = 0;
  }

  //if (TEST_OPT_PROT) { PrintS("N"); mflush(); }
  //L->Normalize(); // HANNES: should have a test
  kTest_L(L);
  return L->GetLmCurrRing();
}
#endif

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
  if (TEST_OPT_OLDSTD)
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
  Print("product criterion:%d chain criterion:%d\n",strat->cp,strat->c3);
  if (hilbcount!=0) Print("hilbert series criterion:%d\n",hilbcount);
  /* in usual case strat->cv is 0, it gets changed only in shift routines */
  if (strat->cv!=0) Print("shift V criterion:%d\n",strat->cv);
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
      Print(" o:%ld e:%d l:%d",
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
    Print("  o:%ld e:%d l:%d",
          strat->L[i].pFDeg(),strat->L[i].ecart,strat->L[i].length);
  }
  PrintLn();
}

#endif


/*2
*construct the set s from F
*/
void initS (ideal F, ideal Q, kStrategy strat)
{
  int   i,pos;

  if (Q!=NULL) i=((IDELEMS(F)+IDELEMS(Q)+(setmaxTinc-1))/setmaxTinc)*setmaxTinc;
  else         i=((IDELEMS(F)+(setmaxTinc-1))/setmaxTinc)*setmaxTinc;
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
        if (pOrdSgn==-1)
        {
          deleteHC(&h, strat);
        }
        if (h.p!=NULL)
        {
          strat->initEcart(&h);
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
      if (pOrdSgn==-1)
      {
        cancelunit(&h);  /*- tries to cancel a unit -*/
        deleteHC(&h, strat);
      }
      if (h.p!=NULL)
      // do not rely on the input being a SB!
      {
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
  if ((strat->sl>=0)
#ifdef HAVE_RINGS
       && nIsUnit(pGetCoeff(strat->S[0]))
#endif
       && pIsConstant(strat->S[0]))
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
        if (pOrdSgn==-1)
        {
          deleteHC(&h,strat);
        }
        if (TEST_OPT_INTSTRATEGY)
        {
          //pContent(h.p);
          h.pCleardenom(); // also does a pContent
        }
        else
        {
          h.pNorm();
        }
        if (h.p!=NULL)
        {
          strat->initEcart(&h);
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
      if (h.p!=NULL)
      {
        if (pOrdSgn==-1)
        {
          cancelunit(&h);  /*- tries to cancel a unit -*/
          deleteHC(&h, strat);
        }
        if (h.p!=NULL)
        {
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
          if (strat->Ll==-1)
            pos =0;
          else
            pos = strat->posInL(strat->L,strat->Ll,&h,strat);
          h.sev = pGetShortExpVector(h.p);
          enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);
        }
      }
    }
  }
  /*- test, if a unit is in F -*/

  if ((strat->Ll>=0)
#ifdef HAVE_RINGS
       && nIsUnit(pGetCoeff(strat->L[strat->Ll].p))
#endif
       && pIsConstant(strat->L[strat->Ll].p))
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
  i=((i+IDELEMS(F)+IDELEMS(P)+15)/16)*16;
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
        if (pOrdSgn==-1)
        {
          deleteHC(&h,strat);
        }
        if (h.p!=NULL)
        {
          strat->initEcart(&h);
          if (strat->sl==-1)
            pos =0;
          else
          {
            pos = posInS(strat,strat->sl,h.p,h.ecart);
          }
          h.sev = pGetShortExpVector(h.p);
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
      if (pOrdSgn==-1)
      {
        deleteHC(&h,strat);
      }
      else
      {
        h.p=redtailBba(h.p,strat->sl,strat);
      }
      if (h.p!=NULL)
      {
        strat->initEcart(&h);
        if (strat->sl==-1)
          pos =0;
        else
          pos = posInS(strat,strat->sl,h.p,h.ecart);
        h.sev = pGetShortExpVector(h.p);
        strat->enterS(h,pos,strat, strat->tl+1);
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
          {
            h.p=redtailBba(h.p,strat->sl,strat);
          }
        }
        else
        {
          h.p=redMora(h.p,strat->sl,strat);
        }
        if(h.p!=NULL)
        {
          strat->initEcart(&h);
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
        strat->initEcart(&h);
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
#ifdef HAVE_RINGS_LOC
    // Leading coef have to be a unit
    if ( !(nIsUnit(p_GetCoeff((*p).p, r))) ) return;
#endif
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
#ifdef HAVE_RINGS_LOC
        (*p).pLength = 1;  // Why wasn't this set already?
#endif
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

#if 0
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
#endif

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
  BOOLEAN change,any_change;
//  Print("nach initS: updateS start mit sl=%d\n",(strat->sl));
//  for (i=0; i<=(strat->sl); i++)
//  {
//    Print("s%d:",i);
//    if (strat->fromQ!=NULL) Print("(Q:%d) ",strat->fromQ[i]);
//    pWrite(strat->S[i]);
//  }
//  Print("pOrdSgn=%d\n", pOrdSgn);
  any_change=FALSE;
  if (pOrdSgn==1)
  {
    while (suc != -1)
    {
      i=suc+1;
      while (i<=strat->sl)
      {
        change=FALSE;
        if (((strat->fromQ==NULL) || (strat->fromQ[i]==0)) && (i>0))
        {
          redSi = pHead(strat->S[i]);
          strat->S[i] = redBba(strat->S[i],i-1,strat);
          //if ((strat->ak!=0)&&(strat->S[i]!=NULL))
          //  strat->S[i]=redQ(strat->S[i],i+1,strat); /*reduce S[i] mod Q*/
          if (pCmp(redSi,strat->S[i])!=0)
          {
            change=TRUE;
            any_change=TRUE;
            #ifdef KDEBUG
            if (TEST_OPT_DEBUG)
            {
              PrintS("reduce:");
              wrp(redSi);PrintS(" to ");p_wrp(strat->S[i], currRing, strat->tailRing);PrintLn();
            }
            #endif
            if (TEST_OPT_PROT)
            {
              if (strat->S[i]==NULL)
                PrintS("V");
              else
                PrintS("v");
              mflush();
            }
          }
          pLmDelete(&redSi);
          if (strat->S[i]==NULL)
          {
            deleteInS(i,strat);
            i--;
          }
          else if (change)
          {
            if (TEST_OPT_INTSTRATEGY)
            {
              //pContent(strat->S[i]);
              strat->S[i]=p_Cleardenom(strat->S[i], currRing);// also does a pContent
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
      if (any_change) reorderS(&suc,strat);
      else break;
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
            h.pCleardenom();// also does a pContent
          }
        }
        else
        {
          h.p = strat->S[i];
        }
        strat->initEcart(&h);
        if (strat->honey)
        {
          strat->ecartS[i] = h.ecart;
        }
        if (strat->sevS[i] == 0) {strat->sevS[i] = pGetShortExpVector(h.p);}
        else assume(strat->sevS[i] == pGetShortExpVector(h.p));
        h.sev = strat->sevS[i];
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
        change=FALSE;
        if (((strat->fromQ==NULL) || (strat->fromQ[i]==0)) && (i>0))
        {
          redSi=pHead((strat->S)[i]);
          (strat->S)[i] = redMora((strat->S)[i],i-1,strat);
          if ((strat->S)[i]==NULL)
          {
            deleteInS(i,strat);
            i--;
          }
          else if (pCmp((strat->S)[i],redSi)!=0)
          {
            any_change=TRUE;
            h.p = strat->S[i];
            strat->initEcart(&h);
            strat->ecartS[i] = h.ecart;
            if (TEST_OPT_INTSTRATEGY)
            {
              strat->S[i]=p_Cleardenom(strat->S[i], currRing);// also does a pContent
            }
            else
            {
              pNorm(strat->S[i]); // == h.p
            }
            h.sev =  pGetShortExpVector(h.p);
            strat->sevS[i] = h.sev;
          }
          pLmDelete(&redSi);
          kTest(strat);
        }
        i++;
      }
#ifdef KDEBUG
      kTest(strat);
#endif
      if (any_change) reorderS(&suc,strat);
      else { suc=-1; break; }
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
      if ((strat->fromQ==NULL) || (strat->fromQ[i]==0))
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
        h.length = h.pLength = pLength(h.p);
      }
      if ((strat->fromQ==NULL) || (strat->fromQ[i]==0))
        cancelunit1(&h,&suc,strat->sl,strat);
      h.SetpFDeg();
      /*puts the elements of S also to T*/
      enterT(h,strat);
      strat->S_2_R[i] = strat->tl;
    }
    if (suc!= -1) updateS(toT,strat);
  }
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
      strat->lenSw=(wlen_type*)omRealloc0Size(strat->lenSw,
                                       IDELEMS(strat->Shdl)*sizeof(wlen_type),
                                       (IDELEMS(strat->Shdl)+setmaxTinc)
                                                 *sizeof(wlen_type));
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
            (strat->sl - atS + 1)*sizeof(wlen_type));
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
  assume(p.pLength == 0 || pLength(p.p) == p.pLength || rIsSyzIndexRing(currRing)); // modulo syzring
  assume(p.FDeg == p.pFDeg());
  assume(!p.is_normalized || nIsOne(pGetCoeff(p.p)));

#ifdef KDEBUG
  // do not put an LObject twice into T:
  for(i=strat->tl;i>=0;i--)
  {
    if (p.p==strat->T[i].p)
    {
      printf("already in T at pos %d of %d, atT=%d\n",i,strat->tl,atT);
      return;
    }
  }
#endif
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
  strat->enterOnePair=enterOnePairNormal;
  strat->chainCrit=chainCritNormal;
#ifdef HAVE_RINGS
  if (rField_is_Ring(currRing))
  {
    strat->enterOnePair=enterOnePairRing;
    strat->chainCrit=chainCritRing;
  }
#endif
#ifdef HAVE_RATGRING
  if (rIsRatGRing(currRing))
  {
     strat->chainCrit=chainCritPart;
     /* enterOnePairNormal get rational part in it */
  }
#endif

  strat->sugarCrit =        TEST_OPT_SUGARCRIT;
  strat->Gebauer =          strat->homog || strat->sugarCrit;
  strat->honey =            !strat->homog || strat->sugarCrit || TEST_OPT_WEIGHTM;
  if (TEST_OPT_NOT_SUGAR) strat->honey = FALSE;
  strat->pairtest = NULL;
  /* alway use tailreduction, except:
  * - in local rings, - in lex order case, -in ring over extensions */
  strat->noTailReduction = !TEST_OPT_REDTAIL;

#ifdef HAVE_PLURAL
  // and r is plural_ring
  //  hence this holds for r a rational_plural_ring
  if( rIsPluralRing(currRing) || (rIsSCA(currRing) && !strat->z2homog) )
  {    //or it has non-quasi-comm type... later
    strat->sugarCrit = FALSE;
    strat->Gebauer = FALSE;
    strat->honey = FALSE;
  }
#endif

#ifdef HAVE_RINGS
  // Coefficient ring?
  if (rField_is_Ring(currRing))
  {
    strat->sugarCrit = FALSE;
    strat->Gebauer = FALSE ;
    strat->honey = FALSE;
  }
#endif
  #ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    if (strat->homog) PrintS("ideal/module is homogeneous\n");
    else              PrintS("ideal/module is not homogeneous\n");
  }
  #endif
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
      if (TEST_OPT_OLDSTD)
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
    if (strat->homog)
    {
       strat->posInL = posInL110;
       strat->posInT = posInT110;
    }
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
#ifdef HAVE_RINGS
  if (rField_is_Ring(currRing))
  {
    strat->posInL = posInL11;
    strat->posInT = posInT11;
  }
#endif
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
  strat->Lmax = ((IDELEMS(F)+setmaxLinc-1)/setmaxLinc)*setmaxLinc;
  strat->Ll = -1;
  strat->L = initL(((IDELEMS(F)+setmaxLinc-1)/setmaxLinc)*setmaxLinc);
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
  if (!TEST_OPT_SB_1)
  {
    updateS(TRUE,strat);
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
  pLmDelete(&strat->tail);
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
      //&& (strat->syzComp>0)
      //&& (pGetComp(r->m[l])<=strat->syzComp)
      )
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
    BOOLEAN reduction_found=FALSE;
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
              reduction_found=TRUE;
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
    if (/*TEST_OPT_REDSB &&*/ reduction_found)
    {
      for (l=IDELEMS(r)-1;l>=0;l--)
      {
        if (r->m[l]!=NULL)
        {
          for(q=IDELEMS(r)-1;q>=0;q--)
          {
            if ((l!=q)
            && (r->m[q]!=NULL)
            &&(pLmDivisibleBy(r->m[l],r->m[q])))
            {
              pDelete(&r->m[q]);
            }
          }
        }
      }
    }
  }
  idSkipZeroes(r);
}

void completeReduce (kStrategy strat, BOOLEAN withT)
{
  int i;
  int low = (((pOrdSgn==1) && (strat->ak==0)) ? 1 : 0);
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
    int end_pos=strat->sl;
    if ((strat->fromQ!=NULL) && (strat->fromQ[i])) continue; // do not reduce Q_i
    if (strat->ak==0) end_pos=i-1;
    TObject* T_j = strat->s_2_t(i);
    if (T_j != NULL)
    {
      L = *T_j;
      poly p;
      if (pOrdSgn == 1)
        strat->S[i] = redtailBba(&L, end_pos, strat, withT);
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
        strat->S[i] = redtailBba(strat->S[i], end_pos, strat, withT);
      else
        strat->S[i] = redtail(strat->S[i], strat->sl, strat);
      if (TEST_OPT_INTSTRATEGY)
        strat->S[i]=p_Cleardenom(strat->S[i], currRing);
    }
    if (TEST_OPT_PROT)
      PrintS("-");
  }
  if (TEST_OPT_PROT) PrintLn();
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

#if 0
  if (currRing->weight_all_1)
    scComputeHC(strat->Shdl,NULL,strat->ak,strat->kHEdge, strat->tailRing);
  else
    scComputeHCw(strat->Shdl,NULL,strat->ak,strat->kHEdge, strat->tailRing);
#else
  scComputeHC(strat->Shdl,NULL,strat->ak,strat->kHEdge, strat->tailRing);
#endif
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
    #ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      Print("H(%d):",j);
      wrp(strat->kHEdge);
      PrintLn();
    }
    #endif
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
  if (strat->overflow) return FALSE;
  assume(L->p1 != NULL && L->p2 != NULL);
  // shift changes: from 0 to -1
  assume(L->i_r1 >= -1 && L->i_r1 <= strat->tl);
  assume(L->i_r2 >= -1 && L->i_r2 <= strat->tl);
  assume(strat->tailRing != currRing);

  if (! k_GetLeadTerms(L->p1, L->p2, currRing, m1, m2, strat->tailRing))
    return FALSE;
  // shift changes: extra case inserted
  if ((L->i_r1 == -1) || (L->i_r2 == -1) )
  {
    return TRUE;
  }
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

#ifdef HAVE_RINGS
/***************************************************************
 *
 * Checks, if we can compute the gcd poly / strong pair
 * gcd-poly = m1 * R[atR] + m2 * S[atS]
 *
 ***************************************************************/
BOOLEAN kCheckStrongCreation(int atR, poly m1, int atS, poly m2, kStrategy strat)
{
  assume(strat->S_2_R[atS] >= -1 && strat->S_2_R[atS] <= strat->tl);
  //assume(strat->tailRing != currRing);

  poly p1_max = (strat->R[atR])->max;
  poly p2_max = (strat->R[strat->S_2_R[atS]])->max;

  if ((p1_max != NULL && !p_LmExpVectorAddIsOk(m1, p1_max, strat->tailRing)) ||
      (p2_max != NULL && !p_LmExpVectorAddIsOk(m2, p2_max, strat->tailRing)))
  {
    return FALSE;
  }
  return TRUE;
}
#endif

BOOLEAN kStratChangeTailRing(kStrategy strat, LObject *L, TObject* T, unsigned long expbound)
{
  assume((strat->tailRing == currRing) || (strat->tailRing->bitmask < currRing->bitmask));
  /* initial setup or extending */

  if (expbound == 0) expbound = strat->tailRing->bitmask << 1;
  if (expbound >= currRing->bitmask) return FALSE;
  strat->overflow=FALSE;
  ring new_tailRing = rModifyRing(currRing,
                                  // Hmmm .. the condition pFDeg == pDeg
                                  // might be too strong
#ifdef HAVE_RINGS
                                  (strat->homog && pFDeg == pDeg && !(rField_is_Ring(currRing))), // TODO Oliver
#else
                                  (strat->homog && pFDeg == pDeg), // omit_degree
#endif
                                  (strat->ak==0), // omit_comp if the input is an ideal 
                                  expbound); // exp_limit

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
  long e;

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
  if (rField_is_Ring(currRing))
  {
    l *= 2;
  }
  e = p_GetMaxExp(l, currRing);
  if (e <= 1) e = 2;

  kStratChangeTailRing(strat, NULL, NULL, e);
}

skStrategy::skStrategy()
{
  memset(this, 0, sizeof(skStrategy));
#ifndef NDEBUG
  strat_nr++;
  nr=strat_nr;
  if (strat_fac_debug) Print("s(%d) created\n",nr);
#endif
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


//#ifdef HAVE_MORE_POS_IN_T
#if 1
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


// determines the position based on: 1.) pLength
int posInT_pLength(const TSet set,const int length,LObject &p)
{
  int ol = p.GetpLength();
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
      if (set[an].pLength>ol) return an;
      return en;
    }
    i=(an+en) / 2;
    if (set[i].pLength>ol) en=i;
    else                        an=i;
  }
}
#endif

// kstd1.cc:
int redFirst (LObject* h,kStrategy strat);
int redEcart (LObject* h,kStrategy strat);
void enterSMora (LObject p,int atS,kStrategy strat, int atR=-1);
void enterSMoraNF (LObject p,int atS,kStrategy strat, int atR=-1);
// ../Singular/misc.cc:
char *  showOption();

void kDebugPrint(kStrategy strat)
{
  PrintS("red: ");
    if (strat->red==redFirst) PrintS("redFirst\n");
    else if (strat->red==redHoney) PrintS("redHoney\n");
    else if (strat->red==redEcart) PrintS("redEcart\n");
    else if (strat->red==redHomog) PrintS("redHomog\n");
    else  Print("%p\n",(void*)strat->red);
  PrintS("posInT: ");
    if (strat->posInT==posInT0) PrintS("posInT0\n");
    else if (strat->posInT==posInT0) PrintS("posInT0\n");
    else if (strat->posInT==posInT1) PrintS("posInT1\n");
    else if (strat->posInT==posInT11) PrintS("posInT11\n");
    else if (strat->posInT==posInT110) PrintS("posInT110\n");
    else if (strat->posInT==posInT13) PrintS("posInT13\n");
    else if (strat->posInT==posInT15) PrintS("posInT15\n");
    else if (strat->posInT==posInT17) PrintS("posInT17\n");
    else if (strat->posInT==posInT17_c) PrintS("posInT17_c\n");
    else if (strat->posInT==posInT19) PrintS("posInT19\n");
    else if (strat->posInT==posInT2) PrintS("posInT2\n");
#ifdef HAVE_MORE_POS_IN_T
    else if (strat->posInT==posInT_EcartFDegpLength) PrintS("posInT_EcartFDegpLength\n");
    else if (strat->posInT==posInT_FDegpLength) PrintS("posInT_FDegpLength\n");
    else if (strat->posInT==posInT_pLength) PrintS("posInT_pLength\n");
#endif
    else if (strat->posInT==posInT_EcartpLength) PrintS("posInT_EcartpLength\n");
    else if (strat->posInT==posInTrg0) PrintS("posInTrg0\n");
    else  Print("%p\n",(void*)strat->posInT);
  PrintS("posInL: ");
    if (strat->posInL==posInL0) PrintS("posInL0\n");
    else if (strat->posInL==posInL10) PrintS("posInL10\n");
    else if (strat->posInL==posInL11) PrintS("posInL11\n");
    else if (strat->posInL==posInL110) PrintS("posInL110\n");
    else if (strat->posInL==posInL13) PrintS("posInL13\n");
    else if (strat->posInL==posInL15) PrintS("posInL15\n");
    else if (strat->posInL==posInL17) PrintS("posInL17\n");
    else if (strat->posInL==posInL17_c) PrintS("posInL17_c\n");
    else if (strat->posInL==posInLSpecial) PrintS("posInLSpecial\n");
    else if (strat->posInL==posInLrg0) PrintS("posInLrg0\n");
    else  Print("%p\n",(void*)strat->posInL);
  PrintS("enterS: ");
    if (strat->enterS==enterSBba) PrintS("enterSBba\n");
    else if (strat->enterS==enterSMora) PrintS("enterSMora\n");
    else if (strat->enterS==enterSMoraNF) PrintS("enterSMoraNF\n");
    else  Print("%p\n",(void*)strat->enterS);
  PrintS("initEcart: ");
    if (strat->initEcart==initEcartBBA) PrintS("initEcartBBA\n");
    else if (strat->initEcart==initEcartNormal) PrintS("initEcartNormal\n");
    else  Print("%p\n",(void*)strat->initEcart);
  PrintS("initEcartPair: ");
    if (strat->initEcartPair==initEcartPairBba) PrintS("initEcartPairBba\n");
    else if (strat->initEcartPair==initEcartPairMora) PrintS("initEcartPairMora\n");
    else  Print("%p\n",(void*)strat->initEcartPair);
  Print("homog=%d, LazyDegree=%d, LazyPass=%d, ak=%d,\n",
         strat->homog, strat->LazyDegree,strat->LazyPass, strat->ak);
  Print("honey=%d, sugarCrit=%d, Gebauer=%d, noTailReduction=%d\n",
         strat->honey,strat->sugarCrit,strat->Gebauer,strat->noTailReduction,strat->use_buckets);
  Print("posInLDependsOnLength=%d, use_buckets=%d\n",
         strat->posInLDependsOnLength,strat->use_buckets);
  PrintS(showOption());PrintLn();
  PrintS("LDeg: ");
    if (currRing->pLDeg==pLDeg0) PrintS("pLDeg0");
    else if (currRing->pLDeg==pLDeg0c) PrintS("pLDeg0c");
    else if (currRing->pLDeg==pLDegb) PrintS("pLDegb");
    else if (currRing->pLDeg==pLDeg1) PrintS("pLDeg1");
    else if (currRing->pLDeg==pLDeg1c) PrintS("pLDeg1c");
    else if (currRing->pLDeg==pLDeg1_Deg) PrintS("pLDeg1_Deg");
    else if (currRing->pLDeg==pLDeg1c_Deg) PrintS("pLDeg1c_Deg");
    else if (currRing->pLDeg==pLDeg1_Totaldegree) PrintS("pLDeg1_Totaldegree");
    else if (currRing->pLDeg==pLDeg1c_Totaldegree) PrintS("pLDeg1c_Totaldegree");
    else if (currRing->pLDeg==pLDeg1_WFirstTotalDegree) PrintS("pLDeg1_WFirstTotalDegree");
    else if (currRing->pLDeg==pLDeg1c_WFirstTotalDegree) PrintS("pLDeg1c_WFirstTotalDegree");
    else Print("? (%lx)", (long)currRing->pLDeg);
    PrintS(" / ");
    if (strat->tailRing->pLDeg==pLDeg0) PrintS("pLDeg0");
    else if (strat->tailRing->pLDeg==pLDeg0c) PrintS("pLDeg0c");
    else if (strat->tailRing->pLDeg==pLDegb) PrintS("pLDegb");
    else if (strat->tailRing->pLDeg==pLDeg1) PrintS("pLDeg1");
    else if (strat->tailRing->pLDeg==pLDeg1c) PrintS("pLDeg1c");
    else if (strat->tailRing->pLDeg==pLDeg1_Deg) PrintS("pLDeg1_Deg");
    else if (strat->tailRing->pLDeg==pLDeg1c_Deg) PrintS("pLDeg1c_Deg");
    else if (strat->tailRing->pLDeg==pLDeg1_Totaldegree) PrintS("pLDeg1_Totaldegree");
    else if (strat->tailRing->pLDeg==pLDeg1c_Totaldegree) PrintS("pLDeg1c_Totaldegree");
    else if (strat->tailRing->pLDeg==pLDeg1_WFirstTotalDegree) PrintS("pLDeg1_WFirstTotalDegree");
    else if (strat->tailRing->pLDeg==pLDeg1c_WFirstTotalDegree) PrintS("pLDeg1c_WFirstTotalDegree");
    else Print("? (%lx)", (long)strat->tailRing->pLDeg);
    Print(" syzring:%d, syzComp(strat):%d syzComb(ring)\n",rIsSyzIndexRing(currRing),strat->syzComp,rGetCurrSyzLimit());

}


#ifdef HAVE_SHIFTBBA
poly pMove2CurrTail(poly p, kStrategy strat)
{
  /* assume: p is completely in currRing */
  /* produces an object with LM in curring
     and TAIL in tailring */
  if (pNext(p)!=NULL)
  {
    pNext(p) = prMoveR(pNext(p), /* src */ currRing, /* dest */ strat->tailRing);
  }
  return(p);
}
#endif

#ifdef HAVE_SHIFTBBA
poly pMoveCurrTail2poly(poly p, kStrategy strat)
{
  /* assume: p has  LM in curring and TAIL in tailring */
  /* convert it to complete currRing */

  /* check that LM is in currRing */
  assume(p_LmCheckIsFromRing(p, currRing));

  if (pNext(p)!=NULL)
  {
    pNext(p) = prMoveR(pNext(p), /* src */ strat->tailRing, /* dest */currRing);
  }
  return(p);
}
#endif

#ifdef HAVE_SHIFTBBA
poly pCopyL2p(LObject H, kStrategy strat)
{
    /* restores a poly in currRing from LObject */
    LObject h = H;
    h.Copy();
    poly p;
    if (h.p == NULL)
    {
      if (h.t_p != NULL)
      {
         p = prMoveR(h.t_p, /* source ring: */ strat->tailRing, /* dest. ring: */ currRing);
        return(p);
      }
      else
      {
        /* h.tp == NULL -> the object is NULL */
        return(NULL);
      }
    }
    /* we're here if h.p != NULL */
    if (h.t_p == NULL)
    {
       /* then h.p is the whole poly in currRing */
       p = h.p;
      return(p);
    }
    /* we're here if h.p != NULL and h.t_p != NULL */
    // clean h.p, get poly from t_p
     pNext(h.p)=NULL;
     pDelete(&h.p);
     p = prMoveR(h.t_p, /* source ring: */ strat->tailRing,
                         /* dest. ring: */ currRing);
     // no need to clean h: we re-used the polys
    return(p);
}
#endif

//LObject pCopyp2L(poly p, kStrategy strat)
//{
    /* creates LObject from the poly in currRing */
  /* actually put p into L.p and make L.t_p=NULL : does not work */

//}

// poly pCopyL2p(LObject H, kStrategy strat)
// {
//   /* restores a poly in currRing from LObject */
//   LObject h = H;
//   h.Copy();
//   poly p;
//   if (h.p == NULL)
//   {
//     if (h.t_p != NULL)
//     {
//       p = p_ShallowCopyDelete(h.t_p, (strat->tailRing != NULL ? strat->tailRing : currRing), strat->tailBin);
//       return(p);
//     }
//     else
//     {
//       /* h.tp == NULL -> the object is NULL */
//       return(NULL);
//     }
//   }
//   /* we're here if h.p != NULL */

//   if (h.t_p == NULL)
//   {
//     /* then h.p is the whole poly in tailRing */
//     if (strat->tailBin != NULL && (pNext(h.p) != NULL))
//     {
//       p = p_ShallowCopyDelete(h.p, (strat->tailRing != NULL ? strat->tailRing : currRing), strat->tailBin);
//     }
//     return(p);
//   }
//   /* we're here if h.p != NULL and h.t_p != NULL */
//   p = pCopy(pHead(h.p)); // in currRing
//   if (strat->tailBin != NULL && (pNext(h.p) != NULL))
//   {
//     //    pNext(p) = p_ShallowCopyDelete(pNext(h.t_p), (strat->tailRing != NULL ? strat->tailRing : currRing), strat->tailBin);
//     poly pp = p_Copy(pNext(h.p), strat->tailRing);
//     //    poly p3 = p_Copy(pNext(h.p), currRing); // error
//       // p_ShallowCopyDelete(pNext(h.p), currRing, strat->tailBin); // the same as pp
//     poly p5 = p_ShallowCopyDelete(pNext(h.p), strat->tailRing, strat->tailBin);
//     pNext(p) = p_ShallowCopyDelete(h.t_p, strat->tailRing, strat->tailBin);
//     poly p4 = p_Copy(h.t_p, strat->tailRing);
//     //    if (p.t_p != NULL) pNext(p.t_p) = pNext(p.p);
//   }
//   //  pTest(p);
//   return(p);
// }

#ifdef HAVE_SHIFTBBA
/* including the self pairs */
void updateSShift(kStrategy strat,int uptodeg,int lV)
{
  /* to use after updateS(toT=FALSE,strat) */
  /* fills T with shifted elt's of S */
  int i;
  LObject h;
  int atT = -1; // or figure out smth better
  strat->tl = -1; // init
  for (i=0; i<=strat->sl; i++)
  {
    memset(&h,0,sizeof(h));
    h.p =  strat->S[i]; // lm in currRing, tail in TR
    strat->initEcart(&h);
    h.sev = strat->sevS[i];
    h.t_p = NULL;
    h.GetTP(); // creates correct t_p
    /*puts the elements of S with their shifts to T*/
    //    int atT, int uptodeg, int lV)
    strat->S_2_R[i] = strat->tl + 1; // the el't with shift 0 will be inserted first
    // need a small check for above; we insert >=1 elements
    // insert this check into kTest_TS ?
    enterTShift(h,strat,atT,uptodeg,lV);
  }
  /* what about setting strat->tl? */
}
#endif

#ifdef HAVE_SHIFTBBA
void initBuchMoraShift (ideal F,ideal Q,kStrategy strat)
{
  strat->interpt = BTEST1(OPT_INTERRUPT);
  strat->kHEdge=NULL;
  if (pOrdSgn==1) strat->kHEdgeFound=FALSE;
  /*- creating temp data structures------------------- -*/
  strat->cp = 0;
  strat->c3 = 0;
  strat->cv = 0;
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
  if (!TEST_OPT_SB_1)
  {
    /* the only change: we do not fill the set T*/
    updateS(FALSE,strat);
  }
  if (strat->fromQ!=NULL) omFreeSize(strat->fromQ,IDELEMS(strat->Shdl)*sizeof(int));
  strat->fromQ=NULL;
  /* more changes: fill the set T with all the shifts of elts of S*/
  /* is done by other procedure */
}
#endif

#ifdef HAVE_SHIFTBBA
/*1
* put the pairs (sh \dot s[i],p)  into the set B, ecart=ecart(p)
*/
void enterOnePairManyShifts (int i, poly p, int ecart, int isFromQ, kStrategy strat, int atR, int uptodeg, int lV)
{
  /* p comes from strat->P.p, that is LObject with LM in currRing and Tail in tailRing */

  assume(p_LmCheckIsFromRing(p,currRing));
  assume(p_CheckIsFromRing(pNext(p),strat->tailRing));

  /* cycles through all shifts of s[i] until uptodeg - lastVblock(s[i]) */
  /* that is create the pairs (f, s \dot g)  */

  poly qq = strat->S[i]; //  lm in currRing, tail in tailRing

  //  poly q = pCopy(pHead(strat->S[i])); // lm in currRing
  //  pNext(q) = prCopyR(pNext(strat->S[i]),strat->tailRing,currRing); // zero shift

 /* determine how many elements we have to insert for a given s[i] */
  /* x(0)y(1)z(2) : lastVblock-1=2, to add until lastVblock=uptodeg-1 */
  /* hence, a total number of elt's to add is: */
  /*  int toInsert = 1 + (uptodeg-1) - (pLastVblock(p.p, lV) -1);  */
  int toInsert =  itoInsert(qq, uptodeg,  lV, strat->tailRing);

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      //      Print("entered ManyShifts: with toInsert=%d",toInsert); PrintLn();
    }
#endif

  assume(i<=strat->sl); // from OnePair
  if (strat->interred_flag) return; // ?

  /* these vars hold for all shifts of s[i] */
  int ecartq = 0; //Hans says it's ok; we're in the homog case, no ecart

  int qfromQ;
  if (strat->fromQ != NULL)
  {
    qfromQ = strat->fromQ[i];
  }
  else
  {
    qfromQ = -1;
  }

  int j;

  poly q, s;

  // for the 0th shift: insert the orig. pair
  enterOnePairShift(qq, p, ecart, isFromQ, strat, -1, ecartq, qfromQ, j, i, uptodeg, lV);

  for (j=1; j<= toInsert; j++)
  {
    //    q = pLPshift(strat->S[i],j,uptodeg,lV);
    q = p_LPshiftT(qq, j, uptodeg, lV, strat, currRing);
    //    q = p_mLPshift(qq,j,uptodeg,lV,currRing); // lm in currRing, shift this monomial
    //    s = p_LPshift(pNext(qq), j, uptodeg, lV, strat->tailRing); // from tailRing
    //    pNext(q) = s; // in tailRing
    /* here we need to call enterOnePair with two polys ... */

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      //      PrintS("ManyShifts: calling enterOnePairShift(q,p)");      PrintLn();
    }
#endif
    enterOnePairShift(q, p, ecart, isFromQ, strat, -1, ecartq, qfromQ, j, i, uptodeg, lV);
  }
}
#endif

#ifdef HAVE_SHIFTBBA
/*1
* put the pairs (sh \dot qq,p)  into the set B, ecart=ecart(p)
* despite the name, not only self shifts
*/
void enterOnePairSelfShifts (poly qq, poly p, int ecart, int isFromQ, kStrategy strat, int atR, int uptodeg, int lV)
{

  /* format: p,qq are in LObject form: lm in CR, tail in TR */
  /* for true self pairs qq ==p  */
  /* we test both qq and p */
  assume(p_LmCheckIsFromRing(qq,currRing));
  assume(p_CheckIsFromRing(pNext(qq),strat->tailRing));
  assume(p_LmCheckIsFromRing(p,currRing));
  assume(p_CheckIsFromRing(pNext(p),strat->tailRing));

  /* since this proc is applied twice for (h, s*g) and (g,s*h), init j with 1 only */

  //  int j = 0;
  int j = 1;

  /* for such self pairs start with 1, not with 0 */
  if (qq == p) j=1;

  /* should cycle through all shifts of q until uptodeg - lastVblock(q) */
  /* that is create the pairs (f, s \dot g)  */

  int toInsert =  itoInsert(qq, uptodeg,  lV, strat->tailRing);

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      //      Print("entered SelfShifts: with toInsert=%d",toInsert); PrintLn();
    }
#endif

  poly q, s;

  if (strat->interred_flag) return; // ?

  /* these vars hold for all shifts of s[i] */
  int ecartq = 0; //Hans says it's ok; we're in the homog case, no ecart
  int qfromQ = 0; // strat->fromQ[i];

  for (; j<= toInsert; j++)
  {
    //    q = pLPshift(strat->S[i],j,uptodeg,lV);
    /* we increase shifts by one; must delete q there*/
    //    q = qq; q = pMoveCurrTail2poly(q,strat);
    //    q = pLPshift(q,j,uptodeg,lV); //,currRing);
    q = p_LPshiftT(qq, j, uptodeg, lV, strat, currRing);
    //    q = p_mLPshift(qq,j,uptodeg,lV,currRing); // lm in currRing, shift this monomial
    //    s = p_LPshift(pNext(qq), j, uptodeg, lV, strat->tailRing); // from tailRing
    //    pNext(q) = s; // in tailRing
    /* here we need to call enterOnePair with two polys ... */
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      //      PrintS("SelfShifts: calling enterOnePairShift(q,p)");      PrintLn();
    }
#endif
    enterOnePairShift(q, p, ecart, isFromQ, strat, -1, ecartq, qfromQ, j, -1, uptodeg, lV);
  }
}
#endif

#ifdef HAVE_SHIFTBBA
/*2
* put the pair (q,p)  into the set B, ecart=ecart(p), q is the shift of some s[i]
*/
void enterOnePairShift (poly q, poly p, int ecart, int isFromQ, kStrategy strat, int atR, int ecartq, int qisFromQ, int shiftcount, int ifromS, int uptodeg, int lV)
{

  /* Format: q and p are like strat->P.p, so lm in CR, tail in TR */

  /* check this Formats: */
  assume(p_LmCheckIsFromRing(q,currRing));
  assume(p_CheckIsFromRing(pNext(q),strat->tailRing));
  assume(p_LmCheckIsFromRing(p,currRing));
  assume(p_CheckIsFromRing(pNext(p),strat->tailRing));

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
//       PrintS("enterOnePairShift(q,p) invoked with q = ");
//       wrp(q); //      wrp(pHead(q));
//       PrintS(", p = ");
//       wrp(p); //wrp(pHead(p));
//       PrintLn();
    }
#endif

  /* poly q stays for s[i], ecartq = ecart(q), qisFromQ = applies to q */

  int qfromQ = qisFromQ;

  /* need additionally: int up_to_degree, poly V0 with the variables in (0)  or just the number lV = the length of the first block */

  if (strat->interred_flag) return;

  int      l,j,compare;
  LObject  Lp;
  Lp.i_r = -1;

#ifdef KDEBUG
  Lp.ecart=0; Lp.length=0;
#endif
  /*- computes the lcm(s[i],p) -*/
  Lp.lcm = pInit();

  pLcm(p,q, Lp.lcm); // q is what was strat->S[i], so a poly in LM/TR presentation
  pSetm(Lp.lcm);

  /* apply the V criterion */
  if (!isInV(Lp.lcm, lV))
  {
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("V crit applied to q = ");
      wrp(q); //      wrp(pHead(q));
      PrintS(", p = ");
      wrp(p); //wrp(pHead(p));
      PrintLn();
    }
#endif
    pLmFree(Lp.lcm);
    Lp.lcm=NULL;
    /* + counter for applying the V criterion */
    strat->cv++;
    return;
  }

  if (strat->sugarCrit && ALLOW_PROD_CRIT(strat))
  {
    if((!((ecartq>0)&&(ecart>0)))
    && pHasNotCF(p,q))
    {
    /*
    *the product criterion has applied for (s,p),
    *i.e. lcm(s,p)=product of the leading terms of s and p.
    *Suppose (s,r) is in L and the leading term
    *of p divides lcm(s,r)
    *(==> the leading term of p divides the leading term of r)
    *but the leading term of s does not divide the leading term of r
    *(notice that this condition is automatically satisfied if r is still
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
      Lp.ecart = si_max(ecart,ecartq);
    if (strat->fromT && (ecartq>ecart))
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
          if ((strat->fromQ==NULL) || (isFromQ==0) || (qfromQ==0))
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
    if (ALLOW_PROD_CRIT(strat))
    {
      // if currRing->nc_type!=quasi (or skew)
      // TODO: enable productCrit for super commutative algebras...
      if(/*(strat->ak==0) && productCrit(p,strat->S[i])*/
      pHasNotCF(p,q))
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
          strat->cp++;
          pLmFree(Lp.lcm);
          Lp.lcm=NULL;
          return;
      }
      if (strat->fromT && (ecartq>ecart))
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
          if ((strat->fromQ==NULL) || (isFromQ==0) || (qfromQ==0))
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
  }
  /*
  *the pair (S[i],p) enters B if the spoly != 0
  */
  /*-  compute the short s-polynomial -*/
  if (strat->fromT && !TEST_OPT_INTSTRATEGY)
    pNorm(p);
  if ((q==NULL) || (p==NULL))
    return;
  if ((strat->fromQ!=NULL) && (isFromQ!=0) && (qfromQ!=0))
    Lp.p=NULL;
  else
  {
//     if ( rIsPluralRing(currRing) )
//     {
//       if(pHasNotCF(p, q))
//       {
//         if(ncRingType(currRing) == nc_lie)
//         {
//             // generalized prod-crit for lie-type
//             strat->cp++;
//             Lp.p = nc_p_Bracket_qq(pCopy(p),q);
//         }
//         else
//         if( ALLOW_PROD_CRIT(strat) )
//         {
//             // product criterion for homogeneous case in SCA
//             strat->cp++;
//             Lp.p = NULL;
//         }
//         else
//           Lp.p = nc_CreateSpoly(q,p,currRing); // ?
//       }
//       else  Lp.p = nc_CreateSpoly(q,p,currRing);
//     }
//     else
//     {

    /* ksCreateShortSpoly needs two Lobject-kind presentations */
    /* p is already in this form, so convert q */
    //    q = pMove2CurrTail(q, strat);
    Lp.p = ksCreateShortSpoly(q, p, strat->tailRing);
      //  }
  }
  if (Lp.p == NULL)
  {
    /*- the case that the s-poly is 0 -*/
    /* TEMPORARILY DISABLED FOR SHIFTS because there is no i*/
//      if (strat->pairtest==NULL) initPairtest(strat);
//      strat->pairtest[i] = TRUE;/*- hint for spoly(S^[i],p)=0 -*/
//      strat->pairtest[strat->sl+1] = TRUE;
    /* END _ TEMPORARILY DISABLED FOR SHIFTS */
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
    /* both of them should have their LM in currRing and TAIL in tailring */
    Lp.p1 = q;  // already in the needed form
    Lp.p2 = p; // already in the needed form

    if ( !rIsPluralRing(currRing) )
      pNext(Lp.p) = strat->tail;

    /* TEMPORARILY DISABLED FOR SHIFTS because there's no i*/
    /* at the beginning we DO NOT set atR = -1 ANYMORE*/
    if ( (atR >= 0) && (shiftcount==0) && (ifromS >=0) )
    {
      Lp.i_r1 = kFindInT(Lp.p1,strat); //strat->S_2_R[ifromS];
      Lp.i_r2 = atR;
    }
    else
    {
      /* END _ TEMPORARILY DISABLED FOR SHIFTS */
      Lp.i_r1 = -1;
      Lp.i_r2 = -1;
     }
    strat->initEcartPair(&Lp,q,p,ecartq,ecart);

    if (TEST_OPT_INTSTRATEGY)
    {
      if (!rIsPluralRing(currRing))
        nDelete(&(Lp.p->coef));
    }

    l = strat->posInL(strat->B,strat->Bl,&Lp,strat);
    enterL(&strat->B,&strat->Bl,&strat->Bmax,Lp,l);
  }
}
#endif

#ifdef HAVE_SHIFTBBA
/*2
*(s[0],h),...,(s[k],h) will be put to the pairset L(via initenterpairs)
*superfluous elements in S will be deleted
*/
void enterpairsShift (poly h,int k,int ecart,int pos,kStrategy strat, int atR,int uptodeg, int lV)
{
  /* h is strat->P.p, that is LObject with LM in currRing and Tail in tailRing */
  /* Q: what is exactly the strat->fromT ? A: a local case trick; don't need it yet*/
  int j=pos;

#ifdef HAVE_RINGS
  assume (!rField_is_Ring(currRing));
#endif
  initenterpairsShift(h,k,ecart,0,strat, atR,uptodeg,lV);
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
#endif

#ifdef HAVE_SHIFTBBA
/*3
*(s[0], s \dot h),...,(s[k],s \dot h) will be put to the pairset L
* also the pairs (h, s\dot s[0]), ..., (h, s\dot s[k]) enter L
* additionally we put the pairs (h, s \sdot h) for s>=1 to L
*/
void initenterpairsShift (poly h,int k,int ecart,int isFromQ, kStrategy strat, int atR, int uptodeg, int lV)
{
  /* h comes from strat->P.p, that is LObject with LM in currRing and Tail in tailRing */
  //  atR = -1;
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
            enterOnePairManyShifts(j,h,ecart,isFromQ,strat, atR,uptodeg,lV);
            // other side pairs:
            enterOnePairSelfShifts(h,strat->S[j],ecart,isFromQ,strat, atR,uptodeg,lV);
          //Print("j:%d, Ll:%d\n",j,strat->Ll);
          }
        }
      }
      else
      {
        new_pair=TRUE;
        for (j=0; j<=k; j++)
        {
          enterOnePairManyShifts(j,h,ecart,isFromQ,strat, atR,uptodeg,lV);
          // other side pairs
          enterOnePairSelfShifts(h,strat->S[j],ecart,isFromQ,strat, atR,uptodeg,lV);
        }
        /* HERE we put (h, s*h) pairs */
       /* enterOnePairSelfShifts (poly qq, poly p, int ecart, int isFromQ, kStrategy strat, int atR, int uptodeg, int lV); */
       enterOnePairSelfShifts (h, h, ecart, isFromQ, strat, atR, uptodeg, lV);
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
          enterOnePairManyShifts(j,h,ecart,isFromQ,strat, atR, uptodeg, lV);
          // other side pairs
          enterOnePairSelfShifts(h,strat->S[j],ecart,isFromQ,strat, atR,uptodeg,lV);
        //Print("j:%d, Ll:%d\n",j,strat->Ll);
        }
      }
      /* HERE we put (h, s*h) pairs */
      enterOnePairSelfShifts (h, h, ecart, isFromQ, strat, atR, uptodeg, lV);
    }

    if (new_pair)
    {
      strat->chainCrit(h,ecart,strat);
    }

  }
}
#endif

#ifdef HAVE_SHIFTBBA
/*2
* puts p to the set T, starting with the at position atT
* and inserts all admissible shifts of p
*/
void enterTShift(LObject p, kStrategy strat, int atT, int uptodeg, int lV)
{
  /* determine how many elements we have to insert */
  /* x(0)y(1)z(2) : lastVblock-1=2, to add until lastVblock=uptodeg-1 */
  /* hence, a total number of elt's to add is: */
  /*  int toInsert = 1 + (uptodeg-1) - (pLastVblock(p.p, lV) -1);  */

  int toInsert =  itoInsert(p.p, uptodeg,  lV, strat->tailRing);

#ifdef PDEBUG
  //  Print("enterTShift uses toInsert: %d", toInsert);  PrintLn();
#endif
  int i;

  if (atT < 0)
    atT = strat->posInT(strat->T, strat->tl, p);

  /* can call enterT in a sequence, e.g. */

  /* shift0 = it's our model for further shifts */
  enterT(p,strat,atT);
  LObject qq;
  for (i=1; i<=toInsert; i++) // toIns - 1?
  {
    qq      = p; //qq.Copy();
    qq.p    = NULL;
    qq.max  = NULL;
    qq.t_p = p_LPshift(p_Copy(p.t_p,strat->tailRing), i, uptodeg, lV, strat->tailRing); // direct shift
    qq.GetP();
    // update q.sev
    qq.sev = pGetShortExpVector(qq.p);
    /* enter it into T, first el't is with the shift 0 */
    // compute the position for qq
    atT = strat->posInT(strat->T, strat->tl, qq);
    enterT(qq,strat,atT);
  }
/* Q: what to do with this one in the orig enterT ? */
/*  strat->R[strat->tl] = &(strat->T[atT]); */
/* Solution: it is done by enterT each time separately */
}
#endif

#ifdef HAVE_SHIFTBBA
poly redtailBbaShift (LObject* L, int pos, kStrategy strat, BOOLEAN withT, BOOLEAN normalize)
{
  /* for the shift case need to run it with withT = TRUE */
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
      if (withT)
      {
        int j;
        j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, &Ln);
        if (j < 0) break;
        With = &(strat->T[j]);
      }
      else
      {
        With = kFindDivisibleByInS(strat, pos, &Ln, &With_s);
        if (With == NULL) break;
      }
      if (normalize && (!TEST_OPT_INTSTRATEGY) && (!nIsOne(pGetCoeff(With->p))))
      {
        With->pNorm();
        //if (TEST_OPT_PROT) { PrintS("n"); mflush(); }
      }
      strat->redTailChange=TRUE;
      if (ksReducePolyTail(L, With, &Ln))
      {
        // reducing the tail would violate the exp bound
        //  set a flag and hope for a retry (in bba)
        strat->completeReduce_retry=TRUE;
        if ((Ln.p != NULL) && (Ln.t_p != NULL)) Ln.p=NULL;
        do
        {
          pNext(h) = Ln.LmExtractAndIter();
          pIter(h);
          L->pLength++;
        } while (!Ln.IsNull());
        goto all_done;
      }
      if (Ln.IsNull()) goto all_done;
      if (! withT) With_s.Init(currRing);
    }
    pNext(h) = Ln.LmExtractAndIter();
    pIter(h);
    L->pLength++;
  }

  all_done:
  Ln.Delete();
  if (L->p != NULL) pNext(L->p) = pNext(p);

  if (strat->redTailChange)
  {
    L->last = NULL;
    L->length = 0;
  }
  L->Normalize(); // HANNES: should have a test
  kTest_L(L);
  return L->GetLmCurrRing();
}
#endif
