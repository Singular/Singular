/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT:
*/

// TODO: why the following is here instead of mod2.h???

// define if tailrings should be used
#define HAVE_TAIL_RING

// define if buckets should be used
#define MORA_USE_BUCKETS

#include <kernel/mod2.h>
#include <omalloc/omalloc.h>

#ifndef NDEBUG
# define MYTEST 0
#else /* ifndef NDEBUG */
# define MYTEST 0
#endif /* ifndef NDEBUG */

#if MYTEST
#ifdef HAVE_TAIL_RING
#undef HAVE_TAIL_RING
#endif /* ifdef HAVE_TAIL_RING */
#endif /* if MYTEST */

#include <kernel/options.h>
#include <kernel/kutil.h>
#include <kernel/kInline.cc>
#include <kernel/polys.h>
#include <kernel/febase.h>
#include <kernel/kstd1.h>
#include <kernel/khstd.h>
#include <kernel/stairc.h>
#include <kernel/weight.h>
//#include "cntrlc.h"
#include <kernel/intvec.h>
#include <kernel/ideals.h>
//#include "../Singular/ipid.h"
#include <kernel/timer.h>

//#include "ipprint.h"

#ifdef HAVE_PLURAL
#include <kernel/sca.h>
#endif


/* the list of all options which give a warning by test */
BITSET kOptions=Sy_bit(OPT_PROT)           /*  0 */
                |Sy_bit(OPT_REDSB)         /*  1 */
                |Sy_bit(OPT_NOT_SUGAR)     /*  3 */
                |Sy_bit(OPT_INTERRUPT)     /*  4 */
                |Sy_bit(OPT_SUGARCRIT)     /*  5 */
                |Sy_bit(OPT_REDTHROUGH)
                |Sy_bit(OPT_OLDSTD)
                |Sy_bit(OPT_FASTHC)        /* 10 */
                |Sy_bit(OPT_INTSTRATEGY)   /* 26 */
                |Sy_bit(OPT_INFREDTAIL)    /* 28 */
                |Sy_bit(OPT_NOTREGULARITY) /* 30 */
                |Sy_bit(OPT_WEIGHTM);      /* 31 */

/* the list of all options which may be used by option and test */
BITSET validOpts=Sy_bit(0)
                |Sy_bit(1)
                |Sy_bit(2) // obachman 10/00: replaced by notBucket
                |Sy_bit(3)
                |Sy_bit(4)
                |Sy_bit(5)
                |Sy_bit(6)
//                |Sy_bit(7) obachman 11/00 tossed: 12/00 used for redThrough
                |Sy_bit(7) // OPT_REDTHROUGH
                |Sy_bit(8) // obachman 11/00 tossed -> motsak 2011 experimental: OPT_NO_SYZ_MINIM
                |Sy_bit(9)
                |Sy_bit(10)
                |Sy_bit(11)
                |Sy_bit(12)
                |Sy_bit(13)
                |Sy_bit(14)
                |Sy_bit(15)
                |Sy_bit(16)
                |Sy_bit(17)
                |Sy_bit(18)
                |Sy_bit(19)
//                |Sy_bit(20) obachman 11/00 tossed: 12/00 used for redOldStd
                |Sy_bit(OPT_OLDSTD)
                |Sy_bit(21)
                |Sy_bit(22)
                /*|Sy_bit(23)*/
                /*|Sy_bit(24)*/
                |Sy_bit(OPT_REDTAIL)
                |Sy_bit(OPT_INTSTRATEGY)
                |Sy_bit(27)
                |Sy_bit(28)
                |Sy_bit(29)
                |Sy_bit(30)
                |Sy_bit(31);

//static BOOLEAN posInLOldFlag;
           /*FALSE, if posInL == posInL10*/
// returns TRUE if mora should use buckets, false otherwise
static BOOLEAN kMoraUseBucket(kStrategy strat);

static void kOptimizeLDeg(pLDegProc ldeg, kStrategy strat)
{
//  if (strat->ak == 0 && !rIsSyzIndexRing(currRing))
    strat->length_pLength = TRUE;
//  else
//    strat->length_pLength = FALSE;

  if ((ldeg == pLDeg0c /*&& !rIsSyzIndexRing(currRing)*/) ||
      (ldeg == pLDeg0 && strat->ak == 0))
  {
    strat->LDegLast = TRUE;
  }
  else
  {
    strat->LDegLast = FALSE;
  }
}


static int doRed (LObject* h, TObject* with,BOOLEAN intoT,kStrategy strat)
{
  poly hp;
  int ret;
#if KDEBUG > 0
  kTest_L(h);
  kTest_T(with);
#endif
  // Hmmm ... why do we do this -- polys from T should already be normalized
  if (!TEST_OPT_INTSTRATEGY)
    with->pNorm();
#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    PrintS("reduce ");h->wrp();PrintS(" with ");with->wrp();PrintLn();
  }
#endif
  if (intoT)
  {
    // need to do it exacly like this: otherwise
    // we might get errors
    LObject L= *h;
    L.Copy();
    h->GetP();
    h->SetLength(strat->length_pLength);
    ret = ksReducePoly(&L, with, strat->kNoetherTail(), NULL, strat);
    if (ret)
    {
      if (ret < 0) return ret;
      if (h->tailRing != strat->tailRing)
        h->ShallowCopyDelete(strat->tailRing,
                             pGetShallowCopyDeleteProc(h->tailRing,
                                                       strat->tailRing));
    }
    enterT(*h,strat);
    *h = L;
  }
  else
    ret = ksReducePoly(h, with, strat->kNoetherTail(), NULL, strat);
#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    PrintS("to ");h->wrp();PrintLn();
  }
#endif
  return ret;
}

int redEcart (LObject* h,kStrategy strat)
{
  poly pi;
  int i,at,ei,li,ii;
  int j = 0;
  int pass = 0;
  long d,reddeg;

  d = h->GetpFDeg()+ h->ecart;
  reddeg = strat->LazyDegree+d;
  h->SetShortExpVector();
  loop
  {
    j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h);
    if (j < 0)
    {
      if (strat->honey) h->SetLength(strat->length_pLength);
      return 1;
    }

    ei = strat->T[j].ecart;
    ii = j;

    if (ei > h->ecart && ii < strat->tl)
    {
      li = strat->T[j].length;
      // the polynomial to reduce with (up to the moment) is;
      // pi with ecart ei and length li
      // look for one with smaller ecart
      i = j;
      loop
      {
        /*- takes the first possible with respect to ecart -*/
        i++;
#if 1
        if (i > strat->tl) break;
        if ((strat->T[i].ecart < ei || (strat->T[i].ecart == ei &&
                                        strat->T[i].length < li))
            &&
            p_LmShortDivisibleBy(strat->T[i].GetLmTailRing(), strat->sevT[i], h->GetLmTailRing(), ~h->sev, strat->tailRing))
#else
          j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h, i);
        if (j < 0) break;
        i = j;
        if (strat->T[i].ecart < ei || (strat->T[i].ecart == ei &&
                                        strat->T[i].length < li))
#endif
        {
          // the polynomial to reduce with is now
          ii = i;
          ei = strat->T[i].ecart;
          if (ei <= h->ecart) break;
          li = strat->T[i].length;
        }
      }
    }

    // end of search: have to reduce with pi
    if (ei > h->ecart)
    {
      // It is not possible to reduce h with smaller ecart;
      // if possible h goes to the lazy-set L,i.e
      // if its position in L would be not the last one
      strat->fromT = TRUE;
      if (!TEST_OPT_REDTHROUGH && strat->Ll >= 0) /*- L is not empty -*/
      {
        h->SetLmCurrRing();
        if (strat->honey && strat->posInLDependsOnLength)
          h->SetLength(strat->length_pLength);
        assume(h->FDeg == h->pFDeg());
        at = strat->posInL(strat->L,strat->Ll,h,strat);
        if (at <= strat->Ll)
        {
          /*- h will not become the next element to reduce -*/
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
#ifdef KDEBUG
          if (TEST_OPT_DEBUG) Print(" ecart too big; -> L%d\n",at);
#endif
          h->Clear();
          strat->fromT = FALSE;
          return -1;
        }
      }
    }

    // now we finally can reduce
    doRed(h,&(strat->T[ii]),strat->fromT,strat);
    h->last=NULL;
    strat->fromT=FALSE;

    // are we done ???
    if (h->IsNull())
    {
      if (h->lcm!=NULL) pLmFree(h->lcm);
      h->Clear();
      return 0;
    }

    // NO!
    h->SetShortExpVector();
    h->SetpFDeg();
    if (strat->honey)
    {
      if (ei <= h->ecart)
        h->ecart = d-h->GetpFDeg();
      else
        h->ecart = d-h->GetpFDeg()+ei-h->ecart;
    }
    else
      // this has the side effect of setting h->length
      h->ecart = h->pLDeg(strat->LDegLast) - h->GetpFDeg();
#if 0
    if (strat->syzComp!=0)
    {
      if ((strat->syzComp>0) && (h->Comp() > strat->syzComp))
      {
        assume(h->MinComp() > strat->syzComp);
        if (strat->honey) h->SetLength();
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) PrintS(" > syzComp\n");
#endif
        return -2;
      }
    }
#endif
    /*- try to reduce the s-polynomial -*/
    pass++;
    d = h->GetpFDeg()+h->ecart;
    /*
     *test whether the polynomial should go to the lazyset L
     *-if the degree jumps
     *-if the number of pre-defined reductions jumps
     */
    if (!TEST_OPT_REDTHROUGH && (strat->Ll >= 0)
        && ((d >= reddeg) || (pass > strat->LazyPass)))
    {
      h->SetLmCurrRing();
      if (strat->honey && strat->posInLDependsOnLength)
        h->SetLength(strat->length_pLength);
      assume(h->FDeg == h->pFDeg());
      at = strat->posInL(strat->L,strat->Ll,h,strat);
      if (at <= strat->Ll)
      {
        int dummy=strat->sl;
        if (kFindDivisibleByInS(strat, &dummy, h) < 0)
        {
          if (strat->honey && !strat->posInLDependsOnLength)
            h->SetLength(strat->length_pLength);
          return 1;
        }
        enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) Print(" degree jumped; ->L%d\n",at);
#endif
        h->Clear();
        return -1;
      }
    }
    else if ((TEST_OPT_PROT) && (strat->Ll < 0) && (d >= reddeg))
    {
      Print(".%ld",d);mflush();
      reddeg = d+1;
      if (h->pTotalDeg()+h->ecart >= strat->tailRing->bitmask)
      {
        strat->overflow=TRUE;
        //Print("OVERFLOW in redEcart d=%ld, max=%ld",d,strat->tailRing->bitmask);
        h->GetP();
        at = strat->posInL(strat->L,strat->Ll,h,strat);
        enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
        h->Clear();
        return -1;
      }
    }
  }
}

