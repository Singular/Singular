/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT -  Kernel: noncomm. alg. of Buchberger
*/
#define PLURAL_INTERNAL_DECLARATIONS

#include "kernel/mod2.h"

#ifdef HAVE_PLURAL

#include "misc/options.h"
#include "misc/intvec.h"

#include "polys/weight.h"
#include "kernel/polys.h"
#include "polys/monomials/ring.h"

#include "polys/nc/gb_hack.h"
#include "polys/nc/nc.h"
#include "polys/nc/sca.h"


#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/khstd.h"
//#include "spolys.h"
//#include "cntrlc.h"
#include "kernel/GBEngine/ratgring.h"
#include "kernel/GBEngine/kutil.h"

#include "kernel/GBEngine/nc.h"

#if 0
/*3
* reduction of p2 with p1
* do not destroy p1 and p2
* p1 divides p2 -> for use in NF algorithm
*/
poly gnc_ReduceSpolyNew(const poly p1, poly p2/*,poly spNoether*/, const ring r)
{
  return(nc_ReduceSPoly(p1,p_Copy(p2,r)/*,spNoether*/,r));
}
#endif

/*2
*reduces h with elements from T choosing  the first possible
* element in t with respect to the given pDivisibleBy
*/
int redGrFirst (LObject* h,kStrategy strat)
{
  int at,reddeg,d,i;
  int pass = 0;
  int j = 0;

  d = currRing->pFDeg((*h).p,currRing)+(*h).ecart;
  reddeg = strat->LazyDegree+d;
  loop
  {
    if (j > strat->sl)
    {
#ifdef KDEBUG
      if (TEST_OPT_DEBUG) PrintLn();
#endif
      return 0;
    }
#ifdef KDEBUG
    if (TEST_OPT_DEBUG) Print("%d",j);
#endif
    if (pDivisibleBy(strat->S[j],(*h).p))
    {
#ifdef KDEBUG
      if (TEST_OPT_DEBUG) PrintS("+\n");
#endif
      /*
      * the polynomial to reduce with is;
      * T[j].p
      */
      if (!TEST_OPT_INTSTRATEGY)
        pNorm(strat->S[j]);
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        wrp(h->p);
        PrintS(" with ");
        wrp(strat->S[j]);
      }
#endif
      (*h).p = nc_ReduceSpoly(strat->S[j],(*h).p, currRing);
      //spSpolyRed(strat->T[j].p,(*h).p,strat->kNoether);

#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS(" to ");
        wrp(h->p);
      }
#endif
      if ((*h).p == NULL)
      {
        kDeleteLcm(h);
        return 0;
      }
      if (TEST_OPT_INTSTRATEGY)
      {
        h->pCleardenom();// also removes Content
      }
      /*computes the ecart*/
      d = currRing->pLDeg((*h).p,&((*h).length),currRing);
      (*h).FDeg=currRing->pFDeg((*h).p,currRing);
      (*h).ecart = d-(*h).FDeg; /*pFDeg((*h).p);*/
      if ((strat->syzComp!=0) && !strat->honey)
      {
        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
        {
#ifdef KDEBUG
          if (TEST_OPT_DEBUG) PrintS(" > sysComp\n");
#endif
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
#ifdef KDEBUG
          if (TEST_OPT_DEBUG) Print(" degree jumped; ->L%d\n",at);
#endif
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
#ifdef KDEBUG
      if TEST_OPT_DEBUG PrintLn();
#endif
    }
    else
    {
#ifdef KDEBUG
      if (TEST_OPT_DEBUG) PrintS("-");
#endif
      j++;
    }
  }
}
void ratGB_divide_out(poly p)
{
  /* extracts monomial content from localized expression  */
  /* searches for an m (monomial in var 1.. real_var_start-1)
   * such that m divides p and divides p by this m if it exist*/
  if (p==NULL) return;
  poly root=p;
  assume(rIsRatGRing(currRing));
  poly f=pHead(p);
  int i;
  for (i=currRing->real_var_start;i<=currRing->real_var_end;i++)
  {
    pSetExp(f,i,0);
  }
  loop
  {
    pIter(p);
    if (p==NULL) { pSetm(f); break;}
    for (i=1;i<=rVar(currRing);i++)
    {
      pSetExp(f,i,si_min(pGetExp(f,i),pGetExp(p,i)));
    }
  }
  if (!pIsConstant(f))
  {
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("divide out:");p_wrp(f,currRing);
      PrintS(" from ");pWrite(root);
    }
#endif
    p=root;
    loop
    {
      if (p==NULL) break;
      for (i=1;i<=rVar(currRing);i++)
      {
        pSetExp(p,i,pGetExp(p,i)-pGetExp(f,i));
      }
      pSetm(p);
      pIter(p);
    }
  }
  pDelete(&f);
}

