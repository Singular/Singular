/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kstd2.cc,v 1.36 1999-10-18 11:19:27 obachman Exp $ */
/*
*  ABSTRACT -  Kernel: alg. of Buchberger
*/

#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "polys.h"
#include "ideals.h"
#include "febase.h"
#include "kutil.h"
#include "kstd1.h"
#include "khstd.h"
#include "cntrlc.h"
#include "weight.h"
#include "ipid.h"
#include "ipshell.h"
#include "intvec.h"

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
  if (pIsConstantComp(q))
    return pHead(q);
  return NULL;
}

/*2
*  reduction procedure for the syz
*  and TEST_OPT_MINRES: special minimizing during computations
*  assumes homogeneous case and degree-ordering
*/
#if 0
static int redSyz (LObject* h,kStrategy strat)
{
  int j = 0,i=0,pos;
  BOOLEAN exchanged=pDivisibleBy((*h).p2,(*h).p1);
  poly p,q;
  unsigned long not_sev;

  if (exchanged)
  {
    q = kFromInput((*h).p1,strat);
    if (q==NULL)
    {
      exchanged = FALSE;
    }
    else
    {
      while (i<=strat->Ll)
      {
        if ((strat->L[i].p1==strat->P.p1) || (strat->L[i].p2==strat->P.p1))
        {
          deleteInL(strat->L,&strat->Ll,i,strat);
        }
        else
          i++;
      }
      i = 0;
    }
  }
  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    wrp(h->p);
  }
  loop
  {
    i = 0;
    j = strat->sl + 1;
    not_sev = ~ PGetShortExpVector(h->p);
    while (i <= strat->tl)
    {
      if (pShortDivisibleBy(strat->T[i], strat->T[i].sev, h->p, not_sev))
      {
        if ((!exchanged) && (pEqual((*h).p,strat->T[i].p)))
        {
          j = 0;
          while (j<=strat->sl)
          {
            if (strat->S[j] == strat->T[i].p) break;
          }
          if (j <= strat->sl) break;
        }
        else
        {
          break;
        }
      }
      i++;
    }

    if (i > strat->tl)
    {
      // nothing to reduce with
      if (exchanged)
      {
        assume(q!= NULL);
        if (pGetComp((*h).p) > strat->syzComp)
        {
          pDelete(&((*h).p));
          pDelete(&q);
          return;
        }
        else
        {
          if (!TEST_OPT_INTSTRATEGY)
          {
            pos = posInS(strat->S,strat->sl,(*h).p);
            pNorm((*h).p);
            (*h).p = redtailSyz((*h).p,pos-1,strat);
          }
          p = (*h).p;
          while ((pNext(p)!=NULL) && (pGetComp(pNext(p))<=strat->syzComp))
            pIter(p);
          pDelete(&pNext(p));
          pNext(p) = q;
          q = NULL;
        }
      }
      else if (!TEST_OPT_INTSTRATEGY)
      {
        pos = posInS(strat->S,strat->sl,(*h).p);
        pNorm((*h).p);
        (*h).p = redtailSyz((*h).p,pos-1,strat);
      }
      if (q != NULL) pDelete(&q);
      enterTBba((*h),strat->tl+1,strat);
      return;
    }

    // found one to reduce
    if (j <= strat->sl)
    {
      // might have found a better one ?
      q = kFromInput(strat->S[j],strat);
      if (q!=NULL)
      {
        exchanged = TRUE;
        p = strat->S[j];
        if (!TEST_OPT_INTSTRATEGY)
          pNorm((*h).p);
        else
        {
          //pContent((*h).p);
          pCleardenom((*h).p);// also does a pContent
        }
        strat->S[j] = (*h).p;
        (*h).p = p;
         strat->T[i].p = strat->S[j];
        for (i=0;i<=strat->Ll;i++)
        {
          if (strat->L[i].p1==p) strat->L[i].p1=strat->S[j];
          if (strat->L[i].p2==p) strat->L[i].p2=strat->S[j];
        }
      }
    }

    if (TEST_OPT_DEBUG)
    {
      PrintS(" with ");
      wrp(strat->T[i].p);
    }
    kbReducePoly(h, &(strat->T[i]), strat->kNoether);
    if (TEST_OPT_DEBUG)
    {
      PrintS("\nto "); wrp((*h).p);PrintLn();
    }
    if ((*h).p == NULL)
    {
      if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
      (*h).lcm=NULL;
#endif
      if (q != NULL) pDelete(&q);
      return;
    }
  }
}

