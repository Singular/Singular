/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kstd2.cc,v 1.12 1998-03-16 14:56:32 obachman Exp $ */
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
#include "kstd2.h"
#include "khstd.h"
#include "spolys.h"
#include "cntrlc.h"
#include "weight.h"
#include "ipid.h"
#include "ipshell.h"
#include "intvec.h"
#ifdef STDTRACE
#include "comm.h"
#include "lists.h"
#endif
#ifdef COMP_FAST
#include "spSpolyLoop.h"
#endif

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
static void redSyz (LObject* h,kStrategy strat)
{
  int j = 0,i=0,pos;
  BOOLEAN exchanged=pDivisibleBy((*h).p2,(*h).p1);
  poly p,q;

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
//PrintS("#");
          deleteInL(strat->L,&strat->Ll,i,strat);
        }
        else
          i++;
      }
      i = 0;
    }
  }
#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    wrp(h->p);
    PrintS(" ");
  }
#endif
  loop
  {
#ifdef KDEBUG
    if (TEST_OPT_DEBUG) Print("%d",j);
#endif
    if (pDivisibleBy(strat->S[j],(*h).p))
    {
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS("+\nwith ");
        wrp(strat->S[j]);
      }
#endif
      if ((!exchanged) && (pEqual((*h).p,strat->S[j])))
      {
//PrintS("@");
        q = kFromInput(strat->S[j],strat);
        if (q!=NULL)
        {
//Print("%d*",pGetComp(q));
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
          while ((i<=strat->tl) && (strat->T[i].p!=p)) i++;
          if (i<=strat->tl) strat->T[i].p = strat->S[j];
          for (i=0;i<=strat->Ll;i++)
          {
            if (strat->L[i].p1==p) strat->L[i].p1=strat->S[j];
            if (strat->L[i].p2==p) strat->L[i].p2=strat->S[j];
          }
        }
      }
      //if (strat->interpt) test_int_std(strat->kIdeal);
      /*- compute the s-polynomial -*/
      (*h).p = spSpolyRed(strat->S[j],(*h).p,strat->kNoether,
                          strat->spSpolyLoop);
      if ((*h).p == NULL)
      {
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) PrintS(" to 0\n");
#endif
        if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
        (*h).lcm=NULL;
#endif
        return;
      }
/*- try to reduce the s-polynomial -*/
      j = 0;
    }
    else
    {
      if (j >= strat->sl)
      {
        if (exchanged)
        {
          if (pGetComp((*h).p) > strat->syzComp)
          {
//PrintS("syz");
            pDelete(&((*h).p));
            return;
          }
          else
          {
            if (!TEST_OPT_INTSTRATEGY)
            {
//PrintS("r");
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
        return;
      }
      j++;
    }
  }
}

/*2
*  reduction procedure for the homogeneous case
*  and the case of a degree-ordering
*/
static void redHomog (LObject* h,kStrategy strat)
{
  if (strat->tl<0)
  {
    enterTBba((*h),0,strat);
    return;
  }

  int j = 0;

#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    wrp(h->p);
    PrintS(" ");
  }
#endif
  if (strat->ak!=0)
  {
    loop
      {
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) Print("%d",j);
#endif
        if (pDivisibleBy1(strat->S[j],(*h).p))
        {
#ifdef KDEBUG
          if (TEST_OPT_DEBUG)
          {
            PrintS("+\nwith ");
            wrp(strat->S[j]);
          }
#endif
          //if (strat->interpt) test_int_std(strat->kIdeal);
          /*- compute the s-polynomial -*/
          (*h).p = spSpolyRed(strat->S[j],(*h).p,strat->kNoether,
                              strat->spSpolyLoop);
          if ((*h).p == NULL)
          {
#ifdef KDEBUG
            if (TEST_OPT_DEBUG) PrintS(" to 0\n");
#endif
            if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
            (*h).lcm=NULL;
#endif
            return;
          }
          j = 0;
        }
        else
        {
          if (j >= strat->sl)
          {
            enterTBba((*h),strat->tl+1,strat);
            return;
          }
          j++;
        }
      }
  }
  else
  {
    // no module component
    loop
      {
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) Print("%d",j);
#endif
        if (pDivisibleBy2(strat->S[j],(*h).p))
        {
#ifdef KDEBUG
          if (TEST_OPT_DEBUG)
          {
            PrintS("+\nwith ");
            wrp(strat->S[j]);
          }
#endif
          //if (strat->interpt) test_int_std(strat->kIdeal);
          /*- compute the s-polynomial -*/
          (*h).p = spSpolyRed(strat->S[j],(*h).p,strat->kNoether,
                              strat->spSpolyLoop);
          if ((*h).p == NULL)
          {
#ifdef KDEBUG
            if (TEST_OPT_DEBUG) PrintS(" to 0\n");
#endif
            if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
            (*h).lcm=NULL;
#endif
            return;
          }
          j = 0;
        }
        else
        {
          if (j >= strat->sl)
          {
            enterTBba((*h),strat->tl+1,strat);
            return;
          }
          j++;
        }
      }
  }
}