#ifdef HAVE_RATGRING
/*2
*reduces h with elements from T choosing  the first possible
* element in t with respect to the given pDivisibleBy
* for use in ratGB
*/
int redGrRatGB (LObject* h,kStrategy strat)
{
  int at,reddeg,d,i;
  int pass = 0;
  int j = 0;
  int c_j=-1, c_e=-2;
  poly c_p=NULL;
  assume(strat->tailRing==currRing);

  ratGB_divide_out((*h).p);
  d = currRing->pFDeg((*h).p,currRing)+(*h).ecart;
  reddeg = strat->LazyDegree+d;
  if (!TEST_OPT_INTSTRATEGY)
  {
    h->pCleardenom();// also does a pContentRat
  }
  loop
  {
    if (j > strat->sl)
    {
      if (c_j>=0)
      {
        /*
        * the polynomial to reduce with is;
        * S[c_j]
        */
        if (!TEST_OPT_INTSTRATEGY)
          pNorm(strat->S[c_j]);
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
        if (TEST_OPT_DEBUG)
        {
          wrp(h->p);
          Print(" with S[%d]= ",c_j);
          wrp(strat->S[c_j]);
        }
#endif
    //poly hh = nc_CreateSpoly(strat->S[c_j],(*h).p, currRing);
    //        Print("vor nc_rat_ReduceSpolyNew (ce:%d) ",c_e);wrp(h->p);PrintLn();
    //if(c_e==-1)
    //  c_p = nc_CreateSpoly(pCopy(strat->S[c_j]),pCopy((*h).p), currRing);
    //else
    //          c_p=nc_rat_ReduceSpolyNew(strat->S[c_j],pCopy((*h).p), currRing->real_var_start-1,currRing);
    //        Print("nach nc_rat_ReduceSpolyNew ");wrp(c_p);PrintLn();
    //        pDelete(&((*h).p));

        c_p=nc_rat_ReduceSpolyNew(strat->S[c_j],(*h).p, currRing->real_var_start-1,currRing);
        (*h).p=c_p;
        if (!TEST_OPT_INTSTRATEGY)
        {
          h->pCleardenom();// also removes Content
        }

#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
        {
          PrintS(" to ");
          wrp(h->p);
          PrintLn();
        }
#endif
        if ((*h).p == NULL)
        {
          kDeleteLcm(h);
          return 0;
        }
        ratGB_divide_out((*h).p);
        d = currRing->pLDeg((*h).p,&((*h).length),currRing);
        (*h).FDeg=currRing->pFDeg((*h).p,currRing);
        (*h).ecart = d-(*h).FDeg; /*pFDeg((*h).p);*/
        /*- try to reduce the s-polynomial again -*/
        pass++;
        j=0;
        c_j=-1; c_e=-2; c_p=NULL;
      }
      else
      { // nothing found
        return 0;
      }
    }
    // first try usual division
    if (p_LmDivisibleBy(strat->S[j],(*h).p,currRing))
    {
#ifdef KDEBUG
      if(TEST_OPT_DEBUG)
      {
        p_wrp(h->p,currRing); Print(" divisible by S[%d]=",j);
        p_wrp(strat->S[j],currRing); PrintS(" e=-1\n");
      }
#endif
      if ((c_j<0)||(c_e>=0))
      {
        c_e=-1; c_j=j;
      }
    }
    else
    if (p_LmDivisibleByPart(strat->S[j],(*h).p,currRing,
        currRing->real_var_start,currRing->real_var_end))
    {
      int a_e=(p_Totaldegree(strat->S[j],currRing)-currRing->pFDeg(strat->S[j],currRing));
#ifdef KDEBUG
      if(TEST_OPT_DEBUG)
      {
        p_wrp(h->p,currRing); Print(" divisibly by S[%d]=",j);
        p_wrp(strat->S[j],currRing); Print(" e=%d\n",a_e);
      }
#endif
      if ((c_j<0)||(c_e>a_e))
      {
        c_e=a_e; c_j=j;
        //c_p = nc_CreateSpoly(pCopy(strat->S[c_j]),pCopy((*h).p), currRing);
      }
      /*computes the ecart*/
      if ((strat->syzComp!=0) && !strat->honey)
      {
        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
        {
#ifdef KDEBUG
          if (TEST_OPT_DEBUG) PrintS(" > sysComp\n");
#endif
          return 0;
        }
      }
    }
    else
    {
#ifdef KDEBUG
      if(TEST_OPT_DEBUG)
      {
        p_wrp(h->p,currRing); Print(" not divisibly by S[%d]=",j);
        p_wrp(strat->S[j],currRing); PrintLn();
      }
#endif
    }
    j++;
  }
}
#endif

