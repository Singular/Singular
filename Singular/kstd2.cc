/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kstd2.cc,v 1.58 2000-11-06 14:47:34 obachman Exp $ */
/*
*  ABSTRACT -  Kernel: alg. of Buchberger
*/

// define to enable tailRings
// #define HAVE_TAIL_RING

// #define PDEBUG 2

#include "mod2.h"
#include "kutil.h"
#include "tok.h"
#include "omalloc.h"
#include "polys.h"
#include "ideals.h"
#include "febase.h"
#include "kstd1.h"
#include "khstd.h"
#include "cntrlc.h"
#include "weight.h"
#include "ipid.h"
#include "ipshell.h"
#include "intvec.h"

// redBest is broken, so, fix it and then define this
// #define HAVE_RED_BEST

// #include "timer.h"

/*2
* consider the part above syzComp:
* (assume the polynomial comes from a syz computation)
* - it is a constant term: return a copy of it
* - else: return NULL
*/
static poly kFromInput(poly p,kStrategy strat)
{
  poly q=p;

  if (pGetComp(q)>strat->syzComp) return NULL;
  while ((q!=NULL) && (pGetComp(q)<=strat->syzComp)) pIter(q);
  if (pLmIsConstantComp(q))
    return pHead(q);
  return NULL;
}

// return -1 if no divisor is found
//        number of first divisor, otherwise
static int kFindDivisibleByInT(const TSet &T, const unsigned long* sevT,
                               const int tl, const LObject* L)
{
  unsigned long not_sev = ~L->sev;
  int j = 0;
  poly p;
  ring r;
  L->GetLm(p, r);

  assume(~not_sev == p_GetShortExpVector(p, r));
  
  if (r == currRing)
  {
    while (1)
    {
      if (j > tl) return -1;
#if defined(PDEBUG) || defined(PDIV_DEBUG)
      if (p_LmShortDivisibleBy(T[j].p, sevT[j],
                               p, not_sev, r))
        return j;
#else
      if (!(sevT[j] & not_sev) &&
          p_LmDivisibleBy(T[j].p, p, r))
        return j;
#endif
      j++;
    }
  }
  else
  {
    while (1)
    {
      if (j > tl) return -1;
#if defined(PDEBUG) || defined(PDIV_DEBUG)
      if (p_LmShortDivisibleBy(T[j].t_p, sevT[j],
                               p, not_sev, r))
        return j;
#else
      if (!(sevT[j] & not_sev) &&
          p_LmDivisibleBy(T[j].t_p, p, r))
        return j;
#endif
      j++;
    }
  }
  return -1;
}

// same as above, only with set S
static int kFindDivisibleByInS(const polyset &S, unsigned long* sev, int sl, LObject* L)
{
  unsigned long not_sev = ~L->sev;
  poly p = L->GetLmCurrRing();
  int j = 0;

  assume(~not_sev == p_GetShortExpVector(p, currRing));
  while (1)
  {
    if (j > sl) return -1;
#if defined(PDEBUG) || defined(PDIV_DEBUG)
    if (p_LmShortDivisibleBy(S[j], sev[j],
                             p, not_sev, currRing))
        return j;
#else
    if ( !(sev[j] & not_sev) &&
         p_LmDivisibleBy(S[j], p, currRing))
      return j;
#endif
    j++;
  }
}
  
/*2
*  reduction procedure for the homogeneous case
*  and the case of a degree-ordering
*/
static int redHomog (LObject* h,kStrategy strat)
{
  if (strat->tl<0) return 1;
#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    h->wrp();
    PrintS(" ");
  }
#endif
  int j;
  while (1)
  {
    // find a poly with which we can reduce
    h->SetShortExpVector();
    j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h);
    if (j < 0) return 1;
    
    // now we found one which is divisible -- reduce it
    ksReducePoly(h, &(strat->T[j]), strat->kNoether);

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("\nto ");
      h->wrp();
      PrintLn();
    }