/*2
*  reduction procedure for the homogeneous case
*  and the case of a degree-ordering
*/
static void redHomog0 (LObject* h,kStrategy strat)
{
  if (strat->tl<0)
  {
    enterTBba((*h),0,strat);
    return;
  }

  int j = 0;
  int k = 0;

#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    wrp(h->p);
    PrintS(" ");
  }
#endif
  if (strat->ak)
  {
    loop
      {
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) Print("%d",j);
#endif
        if (pDivisibleBy1(strat->T[j].p,(*h).p))
        {
          //if (strat->interpt) test_int_std(strat->kIdeal);
#ifdef KDEBUG
          if (TEST_OPT_DEBUG)
          {
            PrintS("+\nwith ");
            wrp(strat->S[j]);
          }
#endif
          /*- compute the s-polynomial -*/
          (*h).p = spSpolyRed(strat->T[j].p,(*h).p,strat->kNoether,
                              strat->spSpolyLoop);
          if ((*h).p == NULL)
          {
#ifdef KDEBUG
            if (TEST_OPT_DEBUG) PrintS(" to 0\n");
#endif
            if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
            (*h).lcm=NULL;
#endif
            return;
          }
          j = 0;
        }
        else
        {
          if (j >= strat->tl)
          {
            //pContent((*h).p);
            pCleardenom((*h).p);// also does a pContent
/*
 *       (*h).length=pLength0((*h).p);
 */
            k=strat->posInT(strat->T,strat->tl,(*h));
            enterTBba((*h),k,strat);
            return;
          }
          j++;
        }
      }
  }
  else
  {
    loop
      {

        // no module component
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) Print("%d",j);
#endif
        if (pDivisibleBy2(strat->T[j].p,(*h).p))
        {
          //if (strat->interpt) test_int_std(strat->kIdeal);
#ifdef KDEBUG
          if (TEST_OPT_DEBUG)
          {
            PrintS("+\nwith ");
            wrp(strat->S[j]);
          }
#endif
          /*- compute the s-polynomial -*/
          (*h).p = spSpolyRed(strat->T[j].p,(*h).p,strat->kNoether,
                              strat->spSpolyLoop);
          if ((*h).p == NULL)
          {
#ifdef KDEBUG
            if (TEST_OPT_DEBUG) PrintS(" to 0\n");
#endif
            if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
            (*h).lcm=NULL;
#endif
            return;
          }
          j = 0;
        }
        else
        {
          if (j >= strat->tl)
          {
            //pContent((*h).p);
            pCleardenom((*h).p);// also does a pContent
/*
 *       (*h).length=pLength0((*h).p);
 */
            k=strat->posInT(strat->T,strat->tl,(*h));
            enterTBba((*h),k,strat);
            return;
          }
          j++;
        }
      }
  }
}


/*2
*  reduction procedure for the inhomogeneous case
*  and not a degree-ordering
*/
static void redLazy (LObject* h,kStrategy strat)
{
  if (strat->tl<0)
  {
    enterTBba((*h),0,strat);
    return;
  }

  int at,d,i;
  int j = 0;
  int pass = 0;
  int reddeg = pFDeg((*h).p);

#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    wrp(h->p);
    PrintS(" ");
  }