/*2
*  reduction procedure for the homogeneous case
*  and the case of a degree-ordering
*/
#if 0
// currently unused
static int nc_redHomog (LObject* h,kStrategy strat)
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
      (*h).p = nc_ReduceSpoly(strat->S[j],(*h).p,currRing);
      if ((*h).p == NULL)
      {
        if (TEST_OPT_DEBUG) PrintS(" to 0\n");
        kDeleteLcm(h);
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
#endif

#if 0
/*2
*  reduction procedure for the homogeneous case
*  and the case of a degree-ordering
*/
static int nc_redHomog0 (LObject* h,kStrategy strat)
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
      (*h).p = nc_ReduceSpoly(strat->T[j].p,(*h).p,strat->kNoether,currRing);
      if ((*h).p == NULL)
      {
        if (TEST_OPT_DEBUG) PrintS(" to 0\n");
        kDeleteLcm(h);
        return 0;
      }
      else
      {
        if (TEST_OPT_INTSTRATEGY)
        {
          h->pCleardenom();// also removes Content
        }
        if (strat->syzComp!=0)
        {
          if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
          {
/*
*           (*h).length=pLength0((*h).p);
*/
            enterT((*h),strat);
            return 0;
          }
        }
      }
      /*- try to reduce the s-polynomial -*/
      j = 0;
    }
    else
    {
      if (j >= strat->tl)
      {
        if (TEST_OPT_INTSTRATEGY)
        {
          h->pCleardenom();// also removes Content
        }
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
static int nc_redLazy (LObject* h,kStrategy strat)
{
  if (strat->tl<0)
  {
    enterT((*h),strat);
    return 0;
  }

  int at,d,i;
  int j = 0;
  int pass = 0;
  int reddeg = currRing->pFDeg((*h).p,currRing);

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
      (*h).p = nc_ReduceSpoly(strat->S[j],(*h).p,strat->kNoether,currRing);
      if ((*h).p == NULL)
      {
        if (TEST_OPT_DEBUG) PrintS(" to 0\n");
        kDeleteLcm(h);
        return 0;
      }
//      else if (strat->syzComp)
//      {
//        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
//        {
//          if (TEST_OPT_DEBUG) PrintS(" > syzComp\n");
//          if (TEST_OPT_INTSTRATEGY) p_Content(h->p,currRing);
//          enterTBba((*h),strat->tl+1,strat);
//          return;
//        }
//      }
      else
      {
        if (TEST_OPT_DEBUG)
        {
          PrintS("to:");
          wrp((*h).p);
          PrintLn();
        }
        if (TEST_OPT_INTSTRATEGY)
        {
          pCleardenom(h->p);// also removes Content
        }
      }
      /*- try to reduce the s-polynomial -*/
      pass++;
      d = currRing->pFDeg((*h).p,currRing);
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
          h->pCleardenom();// also removes Content
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
static int nc_redHoney (LObject*  h,kStrategy strat)
{
  if (strat->tl<0)
  {
    enterT((*h),strat);
    return 0;
  }

  poly pi;
  int i,j,at,reddeg,d,pass,ei;

  pass = j = 0;
  d = reddeg = currRing->pFDeg((*h).p,currRing)+(*h).ecart;
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
        (*h).p = nc_ReduceSpoly(pi,(*h).p,strat->kNoether,currRing);
      }
      else
        (*h).p = nc_ReduceSpoly(pi,(*h).p,strat->kNoether,currRing);
      if (TEST_OPT_DEBUG)
      {
        PrintS(" to ");
        wrp((*h).p);
        PrintLn();
      }
      if ((*h).p == NULL)
      {
        kDeleteLcm(h);
        return 0;
      }
      if (TEST_OPT_INTSTRATEGY)
      {
        h->pCleardenom();// also does remove Content
      }
      /* compute the ecart */
      if (ei <= (*h).ecart)
        (*h).ecart = d-currRing->pFDeg((*h).p,currRing);
      else
        (*h).ecart = d-currRing->pFDeg((*h).p,currRing)+ei-(*h).ecart;
//      if (strat->syzComp)
//      {
//        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
//        {
//          if (TEST_OPT_DEBUG)
//            PrintS("  >syzComp\n");
//          if (TEST_OPT_INTSTRATEGY) p_Content(h->p,currRing);
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
      d = currRing->pFDeg((*h).p,currRing)+(*h).ecart;
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
          h->pCleardenom();// also does remove Content
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
static int nc_redBest (LObject*  h,kStrategy strat)
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
    reddeg = currRing->pFDeg((*h).p,currRing)+(*h).ecart;
  else
    reddeg = currRing->pFDeg((*h).p,currRing);
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
      p = nc_CreateShortSpoly(strat->T[j].p,(*h).p);
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
              ph = nc_CreateShortSpoly(strat->T[j].p,(*h).p);
              if (ph==NULL)
              {
                pLmFree(p);
                pDelete(&((*h).p));
                kDeleteLcm(h);
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
        (*h).p = nc_ReduceSpoly(strat->T[jbest].p,(*h).p,strat->kNoether,currRing);
      }
      else
      {
        kDeleteLcm(h);
        return 0;
      }
      if (strat->honey && currRing->pLexOrder)
        strat->initEcart(h);
      /* h.length:=l; */
      /* try to reduce the s-polynomial */
//      if (strat->syzComp)
//      {
//        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
//        {
//          if (TEST_OPT_DEBUG)
//            PrintS(" >syzComp\n");
//          if (TEST_OPT_INTSTRATEGY) p_Content(h->p,currRing);
//          at=strat->posInT(strat->T,strat->tl,(*h));
//          enterTBba((*h),at,strat);
//          return;
//        }
//      }
      if (strat->honey || currRing->pLexOrder)
      {
        pass++;
        d = currRing->pFDeg((*h).p,currRing);
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
          h->pCleardenom();// also removes Content
        }
        enterT((*h),strat);
        return 0;
      }
      j++;
    }
  }
}