#endif
    if (h->GetLmTailRing() == NULL)
    {
      if (h->lcm!=NULL) pLmFree((*h).lcm);
#ifdef KDEBUG
      (*h).lcm=NULL;
#endif
      return 0;
    }
  }
}


/*2
*  reduction procedure for the inhomogeneous case
*  and not a degree-ordering
*/
static int redLazy (LObject* h,kStrategy strat)
{
  if (strat->tl<0) return 1;
  int at,d,i;
  int j = 0;
  int pass = 0;
  int reddeg = h->pFDeg();

  h->SetShortExpVector();
  while (1)
  {
    j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h);
    if (j < 0) return 1;

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("red:");
      h->wrp();
      PrintS(" with ");
      strat->T[j].wrp();
    }
#endif

    ksReducePoly(h, &(strat->T[j]), strat->kNoether);

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("\nto ");
      h->wrp();
      PrintLn();
    }
#endif
    
    if (h->GetLmTailRing() == NULL)
    {
      if (h->lcm!=NULL) pLmFree((*h).lcm);
#ifdef KDEBUG
      (*h).lcm=NULL;
#endif
      return 0;
    }
    h->SetShortExpVector();
    d = h->pFDeg();
    /*- try to reduce the s-polynomial -*/
    pass++;
    if ((strat->Ll >= 0) && ((d > reddeg) || (pass > strat->LazyPass)))
    {
      h->SetLmCurrRing();
      at = posInL11(strat->L,strat->Ll,*h,strat);
      if (at <= strat->Ll)
      {
        if (kFindDivisibleByInS(strat->S, strat->sevS, strat->sl, h) < 0)
          return 1;
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) Print(" ->L[%d]\n",at);
#endif
        h->CanonicalizeP();
        enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
        h->Clear();
        return -1;
      }
    }
    else if ((TEST_OPT_PROT) && (strat->Ll < 0) && (d != reddeg))
    {
      Print(".%d",d);mflush();
      reddeg = d;
    }
  }
}

/*2
*  reduction procedure for the sugar-strategy (honey)
* reduces h with elements from T choosing first possible
* element in T with respect to the given ecart
*/
static int redHoney (LObject* h, kStrategy strat)
{
  if (strat->tl<0) return 1;

  poly pi, h_p;
  int i,j,at,reddeg,d,pass,ei, ii, h_d;
  unsigned long not_sev;

  pass = j = 0;
  d = reddeg = h->pFDeg() + h->ecart;
  h->SetShortExpVector();
  h_p = h->GetLmTailRing();
  not_sev = ~ h->sev;
  loop
  {
    j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h);
    if (j < 0) return 1;

    pi = strat->T[j].p;
    ei = strat->T[j].ecart;
    ii = j;
    /*
     * the polynomial to reduce with (up to the moment) is;
     * pi with ecart ei
     */
    i = j;
    loop
    {
      /*- takes the first possible with respect to ecart -*/
      i++;
      if (i > strat->tl)
        break;
      if ((!TEST_OPT_REDBEST) && (ei <= (*h).ecart))
        break;
      if ((strat->T[i].ecart < ei) && 
          p_LmShortDivisibleBy(strat->T[i].GetLmTailRing(), strat->sevT[i], 
                               h_p, not_sev, strat->tailRing))
      {
        /*
         * the polynomial to reduce with is now;
         */
        pi = strat->T[i].p;
        ei = strat->T[i].ecart;
        ii = i;
      }
    }

    /*
     * end of search: have to reduce with pi
     */
    if ((pass!=0) && (ei > (*h).ecart))
    {
      h->SetLmCurrRing();
      /*
       * It is not possible to reduce h with smaller ecart;
       * if possible h goes to the lazy-set L,i.e
       * if its position in L would be not the last one
       */
      if (strat->Ll >= 0) /* L is not empty */
      {
        at = strat->posInL(strat->L,strat->Ll,*h,strat);

        if(at <= strat->Ll)
          /*- h will not become the next element to reduce -*/
        {
          h->CanonicalizeP();
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
#ifdef KDEBUG
          if (TEST_OPT_DEBUG) Print(" ecart too big: -> L%d\n",at);
#endif
          h->Clear();
          return -1;
        }
      }
      if (TEST_OPT_MOREPAIRS)
      {
        /*put the polynomial also in the pair set*/
        strat->fromT = TRUE;
        h->GetP();
        if (!TEST_OPT_INTSTRATEGY) h->pNorm();
        enterpairs((*h).p,strat->sl,(*h).ecart,0,strat);
      }
    }
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("red:");
      h->wrp();
      PrintS(" with ");
      strat->T[ii].wrp();
    }