#endif
  loop
  {
    if (pDivisibleBy1(strat->S[j],(*h).p))
    {
      //if (strat->interpt) test_int_std(strat->kIdeal);
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        Print("\nwith S[%d] ",j);
        wrp(strat->S[j]);
      }
#endif
      /*- compute the s-polynomial -*/
      (*h).p = spSpolyRed(strat->S[j],(*h).p,strat->kNoether
                          , strat->spSpolyLoop);
      if ((*h).p == NULL)
      {
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) PrintS(" to 0\n");
#endif
        if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
        (*h).lcm=NULL;
#endif
        return;
      }
#ifdef KDEBUG
      else if (TEST_OPT_DEBUG)
      {
        PrintS("to:");
        wrp((*h).p);
        PrintLn();
      }
#endif
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
            if (i<0)
            {
              enterTBba((*h),strat->tl+1,strat);
              return;
            }
          }
          while (!pDivisibleBy1(strat->S[i],(*h).p));
#ifdef KDEBUG
          if (TEST_OPT_DEBUG) Print(" ->L[%d]\n",at);
#endif
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
          (*h).p = NULL;
          return;
        }
      }
      else if ((TEST_OPT_PROT) && (strat->Ll < 0) && (d != reddeg))
      {
        Print(".%d",d);mflush();
        reddeg = d;
      }
      j = 0;
    }
    else
    {
      if (j >= strat->sl)
      {
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) PrintLn();
#endif
        if (TEST_OPT_INTSTRATEGY)
        {
          //pContent(h->p);
          pCleardenom(h->p);// also does a pContent
        }
        enterTBba((*h),strat->tl+1,strat);
        return;
      }
      j++;
    }
  }
}

/*2
*  reduction procedure for the sugar-strategy (honey)
* reduces h with elements from T choosing first possible
* element in T with respect to the given ecart
*/
static void redHoney (LObject*  h,kStrategy strat)
{
  if (strat->tl<0)
  {
    enterTBba((*h),0,strat);
    return;
  }

  poly pi;
  int i,j,at,reddeg,d,pass,ei;

  pass = j = 0;
  d = reddeg = pFDeg((*h).p)+(*h).ecart;
#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    wrp((*h).p);
  }
#endif
  loop
  {
    if (pDivisibleBy1(strat->T[j].p,(*h).p))
    {
#ifdef KDEBUG
      if (TEST_OPT_DEBUG) Print(" T[%d]",j);
#endif
      pi = strat->T[j].p;
      ei = strat->T[j].ecart;
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
        if ((strat->T[i].ecart < ei) && pDivisibleBy1(strat->T[i].p,(*h).p))
        {
#ifdef KDEBUG
          if (TEST_OPT_DEBUG) Print(" T[%d]",i);
#endif
          /*
          * the polynomial to reduce with is now;
          */
          pi = strat->T[i].p;
          ei = strat->T[i].ecart;
        }
      }

      /*
      * end of search: have to reduce with pi
      */
      if (ei > (*h).ecart)
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
#ifdef KDEBUG
            if (TEST_OPT_DEBUG) Print(" ecart too big: -> L%d\n",at);
#endif
            (*h).p = NULL;
            return;
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
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS("\nwith ");
        wrp(pi);
      }
#endif
      if (strat->fromT)
      {
        strat->fromT=FALSE;
        (*h).p = spSpolyRedNew(pi,(*h).p,strat->kNoether,
                               strat->spSpolyLoop);
      }
      else
        (*h).p = spSpolyRed(pi,(*h).p,strat->kNoether, strat->spSpolyLoop);
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS(" to ");
        wrp((*h).p);
        PrintLn();
      }
#endif
      if ((*h).p == NULL)
      {
        if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
        (*h).lcm=NULL;
#endif
        return;
      }
      /* compute the ecart */
      if (ei <= (*h).ecart)
        (*h).ecart = d-pFDeg((*h).p);
      else
        (*h).ecart = d-pFDeg((*h).p)+ei-(*h).ecart;