#endif

#ifdef HAVE_RATGRING
void nc_gr_initBba(ideal F, kStrategy strat)
#else
void nc_gr_initBba(ideal, kStrategy strat)
#endif
{
  assume(rIsPluralRing(currRing));

  // int i;
//  idhdl h;
 /* setting global variables ------------------- */
  strat->enterS = enterSBba;

/*
  if ((BTEST1(20)) && (!strat->honey))
    strat->red = nc_redBest;
  else if (strat->honey)
    strat->red = nc_redHoney;
  else if (currRing->pLexOrder && !strat->homog)
    strat->red = nc_redLazy;
  else if (TEST_OPT_INTSTRATEGY && strat->homog)
    strat->red = nc_redHomog0;
  else
    strat->red = nc_redHomog;
*/

//   if (rIsPluralRing(currRing))
    strat->red = redGrFirst;
#ifdef HAVE_RATGRING
  if (rIsRatGRing(currRing))
  {
    int ii=IDELEMS(F)-1;
    int jj;
    BOOLEAN is_rat_id=FALSE;
    for(;ii>=0;ii--)
    {
      for(jj=currRing->real_var_start;jj<=currRing->real_var_end;jj++)
      {
        if(pGetExp(F->m[ii],jj)>0) { is_rat_id=TRUE; break; }
      }
      if (is_rat_id) break;
    }
    if (is_rat_id) strat->red=redGrRatGB;
  }
#endif

  if (currRing->pLexOrder && strat->honey)
    strat->initEcart = initEcartNormal;
  else
    strat->initEcart = initEcartBBA;
  if (strat->honey)
    strat->initEcartPair = initEcartPairMora;
  else
    strat->initEcartPair = initEcartPairBba;
//  if ((TEST_OPT_WEIGHTM)&&(F!=NULL))
//  {
//     //interred  machen   Aenderung
//     pFDegOld=currRing->pFDeg;
//     pLDegOld=currRing->pLDeg;
//  //   h=ggetid("ecart");
//  //   if ((h!=NULL) && (IDTYP(h)==INTVEC_CMD))
//  //   {
//  //     ecartWeights=iv2array(IDINTVEC(h));
//  //   }
//  //   else
//    {
//      ecartWeights=(short *)omAlloc(((currRing->N)+1)*sizeof(short));
//      /*uses automatic computation of the ecartWeights to set them*/
//      kEcartWeights(F->m,IDELEMS(F)-1,ecartWeights);
//    }
//    currRing->pFDeg=totaldegreeWecart;
//    currRing->pLDeg=maxdegreeWecart;
//    for(i=1; i<=(currRing->N); i++)
//      Print(" %d",ecartWeights[i]);
//    PrintLn();
//    mflush();
//  }
}