#endif
    if (strat->fromT)
    {
      strat->fromT=FALSE;
      h->p = p_Copy(h->p, currRing, strat->tailRing);
    }

    ksReducePoly(h, &(strat->T[ii]), strat->kNoether);

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("\nto ");
      h->wrp();
      PrintLn();
    }
#endif

    h_p = h->GetLmTailRing();
    if (h_p == NULL)
    {
      if (h->lcm!=NULL) pLmFree((*h).lcm);
#ifdef KDEBUG
      (*h).lcm=NULL;
#endif
      return 0;
    }
    h->SetShortExpVector();
    not_sev = ~ h->sev;
    h_d = h->pFDeg();
    /* compute the ecart */
    if (ei <= (*h).ecart)
      (*h).ecart = d-h_d;
    else
      (*h).ecart = d-h_d+ei-(*h).ecart;
    /*
     * try to reduce the s-polynomial h
     *test first whether h should go to the lazyset L
     *-if the degree jumps
     *-if the number of pre-defined reductions jumps
     */
    pass++;
    d = h_d +(*h).ecart;
    if ((strat->Ll >= 0) && ((d > reddeg) || (pass > strat->LazyPass)))
    {
      h->SetLmCurrRing();
      at = strat->posInL(strat->L,strat->Ll,*h,strat);
      if (at <= strat->Ll)
      {
        if (kFindDivisibleByInS(strat->S, strat->sevS, strat->sl, h) < 0)
          return 1;
        h->CanonicalizeP();
        enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
          Print(" degree jumped: -> L%d\n",at);
#endif
        h->Clear();
        return -1;
      }
    }
    else if (TEST_OPT_PROT && (strat->Ll < 0) && (d > reddeg))
    {
      reddeg = d;
      Print(".%d",d); mflush();
    }
  }
}