#else

static int redSyz (LObject* h,kStrategy strat)
{
  int j = 0,i=0,pos;
  BOOLEAN exchanged=pDivisibleBy((*h).p2,(*h).p1);
  poly p,q;
  unsigned long not_sev;

  if (exchanged)
  {
    q = kFromInput((*h).p1,strat);
    if (q==NULL)
    {
      exchanged = FALSE;
    }
    else
    {
      while (i<=strat->Ll)
      {
        if ((strat->L[i].p1==strat->P.p1) || (strat->L[i].p2==strat->P.p1))
        {
          deleteInL(strat->L,&strat->Ll,i,strat);
        }
        else
          i++;
      }
      i = 0;
    }
  }
  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    wrp(h->p);
  }
  h->sev = pGetShortExpVector(h->p);
  not_sev = ~ h->sev;
  loop
  {
    if (pShortDivisibleBy(strat->S[j], strat->sevS[j], h->p, not_sev))
    {
      if ((!exchanged) && (pEqual((*h).p,strat->S[j])))
      {
        q = kFromInput(strat->S[j],strat);
        if (q!=NULL)
        {
          exchanged = TRUE;
          p = strat->S[j];
          if (!TEST_OPT_INTSTRATEGY)
            pNorm((*h).p);
          else
          {
            //pContent((*h).p);
            pCleardenom((*h).p);// also does a pContent
          }
          strat->S[j] = (*h).p;
          strat->sevS[j] = ~ not_sev;
          (*h).p = p;
          h->sev = strat->sevS[j];
          while ((i<=strat->tl) && (strat->T[i].p!=p)) i++;
          if (i<=strat->tl) strat->T[i].SetP(strat->S[j]);
          for (i=0;i<=strat->Ll;i++)
          {
            if (strat->L[i].p1==p) strat->L[i].p1=strat->S[j];
            if (strat->L[i].p2==p) strat->L[i].p2=strat->S[j];
          }
        }
      }
      //if (strat->interpt) test_int_std(strat->kIdeal);
      /*- compute the s-polynomial -*/
      if (TEST_OPT_DEBUG)
      {
        PrintS(" with ");
        wrp(strat->S[j]);
      }
      (*h).p = ksOldSpolyRed(strat->S[j],(*h).p,strat->kNoether);
      if (TEST_OPT_DEBUG)
      {
        PrintS("\nto "); wrp((*h).p);PrintLn();
      }
      if ((*h).p == NULL)
      {
        if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
        (*h).lcm=NULL;
#endif
        return 0;
      }
/*- try to reduce the s-polynomial -*/
      j = 0;
      h->sev = pGetShortExpVector(h->p);
      not_sev = ~ h->sev;
    }
    else
    {
      if (j >= strat->sl)
      {
        if (exchanged)
        {
          if (pGetComp((*h).p) > strat->syzComp)
          {
            pDelete(&((*h).p));
            return 0;
          }
          else
          {
            if (!TEST_OPT_INTSTRATEGY)
            {
              pos = posInS(strat->S,strat->sl,(*h).p);
              pNorm((*h).p);
              (*h).p = redtailSyz((*h).p,pos-1,strat);
            }
            p = (*h).p;
            while ((pNext(p)!=NULL) && (pGetComp(pNext(p))<=strat->syzComp))
               pIter(p);
            pDelete(&pNext(p));
            pNext(p) = q;
          }
        }
        else if (!TEST_OPT_INTSTRATEGY)
        {
          pos = posInS(strat->S,strat->sl,(*h).p);
          pNorm((*h).p);
          (*h).p = redtailSyz((*h).p,pos-1,strat);
        }
        enterTBba((*h),strat->tl+1,strat);
        return 1;
      }
      j++;
    }
  }
}