/*2
*reduces h with elements from T choosing  the first possible
* element in t with respect to the given pDivisibleBy
*/
int redFirst (LObject* h,kStrategy strat)
{
  if (h->IsNull()) return 0;

  int at;
  long reddeg,d;
  int pass = 0;
  int j = 0;

  if (! strat->homog)
  {
    d = h->GetpFDeg() + h->ecart;
    reddeg = strat->LazyDegree+d;
  }
  h->SetShortExpVector();
  loop
  {
    j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h);
    if (j < 0)
    {
      h->SetDegStuffReturnLDeg(strat->LDegLast);
      return 1;
    }

    if (!TEST_OPT_INTSTRATEGY)
      strat->T[j].pNorm();
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("reduce ");
      h->wrp();
      PrintS(" with ");
      strat->T[j].wrp();
    }
#endif
    ksReducePoly(h, &(strat->T[j]), strat->kNoetherTail(), NULL, strat);
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS(" to ");
      wrp(h->p);
      PrintLn();
    }
#endif
    if (h->IsNull())
    {
      if (h->lcm!=NULL) pLmFree(h->lcm);
      h->Clear();
      return 0;
    }
    h->SetShortExpVector();

#if 0
    if ((strat->syzComp!=0) && !strat->honey)
    {
      if ((strat->syzComp>0) &&
          (h->Comp() > strat->syzComp))
      {
        assume(h->MinComp() > strat->syzComp);
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) PrintS(" > syzComp\n");
#endif
        if (strat->homog)
          h->SetDegStuffReturnLDeg(strat->LDegLast);
        return -2;
      }
    }
#endif
    if (!strat->homog)
    {
      if (!TEST_OPT_OLDSTD && strat->honey)
      {
        h->SetpFDeg();
        if (strat->T[j].ecart <= h->ecart)
          h->ecart = d - h->GetpFDeg();
        else
          h->ecart = d - h->GetpFDeg() + strat->T[j].ecart - h->ecart;

        d = h->GetpFDeg() + h->ecart;
      }
      else
        d = h->SetDegStuffReturnLDeg(strat->LDegLast);
      /*- try to reduce the s-polynomial -*/
      pass++;
      /*
       *test whether the polynomial should go to the lazyset L
       *-if the degree jumps
       *-if the number of pre-defined reductions jumps
       */
      if (!TEST_OPT_REDTHROUGH && (strat->Ll >= 0)
          && ((d >= reddeg) || (pass > strat->LazyPass)))
      {
        h->SetLmCurrRing();
        if (strat->posInLDependsOnLength)
          h->SetLength(strat->length_pLength);
        at = strat->posInL(strat->L,strat->Ll,h,strat);
        if (at <= strat->Ll)
        {
          int dummy=strat->sl;
          if (kFindDivisibleByInS(strat,&dummy, h) < 0)
            return 1;
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
#ifdef KDEBUG
          if (TEST_OPT_DEBUG) Print(" degree jumped; ->L%d\n",at);
#endif
          h->Clear();
          return -1;
        }
      }
      if ((TEST_OPT_PROT) && (strat->Ll < 0) && (d >= reddeg))
      {
        reddeg = d+1;
        Print(".%ld",d);mflush();
        if (h->pTotalDeg()+h->ecart >= strat->tailRing->bitmask)
        {
          strat->overflow=TRUE;
          //Print("OVERFLOW in redFirst d=%ld, max=%ld",d,strat->tailRing->bitmask);
          h->GetP();
          at = strat->posInL(strat->L,strat->Ll,h,strat);
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
          h->Clear();
          return -1;
        }
      }
    }
  }
}

/*2
* reduces h with elements from T choosing first possible
* element in T with respect to the given ecart
* used for computing normal forms outside kStd
*/
static poly redMoraNF (poly h,kStrategy strat, int flag)
{
  LObject H;
  H.p = h;
  int j = 0;
  int z = 10;
  int o = H.SetpFDeg();
  H.ecart = pLDeg(H.p,&H.length,currRing)-o;
  if ((flag & 2) == 0) cancelunit(&H,TRUE);
  H.sev = pGetShortExpVector(H.p);
  unsigned long not_sev = ~ H.sev;
  loop
  {
    if (j > strat->tl)
    {
      return H.p;
    }
    if (TEST_V_DEG_STOP)
    {
      if (kModDeg(H.p)>Kstd1_deg) pLmDelete(&H.p);
      if (H.p==NULL) return NULL;
    }
    if (p_LmShortDivisibleBy(strat->T[j].GetLmTailRing(), strat->sevT[j], H.GetLmTailRing(), not_sev, strat->tailRing))
    {
      //if (strat->interpt) test_int_std(strat->kIdeal);
      /*- remember the found T-poly -*/
      poly pi = strat->T[j].p;
      int ei = strat->T[j].ecart;
      int li = strat->T[j].length;
      int ii = j;
      /*
      * the polynomial to reduce with (up to the moment) is;
      * pi with ecart ei and length li
      */
      loop
      {
        /*- look for a better one with respect to ecart -*/
        /*- stop, if the ecart is small enough (<=ecart(H)) -*/
        j++;
        if (j > strat->tl) break;
        if (ei <= H.ecart) break;
        if (((strat->T[j].ecart < ei)
          || ((strat->T[j].ecart == ei)
        && (strat->T[j].length < li)))
        && pLmShortDivisibleBy(strat->T[j].p,strat->sevT[j], H.p, not_sev))
        {
          /*
          * the polynomial to reduce with is now;
          */
          pi = strat->T[j].p;
          ei = strat->T[j].ecart;
          li = strat->T[j].length;
          ii = j;
        }
      }
      /*
      * end of search: have to reduce with pi
      */
      z++;
      if (z>10)
      {
        pNormalize(H.p);
        z=0;
      }
      if ((ei > H.ecart) && (!strat->kHEdgeFound))
      {
        /*
        * It is not possible to reduce h with smaller ecart;
        * we have to reduce with bad ecart: H has to enter in T
        */
        doRed(&H,&(strat->T[ii]),TRUE,strat);
        if (H.p == NULL)
          return NULL;
      }
      else
      {
        /*
        * we reduce with good ecart, h need not to be put to T
        */
        doRed(&H,&(strat->T[ii]),FALSE,strat);
        if (H.p == NULL)
          return NULL;
      }
      /*- try to reduce the s-polynomial -*/
      o = H.SetpFDeg();
      if ((flag &2 ) == 0) cancelunit(&H,TRUE);
      H.ecart = pLDeg(H.p,&(H.length),currRing)-o;
      j = 0;
      H.sev = pGetShortExpVector(H.p);
      not_sev = ~ H.sev;
    }
    else
    {
      j++;
    }
  }
}

/*2
*reorders  L with respect to posInL
*/
void reorderL(kStrategy strat)
{
  int i,j,at;
  LObject p;

  for (i=1; i<=strat->Ll; i++)
  {
    at = strat->posInL(strat->L,i-1,&(strat->L[i]),strat);
    if (at != i)
    {
      p = strat->L[i];
      for (j=i-1; j>=at; j--) strat->L[j+1] = strat->L[j];
      strat->L[at] = p;
    }
  }
}

/*2
*reorders  T with respect to length
*/
void reorderT(kStrategy strat)
{
  int i,j,at;
  TObject p;
  unsigned long sev;


  for (i=1; i<=strat->tl; i++)
  {
    if (strat->T[i-1].length > strat->T[i].length)
    {
      p = strat->T[i];
      sev = strat->sevT[i];
      at = i-1;
      loop
      {
        at--;
        if (at < 0) break;
        if (strat->T[i].length > strat->T[at].length) break;
      }
      for (j = i-1; j>at; j--)
      {
        strat->T[j+1]=strat->T[j];
        strat->sevT[j+1]=strat->sevT[j];
        strat->R[strat->T[j+1].i_r] = &(strat->T[j+1]);
      }
      strat->T[at+1]=p;
      strat->sevT[at+1] = sev;
      strat->R[p.i_r] = &(strat->T[at+1]);
    }
  }
}

/*2
*looks whether exactly pVariables-1 axis are used
*returns last != 0 in this case
*last is the (first) unused axis
*/
void missingAxis (int* last,kStrategy strat)
{
  int   i = 0;
  int   k = 0;

  *last = 0;
  if (!currRing->MixedOrder)
  {
    loop
    {
      i++;
      if (i > pVariables) break;
      if (strat->NotUsedAxis[i])
      {
        *last = i;
        k++;
      }
      if (k>1)
      {
        *last = 0;
        break;
      }
    }
  }
}

/*2
*last is the only non used axis, it looks
*for a monomial in p being a pure power of this
*variable and returns TRUE in this case
*(*length) gives the length between the pure power and the leading term
*(should be minimal)
*/
BOOLEAN hasPurePower (const poly p,int last, int *length,kStrategy strat)
{
  poly h;
  int i;

  if (pNext(p) == strat->tail)
    return FALSE;
  pp_Test(p, currRing, strat->tailRing);
  if (strat->ak <= 0 || p_MinComp(p, currRing, strat->tailRing) == strat->ak)
  {
    i = p_IsPurePower(p, currRing);
    if (i == last)
    {
      *length = 0;
      return TRUE;
    }
    *length = 1;
    h = pNext(p);
    while (h != NULL)
    {
      i = p_IsPurePower(h, strat->tailRing);
      if (i==last) return TRUE;
      (*length)++;
      pIter(h);
    }
  }
  return FALSE;
}

