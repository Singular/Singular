/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gr_kstd2.cc,v 1.9 2002-07-12 13:48:28 levandov Exp $ */
/*
*  ABSTRACT -  Kernel: alg. of Buchberger
*/

#include "mod2.h"
#ifdef HAVE_PLURAL
#include "omalloc.h"
#include "polys.h"
#include "ideals.h"
#include "febase.h"
#include "kutil.h"
#include "kstd1.h"
#include "khstd.h"
//#include "spolys.h"
#include "cntrlc.h"
#include "weight.h"
#include "ipid.h"
#include "ipshell.h"
#include "intvec.h"
#include "tok.h"
#include "gring.h"

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
*reduces h with elements from T choosing  the first possible
* element in t with respect to the given pDivisibleBy
*/
int redGrFirst (LObject* h,kStrategy strat)
{
  int at,reddeg,d,i;
  int pass = 0;
  int j = 0;

  d = pFDeg((*h).p)+(*h).ecart;
  reddeg = strat->LazyDegree+d;
  loop
  {
    if (j > strat->sl)
    {
      if (TEST_OPT_DEBUG) PrintLn();
      return 0;
    }
    if (TEST_OPT_DEBUG) Print("%d",j);
    if (pDivisibleBy(strat->S[j],(*h).p))
    {
      if (TEST_OPT_DEBUG) PrintS("+\n");
      /*
      * the polynomial to reduce with is;
      * T[j].p
      */
      if (!TEST_OPT_INTSTRATEGY)
        pNorm(strat->S[j]);
      if (TEST_OPT_DEBUG)
      {
        wrp(h->p);
        PrintS(" with ");
        wrp(strat->S[j]);
      }
      (*h).p = nc_spGSpolyRed(strat->S[j],(*h).p, NULL, currRing);
      //spSpolyRed(strat->T[j].p,(*h).p,strat->kNoether);
      
      if (TEST_OPT_DEBUG)
      {
        PrintS(" to ");
        wrp(h->p);
      }
      if ((*h).p == NULL)
      {
        if (h->lcm!=NULL) p_LmFree((*h).lcm, currRing);
        return 0;
      }
      if (TEST_OPT_INTSTRATEGY)
      {
	pCleardenom((*h).p);
      }
      /*computes the ecart*/
      d = pLDeg((*h).p,&((*h).length));
      (*h).FDeg=pFDeg((*h).p);
      (*h).ecart = d-(*h).FDeg; /*pFDeg((*h).p);*/
      if ((strat->syzComp!=0) && !strat->honey)
      {
        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
        {
          if (TEST_OPT_DEBUG) PrintS(" > sysComp\n");
          return 0;
        }
      }
      /*- try to reduce the s-polynomial -*/
      pass++;
      /*
      *test whether the polynomial should go to the lazyset L
      *-if the degree jumps
      *-if the number of pre-defined reductions jumps
      */
      if ((strat->Ll >= 0)
      && ((d >= reddeg) || (pass > strat->LazyPass))
      && !strat->homog)
      {
        at = strat->posInL(strat->L,strat->Ll,h,strat);
        if (at <= strat->Ll)
        {
          i=strat->sl+1;
          do
          {
            i--;
            if (i<0) return 0;
          } while (!pDivisibleBy(strat->S[i],(*h).p));
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
          if (TEST_OPT_DEBUG) Print(" degree jumped; ->L%d\n",at);
          (*h).p = NULL;
          return 0;
        }
      }
      if ((TEST_OPT_PROT) && (strat->Ll < 0) && (d >= reddeg))
      {
        reddeg = d+1;
        Print(".%d",d);mflush();
      }
      j = 0;
      if TEST_OPT_DEBUG PrintLn();
    }
    else
    {
      if (TEST_OPT_DEBUG) PrintS("-");
      j++;
    }
  }
}