#endif

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
    wrp(h->p);
    PrintS(" ");
  }
#endif
  int j;
  unsigned long not_sev;
  while (1)
  {
    j = 0;
    h->sev = pGetShortExpVector(h->p);
    not_sev = ~ h->sev;
    while ( j <= strat->tl)
    {
      if (pShortDivisibleBy(strat->T[j].p, strat->T[j].sev, 
                            h->p, not_sev)) break;
      j++;
    }

    if (j > strat->tl) return 1;

    // now we found one which is divisible
    ksReducePoly(h, &(strat->T[j]), strat->kNoether);
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("\nto ");
      wrp(h->p);
      PrintLn();
    }
#endif
    if (h->p == NULL)
    {
      if (h->lcm!=NULL) pFree1((*h).lcm);
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
  int reddeg = pFDeg((*h).p);
  int not_sev;

  h->sev = pGetShortExpVector(h->p);
  not_sev = ~ h->sev;
  while (1)
  {
    j = 0;
    while (j <= strat->tl)
    {
      if (pShortDivisibleBy(strat->T[j].p,strat->T[j].sev,
                            h->p, not_sev))
          break;
      j++;
    }

    if (j>strat->tl) return 1;

    if (TEST_OPT_DEBUG)
    {
      PrintS("red:");
      wrp(h->p);
      PrintS(" with ");
      wrp(strat->T[j].p);
    }

    ksReducePoly(h, &(strat->T[j]), strat->kNoether);

    if (TEST_OPT_DEBUG)
    {
      PrintS("\nto ");
      wrp(h->p);
      PrintLn();
    }

    if ((*h).p == NULL)
    {
      if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
      (*h).lcm=NULL;
#endif
      return 0;
    }
    h->sev = pGetShortExpVector(h->p);
    not_sev = ~ h->sev;
    /*- try to reduce the s-polynomial -*/
    pass++;
    d = pFDeg((*h).p);
    if ((strat->Ll >= 0) && ((d > reddeg) || (pass > strat->LazyPass)))
    {
      at = posInL11(strat->L,strat->Ll,*h,strat);
      if (at <= strat->Ll)
      {
        i=strat->sl+1;
        do
        {
          i--;
          if (i<0) return 1;
        }
        while (!pShortDivisibleBy(strat->S[i], strat->sevS[i], 
                                  h->p, not_sev));
        if (TEST_OPT_DEBUG) Print(" ->L[%d]\n",at);
        enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
        (*h).p = NULL;
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
static int redHoney (LObject*  h,kStrategy strat)
{
  if (strat->tl<0) return 1;

  poly pi;
  int i,j,at,reddeg,d,pass,ei, ii;
  unsigned long not_sev;

  pass = j = 0;
  d = reddeg = pFDeg((*h).p)+(*h).ecart;
  h->sev = pGetShortExpVector(h->p);
  not_sev = ~ h->sev;
  loop
  {
    j = 0;
    while (j<= strat->tl)
    {
      if (pShortDivisibleBy(strat->T[j].p, strat->T[j].sev, 
                            h->p, not_sev)) break;
      j++;
    }

    if (j > strat->tl)
    {
      h->sev = ~ not_sev;
      return 1;
    }

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
      if ((!BTEST1(20)) && (ei <= (*h).ecart))
        break;
      if ((strat->T[i].ecart < ei) && 
          pShortDivisibleBy(strat->T[i].p, strat->T[i].sev,
                            h->p, not_sev))
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
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
          if (TEST_OPT_DEBUG) Print(" ecart too big: -> L%d\n",at);
          (*h).p = NULL;
          return -1;
        }
      }
      if (TEST_OPT_MOREPAIRS)
      {
        /*put the polynomial also in the pair set*/
        strat->fromT = TRUE;
        if (!TEST_OPT_INTSTRATEGY) pNorm((*h).p);
        enterpairs((*h).p,strat->sl,(*h).ecart,0,strat);
      }
    }
    if (TEST_OPT_DEBUG)
    {
      PrintS("red:");
      wrp(h->p);
      PrintS(" with ");
      wrp(pi);
    }
    if (strat->fromT)
    {
      strat->fromT=FALSE;
      h->p = pCopy(h->p);
    }
    ksReducePoly(h, &(strat->T[ii]), strat->kNoether);
    if (TEST_OPT_DEBUG)
    {
      PrintS("\nto ");
      wrp(h->p);
      PrintLn();
    }
    if ((*h).p == NULL)
    {
      if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
      (*h).lcm=NULL;
#endif
      return 0;
    }
    h->sev = pGetShortExpVector(h->p);
    not_sev = ~ h->sev;
    /* compute the ecart */
    if (ei <= (*h).ecart)
      (*h).ecart = d-pFDeg((*h).p);
    else
      (*h).ecart = d-pFDeg((*h).p)+ei-(*h).ecart;
    /*
     * try to reduce the s-polynomial h
     *test first whether h should go to the lazyset L
     *-if the degree jumps
     *-if the number of pre-defined reductions jumps
     */
    pass++;
    d = pFDeg((*h).p)+(*h).ecart;
    if ((strat->Ll >= 0) && ((d > reddeg) || (pass > strat->LazyPass)))
    {
      at = strat->posInL(strat->L,strat->Ll,*h,strat);
      if (at <= strat->Ll)
      {
        /*test if h is already standardbasis element*/
        i=strat->sl+1;
        do
        {
          i--;
          if (i<0) return 1;
        } while (!pShortDivisibleBy(strat->S[i], strat->sevS[i], 
                                    h->p, not_sev));
        enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
        if (TEST_OPT_DEBUG)
          Print(" degree jumped: -> L%d\n",at);
        (*h).p = NULL;
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
                pFree1(p);
                pDelete(&((*h).p));
                if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
                (*h).lcm=NULL;
#endif
                return 0;
              }
              else if (pComp0(ph,p) == -1)
              {
                pFree1(p);
                p = ph;
                jbest = j;
              }
              else
              {
                pFree1(ph);
              }
            }
          }
        }
        pFree1(p);
        (*h).p = ksOldSpolyRed(strat->T[jbest].p,(*h).p,strat->kNoether);
      }
      else
      {
        if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
        (*h).lcm=NULL;
#endif
        (*h).p = NULL;
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
            (*h).p = NULL;
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
    if (pShortDivisibleBy(strat->S[j], strat->sevS[j], h, not_sev))
    {
      //if (strat->interpt) test_int_std(strat->kIdeal);
      /*- compute the s-polynomial -*/
      if (TEST_OPT_DEBUG)
      {
        PrintS("red:");
        wrp(h);
        PrintS(" with ");
        wrp(strat->S[j]);
      }
      h = ksOldSpolyRed(strat->S[j],h,strat->kNoether);
      if (TEST_OPT_DEBUG)
      {
        PrintS("\nto:");
        wrp(h);
        PrintLn();
      }
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
  if ((BTEST1(20)) && (!strat->honey))
    strat->red = redBest;
  else if (strat->honey)
    strat->red = redHoney;
  else if (pLexOrder && !strat->homog)
    strat->red = redLazy;
  else
    strat->red = redHomog;
  if (TEST_OPT_MINRES && strat->homog && (strat->syzComp >0))
  {
    strat->red = redSyz;
    strat->posInT = posInT0;
  }

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
      ecartWeights=(short *)Alloc((pVariables+1)*sizeof(short));
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

void kinitBbaHeaps(kStrategy heap)
{
#if 0
  // use extra heap for monoms of T, if not syzcomp
  if (!strat->red == redSyz)
  {
    strat->THeap = mmCreateTempHeap();
  }
#endif
}


ideal bba (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat)
{
  int   srmax,lrmax, red_result;
  int   olddeg,reduc;
  int hilbeledeg=1,hilbcount=0,minimcnt=0;

  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  initHilbCrit(F,Q,&hilb,strat);
  initBba(F,strat);
  initBuchMoraPos(strat);
  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  /*Shdl=*/initBuchMora(F, Q,strat);
  if (strat->minim>0) strat->M=idInit(IDELEMS(F),F->rank);
  kinitBbaHeaps(strat);
  srmax = strat->sl;
  reduc = olddeg = lrmax = 0;
  /* compute------------------------------------------------------- */
  while (strat->Ll >= 0)
  {
    if (strat->Ll > lrmax) lrmax =strat->Ll;/*stat.*/
    if (TEST_OPT_DEBUG) messageSets(strat);
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
      /* deletes the short spoly and computes */
      pFree1(strat->P.p);
      /* the real one */
      ksCreateSpoly(&(strat->P),
                    strat->kNoether);
    }
    if((strat->P.p1==NULL) && (strat->minim>0)) strat->P.p2=pCopy(strat->P.p);

    if (TEST_OPT_PROT)
      message((strat->honey ? strat->P.ecart : 0) + pFDeg(strat->P.p),
              &olddeg,&reduc,strat);

    /* reduction of the element choosen from L */
    red_result = strat->red(&strat->P,strat);

    // reduktion to non-zero new poly
    if (red_result == 1)
    {
      /* statistic */
      if (TEST_OPT_PROT) PrintS("s");
      /* enter P.p into s and L */
      {
        int pos=posInS(strat->S,strat->sl,strat->P.p);
        if (TEST_OPT_INTSTRATEGY)
        {
          pCleardenom(strat->P.p);
          if ((!TEST_OPT_MINRES)||(strat->syzComp==0)||(!strat->homog))
          {
            strat->P.p = redtailBba(strat->P.p,pos-1,strat);
            pCleardenom(strat->P.p);
          }
        }
        else
        {
          pNorm(strat->P.p);
          if ((!TEST_OPT_MINRES)||(strat->syzComp==0)||(!strat->homog))
          {
            strat->P.p = redtailBba(strat->P.p,pos-1,strat);
          }
        }

        if (TEST_OPT_DEBUG){PrintS("new s:");wrp(strat->P.p);PrintLn();}

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
        enterTBba(strat->P, strat->posInT(strat->T,strat->tl,strat->P), 
                  strat);
        enterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat);
        // do we really need to recompute posInS
        strat->enterS(strat->P,posInS(strat->S,strat->sl,strat->P.p), strat);
        if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);
      }
      if (strat->P.lcm!=NULL) pFree1(strat->P.lcm);
      if (strat->sl>srmax) srmax = strat->sl;
    }
#ifdef KDEBUG
    strat->P.lcm=NULL;
#endif
    kTest_TS(strat);
  }
  if (TEST_OPT_DEBUG) messageSets(strat);
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
      Free((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
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
    Free((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  if (strat->sevS != NULL)
    Free((ADDRESS)strat->sevS,IDELEMS(strat->Shdl)*sizeof(unsigned long));
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
    Free((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  if (strat->sevS != NULL)
    Free((ADDRESS)strat->sevS,IDELEMS(strat->Shdl)*sizeof(unsigned long));
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
      Free((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
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
  kStrategy strat=(kStrategy)Alloc0SizeOf(skStrategy);

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
  kFreeStrat(strat);
  if ((delete_w)&&(w!=NULL)&&(*w!=NULL)) delete *w;
  idSkipZeroes(r);
  return r;
}
