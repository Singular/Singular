/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kutil.cc,v 1.45 1999-10-26 17:15:19 Singular Exp $ */
/*
* ABSTRACT: kernel: utils for kStd
*/

#include <stdlib.h>
#include <string.h>
#include "mod2.h"
#include "tok.h"
#include "febase.h"
#include "mmemory.h"
#include "numbers.h"
#include "polys.h"
#include "ring.h"
#include "ideals.h"
#include "timer.h"
#include "cntrlc.h"
#include "stairc.h"
#include "subexpr.h"
#include "kstd1.h"
#include "kutil.h"

static poly redMora (poly h,int maxIndex,kStrategy strat);
static poly redBba (poly h,int maxIndex,kStrategy strat);

BITSET  test=(BITSET)0;
int     HCord;
int        Kstd1_deg;
int        mu=32000;

/*2
*deletes higher monomial of p, re-compute ecart and length
*works only for orderings with ecart =pFDeg(end)-pFDeg(start)
*/
void deleteHC(poly* p, int* e, int* l,kStrategy strat)
{
  poly p1;

  if (strat->kHEdgeFound)
  {
    if (pComp(*p,strat->kNoether) == -1)
    {
      pDelete(p);
      *l = 0;
      *e = -1;
      return;
    }
    p1 = *p;
    while (pNext(p1)!=NULL)
    {
      if (pComp0(pNext(p1), strat->kNoether) == -1)
        pDelete(&pNext(p1));
      else
        pIter(p1);
    }
    *e = pLDeg(*p,l)-pFDeg(*p);
  }
}