//      if (strat->syzComp)
//      {
//        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
//        {
#ifdef KDEBUG
//          if (TEST_OPT_DEBUG)
#endif
//            PrintS("  >syzComp\n");
//          if (TEST_OPT_INTSTRATEGY) pContent(h->p);
//          at=strat->posInT(strat->T,strat->tl,(*h));
//          enterTBba((*h),at,strat);
//          return;
//        }
//      }
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
            if (i<0)
            {
              at=strat->posInT(strat->T,strat->tl,(*h));
              enterTBba((*h),at,strat);
              return;
            }
          } while (!pDivisibleBy1(strat->S[i],(*h).p));
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
#ifdef KDEBUG
          if (TEST_OPT_DEBUG)
            Print(" degree jumped: -> L%d\n",at);
#endif
          (*h).p = NULL;
          return;
        }
      }
      else if (TEST_OPT_PROT && (strat->Ll < 0) && (d > reddeg))
      {
        reddeg = d;
        Print(".%d",d); mflush();
      }
      j = 0;
    }
    else
    {
      if (j >= strat->tl)
      {
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) PrintLn();
#endif
        if (TEST_OPT_INTSTRATEGY)
        {
          //pContent(h->p);
          pCleardenom(h->p);// also does a pContent
        }
        at=strat->posInT(strat->T,strat->tl,(*h));
        enterTBba((*h),at,strat);
        return;
      }
      j++;
    }
  }
}