#ifdef HAVE_RED_BEST
/*2
*  reduction procedure for tests only
*  reduces with elements from T and chooses the best possible
*/
static int redBest (LObject*  h,kStrategy strat)
{
  if (strat->tl<0) return 1;
  int j,jbest,at,reddeg,d,pass;
  poly     p,ph;
  pass = j = 0;

  if (strat->honey)
    reddeg = pFDeg((*h).p)+(*h).ecart;
  else
    reddeg = pFDeg((*h).p);
  loop
  {
    if (pDivisibleBy(strat->T[j].p,(*h).p))
    {
      //if (strat->interpt) test_int_std(strat->kIdeal);
      /* compute the s-polynomial */
      if (!TEST_OPT_INTSTRATEGY) pNorm((*h).p);
      p = ksCreateShortSpoly(strat->T[j].p,(*h).p);
      /* computes only the first monomial of the spoly  */
      if (p)
      {
        jbest = j;
        /* looking for the best possible reduction */
        if ((strat->syzComp==0) || (pMinComp(p) <= strat->syzComp))
        {
          loop
          {
            j++;
            if (j > strat->tl)
              break;
            if (pDivisibleBy(strat->T[j].p,(*h).p))
            {
              ph = ksCreateShortSpoly(strat->T[j].p,(*h).p);
              if (ph==NULL)
              {
                pLmFree(p);
                pDelete(&((*h).p));
                if (h->lcm!=NULL) pLmFree((*h).lcm);
#ifdef KDEBUG
                (*h).lcm=NULL;
#endif
                return 0;
              }
              else if (pLmCmp(ph,p) == -1)
              {
                pLmFree(p);
                p = ph;
                jbest = j;
              }
              else
              {
                pLmFree(ph);
              }
            }
          }
        }
        pLmFree(p);
        (*h).p = ksOldSpolyRed(strat->T[jbest].p,(*h).p,strat->kNoether);
      }
      else
      {
        if (h->lcm!=NULL) pLmFree((*h).lcm);
#ifdef KDEBUG
        (*h).lcm=NULL;
#endif
        h->Clear();
        return 0;
      }
      if (strat->honey && pLexOrder)
        strat->initEcart(h);
      if (strat->honey || pLexOrder)
      {
        pass++;
        d = pFDeg((*h).p);
        if (strat->honey)
          d += (*h).ecart;
        if ((strat->Ll >= 0) && ((pass > strat->LazyPass) || (d > reddeg)))
        {
          at = strat->posInL(strat->L,strat->Ll,*h,strat);
          if (at <= strat->Ll)
          {
            enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
            h->Clear();
            return -1;
          }
        }
        else if (TEST_OPT_PROT && (strat->Ll < 0) && (d != reddeg))
        {
          reddeg = d;
          Print("%d.");
          mflush();
        }
      }
      j = 0;
    }
    else
    {
      if (j >= strat->tl) return 1;
      j++;
    }
  }
}
#endif
/*2
*  reduction procedure for the normal form
*/

static poly redNF (poly h,kStrategy strat)
{
  int j = 0;
  int z = 3;
  unsigned long not_sev;

  if (0 > strat->sl)
  {
    return h;
  }
  not_sev = ~ pGetShortExpVector(h);
  loop
  {
    if (pLmShortDivisibleBy(strat->S[j], strat->sevS[j], h, not_sev))
    {
      //if (strat->interpt) test_int_std(strat->kIdeal);
      /*- compute the s-polynomial -*/
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS("red:");
        wrp(h);
        PrintS(" with ");
        wrp(strat->S[j]);
      }
#endif
      h = ksOldSpolyRed(strat->S[j],h,strat->kNoether);
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS("\nto:");
        wrp(h);
        PrintLn();
      }
#endif
      if (h == NULL) return NULL;
      z++;
      if (z>=10)
      {
        z=0;
        pNormalize(h);
      }
      /*- try to reduce the s-polynomial -*/
      j = 0;
      not_sev = ~ pGetShortExpVector(h);
    }
    else
    {
      if (j >= strat->sl) return h;
      j++;
    }
  }
}

void initBba(ideal F,kStrategy strat)
{
  int i;
  idhdl h;
 /* setting global variables ------------------- */
  strat->enterS = enterSBba;
#ifdef HAVE_RED_BEST
  if ((TEST_OPT_REDBEST) && (!strat->honey))
    strat->red = redBest;
  else 
#endif
    if (strat->honey)
    strat->red = redHoney;
  else if (pLexOrder && !strat->homog)
    strat->red = redLazy;
  else
    strat->red = redHomog;
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
    h=ggetid("ecart");
    if ((h!=NULL) && (IDTYP(h)==INTVEC_CMD))
    {
      ecartWeights=iv2array(IDINTVEC(h));
    }
    else
    {
      ecartWeights=(short *)omAlloc((pVariables+1)*sizeof(short));
      /*uses automatic computation of the ecartWeights to set them*/
      kEcartWeights(F->m,IDELEMS(F)-1,ecartWeights);
    }
    pFDeg=totaldegreeWecart;
    pLDeg=maxdegreeWecart;
    for(i=1; i<=pVariables; i++)
      Print(" %d",ecartWeights[i]);
    PrintLn();
    mflush();
  }
}

