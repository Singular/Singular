/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kstd1.cc,v 1.43 1999-10-22 09:07:02 obachman Exp $ */
/*
* ABSTRACT:
*/

#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "polys.h"
#include "febase.h"
#include "kutil.h"
#include "kstd1.h"
#include "khstd.h"
#include "stairc.h"
#include "weight.h"
#include "cntrlc.h"
#include "intvec.h"
#include "ideals.h"
#include "ipshell.h"
#include "ipid.h"
#include "timer.h"
#include "lists.h"

//#include "ipprint.h"

/* the list of all options which give a warning by test */
BITSET kOptions=Sy_bit(0)|Sy_bit(1)|Sy_bit(3)|Sy_bit(4)|Sy_bit(5)
                |Sy_bit(8)|Sy_bit(10)|Sy_bit(21)
                |Sy_bit(OPT_INTSTRATEGY)
                |Sy_bit(30)|Sy_bit(31);

/* the list of all options which may be used by option and test */
BITSET validOpts=Sy_bit(0)|Sy_bit(1)|Sy_bit(2)|Sy_bit(3)
             |Sy_bit(4)|Sy_bit(5)|Sy_bit(6)|Sy_bit(7)
             |Sy_bit(8)|Sy_bit(9)|Sy_bit(10)
             |Sy_bit(11)|Sy_bit(12)|Sy_bit(13)|Sy_bit(14)|Sy_bit(15)
             |Sy_bit(16)|Sy_bit(17)|Sy_bit(18)|Sy_bit(19)|Sy_bit(20)
             |Sy_bit(21)
             |Sy_bit(22)
             /*|Sy_bit(23)*/
             /*|Sy_bit(24)*/
             |Sy_bit(OPT_REDTAIL)
             |Sy_bit(OPT_INTSTRATEGY)|Sy_bit(27)|Sy_bit(28)
             |Sy_bit(29)
             |Sy_bit(30)|Sy_bit(31);

//static int (*posInLOld)(LSet L,int Ll, LObject l,kStrategy strat);
//static int  lastAxis=0;
//static BOOLEAN update=TRUE;
//static BOOLEAN posInLOldFlag;
           /*FALSE, if posInL == posInL10*/

/*0 implementation*/

/*2
*p is a polynomial in the set s;
*recompute p and its ecart e with respect to the new noether
*(cut every monomial of pNext(p) above noether)
*/
void deleteHCs (TObject* p,kStrategy strat)
{
  poly p1;
  int o;

  if (strat->kHEdgeFound)
  {
    p1 = (*p).p;
    o = pFDeg(p1);
    while (pNext(p1) != NULL)
    {
      if (pComp0(pNext(p1),strat->kNoether) == -1)
      {
        pDelete(&(pNext(p1)));
        (*p).ecart = pLDeg((*p).p,&((*p).length))-o;
      }
      else
      {
        pIter(p1);
      }
    }
  }
}


void doRed (LObject* h,poly* with,BOOLEAN intoT,kStrategy strat)
{
  poly hp;
#ifdef KDEBUG
  pTest((*h).p);
  //pTest(*with);
#endif

  if (!TEST_OPT_INTSTRATEGY)
    pNorm(*with);
  if (TEST_OPT_DEBUG)
  {
    PrintS("reduce ");wrp((*h).p); PrintS(" with ");wrp(*with);PrintLn();
  }
  if (intoT)
  {
    hp = ksOldSpolyRedNew(*with,(*h).p,strat->kNoether);
    enterT(*h,strat);
    (*h).p = hp;
  }
  else
  {
    (*h).p = ksOldSpolyRed(*with,(*h).p,strat->kNoether);
  }
  if (TEST_OPT_DEBUG)
  {
    PrintS("to ");wrp((*h).p);PrintLn();
  }
}

/*2
* reduces h with elements from T choosing first possible
* element in T with respect to the given ecart
* requires thar T is sorted by ecart
*/
int redEcart19 (LObject* h,kStrategy strat)
{
  int i,at,reddeg,d;
  int j = 0;
  int pass = 0;
  unsigned long not_sev;
  h->sev = pGetShortExpVector(h->p);
  not_sev = ~ h->sev;

  if (TEST_OPT_CANCELUNIT) cancelunit(h);
  d = pFDeg((*h).p)+(*h).ecart;
  reddeg = strat->LazyDegree+d;
  loop
  {
    if (j > strat->tl)
    {
      return 1;
    }
    if (pShortDivisibleBy(strat->T[j].p, strat->T[j].sev, (*h).p, not_sev))
    {
      //if (strat->interpt) test_int_std(strat->kIdeal);
      /*- compute the s-polynomial -*/
      if (strat->T[j].ecart > (*h).ecart)
      {
        /*
        * It is not possible to reduce h with smaller ecart;
        * if possible h goes to the lazy-set L,i.e
        * if its position in L would be not the last one
        */
        strat->fromT = TRUE;
        if (strat->Ll >= 0) /*- L is not empty -*/
        {
          at = strat->posInL(strat->L,strat->Ll,(*h),strat);
          if (at <= strat->Ll)
          {
            /*- h will not become the next element to reduce -*/
            enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
            if (TEST_OPT_DEBUG) Print(" ecart too big; -> L%d\n",at);
            (*h).p = NULL;
            strat->fromT = FALSE;
            return -1;
          }
        }
        /*put the polynomial also in the pair set*/
        if (TEST_OPT_MOREPAIRS)
        {
          if (!TEST_OPT_INTSTRATEGY)
            pNorm((*h).p);
          enterpairs((*h).p,strat->sl,(*h).ecart,0,strat);
        }
      }
      doRed(h,&strat->T[j].p,strat->fromT,strat);
      strat->fromT=FALSE;
      if ((*h).p == NULL)
      {
        if (h->lcm!=NULL) pFree1((*h).lcm);
        return 0;
      }
      h->sev = pGetShortExpVector(h->p);
      not_sev = ~ h->sev;
      /*computes the ecart*/
      if (strat->honey)
      {
        if (strat->T[j].ecart <= (*h).ecart)
          (*h).ecart = d-pFDeg((*h).p);
        else
          (*h).ecart = d-pFDeg((*h).p)+strat->T[j].ecart-(*h).ecart;
        (*h).length = pLength((*h).p);
      }
      else
        (*h).ecart = pLDeg((*h).p,&((*h).length))-pFDeg((*h).p);
      if (TEST_OPT_CANCELUNIT) cancelunit(h);
      if ((strat->syzComp!=0) && !strat->honey)
      {
        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
        {
          if (TEST_OPT_DEBUG) PrintS(" > sysComp\n");
          return -2;
        }
      }
      /*- try to reduce the s-polynomial -*/
      pass++;
      d = pFDeg((*h).p)+(*h).ecart;
      /*
      *test whether the polynomial should go to the lazyset L
      *-if the degree jumps
      *-if the number of pre-defined reductions jumps
      */
      if ((strat->Ll >= 0) &&  ((d >= reddeg) || (pass > strat->LazyPass)))
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
                                      (*h).p, not_sev));
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
          if (TEST_OPT_DEBUG) Print(" degree jumped; ->L%d\n",at);
          (*h).p = NULL;
          return -1;
        }
      }
      else if ((TEST_OPT_PROT) && (strat->Ll < 0) && (d >= reddeg))
      {
        reddeg = d+1;
        Print(".%d",d);mflush();
      }
      j = 0;
    }
    else
    {
      j++;
    }
  }
}