#define MYTEST 0

ideal k_gnc_gr_bba(const ideal F, const ideal Q, const intvec *, const bigintmat *, kStrategy strat, const ring _currRing)
{
  const ring save = currRing; if( currRing != _currRing ) rChangeCurrRing(_currRing);

#if MYTEST
   PrintS("<gnc_gr_bba>\n");
#endif

#ifdef HAVE_PLURAL
#if MYTEST
   PrintS("currRing: \n");
   rWrite(currRing);
#ifdef RDEBUG
   rDebugPrint(currRing);
#endif

   PrintS("F: \n");
   idPrint(F);
   PrintS("Q: \n");
   idPrint(Q);
#endif
#endif

  assume(currRing->OrdSgn != -1); // no mora!!! it terminates only for global ordering!!! (?)// alternate algebra?

  // intvec *w=NULL;
  // intvec *hilb=NULL;
  int   olddeg,reduc;
  int red_result=1;
  int /*hilbeledeg=1,*/hilbcount=0/*,minimcnt=0*/;

  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  // initHilbCrit(F,Q,&hilb,strat);
  /* in plural we don't need Hilb yet */
  nc_gr_initBba(F,strat);
  initBuchMoraPos(strat);
  if (rIsRatGRing(currRing))
  {
    strat->posInL=posInL0; // by pCmp of lcm
  }
  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  /*Shdl=*/initBuchMora(F, Q,strat);
  strat->posInT=posInT110;
  reduc = olddeg = 0;

  /* compute------------------------------------------------------- */
  while (strat->Ll >= 0)
  {
    if (TEST_OPT_DEBUG) messageSets(strat);

    if (strat->Ll== 0) strat->interpt=TRUE;
    if (TEST_OPT_DEGBOUND
    && ((strat->honey
    && (strat->L[strat->Ll].ecart+currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
       || ((!strat->honey) && (currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))))
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
    //kTest(strat);

    if (strat->P.p != NULL)
    if (pNext(strat->P.p) == strat->tail)
    {
      /* deletes the short spoly and computes */
      pLmFree(strat->P.p);
      /* the real one */
//      if (ncRingType(currRing)==nc_lie) /* prod crit */
//        if(pHasNotCF(strat->P.p1,strat->P.p2))
//        {
//          strat->cp++;
//          /* prod.crit itself in nc_CreateSpoly */
//        }


      if( ! rIsRatGRing(currRing) )
      {
        strat->P.p = nc_CreateSpoly(strat->P.p1,strat->P.p2,currRing);
      }
#ifdef HAVE_RATGRING
      else
      {
        /* rational case */
        strat->P.p = nc_rat_CreateSpoly(strat->P.p1,strat->P.p2,currRing->real_var_start-1,currRing);
      }
#endif


#ifdef PDEBUG
      p_Test(strat->P.p, currRing);
#endif

#if MYTEST
      if (TEST_OPT_DEBUG)
      {
        PrintS("p1: "); pWrite(strat->P.p1);
        PrintS("p2: "); pWrite(strat->P.p2);
        PrintS("SPoly: "); pWrite(strat->P.p);
      }
#endif
    }


    if (strat->P.p != NULL)
    {
      if (TEST_OPT_PROT)
        message((strat->honey ? strat->P.ecart : 0) + strat->P.pFDeg(),
              &olddeg,&reduc,strat, red_result);

#if MYTEST
      if (TEST_OPT_DEBUG)
      {
        PrintS("p1: "); pWrite(strat->P.p1);
        PrintS("p2: "); pWrite(strat->P.p2);
        PrintS("SPoly before: "); pWrite(strat->P.p);
      }
#endif

      /* reduction of the element chosen from L */
      strat->red(&strat->P,strat);

#if MYTEST
      if (TEST_OPT_DEBUG)
      {
        PrintS("red SPoly: "); pWrite(strat->P.p);
      }
#endif
    }
    if (strat->P.p != NULL)
    {
      if (TEST_OPT_PROT)
      {
        PrintS("s\n");
      }
      /* enter P.p into s and L */
      {
/* quick unit detection in the rational case */
#ifdef HAVE_RATGRING
        if( rIsRatGRing(currRing) )
        {
          if ( p_LmIsConstantRat(strat->P.p, currRing) )
          {
#ifdef PDEBUG
             PrintS("unit element detected:");
             p_wrp(strat->P.p,currRing);
#endif
            p_Delete(&strat->P.p,currRing, strat->tailRing);
            strat->P.p = pOne();
          }
      }
#endif
        strat->P.sev=0;
        int pos=posInS(strat,strat->sl,strat->P.p, strat->P.ecart);
        {
          if (TEST_OPT_INTSTRATEGY)
          {
            if ((strat->syzComp==0)||(!strat->homog))
            {
              #ifdef HAVE_RATGRING
              if(!rIsRatGRing(currRing))
              #endif
                strat->P.p = redtailBba(strat->P.p,pos-1,strat);
            }

            strat->P.p=p_Cleardenom(strat->P.p, currRing);
          }
          else
          {
            pNorm(strat->P.p);
            if ((strat->syzComp==0)||(!strat->homog))
            {
              strat->P.p = redtailBba(strat->P.p,pos-1,strat);
            }
          }
          if (TEST_OPT_DEBUG)
          {
            PrintS("new s:"); wrp(strat->P.p);
            PrintLn();
#if MYTEST
            PrintS("s: "); pWrite(strat->P.p);
#endif

          }
          // kTest(strat);
          //
          enterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat);

          if (strat->sl==-1) pos=0;
          else pos=posInS(strat,strat->sl,strat->P.p,strat->P.ecart);

          strat->enterS(strat->P,pos,strat,-1);
        }
//      if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);
      }
      kDeleteLcm(&strat->P);
    }
    //kTest(strat);
  }
  if (TEST_OPT_DEBUG) messageSets(strat);

  /* complete reduction of the standard basis--------- */
  if (TEST_OPT_SB_1)
  {
    int k=1;
    int j;
    while(k<=strat->sl)
    {
      j=0;
      loop
      {
        if (j>=k) break;
        clearS(strat->S[j],strat->sevS[j],&k,&j,strat);
        j++;
      }
      k++;
    }
  }

  if (TEST_OPT_REDSB)
     completeReduce(strat);
  /* release temp data-------------------------------- */
  exitBuchMora(strat);
//  if (TEST_OPT_WEIGHTM)
//  {
//    currRing->pFDeg=pFDegOld;
//    currRing->pLDeg=pLDegOld;
//    if (ecartWeights)
//    {
//      omFreeSize((ADDRESS)ecartWeights,((currRing->N)+1)*sizeof(short));
//      ecartWeights=NULL;
//    }
//  }
  if (TEST_OPT_PROT) messageStat(hilbcount,strat);
  if (Q!=NULL) updateResult(strat->Shdl,Q,strat);


#if MYTEST
  PrintS("</gnc_gr_bba>\n");
#endif

  if( currRing != save )     rChangeCurrRing(save);

  return (strat->Shdl);
}

ideal k_gnc_gr_mora(const ideal F, const ideal Q, const intvec *, const bigintmat *, kStrategy strat, const ring _currRing)
{
  if ((ncRingType(_currRing)==nc_skew)
  || id_HomIdeal(F,Q,_currRing))
    return gnc_gr_bba(F, Q, NULL, NULL, strat, _currRing);
  else
  {
    WerrorS("not implemented: std for inhomogeneous ideasl in local orderings");
    return NULL;
  }
}

#endif