/*2
*  reduction procedure for tests only
*  reduces with elements from T and chooses the best possible
*/
static void redBest (LObject*  h,kStrategy strat)
{
  if (strat->tl<0)
  {
    enterTBba((*h),0,strat);
    return;
  }

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
#ifdef SDRING
      // spSpolyShortBba will not work in the SRING case
      if (pSDRING)
      {
        p=spSpolyCreate(strat->T[j].p,(*h).p,strat->kNoether);
        if (p!=NULL) pDelete(&pNext(p));
      }
      else
#endif
      p = spSpolyShortBba(strat->T[j].p,(*h).p);
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
#ifdef SDRING
              // spSpolyShortBba will not work in the SRING case
              if (pSDRING)
              {
                ph=spSpolyCreate(strat->T[j].p,(*h).p,strat->kNoether);
                if (ph!=NULL) pDelete(&pNext(ph));
              }
              else
#endif
              ph = spSpolyShortBba(strat->T[j].p,(*h).p);
              if (ph==NULL)
              {
                pFree1(p);
                pDelete(&((*h).p));
                if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
                (*h).lcm=NULL;
#endif
                return;
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
        (*h).p = spSpolyRed(strat->T[jbest].p,(*h).p,strat->kNoether,
                            strat->spSpolyLoop);
      }
      else
      {
        if (h->lcm!=NULL) pFree1((*h).lcm);
#ifdef KDEBUG
        (*h).lcm=NULL;
#endif
        (*h).p = NULL;
        return;
      }
      if (strat->honey && pLexOrder)
        strat->initEcart(h);
      /* h.length:=l; */
      /* try to reduce the s-polynomial */
//      if (strat->syzComp)
//      {
//        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
//        {
#ifdef KDEBUG
//          if (TEST_OPT_DEBUG)
#endif
//            PrintS(" >syzComp\n");
//          if (TEST_OPT_INTSTRATEGY) pContent(h->p);
//          at=strat->posInT(strat->T,strat->tl,(*h));
//          enterTBba((*h),at,strat);
//          return;
//        }
//      }
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
            return;
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
      if (j >= strat->tl)
      {
        if (TEST_OPT_INTSTRATEGY)
        {
          //pContent(h->p);
          pCleardenom(h->p);// also does a pContent
        }
        at=strat->posInT(strat->T,strat->tl,(*h));
        enterTBba((*h),at,strat);
        return;
      }
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

#ifdef KDEBUG
 if (TEST_OPT_DEBUG)
 {
   PrintS("red:");wrp(h);
 }
#endif
  if (0 > strat->tl)
  {
#ifdef KDEBUG
    if (TEST_OPT_DEBUG) PrintLn();
#endif
    return h;
  }
  loop
  {
    if (pDivisibleBy1(strat->S[j],h))
    {
      //if (strat->interpt) test_int_std(strat->kIdeal);
      /*- compute the s-polynomial -*/
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        Print("\nwith S[%d]:",j);wrp(strat->S[j]);
      }
#endif
      h = spSpolyRed(strat->S[j],h,strat->kNoether, strat->spSpolyLoop);
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS("\nto:");
        wrp(h);
        if (h==NULL) PrintLn();
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
  else if (TEST_OPT_INTSTRATEGY && strat->homog)
    strat->red = redHomog0;
  else
    strat->red = redHomog;
  if (TEST_OPT_MINRES && strat->homog && (strat->syzComp >0))
    strat->red = redSyz;

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

#ifdef STDTRACE
lists bbaLink (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat, stdLink stdTrace)
{
  int oldLl;
  int srmax,lrmax;
  int olddeg,reduc;
  int anzTupel=0, anzNew = 0, anzSkipped=0;
#ifdef SDRING
  polyset aug=(polyset)Alloc(setmax*sizeof(poly));
  int augmax=setmax, augl=-1;
  poly oldLcm=NULL;
#endif
  int hilbeledeg=1,hilbcount=0,minimcnt=0;

  if(stdTrace!=NULL) stdTrace->Start(strat);

  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  initHilbCrit(F,Q,&hilb,strat);
  initBba(F,strat);
  initBuchMoraPos(strat);
  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  /*Shdl=*/initBuchMora(F, Q,strat);
  if (strat->minim>0)
  {
    strat->M=idInit(IDELEMS(F),F->rank);
  }
  srmax = strat->sl;
  reduc = olddeg = lrmax = 0;
  /* compute------------------------------------------------------- */
  while ((stdTrace!=NULL && !stdTrace->CheckEnd(strat)) || (stdTrace == NULL && strat->Ll>=0))
    {
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if(stdTrace!=NULL && stdTrace->Receive)
	{
	  stdTrace->ReceiveMsg();
	  stdTrace->ParseMessage(strat);
	}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if((stdTrace!=NULL && stdTrace->Verwaltung) || (stdTrace == NULL))
	{
	  if (strat->Ll > lrmax) lrmax =strat->Ll;/*stat.*/
#ifdef KDEBUG
	  if (TEST_OPT_DEBUG) messageSets(strat);
#endif
	  //test_int_std(strat->kIdeal);
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
	}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if((stdTrace!=NULL && stdTrace->TupelL) || (stdTrace == NULL ))
	{
	  /* picks the last element from the lazyset L */
	  strat->P = strat->L[strat->Ll];
	  anzTupel++;
	  strat->Ll--;
          if(stdTrace!=NULL && stdTrace->TupelStore)
            {
              if (TEST_OPT_PROT) PrintS(":");
              stdTrace->Store(strat->P);
	      strat->P.p=NULL;
	      anzSkipped++;
            }
	}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if((stdTrace!=NULL && stdTrace->SPoly) || (stdTrace == NULL))
	{
	  kTest(strat);
	  if (pNext(strat->P.p) == strat->tail)
	    {
	      /* deletes the short spoly and computes */
	      pFree1(strat->P.p);
	      /* the real one */
	      strat->P.p = spSpolyCreate(strat->P.p1,strat->P.p2,strat->kNoether);
	    }
	  if((strat->P.p1==NULL) && (strat->minim>0))
	    strat->P.p2=pCopy(strat->P.p);
	}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if((stdTrace!=NULL && stdTrace->Reduzieren) || (stdTrace == NULL))
	{
#ifdef SDRING
	  if (strat->P.p != NULL)
#endif
	    {
	      if (strat->honey)
		{
		  if (TEST_OPT_PROT) message(strat->P.ecart+pFDeg(strat->P.p),&olddeg,&reduc,strat);
		}
	      else
		{
		  if (TEST_OPT_PROT) message(pFDeg(strat->P.p),&olddeg,&reduc,strat);
		}
	      /* reduction of the element choosen from L */
	      oldLl=strat->Ll;
	      strat->red(&strat->P,strat);
	      if(stdTrace!=NULL && stdTrace->TupelPosition)
		stdTrace->CheckPosition(strat,oldLl);
	      if((stdTrace!=NULL && stdTrace->TupelMelden))
		stdTrace->SendTupel(strat);
	      if(stdTrace!=NULL && strat->P.p!=NULL && stdTrace->Modus==ModCheck)
		anzNew++;
	    }
	  if (strat->P.p != NULL)
	    {
#ifdef SDRING
	      aug[0]=strat->P.p;
	      augl=0;
	      if (pSDRING)
          {
            oldLcm=strat->P.lcm;
#ifdef SRING
            if (pSRING) psAug(pCopy(strat->P.p),pOne(),&aug,&augl,&augmax);
#endif
#ifdef DRING
            if (pDRING) pdAug(pCopy(strat->P.p),&aug,&augl,&augmax);
#endif
#ifdef KDEBUG
            if (TEST_OPT_DEBUG)
            {
              PrintS(" aug of ");
              wrp(aug[0]);
              PrintLn();
              int iiaug=augl;
              while (iiaug>=0)
              {
                Print(" to %d:",iiaug);
                wrp(aug[iiaug]);
                PrintLn();
                iiaug--;
              }
            }
#endif
          }
	      for (augl++;augl != 0;)
		{
		  strat->P.p=aug[--augl];
		  aug[augl]=NULL;
		  if (pSDRING)
		    {
		      if (oldLcm==NULL) strat->P.lcm=NULL;
		      else  strat->P.lcm=pCopy1(oldLcm);
		    }
		  if ((augl!=0)&&(strat->P.p!=NULL))
		    strat->red(&strat->P,strat);
		  if (strat->P.p != NULL)
		    {
#endif
		      /* statistic */
		      if (TEST_OPT_PROT) PrintS("s");
		      /* enter P.p into s and L */
		      {
			int pos=posInS(strat->S,strat->sl,strat->P.p);
#ifdef SDRING
			if ((pSDRING) && (pos<=strat->sl)&& (pComparePolys(strat->P.p,strat->S[pos])))
			  {
			    if (TEST_OPT_PROT)
			      PrintS("d");
			  }
			else
#endif
			  {
			    if (TEST_OPT_INTSTRATEGY)
			      {
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
#ifdef KDEBUG
			    if (TEST_OPT_DEBUG)
			      {
				PrintS("new s:");
				wrp(strat->P.p);
				PrintLn();
			      }
#endif
			    if((strat->P.p1==NULL) && (strat->minim>0))
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
			    enterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat);
			    if (strat->sl==-1) pos=0;
			    else pos=posInS(strat->S,strat->sl,strat->P.p);
			    strat->enterS(strat->P,pos,strat);
			  }
			if (hilb!=NULL)
			  {  // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
			    oldLl=strat->Ll;
			    khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);
			    if(stdTrace!=NULL)
			      stdTrace->CheckHilb(strat,oldLl);
			  }
		      }
		      if (strat->P.lcm!=NULL) pFree1(strat->P.lcm);
#ifdef SDRING
		    }
		}
	      /* delete the old pair */
	      if (pSDRING &&(oldLcm!=NULL)) pFree1(oldLcm);
#endif
	      if (strat->sl>srmax) srmax = strat->sl;
	    }
 	  if(stdTrace!=NULL && stdTrace->TupelTesten)
 	    stdTrace->TupelDifferent(strat);
	}
#ifdef KDEBUG
      strat->P.lcm=NULL;
#endif
      kTest(strat);
    }
  if(stdTrace !=NULL)
    if(TEST_OPT_PROT)
      {
	Print("\n(Tupel  Skipped  New) = (%i  %i  %i)\n",anzTupel, anzSkipped, anzNew);
      }
  if((stdTrace!=NULL && stdTrace->ResultSend) || (stdTrace == NULL))
    {
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
	      Free((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
	      ecartWeights=NULL;
	    }
	}
#ifdef SDRING
      Free((ADDRESS)aug,augmax*sizeof(poly));
#endif
      if (TEST_OPT_PROT) messageStat(srmax,lrmax,hilbcount,strat);
      if (Q!=NULL) updateResult(strat->Shdl,Q,strat);
    }
  if(stdTrace!=NULL)
    stdTrace->End(strat);
  lists l=(lists)Alloc(sizeof(slists));
  l->Init(2);
  l->m[0].rtyp = IDEAL_CMD;
  l->m[0].data = (void *) strat->Shdl;
   if(stdTrace!=NULL )
     {
       l->m[1].rtyp = LIST_CMD;
       l->m[1].data = (void *)stdTrace->RestTupel();
     }
  return (l);
}

#else

ideal bba (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat)
{
  int   srmax,lrmax;
  int   olddeg,reduc;
#ifdef SDRING
  polyset aug=(polyset)Alloc(setmax*sizeof(poly));
  int augmax=setmax, augl=-1;
  poly oldLcm=NULL;
#endif
  int hilbeledeg=1,hilbcount=0,minimcnt=0;

  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  initHilbCrit(F,Q,&hilb,strat);
  initBba(F,strat);
  initBuchMoraPos(strat);
  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  /*Shdl=*/initBuchMora(F, Q,strat);
  if (strat->minim>0)
  {
    strat->M=idInit(IDELEMS(F),F->rank);
  }
  srmax = strat->sl;
  reduc = olddeg = lrmax = 0;
  /* compute------------------------------------------------------- */
  while (strat->Ll >= 0)
  {
    if (strat->Ll > lrmax) lrmax =strat->Ll;/*stat.*/
#ifdef KDEBUG
    if (TEST_OPT_DEBUG) messageSets(strat);
#endif
    //test_int_std(strat->kIdeal);
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
    kTest(strat);
    if (pNext(strat->P.p) == strat->tail)
    {
      /* deletes the short spoly and computes */
      pFree1(strat->P.p);
      /* the real one */
      strat->P.p = spSpolyCreate(strat->P.p1,strat->P.p2,strat->kNoether);
    }
    kTest(strat);
    if((strat->P.p1==NULL) && (strat->minim>0))
      strat->P.p2=pCopy(strat->P.p);
#ifdef SDRING
    if (strat->P.p != NULL)
#endif
    {
      if (strat->honey)
      {
        if (TEST_OPT_PROT) message(strat->P.ecart+pFDeg(strat->P.p),&olddeg,&reduc,strat);
      }
      else
      {
        if (TEST_OPT_PROT) message(pFDeg(strat->P.p),&olddeg,&reduc,strat);
      }
      kTest(strat);
      /* reduction of the element choosen from L */
      strat->red(&strat->P,strat);
      pTest(strat->P.p);
    }
    kTest(strat);
    if (strat->P.p != NULL)
    {
#ifdef SDRING
      aug[0]=strat->P.p;
      augl=0;
      if (pSDRING)
      {
        oldLcm=strat->P.lcm;
#ifdef SRING
        if (pSRING) psAug(pCopy(strat->P.p),pOne(),&aug,&augl,&augmax);
#endif
#ifdef DRING
        if (pDRING) pdAug(pCopy(strat->P.p),&aug,&augl,&augmax);
#endif
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
        {
          PrintS(" aug of ");
          wrp(aug[0]);
          PrintLn();
          int iiaug=augl;
          while (iiaug>=0)
          {
            Print(" to %d:",iiaug);
            wrp(aug[iiaug]);
            PrintLn();
            iiaug--;
          }
        }
#endif
      }
      for (augl++;augl != 0;)
      {
        strat->P.p=aug[--augl];
        aug[augl]=NULL;
        if (pSDRING)
        {
          if (oldLcm==NULL) strat->P.lcm=NULL;
          else  strat->P.lcm=pCopy1(oldLcm);
        }
        if ((augl!=0)&&(strat->P.p!=NULL))
          strat->red(&strat->P,strat);
        if (strat->P.p != NULL)
        {
#endif
          /* statistic */
          if (TEST_OPT_PROT) PrintS("s");
          /* enter P.p into s and L */
          {
            int pos=posInS(strat->S,strat->sl,strat->P.p);
#ifdef SDRING
            if ((pSDRING) && (pos<=strat->sl)&& (pComparePolys(strat->P.p,strat->S[pos])))
            {
              if (TEST_OPT_PROT)
                PrintS("d");
            }
            else
#endif
            {
              if (TEST_OPT_INTSTRATEGY)
              {
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
#ifdef KDEBUG
              if (TEST_OPT_DEBUG)
              {
                PrintS("new s:");
                wrp(strat->P.p);
                PrintLn();
              }
#endif
              if((strat->P.p1==NULL) && (strat->minim>0))
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
              enterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat);
              if (strat->sl==-1) pos=0;
              else pos=posInS(strat->S,strat->sl,strat->P.p);
              strat->enterS(strat->P,pos,strat);
            }
            if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);
          }
          if (strat->P.lcm!=NULL) pFree1(strat->P.lcm);
#ifdef SDRING
        }
      }
      /* delete the old pair */
      if (pSDRING &&(oldLcm!=NULL)) pFree1(oldLcm);
#endif
      if (strat->sl>srmax) srmax = strat->sl;
    }
#ifdef KDEBUG
    strat->P.lcm=NULL;
#endif
    kTest(strat);
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
      Free((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }
#ifdef SDRING
  Free((ADDRESS)aug,augmax*sizeof(poly));
#endif
  if (TEST_OPT_PROT) messageStat(srmax,lrmax,hilbcount,strat);
  if (Q!=NULL) updateResult(strat->Shdl,Q,strat);
  return (strat->Shdl);
}
#endif

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
#ifdef COMP_FAST
  strat->spSpolyLoop = spGetSpolyLoop(currRing, strat);
#endif
  /*- init local data struct.---------------------------------------- -*/
  /*Shdl=*/initS(F,Q,strat);
  /*- compute------------------------------------------------------- -*/
  if ((TEST_OPT_INTSTRATEGY)&&(lazyReduce==0))
  {
    for (i=strat->sl;i>=0;i--)
      pNorm(strat->S[i]);
  }
  kTest(strat);
  p = redNF(pCopy(q),strat);
  kTest(strat);
  if ((p!=NULL)&&(lazyReduce==0))
  {
    p = redtailBba(p,strat->sl,strat);
  }
  kTest(strat);
  /*- release temp data------------------------------- -*/
  Free((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  idDelete(&strat->Shdl);
  test=save_test;
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
#ifdef COMP_FAST
  strat->spSpolyLoop = spGetSpolyLoop(currRing, strat);
#endif
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
      p = redNF(pCopy(q->m[i]),strat);
      if ((p!=NULL)&&(lazyReduce==0))
      {
        p = redtailBba(p,strat->sl,strat);
      }
      res->m[i]=p;
    }
    //else
    //  res->m[i]=NULL;
    if (TEST_OPT_PROT)
    {
      PrintS("-");mflush();
    }
  }
  /*- release temp data------------------------------- -*/
  Free((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  idDelete(&strat->Shdl);
  test=save_test;
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
  kStrategy strat=(kStrategy)Alloc0(sizeof(skStrategy));

  if (currRing->ch==0) strat->LazyPass=2;
  else                 strat->LazyPass=20;
  strat->LazyDegree = 1;
  if ((h==testHomog))
  {
    if (idRankFreeModule(F)==0)
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
      pOldFDeg = pFDeg;
      pFDeg = kModDeg;
      toReset = TRUE;
    }
    pLexOrder = TRUE;
    strat->LazyPass*=2;
  }
  strat->homog=h;
  spSet(currRing);
#ifdef COMP_FAST
  strat->spSpolyLoop = spGetSpolyLoop(currRing, strat);
#endif
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
  Free((ADDRESS)strat,sizeof(skStrategy));
  if ((delete_w)&&(w!=NULL)&&(*w!=NULL)) delete *w;
  idSkipZeroes(r);
  return r;
}