BOOLEAN hasPurePower (LObject *L,int last, int *length,kStrategy strat)
{
  if (L->bucket != NULL)
  {
    poly p = L->CanonicalizeP();
    BOOLEAN ret = hasPurePower(p, last, length, strat);
    pNext(p) = NULL;
    return ret;
  }
  else
  {
    return hasPurePower(L->p, last, length, strat);
  }
}

/*2
* looks up the position of polynomial p in L
* in the case of looking for the pure powers
*/
int posInL10 (const LSet set,const int length, LObject* p,const kStrategy strat)
{
  int j,dp,dL;

  if (length<0) return 0;
  if (hasPurePower(p,strat->lastAxis,&dp,strat))
  {
    int op= p->GetpFDeg() +p->ecart;
    for (j=length; j>=0; j--)
    {
      if (!hasPurePower(&(set[j]),strat->lastAxis,&dL,strat))
        return j+1;
      if (dp < dL)
        return j+1;
      if ((dp == dL)
          && (set[j].GetpFDeg()+set[j].ecart >= op))
        return j+1;
    }
  }
  j=length;
  loop
  {
    if (j<0) break;
    if (!hasPurePower(&(set[j]),strat->lastAxis,&dL,strat)) break;
    j--;
  }
  return strat->posInLOld(set,j,p,strat);
}


/*2
* computes the s-polynomials L[ ].p in L
*/
void updateL(kStrategy strat)
{
  LObject p;
  int dL;
  int j=strat->Ll;
  loop
  {
    if (j<0) break;
    if (hasPurePower(&(strat->L[j]),strat->lastAxis,&dL,strat))
    {
      p=strat->L[strat->Ll];
      strat->L[strat->Ll]=strat->L[j];
      strat->L[j]=p;
      break;
    }
    j--;
  }
  if (j<0)
  {
    j=strat->Ll;
    loop
    {
      if (j<0) break;
      if (pNext(strat->L[j].p) == strat->tail)
      {
#ifdef HAVE_RINGS
        if (rField_is_Ring(currRing))
          pLmDelete(strat->L[j].p);    /*deletes the short spoly and computes*/
        else
#else
          pLmFree(strat->L[j].p);    /*deletes the short spoly and computes*/
#endif
        strat->L[j].p = NULL;
        poly m1 = NULL, m2 = NULL;
        // check that spoly creation is ok
        while (strat->tailRing != currRing &&
               !kCheckSpolyCreation(&(strat->L[j]), strat, m1, m2))
        {
          assume(m1 == NULL && m2 == NULL);
          // if not, change to a ring where exponents are at least
          // large enough
          kStratChangeTailRing(strat);
        }
        /* create the real one */
        ksCreateSpoly(&(strat->L[j]), strat->kNoetherTail(), FALSE,
                      strat->tailRing, m1, m2, strat->R);

        strat->L[j].SetLmCurrRing();
        if (!strat->honey)
          strat->initEcart(&strat->L[j]);
        else
          strat->L[j].SetLength(strat->length_pLength);

        BOOLEAN pp = hasPurePower(&(strat->L[j]),strat->lastAxis,&dL,strat);

        if (strat->use_buckets) strat->L[j].PrepareRed(TRUE);

        if (pp)
        {
          p=strat->L[strat->Ll];
          strat->L[strat->Ll]=strat->L[j];
          strat->L[j]=p;
          break;
        }
      }
      j--;
    }
  }
}

/*2
* computes the s-polynomials L[ ].p in L and
* cuts elements in L above noether
*/
void updateLHC(kStrategy strat)
{
  int i = 0;
  kTest_TS(strat);
  while (i <= strat->Ll)
  {
    if (pNext(strat->L[i].p) == strat->tail)
    {
       /*- deletes the int spoly and computes -*/
      if (pLmCmp(strat->L[i].p,strat->kNoether) == -1)
      {
        pLmFree(strat->L[i].p);
        strat->L[i].p = NULL;
      }
      else
      {
        pLmFree(strat->L[i].p);
        strat->L[i].p = NULL;
        poly m1 = NULL, m2 = NULL;
        // check that spoly creation is ok
        while (strat->tailRing != currRing &&
               !kCheckSpolyCreation(&(strat->L[i]), strat, m1, m2))
        {
          assume(m1 == NULL && m2 == NULL);
          // if not, change to a ring where exponents are at least
          // large enough
          kStratChangeTailRing(strat);
        }
        /* create the real one */
        ksCreateSpoly(&(strat->L[i]), strat->kNoetherTail(), FALSE,
                      strat->tailRing, m1, m2, strat->R);
        if (! strat->L[i].IsNull())
        {
          strat->L[i].SetLmCurrRing();
          strat->L[i].SetpFDeg();
          strat->L[i].ecart
            = strat->L[i].pLDeg(strat->LDegLast) - strat->L[i].GetpFDeg();
          if (strat->use_buckets) strat->L[i].PrepareRed(TRUE);
        }
      }
    }
    else
      deleteHC(&(strat->L[i]), strat);
   if (strat->L[i].IsNull())
      deleteInL(strat->L,&strat->Ll,i,strat);
    else
    {
#ifdef KDEBUG
      kTest_L(&(strat->L[i]), strat->tailRing, TRUE, i, strat->T, strat->tl);
#endif
      i++;
    }
  }
  kTest_TS(strat);
}

/*2
* cuts in T above strat->kNoether and tries to cancel a unit
*/
void updateT(kStrategy strat)
{
  int i = 0;
  LObject p;

  while (i <= strat->tl)
  {
    p = strat->T[i];
    deleteHC(&p,strat, TRUE);
    /*- tries to cancel a unit: -*/
    cancelunit(&p);
    if (p.p != strat->T[i].p)
    {
      strat->sevT[i] = pGetShortExpVector(p.p);
      p.SetpFDeg();
    }
    strat->T[i] = p;
    i++;
  }
}