/*2
* reduces h with elements from T choosing first possible
* element in T with respect to the given ecart
*/
int redEcart (LObject* h,kStrategy strat)
{
  poly pi;
  int i,at,reddeg,d,ei,li;
  int j = 0;
  int pass = 0;
  unsigned long not_sev;

  if (TEST_OPT_CANCELUNIT) cancelunit(h);
  d = pFDeg((*h).p)+(*h).ecart;
  reddeg = strat->LazyDegree+d;
  h->sev = pGetShortExpVector(h->p);
  not_sev = ~ h->sev;
  loop
  {
    if (j > strat->tl)
    {
      return 1;
    }
    if (pShortDivisibleBy(strat->T[j].p, strat->T[j].sev, (*h).p, not_sev))
    {
      //if (strat->interpt) test_int_std(strat->kIdeal);
      /*- compute the s-polynomial -*/
      pi = strat->T[j].p;
      ei = strat->T[j].ecart;
      li = strat->T[j].length;
      /*
      * the polynomial to reduce with (up to the moment) is;
      * pi with ecart ei and length li
      */
      i = j;
      loop
      {
      /*- takes the first possible with respect to ecart -*/
        if (ei <= (*h).ecart) break;
        i++;
        if (i > strat->tl) break;
        if ((((strat->T[i]).ecart < ei)
          || (((strat->T[i]).ecart == ei)
          && ((strat->T[i]).length < li)))
          && pShortDivisibleBy(strat->T[i].p, strat->T[i].sev,
                               (*h).p, not_sev))
        {
          /*
           * the polynomial to reduce with is now;
           */
          pi = strat->T[i].p;
          ei = strat->T[i].ecart;
          li = strat->T[i].length;
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
        strat->fromT = TRUE;
        if (strat->Ll >= 0) /*- L is not empty -*/
        {
          at = strat->posInL(strat->L,strat->Ll,(*h),strat);
          if (at <= strat->Ll)
          {
            /*- h will not become the next element to reduce -*/
            enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
            if (TEST_OPT_DEBUG) Print(" ecart too big; -> L%d\n",at);
            (*h).p = NULL;
            strat->fromT = FALSE;
            return -1;
          }
        }
        /*put the polynomial also in the pair set*/
        if (TEST_OPT_MOREPAIRS)
        {
          if (!TEST_OPT_INTSTRATEGY)
            pNorm((*h).p);
          enterpairs((*h).p,strat->sl,(*h).ecart,0,strat);
        }
      }
      doRed(h,&pi,strat->fromT,strat);
      strat->fromT=FALSE;
      if ((*h).p == NULL)
      {
        if (h->lcm!=NULL) pFree1((*h).lcm);
        return 0;
      }
      h->sev = pGetShortExpVector(h->p);
      not_sev = ~ h->sev;
      /*computes the ecart*/
      if (strat->honey)
      {
        if (ei <= (*h).ecart)
          (*h).ecart = d-pFDeg((*h).p);
        else
          (*h).ecart = d-pFDeg((*h).p)+ei-(*h).ecart;
        pLDeg((*h).p,&((*h).length));
        //(*h).length = pLength((*h).p);
      }
      else
        (*h).ecart = pLDeg((*h).p,&((*h).length))-pFDeg((*h).p);
      if (TEST_OPT_CANCELUNIT) cancelunit(h);
      if (strat->syzComp!=0)
      {
        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
        {
          if (TEST_OPT_DEBUG) PrintS(" > sysComp\n");
          return -2;
        }
      }
      /*- try to reduce the s-polynomial -*/
      pass++;
      d = pFDeg((*h).p)+(*h).ecart;
      /*
      *test whether the polynomial should go to the lazyset L
      *-if the degree jumps
      *-if the number of pre-defined reductions jumps
      */
      if ((strat->Ll >= 0)
      && ((d >= reddeg) || (pass > strat->LazyPass)))
      {
        at = strat->posInL(strat->L,strat->Ll,*h,strat);
        if (at <= strat->Ll)
        {
          i=strat->sl+1;
          do
          {
            i--;
            if (i<0) return 1;
          } while (!pShortDivisibleBy(strat->S[i], strat->sevS[i], 
                                      (*h).p, not_sev));
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
          if (TEST_OPT_DEBUG) Print(" degree jumped; ->L%d\n",at);
          (*h).p = NULL;
          return -1;
        }
      }
      else if ((TEST_OPT_PROT) && (strat->Ll < 0) && (d >= reddeg))
      {
        Print(".%d",d);mflush();
        reddeg = d+1;
      }
      j = 0;
    }
    else
    {
      j++;
    }
  }
}

/*2
*reduces h with elements from T choosing  the first possible
* element in t with respect to the given pDivisibleBy
*/
int redFirst (LObject* h,kStrategy strat)
{
  int at,reddeg,d,i;
  int pass = 0;
  int j = 0;

  if (h->p == NULL) return 0;
  if (TEST_OPT_CANCELUNIT) cancelunit(h);
  d = pFDeg((*h).p)+(*h).ecart;
  reddeg = strat->LazyDegree+d;
  unsigned long not_sev;
  h->sev = pGetShortExpVector(h->p);
  not_sev = ~ h->sev;
  loop
  {
    if (j > strat->tl)
    {
      assume(h->sev == pGetShortExpVector(h->p));
      return 1;
    }
    if (pShortDivisibleBy(strat->T[j].p, strat->T[j].sev, (*h).p, not_sev))
    {
      //if (strat->interpt) test_int_std(strat->kIdeal);
      /*
      * the polynomial to reduce with is;
      * T[j].p
      */
      if (!TEST_OPT_INTSTRATEGY)
        pNorm(strat->T[j].p);
      if (TEST_OPT_DEBUG)
      {
        PrintS("reduce ");
        wrp(h->p);
        PrintS(" with ");
        wrp(strat->T[j].p);
      }
      (*h).p = ksOldSpolyRed(strat->T[j].p,(*h).p,strat->kNoether);
      if (TEST_OPT_DEBUG)
      {
        PrintS(" to ");
        wrp(h->p);
        PrintLn();
      }
      if ((*h).p == NULL)
      {
        if (h->lcm!=NULL) pFree1((*h).lcm);
        h->sev = 0;
        return 0;
      }
      h->sev = pGetShortExpVector(h->p);
      not_sev = ~ h->sev;
      if (TEST_OPT_CANCELUNIT) cancelunit(h);
      /*computes the ecart*/
      d = pLDeg((*h).p,&((*h).length));
      (*h).ecart = d-pFDeg((*h).p);
      if ((strat->syzComp!=0) && !strat->honey)
      {
        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
        {
          if (TEST_OPT_DEBUG) PrintS(" > sysComp\n");
          return -2;
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
        at = strat->posInL(strat->L,strat->Ll,*h,strat);
        if (at <= strat->Ll)
        {
          i=strat->sl+1;
          do
          {
            i--;
            if (i<0) return 1;
          } while (!pShortDivisibleBy(strat->S[i],strat->sevS[i], 
                                      (*h).p, not_sev));
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
          if (TEST_OPT_DEBUG) Print(" degree jumped; ->L%d\n",at);
          (*h).p = NULL;
          h->sev = 0;
          return 0;
        }
      }
      if ((TEST_OPT_PROT) && (strat->Ll < 0) && (d >= reddeg))
      {
        reddeg = d+1;
        Print(".%d",d);mflush();
      }
      j = 0;
    }
    else
    {
      j++;
    }
  }
}

/*2
* reduces h with elements from T choosing the best possible
* element in t with respect to the ecart and length
*/
int redMoraBest (LObject* h,kStrategy strat)
{
  poly pi;
  int reddeg,d,ei,li,i,at;
  int j = 0;
  int pass = 0;

  if (TEST_OPT_CANCELUNIT) cancelunit(h);
  d = pFDeg((*h).p)+(*h).ecart;
  reddeg = strat->LazyDegree+d;
  unsigned long not_sev;
  h->sev = pGetShortExpVector(h->p);
  not_sev = ~ h->sev;
  loop
  {
    if (j > strat->tl)
    {
      return 1;
    }
    if (pShortDivisibleBy(strat->T[j].p, strat->T[j].sev, (*h).p, not_sev))
    {
      //if (strat->interpt) test_int_std(strat->kIdeal);
      /*- compute the s-polynomial -*/
      pi = strat->T[j].p;
      ei = strat->T[j].ecart;
      li = strat->T[j].length;
      /*
      * the polynomial to reduce with (up to the moment) is;
      * pi with ecart ei and length li
      */
      i = j;
      loop
      {
        /*- takes the best possible with respect to ecart and length -*/
        i++;
        if (i > strat->tl) break;
        if (((strat->T[i].ecart < ei)
          || ((strat->T[i].ecart == ei)
        && (strat->T[i].length < li)))
            && pShortDivisibleBy(strat->T[i].p, strat->T[i].sev, 
                                 (*h).p, not_sev))
        {
          /*
          * the polynomial to reduce with is now:
          */
          pi = strat->T[i].p;
          ei = strat->T[i].ecart;
          li = strat->T[i].length;
        }
      }
      /*
      * end of search: best is pi
      */
      if ((ei > (*h).ecart) && (!strat->kHEdgeFound))
      {
        /*
        * It is not possible to reduce h with smaller ecart;
        * if possible h goes to the lazy-set L,i.e
        * if its position in L would be not the last one
        */
        strat->fromT = TRUE;
        if (strat->Ll >= 0) /*- L is not empty -*/
        {
          at = strat->posInL(strat->L,strat->Ll,(*h),strat);
          if (at <= strat->Ll)
          {
            /*- h will not become the next element to reduce -*/
            enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
            if (TEST_OPT_DEBUG) Print(" ecart too big; -> L%d\n",at);
            (*h).p = NULL;
            strat->fromT = FALSE;
            return -1;
          }
        }
        /*put the polynomial also in the pair set*/
        if (TEST_OPT_MOREPAIRS)
        {
          if (!TEST_OPT_INTSTRATEGY)
            pNorm((*h).p);
          enterpairs((*h).p,strat->sl,(*h).ecart,0,strat);
        }
      }
      doRed(h,&pi,strat->fromT,strat);
      strat->fromT=FALSE;
      if ((*h).p == NULL)
      {
        if (h->lcm!=NULL) pFree1((*h).lcm);
        return 0;
      }
      h->sev = pGetShortExpVector(h->p);
      not_sev = ~ h->sev;
      /*computes the ecart*/
      if (strat->honey)
      {
        if (ei <= (*h).ecart)
          (*h).ecart = d-pFDeg((*h).p);
        else
          (*h).ecart = d-pFDeg((*h).p)+ei-(*h).ecart;
        (*h).length = pLength((*h).p);
      }
      else
        (*h).ecart = pLDeg((*h).p,&((*h).length))-pFDeg((*h).p);
      if (TEST_OPT_CANCELUNIT) cancelunit(h);
      if ((strat->syzComp!=0) && !strat->honey)
      {
        if ((strat->syzComp>0) && (pMinComp((*h).p) > strat->syzComp))
        {
          if (TEST_OPT_DEBUG) PrintS(" > sysComp\n");
          return -2;
        }
      }
      /*- try to reduce the s-polynomial -*/
      pass++;
      d = pFDeg((*h).p)+(*h).ecart;
      /*
      *test whether the polynomial should go to the lazyset L
      *-if the degree jumps
      *-if the number of pre-defined reductions jumps
      */
      if ((strat->Ll >= 0)
      && ((d >= reddeg) || (pass > strat->LazyPass)))
      {
        at = strat->posInL(strat->L,strat->Ll,*h,strat);
        if (at <= strat->Ll)
        {
          i=strat->sl+1;
          do
          {
            i--;
            if (i<0) return 1;
          } while (!pShortDivisibleBy(strat->S[i],strat->sevS[i], 
                                      (*h).p, not_sev));
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
          if (TEST_OPT_DEBUG) Print(" degree jumped; ->L%d\n",at);
          (*h).p = NULL;
          return -1;
        }
      }
      else if ((TEST_OPT_PROT) && (strat->Ll < 0) && (d >= reddeg))
      {
        reddeg = d+1;
        Print(".%d",d);mflush();
      }
      j = 0;
    }
    else
    {
      j++;
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
  poly pi;
  LObject H;
  int o,ei,li;
  int j = 0;
  int z = 10;
  unsigned long not_sev;
  H.p = h;
  o = pFDeg(h);
  H.ecart = pLDeg(H.p,&H.length)-o;
  if (flag==0) cancelunit(&H);
  H.sev = pGetShortExpVector(H.p);
  not_sev = ~ H.sev;
  loop
  {
    if (j > strat->tl)
    {
      return H.p;
    }
    if (pShortDivisibleBy(strat->T[j].p, strat->T[j].sev, H.p, not_sev))
    {
      //if (strat->interpt) test_int_std(strat->kIdeal);
      /*- remember the found T-poly -*/
      pi = strat->T[j].p;
      ei = strat->T[j].ecart;
      li = strat->T[j].length;
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
        && pShortDivisibleBy(strat->T[j].p,strat->T[j].sev, H.p, not_sev))
        {
          /*
          * the polynomial to reduce with is now;
          */
          pi = strat->T[j].p;
          ei = strat->T[j].ecart;
          li = strat->T[j].length;
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
        * we have to reduce with bad ecart: H has t enter in T
        */
        doRed(&H,&pi,TRUE,strat);
        if (H.p == NULL)
        {
          return NULL;
        }
      }
      else
      {
        /*
        * we reduce with good ecart, h need not to be put to T
        */
        doRed(&H,&pi,FALSE,strat);
        if (H.p == NULL)
        {
          return NULL;
        }
      }
      /*- try to reduce the s-polynomial -*/
      o = pFDeg(H.p);
      cancelunit(&H);
      H.ecart = pLDeg(H.p,&(H.length))-o;
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
    at = strat->posInL(strat->L,i-1,strat->L[i],strat);
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

  for (i=1; i<=strat->tl; i++)
  {
    if (strat->T[i-1].length > strat->T[i].length)
    {
      p = strat->T[i];
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
      }
      strat->T[at+1]=p;
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

/*2
*last is the only non used axis, it looks
*for a monomial in p being a pure power of this
*variable and returns TRUE in this case
*(*length) gives the length between the pure power and the leading term
*(should be minimal)
*/
BOOLEAN hasPurePower (poly p,int last, int *length,kStrategy strat)
{
  poly h;
  int i;

  if (pNext(p) == strat->tail)
    return FALSE;
  if (pMinComp(p) == strat->ak)
  {
    *length = 0;
    h = p;
    while (h != NULL)
    {
      i = pIsPurePower(h);
      if (i==last) return TRUE;
      (*length)++;
      pIter(h);
    }
  }
  return FALSE;
}

/*2
* looks up the position of polynomial p in L
* in the case of looking for the pure powers
*/
int posInL10 (LSet const set, int length, const LObject &p,kStrategy const strat)
{
  int j,dp,dL;

  if (length<0) return 0;
  if (hasPurePower(p.p,strat->lastAxis,&dp,strat))
  {
    int op= pFDeg(p.p)+p.ecart;
    for (j=length; j>=0; j--)
    {
      if (!hasPurePower(set[j].p,strat->lastAxis,&dL,strat))
        return j+1;
      if (dp < dL)
        return j+1;
      if ((dp == dL)
      && (pFDeg(set[j].p)+set[j].ecart >= op))
        return j+1;
    }
  }
  j=length;
  loop
  {
    if (j<0) break;
    if (!hasPurePower(set[j].p,strat->lastAxis,&dL,strat)) break;
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
    if (hasPurePower(strat->L[j].p,strat->lastAxis,&dL,strat))
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
        pFree1(strat->L[j].p);    /*deletes the short spoly and computes*/
        strat->L[j].p=ksOldCreateSpoly(strat->L[j].p1,
                                    strat->L[j].p2,
                                    strat->kNoether);   /*the real one*/
        if (!strat->honey)
          strat->initEcart(&strat->L[j]);
        else
          strat->L[j].length = pLength(strat->L[j].p);
        if (hasPurePower(strat->L[j].p,strat->lastAxis,&dL,strat))
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
  while (i <= strat->Ll)
  {
    if (pNext(strat->L[i].p) == strat->tail)
    {
       /*- deletes the int spoly and computes -*/
      if (pComp0(strat->L[i].p,strat->kNoether) == -1)
      {
        pFree1(strat->L[i].p);
        strat->L[i].p = NULL;
      }
      else
      {
        pFree1(strat->L[i].p);
        strat->L[i].p = ksOldCreateSpoly(strat->L[i].p1,
                                         strat->L[i].p2,
                                         strat->kNoether);
        
                                      
        strat->L[i].ecart = pLDeg(strat->L[i].p,&strat->L[i].length)-pFDeg(strat->L[i].p);
      }
    }
    else
      deleteHC(&strat->L[i].p,&strat->L[i].ecart,&strat->L[i].length,strat);
   if (strat->L[i].p == NULL)
      deleteInL(strat->L,&strat->Ll,i,strat);
    else
      i++;
  }
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
    deleteHCs(&strat->T[i],strat);
    /*- tries to cancel a unit: -*/
    p.p = strat->T[i].p;
    p.ecart = strat->T[i].ecart;
    p.length = strat->T[i].length;
    cancelunit(&p);
    strat->T[i].p = p.p;
    strat->T[i].ecart = p.ecart;
    strat->T[i].length = p.length;
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
    strat->update = (strat->tl == -1);
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
    if (TEST_OPT_FASTHC)
    {
      strat->posInL = strat->posInLOld;
      strat->lastAxis = 0;
    }
    if (BTEST1(27))
      return;
    if (!BTEST1(20))        /*- take the first possible -*/
      strat->red = redFirst;
    updateT(strat);
    strat->posInT = posInT2;
    reorderT(strat);
  }
}

/*2
*-puts p to the standardbasis s at position at
*-reduces the tail of p if TEST_OPT_REDTAIL
*-tries to cancel a unit
*-HEckeTest
*  if TRUE
*  - decides about reduction-strategies
*  - computes noether
*  - stops computation if BTEST1(27)
*  - cuts the tails of the polynomials
*    in s,t and the elements in L above noether
*    and cancels units if possible
*  - reorders s,L
*/
void enterSMora (LObject p,int atS,kStrategy strat)
{
  int i;

  strat->news = TRUE;
  /*- puts p to the standardbasis s at position atS -*/
  if (strat->sl == IDELEMS(strat->Shdl)-1)
  {
    pEnlargeSet(&strat->S,IDELEMS(strat->Shdl),setmax);
    strat->sevS = (unsigned long*) ReAlloc0(strat->sevS,
                                    IDELEMS(strat->Shdl)*sizeof(unsigned long),
                                    (IDELEMS(strat->Shdl)+setmax)
                                           *sizeof(unsigned long));
    strat->ecartS = (intset) ReAlloc(strat->ecartS,
                                     IDELEMS(strat->Shdl)*sizeof(int),
                                     (IDELEMS(strat->Shdl)+setmax)*sizeof(int));
    if (strat->fromQ)
    {
      strat->fromQ = (intset)ReAlloc(strat->fromQ,
                                    IDELEMS(strat->Shdl)*sizeof(int),
                                    (IDELEMS(strat->Shdl)+setmax)*sizeof(int));
    }
    IDELEMS(strat->Shdl) += setmax;
    strat->Shdl->m=strat->S;
  }
  for (i=strat->sl+1; i>atS; i--)
  {
    strat->S[i] = strat->S[i-1];
    strat->ecartS[i] = strat->ecartS[i-1];
    strat->sevS[i] = strat->sevS[i-1];
  }
  if (strat->fromQ)
  {
    for (i=strat->sl+1; i>=atS+1; i--)
    {
      strat->fromQ[i] = strat->fromQ[i-1];
    }
    strat->fromQ[atS]=0;
  }
  /*- save result -*/
  strat->S[atS] = p.p;
  strat->ecartS[atS] = p.ecart;
  if (p.sev == 0) p.sev = pGetShortExpVector(p.p);
  else assume(p.sev == pGetShortExpVector(p.p));
  strat->sevS[atS] = p.sev;
  strat->sl++;
  if (TEST_OPT_DEBUG)
  {
    Print("new s%d:",atS);
    wrp(p.p);
    PrintLn();
  }
  if ((!strat->kHEdgeFound) || (strat->kNoether!=NULL)) HEckeTest(p.p,strat);
  if (strat->kHEdgeFound)
  {
    if (newHEdge(strat->S,strat->ak,strat))
    {
      firstUpdate(strat);
      if (BTEST1(27))
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
void enterSMoraNF (LObject p, int atS,kStrategy strat)
{
  int i;

  /*- puts p to the standardbasis s at position at -*/
  if (strat->sl == IDELEMS(strat->Shdl)-1)
  {
    pEnlargeSet(&strat->S,IDELEMS(strat->Shdl),setmax);
    strat->sevS = (unsigned long*) ReAlloc0(strat->sevS,
                                    IDELEMS(strat->Shdl)*sizeof(unsigned long),
                                    (IDELEMS(strat->Shdl)+setmax)
                                           *sizeof(unsigned long));
    strat->ecartS=(intset)ReAlloc(strat->ecartS,
                                  IDELEMS(strat->Shdl)*sizeof(intset),
                                  (IDELEMS(strat->Shdl)+setmax)*sizeof(intset));
    IDELEMS(strat->Shdl) += setmax;
    strat->Shdl->m=strat->S;
  }
  for (i=strat->sl+1; i>atS; i--)
  {
    strat->S[i] = strat->S[i-1];
    strat->ecartS[i] = strat->ecartS[i-1];
    strat->sevS[i] = strat->sevS[i-1];
  }
  strat->S[atS] = p.p;/*- save result -*/
  strat->ecartS[atS] = p.ecart;
  if (p.sev == 0) p.sev = pGetShortExpVector(p.p);
  else assume(p.sev == pGetShortExpVector(p.p));
  strat->sevS[atS] = p.sev;
  strat->sl++;
  if ((!strat->kHEdgeFound) || (strat->kNoether!=NULL)) HEckeTest(p.p,strat);
  if (strat->kHEdgeFound)
    newHEdge(strat->S,strat->ak,strat);
  else if (strat->kNoether!=NULL)
    strat->kHEdgeFound = TRUE;
}

void initMora(ideal F,kStrategy strat)
{
  int i,j;
  idhdl h;

  strat->NotUsedAxis = (BOOLEAN *)Alloc((pVariables+1)*sizeof(BOOLEAN));
  for (j=pVariables; j>0; j--) strat->NotUsedAxis[j] = TRUE;
  strat->enterS = enterSMora;
  strat->initEcartPair = initEcartPairMora; /*- ecart approximation -*/
  strat->posInLOld = strat->posInL;
  strat->posInLOldFlag = TRUE;
  strat->initEcart = initEcartNormal;
  strat->kHEdgeFound = ppNoether != NULL;
  if ( strat->kHEdgeFound )
     strat->kNoether = pCopy(ppNoether);
  if (BTEST1(20))
    strat->red = redMoraBest;/*- look for the best in T -*/
  else if (strat->kHEdgeFound || strat->homog)
    strat->red = redFirst;  /*take the first possible in T*/
  else if (BTEST1(19))
    strat->red = redEcart19;/*take the first possible in T, rquires T sorted by ecart*/
  else
    strat->red = redEcart;/*take the first possible in under ecart-restriction*/
  if (strat->kHEdgeFound)
  {
    strat->HCord = pFDeg(ppNoether)+1;
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

ideal mora (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat)
{
  int srmax;
  int lrmax = 0;
  int olddeg = 0;
  int reduc = 0;
  int hilbeledeg=1,hilbcount=0;

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
  /*- compute-------------------------------------------*/
  while (strat->Ll >= 0)
  {
    if (lrmax< strat->Ll) lrmax=strat->Ll; /*stat*/
    //test_int_std(strat->kIdeal);
    if (TEST_OPT_DEBUG) messageSets(strat);
    if (TEST_OPT_DEGBOUND
    && (strat->L[strat->Ll].ecart+pFDeg(strat->L[strat->Ll].p)> Kstd1_deg))
    {
      /*
      * stops computation if
      * - 24 (degBound)
      *   && upper degree is bigger than Kstd1_deg
      */
      while ((strat->Ll >= 0)
        && (strat->L[strat->Ll].ecart+pFDeg(strat->L[strat->Ll].p)> Kstd1_deg)
        && (strat->L[strat->Ll].p1!=NULL) && (strat->L[strat->Ll].p2!=NULL))
      {
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
        //if (TEST_OPT_PROT)
        //{
        //   PrintS("D"); mflush();
        //}
      }
      if (strat->Ll<0) break;
    }
    strat->P = strat->L[strat->Ll];/*- picks the last element from the lazyset L -*/
    if (strat->Ll==0) strat->interpt=TRUE;
    strat->Ll--;
    if (pNext(strat->P.p) == strat->tail)
    {
      pFree1(strat->P.p);/*- deletes the short spoly and computes -*/
      strat->P.p = ksOldCreateSpoly(strat->P.p1,
                                    strat->P.p2,
                                    strat->kNoether);/*- the real one -*/
      if (!strat->honey)
        strat->initEcart(&strat->P);
      else
        strat->P.length = pLength(strat->P.p);
    }
    {
      if (TEST_OPT_PROT) message(strat->P.ecart+pFDeg(strat->P.p),&olddeg,&reduc,strat);
      strat->red(&strat->P,strat);/*- reduction of the element choosen from L -*/
    }
    if (strat->P.p != NULL)
    {
      assume(strat->P.sev == 0 || strat->P.sev == pGetShortExpVector(strat->P.p));
          if (TEST_OPT_PROT) PrintS("s");/*- statistic -*/
          /*- enter P.p into s and b: -*/
          if (!TEST_OPT_INTSTRATEGY)
          {
            pNorm(strat->P.p);
          }
          strat->P.p = redtail(strat->P.p,strat->sl,strat);
          if ((!strat->noTailReduction) && (!strat->honey))
          {
            strat->initEcart(&strat->P);
          }
          if (TEST_OPT_INTSTRATEGY)
          {
            //pContent(strat->P.p);
            pCleardenom(strat->P.p);// also does a pContent
          }
          cancelunit(&strat->P);/*- tries to cancel a unit -*/
          enterT(strat->P,strat);
          {
            int pos;
            {
              enterpairs(strat->P.p,strat->sl,strat->P.ecart,0,strat);
              if (strat->sl==-1)
              {
                pos=0;
              }
              else
              {
                pos = posInS(strat->S,strat->sl,strat->P.p);
              }
              strat->enterS(strat->P,pos,strat);
            }
            if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);
          }
          if (strat->P.lcm!=NULL) pFree1(strat->P.lcm);
          strat->P.lcm=NULL;
#ifdef KDEBUG
      memset(&strat->P,0,sizeof(strat->P));
#endif
      if (strat->sl>srmax) srmax = strat->sl; /*stat.*/
      if (strat->Ll>lrmax) lrmax = strat->Ll;
    }
    if (strat->kHEdgeFound)
    {
      if ((BTEST1(27))
      || ((TEST_OPT_MULTBOUND) && (scMult0Int((strat->Shdl)) < mu)))
      {
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
  }
  /*- complete reduction of the standard basis------------------------ -*/
  if (TEST_OPT_REDSB) completeReduce(strat);
  /*- release temp data------------------------------- -*/
  exitBuchMora(strat);
  /*- polynomials used for HECKE: HC, noether -*/
  if (BTEST1(27))
  {
    if (strat->kHEdge!=NULL)
      Kstd1_mu=pFDeg(strat->kHEdge);
    else
      Kstd1_mu=-1;
  }
  pDelete(&strat->kHEdge);
  strat->update = TRUE; //???
  strat->lastAxis = 0; //???
  pDelete(&strat->kNoether);
  Free((ADDRESS)strat->NotUsedAxis,(pVariables+1)*sizeof(BOOLEAN));
  if (TEST_OPT_PROT) messageStat(srmax,lrmax,hilbcount,strat);
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
  idTest(strat->Shdl);
  return (strat->Shdl);
}

poly kNF1 (ideal F,ideal Q,poly q, kStrategy strat, int lazyReduce)
{
  poly   p;
  int   i;
  int   j;
  int   o;
  LObject   h;
  BITSET save_test=test;

  if ((idIs0(F))&&(Q==NULL))
    return pCopy(q); /*F=0*/
  strat->ak = idRankFreeModule(F);
  /*- creating temp data structures------------------- -*/
  strat->kHEdgeFound = ppNoether != NULL;
  strat->kNoether    = pCopy(ppNoether);
  test|=Sy_bit(OPT_REDTAIL);
  test&=~Sy_bit(OPT_INTSTRATEGY);
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
  strat->tmax = setmax;
  strat->T = initT();
  /*- set S -*/
  strat->sl = -1;
  /*- init local data struct.-------------------------- -*/
  /*Shdl=*/initS(F,Q,strat);
  if ((lazyReduce & 1)==0)
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
    enterT(h,strat);
  }
  /*- compute------------------------------------------- -*/
  p = pCopy(q);
  deleteHC(&p,&o,&j,strat);
  if (TEST_OPT_PROT) { PrintS("r"); mflush(); }
  if (p!=NULL) p = redMoraNF(p,strat, lazyReduce & 2);
  if ((p!=NULL)&&((lazyReduce & 1)==0))
  {
    if (TEST_OPT_PROT) { PrintS("t"); mflush(); }
    p = redtail(p,strat->sl,strat);
  }
  /*- release temp data------------------------------- -*/
  cleanT(strat);
  Free((ADDRESS)strat->T,strat->tmax*sizeof(TObject));
  Free((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  Free((ADDRESS)strat->sevS,IDELEMS(strat->Shdl)*sizeof(unsigned long));
  Free((ADDRESS)strat->NotUsedAxis,(pVariables+1)*sizeof(BOOLEAN));
  if ((Q!=NULL)&&(strat->fromQ!=NULL))
  {
    i=((IDELEMS(Q)+IDELEMS(F)+15)/16)*16;
    Free((ADDRESS)strat->fromQ,i*sizeof(int));
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
      Free((ADDRESS *)&ecartWeights,(pVariables+1)*sizeof(short));
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
  poly   p;
  int   i;
  int   j;
  int   o;
  LObject   h;
  ideal res;
  BITSET save_test=test;

  if (idIs0(q)) return idInit(1,q->rank);
  if ((idIs0(F))&&(Q==NULL))
    return idCopy(q); /*F=0*/
  strat->ak = idRankFreeModule(F);
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
  strat->tmax = setmax;
  strat->T = initT();
  /*- set S -*/
  strat->sl = -1;
  /*- init local data struct.-------------------------- -*/
  /*Shdl=*/initS(F,Q,strat);
  if (TEST_OPT_INTSTRATEGY && ((lazyReduce & 1)==0))
  {
    for (i=strat->sl; i>=0; i--)
      pNorm(strat->S[i]);
  }
  /*- compute------------------------------------------- -*/
  res=idInit(IDELEMS(q),q->rank);
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
          h.length = pLength(h.p);
          if (strat->sevS[j] == 0) strat->sevS[j] = pGetShortExpVector(h.p);
          else assume(strat->sevS[j] == pGetShortExpVector(h.p));
          h.sev = strat->sevS[j];
          h.length = pLength(h.p);
          enterT(h,strat);
        }
        if (TEST_OPT_PROT) { PrintS("r"); mflush(); }
        p = redMoraNF(p,strat, lazyReduce & 2);
        if ((p!=NULL)&&((lazyReduce & 1)==0))
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
  Free((ADDRESS)strat->T,strat->tmax*sizeof(TObject));
  Free((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  Free((ADDRESS)strat->sevS,IDELEMS(strat->Shdl)*sizeof(unsigned long));
  Free((ADDRESS)strat->NotUsedAxis,(pVariables+1)*sizeof(BOOLEAN));
  if ((Q!=NULL)&&(strat->fromQ!=NULL))
  {
    i=((IDELEMS(Q)+IDELEMS(F)+15)/16)*16;
    Free((ADDRESS)strat->fromQ,i*sizeof(int));
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
      Free((ADDRESS *)&ecartWeights,(pVariables+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }
  idDelete(&strat->Shdl);
  test=save_test;
  if (TEST_OPT_PROT) PrintLn();
  return res;
}

pFDegProc pOldFDeg;
intvec * kModW, * kHomW;

int kModDeg(poly p)
{
  int o=pWDegree(p);
  int i=pGetComp(p);
  if (i==0) return o;
  return o+(*kModW)[i-1];
}
int kHomModDeg(poly p)
{
  int i;
  int j=0;

  for (i=pVariables;i>0;i--)
    j+=pGetExp(p,i)*(*kHomW)[i-1];
  if (kModW == NULL) return j;
  i = pGetComp(p);
  if (i==0) return j;
  return j+(*kModW)[i-1];
}
ideal kStd(ideal F, ideal Q, tHomog h,intvec ** w, intvec *hilb,int syzComp,
          int newIdeal, intvec *vw)
{
  ideal r;
  BOOLEAN b=pLexOrder,toReset=FALSE;
  BOOLEAN delete_w=(w==NULL);
  kStrategy strat=(kStrategy)Alloc0SizeOf(skStrategy);

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
    pOldFDeg = pFDeg;
    pFDeg = kHomModDeg;
    toReset = TRUE;
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
  pLexOrder=b;
  if (h==isHomog)
  {
    if ((w!=NULL) && (*w!=NULL))
    {
      strat->kModW = kModW = *w;
      if (vw == NULL)
      {
        pOldFDeg = pFDeg;
        pFDeg = kModDeg;
        toReset = TRUE;
      }
    }
    pLexOrder = TRUE;
    if (hilb==NULL) strat->LazyPass*=2;
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
  idTest(r);
#endif
  if (toReset)
  {
    kModW = NULL;
    pFDeg = pOldFDeg;
  }
  pLexOrder = b;
//Print("%d reductions canceled \n",strat->cel);
  HCord=strat->HCord;
  kFreeStrat(strat);
  if ((delete_w)&&(w!=NULL)&&(*w!=NULL)) delete *w;
  return r;
}

//##############################################################
//##############################################################
//##############################################################
//##############################################################
//##############################################################

lists min_std(ideal F, ideal Q, tHomog h,intvec ** w, intvec *hilb,int syzComp,
              int reduced)
{
  ideal r=NULL;
  int Kstd1_OldDeg,i;
  intvec* temp_w=NULL;
  BOOLEAN b=pLexOrder,toReset=FALSE;
  BOOLEAN delete_w=(w==NULL);
  BOOLEAN oldDegBound=TEST_OPT_DEGBOUND;
  kStrategy strat=(kStrategy)Alloc0SizeOf(skStrategy);

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
    if ((w!=NULL) && (*w!=NULL))
    {
      kModW = *w;
      strat->kModW = *w;
      pOldFDeg = pFDeg;
      pFDeg = kModDeg;
      toReset = TRUE;
      if (reduced>1)
      {
        Kstd1_OldDeg=Kstd1_deg;
        Kstd1_deg = -1;
        for (i=IDELEMS(F)-1;i>=0;i--)
        {
          if ((F->m[i]!=NULL) && (pFDeg(F->m[i])>=Kstd1_deg))
            Kstd1_deg = pFDeg(F->m[i])+1;
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
    for (i=0; i<IDELEMS(r); i++) pTest(r->m[i]);
  }
#endif
  idSkipZeroes(r);
  if (toReset)
  {
    kModW = NULL;
    pFDeg = pOldFDeg;
  }
  pLexOrder = b;
  HCord=strat->HCord;
  if ((delete_w)&&(temp_w!=NULL)) delete temp_w;
  lists l=(lists)AllocSizeOf(slists);
  l->Init(2);
  if (strat->ak==0)
  {
    l->m[0].rtyp=IDEAL_CMD;
    l->m[1].rtyp=IDEAL_CMD;
  }
  else
  {
    l->m[0].rtyp=MODUL_CMD;
    l->m[1].rtyp=MODUL_CMD;
  }
  l->m[0].data=(void *)r;
  setFlag(&(l->m[0]),FLAG_STD);
  if (strat->M==NULL)
  {
    l->m[1].data=(void *)idInit(1,F->rank);
    Warn("no minimal generating set computed");
  }
  else
  {
    idSkipZeroes(strat->M);
    l->m[1].data=(void *)strat->M;
  }
  kFreeStrat(strat);
  if (reduced>2)
  {
    Kstd1_deg=Kstd1_OldDeg;
    if (!oldDegBound)
      test &= ~Sy_bit(OPT_DEGBOUND);
  }
  return l;
}

poly kNF(ideal F, ideal Q, poly p,int syzComp, int lazyReduce)
{
  if (p==NULL)
     return NULL;
  kStrategy strat=(kStrategy)Alloc0SizeOf(skStrategy);
  strat->syzComp = syzComp;
  if (pOrdSgn==-1)
    p=kNF1(F,Q,p,strat,lazyReduce);
  else
    p=kNF2(F,Q,p,strat,lazyReduce);
  kFreeStrat(strat);
  return p;
}

ideal kNF(ideal F, ideal Q, ideal p,int syzComp,int lazyReduce)
{
  ideal res;
  if (TEST_OPT_PROT)
  {
    Print("(S:%d)",IDELEMS(p));mflush();
  }
  kStrategy strat=(kStrategy)Alloc0SizeOf(skStrategy);
  strat->syzComp = syzComp;
  if (pOrdSgn==-1)
    res=kNF1(F,Q,p,strat,lazyReduce);
  else
    res=kNF2(F,Q,p,strat,lazyReduce);
  kFreeStrat(strat);
  return res;
}

/*2
*interreduces F
*/
ideal kInterRed (ideal F, ideal Q)
{
  int j;
  kStrategy strat = (kStrategy)Alloc0SizeOf(skStrategy);

//  if (TEST_OPT_PROT)
//  {
//    writeTime("start InterRed:");
//    mflush();
//  }
  //strat->syzComp     = 0;
  strat->kHEdgeFound = ppNoether != NULL;
  strat->kNoether=pCopy(ppNoether);
  strat->ak = idRankFreeModule(F);
  initBuchMoraCrit(strat);
  strat->NotUsedAxis = (BOOLEAN *)Alloc((pVariables+1)*sizeof(BOOLEAN));
  for (j=pVariables; j>0; j--) strat->NotUsedAxis[j] = TRUE;
  strat->enterS      = enterSBba;
  strat->posInT      = posInT0;
  strat->initEcart   = initEcartNormal;
  strat->sl   = -1;
  strat->tl          = -1;
  strat->tmax        = setmax;
  strat->T           = initT();
  if (pOrdSgn == -1)   strat->honey = TRUE;
  initS(F,Q,strat);
  if (TEST_OPT_REDSB)
    strat->noTailReduction=FALSE;
  updateS(TRUE,strat);
  if (TEST_OPT_REDSB && TEST_OPT_INTSTRATEGY)
    completeReduce(strat);
  pDelete(&strat->kHEdge);
  Free((ADDRESS)strat->T,strat->tmax*sizeof(TObject));
  Free((ADDRESS)strat->ecartS,IDELEMS(strat->Shdl)*sizeof(int));
  Free((ADDRESS)strat->sevS,IDELEMS(strat->Shdl)*sizeof(unsigned long));
  Free((ADDRESS)strat->NotUsedAxis,(pVariables+1)*sizeof(BOOLEAN));
  if (strat->fromQ)
  {
    for (j=0;j<IDELEMS(strat->Shdl);j++)
    {
      if(strat->fromQ[j]) pDelete(&strat->Shdl->m[j]);
    }
    Free((ADDRESS)strat->fromQ,IDELEMS(strat->Shdl)*sizeof(int));
    strat->fromQ=NULL;
    idSkipZeroes(strat->Shdl);
  }
//  if (TEST_OPT_PROT)
//  {
//    writeTime("end Interred:");
//    mflush();
//  }
  ideal shdl=strat->Shdl;
  kFreeStrat(strat);
  return shdl;
}