/*2
*  reduction procedure for the homogeneous case
*  and the case of a degree-ordering
*/
static int redHomog (LObject* h,kStrategy strat)
{
  if (strat->tl<0)
  {
    enterT((*h),strat);
    return 1;
  }

  int j = 0;

  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    wrp(h->p);
    PrintS(" ");
  }
  loop
  {
    if (TEST_OPT_DEBUG) Print("%d",j);
    if (pDivisibleBy(strat->S[j],(*h).p))
    {
      if (TEST_OPT_DEBUG)
      {
        PrintS("+\nwith ");
        wrp(strat->S[j]);
      }
      /*- compute the s-polynomial -*/
      (*h).p = nc_spGSpolyRed(strat->S[j],(*h).p,strat->kNoether,currRing);
      if ((*h).p == NULL)
      {
        if (TEST_OPT_DEBUG) PrintS(" to 0\n");
        if (h->lcm!=NULL) pLmFree((*h).lcm);
        (*h).lcm=NULL;
        return 0;
      }
/*
*      else if (strat->syzComp)
*      {
*        if (pMinComp((*h).p) > strat->syzComp)
*        {
*          enterT((*h),strat);
*          return;
*        }
*      }
*/
      /*- try to reduce the s-polynomial -*/
      j = 0;
    }
    else
    {
      if (j >= strat->sl)
      {
        enterT((*h),strat);
        return 1;
      }
      j++;
    }
  }
}

/*2
*  reduction procedure for the homogeneous case
*  and the case of a degree-ordering
*/
static int redHomog0 (LObject* h,kStrategy strat)
{
  if (strat->tl<0)
  {
    enterT((*h),strat);
    return 0;
  }

  int j = 0;
  int k = 0;

  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    wrp(h->p);
    PrintS(" ");
  }
  loop
  {
    if (TEST_OPT_DEBUG) Print("%d",j);
    if (pDivisibleBy(strat->T[j].p,(*h).p))
    {
      if (TEST_OPT_DEBUG)
      {
        PrintS("+\nwith ");
        wrp(strat->S[j]);
      }
      /*- compute the s-polynomial -*/
      (*h).p = nc_spGSpolyRed(strat->T[j].p,(*h).p,strat->kNoether,currRing);
      if ((*h).p == NULL)
      {
        if (TEST_OPT_DEBUG) PrintS(" to 0\n");
        if (h->lcm!=NULL) pLmFree((*h).lcm);
        (*h).lcm=NULL;
        return 0;
      }
      else if (strat->syzComp!=0)
      {
        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
        {
          //pContent((*h).p);
          pCleardenom((*h).p);// also does a pContent
/*
*         (*h).length=pLength0((*h).p);
*/
          enterT((*h),strat);
          return 0;
        }
      }
      /*- try to reduce the s-polynomial -*/
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
        enterT((*h),strat);
        return 0;
      }
      j++;
    }
  }
}