/*2
* arranges red, pos and T if strat->kHEdgeFound (first time)
*/
void firstUpdate(kStrategy strat)
{
  if (strat->update)
  {
    kTest_TS(strat);
    strat->update = (strat->tl == -1);
    if (TEST_OPT_WEIGHTM)
    {
      pRestoreDegProcs(pFDegOld, pLDegOld);
      if (strat->tailRing != currRing)
      {
        strat->tailRing->pFDeg = strat->pOrigFDeg_TailRing;
        strat->tailRing->pLDeg = strat->pOrigLDeg_TailRing;
      }
      int i;
      for (i=strat->Ll; i>=0; i--)
      {
        strat->L[i].SetpFDeg();
      }
      for (i=strat->tl; i>=0; i--)
      {
        strat->T[i].SetpFDeg();
      }
      if (ecartWeights)
      {
        omFreeSize((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
        ecartWeights=NULL;
      }
    }
    if (TEST_OPT_FASTHC)
    {
      strat->posInL = strat->posInLOld;
      strat->lastAxis = 0;
    }
    if (TEST_OPT_FINDET)
      return;
    strat->red = redFirst;
    strat->use_buckets = kMoraUseBucket(strat);
    updateT(strat);
    strat->posInT = posInT2;
    reorderT(strat);
  }
  kTest_TS(strat);
}

/*2
*-puts p to the standardbasis s at position at
*-reduces the tail of p if TEST_OPT_REDTAIL
*-tries to cancel a unit
*-HEckeTest
*  if TRUE
*  - decides about reduction-strategies
*  - computes noether
*  - stops computation if TEST_OPT_FINDET
*  - cuts the tails of the polynomials
*    in s,t and the elements in L above noether
*    and cancels units if possible
*  - reorders s,L
*/
void enterSMora (LObject p,int atS,kStrategy strat, int atR = -1)
{
  int i;
  enterSBba(p, atS, strat, atR);
  #ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    Print("new s%d:",atS);
    p_wrp(p.p,currRing,strat->tailRing);
    PrintLn();
  }
  #endif
  if ((!strat->kHEdgeFound) || (strat->kNoether!=NULL)) HEckeTest(p.p,strat);
  if (strat->kHEdgeFound)
  {
    if (newHEdge(strat->S,strat))
    {
      firstUpdate(strat);
      if (TEST_OPT_FINDET)
        return;
      /*- cuts elements in L above noether and reorders L -*/
      updateLHC(strat);
      /*- reorders L with respect to posInL -*/
      reorderL(strat);
    }
  }
  else if (strat->kNoether!=NULL)
    strat->kHEdgeFound = TRUE;
  else if (TEST_OPT_FASTHC)
  {
    if (strat->posInLOldFlag)
    {
      missingAxis(&strat->lastAxis,strat);
      if (strat->lastAxis)
      {
        strat->posInLOld = strat->posInL;
        strat->posInLOldFlag = FALSE;
        strat->posInL = posInL10;
        strat->posInLDependsOnLength = TRUE;
        updateL(strat);
        reorderL(strat);
      }
    }
    else if (strat->lastAxis)
      updateL(strat);
  }
}

/*2
*-puts p to the standardbasis s at position at
*-HEckeTest
*  if TRUE
*  - computes noether
*/
void enterSMoraNF (LObject p, int atS,kStrategy strat, int atR = -1)
{
  int i;

  enterSBba(p, atS, strat, atR);
  if ((!strat->kHEdgeFound) || (strat->kNoether!=NULL)) HEckeTest(p.p,strat);
  if (strat->kHEdgeFound)
    newHEdge(strat->S,strat);
  else if (strat->kNoether!=NULL)
    strat->kHEdgeFound = TRUE;
}

void initBba(ideal F,kStrategy strat)
{
  int i;
  idhdl h;
 /* setting global variables ------------------- */
  strat->enterS = enterSBba;
    strat->red = redHoney;
  if (strat->honey)
    strat->red = redHoney;
  else if (pLexOrder && !strat->homog)
    strat->red = redLazy;
  else
  {
    strat->LazyPass *=4;
    strat->red = redHomog;
  }
#ifdef HAVE_RINGS  //TODO Oliver
  if (rField_is_Ring(currRing))
  {
    strat->red = redRing;
  }
#endif
  if (pLexOrder && strat->honey)
    strat->initEcart = initEcartNormal;
  else
    strat->initEcart = initEcartBBA;
  if (strat->honey)
    strat->initEcartPair = initEcartPairMora;
  else
    strat->initEcartPair = initEcartPairBba;
  strat->kIdeal = NULL;
  //if (strat->ak==0) strat->kIdeal->rtyp=IDEAL_CMD;
  //else              strat->kIdeal->rtyp=MODUL_CMD;
  //strat->kIdeal->data=(void *)strat->Shdl;
  if ((TEST_OPT_WEIGHTM)&&(F!=NULL))
  {
    //interred  machen   Aenderung
    pFDegOld=pFDeg;
    pLDegOld=pLDeg;
    //h=ggetid("ecart");
    //if ((h!=NULL) /*&& (IDTYP(h)==INTVEC_CMD)*/)
    //{
    //  ecartWeights=iv2array(IDINTVEC(h));
    //}
    //else
    {
      ecartWeights=(short *)omAlloc((pVariables+1)*sizeof(short));
      /*uses automatic computation of the ecartWeights to set them*/
      kEcartWeights(F->m,IDELEMS(F)-1,ecartWeights);
    }
    pRestoreDegProcs(totaldegreeWecart, maxdegreeWecart);
    if (TEST_OPT_PROT)
    {
      for(i=1; i<=pVariables; i++)
        Print(" %d",ecartWeights[i]);
      PrintLn();
      mflush();
    }
  }
}

void initMora(ideal F,kStrategy strat)
{
  int i,j;
  idhdl h;

  strat->NotUsedAxis = (BOOLEAN *)omAlloc((pVariables+1)*sizeof(BOOLEAN));
  for (j=pVariables; j>0; j--) strat->NotUsedAxis[j] = TRUE;
  strat->enterS = enterSMora;
  strat->initEcartPair = initEcartPairMora; /*- ecart approximation -*/
  strat->posInLOld = strat->posInL;
  strat->posInLOldFlag = TRUE;
  strat->initEcart = initEcartNormal;
  strat->kHEdgeFound = ppNoether != NULL;
  if ( strat->kHEdgeFound )
     strat->kNoether = pCopy(ppNoether);
  else if (strat->kHEdgeFound || strat->homog)
    strat->red = redFirst;  /*take the first possible in T*/
  else
    strat->red = redEcart;/*take the first possible in under ecart-restriction*/
  if (strat->kHEdgeFound)
  {
    strat->HCord = pFDeg(ppNoether,currRing)+1;
    strat->posInT = posInT2;
  }
  else
  {
    strat->HCord = 32000;/*- very large -*/
  }
  /*reads the ecartWeights used for Graebes method from the
   *intvec ecart and set ecartWeights
   */
  if ((TEST_OPT_WEIGHTM)&&(F!=NULL))
  {
    //interred  machen   Aenderung
    pFDegOld=pFDeg;
    pLDegOld=pLDeg;
    //h=ggetid("ecart");
    //if ((h!=NULL) /*&& (IDTYP(h)==INTVEC_CMD)*/)
    //{
    //  ecartWeights=iv2array(IDINTVEC(h));
    //}
    //else
    {
      ecartWeights=(short *)omAlloc((pVariables+1)*sizeof(short));
      /*uses automatic computation of the ecartWeights to set them*/
      kEcartWeights(F->m,IDELEMS(F)-1,ecartWeights);
    }

    pSetDegProcs(totaldegreeWecart, maxdegreeWecart);
    if (TEST_OPT_PROT)
    {
      for(i=1; i<=pVariables; i++)
        Print(" %d",ecartWeights[i]);
      PrintLn();
      mflush();
    }
  }
  kOptimizeLDeg(pLDeg, strat);
}

#ifdef HAVE_ASSUME
static int mora_count = 0;
static int mora_loop_count;
#endif

void kDebugPrint(kStrategy strat);

ideal mora (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat)
{
#ifdef HAVE_ASSUME
  mora_count++;
  mora_loop_count = 0;
#endif
#ifdef KDEBUG
  om_Opts.MinTrack = 5;
#endif
  int srmax;
  int lrmax = 0;
  int olddeg = 0;
  int reduc = 0;
  int red_result = 1;
  int hilbeledeg=1,hilbcount=0;
  int save_test=test;
  if (currRing->MixedOrder)
  {
    test &= ~Sy_bit(OPT_REDSB);
    test &= ~Sy_bit(OPT_REDTAIL);
  }

  strat->update = TRUE;
  /*- setting global variables ------------------- -*/
  initBuchMoraCrit(strat);
  initHilbCrit(F,Q,&hilb,strat);
  initMora(F,strat);
  initBuchMoraPos(strat);
  /*Shdl=*/initBuchMora(F,Q,strat);
  if (TEST_OPT_FASTHC) missingAxis(&strat->lastAxis,strat);
  /*updateS in initBuchMora has Hecketest
  * and could have put strat->kHEdgdeFound FALSE*/
  if (ppNoether!=NULL)
  {
    strat->kHEdgeFound = TRUE;
  }
  if (strat->kHEdgeFound && strat->update)
  {
    firstUpdate(strat);
    updateLHC(strat);
    reorderL(strat);
  }
  if (TEST_OPT_FASTHC && (strat->lastAxis) && strat->posInLOldFlag)
  {
    strat->posInLOld = strat->posInL;
    strat->posInLOldFlag = FALSE;
    strat->posInL = posInL10;
    updateL(strat);
    reorderL(strat);
  }
  srmax = strat->sl;

  kTest_TS(strat);
  strat->use_buckets = kMoraUseBucket(strat);
  /*- compute-------------------------------------------*/

#ifdef HAVE_TAIL_RING
//  if (strat->homog && strat->red == redFirst)
  kStratInitChangeTailRing(strat);
#endif
  if (BVERBOSE(23))
  {
    kDebugPrint(strat);
  }

  while (strat->Ll >= 0)
  {
#ifdef HAVE_ASSUME
    mora_loop_count++;
#endif
    if (lrmax< strat->Ll) lrmax=strat->Ll; /*stat*/
    //test_int_std(strat->kIdeal);
    #ifdef KDEBUG
    if (TEST_OPT_DEBUG) messageSets(strat);
    #endif
    if (TEST_OPT_DEGBOUND
    && (strat->L[strat->Ll].ecart+strat->L[strat->Ll].GetpFDeg()> Kstd1_deg))
    {
      /*
      * stops computation if
      * - 24 (degBound)
      *   && upper degree is bigger than Kstd1_deg
      */
      while ((strat->Ll >= 0)
        && (strat->L[strat->Ll].p1!=NULL) && (strat->L[strat->Ll].p2!=NULL)
        && (strat->L[strat->Ll].ecart+strat->L[strat->Ll].GetpFDeg()> Kstd1_deg)
      )
      {
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
        //if (TEST_OPT_PROT)
        //{
        //   PrintS("D"); mflush();
        //}
      }
      if (strat->Ll<0) break;
      else strat->noClearS=TRUE;
    }
    strat->P = strat->L[strat->Ll];/*- picks the last element from the lazyset L -*/
    if (strat->Ll==0) strat->interpt=TRUE;
    strat->Ll--;

    // create the real Spoly
    if (pNext(strat->P.p) == strat->tail)
    {
      /*- deletes the short spoly and computes -*/
#ifdef HAVE_RINGS_LOC
      if (rField_is_Ring(currRing))
        pLmDelete(strat->P.p);
      else
#endif
      pLmFree(strat->P.p);
      strat->P.p = NULL;
      poly m1 = NULL, m2 = NULL;
      // check that spoly creation is ok
      while (strat->tailRing != currRing &&
             !kCheckSpolyCreation(&(strat->P), strat, m1, m2))
      {
        assume(m1 == NULL && m2 == NULL);
        // if not, change to a ring where exponents are large enough
        kStratChangeTailRing(strat);
      }
      /* create the real one */
      ksCreateSpoly(&(strat->P), strat->kNoetherTail(), strat->use_buckets,
                    strat->tailRing, m1, m2, strat->R);
      if (!strat->use_buckets)
        strat->P.SetLength(strat->length_pLength);
    }
    else if (strat->P.p1 == NULL)
    {
      // for input polys, prepare reduction (buckets !)
      strat->P.SetLength(strat->length_pLength);
      strat->P.PrepareRed(strat->use_buckets);
    }

    if (!strat->P.IsNull())
    {
      // might be NULL from noether !!!
      if (TEST_OPT_PROT)
        message(strat->P.ecart+strat->P.GetpFDeg(),&olddeg,&reduc,strat, red_result);
      // reduce
      red_result = strat->red(&strat->P,strat);
    }

    if (! strat->P.IsNull())
    {
      strat->P.GetP();
      // statistics
      if (TEST_OPT_PROT) PrintS("s");
      // normalization
      if (!TEST_OPT_INTSTRATEGY)
        strat->P.pNorm();
      // tailreduction
      strat->P.p = redtail(&(strat->P),strat->sl,strat);
      // set ecart -- might have changed because of tail reductions
      if ((!strat->noTailReduction) && (!strat->honey))
        strat->initEcart(&strat->P);
      // cancel unit
      cancelunit(&strat->P);
      // for char 0, clear denominators
      if (TEST_OPT_INTSTRATEGY)
        strat->P.pCleardenom();

      // put in T
      enterT(strat->P,strat);
      // build new pairs
#ifdef HAVE_RINGS_LOC
      if (rField_is_Ring(currRing))
        superenterpairs(strat->P.p,strat->sl,strat->P.ecart,0,strat, strat->tl);
      else
#endif
      enterpairs(strat->P.p,strat->sl,strat->P.ecart,0,strat, strat->tl);
      // put in S
      strat->enterS(strat->P,
                    posInS(strat,strat->sl,strat->P.p, strat->P.ecart),
                    strat, strat->tl);

      // apply hilbert criterion
      if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);

      // clear strat->P
      if (strat->P.lcm!=NULL)
#ifdef HAVE_RINGS_LOC
        pLmDelete(strat->P.lcm);
#else
        pLmFree(strat->P.lcm);
#endif
      strat->P.lcm=NULL;
#ifdef KDEBUG
      // make sure kTest_TS does not complain about strat->P
      memset(&strat->P,0,sizeof(strat->P));
#endif
      if (strat->sl>srmax) srmax = strat->sl; /*stat.*/
      if (strat->Ll>lrmax) lrmax = strat->Ll;
    }
    if (strat->kHEdgeFound)
    {
      if ((TEST_OPT_FINDET)
      || ((TEST_OPT_MULTBOUND) && (scMult0Int((strat->Shdl)) < Kstd1_mu)))
      {
        // obachman: is this still used ???
        /*
        * stops computation if strat->kHEdgeFound and
        * - 27 (finiteDeterminacyTest)
        * or
        * - 23
        *   (multBound)
        *   && multiplicity of the ideal is smaller then a predefined number mu
        */
        while (strat->Ll >= 0) deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      }
    }
    kTest_TS(strat);
  }
  /*- complete reduction of the standard basis------------------------ -*/
  if (TEST_OPT_REDSB) completeReduce(strat);
  else if (TEST_OPT_PROT) PrintLn();
  /*- release temp data------------------------------- -*/
  exitBuchMora(strat);
  /*- polynomials used for HECKE: HC, noether -*/
  if (TEST_OPT_FINDET)
  {
    if (strat->kHEdge!=NULL)
      Kstd1_mu=pFDeg(strat->kHEdge,currRing);
    else
      Kstd1_mu=-1;
  }
  pDelete(&strat->kHEdge);
  strat->update = TRUE; //???
  strat->lastAxis = 0; //???
  pDelete(&strat->kNoether);
  omFreeSize((ADDRESS)strat->NotUsedAxis,(pVariables+1)*sizeof(BOOLEAN));
  if (TEST_OPT_PROT) messageStat(srmax,lrmax,hilbcount,strat);
  if (TEST_OPT_WEIGHTM)
  {
    pRestoreDegProcs(pFDegOld, pLDegOld);
    if (ecartWeights)
    {
      omFreeSize((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }
  if (Q!=NULL) updateResult(strat->Shdl,Q,strat);
  test=save_test;
  idTest(strat->Shdl);
  return (strat->Shdl);
}

poly kNF1 (ideal F,ideal Q,poly q, kStrategy strat, int lazyReduce)
{
  assume(q!=NULL);
  assume(!(idIs0(F)&&(Q==NULL)));

// lazy_reduce flags: can be combined by |
//#define KSTD_NF_LAZY   1
  // do only a reduction of the leading term
//#define KSTD_NF_ECART  2
  // only local: recude even with bad ecart
  poly   p;
  int   i;
  int   j;
  int   o;
  LObject   h;
  BITSET save_test=test;

  //if ((idIs0(F))&&(Q==NULL))
  //  return pCopy(q); /*F=0*/
  //strat->ak = si_max(idRankFreeModule(F),pMaxComp(q));
  /*- creating temp data structures------------------- -*/
  strat->kHEdgeFound = ppNoether != NULL;
  strat->kNoether    = pCopy(ppNoether);
  test|=Sy_bit(OPT_REDTAIL);
  test&=~Sy_bit(OPT_INTSTRATEGY);
  if (TEST_OPT_STAIRCASEBOUND
  && (! TEST_V_DEG_STOP)
  && (0<Kstd1_deg)
  && ((!strat->kHEdgeFound)
    ||(TEST_OPT_DEGBOUND && (pWTotaldegree(strat->kNoether)<Kstd1_deg))))
  {
    pDelete(&strat->kNoether);
    strat->kNoether=pOne();
    pSetExp(strat->kNoether,1, Kstd1_deg+1);
    pSetm(strat->kNoether);
    strat->kHEdgeFound=TRUE;
  }
  initBuchMoraCrit(strat);
  initBuchMoraPos(strat);
  initMora(F,strat);
  strat->enterS = enterSMoraNF;
  /*- set T -*/
  strat->tl = -1;
  strat->tmax = setmaxT;
  strat->T = initT();
  strat->R = initR();
  strat->sevT = initsevT();
  /*- set S -*/
  strat->sl = -1;
  /*- init local data struct.-------------------------- -*/
  /*Shdl=*/initS(F,Q,strat);
  if ((strat->ak!=0)
  && (strat->kHEdgeFound))
  {
    if (strat->ak!=1)
    {
      pSetComp(strat->kNoether,1);
      pSetmComp(strat->kNoether);
      poly p=pHead(strat->kNoether);
      pSetComp(p,strat->ak);
      pSetmComp(p);
      p=pAdd(strat->kNoether,p);
      strat->kNoether=pNext(p);
      p_LmFree(p,currRing);
    }
  }
  if ((lazyReduce & KSTD_NF_LAZY)==0)
  {
    for (i=strat->sl; i>=0; i--)
      pNorm(strat->S[i]);
  }
  /*- puts the elements of S also to T -*/
  for (i=0; i<=strat->sl; i++)
  {
    h.p = strat->S[i];
    h.ecart = strat->ecartS[i];
    if (strat->sevS[i] == 0) strat->sevS[i] = pGetShortExpVector(h.p);
    else assume(strat->sevS[i] == pGetShortExpVector(h.p));
    h.length = pLength(h.p);
    h.sev = strat->sevS[i];
    h.SetpFDeg();
    enterT(h,strat);
  }
  /*- compute------------------------------------------- -*/
  p = pCopy(q);
  deleteHC(&p,&o,&j,strat);
  kTest(strat);
  if (TEST_OPT_PROT) { PrintS("r"); mflush(); }
  if (BVERBOSE(23)) kDebugPrint(strat);
  if (p!=NULL) p = redMoraNF(p,strat, lazyReduce & KSTD_NF_ECART);
  if ((p!=NULL)&&((lazyReduce & KSTD_NF_LAZY)==0))
  {
    if (TEST_OPT_PROT) { PrintS("t"); mflush(); }
    p = redtail(p,strat->sl,strat);
  }
  /*- release temp data------------------------------- -*/
  cleanT(strat);
  omFreeSize((ADDRESS)strat->T,strat->tmax*sizeof(TObject));
  omFreeSize((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  omFreeSize((ADDRESS)strat->sevS,IDELEMS(strat->Shdl)*sizeof(unsigned long));
  omFreeSize((ADDRESS)strat->NotUsedAxis,(pVariables+1)*sizeof(BOOLEAN));
  omfree(strat->sevT);
  omfree(strat->S_2_R);
  omfree(strat->R);

  if ((Q!=NULL)&&(strat->fromQ!=NULL))
  {
    i=((IDELEMS(Q)+IDELEMS(F)+15)/16)*16;
    omFreeSize((ADDRESS)strat->fromQ,i*sizeof(int));
    strat->fromQ=NULL;
  }
  pDelete(&strat->kHEdge);
  pDelete(&strat->kNoether);
  if ((TEST_OPT_WEIGHTM)&&(F!=NULL))
  {
    pRestoreDegProcs(pFDegOld, pLDegOld);
    if (ecartWeights)
    {
      omFreeSize((ADDRESS *)&ecartWeights,(pVariables+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }
  idDelete(&strat->Shdl);
  test=save_test;
  if (TEST_OPT_PROT) PrintLn();
  return p;
}

ideal kNF1 (ideal F,ideal Q,ideal q, kStrategy strat, int lazyReduce)
{
  assume(!idIs0(q));
  assume(!(idIs0(F)&&(Q==NULL)));

// lazy_reduce flags: can be combined by |
//#define KSTD_NF_LAZY   1
  // do only a reduction of the leading term
//#define KSTD_NF_ECART  2
  // only local: recude even with bad ecart
  poly   p;
  int   i;
  int   j;
  int   o;
  LObject   h;
  ideal res;
  BITSET save_test=test;

  //if (idIs0(q)) return idInit(IDELEMS(q),si_max(q->rank,F->rank));
  //if ((idIs0(F))&&(Q==NULL))
  //  return idCopy(q); /*F=0*/
  //strat->ak = si_max(idRankFreeModule(F),idRankFreeModule(q));
  /*- creating temp data structures------------------- -*/
  strat->kHEdgeFound = ppNoether != NULL;
  strat->kNoether=pCopy(ppNoether);
  test|=Sy_bit(OPT_REDTAIL);
  if (TEST_OPT_STAIRCASEBOUND
  && (0<Kstd1_deg)
  && ((!strat->kHEdgeFound)
    ||(TEST_OPT_DEGBOUND && (pWTotaldegree(strat->kNoether)<Kstd1_deg))))
  {
    pDelete(&strat->kNoether);
    strat->kNoether=pOne();
    pSetExp(strat->kNoether,1, Kstd1_deg+1);
    pSetm(strat->kNoether);
    strat->kHEdgeFound=TRUE;
  }
  initBuchMoraCrit(strat);
  initBuchMoraPos(strat);
  initMora(F,strat);
  strat->enterS = enterSMoraNF;
  /*- set T -*/
  strat->tl = -1;
  strat->tmax = setmaxT;
  strat->T = initT();
  strat->R = initR();
  strat->sevT = initsevT();
  /*- set S -*/
  strat->sl = -1;
  /*- init local data struct.-------------------------- -*/
  /*Shdl=*/initS(F,Q,strat);
  if ((strat->ak!=0)
  && (strat->kHEdgeFound))
  {
    if (strat->ak!=1)
    {
      pSetComp(strat->kNoether,1);
      pSetmComp(strat->kNoether);
      poly p=pHead(strat->kNoether);
      pSetComp(p,strat->ak);
      pSetmComp(p);
      p=pAdd(strat->kNoether,p);
      strat->kNoether=pNext(p);
      p_LmFree(p,currRing);
    }
  }
  if (TEST_OPT_INTSTRATEGY && ((lazyReduce & KSTD_NF_LAZY)==0))
  {
    for (i=strat->sl; i>=0; i--)
      pNorm(strat->S[i]);
  }
  /*- compute------------------------------------------- -*/
  res=idInit(IDELEMS(q),strat->ak);
  for (i=0; i<IDELEMS(q); i++)
  {
    if (q->m[i]!=NULL)
    {
      p = pCopy(q->m[i]);
      deleteHC(&p,&o,&j,strat);
      if (p!=NULL)
      {
        /*- puts the elements of S also to T -*/
        for (j=0; j<=strat->sl; j++)
        {
          h.p = strat->S[j];
          h.ecart = strat->ecartS[j];
          h.pLength = h.length = pLength(h.p);
          if (strat->sevS[j] == 0) strat->sevS[j] = pGetShortExpVector(h.p);
          else assume(strat->sevS[j] == pGetShortExpVector(h.p));
          h.sev = strat->sevS[j];
          h.SetpFDeg();
          enterT(h,strat);
        }
        if (TEST_OPT_PROT) { PrintS("r"); mflush(); }
        p = redMoraNF(p,strat, lazyReduce & KSTD_NF_ECART);
        if ((p!=NULL)&&((lazyReduce & KSTD_NF_LAZY)==0))
        {
          if (TEST_OPT_PROT) { PrintS("t"); mflush(); }
          p = redtail(p,strat->sl,strat);
        }
        cleanT(strat);
      }
      res->m[i]=p;
    }
    //else
    //  res->m[i]=NULL;
  }
  /*- release temp data------------------------------- -*/
  omFreeSize((ADDRESS)strat->T,strat->tmax*sizeof(TObject));
  omFreeSize((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  omFreeSize((ADDRESS)strat->sevS,IDELEMS(strat->Shdl)*sizeof(unsigned long));
  omFreeSize((ADDRESS)strat->NotUsedAxis,(pVariables+1)*sizeof(BOOLEAN));
  omfree(strat->sevT);
  omfree(strat->S_2_R);
  omfree(strat->R);
  if ((Q!=NULL)&&(strat->fromQ!=NULL))
  {
    i=((IDELEMS(Q)+IDELEMS(F)+15)/16)*16;
    omFreeSize((ADDRESS)strat->fromQ,i*sizeof(int));
    strat->fromQ=NULL;
  }
  pDelete(&strat->kHEdge);
  pDelete(&strat->kNoether);
  if ((TEST_OPT_WEIGHTM)&&(F!=NULL))
  {
    pFDeg=pFDegOld;
    pLDeg=pLDegOld;
    if (ecartWeights)
    {
      omFreeSize((ADDRESS *)&ecartWeights,(pVariables+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }
  idDelete(&strat->Shdl);
  test=save_test;
  if (TEST_OPT_PROT) PrintLn();
  return res;
}

pFDegProc pFDegOld;
pLDegProc pLDegOld;
intvec * kModW, * kHomW;

long kModDeg(poly p, ring r)
{
  long o=pWDegree(p, r);
  long i=p_GetComp(p, r);
  if (i==0) return o;
  //assume((i>0) && (i<=kModW->length()));
  if (i<=kModW->length())
    return o+(*kModW)[i-1];
  return o;
}
long kHomModDeg(poly p, ring r)
{
  int i;
  long j=0;

  for (i=r->N;i>0;i--)
    j+=p_GetExp(p,i,r)*(*kHomW)[i-1];
  if (kModW == NULL) return j;
  i = p_GetComp(p,r);
  if (i==0) return j;
  return j+(*kModW)[i-1];
}

ideal kStd(ideal F, ideal Q, tHomog h,intvec ** w, intvec *hilb,int syzComp,
          int newIdeal, intvec *vw)
{
  if(idIs0(F))
    return idInit(1,F->rank);

  ideal r;
  BOOLEAN b=pLexOrder,toReset=FALSE;
  BOOLEAN delete_w=(w==NULL);
  kStrategy strat=new skStrategy;

  if(!TEST_OPT_RETURN_SB)
    strat->syzComp = syzComp;
  if (TEST_OPT_SB_1)
    strat->newIdeal = newIdeal;
  if (rField_has_simple_inverse())
    strat->LazyPass=20;
  else
    strat->LazyPass=2;
  strat->LazyDegree = 1;
  strat->enterOnePair=enterOnePairNormal;
  strat->chainCrit=chainCritNormal;
  strat->ak = idRankFreeModule(F);
  strat->kModW=kModW=NULL;
  strat->kHomW=kHomW=NULL;
  if (vw != NULL)
  {
    pLexOrder=FALSE;
    strat->kHomW=kHomW=vw;
    pFDegOld = pFDeg;
    pLDegOld = pLDeg;
    pSetDegProcs(kHomModDeg);
    toReset = TRUE;
  }
  if (h==testHomog)
  {
    if (strat->ak == 0)
    {
      h = (tHomog)idHomIdeal(F,Q);
      w=NULL;
    }
    else if (!TEST_OPT_DEGBOUND)
    {
      h = (tHomog)idHomModule(F,Q,w);
    }
  }
  pLexOrder=b;
  if (h==isHomog)
  {
    if (strat->ak > 0 && (w!=NULL) && (*w!=NULL))
    {
      strat->kModW = kModW = *w;
      if (vw == NULL)
      {
        pFDegOld = pFDeg;
        pLDegOld = pLDeg;
        pSetDegProcs(kModDeg);
        toReset = TRUE;
      }
    }
    pLexOrder = TRUE;
    if (hilb==NULL) strat->LazyPass*=2;
  }
  strat->homog=h;
#ifdef KDEBUG
  idTest(F);
  idTest(Q);

#if MYTEST
  if (TEST_OPT_DEBUG)
  {
    PrintS("// kSTD: currRing: ");
    rWrite(currRing);
  }
#endif

#endif
#ifdef HAVE_PLURAL
  if (rIsPluralRing(currRing))
  {
    const BOOLEAN bIsSCA  = rIsSCA(currRing) && strat->z2homog; // for Z_2 prod-crit
    strat->no_prod_crit   = ! bIsSCA;
    if (w!=NULL)
      r = nc_GB(F, Q, *w, hilb, strat);
    else
      r = nc_GB(F, Q, NULL, hilb, strat);
  }
  else
#endif
#ifdef HAVE_RINGS
  if (rField_is_Ring(currRing))
    r=bba(F,Q,NULL,hilb,strat);
  else
#endif
  {
    if (pOrdSgn==-1)
    {
      if (w!=NULL)
        r=mora(F,Q,*w,hilb,strat);
      else
        r=mora(F,Q,NULL,hilb,strat);
    }
    else
    {
      if (w!=NULL)
        r=bba(F,Q,*w,hilb,strat);
      else
        r=bba(F,Q,NULL,hilb,strat);
    }
  }
#ifdef KDEBUG
  idTest(r);
#endif
  if (toReset)
  {
    kModW = NULL;
    pRestoreDegProcs(pFDegOld, pLDegOld);
  }
  pLexOrder = b;
//Print("%d reductions canceled \n",strat->cel);
  HCord=strat->HCord;
  delete(strat);
  if ((delete_w)&&(w!=NULL)&&(*w!=NULL)) delete *w;
  return r;
}

#ifdef HAVE_SHIFTBBA
ideal kStdShift(ideal F, ideal Q, tHomog h,intvec ** w, intvec *hilb,int syzComp,
                int newIdeal, intvec *vw, int uptodeg, int lV)
{
  ideal r;
  BOOLEAN b=pLexOrder,toReset=FALSE;
  BOOLEAN delete_w=(w==NULL);
  kStrategy strat=new skStrategy;

  if(!TEST_OPT_RETURN_SB)
    strat->syzComp = syzComp;
  if (TEST_OPT_SB_1)
    strat->newIdeal = newIdeal;
  if (rField_has_simple_inverse())
    strat->LazyPass=20;
  else
    strat->LazyPass=2;
  strat->LazyDegree = 1;
  strat->ak = idRankFreeModule(F);
  strat->kModW=kModW=NULL;
  strat->kHomW=kHomW=NULL;
  if (vw != NULL)
  {
    pLexOrder=FALSE;
    strat->kHomW=kHomW=vw;
    pFDegOld = pFDeg;
    pLDegOld = pLDeg;
    pSetDegProcs(kHomModDeg);
    toReset = TRUE;
  }
  if (h==testHomog)
  {
    if (strat->ak == 0)
    {
      h = (tHomog)idHomIdeal(F,Q);
      w=NULL;
    }
    else if (!TEST_OPT_DEGBOUND)
    {
      h = (tHomog)idHomModule(F,Q,w);
    }
  }
  pLexOrder=b;
  if (h==isHomog)
  {
    if (strat->ak > 0 && (w!=NULL) && (*w!=NULL))
    {
      strat->kModW = kModW = *w;
      if (vw == NULL)
      {
        pFDegOld = pFDeg;
        pLDegOld = pLDeg;
        pSetDegProcs(kModDeg);
        toReset = TRUE;
      }
    }
    pLexOrder = TRUE;
    if (hilb==NULL) strat->LazyPass*=2;
  }
  strat->homog=h;
#ifdef KDEBUG
  idTest(F);
#endif
  if (pOrdSgn==-1)
  {
    /* error: no local ord yet with shifts */
    Print("No local ordering possible for shifts");
    return(NULL);
  }
  else
  {
    /* global ordering */
    if (w!=NULL)
      r=bbaShift(F,Q,*w,hilb,strat,uptodeg,lV);
    else
      r=bbaShift(F,Q,NULL,hilb,strat,uptodeg,lV);
  }
#ifdef KDEBUG
  idTest(r);
#endif
  if (toReset)
  {
    kModW = NULL;
    pRestoreDegProcs(pFDegOld, pLDegOld);
  }
  pLexOrder = b;
//Print("%d reductions canceled \n",strat->cel);
  HCord=strat->HCord;
  delete(strat);
  if ((delete_w)&&(w!=NULL)&&(*w!=NULL)) delete *w;
  return r;
}
#endif

//##############################################################
//##############################################################
//##############################################################
//##############################################################
//##############################################################

ideal kMin_std(ideal F, ideal Q, tHomog h,intvec ** w, ideal &M, intvec *hilb,
              int syzComp, int reduced)
{
  if(idIs0(F))
  {
    M=idInit(1,F->rank);
    return idInit(1,F->rank);
  }

  ideal r=NULL;
  int Kstd1_OldDeg = Kstd1_deg,i;
  intvec* temp_w=NULL;
  BOOLEAN b=pLexOrder,toReset=FALSE;
  BOOLEAN delete_w=(w==NULL);
  BOOLEAN oldDegBound=TEST_OPT_DEGBOUND;
  kStrategy strat=new skStrategy;

  if(!TEST_OPT_RETURN_SB)
     strat->syzComp = syzComp;
  if (rField_has_simple_inverse())
    strat->LazyPass=20;
  else
    strat->LazyPass=2;
  strat->LazyDegree = 1;
  strat->minim=(reduced % 2)+1;
  strat->ak = idRankFreeModule(F);
  if (delete_w)
  {
    temp_w=new intvec((strat->ak)+1);
    w = &temp_w;
  }
  if ((h==testHomog)
  )
  {
    if (strat->ak == 0)
    {
      h = (tHomog)idHomIdeal(F,Q);
      w=NULL;
    }
    else
    {
      h = (tHomog)idHomModule(F,Q,w);
    }
  }
  if (h==isHomog)
  {
    if (strat->ak > 0 && (w!=NULL) && (*w!=NULL))
    {
      kModW = *w;
      strat->kModW = *w;
      assume(pFDeg != NULL && pLDeg != NULL);
      pFDegOld = pFDeg;
      pLDegOld = pLDeg;
      pSetDegProcs(kModDeg);

      toReset = TRUE;
      if (reduced>1)
      {
        Kstd1_OldDeg=Kstd1_deg;
        Kstd1_deg = -1;
        for (i=IDELEMS(F)-1;i>=0;i--)
        {
          if ((F->m[i]!=NULL) && (pFDeg(F->m[i],currRing)>=Kstd1_deg))
            Kstd1_deg = pFDeg(F->m[i],currRing)+1;
        }
      }
    }
    pLexOrder = TRUE;
    strat->LazyPass*=2;
  }
  strat->homog=h;
  if (pOrdSgn==-1)
  {
    if (w!=NULL)
      r=mora(F,Q,*w,hilb,strat);
    else
      r=mora(F,Q,NULL,hilb,strat);
  }
  else
  {
    if (w!=NULL)
      r=bba(F,Q,*w,hilb,strat);
    else
      r=bba(F,Q,NULL,hilb,strat);
  }
#ifdef KDEBUG
  {
    int i;
    for (i=IDELEMS(r)-1; i>=0; i--) pTest(r->m[i]);
  }
#endif
  idSkipZeroes(r);
  if (toReset)
  {
    pRestoreDegProcs(pFDegOld, pLDegOld);
    kModW = NULL;
  }
  pLexOrder = b;
  HCord=strat->HCord;
  if ((delete_w)&&(temp_w!=NULL)) delete temp_w;
  if ((IDELEMS(r)==1) && (r->m[0]!=NULL) && pIsConstant(r->m[0]) && (strat->ak==0))
  {
    M=idInit(1,F->rank);
    M->m[0]=pOne();
    //if (strat->ak!=0) { pSetComp(M->m[0],strat->ak); pSetmComp(M->m[0]); }
    if (strat->M!=NULL) idDelete(&strat->M);
  }
  else if (strat->M==NULL)
  {
    M=idInit(1,F->rank);
    Warn("no minimal generating set computed");
  }
  else
  {
    idSkipZeroes(strat->M);
    M=strat->M;
  }
  delete(strat);
  if (reduced>2)
  {
    Kstd1_deg=Kstd1_OldDeg;
    if (!oldDegBound)
      test &= ~Sy_bit(OPT_DEGBOUND);
  }
  else
  {
    if (IDELEMS(M)>IDELEMS(r)) { idDelete(&M); M=idCopy(r); }
  }
  return r;
}

poly kNF(ideal F, ideal Q, poly p,int syzComp, int lazyReduce)
{
  if (p==NULL)
     return NULL;

  poly pp = p;

#ifdef HAVE_PLURAL
  if(rIsSCA(currRing))
  {
    const unsigned int m_iFirstAltVar = scaFirstAltVar(currRing);
    const unsigned int m_iLastAltVar  = scaLastAltVar(currRing);
    pp = p_KillSquares(pp, m_iFirstAltVar, m_iLastAltVar, currRing);

    if(Q == currQuotient)
      Q = SCAQuotient(currRing);
  }
#endif

  if ((idIs0(F))&&(Q==NULL))
  {
#ifdef HAVE_PLURAL
    if(p != pp)
      return pp;
#endif
    return pCopy(p); /*F+Q=0*/
  }

  kStrategy strat=new skStrategy;
  strat->syzComp = syzComp;
  strat->ak = si_max(idRankFreeModule(F),pMaxComp(p));
  poly res;

  if (pOrdSgn==-1)
    res=kNF1(F,Q,pp,strat,lazyReduce);
  else
    res=kNF2(F,Q,pp,strat,lazyReduce);
  delete(strat);

#ifdef HAVE_PLURAL
  if(pp != p)
    p_Delete(&pp, currRing);
#endif
  return res;
}

ideal kNF(ideal F, ideal Q, ideal p,int syzComp,int lazyReduce)
{
  ideal res;
  if (TEST_OPT_PROT)
  {
    Print("(S:%d)",IDELEMS(p));mflush();
  }
  if (idIs0(p))
    return idInit(IDELEMS(p),si_max(p->rank,F->rank));

  ideal pp = p;
#ifdef HAVE_PLURAL
  if(rIsSCA(currRing))
  {
    const unsigned int m_iFirstAltVar = scaFirstAltVar(currRing);
    const unsigned int m_iLastAltVar  = scaLastAltVar(currRing);
    pp = id_KillSquares(pp, m_iFirstAltVar, m_iLastAltVar, currRing, false);

    if(Q == currQuotient)
      Q = SCAQuotient(currRing);
  }
#endif

  if ((idIs0(F))&&(Q==NULL))
  {
#ifdef HAVE_PLURAL
    if(p != pp)
      return pp;
#endif
    return idCopy(p); /*F+Q=0*/
  }

  kStrategy strat=new skStrategy;
  strat->syzComp = syzComp;
  strat->ak = si_max(idRankFreeModule(F),idRankFreeModule(p));
  if (strat->ak>0) // only for module case, see Tst/Short/bug_reduce.tst
  {
    strat->ak = si_max(strat->ak,(int)F->rank);
  }

  if (pOrdSgn==-1)
    res=kNF1(F,Q,pp,strat,lazyReduce);
  else
    res=kNF2(F,Q,pp,strat,lazyReduce);
  delete(strat);

#ifdef HAVE_PLURAL
  if(pp != p)
    id_Delete(&pp, currRing);
#endif

  return res;
}

/*2
*interreduces F
*/
// old version
ideal kInterRedOld (ideal F, ideal Q)
{
  int j;
  kStrategy strat = new skStrategy;

  ideal tempF = F;
  ideal tempQ = Q;

#ifdef HAVE_PLURAL
  if(rIsSCA(currRing))
  {
    const unsigned int m_iFirstAltVar = scaFirstAltVar(currRing);
    const unsigned int m_iLastAltVar  = scaLastAltVar(currRing);
    tempF = id_KillSquares(F, m_iFirstAltVar, m_iLastAltVar, currRing);

    // this should be done on the upper level!!! :
    //    tempQ = SCAQuotient(currRing);

    if(Q == currQuotient)
      tempQ = SCAQuotient(currRing);
  }
#endif

//  if (TEST_OPT_PROT)
//  {
//    writeTime("start InterRed:");
//    mflush();
//  }
  //strat->syzComp     = 0;
  strat->kHEdgeFound = ppNoether != NULL;
  strat->kNoether=pCopy(ppNoether);
  strat->ak = idRankFreeModule(tempF);
  initBuchMoraCrit(strat);
  strat->NotUsedAxis = (BOOLEAN *)omAlloc((pVariables+1)*sizeof(BOOLEAN));
  for (j=pVariables; j>0; j--) strat->NotUsedAxis[j] = TRUE;
  strat->enterS      = enterSBba;
  strat->posInT      = posInT17;
  strat->initEcart   = initEcartNormal;
  strat->sl   = -1;
  strat->tl          = -1;
  strat->tmax        = setmaxT;
  strat->T           = initT();
  strat->R           = initR();
  strat->sevT        = initsevT();
  if (pOrdSgn == -1)   strat->honey = TRUE;
  initS(tempF, tempQ, strat);
  if (TEST_OPT_REDSB)
    strat->noTailReduction=FALSE;
  updateS(TRUE,strat);
  if (TEST_OPT_REDSB && TEST_OPT_INTSTRATEGY)
    completeReduce(strat);
  //else if (TEST_OPT_PROT) PrintLn();
  pDelete(&strat->kHEdge);
  omFreeSize((ADDRESS)strat->T,strat->tmax*sizeof(TObject));
  omFreeSize((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  omFreeSize((ADDRESS)strat->sevS,IDELEMS(strat->Shdl)*sizeof(unsigned long));
  omFreeSize((ADDRESS)strat->NotUsedAxis,(pVariables+1)*sizeof(BOOLEAN));
  omfree(strat->sevT);
  omfree(strat->S_2_R);
  omfree(strat->R);

  if (strat->fromQ)
  {
    for (j=IDELEMS(strat->Shdl)-1;j>=0;j--)
    {
      if(strat->fromQ[j]) pDelete(&strat->Shdl->m[j]);
    }
    omFreeSize((ADDRESS)strat->fromQ,IDELEMS(strat->Shdl)*sizeof(int));
  }
//  if (TEST_OPT_PROT)
//  {
//    writeTime("end Interred:");
//    mflush();
//  }
  ideal shdl=strat->Shdl;
  idSkipZeroes(shdl);
  if (strat->fromQ)
  {
    strat->fromQ=NULL;
    ideal res=kInterRed(shdl,NULL);
    idDelete(&shdl);
    shdl=res;
  }
  delete(strat);
#ifdef HAVE_PLURAL
  if( tempF != F )
    id_Delete( &tempF, currRing);
#endif
  return shdl;
}
// new version
ideal kInterRedBba (ideal F, ideal Q, int &need_retry)
{
  need_retry=0;
  int   srmax,lrmax, red_result = 1;
  int   olddeg,reduc;
  BOOLEAN withT = FALSE;
  BOOLEAN toReset=FALSE;
  kStrategy strat=new skStrategy;
  tHomog h;
  intvec * w=NULL;

  if (rField_has_simple_inverse())
    strat->LazyPass=20;
  else
    strat->LazyPass=2;
  strat->LazyDegree = 1;
  strat->ak = idRankFreeModule(F);
  strat->syzComp = strat->ak;
  strat->kModW=kModW=NULL;
  strat->kHomW=kHomW=NULL;
  if (strat->ak == 0)
  {
    h = (tHomog)idHomIdeal(F,Q);
    w=NULL;
  }
  else if (!TEST_OPT_DEGBOUND)
  {
    h = (tHomog)idHomModule(F,Q,&w);
  }
  if (h==isHomog)
  {
    if (strat->ak > 0 && (w!=NULL) && (w!=NULL))
    {
      strat->kModW = kModW = w;
      pFDegOld = pFDeg;
      pLDegOld = pLDeg;
      pSetDegProcs(kModDeg);
      toReset = TRUE;
    }
    strat->LazyPass*=2;
  }
  strat->homog=h;
#ifdef KDEBUG
  idTest(F);
#endif

  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  initBuchMoraPos(strat);
  initBba(F,strat);
  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  strat->posInL=posInL0; /* ord according pComp */

  /*Shdl=*/initBuchMora(F, Q, strat);
  srmax = strat->sl;
  reduc = olddeg = lrmax = 0;

#ifndef NO_BUCKETS
  if (!TEST_OPT_NOT_BUCKETS)
    strat->use_buckets = 1;
#endif

  // redtailBBa against T for inhomogenous input
  if (!TEST_OPT_OLDSTD)
    withT = ! strat->homog;

  // strat->posInT = posInT_pLength;
  kTest_TS(strat);

#ifdef HAVE_TAIL_RING
  kStratInitChangeTailRing(strat);
#endif

  /* compute------------------------------------------------------- */
  while (strat->Ll >= 0)
  {
    if (strat->Ll > lrmax) lrmax =strat->Ll;/*stat.*/
    #ifdef KDEBUG
      if (TEST_OPT_DEBUG) messageSets(strat);
    #endif
    if (strat->Ll== 0) strat->interpt=TRUE;
    /* picks the last element from the lazyset L */
    strat->P = strat->L[strat->Ll];
    strat->Ll--;

    if (strat->P.p1 == NULL)
    {
      // for input polys, prepare reduction
      strat->P.PrepareRed(strat->use_buckets);
    }

    if (strat->P.p == NULL && strat->P.t_p == NULL)
    {
      red_result = 0;
    }
    else
    {
      int deg_before=olddeg;
      if (TEST_OPT_PROT)
        message(strat->P.pFDeg(),
                &olddeg,&reduc,strat, red_result);

      /* reduction of the element chosen from L */
      red_result = strat->red(&strat->P,strat);
    }

    // reduction to non-zero new poly
    if (red_result == 1)
    {
      /* statistic */
      if (TEST_OPT_PROT) PrintS("s");

      // get the polynomial (canonicalize bucket, make sure P.p is set)
      strat->P.GetP(strat->lmBin);

      int pos=posInS(strat,strat->sl,strat->P.p,strat->P.ecart);

      // reduce the tail and normalize poly
      // in the ring case we cannot expect LC(f) = 1,
      // therefore we call pContent instead of pNorm
      if ((TEST_OPT_INTSTRATEGY) || (rField_is_Ring(currRing)))
      {
        strat->P.pCleardenom();
        if (0)
        //if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
        {
          strat->P.p = redtailBba(&(strat->P),pos-1,strat, withT);
          strat->P.pCleardenom();
        }
      }
      else
      {
        strat->P.pNorm();
        if (0)
        //if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
          strat->P.p = redtailBba(&(strat->P),pos-1,strat, withT);
      }

#ifdef KDEBUG
      if (TEST_OPT_DEBUG){PrintS("new s:");strat->P.wrp();PrintLn();}
#endif

      // enter into S, L, and T
      //if ((!TEST_OPT_IDLIFT) || (pGetComp(strat->P.p) <= strat->syzComp))
        enterT(strat->P, strat);
      // posInS only depends on the leading term
      strat->enterS(strat->P, pos, strat, strat->tl);

      if (strat->P.lcm!=NULL)
#ifdef HAVE_RINGS
        pLmDelete(strat->P.lcm);
#else
        pLmFree(strat->P.lcm);
#endif
      if (strat->sl>srmax) srmax = strat->sl;
      if (pos<strat->sl)
      {
        need_retry++;
        // move all "larger" elements fromS to L
        // remove them from T
        int ii=pos+1;
        for(;ii<=strat->sl;ii++)
        {
          LObject h;
          memset(&h,0,sizeof(h));
          h.tailRing=strat->tailRing;
          h.p=strat->S[ii]; strat->S[ii]=NULL;
          strat->initEcart(&h);
          h.sev=strat->sevS[ii];
          int jj=strat->tl;
          while (jj>=0)
          {
            if (strat->T[jj].p==h.p)
            {
              strat->T[jj].p=NULL;
              if (jj<strat->tl)
              {
                memmove(&(strat->T[jj]),&(strat->T[jj+1]),
                        (strat->tl-jj)*sizeof(strat->T[jj]));
                memmove(&(strat->sevT[jj]),&(strat->sevT[jj+1]),
                        (strat->tl-jj)*sizeof(strat->sevT[jj]));
              }
              strat->tl--;
              break;
            }
            jj--;
          }
          int lpos=strat->posInL(strat->L,strat->Ll,&h,strat);
          enterL(&strat->L,&strat->Ll,&strat->Lmax,h,lpos);
          #ifdef KDEBUG
          if (TEST_OPT_DEBUG)
          {
            Print("move S[%d] -> L[%d]: ",ii,pos);
            p_wrp(h.p,currRing, strat->tailRing);
            PrintLn();
          }
          #endif
        }
        if (strat->fromQ!=NULL)
        {
          for(ii=pos+1;ii<=strat->sl;ii++) strat->fromQ[ii]=0;
        }
        strat->sl=pos;
      }
    }

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      messageSets(strat);
    }
    memset(&(strat->P), 0, sizeof(strat->P));
#endif
    //kTest_TS(strat);: i_r out of sync in kInterRedBba, but not used!
  }
#ifdef KDEBUG
  //if (TEST_OPT_DEBUG) messageSets(strat);
#endif
  /* complete reduction of the standard basis--------- */

  if((need_retry<=0) && (TEST_OPT_REDSB))
  {
    completeReduce(strat);
#ifdef HAVE_TAIL_RING
    if (strat->completeReduce_retry)
    {
      // completeReduce needed larger exponents, retry
      // to reduce with S (instead of T)
      // and in currRing (instead of strat->tailRing)
      cleanT(strat);strat->tailRing=currRing;
      int i;
      for(i=strat->sl;i>=0;i--) strat->S_2_R[i]=-1;
      completeReduce(strat);
    }
#endif
  }
  else if (TEST_OPT_PROT) PrintLn();

  /* release temp data-------------------------------- */
  exitBuchMora(strat);
  if (TEST_OPT_WEIGHTM)
  {
    pRestoreDegProcs(pFDegOld, pLDegOld);
    if (ecartWeights)
    {
      omFreeSize((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }
  //if (TEST_OPT_PROT) messageStat(srmax,lrmax,0/*hilbcount*/,strat);
  if (Q!=NULL) updateResult(strat->Shdl,Q,strat);
  ideal res=strat->Shdl;
  strat->Shdl=NULL;
  delete strat;
  if (w!=NULL) delete w;
  return res;
}
ideal kInterRed (ideal F, ideal Q)
{
#ifdef HAVE_PLURAL
  if(rIsPluralRing(currRing)) return kInterRedOld(F,Q);
#endif
  if ((pOrdSgn==-1)
  || (rField_is_numeric(currRing)))
    return kInterRedOld(F,Q);

    //return kInterRedOld(F,Q);

  BITSET save=test;
  //test|=Sy_bit(OPT_NOT_SUGAR);
  test|=Sy_bit(OPT_REDTHROUGH);
  //test&= ~Sy_bit(OPT_REDTAIL);
  //test&= ~Sy_bit(OPT_REDSB);
  //extern char * showOption() ;
  //Print("%s\n",showOption());

  int need_retry;
  int counter=3;
  ideal res, res1;
  int elems;
  ideal null=NULL;
  if ((Q==NULL) || (!TEST_OPT_REDSB))
  {
    elems=idElem(F);
    res=kInterRedBba(F,Q,need_retry);
  }
  else
  {
    ideal FF=idSimpleAdd(F,Q);
    res=kInterRedBba(FF,NULL,need_retry);
    idDelete(&FF);
    null=idInit(1,1);
    res1=kNF(null,Q,res);
    idDelete(&res);
    res=res1;
  }
  while (need_retry && (counter>0))
  {
    #ifdef KDEBUG
    if (TEST_OPT_DEBUG) { Print("retry counter %d\n",counter); }
    #endif
    res1=kInterRedBba(res,Q,need_retry);
    int new_elems=idElem(res1);
    counter -= (new_elems >= elems);
    elems = new_elems;
    idDelete(&res);
    if ((Q!=NULL) && (TEST_OPT_REDSB))
    {
      res=kNF(null,Q,res1);
      idDelete(&res1);
    }
    else
      res = res1;
  }
  if (null!=NULL) idDelete(&null);
  test=save;
  idSkipZeroes(res);
  return res;
}

// returns TRUE if mora should use buckets, false otherwise
static BOOLEAN kMoraUseBucket(kStrategy strat)
{
#ifdef MORA_USE_BUCKETS
  if (TEST_OPT_NOT_BUCKETS)
    return FALSE;
  if (strat->red == redFirst)
  {
#ifdef NO_LDEG
    if (strat->syzComp==0)
      return TRUE;
#else
    if ((strat->homog || strat->honey) && (strat->syzComp==0))
      return TRUE;
#endif
  }
  else
  {
    assume(strat->red == redEcart);
    if (strat->honey && (strat->syzComp==0))
      return TRUE;
  }
#endif
  return FALSE;
}