ideal bba (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat)
{
  om_Opts.MinTrack = 5;
  int   srmax,lrmax, red_result;
  int   olddeg,reduc;
  int hilbeledeg=1,hilbcount=0,minimcnt=0;

  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  initBuchMoraPos(strat);
  initHilbCrit(F,Q,&hilb,strat);
  initBba(F,strat);
  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  /*Shdl=*/initBuchMora(F, Q,strat);
  if (strat->minim>0) strat->M=idInit(IDELEMS(F),F->rank);
  srmax = strat->sl;
  reduc = olddeg = lrmax = 0;

  if (!TEST_OPT_NOT_BUCKETS && 
      (strat->red == redHomog || strat->red == redHoney))
    strat->use_buckets = 1;

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
    if (TEST_OPT_DEGBOUND
        && ((strat->honey && (strat->L[strat->Ll].ecart+pFDeg(strat->L[strat->Ll].p)>Kstd1_deg))
            || ((!strat->honey) && (pFDeg(strat->L[strat->Ll].p)>Kstd1_deg))))
    {
      /*
       *stops computation if
       * 24 IN test and the degree +ecart of L[strat->Ll] is bigger then
       *a predefined number Kstd1_deg
       */
      while (strat->Ll >= 0) deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      break;
    }
    /* picks the last element from the lazyset L */
    strat->P = strat->L[strat->Ll];
    strat->Ll--;

    if (pNext(strat->P.p) == strat->tail)
    {
      poly m1 = NULL, m2 = NULL;
      /* check that spoly creation is ok */
      while (strat->tailRing != currRing && 
             !kCheckSpolyCreation(strat, m1, m2))
      {
        assume(m1 == NULL && m2 == NULL);
        // if not, change to a ring where exponents are at least
        // twice as large as current ones -- should be enough to
        // get through ksCreateSpoly correctly
        kStratChangeTailRing(strat);
      }
      /* deletes the short spoly and computes */
      pLmFree(strat->P.p);
      strat->P.p=NULL;
      /* create the real one */
      ksCreateSpoly(&(strat->P), strat->kNoether, strat->use_buckets, 
                    strat->tailRing, m1, m2);
    }

    if  ((strat->P.p1==NULL) && (strat->minim>0)) 
      strat->P.p2=pCopy(strat->P.p);

    if (TEST_OPT_PROT)
      message((strat->honey ? strat->P.ecart : 0) + strat->P.pFDeg(),
              &olddeg,&reduc,strat);

    /* reduction of the element choosen from L */
    red_result = strat->red(&strat->P,strat);

    // reduction to non-zero new poly
    if (red_result == 1)
    {
      /* statistic */
      if (TEST_OPT_PROT) PrintS("s");
      strat->P.GetP(strat->lmBin);
      /* enter P.p into s and L */
      {
        int pos=posInS(strat->S,strat->sl,strat->P.p);
        if (TEST_OPT_INTSTRATEGY)
        {
          strat->P.pCleardenom();
          if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
          {
            strat->P.p = redtailBba(&(strat->P),pos-1,strat);
            if (strat->redTailChange)  strat->P.pLength = 0;
            strat->P.pCleardenom();
          }
        }
        else
        {
          strat->P.pNorm();
          if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
          {
            strat->P.p = redtailBba(&(strat->P),pos-1,strat);
            if (strat->redTailChange)  strat->P.pLength = 0;
          }
        }
#ifdef KDEBUG
        if (TEST_OPT_DEBUG){PrintS("new s:");strat->P.wrp();PrintLn();}
#endif
        if ((strat->P.p1==NULL) && (strat->minim>0))
        {
          if (strat->minim==1)
          {
            strat->M->m[minimcnt]=pCopy(strat->P.p);
            pDelete(&strat->P.p2);
          }
          else
          {
            strat->M->m[minimcnt]=strat->P.p2;
            strat->P.p2=NULL;
          }
          minimcnt++;
        }

        // put stuff into T-Set
        enterT(strat->P, strat);
        enterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat, strat->tl);
        // posInS only depends on the leading term
        strat->enterS(strat->P, pos, strat, strat->tl);
        if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);
      }
      if (strat->P.lcm!=NULL) pLmFree(strat->P.lcm);
      if (strat->sl>srmax) srmax = strat->sl;
    }