/*2
*  reduction procedure for the inhomogeneous case
*  and not a degree-ordering
*/
static int redLazy (LObject* h,kStrategy strat)
{
  if (strat->tl<0)
  {
    enterT((*h),strat);
    return 0;
  }

  int at,d,i;
  int j = 0;
  int pass = 0;
  int reddeg = pFDeg((*h).p);

  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    wrp(h->p);
    PrintS(" ");
  }
  loop
  {
    if (TEST_OPT_DEBUG) Print("%d",j);
    if (pDivisibleBy(strat->S[j],(*h).p))
    {
      if (TEST_OPT_DEBUG)
      {
        PrintS("+\nwith ");
        wrp(strat->S[j]);
      }
      /*- compute the s-polynomial -*/
      (*h).p = nc_spGSpolyRed(strat->S[j],(*h).p,strat->kNoether,currRing);
      if ((*h).p == NULL)
      {
        if (TEST_OPT_DEBUG) PrintS(" to 0\n");
        if (h->lcm!=NULL) pLmFree((*h).lcm);
        (*h).lcm=NULL;
        return 0;
      }
//      else if (strat->syzComp)
//      {
//        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
//        {
//          if (TEST_OPT_DEBUG) PrintS(" > syzComp\n");
//          if (TEST_OPT_INTSTRATEGY) pContent(h->p);
//          enterTBba((*h),strat->tl+1,strat);
//          return;
//        }
//      }
      else if (TEST_OPT_DEBUG)
      {
        PrintS("to:");
        wrp((*h).p);
        PrintLn();
      }
      /*- try to reduce the s-polynomial -*/
      pass++;
      d = pFDeg((*h).p);
      if ((strat->Ll >= 0) && ((d > reddeg) || (pass > strat->LazyPass)))
      {
        at = posInL11(strat->L,strat->Ll,h,strat);
        if (at <= strat->Ll)
        {
          i=strat->sl+1;
          do
          {
            i--;
            if (i<0)
            {
              enterT((*h),strat);
              return 0;
            }
          }
           while (!pDivisibleBy(strat->S[i],(*h).p));
          if (TEST_OPT_DEBUG) Print(" ->L[%d]\n",at);
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
          (*h).p = NULL;
          return 0;
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
      if (TEST_OPT_DEBUG) PrintS("-");
      if (j >= strat->sl)
      {
        if (TEST_OPT_DEBUG) PrintLn();
        if (TEST_OPT_INTSTRATEGY)
        {
          //pContent(h->p);
          pCleardenom(h->p);// also does a pContent
        }
        enterT((*h),strat);
        return 0;
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
static int redHoney (LObject*  h,kStrategy strat)
{
  if (strat->tl<0)
  {
    enterT((*h),strat);
    return 0;
  }

  poly pi;
  int i,j,at,reddeg,d,pass,ei;

  pass = j = 0;
  d = reddeg = pFDeg((*h).p)+(*h).ecart;
  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    wrp((*h).p);
  }
  loop
  {
    if (TEST_OPT_DEBUG) Print("%d",j);
    if (pDivisibleBy(strat->T[j].p,(*h).p))
    {
      if (TEST_OPT_DEBUG) PrintS("+");
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
        if (TEST_OPT_DEBUG) Print("%d",i);
        if ((strat->T[i].ecart < ei) && pDivisibleBy(strat->T[i].p,(*h).p))
        {
          if (TEST_OPT_DEBUG) PrintS("+");
          /*
          * the polynomial to reduce with is now;
          */
          pi = strat->T[i].p;
          ei = strat->T[i].ecart;
        }
        else if (TEST_OPT_DEBUG) PrintS("-");
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
          at = strat->posInL(strat->L,strat->Ll,h,strat);
          if(at <= strat->Ll)
          /*- h will not become the next element to reduce -*/
          {
            enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
            if (TEST_OPT_DEBUG) Print(" ecart too big: -> L%d\n",at);
            (*h).p = NULL;
            return 0;
          }
        }
      }
      if (TEST_OPT_DEBUG)
      {
        PrintS("\nwith ");
        wrp(pi);
      }
      if (strat->fromT)
      {
        strat->fromT=FALSE;
        (*h).p = nc_spGSpolyRedNew(pi,(*h).p,strat->kNoether,currRing);
      }
      else
        (*h).p = nc_spGSpolyRed(pi,(*h).p,strat->kNoether,currRing);
      if (TEST_OPT_DEBUG)
      {
        PrintS(" to ");
        wrp((*h).p);
        PrintLn();
      }
      if ((*h).p == NULL)
      {
        if (h->lcm!=NULL) pLmFree((*h).lcm);
        (*h).lcm=NULL;
        return 0;
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
//          if (TEST_OPT_DEBUG)
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
        at = strat->posInL(strat->L,strat->Ll,h,strat);
        if (at <= strat->Ll)
        {
          /*test if h is already standardbasis element*/
          i=strat->sl+1;
          do
          {
            i--;
            if (i<0)
            {
              enterT((*h),strat);
              return 0;
            }
          } while (!pDivisibleBy(strat->S[i],(*h).p));
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
          if (TEST_OPT_DEBUG)
            Print(" degree jumped: -> L%d\n",at);
          (*h).p = NULL;
          return 0;
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
      if (TEST_OPT_DEBUG) PrintS("-");
      if (j >= strat->tl)
      {
        if (TEST_OPT_DEBUG) PrintLn();
        if (TEST_OPT_INTSTRATEGY)
        {
          //pContent(h->p);
          pCleardenom(h->p);// also does a pContent
        }
        enterT((*h),strat);
        return 0;
      }
      j++;
    }
  }
}

/*2
*  reduction procedure for tests only
*  reduces with elements from T and chooses the best possible
*/
static int redBest (LObject*  h,kStrategy strat)
{
  if (strat->tl<0)
  {
    enterT((*h),strat);
    return 0;
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
      p = nc_spShort(strat->T[j].p,(*h).p);
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
              ph = nc_spShort(strat->T[j].p,(*h).p);
              if (ph==NULL)
              {
                pLmFree(p);
                pDelete(&((*h).p));
                if (h->lcm!=NULL)
		{
		  pLmFree((*h).lcm);
                  (*h).lcm=NULL;
		}
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
        (*h).p = nc_spGSpolyRed(strat->T[jbest].p,(*h).p,strat->kNoether,currRing);
      }
      else
      {
        if (h->lcm!=NULL)
	{
	  pLmFree((*h).lcm);
          (*h).lcm=NULL;
	}  
        (*h).p = NULL;
        return 0;
      }
      if (strat->honey && pLexOrder)
        strat->initEcart(h);
      /* h.length:=l; */
      /* try to reduce the s-polynomial */
//      if (strat->syzComp)
//      {
//        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
//        {
//          if (TEST_OPT_DEBUG)
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
          at = strat->posInL(strat->L,strat->Ll,h,strat);
          if (at <= strat->Ll)
          {
            enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
            (*h).p = NULL;
            return 0;
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
        enterT((*h),strat);
        return 0;
      }
      j++;
    }
  }
}

static void gr_initBba(ideal F,kStrategy strat)
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
  if (rIsPluralRing(currRing))
  {
    strat->red = redGrFirst;
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

ideal gr_bba (ideal F, ideal Q,kStrategy strat)
{
  intvec *w=NULL;
  intvec *hilb=NULL;
  int   srmax,lrmax;
  int   olddeg,reduc;
  int red_result=1;
  int hilbeledeg=1,hilbcount=0,minimcnt=0;

  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  // initHilbCrit(F,Q,&hilb,strat);
  /* in plural we don't need Hilb yet */
  gr_initBba(F,strat);
  initBuchMoraPos(strat);
  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  /*Shdl=*/initBuchMora(F, Q,strat);
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
    kTest(strat);
    if (pNext(strat->P.p) == strat->tail)
    {
      /* deletes the short spoly and computes */
      pLmFree(strat->P.p);
      /* the real one */
      if ((currRing->nc->type==nc_lie) && pHasNotCF(strat->P.p1,strat->P.p2)) /* prod crit */
      {
	strat->cp++;
	/* prod.crit itself in nc_spGSpolyCreate */
      }
      strat->P.p = nc_spGSpolyCreate(strat->P.p1,strat->P.p2,strat->kNoether,currRing);
    }
    if (strat->P.p != NULL)
    {
      if (TEST_OPT_PROT)
      message((strat->honey ? strat->P.ecart : 0) + strat->P.pFDeg(),
              &olddeg,&reduc,strat, red_result);
      /* reduction of the element chosen from L */
      strat->red(&strat->P,strat);
    }
    if (strat->P.p != NULL)
    {
          /* statistic */
          if (TEST_OPT_PROT) 
	  {
	    PrintS("s\n");
	  }
          /* enter P.p into s and L */
          {
            strat->P.sev=0;
            int pos=posInS(strat,strat->sl,strat->P.p, strat->P.ecart);
            {
              if (TEST_OPT_INTSTRATEGY)
              {
                if ((strat->syzComp==0)||(!strat->homog))
                {
                  /* strat->P.p = redtailBba(strat->P.p,pos-1,strat);   */                     }
		pCleardenom(strat->P.p);
              }
              else
              {
                pNorm(strat->P.p);
                if ((strat->syzComp==0)||(!strat->homog))
                {
                  //strat->P.p = redtailBba(strat->P.p,pos-1,strat);
                }
              }
	      // PLURAL debug
	      /* should be used only internally!!! */
              pWrite(strat->P.p);

              if (TEST_OPT_DEBUG)
              {
                PrintS("new s:");
                wrp(strat->P.p);
                PrintLn();
              }
              enterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat);
              if (strat->sl==-1) pos=0;
              else pos=posInS(strat,strat->sl,strat->P.p,strat->P.ecart);
              strat->enterS(strat->P,pos,strat);
            }
            if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);
          }
          if (strat->P.lcm!=NULL) pLmFree(strat->P.lcm);
      if (strat->sl>srmax) srmax = strat->sl;
    }
#ifdef KDEBUG
    strat->P.lcm=NULL;
#endif
    kTest(strat);
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
      omFreeSize((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }
  if (TEST_OPT_PROT) messageStat(srmax,lrmax,hilbcount,strat);
  if (Q!=NULL) updateResult(strat->Shdl,Q,strat);
  return (strat->Shdl);
}
#endif