/*2
*tests if p.p=monomial*unit and cancels the unit
*/
void cancelunit (LObject* p)
{
  int  i;
  poly h;

  if(pIsVector((*p).p))
  {
    if(!pOneComp((*p).p)) return;
  }
  if ((*p).ecart != 0)
  {
    for(i=1;i<=pVariables;i++)
    {
      if ((pGetExp((*p).p,1)>0) && (rIsPolyVar(i)==TRUE)) return;
    }
    h = pNext(((*p).p));
    loop
    {
      if (h==NULL)
      {
        pDelete(&(pNext((*p).p)));
        (*p).ecart = 0;
        (*p).length = 1;
        return;
      }
      i = 0;
      loop
      {
        i++;
        if (pGetExp((*p).p,i) > pGetExp(h,i)) return ;
        if (i == pVariables) break;
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
  if (pLexOrder)
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
inline static intset initec (int maxnr)
{
  return (intset)Alloc(maxnr*sizeof(int));
}

inline static unsigned long* initsevS (int maxnr)
{
  return (unsigned long*)Alloc0(maxnr*sizeof(unsigned long));
}

static inline void enlargeT (TSet* T,int* length,int incr)
{
  *T = (TSet)ReAlloc0((ADDRESS)(*T),(*length)*sizeof(TObject),
                      ((*length)+incr)*sizeof(TObject));
  (*length) += incr;
}

void cleanT (kStrategy strat)
{
  int i,j;
  poly  p;

  for (j=0; j<=strat->tl; j++)
  {
    p = strat->T[j].p;
    strat->T[j].p=NULL;
    i = -1;
    loop
    {
      i++;
      if (i>strat->sl)
      {
        if (strat->T[j].heap != NULL)
          pHeapDelete(&p, strat->T[j].heap);
        else
          pDelete(&p);
        break;
      }
      if (p == strat->S[i])
      {
#if 0
        if (strat->T[j].heap != NULL)
          strat->S[i]
            = pShallowCopyDelete(mm_specHeap, &p, strat->T[j].heap);
#endif
        break;
      }
    }
  }
  strat->tl=-1;
}

LSet initL ()
{
  return (LSet)Alloc(setmax*sizeof(LObject));
}

static inline void enlargeL (LSet* L,int* length,int incr)
{
  LSet h;

  *L = (LSet)ReAlloc((ADDRESS)(*L),(*length)*sizeof(LObject),
                                   ((*length)+incr)*sizeof(LObject));
  (*length) += incr;
}

void initPairtest(kStrategy strat)
{
  strat->pairtest = (BOOLEAN *)Alloc0((strat->sl+2)*sizeof(BOOLEAN));
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

#ifdef KDEBUG
BOOLEAN K_Test_L(char *f , int l, LObject *L,
                 BOOLEAN testp, int lpos, TSet T, int tlength)
{
  BOOLEAN ret = TRUE;

  if (testp)
  {
    if (! pDBTest(L->p, L->heap, f, l))
    {
      Warn("for L->p\n");
      ret = FALSE;
    }
  }

  if (L->pLength != 0 && L->pLength != pLength(L->p))
  {
    Warn("L[%d] length error: has %d, specified to have %d\n",
          lpos, pLength(L->p), L->pLength);
    ret = FALSE;
  }
  if (L->p1 == NULL)
  {
    // L->p2 either NULL or poly from global heap
    ret &= pDBTest(L->p2, f, l);
  }
  else if (tlength > 0 && T != NULL)
  {
    // now p1 and p2 must be != NULL and must be contained in T
    int i;
    for (i=0; i<tlength; i++)
      if (L->p1 == T[i].p) break;
    if (i>=tlength)
    {
      Warn("L[%d].p1 not in T \n",lpos);
      ret = FALSE;
    }
    for (i=0; i<tlength; i++)
      if (L->p2 == T[i].p) break;
    if (i>=tlength)
    {
      Warn("L[%d].p2 not in T \n",lpos);
      ret &= FALSE;
    }
  }
  return ret;
}

BOOLEAN K_Test (char *f, int l, kStrategy strat, int pref)
{
  int i;
  BOOLEAN ret = TRUE;
  // test P
#ifdef MDEBUG
  if (pref) mmStartReferenceWatch();
#endif
  ret = K_Test_L(f, l, &(strat->P),
                 (strat->P.p != NULL && pNext(strat->P.p) != strat->tail),
                 -1, strat->T, strat->tl+1);

  if (ret == FALSE)
  {
    Warn("for strat->P\n");
  }

  // test T
  if (strat->T != NULL)
  {
#ifdef MDEBUG
    if (! pref) mmStartReferenceWatch();
#endif
    for (i=0; i<=strat->tl; i++)
    {
      if (K_Test_T(f, l, &(strat->T[i]), i) == FALSE)
      {
        ret = FALSE;
      }
    }
  }
#ifdef MDEBUG
  mmStopReferenceWatch();
#endif
  // test L
  if (strat->L != NULL)
  {
    for (i=0; i<=strat->Ll; i++)
    {
      if (strat->L[i].p == NULL)
      {
        Warn("L[%d].p is NULL\n", i);
        ret = FALSE;
      }
      if (K_Test_L(f, l, &(strat->L[i]),
                   (pNext(strat->L[i].p) != strat->tail), i,
                   strat->T, strat->tl + 1) == FALSE)
      {
        Warn("for strat->L[%d]\n", i);
        ret = FALSE;
      }
    }
  }
  // test S
  if (strat->S != NULL)
    ret = ret &&  K_Test_S(f, l, strat);

  return ret;
}

BOOLEAN K_Test_S(char* f, int l, kStrategy strat)
{
  int i;
  BOOLEAN ret = TRUE;
  for (i=0; i<=strat->sl; i++)
  {
    if (strat->S[i] != NULL && strat->sevS[i] != 0 && strat->sevS[i] !=
        pGetShortExpVector(strat->S[i]))
    {
      Warn("S[%d] wrong sev: has %o, specified to have %o in %s:%d\n",
           i , pGetShortExpVector(strat->S[i]), strat->sevS[i],f, l);
      ret = FALSE;
    }
  }
  return ret;
}


BOOLEAN K_Test_T(char* f, int l, TObject * T, int i)
{
  BOOLEAN ret = pDBTest(T->p, T->heap, f, l);
  if (ret == FALSE) Warn("for T[%d]\n", i);
  if (T->pLength != 0 &&
      T->pLength != pLength(T->p))
  {
    Warn("T[%d] length error: has %d, specified to have %d in %s:%d\n",
          i , pLength(T->p), T->pLength,f, l);
    ret = FALSE;
  }
  if (T->sev != 0 && pGetShortExpVector(T->p) != T->sev)
  {
    Warn("T[%d] wrong sev: has %o, specified to have %o in %s:%d\n",
          i , pGetShortExpVector(T->p), T->sev,f, l);
    ret = FALSE;
  }
  return ret;
}



BOOLEAN K_Test_TS(char *f, int l, kStrategy strat)
{
  int i, j;
  BOOLEAN ret = TRUE;
  K_Test(f, l, strat);

  // test S
  if (strat->S != NULL)
  {
    for (i=0; i<=strat->sl; i++)
    {
      for (j=0; j<=strat->tl; j++)
        if (strat->S[i] == strat->T[j].p) break;
      if (j > strat->tl)
      {
        Warn("S[%d] not in T\n", i);
        ret = FALSE;
      }
    }
  }
  return ret;
}

#endif

/*2
*cancels the i-th polynomial in the standardbase s
*/
void deleteInS (int i,kStrategy strat)
{
  int j;

  for (j=i; j<strat->sl; j++)
  {
    strat->S[j] = strat->S[j+1];
    strat->ecartS[j] = strat->ecartS[j+1];
    strat->sevS[j] = strat->sevS[j+1];
  }
  if (strat->fromQ!=NULL)
  {
    for (j=i; j<strat->sl; j++)
    {
      strat->fromQ[j] = strat->fromQ[j+1];
    }
  }
  strat->S[strat->sl] = NULL;
  strat->sl--;
}

/*2
*cancels the j-th polynomial in the set
*/
void deleteInL (LSet set, int *length, int j,kStrategy strat)
{
  int i;

  if (set[j].lcm!=NULL)
    pFree1(set[j].lcm);
  if (set[j].p!=NULL)
  {
    if (pNext(set[j].p) == strat->tail)
    {
      pFree1(set[j].p);
      /*- tail belongs to several int spolys -*/
    }
    else
    {
      // search p in T, if it is there, do not delete it
      int i=strat->tl;
      poly p=set[j].p;
      if (p!=NULL)
      loop
      {
        if (i < 0)
        {
          //if (strat->next!=NULL)
          //{
          //  strat=strat->next;
          //  i=strat->tl;
          //}
          //else
          {
            /* not found : */
            pDelete(&p);
            break;
          }
        }
        else
        {
          if (strat->T[i].p==p)
          {
            /* found : */
            p=NULL;
            break;
          }
          i--;
        }
      }
    }
    set[j].p=NULL;
  }
  if ((*length)>0)
  {
    for (i=j; i < (*length); i++)
      set[i] = set[i+1];
  }
#ifdef KDEBUG
  memset(&(set[*length]),0,sizeof(LObject));
#endif
  (*length)--;
}

/*2
*is used after updating the pairset,if the leading term of p
*devides the leading term of some S[i] it will be canceled
*/
void clearS (poly p, int* at, int* k,kStrategy strat)
{
  unsigned long sev = pGetShortExpVector(p);
  if (!pShortDivisibleBy(p,sev, strat->S[*at], ~ strat->sevS[*at])) return;
  deleteInS((*at),strat);
  (*at)--;
  (*k)--;
}

/*2
*enters p at position at in L
*/
void enterL (LSet *set,int *length, int *LSetmax, LObject p,int at)
{
  int i;

  if ((*length)>=0)
  {
    if ((*length) == (*LSetmax)-1) enlargeL(set,LSetmax,setmax);
    for (i=(*length)+1; i>=at+1; i--) (*set)[i] = (*set)[i-1];
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
  h->ecart = pLDeg(h->p,&(h->length))-pFDeg(h->p);
}

void initEcartBBA (LObject* h)
{
  (*h).ecart = 0;
//#ifdef KDEBUG
  (*h).length = 0;
//#endif
}

void initEcartPairBba (LObject* Lp,poly f,poly g,int ecartF,int ecartG)
{
//#ifdef KDEBUG
  (*Lp).ecart = 0;
  (*Lp).length = 0;
//#endif
}

void initEcartPairMora (LObject* Lp,poly f,poly g,int ecartF,int ecartG)
{
  (*Lp).ecart = max(ecartF,ecartG);
  (*Lp).ecart = (*Lp).ecart-(pFDeg((*Lp).p)-pFDeg((*Lp).lcm));
//#ifdef KDEBUG
  (*Lp).length = 0;
//#endif
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
void enterOnePair (int i,poly p,int ecart, int isFromQ,kStrategy strat)
{
  int      l,j,compare;
  LObject  Lp;

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
    *of p devides lcm(s,r)
    *(==> the leading term of p devides the leading term of r)
    *but the leading term of s does not devide the leading term of r
    *(notice that tis condition is automatically satisfied if r is still
    *in S), then (s,r) can be canceled.
    *This should be done here because the
    *case lcm(s,r)=lcm(s,p) is not covered by chainCrit.
    */
      strat->cp++;
      pFree1(Lp.lcm);
      Lp.lcm=NULL;
      return;
    }
    else
      Lp.ecart = max(ecart,strat->ecartS[i]);
    if (strat->fromT && (strat->ecartS[i]>ecart))
    {
      pFree1(Lp.lcm);
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
            pFree1(Lp.lcm);
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
      pFree1(Lp.lcm);
      Lp.lcm=NULL;
      return;
    }
    if (strat->fromT && (strat->ecartS[i]>ecart))
    {
      pFree1(Lp.lcm);
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
          pFree1(Lp.lcm);
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
    Lp.p = ksCreateShortSpoly(strat->S[i],p);
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
    if (Lp.lcm!=NULL) pFree1(Lp.lcm);
  }
  else
  {
    /*- the pair (S[i],p) enters B -*/
    Lp.p1 = strat->S[i];
    Lp.p2 = p;
    pNext(Lp.p) = strat->tail;
    strat->initEcartPair(&Lp,strat->S[i],p,strat->ecartS[i],ecart);
    if (TEST_OPT_INTSTRATEGY)
    {
      nDelete(&(Lp.p->coef));
    }
    l = strat->posInL(strat->B,strat->Bl,Lp,strat);
    enterL(&strat->B,&strat->Bl,&strat->Bmax,Lp,l);
  }
}

/*2
* put the pair (s[i],p) into the set L, ecart=ecart(p)
* in the case that s forms a SB of (s)
*/
void enterOnePairSpecial (int i,poly p,int ecart,kStrategy strat)
{
  int      l,j,compare;
  LObject  Lp;

  Lp.lcm = pInit();
  pLcm(p,strat->S[i],Lp.lcm);
  pSetm(Lp.lcm);
  if(pHasNotCF(p,strat->S[i]))
  {
    strat->cp++;
    pFree1(Lp.lcm);
    Lp.lcm=NULL;
    return;
  }
  for(j = strat->Ll;j>=0;j--)
  {
    compare=pDivComp(strat->L[j].lcm,Lp.lcm);
    if ((compare==1) || (pEqual(strat->L[j].lcm,Lp.lcm)))
    {
      strat->c3++;
      pFree1(Lp.lcm);
      return;
    }
    else if (compare ==-1)
    {
      deleteInL(strat->L,&strat->Ll,j,strat);
      strat->c3++;
    }
  }
  /*-  compute the short s-polynomial -*/

  Lp.p = ksCreateShortSpoly(strat->S[i],p);
  if (Lp.p == NULL)
  {
     pFree1(Lp.lcm);
  }
  else
  {
    /*- the pair (S[i],p) enters B -*/
    Lp.p1 = strat->S[i];
    Lp.p2 = p;
    pNext(Lp.p) = strat->tail;
    strat->initEcartPair(&Lp,strat->S[i],p,strat->ecartS[i],ecart);
    if (TEST_OPT_INTSTRATEGY)
    {
      nDelete(&(Lp.p->coef));
    }
    l = strat->posInL(strat->L,strat->Ll,Lp,strat);
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
    Free((ADDRESS)strat->pairtest,(strat->sl+2)*sizeof(BOOLEAN));
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
          if (pEqual(strat->B[j].lcm,strat->B[i].lcm))
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
          if (pEqual(strat->B[j].lcm,strat->B[i].lcm))
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
      j = strat->posInL(strat->L,j-1,strat->B[i],strat);
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
      j = strat->posInL(strat->L,j,strat->B[i],strat);
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
          if ((strat->L[i].p2 == p) && pEqual(strat->L[j].lcm,strat->L[i].lcm))
          {
            /*L[i] could be canceled but we search for a better one to cancel*/
            strat->c3++;
            if (isInPairsetL(i-1,strat->L[j].p1,strat->L[i].p1,&l,strat)
            && (pNext(strat->L[l].p) == strat->tail)
            && (!pEqual(strat->L[i].p,strat->L[l].p))
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
void initenterpairs (poly h,int k,int ecart,int isFromQ,kStrategy strat)
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
            enterOnePair(j,h,ecart,isFromQ,strat);
          //Print("j:%d, Ll:%d\n",j,strat->Ll);
          }
        }
      }
      else
      {
        new_pair=TRUE;
        for (j=0; j<=k; j++)
        {
          enterOnePair(j,h,ecart,isFromQ,strat);
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
          enterOnePair(j,h,ecart,isFromQ,strat);
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
void enterpairs (poly h,int k,int ecart,int pos,kStrategy strat)
{
  int j=pos;

  initenterpairs(h,k,ecart,0,strat);
  if ((!strat->fromT)
  && ((strat->syzComp==0)
    ||(pGetComp(h)<=strat->syzComp)))
  {
    //Print("start clearS k=%d, pos=%d, sl=%d\n",k,pos,strat->sl);
    loop
    {
      if (j > k) break;
      clearS(h,&j,&k,strat);
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
void enterpairsSpecial (poly h,int k,int ecart,int pos,kStrategy strat)
{
  int j;

  for (j=0; j<=k; j++)
  {
    if ((pGetComp(h)==pGetComp(strat->S[j]))
    || (0==pGetComp(strat->S[j])))
    {
      enterOnePairSpecial(j,h,ecart,strat);
    }
  }
  j=pos;
  loop
  {
    if (j > k) break;
    clearS(h,&j,&k,strat);
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
        if (pShortDivisibleBy(strat->S[i], strat->sevS[i],
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
  int i,j,at,ecart;
  int fq=0;
  unsigned long sev;
  poly  p;

  *suc = -1;
  for (i=1; i<=strat->sl; i++)
  {
    at = posInS(strat->S,i-1,strat->S[i]);
    if (at != i)
    {
      if ((*suc > at) || (*suc == -1)) *suc = at;
      p = strat->S[i];
      ecart = strat->ecartS[i];
      sev = strat->sevS[i];
      if (strat->fromQ!=NULL) fq=strat->fromQ[i];
      for (j=i; j>=at+1; j--)
      {
        strat->S[j] = strat->S[j-1];
        strat->ecartS[j] = strat->ecartS[j-1];
        strat->sevS[j] = strat->sevS[j-1];
      }
      strat->S[at] = p;
      strat->ecartS[at] = ecart;
      strat->sevS[at] = sev;
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
int posInS (polyset set,int length,poly p)
{
  if(length==-1) return 0;
  int i;
  int an = 0;
  int en= length;
  if (pMixedOrder)
  {
    int o=pWTotaldegree(p);
    int oo=pWTotaldegree(set[length]);

    if ((oo<o)
    || ((o==oo) && (pComp0(set[length],p)!= pOrdSgn)))
      return length+1;

    loop
    {
      if (an >= en-1)
      {
        if ((pWTotaldegree(set[an])>=o) && (pComp0(set[an],p) == pOrdSgn))
        {
          return an;
        }
        return en;
      }
      i=(an+en) / 2;
      if ((pWTotaldegree(set[an])>=o)
      && (pComp0(set[i],p) == pOrdSgn)) en=i;
      else                              an=i;
    }
  }
  else
  {
    if (pComp0(set[length],p)!= pOrdSgn)
      return length+1;

    loop
    {
      if (an >= en-1)
      {
        if (pComp0(set[an],p) == pOrdSgn) return an;
        return en;
      }
      i=(an+en) / 2;
      if (pComp0(set[i],p) == pOrdSgn) en=i;
      else                             an=i;
    }
  }
}


/*2
* looks up the position of p in set
* the position is the last one
*/
int posInT0 (const TSet set,const int length,const LObject &p)
{
  return (length+1);
}


/*2
* looks up the position of p in T
* set[0] is the smallest with respect to the ordering-procedure
* pComp
*/
int posInT1 (const TSet set,const int length,const LObject &p)
{
  if (length==-1) return 0;

  if (pComp0(set[length].p,p.p)!= pOrdSgn) return length+1;

  int i;
  int an = 0;
  int en= length;

  loop
  {
    if (an >= en-1)
    {
      if (pComp0(set[an].p,p.p) == pOrdSgn) return an;
      return en;
    }
    i=(an+en) / 2;
    if (pComp0(set[i].p,p.p) == pOrdSgn) en=i;
    else                                 an=i;
  }
}

/*2
* looks up the position of p in T
* set[0] is the smallest with respect to the ordering-procedure
* length
*/
int posInT2 (const TSet set,const int length,const LObject &p)
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
int posInT11 (const TSet set,const int length,const LObject &p)
/*{
 * int j=0;
 * int o;
 *
 * o = pFDeg(p.p);
 * loop
 * {
 *   if ((pFDeg(set[j].p) > o)
 *   || ((pFDeg(set[j].p) == o) && (pComp0(set[j].p,p.p) == pOrdSgn)))
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

  int o = pFDeg(p.p);
  int op = pFDeg(set[length].p);

  if ((op < o)
  || ((op == o) && (pComp0(set[length].p,p.p) != pOrdSgn)))
    return length+1;

  int i;
  int an = 0;
  int en= length;

  loop
  {
    if (an >= en-1)
    {
      op= pFDeg(set[an].p);
      if ((op > o)
      || (( op == o) && (pComp0(set[an].p,p.p) == pOrdSgn)))
        return an;
      return en;
    }
    i=(an+en) / 2;
    op = pFDeg(set[i].p);
    if (( op > o)
    || (( op == o) && (pComp0(set[i].p,p.p) == pOrdSgn)))
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
int posInT110 (const TSet set,const int length,const LObject &p)
{
  if (length==-1) return 0;

  int o = pFDeg(p.p);
  int op = pFDeg(set[length].p);

  if (( op < o)
  || (( op == o) && (set[length].length<p.length))
  || (( op == o) && (set[length].length == p.length)
     && (pComp0(set[length].p,p.p) != pOrdSgn)))
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op = pFDeg(set[an].p);
      if (( op > o)
      || (( op == o) && (set[an].length > p.length))
      || (( op == o) && (set[an].length == p.length)
         && (pComp0(set[an].p,p.p) == pOrdSgn)))
        return an;
      return en;
    }
    i=(an+en) / 2;
    op = pFDeg(set[i].p);
    if (( op > o)
    || (( op == o) && (set[i].length > p.length))
    || (( op == o) && (set[i].length == p.length)
       && (pComp0(set[i].p,p.p) == pOrdSgn)))
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
int posInT13 (const TSet set,const int length,const LObject &p)
{
  if (length==-1) return 0;

  int o = pFDeg(p.p);

  if (pFDeg(set[length].p) <= o)
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      if (pFDeg(set[an].p) > o)
        return an;
      return en;
    }
    i=(an+en) / 2;
    if (pFDeg(set[i].p) > o)
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
int posInT15 (const TSet set,const int length,const LObject &p)
/*{
 *int j=0;
 * int o;
 *
 * o = pFDeg(p.p)+p.ecart;
 * loop
 * {
 *   if ((pFDeg(set[j].p)+set[j].ecart > o)
 *   || ((pFDeg(set[j].p)+set[j].ecart == o)
 *     && (pComp0(set[j].p,p.p) == pOrdSgn)))
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

  int o = pFDeg(p.p) + p.ecart;
  int op = pFDeg(set[length].p)+set[length].ecart;

  if ((op < o)
  || ((op == o)
     && (pComp0(set[length].p,p.p) != pOrdSgn)))
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op = pFDeg(set[an].p)+set[an].ecart;
      if (( op > o)
      || (( op  == o) && (pComp0(set[an].p,p.p) == pOrdSgn)))
        return an;
      return en;
    }
    i=(an+en) / 2;
    op = pFDeg(set[i].p)+set[i].ecart;
    if (( op > o)
    || (( op == o) && (pComp0(set[i].p,p.p) == pOrdSgn)))
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
int posInT17 (const TSet set,const int length,const LObject &p)
/*
*{
* int j=0;
* int  o;
*
*  o = pFDeg(p.p)+p.ecart;
*  loop
*  {
*    if ((pFDeg(set[j].p)+set[j].ecart > o)
*    || (((pFDeg(set[j].p)+set[j].ecart == o)
*      && (set[j].ecart < p.ecart)))
*    || ((pFDeg(set[j].p)+set[j].ecart == o)
*      && (set[j].ecart==p.ecart)
*      && (pComp0(set[j].p,p.p)==pOrdSgn)))
*      return j;
*    j++;
*    if (j > length) return j;
*  }
* }
*/
{
  if (length==-1) return 0;

  int o = pFDeg(p.p) + p.ecart;
  int op = pFDeg(set[length].p)+set[length].ecart;

  if ((op < o)
  || (( op == o) && (set[length].ecart > p.ecart))
  || (( op == o) && (set[length].ecart==p.ecart)
     && (pComp0(set[length].p,p.p) != pOrdSgn)))
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op = pFDeg(set[an].p)+set[an].ecart;
      if (( op > o)
      || (( op == o) && (set[an].ecart < p.ecart))
      || (( op  == o) && (set[an].ecart==p.ecart)
         && (pComp0(set[an].p,p.p) == pOrdSgn)))
        return an;
      return en;
    }
    i=(an+en) / 2;
    op = pFDeg(set[i].p)+set[i].ecart;
    if ((op > o)
    || (( op == o) && (set[i].ecart < p.ecart))
    || (( op == o) && (set[i].ecart == p.ecart)
       && (pComp0(set[i].p,p.p) == pOrdSgn)))
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
int posInT17_c (const TSet set,const int length,const LObject &p)
{
  if (length==-1) return 0;

  int cc = (-1+2*currRing->order[0]==ringorder_c);
  /* cc==1 for (c,..), cc==-1 for (C,..) */
  int o = pFDeg(p.p) + p.ecart;
  int c = pGetComp(p.p)*cc;

  if (pGetComp(set[length].p)*cc < c)
    return length+1;
  if (pGetComp(set[length].p)*cc == c)
  {
    int op = pFDeg(set[length].p)+set[length].ecart;
    if ((op < o)
    || ((op == o) && (set[length].ecart > p.ecart))
    || ((op == o) && (set[length].ecart==p.ecart)
       && (pComp0(set[length].p,p.p) != pOrdSgn)))
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
        int op = pFDeg(set[an].p)+set[an].ecart;
        if ((op > o)
        || ((op == o) && (set[an].ecart < p.ecart))
        || ((op == o) && (set[an].ecart==p.ecart)
           && (pComp0(set[an].p,p.p) == pOrdSgn)))
          return an;
      }
      return en;
    }
    i=(an+en) / 2;
    if (pGetComp(set[i].p)*cc > c)
      en=i;
    else if (pGetComp(set[i].p)*cc == c)
    {
      int op = pFDeg(set[i].p)+set[i].ecart;
      if ((op > o)
      || ((op == o) && (set[i].ecart < p.ecart))
      || ((op == o) && (set[i].ecart == p.ecart)
         && (pComp0(set[i].p,p.p) == pOrdSgn)))
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
int posInT19 (const TSet set,const int length,const LObject &p)
{
  if (length==-1) return 0;

  int o = p.ecart;

  if (set[length].ecart < o)
    return length+1;
  if (set[length].ecart == o)
  {
     int oo=pFDeg(set[length].p);
     int op=pFDeg(p.p);
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
         int oo=pFDeg(set[an].p);
         int op=pFDeg(p.p);
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
       int oo=pFDeg(set[i].p);
       int op=pFDeg(p.p);
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
                   const LObject &p,const kStrategy strat)
{
  if (length<0) return 0;

  int d=pFDeg(p.p);
  int op=pFDeg(set[length].p);

  if ((op > d)
  || ((op == d) && (p.p1!=NULL)&&(set[length].p1==NULL))
  || (pComp0(set[length].p,p.p)== pOrdSgn))
     return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op=pFDeg(set[an].p);
      if ((op > d)
      || ((op == d) && (p.p1!=NULL) && (set[an].p1==NULL))
      || (pComp0(set[an].p,p.p)== pOrdSgn))
         return en;
      return an;
    }
    i=(an+en) / 2;
    op=pFDeg(set[i].p);
    if ((op>d)
    || ((op==d) && (p.p1!=NULL) && (set[i].p1==NULL))
    || (pComp0(set[i].p,p.p) == pOrdSgn))
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
             const LObject &p,const kStrategy strat)
{
  if (length<0) return 0;

  if (pComp0(set[length].p,p.p)== pOrdSgn)
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      if (pComp0(set[an].p,p.p) == pOrdSgn) return en;
      return an;
    }
    i=(an+en) / 2;
    if (pComp0(set[i].p,p.p) == pOrdSgn) an=i;
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
              const LObject &p,const kStrategy strat)
/*{
 * int j=0;
 * int o;
 *
 * o = pFDeg(p.p);
 * loop
 * {
 *   if (j > length)            return j;
 *   if ((pFDeg(set[j].p) < o)) return j;
 *   if ((pFDeg(set[j].p) == o) && (pComp0(set[j].p,p.p) == -pOrdSgn))
 *   {
 *     return j;
 *   }
 *   j++;
 * }
 *}
 */
{
  if (length<0) return 0;

  int o = pFDeg(p.p);
  int op = pFDeg(set[length].p);

  if ((op > o)
  || ((op == o) && (pComp0(set[length].p,p.p) != -pOrdSgn)))
    return length+1;
  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op = pFDeg(set[an].p);
      if ((op > o)
      || ((op == o) && (pComp0(set[an].p,p.p) != -pOrdSgn)))
        return en;
      return an;
    }
    i=(an+en) / 2;
    op = pFDeg(set[i].p);
    if ((op > o)
    || ((op == o) && (pComp0(set[i].p,p.p) != -pOrdSgn)))
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
               const LObject &p,const kStrategy strat)
{
  if (length<0) return 0;

  int o = pFDeg(p.p);
  int op = pFDeg(set[length].p);

  if ((op > o)
  || ((op == o) && (set[length].length >2*p.length))
  || ((op == o) && (set[length].length <= 2*p.length)
     && (pComp0(set[length].p,p.p) != -pOrdSgn)))
    return length+1;
  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op = pFDeg(set[an].p);
      if ((op > o)
      || ((op == o) && (set[an].length >2*p.length))
      || ((op == o) && (set[an].length <=2*p.length)
         && (pComp0(set[an].p,p.p) != -pOrdSgn)))
        return en;
      return an;
    }
    i=(an+en) / 2;
    op = pFDeg(set[i].p);
    if ((op > o)
    || ((op == o) && (set[i].length > 2*p.length))
    || ((op == o) && (set[i].length <= 2*p.length)
       && (pComp0(set[i].p,p.p) != -pOrdSgn)))
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
              const LObject &p,const kStrategy strat)
{
  if (length<0) return 0;

  int o = pFDeg(p.p);

  if (pFDeg(set[length].p) > o)
    return length+1;

  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      if (pFDeg(set[an].p) >= o)
        return en;
      return an;
    }
    i=(an+en) / 2;
    if (pFDeg(set[i].p) >= o)
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
              const LObject &p,const kStrategy strat)
/*{
 * int j=0;
 * int o;
 *
 * o = p.ecart+pFDeg(p.p);
 * loop
 * {
 *   if (j > length)                       return j;
 *   if (pFDeg(set[j].p)+set[j].ecart < o) return j;
 *   if ((pFDeg(set[j].p)+set[j].ecart == o)
 *   && (pComp0(set[j].p,p.p) == -pOrdSgn))
 *   {
 *     return j;
 *   }
 *   j++;
 * }
 *}
 */
{
  if (length<0) return 0;

  int o = pFDeg(p.p) + p.ecart;
  int op = pFDeg(set[length].p) + set[length].ecart;

  if ((op > o)
  || ((op == o) && (pComp0(set[length].p,p.p) != -pOrdSgn)))
    return length+1;
  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      op = pFDeg(set[an].p) + set[an].ecart;
      if ((op > o)
      || ((op == o) && (pComp0(set[an].p,p.p) != -pOrdSgn)))
        return en;
      return an;
    }
    i=(an+en) / 2;
    op = pFDeg(set[i].p) + set[i].ecart;
    if ((op > o)
    || ((op == o) && (pComp0(set[i].p,p.p) != -pOrdSgn)))
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
              const LObject &p,const kStrategy strat)
{
  if (length<0) return 0;

  int o = pFDeg(p.p) + p.ecart;

  if ((pFDeg(set[length].p) + set[length].ecart > o)
  || ((pFDeg(set[length].p) + set[length].ecart == o)
     && (set[length].ecart > p.ecart))
  || ((pFDeg(set[length].p) + set[length].ecart == o)
     && (set[length].ecart == p.ecart)
     && (pComp0(set[length].p,p.p) != -pOrdSgn)))
    return length+1;
  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      if ((pFDeg(set[an].p) + set[an].ecart > o)
      || ((pFDeg(set[an].p) + set[an].ecart == o)
         && (set[an].ecart > p.ecart))
      || ((pFDeg(set[an].p) + set[an].ecart == o)
         && (set[an].ecart == p.ecart)
         && (pComp0(set[an].p,p.p) != -pOrdSgn)))
        return en;
      return an;
    }
    i=(an+en) / 2;
    if ((pFDeg(set[i].p) + set[i].ecart > o)
    || ((pFDeg(set[i].p) + set[i].ecart == o)
       && (set[i].ecart > p.ecart))
    || ((pFDeg(set[i].p) +set[i].ecart == o)
       && (set[i].ecart == p.ecart)
       && (pComp0(set[i].p,p.p) != -pOrdSgn)))
      an=i;
    else
      en=i;
  }
}
#if 0
{
  if (length<0) return 0;

  int o = pFDeg(p.p) + p.ecart;
  int ol = pFDeg(set[length].p) + set[length].ecart;

  if ((ol > o)
  || ((ol == o)
     && (set[length].ecart > p.ecart))
  || ((ol == o)
     && (set[length].ecart == p.ecart)
     //&& (set[length].lp+set[length].length > p.lp+p.length))
     && (set[length].length > p.length))
  || ((ol == o)
     && (set[length].ecart == p.ecart)
     //&& (set[length].lp+set[length].length == p.lp+p.length)
     && (set[length].length == p.length)
     && (pComp0(set[length].p,p.p) != -pOrdSgn)))
    return length+1;
  int i;
  int an = 0;
  int en= length;
  loop
  {
    if (an >= en-1)
    {
      ol = pFDeg(set[an].p) + set[an].ecart;
      if ((ol > o)
      || ((ol == o)
         && (set[an].ecart > p.ecart))
      || ((ol == o)
         && (set[an].ecart == p.ecart)
         //&& (set[length].lp+set[length].length > p.lp+p.length))
         && (set[length].length > p.length))
      || ((ol == o)
         && (set[an].ecart == p.ecart)
         //&& (set[length].lp+set[length].length == p.lp+p.length)
         && (set[length].length == p.length)
         && (pComp0(set[an].p,p.p) != -pOrdSgn)))
        return en;
      return an;
    }
    i=(an+en) / 2;
    ol = pFDeg(set[i].p) + set[i].ecart;
    if ((ol > o)
    || ((ol == o)
       && (set[i].ecart > p.ecart))
    || ((ol == o)
       && (set[i].ecart == p.ecart)
       //&& (set[i].lp+set[i].length > p.lp+p.length))
       && (set[i].length > p.length))
    || ((ol == o)
       && (set[i].ecart == p.ecart)
       //&& (set[i].lp+set[i].length == p.lp+p.length)
       && (set[i].length == p.length)
       && (pComp0(set[i].p,p.p) != -pOrdSgn)))
      an=i;
    else
      en=i;
  }
}
#endif
/*2
* looks up the position of polynomial p in set
* e is the ecart of p
* set[length] is the smallest element in set with respect
* to the ordering-procedure pComp
*/
int posInL17_c (const LSet set, const int length,
                const LObject &p,const kStrategy strat)
{
  if (length<0) return 0;

  int cc = (-1+2*currRing->order[0]==ringorder_c);
  /* cc==1 for (c,..), cc==-1 for (C,..) */
  int c = pGetComp(p.p)*cc;
  int o = pFDeg(p.p) + p.ecart;

  if (pGetComp(set[length].p)*cc > c)
    return length+1;
  if (pGetComp(set[length].p)*cc == c)
  {
    if ((pFDeg(set[length].p) + set[length].ecart > o)
    || ((pFDeg(set[length].p) + set[length].ecart == o)
       && (set[length].ecart > p.ecart))
    || ((pFDeg(set[length].p) + set[length].ecart == o)
       && (set[length].ecart == p.ecart)
       && (pComp0(set[length].p,p.p) != -pOrdSgn)))
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
        if ((pFDeg(set[an].p) + set[an].ecart > o)
        || ((pFDeg(set[an].p) + set[an].ecart == o)
           && (set[an].ecart > p.ecart))
        || ((pFDeg(set[an].p) + set[an].ecart == o)
           && (set[an].ecart == p.ecart)
           && (pComp0(set[an].p,p.p) != -pOrdSgn)))
          return en;
      }
      return an;
    }
    i=(an+en) / 2;
    if (pGetComp(set[i].p)*cc > c)
      an=i;
    else if (pGetComp(set[i].p)*cc == c)
    {
      if ((pFDeg(set[i].p) + set[i].ecart > o)
      || ((pFDeg(set[i].p) + set[i].ecart == o)
         && (set[i].ecart > p.ecart))
      || ((pFDeg(set[i].p) +set[i].ecart == o)
         && (set[i].ecart == p.ecart)
         && (pComp0(set[i].p,p.p) != -pOrdSgn)))
        an=i;
      else
        en=i;
    }
    else
      en=i;
  }
}
/*2
* reduces h using the set S
* procedure used in redtail
*/
/*2
*compute the normalform of the tail p->next of p
*with respect to S
*/
poly redtail (poly p, int pos, kStrategy strat)
{
  if ((!strat->noTailReduction)
  && (pNext(p)!=NULL))
  {
    int j, e, l;
    unsigned long not_sev;

    poly h = p;
    poly hn = pNext(h); // !=NULL
    int op = pFDeg(hn);
    BOOLEAN save_HE=strat->kHEdgeFound;
    strat->kHEdgeFound |= ((Kstd1_deg>0) && (op<=Kstd1_deg));
    loop
    {
      not_sev = ~ pGetShortExpVector(hn);
      e = pLDeg(hn,&l)-op;
      j = 0;
      while (j <= pos)
      {
        if (pShortDivisibleBy(strat->S[j], strat->sevS[j], hn, not_sev)
        && ((e >= strat->ecartS[j])
          || strat->kHEdgeFound)
        )
        {
          ksOldSpolyTail(strat->S[j], p, h, strat->kNoether);
          hn = pNext(h);
          if (hn == NULL) goto all_done;
          not_sev = ~ pGetShortExpVector(hn);
          op = pFDeg(hn);
          if ((Kstd1_deg>0)&&(op>Kstd1_deg)) goto all_done;
          e = pLDeg(hn,&l)-op;
          j = 0;
        }
        else
        {
          j++;
        }
      } /* while (j <= pos) */
      h = hn; /* better for: pIter(h); */
      hn = pNext(h);
      if (hn==NULL) break;
      op = pFDeg(hn);
      if ((Kstd1_deg>0)&&(op>Kstd1_deg)) break;
    }
all_done:
    strat->kHEdgeFound = save_HE;
  }
  return p;
}

/*2
*compute the normalform of the tail p->next of p
*with respect to S
*/
poly redtailBba (poly p, int pos, kStrategy strat)
{
  poly h, hn;
  int j;
  unsigned long not_sev;
  strat->redTailChange=FALSE;

  if (strat->noTailReduction)
  {
    return p;
  }
  h = p;
  hn = pNext(h);
  while(hn != NULL)
  {
    j = 0;
    not_sev = ~ pGetShortExpVector(hn);
    while (j <= pos)
    {
      if (pShortDivisibleBy(strat->S[j], strat->sevS[j], hn, not_sev))
      {
        strat->redTailChange=TRUE;
        assume(p != strat->S[j]);
        ksOldSpolyTail(strat->S[j], p, h, strat->kNoether);
        hn = pNext(h);
        if (hn == NULL)
        {
          return p;
        }
        not_sev = ~ pGetShortExpVector(hn);
        j = 0;
      }
      else
      {
        j++;
      }
    }
    h = hn;
    hn = pNext(h);
  }
  return p;
}

/*2
*compute the "normalform" of the tail p->next of p
*with respect to S for syzygies
*/
poly redtailSyz (poly p, int pos, kStrategy strat)
{
  poly h, hn;
  int j;
  unsigned long not_sev;

  if (strat->noTailReduction)
  {
    return p;
  }
  h = p;
  hn = pNext(h);
  while(hn != NULL)
  {
    j = 0;
    not_sev = ~ pGetShortExpVector(hn);
    while (j <= pos)
    {
      if (pShortDivisibleBy(strat->S[j], strat->sevS[j], hn, not_sev)
          && (!pEqual(strat->S[j],h)))
      {
        ksOldSpolyTail(strat->S[j], p, h, strat->kNoether);
        hn = pNext(h);
        if (hn == NULL)
        {
          return p;
        }
        not_sev = ~ pGetShortExpVector(hn);
        j = 0;
      }
      else
      {
        j++;
      }
    }
    h = hn;
    hn = pNext(h);
  }
  return p;
}

/*2
*checks the change degree and write progress report
*/
void message (int i,int* reduc,int* olddeg,kStrategy strat)
{
  if (i != *olddeg)
  {
    Print("%d",i);
    *olddeg = i;
  }
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
      wrp(strat->S[i]);
    }
    strat->news = FALSE;
  }
  if (strat->newt)
  {
    PrintS("\nset T");
    for (i=0; i<=strat->tl; i++)
    {
      Print("\n  %d:",i);
      wrp(strat->T[i].p);
      Print(" o:%d e:%d l:%d",
        pFDeg(strat->T[i].p),strat->T[i].ecart,strat->T[i].length);
    }
    strat->newt = FALSE;
  }
  PrintS("\nset L");
  for (i=strat->Ll; i>=0; i--)
  {
    Print("\n%d:",i);
    wrp(strat->L[i].p1);
    PrintS("  ");
    wrp(strat->L[i].p2);
    PrintS(" lcm: ");wrp(strat->L[i].lcm);
    PrintS("\n  p : ");
    wrp(strat->L[i].p);
    Print("  o:%d e:%d l:%d",
     pGetOrder(strat->L[i].p),strat->L[i].ecart,strat->L[i].length);
  }
  PrintLn();
}

/*2
*construct the set s from F
*/
void initS (ideal F, ideal Q,kStrategy strat)
{
  LObject h;
  int   i,pos;

  h.ecart=0; h.length=0;
  if (Q!=NULL) i=IDELEMS(Q);
  else i=0;
  i=((i+IDELEMS(F)+15)/16)*16;
  strat->ecartS=initec(i);
  strat->sevS=initsevS(i);
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
        h.p = pCopy(Q->m[i]);
        if (TEST_OPT_INTSTRATEGY)
        {
          //pContent(h.p);
          pCleardenom(h.p); // also does a pContent
        }
        else
        {
          pNorm(h.p);
        }
        strat->initEcart(&h);
        if (pOrdSgn==-1)
        {
          deleteHC(&h.p, &h.ecart, &h.length,strat);
        }
        if (h.p!=NULL)
        {
          if (strat->sl==-1)
            pos =0;
          else
          {
            pos = posInS(strat->S,strat->sl,h.p);
          }
          h.sev = pGetShortExpVector(h.p);
          strat->enterS(h,pos,strat);
          strat->fromQ[pos]=1;
        }
      }
    }
  }
  for (i=0; i<IDELEMS(F); i++)
  {
    if (F->m[i]!=NULL)
    {
      h.p = pCopy(F->m[i]);
        if (TEST_OPT_INTSTRATEGY)
        {
          //pContent(h.p);
          pCleardenom(h.p); // also does a pContent
        }
        else
        {
          pNorm(h.p);
        }
        strat->initEcart(&h);
        if (pOrdSgn==-1)
        {
          cancelunit(&h);  /*- tries to cancel a unit -*/
          deleteHC(&h.p, &h.ecart, &h.length,strat);
        }
        if (TEST_OPT_DEGBOUND
        && (((strat->honey) && (h.ecart+pFDeg(h.p)>Kstd1_deg))
          || ((!(strat->honey)) && (pFDeg(h.p)>Kstd1_deg))))
          pDelete(&h.p);
        else
        if (h.p!=NULL)
        {
          if (strat->sl==-1)
            pos =0;
          else
          {
            pos = posInS(strat->S,strat->sl,h.p);
          }
          h.sev = pGetShortExpVector(h.p);
          strat->enterS(h,pos,strat);
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
  LObject h;
  int   i,pos;

  h.ecart=0; h.length=0;
  if (Q!=NULL) i=IDELEMS(Q);
  else i=0;
  i=((i+16)/16)*16;
  strat->ecartS=initec(i);
  strat->sevS=initsevS(i);
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
        h.p = pCopy(Q->m[i]);
        if (TEST_OPT_INTSTRATEGY)
        {
          //pContent(h.p);
          pCleardenom(h.p); // also does a pContent
        }
        else
        {
          pNorm(h.p);
        }
        strat->initEcart(&h);
        if (pOrdSgn==-1)
        {
          deleteHC(&h.p, &h.ecart, &h.length,strat);
        }
        if (h.p!=NULL)
        {
          if (strat->sl==-1)
            pos =0;
          else
          {
            pos = posInS(strat->S,strat->sl,h.p);
          }
          h.sev = pGetShortExpVector(h.p);
          strat->enterS(h,pos,strat);
          strat->fromQ[pos]=1;
        }
      }
    }
  }
  for (i=0; i<IDELEMS(F); i++)
  {
    if (F->m[i]!=NULL)
    {
      h.p = pCopy(F->m[i]);
      h.p1=NULL;
      h.p2=NULL;
      h.lcm=NULL;
        if (TEST_OPT_INTSTRATEGY)
        {
          //pContent(h.p);
          pCleardenom(h.p); // also does a pContent
        }
        else
        {
          pNorm(h.p);
        }
        strat->initEcart(&h);
        if (pOrdSgn==-1)
        {
          cancelunit(&h);  /*- tries to cancel a unit -*/
          deleteHC(&h.p, &h.ecart, &h.length,strat);
        }
        if (TEST_OPT_DEGBOUND
        && (((strat->honey) && (h.ecart+pFDeg(h.p)>Kstd1_deg))
          || ((!(strat->honey)) && (pFDeg(h.p)>Kstd1_deg))))
          pDelete(&h.p);
        else
        if (h.p!=NULL)
        {
          if (strat->Ll==-1)
            pos =0;
          else
          {
            pos = strat->posInL(strat->L,strat->Ll,h,strat);
          }
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
*construct the set s from F u {P}
*/
void initSSpecial (ideal F, ideal Q, ideal P,kStrategy strat)
{
  LObject h;
  int   i,pos;

  h.ecart=0; h.length=0;
  if (Q!=NULL) i=IDELEMS(Q);
  else i=0;
  i=((i+IDELEMS(F)+15)/16)*16;
  strat->ecartS=initec(i);
  strat->sevS=initsevS(i);
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
        h.p = pCopy(Q->m[i]);
        //if (TEST_OPT_INTSTRATEGY)
        //{
        //  //pContent(h.p);
        //  pCleardenom(h.p); // also does a pContent
        //}
        //else
        //{
        //  pNorm(h.p);
        //}
        strat->initEcart(&h);
        if (pOrdSgn==-1)
        {
          deleteHC(&h.p, &h.ecart, &h.length,strat);
        }
        if (h.p!=NULL)
        {
          if (strat->sl==-1)
            pos =0;
          else
          {
            pos = posInS(strat->S,strat->sl,h.p);
          }
          h.sev = pGetShortExpVector(h.p);
          strat->enterS(h,pos,strat);
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
      h.p = pCopy(F->m[i]);
      if (pOrdSgn==1)
      {
        h.p=redtailBba(h.p,strat->sl,strat);
      }
      strat->initEcart(&h);
      if (pOrdSgn==-1)
      {
          deleteHC(&h.p, &h.ecart, &h.length,strat);
      }
      if (TEST_OPT_DEGBOUND
      && (((strat->honey) && (h.ecart+pFDeg(h.p)>Kstd1_deg))
        || ((!(strat->honey)) && (pFDeg(h.p)>Kstd1_deg))))
        pDelete(&h.p);
      else
      if (h.p!=NULL)
      {
        if (strat->sl==-1)
          pos =0;
        else
        {
          pos = posInS(strat->S,strat->sl,h.p);
        }
        h.sev = pGetShortExpVector(h.p);
        strat->enterS(h,pos,strat);
        h.length = pLength(h.p);
        enterT(h,strat);
      }
    }
  }
  for (i=0; i<IDELEMS(P); i++)
  {
    if (P->m[i]!=NULL)
    {
      h.p=pCopy(P->m[i]);
      strat->initEcart(&h);
      h.length = pLength(h.p);
      if (TEST_OPT_INTSTRATEGY)
      {
        pCleardenom(h.p);
      }
      else
      {
        pNorm(h.p);
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
            pCleardenom(h.p);
          }
          else
          {
            pNorm(h.p);
          }
          h.sev = pGetShortExpVector(h.p);
          pos = posInS(strat->S,strat->sl,h.p);
          enterpairsSpecial(h.p,strat->sl,h.ecart,pos,strat);
          strat->enterS(h,pos,strat);
          enterT(h,strat);
        }
      }
      else
      {
        h.sev = pGetShortExpVector(h.p);
        strat->enterS(h,0,strat);
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
    if (pShortDivisibleBy(strat->S[j],strat->sevS[j],h, not_sev))
       return ksOldSpolyRedNew(strat->S[j],h,strat->kNoether);
    else j++;
  }
  return h;
}

/*2
*tests if p.p=monomial*unit and cancels the unit
*/
void cancelunit1 (LObject* p,int index,kStrategy strat )
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
    if (pShortDivisibleBy(strat->S[j],strat->sevS[j], h, not_sev))
    {
      h = ksOldSpolyRed(strat->S[j],h,strat->kNoether);
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
    if (pShortDivisibleBy(strat->S[j],strat->sevS[j], h, not_sev))
    {
      h = ksOldSpolyRed(strat->S[j],h,strat->kNoether);
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
  poly h1;
  unsigned long not_sev = ~ pGetShortExpVector(h);

  if (maxIndex >= 0)
  {
    e = pLDeg(h,&l)-pFDeg(h);
    do
    {
      if (pShortDivisibleBy(strat->S[j],strat->sevS[j], h, not_sev)
      && ((e >= strat->ecartS[j]) || strat->kHEdgeFound))
      {
        h1 = ksOldSpolyRedNew(strat->S[j],h,strat->kNoether);
        if(TEST_OPT_DEBUG)
        {
          PrintS("reduce "); wrp(h); Print(" with S[%d] (",j);wrp(strat->S[j]);
          PrintS(")\nto "); wrp(h1); PrintLn();
        }
        pDelete(&h);
        if (h1 == NULL) return NULL;
        h = h1;
        e = pLDeg(h,&l)-pFDeg(h);
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
  memset(&h,0,sizeof(h));
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
          if (TEST_OPT_DEBUG && (pComp(redSi,strat->S[i])!=0))
          {
            PrintS("reduce:");
            wrp(redSi);PrintS(" to ");wrp(strat->S[i]);PrintLn();
          }
          if (TEST_OPT_PROT && (pComp(redSi,strat->S[i])!=0))
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
        if (((strat->fromQ==NULL) || (strat->fromQ[i]==0))
        )
          h.p = redtailBba(strat->S[i],i-1,strat);
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
        /*puts the elements of S also to T*/
        enterT(h,strat);
      }
    }
  }
  else
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
          newHEdge(strat->S,strat->ak,strat);
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
        cancelunit1(&h,strat->sl,strat);
      h.length = pLength(h.p);
      /*puts the elements of S also to T*/
      enterT(h,strat);
    }
  }
  if (redSi!=NULL) pDelete1(&redSi);
#ifdef KDEBUG
  kTest(strat);
#endif
}

/*2
* -puts p to the standardbasis s at position at
* -saves the result in S
*/
void enterSBba (LObject p,int atS,kStrategy strat)
{
  int i;

  strat->news = TRUE;
  /*- puts p to the standardbasis s at position at -*/
  if (strat->sl == IDELEMS(strat->Shdl)-1)
  {
    strat->sevS = (unsigned long*) ReAlloc0(strat->sevS,
                                    IDELEMS(strat->Shdl)*sizeof(unsigned long),
                                    (IDELEMS(strat->Shdl)+setmax)
                                           *sizeof(unsigned long));
    strat->ecartS = (intset)ReAlloc(strat->ecartS,
                                    IDELEMS(strat->Shdl)*sizeof(int),
                                    (IDELEMS(strat->Shdl)+setmax)*sizeof(int));
    if (strat->fromQ!=NULL)
    {
      strat->fromQ = (intset)ReAlloc(strat->fromQ,
                                    IDELEMS(strat->Shdl)*sizeof(int),
                                    (IDELEMS(strat->Shdl)+setmax)*sizeof(int));
    }
    pEnlargeSet(&strat->S,IDELEMS(strat->Shdl),setmax);
    IDELEMS(strat->Shdl)+=setmax;
    strat->Shdl->m=strat->S;
  }
  for (i=strat->sl+1; i>=atS+1; i--)
  {
    strat->S[i] = strat->S[i-1];
    if (strat->honey) strat->ecartS[i] = strat->ecartS[i-1];
    strat->sevS[i] = strat->sevS[i-1];
  }
  if (strat->fromQ!=NULL)
  {
    for (i=strat->sl+1; i>=atS+1; i--)
    {
      strat->fromQ[i] = strat->fromQ[i-1];
    }
    strat->fromQ[atS]=0;
  }
  /*- save result -*/
  strat->S[atS] = p.p;
  if (strat->honey) strat->ecartS[atS] = p.ecart;
  if (p.sev == 0)
  {
    p.sev = pGetShortExpVector(p.p);
  }
  else
  {
    assume(p.sev == pGetShortExpVector(p.p));
  }
  strat->sevS[atS] = p.sev;
  strat->sl++;
}

/*2
* puts p to the set T at position atT
*/
void enterT (LObject p,kStrategy strat)
{
  int i,atT;

  pHeapTest(p.p, (p.heap == NULL ? mm_specHeap : p.heap));
  assume(p.pLength == 0 || pLength(p.p) == p.pLength);

  strat->newt = TRUE;
  if (strat->tl >= 0)
  {
    /*- puts p to the standardbasis s at position atT -*/
    atT = strat->posInT(strat->T,strat->tl,p);
    if (strat->tl == strat->tmax-1) enlargeT(&strat->T,&strat->tmax,setmax);
    for (i=strat->tl+1; i>=atT+1; i--) strat->T[i] = strat->T[i-1];
  }
  else atT = 0;
  strat->T[atT].p = p.p;
  strat->T[atT].ecart = p.ecart;
  strat->T[atT].length = p.length;
  strat->T[atT].pLength = p.pLength;
  strat->T[atT].heap = p.heap;
  if (p.sev == 0)
  {
    p.sev = pGetShortExpVector(p.p);
  }
  else
  {
    assume(p.sev == pGetShortExpVector(p.p));
  }
  strat->T[atT].sev = p.sev;
  strat->tl++;
}

/*2
* puts p to the set T at position atT
*/
void enterTBba (LObject p, int atT,kStrategy strat)
{
  int i;

  pHeapTest(p.p, (p.heap == NULL ? mm_specHeap : p.heap));
  assume(p.pLength == 0 || pLength(p.p) == p.pLength);

  strat->newt = TRUE;
  if (strat->tl == strat->tmax-1) enlargeT(&strat->T,&strat->tmax,setmax);
  for (i=strat->tl+1; i>=atT+1; i--)
    strat->T[i] = strat->T[i-1];
  strat->T[atT].p = p.p;
  if (strat->honey)
    strat->T[atT].ecart = p.ecart;
  if (TEST_OPT_INTSTRATEGY)
    strat->T[atT].length = p.length;

  strat->T[atT].heap = p.heap;
  strat->T[atT].pLength = p.pLength;
  if (p.sev == 0)
  {
    p.sev = pGetShortExpVector(p.p);
  }
  else
  {
    assume(p.sev == pGetShortExpVector(p.p));
  }
  strat->T[atT].sev = p.sev;

  strat->tl++;
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
  strat->Gebauer =          BTEST1(2) || strat->homog || strat->sugarCrit;
  strat->honey =            !strat->homog || strat->sugarCrit || TEST_OPT_WEIGHTM;
  if (TEST_OPT_NOT_SUGAR) strat->honey = FALSE;
  strat->pairtest = NULL;
  /* alway use tailreduction, except:
  * - in local rings, - in lex order case, -in ring over extensions */
  strat->noTailReduction = !TEST_OPT_REDTAIL;
  if (TEST_OPT_DEBUG)
  {
    if (strat->homog) PrintS("ideal/module is homogeneous\n");
    else              PrintS("ideal/module is not homogeneous\n");
  }
}

void initBuchMoraPos (kStrategy strat)
{
  if (pOrdSgn==1)
  {
    if (strat->honey)
    {
      strat->posInL = posInL15;
      strat->posInT = posInT15;
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
  strat->Lmax = setmax;
  strat->Ll = -1;
  strat->L = initL();
  /*- set B -*/
  strat->Bmax = setmax;
  strat->Bl = -1;
  strat->B = initL();
  /*- set T -*/
  strat->tl = -1;
  strat->tmax = setmax;
  strat->T = initT();
  /*- init local data struct.---------------------------------------- -*/
  strat->P.ecart=0;
  strat->P.length=0;
  if (pOrdSgn==-1)
  {
    if (strat->kHEdge!=NULL) pSetComp(strat->kHEdge, strat->ak);
    if (strat->kNoether!=NULL) pSetComp(strat->kNoether, strat->ak);
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
  if (strat->fromQ!=NULL) Free((ADDRESS)strat->fromQ,IDELEMS(strat->Shdl)*sizeof(int));
  strat->fromQ=NULL;
}

void exitBuchMora (kStrategy strat)
{
  /*- release temp data -*/
  cleanT(strat);
  Free((ADDRESS)strat->T,(strat->tmax)*sizeof(TObject));
  Free((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  Free((ADDRESS)strat->sevS,IDELEMS(strat->Shdl)*sizeof(int));
  /*- set L: should be empty -*/
  Free((ADDRESS)strat->L,(strat->Lmax)*sizeof(LObject));
  /*- set B: should be empty -*/
  Free((ADDRESS)strat->B,(strat->Bmax)*sizeof(LObject));
  pDelete1(&strat->tail);
  strat->syzComp=0;
  if (strat->kIdeal!=NULL)
  {
    FreeSizeOf((ADDRESS)strat->kIdeal,sleftv);
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
void updateResult(ideal r,ideal Q,kStrategy strat)
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
          &&(pEqual(r->m[l],Q->m[q])))
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
  if(pOrdSgn==1)
  {
    for (i=strat->sl; i>0; i--)
    {
      //if (strat->interpt) test_int_std(strat->kIdeal);
      strat->S[i] = redtailBba(strat->S[i],i-1,strat);
      if (TEST_OPT_INTSTRATEGY)
      {
        //if (strat->redTailChange)
          pCleardenom(strat->S[i]);
      }
      if (TEST_OPT_PROT)
      {
        PrintS("-");mflush();
      }
    }
  }
  else
  {
    for (i=strat->sl; i>=0; i--)
    {
      //if (strat->interpt) test_int_std(strat->kIdeal);
      strat->S[i] = redtail(strat->S[i],strat->sl,strat);
      if (TEST_OPT_INTSTRATEGY)
      {
        pCleardenom(strat->S[i]);
      }
      if (TEST_OPT_PROT)
      {
        PrintS("-");mflush();
      }
    }
  }
}

/*2
* computes the new strat->kHEdge and the new pNoether,
* returns TRUE, if pNoether has changed
*/
BOOLEAN newHEdge(polyset S, int ak,kStrategy strat)
{
  int i,j;
  poly newNoether;

  scComputeHC(strat->Shdl,ak,strat->kHEdge);
  /* compare old and new noether*/
  newNoether = pHead0(strat->kHEdge);
  j = pFDeg(newNoether);
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
  if (pComp(strat->kNoether,newNoether)!=1)
  {
    pDelete(&strat->kNoether);
    strat->kNoether=newNoether;
    return TRUE;
  }
  pFree1(newNoether);
  return FALSE;
}

void kFreeStrat(kStrategy strat)
{
#if 0
  if (strat->THeap != NULL)
  {
    mmMergeHeap(mm_specHeap, strat->THeap);
    mmUnGetTempHeap(&(strat->THeap));
  }
#endif
  FreeSizeOf(strat, skStrategy);
}

rOrderType_t spGetOrderType(ring r, int modrank, int syzcomp)
{
  if (syzcomp > 0)
    return rOrderType_Syz;
  else
  {
    rOrderType_t rot = rGetOrderType(r);

    if ((rot == rOrderType_CompExp || rot == rOrderType_ExpComp) &&
        (modrank == 0))
      return rOrderType_Exp;
    else
      return rot;
  }
}