#ifdef KDEBUG
    memset(&(strat->P), 0, sizeof(strat->P));
#endif
    kTest_TS(strat);
  }
#ifdef KDEBUG
  if (TEST_OPT_DEBUG) messageSets(strat);
#endif
  /* complete reduction of the standard basis--------- */
  if (TEST_OPT_REDSB) completeReduce(strat);
  /* release temp data-------------------------------- */
  exitBuchMora(strat);
  if (TEST_OPT_WEIGHTM)
  {
    pFDeg=pFDegOld;
    pLDeg=pLDegOld;
    if (ecartWeights)
    {
      omFreeSize((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }
  if (TEST_OPT_PROT) messageStat(srmax,lrmax,hilbcount,strat);
  if (Q!=NULL) updateResult(strat->Shdl,Q,strat);
  return (strat->Shdl);
}

poly kNF2 (ideal F,ideal Q,poly q,kStrategy strat, int lazyReduce)
{
  poly   p;
  int   i;

  if ((idIs0(F))&&(Q==NULL))
    return pCopy(q); /*F=0*/
  strat->ak = idRankFreeModule(F);
  /*- creating temp data structures------------------- -*/
  BITSET save_test=test;
  test|=Sy_bit(OPT_REDTAIL);
  initBuchMoraCrit(strat);
  strat->initEcart = initEcartBBA;
  strat->enterS = enterSBba;
  /*- set S -*/
  strat->sl = -1;
  /*- init local data struct.---------------------------------------- -*/
  /*Shdl=*/initS(F,Q,strat);
  /*- compute------------------------------------------------------- -*/
  if ((TEST_OPT_INTSTRATEGY)&&(lazyReduce==0))
  {
    for (i=strat->sl;i>=0;i--)
      pNorm(strat->S[i]);
  }
  kTest(strat);
  if (TEST_OPT_PROT) { PrintS("r"); mflush(); }
  p = redNF(pCopy(q),strat);
  if ((p!=NULL)&&(lazyReduce==0))
  {
    if (TEST_OPT_PROT) { PrintS("t"); mflush(); }
    p = redtailBba(p,strat->sl,strat);
  }
  /*- release temp data------------------------------- -*/
  if (strat->ecartS != NULL)
    omFreeSize((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  if (strat->sevS != NULL)
    omFreeSize((ADDRESS)strat->sevS,IDELEMS(strat->Shdl)*sizeof(unsigned long));
  omfree(strat->T);
  omfree(strat->sevT);
  omfree(strat->R);
  omfree(strat->S_2_R);
  omfree(strat->L);
  omfree(strat->B);
  idDelete(&strat->Shdl);
  test=save_test;
  if (TEST_OPT_PROT) PrintLn();
  return p;
}

ideal kNF2 (ideal F,ideal Q,ideal q,kStrategy strat, int lazyReduce)
{
  poly   p;
  int   i;
  ideal res;

  if (idIs0(q))
    return idInit(1,q->rank);
  if ((idIs0(F))&&(Q==NULL))
    return idCopy(q); /*F=0*/
  strat->ak = idRankFreeModule(F);
  /*- creating temp data structures------------------- -*/
  BITSET save_test=test;
  test|=Sy_bit(OPT_REDTAIL);
  initBuchMoraCrit(strat);
  strat->initEcart = initEcartBBA;
  strat->enterS = enterSBba;
  /*- set S -*/
  strat->sl = -1;
  /*- init local data struct.---------------------------------------- -*/
  /*Shdl=*/initS(F,Q,strat);
  /*- compute------------------------------------------------------- -*/
  res=idInit(IDELEMS(q),q->rank);
  if ((TEST_OPT_INTSTRATEGY)&&(lazyReduce==0))
  {
    for (i=strat->sl;i>=0;i--)
      pNorm(strat->S[i]);
  }
  for (i=IDELEMS(q)-1; i>=0; i--)
  {
    if (q->m[i]!=NULL)
    {
      if (TEST_OPT_PROT) { PrintS("r");mflush(); }
      p = redNF(pCopy(q->m[i]),strat);
      if ((p!=NULL)&&(lazyReduce==0))
      {
        if (TEST_OPT_PROT) { PrintS("t"); mflush(); }
        p = redtailBba(p,strat->sl,strat);
      }
      res->m[i]=p;
    }
    //else
    //  res->m[i]=NULL;
  }
  /*- release temp data------------------------------- -*/
  if (strat->ecartS != NULL)
    omFreeSize((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  if (strat->sevS != NULL)
    omFreeSize((ADDRESS)strat->sevS,IDELEMS(strat->Shdl)*sizeof(unsigned long));
  omfree(strat->T);
  omfree(strat->sevT);
  omfree(strat->R);
  omfree(strat->S_2_R);
  omfree(strat->L);
  omfree(strat->B);
  idDelete(&strat->Shdl);
  test=save_test;
  if (TEST_OPT_PROT) PrintLn();
  return res;
}

static ideal bbared (ideal F, ideal Q,kStrategy strat)
{

  /* complete reduction of the standard basis--------- */
  completeReduce(strat);
  /* release temp data-------------------------------- */
  exitBuchMora(strat);
  if (TEST_OPT_WEIGHTM)
  {
    pFDeg=pFDegOld;
    pLDeg=pLDegOld;
    if (ecartWeights)
    {
      omFreeSize((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }
  if (Q!=NULL) updateResult(strat->Shdl,Q,strat);
  return (strat->Shdl);
}

ideal stdred(ideal F, ideal Q, tHomog h,intvec ** w)
{
  ideal r;
  BOOLEAN b=pLexOrder,toReset=FALSE;
  BOOLEAN delete_w=(w==NULL);
  kStrategy strat=new skStrategy;

  if (rField_has_simple_inverse())
    strat->LazyPass=20;
  else
   strat->LazyPass=2;
  strat->LazyDegree = 1;
  strat->ak = idRankFreeModule(F);
  if ((h==testHomog))
  {
    if (strat->ak==0)
    {
      h = (tHomog)idHomIdeal(F,Q);
      w=NULL;
    }
    else
      h = (tHomog)idHomModule(F,Q,w);
  }
  if (h==isHomog)
  {
    if ((w!=NULL) && (*w!=NULL))
    {
      kModW = *w;
      strat->kModW = *w;
      pOldFDeg = pFDeg;
      pFDeg = kModDeg;
      toReset = TRUE;
    }
    pLexOrder = TRUE;
    strat->LazyPass*=2;
  }
  strat->homog=h;
  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  initBuchMoraPos(strat);
  if (pOrdSgn==1)
    initBba(F,strat);
  else
    initMora(F,strat);
  initBuchMora(F, Q,strat);
  //initS(F,Q,strat);
// Ende der Initalisierung
  r=bbared(F,Q,strat);
#ifdef KDEBUG
  int i;
  for (i=0; i<IDELEMS(r); i++) pTest(r->m[i]);
#endif
// Ende: aufraeumen
  if (toReset)
  {
    kModW = NULL;
    pFDeg = pOldFDeg;
  }
  pLexOrder = b;
  delete(strat);
  if ((delete_w)&&(w!=NULL)&&(*w!=NULL)) delete *w;
  idSkipZeroes(r);
  return r;
}
