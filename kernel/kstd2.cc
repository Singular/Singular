/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT -  Kernel: alg. of Buchberger
*/

// #define PDEBUG 2

// TODO: why the following is here instead of mod2.h???

// define to enable tailRings
#define HAVE_TAIL_RING

#include <kernel/mod2.h>

#ifndef NDEBUG
# define MYTEST 0
#else /* ifndef NDEBUG */
# define MYTEST 0
#endif /* ifndef NDEBUG */

#if MYTEST
# ifdef HAVE_TAIL_RING
#  undef HAVE_TAIL_RING
# endif // ifdef HAVE_TAIL_RING
#endif

// define if no buckets should be used
// #define NO_BUCKETS

#ifdef HAVE_PLURAL
#define PLURAL_INTERNAL_DECLARATIONS 1
#endif
#include <kernel/kutil.h>
#include <kernel/options.h>
#include <omalloc/omalloc.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <kernel/febase.h>
#include <kernel/kstd1.h>
#include <kernel/khstd.h>
#include <kernel/kbuckets.h>
//#include "cntrlc.h"
#include <kernel/weight.h>
#include <kernel/intvec.h>
#ifdef HAVE_PLURAL
#include <kernel/gring.h>
#endif
// #include "timer.h"

/* shiftgb stuff */
#include <kernel/shiftgb.h>

  int (*test_PosInT)(const TSet T,const int tl,LObject &h);
  int (*test_PosInL)(const LSet set, const int length,
                LObject* L,const kStrategy strat);

// return -1 if no divisor is found
//        number of first divisor, otherwise
int kFindDivisibleByInT(const TSet &T, const unsigned long* sevT,
                        const int tl, const LObject* L, const int start)
{
  unsigned long not_sev = ~L->sev;
  int j = start;
  poly p=L->p;
  ring r=currRing;
  if (p==NULL)  { r=L->tailRing; p=L->t_p; }
  L->GetLm(p, r);

  pAssume(~not_sev == p_GetShortExpVector(p, r));

  if (r == currRing)
  {
    loop
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
    loop
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
}

// same as above, only with set S
int kFindDivisibleByInS(const kStrategy strat, int* max_ind, LObject* L)
{
  unsigned long not_sev = ~L->sev;
  poly p = L->GetLmCurrRing();
  int j = 0;

  pAssume(~not_sev == p_GetShortExpVector(p, currRing));
#if 1
  int ende;
  if ((strat->ak>0) || pLexOrder) ende=strat->sl;
  else ende=posInS(strat,*max_ind,p,0)+1;
  if (ende>(*max_ind)) ende=(*max_ind);
#else
  int ende=strat->sl;
#endif
  (*max_ind)=ende;
  loop
  {
    if (j > ende) return -1;
#if defined(PDEBUG) || defined(PDIV_DEBUG)
    if (p_LmShortDivisibleBy(strat->S[j], strat->sevS[j],
                             p, not_sev, currRing))
        return j;
#else
    if ( !(strat->sevS[j] & not_sev) &&
         p_LmDivisibleBy(strat->S[j], p, currRing))
      return j;
#endif
    j++;
  }
}

int kFindNextDivisibleByInS(const kStrategy strat, int start,int max_ind, LObject* L)
{
  unsigned long not_sev = ~L->sev;
  poly p = L->GetLmCurrRing();
  int j = start;

  pAssume(~not_sev == p_GetShortExpVector(p, currRing));
#if 1
  int ende=max_ind;
#else
  int ende=strat->sl;
#endif
  loop
  {
    if (j > ende) return -1;
#if defined(PDEBUG) || defined(PDIV_DEBUG)
    if (p_LmShortDivisibleBy(strat->S[j], strat->sevS[j],
                             p, not_sev, currRing))
        return j;
#else
    if ( !(strat->sevS[j] & not_sev) &&
         p_LmDivisibleBy(strat->S[j], p, currRing))
      return j;
#endif
    j++;
  }
}

#ifdef HAVE_RINGS
poly kFindZeroPoly(poly input_p, ring leadRing, ring tailRing)
{
  // m = currRing->ch

  if (input_p == NULL) return NULL;

  poly p = input_p;
  poly zeroPoly = NULL;
  NATNUMBER a = (NATNUMBER) pGetCoeff(p);

  int k_ind2 = 0;
  int a_ind2 = ind2(a);

  NATNUMBER k = 1;
  // of interest is only k_ind2, special routine for improvement ... TODO OLIVER
  for (int i = 1; i <= leadRing->N; i++)
  {
    k_ind2 = k_ind2 + ind_fact_2(p_GetExp(p, i, leadRing));
  }

  a = (NATNUMBER) pGetCoeff(p);

  number tmp1;
  poly tmp2, tmp3;
  poly lead_mult = p_ISet(1, tailRing);
  if (leadRing->ch <= k_ind2 + a_ind2)
  {
    int too_much = k_ind2 + a_ind2 - leadRing->ch;
    int s_exp;
    zeroPoly = p_ISet(a, tailRing);
    for (int i = 1; i <= leadRing->N; i++)
    {
      s_exp = p_GetExp(p, i,leadRing);
      if (s_exp % 2 != 0)
      {
        s_exp = s_exp - 1;
      }
      while ( (0 < ind2(s_exp)) && (ind2(s_exp) <= too_much) )
      {
        too_much = too_much - ind2(s_exp);
        s_exp = s_exp - 2;
      }
      p_SetExp(lead_mult, i, p_GetExp(p, i,leadRing) - s_exp, tailRing);
      for (NATNUMBER j = 1; j <= s_exp; j++)
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
    p_Setm(lead_mult, tailRing);
    zeroPoly = p_Mult_mm(zeroPoly, lead_mult, tailRing);
    tmp2 = p_NSet(nCopy(pGetCoeff(zeroPoly)), leadRing);
    for (int i = 1; i <= leadRing->N; i++)
    {
      pSetExp(tmp2, i, p_GetExp(zeroPoly, i, tailRing));
    }
    p_Setm(tmp2, leadRing);
    zeroPoly = p_LmDeleteAndNext(zeroPoly, tailRing);
    pNext(tmp2) = zeroPoly;
    return tmp2;
  }
/*  NATNUMBER alpha_k = twoPow(leadRing->ch - k_ind2);
  if (1 == 0 && alpha_k <= a)
  {  // Temporarly disabled, reducing coefficients not compatible with std TODO Oliver
    zeroPoly = p_ISet((a / alpha_k)*alpha_k, tailRing);
    for (int i = 1; i <= leadRing->N; i++)
    {
      for (NATNUMBER j = 1; j <= p_GetExp(p, i, leadRing); j++)
      {
        tmp1 = nInit(j);
        tmp2 = p_ISet(1, tailRing);
        p_SetExp(tmp2, i, 1, tailRing);
        p_Setm(tmp2, tailRing);
        if (nIsZero(tmp1))
        {
          zeroPoly = p_Mult_q(zeroPoly, tmp2, tailRing);
        }
        else
        {
          tmp3 = p_ISet((NATNUMBER) tmp1, tailRing);
          zeroPoly = p_Mult_q(zeroPoly, p_Add_q(tmp2, tmp3, tailRing), tailRing);
        }
      }
    }
    tmp2 = p_ISet((NATNUMBER) pGetCoeff(zeroPoly), leadRing);
    for (int i = 1; i <= leadRing->N; i++)
    {
      pSetExp(tmp2, i, p_GetExp(zeroPoly, i, tailRing));
    }
    p_Setm(tmp2, leadRing);
    zeroPoly = p_LmDeleteAndNext(zeroPoly, tailRing);
    pNext(tmp2) = zeroPoly;
    return tmp2;
  } */
  return NULL;
}
#endif


#ifdef HAVE_RINGS
/*2
*  reduction procedure for the ring Z/2^m
*/
int redRing (LObject* h,kStrategy strat)
{
  if (h->IsNull()) return 0; // spoly is zero (can only occure with zero divisors)
  if (strat->tl<0) return 1;

  int at,i;
  long d;
  int j = 0;
  int pass = 0;
  poly zeroPoly = NULL;

// TODO warum SetpFDeg notwendig?
  h->SetpFDeg();
  assume(h->pFDeg() == h->FDeg);
  long reddeg = h->GetpFDeg();

  h->SetShortExpVector();
  loop
  {
    j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h);
    if (j < 0) return 1;

    ksReducePoly(h, &(strat->T[j]), NULL, NULL, strat); // with debug output

    if (h->GetLmTailRing() == NULL)
    {
      if (h->lcm!=NULL) pLmDelete(h->lcm);
#ifdef KDEBUG
      h->lcm=NULL;
#endif
      h->Clear();
      return 0;
    }
    h->SetShortExpVector();
    d = h->SetpFDeg();
    /*- try to reduce the s-polynomial -*/
    pass++;
    if (!TEST_OPT_REDTHROUGH &&
        (strat->Ll >= 0) && ((d > reddeg) || (pass > strat->LazyPass)))
    {
      h->SetLmCurrRing();
      if (strat->posInLDependsOnLength)
        h->SetLength(strat->length_pLength);
      at = strat->posInL(strat->L,strat->Ll,h,strat);
      if (at <= strat->Ll)
      {
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) Print(" ->L[%d]\n",at);
#endif
        enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);     // NOT RING CHECKED OLIVER
        h->Clear();
        return -1;
      }
    }
    if (d != reddeg)
    {
      if (d >= strat->tailRing->bitmask)
      {
        if (h->pTotalDeg() >= strat->tailRing->bitmask)
        {
          strat->overflow=TRUE;
          //Print("OVERFLOW in redRing d=%ld, max=%ld\n",d,strat->tailRing->bitmask);
          h->GetP();
          at = strat->posInL(strat->L,strat->Ll,h,strat);
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
          h->Clear();
          return -1;
        }
      }
      else if ((TEST_OPT_PROT) && (strat->Ll < 0))
      {
        Print(".%ld",d);mflush();
        reddeg = d;
      }
    }
  }
}
#endif

/*2
*  reduction procedure for the homogeneous case
*  and the case of a degree-ordering
*/
int redHomog (LObject* h,kStrategy strat)
{
  if (strat->tl<0) return 1;
  //if (h->GetLmTailRing()==NULL) return 0; // HS: SHOULD NOT BE NEEDED!
  assume(h->FDeg == h->pFDeg());

  poly h_p;
  int i,j,at,pass, ii;
  unsigned long not_sev;
  long reddeg,d;

  pass = j = 0;
  d = reddeg = h->GetpFDeg();
  h->SetShortExpVector();
  int li;
  h_p = h->GetLmTailRing();
  not_sev = ~ h->sev;
  loop
  {
    j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h);
    if (j < 0) return 1;

    li = strat->T[j].pLength;
    ii = j;
    /*
     * the polynomial to reduce with (up to the moment) is;
     * pi with length li
     */
    i = j;
#if 1
    if (TEST_OPT_LENGTH)
    loop
    {
      /*- search the shortest possible with respect to length -*/
      i++;
      if (i > strat->tl)
        break;
      if (li<=1)
        break;
      if ((strat->T[i].pLength < li)
         &&
          p_LmShortDivisibleBy(strat->T[i].GetLmTailRing(), strat->sevT[i],
                               h_p, not_sev, strat->tailRing))
      {
        /*
         * the polynomial to reduce with is now;
         */
        li = strat->T[i].pLength;
        ii = i;
      }
    }
#endif

    /*
     * end of search: have to reduce with pi
     */
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("red:");
      h->wrp();
      PrintS(" with ");
      strat->T[ii].wrp();
    }
#endif
    assume(strat->fromT == FALSE);

    ksReducePoly(h, &(strat->T[ii]), NULL, NULL, strat);

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
      if (h->lcm!=NULL) pLmFree(h->lcm);
#ifdef KDEBUG
      h->lcm=NULL;
#endif
      return 0;
    }
    h->SetShortExpVector();
    not_sev = ~ h->sev;
    /*
     * try to reduce the s-polynomial h
     *test first whether h should go to the lazyset L
     *-if the degree jumps
     *-if the number of pre-defined reductions jumps
     */
    pass++;
    if (!TEST_OPT_REDTHROUGH && (strat->Ll >= 0) && (pass > strat->LazyPass))
    {
      h->SetLmCurrRing();
      at = strat->posInL(strat->L,strat->Ll,h,strat);
      if (at <= strat->Ll)
      {
        int dummy=strat->sl;
        if (kFindDivisibleByInS(strat, &dummy, h) < 0)
          return 1;
        enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
          Print(" lazy: -> L%d\n",at);
#endif
        h->Clear();
        return -1;
      }
    }
  }
}

/*2
*  reduction procedure for the inhomogeneous case
*  and not a degree-ordering
*/
int redLazy (LObject* h,kStrategy strat)
{
  if (strat->tl<0) return 1;
  int at,i,ii,li;
  int j = 0;
  int pass = 0;
  assume(h->pFDeg() == h->FDeg);
  long reddeg = h->GetpFDeg();
  long d;
  unsigned long not_sev;

  h->SetShortExpVector();
  poly h_p = h->GetLmTailRing();
  not_sev = ~ h->sev;
  loop
  {
    j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h);
    if (j < 0) return 1;

    li = strat->T[j].pLength;
    #if 0
    if (li==0)
    {
      li=strat->T[j].pLength=pLength(strat->T[j].p);
    }
    #endif
    ii = j;
    /*
     * the polynomial to reduce with (up to the moment) is;
     * pi with length li
     */

    i = j;
#if 1
    if (TEST_OPT_LENGTH)
    loop
    {
      /*- search the shortest possible with respect to length -*/
      i++;
      if (i > strat->tl)
        break;
      if (li<=1)
        break;
    #if 0
      if (strat->T[i].pLength==0)
      {
        PrintS("!");
        strat->T[i].pLength=pLength(strat->T[i].p);
      }
   #endif
      if ((strat->T[i].pLength < li)
         &&
          p_LmShortDivisibleBy(strat->T[i].GetLmTailRing(), strat->sevT[i],
                               h_p, not_sev, strat->tailRing))
      {
        /*
         * the polynomial to reduce with is now;
         */
        PrintS("+");
        li = strat->T[i].pLength;
        ii = i;
      }
    }
#endif

    /*
     * end of search: have to reduce with pi
     */


#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("red:");
      h->wrp();
      PrintS(" with ");
      strat->T[ii].wrp();
    }
#endif

    ksReducePoly(h, &(strat->T[ii]), NULL, NULL, strat);

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("\nto ");
      h->wrp();
      PrintLn();
    }
#endif

    h_p=h->GetLmTailRing();

    if (h_p == NULL)
    {
      if (h->lcm!=NULL) pLmFree(h->lcm);
#ifdef KDEBUG
      h->lcm=NULL;
#endif
      return 0;
    }
    h->SetShortExpVector();
    not_sev = ~ h->sev;
    d = h->SetpFDeg();
    /*- try to reduce the s-polynomial -*/
    pass++;
    if (//!TEST_OPT_REDTHROUGH &&
        (strat->Ll >= 0) && ((d > reddeg) || (pass > strat->LazyPass)))
    {
      h->SetLmCurrRing();
      at = strat->posInL(strat->L,strat->Ll,h,strat);
      if (at <= strat->Ll)
      {
#if 1
        int dummy=strat->sl;
        if (kFindDivisibleByInS(strat, &dummy, h) < 0)
          return 1;
#endif
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) Print(" ->L[%d]\n",at);
#endif
        enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
        h->Clear();
        return -1;
      }
    }
    else if (d != reddeg)
    {
      if (d>=strat->tailRing->bitmask)
      {
        if (h->pTotalDeg() >= strat->tailRing->bitmask)
        {
          strat->overflow=TRUE;
          //Print("OVERFLOW in redLazy d=%ld, max=%ld\n",d,strat->tailRing->bitmask);
          h->GetP();
          at = strat->posInL(strat->L,strat->Ll,h,strat);
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
          h->Clear();
          return -1;
        }
      }
      else if ((TEST_OPT_PROT) && (strat->Ll < 0))
      {
        Print(".%ld",d);mflush();
        reddeg = d;
      }
    }
  }
}
/*2
*  reduction procedure for the sugar-strategy (honey)
* reduces h with elements from T choosing first possible
* element in T with respect to the given ecart
*/
int redHoney (LObject* h, kStrategy strat)
{
  if (strat->tl<0) return 1;
  //if (h->GetLmTailRing()==NULL) return 0; // HS: SHOULD NOT BE NEEDED!
  assume(h->FDeg == h->pFDeg());
  poly h_p;
  int i,j,at,pass,ei, ii, h_d;
  unsigned long not_sev;
  long reddeg,d;

  pass = j = 0;
  d = reddeg = h->GetpFDeg() + h->ecart;
  h->SetShortExpVector();
  int li;
  h_p = h->GetLmTailRing();
  not_sev = ~ h->sev;

  h->PrepareRed(strat->use_buckets);
  loop
  {
    j=kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h);
    if (j < 0) return 1;

    ei = strat->T[j].ecart;
    li = strat->T[j].pLength;
    ii = j;
    /*
     * the polynomial to reduce with (up to the moment) is;
     * pi with ecart ei
     */
    i = j;
    if (TEST_OPT_LENGTH)
    loop
    {
      /*- takes the first possible with respect to ecart -*/
      i++;
      if (i > strat->tl)
        break;
      //if (ei < h->ecart)
      //  break;
      if (li<=1)
        break;
      if ((((strat->T[i].ecart < ei) && (ei> h->ecart))
         || ((strat->T[i].ecart <= h->ecart) && (strat->T[i].pLength < li)))
         &&
          p_LmShortDivisibleBy(strat->T[i].GetLmTailRing(), strat->sevT[i],
                               h_p, not_sev, strat->tailRing))
      {
        /*
         * the polynomial to reduce with is now;
         */
        ei = strat->T[i].ecart;
        li = strat->T[i].pLength;
        ii = i;
      }
    }

    /*
     * end of search: have to reduce with pi
     */
    if (!TEST_OPT_REDTHROUGH && (pass!=0) && (ei > h->ecart))
    {
      h->GetTP(); // clears bucket
      h->SetLmCurrRing();
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
          h->last=NULL;
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
#ifdef KDEBUG
          if (TEST_OPT_DEBUG) Print(" ecart too big: -> L%d\n",at);
#endif
          h->Clear();
          return -1;
        }
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
    assume(strat->fromT == FALSE);

    number coef;
    ksReducePoly(h,&(strat->T[ii]),strat->kNoetherTail(),&coef,strat);
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("\nto:");
      h->wrp();
      PrintLn();
    }
#endif
    if(h->IsNull())
    {
      h->Clear();
      if (h->lcm!=NULL) pLmFree(h->lcm);
      #ifdef KDEBUG
      h->lcm=NULL;
      #endif
      return 0;
    }
    h->SetShortExpVector();
    not_sev = ~ h->sev;
    h_d = h->SetpFDeg();
    /* compute the ecart */
    if (ei <= h->ecart)
      h->ecart = d-h_d;
    else
      h->ecart = d-h_d+ei-h->ecart;

    /*
     * try to reduce the s-polynomial h
     *test first whether h should go to the lazyset L
     *-if the degree jumps
     *-if the number of pre-defined reductions jumps
     */
    pass++;
    d = h_d + h->ecart;
    if (!TEST_OPT_REDTHROUGH && (strat->Ll >= 0) && ((d > reddeg) || (pass > strat->LazyPass)))
    {
      h->GetTP(); // clear bucket
      h->SetLmCurrRing();
      at = strat->posInL(strat->L,strat->Ll,h,strat);
      if (at <= strat->Ll)
      {
        int dummy=strat->sl;
        h->last=NULL;
        if (kFindDivisibleByInS(strat, &dummy, h) < 0)
          return 1;
        enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
          Print(" degree jumped: -> L%d\n",at);
#endif
        h->Clear();
        return -1;
      }
    }
    else if (d > reddeg)
    {
      if (d>=strat->tailRing->bitmask)
      {
        if (h->pTotalDeg()+h->ecart >= strat->tailRing->bitmask)
        {
          strat->overflow=TRUE;
          //Print("OVERFLOW in redHoney d=%ld, max=%ld\n",d,strat->tailRing->bitmask);
          h->GetP();
          at = strat->posInL(strat->L,strat->Ll,h,strat);
          h->last=NULL;
          enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
          h->Clear();
          return -1;
        }
      }
      else if (TEST_OPT_PROT && (strat->Ll < 0) )
      {
        //h->wrp(); Print("<%d>\n",h->GetpLength());
        reddeg = d;
        Print(".%ld",d); mflush();
      }
    }
  }
}

/*2
*  reduction procedure for the normal form
*/

poly redNF (poly h,int &max_ind,int nonorm,kStrategy strat)
{
  if (h==NULL) return NULL;
  int j;
  max_ind=strat->sl;

  if (0 > strat->sl)
  {
    return h;
  }
  LObject P(h);
  P.SetShortExpVector();
  P.bucket = kBucketCreate(currRing);
  kBucketInit(P.bucket,P.p,pLength(P.p));
  kbTest(P.bucket);
#ifdef HAVE_RINGS
  BOOLEAN is_ring = rField_is_Ring(currRing);
#endif
#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    PrintS("redNF: starting S: ");
    for( j = 0; j <= max_ind; j++ )
    {
      Print("S[%d] (of size: %d): ", j, pSize(strat->S[j]));
      pWrite(strat->S[j]);
    }
  };
#endif

  loop
  {
    j=kFindDivisibleByInS(strat,&max_ind,&P);
    if (j>=0)
    {
#ifdef HAVE_RINGS
      if (!is_ring)
      {
#endif
        int sl=pSize(strat->S[j]);
        int jj=j;
        loop
        {
          int sll;
          jj=kFindNextDivisibleByInS(strat,jj+1,max_ind,&P);
          if (jj<0) break;
          sll=pSize(strat->S[jj]);
          if (sll<sl)
          {
            #ifdef KDEBUG
            if (TEST_OPT_DEBUG) Print("better(S%d:%d -> S%d:%d)\n",j,sl,jj,sll);
            #endif
            //else if (TEST_OPT_PROT) { PrintS("b"); mflush(); }
            j=jj;
            sl=sll;
          }
        }
        if ((nonorm==0) && (!nIsOne(pGetCoeff(strat->S[j]))))
        {
          pNorm(strat->S[j]);
          //if (TEST_OPT_PROT) { PrintS("n"); mflush(); }
        }
#ifdef HAVE_RINGS
      }
#endif
      nNormalize(pGetCoeff(P.p));
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS("red:");
        wrp(h);
        PrintS(" with ");
        wrp(strat->S[j]);
      }
#endif
#ifdef HAVE_PLURAL
      if (rIsPluralRing(currRing))
      {
        number coef;
        nc_kBucketPolyRed(P.bucket,strat->S[j],&coef);
        nDelete(&coef);
      }
      else
#endif
      {
        number coef;
        coef=kBucketPolyRed(P.bucket,strat->S[j],pLength(strat->S[j]),strat->kNoether);
        nDelete(&coef);
      }
      h = kBucketGetLm(P.bucket);   // FRAGE OLIVER
      if (h==NULL)
      {
        kBucketDestroy(&P.bucket);

#ifdef KDEBUG
        if (TEST_OPT_DEBUG)
        {
          PrintS("redNF: starting S: ");
          for( j = 0; j <= max_ind; j++ )
          {
            Print("S[%d] (of size: %d): ", j, pSize(strat->S[j]));
            pWrite(strat->S[j]);
          }
        };
#endif

        return NULL;
      }
      kbTest(P.bucket);
      P.p=h;
      P.t_p=NULL;
      P.SetShortExpVector();
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS("\nto:");
        wrp(h);
        PrintLn();
      }
#endif
    }
    else
    {
      P.p=kBucketClear(P.bucket);
      kBucketDestroy(&P.bucket);
      pNormalize(P.p);

#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS("redNF: starting S: ");
        for( j = 0; j <= max_ind; j++ )
        {
          Print("S[%d] (of size: %d): ", j, pSize(strat->S[j]));
          pWrite(strat->S[j]);
        }
      };
#endif

      return P.p;
    }
  }
}

#ifdef KDEBUG
static int bba_count = 0;
#endif /* KDEBUG */
void kDebugPrint(kStrategy strat);

ideal bba (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat)
{
#ifdef KDEBUG
  bba_count++;
  int loop_count = 0;
#endif /* KDEBUG */
  om_Opts.MinTrack = 5;
  int   srmax,lrmax, red_result = 1;
  int   olddeg,reduc;
  int hilbeledeg=1,hilbcount=0,minimcnt=0;
  BOOLEAN withT = FALSE;

  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  initBuchMoraPos(strat);
  initHilbCrit(F,Q,&hilb,strat);
  initBba(F,strat);
  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  /*Shdl=*/initBuchMora(F, Q,strat);
  if (strat->minim>0) strat->M=idInit(IDELEMS(F),F->rank);
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

#ifdef KDEBUG
#if MYTEST
  if (TEST_OPT_DEBUG)
  {
    PrintS("bba start GB: currRing: ");
    // rWrite(currRing);PrintLn();
    rDebugPrint(currRing);
    PrintLn();
  }
#endif /* MYTEST */
#endif /* KDEBUG */

#ifdef HAVE_TAIL_RING
  if(!idIs0(F) &&(!rField_is_Ring()))  // create strong gcd poly computes with tailring and S[i] ->to be fixed
    kStratInitChangeTailRing(strat);
#endif
  if (BVERBOSE(23))
  {
    if (test_PosInT!=NULL) strat->posInT=test_PosInT;
    if (test_PosInL!=NULL) strat->posInL=test_PosInL;
    kDebugPrint(strat);
  }


#ifdef KDEBUG
  //kDebugPrint(strat);
#endif
  /* compute------------------------------------------------------- */
  while (strat->Ll >= 0)
  {
    if (strat->Ll > lrmax) lrmax =strat->Ll;/*stat.*/
    #ifdef KDEBUG
      loop_count++;
      if (TEST_OPT_DEBUG) messageSets(strat);
    #endif
    if (strat->Ll== 0) strat->interpt=TRUE;
    if (TEST_OPT_DEGBOUND
        && ((strat->honey && (strat->L[strat->Ll].ecart+pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
            || ((!strat->honey) && (pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))))
    {
      /*
       *stops computation if
       * 24 IN test and the degree +ecart of L[strat->Ll] is bigger then
       *a predefined number Kstd1_deg
       */
      while ((strat->Ll >= 0)
        && (strat->L[strat->Ll].p1!=NULL) && (strat->L[strat->Ll].p2!=NULL)
        && ((strat->honey && (strat->L[strat->Ll].ecart+pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
            || ((!strat->honey) && (pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg)))
        )
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      if (strat->Ll<0) break;
      else strat->noClearS=TRUE;
    }
    /* picks the last element from the lazyset L */
    strat->P = strat->L[strat->Ll];
    strat->Ll--;

    if (pNext(strat->P.p) == strat->tail)
    {
      // deletes the short spoly
#ifdef HAVE_RINGS
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
        // if not, change to a ring where exponents are at least
        // large enough
        if (!kStratChangeTailRing(strat))
        {
          WerrorS("OVERFLOW...");
          break;
        }
      }
      // create the real one
      ksCreateSpoly(&(strat->P), NULL, strat->use_buckets,
                    strat->tailRing, m1, m2, strat->R);
    }
    else if (strat->P.p1 == NULL)
    {
      if (strat->minim > 0)
        strat->P.p2=p_Copy(strat->P.p, currRing, strat->tailRing);
      // for input polys, prepare reduction
      strat->P.PrepareRed(strat->use_buckets);
    }

    if (strat->P.p == NULL && strat->P.t_p == NULL)
    {
      red_result = 0;
    }
    else
    {
      if (TEST_OPT_PROT)
        message((strat->honey ? strat->P.ecart : 0) + strat->P.pFDeg(),
                &olddeg,&reduc,strat, red_result);

      /* reduction of the element chosen from L */
      red_result = strat->red(&strat->P,strat);
      if (errorreported)  break;
    }

    if (strat->overflow)
    {
        if (!kStratChangeTailRing(strat)) { Werror("OVERFLOW.."); break;}
    }

    // reduction to non-zero new poly
    if (red_result == 1)
    {
      // get the polynomial (canonicalize bucket, make sure P.p is set)
      strat->P.GetP(strat->lmBin);
      // in the homogeneous case FDeg >= pFDeg (sugar/honey)
      // but now, for entering S, T, we reset it
      // in the inhomogeneous case: FDeg == pFDeg
      if (strat->homog) strat->initEcart(&(strat->P));

      /* statistic */
      if (TEST_OPT_PROT) PrintS("s");

      int pos=posInS(strat,strat->sl,strat->P.p,strat->P.ecart);

#ifdef KDEBUG
#if MYTEST
      PrintS("New S: "); pDebugPrint(strat->P.p); PrintLn();
#endif /* MYTEST */
#endif /* KDEBUG */

      // reduce the tail and normalize poly
      // in the ring case we cannot expect LC(f) = 1,
      // therefore we call pContent instead of pNorm
      if ((TEST_OPT_INTSTRATEGY) || (rField_is_Ring(currRing)))
      {
        strat->P.pCleardenom();
        if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
        {
          strat->P.p = redtailBba(&(strat->P),pos-1,strat, withT);
          strat->P.pCleardenom();
        }
      }
      else
      {
        strat->P.pNorm();
        if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
          strat->P.p = redtailBba(&(strat->P),pos-1,strat, withT);
      }

#ifdef KDEBUG
      if (TEST_OPT_DEBUG){PrintS("new s:");strat->P.wrp();PrintLn();}
#if MYTEST
      PrintS("New (reduced) S: "); pDebugPrint(strat->P.p); PrintLn();
#endif /* MYTEST */
#endif /* KDEBUG */

      // min_std stuff
      if ((strat->P.p1==NULL) && (strat->minim>0))
      {
        if (strat->minim==1)
        {
          strat->M->m[minimcnt]=p_Copy(strat->P.p,currRing,strat->tailRing);
          p_Delete(&strat->P.p2, currRing, strat->tailRing);
        }
        else
        {
          strat->M->m[minimcnt]=strat->P.p2;
          strat->P.p2=NULL;
        }
        if (strat->tailRing!=currRing && pNext(strat->M->m[minimcnt])!=NULL)
          pNext(strat->M->m[minimcnt])
            = strat->p_shallow_copy_delete(pNext(strat->M->m[minimcnt]),
                                           strat->tailRing, currRing,
                                           currRing->PolyBin);
        minimcnt++;
      }

      // enter into S, L, and T
      //if ((!TEST_OPT_IDLIFT) || (pGetComp(strat->P.p) <= strat->syzComp))
        enterT(strat->P, strat);
#ifdef HAVE_RINGS
      if (rField_is_Ring(currRing))
        superenterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat, strat->tl);
      else
#endif
        enterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat, strat->tl);
      // posInS only depends on the leading term
      strat->enterS(strat->P, pos, strat, strat->tl);
#if 0
      int pl=pLength(strat->P.p);
      if (pl==1)
      {
        //if (TEST_OPT_PROT)
        //PrintS("<1>");
      }
      else if (pl==2)
      {
        //if (TEST_OPT_PROT)
        //PrintS("<2>");
      }
#endif
      if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);
//      Print("[%d]",hilbeledeg);
      if (strat->P.lcm!=NULL)
#ifdef HAVE_RINGS
        pLmDelete(strat->P.lcm);
#else
        pLmFree(strat->P.lcm);
#endif
      if (strat->sl>srmax) srmax = strat->sl;
    }
    else if (strat->P.p1 == NULL && strat->minim > 0)
    {
      p_Delete(&strat->P.p2, currRing, strat->tailRing);
    }

#ifdef KDEBUG
    memset(&(strat->P), 0, sizeof(strat->P));
#endif /* KDEBUG */
    kTest_TS(strat);
  }
#ifdef KDEBUG
#if MYTEST
  PrintS("bba finish GB: currRing: "); rWrite(currRing);
#endif /* MYTEST */
  if (TEST_OPT_DEBUG) messageSets(strat);
#endif /* KDEBUG */

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

  /* complete reduction of the standard basis--------- */
  if (TEST_OPT_REDSB)
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
  if (TEST_OPT_PROT) messageStat(srmax,lrmax,hilbcount,strat);
  if (Q!=NULL) updateResult(strat->Shdl,Q,strat);

#ifdef KDEBUG
#if MYTEST
  PrintS("bba_end: currRing: "); rWrite(currRing);
#endif /* MYTEST */
#endif /* KDEBUG */
  idTest(strat->Shdl);

  return (strat->Shdl);
}

poly kNF2 (ideal F,ideal Q,poly q,kStrategy strat, int lazyReduce)
{
  assume(q!=NULL);
  assume(!(idIs0(F)&&(Q==NULL))); // NF(q, std(0) in polynomial ring?

// lazy_reduce flags: can be combined by |
//#define KSTD_NF_LAZY   1
  // do only a reduction of the leading term
//#define KSTD_NF_NONORM 4
  // only global: avoid normalization, return a multiply of NF
  poly   p;
  int   i;

  //if ((idIs0(F))&&(Q==NULL))
  //  return pCopy(q); /*F=0*/
  //strat->ak = idRankFreeModule(F);
  /*- creating temp data structures------------------- -*/
  BITSET save_test=test;
  test|=Sy_bit(OPT_REDTAIL);
  initBuchMoraCrit(strat);
  strat->initEcart = initEcartBBA;
  strat->enterS = enterSBba;
#ifndef NO_BUCKETS
  strat->use_buckets = (!TEST_OPT_NOT_BUCKETS) && (!rIsPluralRing(currRing));
#endif
  /*- set S -*/
  strat->sl = -1;
  /*- init local data struct.---------------------------------------- -*/
  /*Shdl=*/initS(F,Q,strat);
  /*- compute------------------------------------------------------- -*/
  //if ((TEST_OPT_INTSTRATEGY)&&(lazyReduce==0))
  //{
  //  for (i=strat->sl;i>=0;i--)
  //    pNorm(strat->S[i]);
  //}
  kTest(strat);
  if (TEST_OPT_PROT) { PrintS("r"); mflush(); }
  if (BVERBOSE(23)) kDebugPrint(strat);
  int max_ind;
  p = redNF(pCopy(q),max_ind,lazyReduce & KSTD_NF_NONORM,strat);
  if ((p!=NULL)&&((lazyReduce & KSTD_NF_LAZY)==0))
  {
    if (TEST_OPT_PROT) { PrintS("t"); mflush(); }
    #ifdef HAVE_RINGS
    if (rField_is_Ring())
    {
      p = redtailBba_Z(p,max_ind,strat);
    }
    else
    #endif
    {
      BITSET save=test;
      test &= ~Sy_bit(OPT_INTSTRATEGY);
      p = redtailBba(p,max_ind,strat,(lazyReduce & KSTD_NF_NONORM)==0);
      test=save;
    }
  }
  /*- release temp data------------------------------- -*/
  omfree(strat->sevS);
  omfree(strat->ecartS);
  omfree(strat->T);
  omfree(strat->sevT);
  omfree(strat->R);
  omfree(strat->S_2_R);
  omfree(strat->L);
  omfree(strat->B);
  omfree(strat->fromQ);
  idDelete(&strat->Shdl);
  test=save_test;
  if (TEST_OPT_PROT) PrintLn();
  return p;
}

ideal kNF2 (ideal F,ideal Q,ideal q,kStrategy strat, int lazyReduce)
{
  assume(!idIs0(q));
  assume(!(idIs0(F)&&(Q==NULL)));
// lazy_reduce flags: can be combined by |
//#define KSTD_NF_LAZY   1
  // do only a reduction of the leading term
//#define KSTD_NF_NONORM 4
  // only global: avoid normalization, return a multiply of NF
  poly   p;
  int   i;
  ideal res;
  int max_ind;

  //if (idIs0(q))
  //  return idInit(IDELEMS(q),si_max(q->rank,F->rank));
  //if ((idIs0(F))&&(Q==NULL))
  //  return idCopy(q); /*F=0*/
  //strat->ak = idRankFreeModule(F);
  /*- creating temp data structures------------------- -*/
  BITSET save_test=test;
  test|=Sy_bit(OPT_REDTAIL);
  initBuchMoraCrit(strat);
  strat->initEcart = initEcartBBA;
  strat->enterS = enterSBba;
  /*- set S -*/
  strat->sl = -1;
#ifndef NO_BUCKETS
  strat->use_buckets = (!TEST_OPT_NOT_BUCKETS) && (!rIsPluralRing(currRing));
#endif
  /*- init local data struct.---------------------------------------- -*/
  /*Shdl=*/initS(F,Q,strat);
  /*- compute------------------------------------------------------- -*/
  res=idInit(IDELEMS(q),si_max(q->rank,F->rank));
  BITSET save=test;
  test &= ~Sy_bit(OPT_INTSTRATEGY);
  for (i=IDELEMS(q)-1; i>=0; i--)
  {
    if (q->m[i]!=NULL)
    {
      if (TEST_OPT_PROT) { PrintS("r");mflush(); }
      p = redNF(pCopy(q->m[i]),max_ind,lazyReduce & KSTD_NF_NONORM,strat);
      if ((p!=NULL)&&((lazyReduce & KSTD_NF_LAZY)==0))
      {
        if (TEST_OPT_PROT) { PrintS("t"); mflush(); }
        #ifdef HAVE_RINGS
        if (rField_is_Ring())
        {
          p = redtailBba_Z(p,max_ind,strat);
        }
        else
        #endif
        {
          p = redtailBba(p,max_ind,strat,(lazyReduce & KSTD_NF_NONORM)==0);
        }
      }
      res->m[i]=p;
    }
    //else
    //  res->m[i]=NULL;
  }
  /*- release temp data------------------------------- -*/
  test=save;
  omfree(strat->sevS);
  omfree(strat->ecartS);
  omfree(strat->T);
  omfree(strat->sevT);
  omfree(strat->R);
  omfree(strat->S_2_R);
  omfree(strat->L);
  omfree(strat->B);
  omfree(strat->fromQ);
  idDelete(&strat->Shdl);
  test=save_test;
  if (TEST_OPT_PROT) PrintLn();
  return res;
}

/* shiftgb stuff */
#ifdef HAVE_SHIFTBBA


ideal bbaShift(ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat, int uptodeg, int lV)
{
#ifdef KDEBUG
  bba_count++;
  int loop_count = 0;
#endif
  om_Opts.MinTrack = 5;
  int   srmax,lrmax, red_result = 1;
  int   olddeg,reduc;
  int hilbeledeg=1,hilbcount=0,minimcnt=0;
  BOOLEAN withT = TRUE; // very important for shifts

  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit, NO CHANGES */
  initBuchMoraPos(strat); /*NO CHANGES YET: perhaps later*/
  initHilbCrit(F,Q,&hilb,strat); /*NO CHANGES*/
  initBbaShift(F,strat); /* DONE */
  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  /*Shdl=*/initBuchMoraShift(F, Q,strat); /* updateS with no toT, i.e. no init for T */
  updateSShift(strat,uptodeg,lV); /* initializes T */

  if (strat->minim>0) strat->M=idInit(IDELEMS(F),F->rank);
  srmax = strat->sl;
  reduc = olddeg = lrmax = 0;
  strat->lV=lV;

#ifndef NO_BUCKETS
  if (!TEST_OPT_NOT_BUCKETS)
    strat->use_buckets = 1;
#endif

  // redtailBBa against T for inhomogenous input
  //  if (!TEST_OPT_OLDSTD)
  //    withT = ! strat->homog;

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
    loop_count++;
    if (TEST_OPT_DEBUG) messageSets(strat);
#endif
    if (strat->Ll== 0) strat->interpt=TRUE;
    if (TEST_OPT_DEGBOUND
        && ((strat->honey && (strat->L[strat->Ll].ecart+pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
            || ((!strat->honey) && (pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))))
    {
      /*
       *stops computation if
       * 24 IN test and the degree +ecart of L[strat->Ll] is bigger then
       *a predefined number Kstd1_deg
       */
      while ((strat->Ll >= 0)
        && (strat->L[strat->Ll].p1!=NULL) && (strat->L[strat->Ll].p2!=NULL)
        && ((strat->honey && (strat->L[strat->Ll].ecart+pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
            || ((!strat->honey) && (pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg)))
        )
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      if (strat->Ll<0) break;
      else strat->noClearS=TRUE;
    }
    /* picks the last element from the lazyset L */
    strat->P = strat->L[strat->Ll];
    strat->Ll--;

    if (pNext(strat->P.p) == strat->tail)
    {
      // deletes the short spoly
      pLmFree(strat->P.p);
      strat->P.p = NULL;
      poly m1 = NULL, m2 = NULL;

      // check that spoly creation is ok
      while (strat->tailRing != currRing &&
             !kCheckSpolyCreation(&(strat->P), strat, m1, m2))
      {
        assume(m1 == NULL && m2 == NULL);
        // if not, change to a ring where exponents are at least
        // large enough
        kStratChangeTailRing(strat);
      }
      // create the real one
      ksCreateSpoly(&(strat->P), NULL, strat->use_buckets,
                    strat->tailRing, m1, m2, strat->R);
    }
    else if (strat->P.p1 == NULL)
    {
      if (strat->minim > 0)
        strat->P.p2=p_Copy(strat->P.p, currRing, strat->tailRing);
      // for input polys, prepare reduction
      strat->P.PrepareRed(strat->use_buckets);
    }

    poly qq;

    /* here in the nonhomog case we shrink the new spoly */

    if ( ! strat->homog)
    {
      strat->P.GetP(strat->lmBin); // because shifts are counted with .p structure
      /* in the nonhomog case we have to shrink the polynomial */
      assume(strat->P.t_p!=NULL);
      qq = p_Shrink(strat->P.t_p, lV, strat->tailRing); // direct shrink
      if (qq != NULL)
      {
         /* we're here if Shrink is nonzero */
        //         strat->P.p =  NULL;
        //        strat->P.Delete(); /* deletes P.p and P.t_p */ //error
        strat->P.p   =  NULL; // is not set by Delete
        strat->P.t_p =  qq;
        strat->P.GetP(strat->lmBin);
        // update sev and length
        strat->initEcart(&(strat->P));
        strat->P.sev = pGetShortExpVector(strat->P.p);
//         strat->P.FDeg = strat->P.pFDeg();
//         strat->P.length = strat->P.pLDeg();
//         strat->P.pLength =strat->P.GetpLength(); //pLength(strat->P.p);
      }
      else
      {
         /* Shrink is zero, like y(1)*y(2) - y(1)*y(3)*/
#ifdef KDEBUG
         if (TEST_OPT_DEBUG){PrintS("nonzero s shrinks to 0");PrintLn();}
#endif
         //         strat->P.Delete();  // cause error
         strat->P.p = NULL;
         strat->P.t_p = NULL;
           //         strat->P.p = NULL; // or delete strat->P.p ?
       }
    }
      /* end shrinking poly in the nonhomog case */

    if (strat->P.p == NULL && strat->P.t_p == NULL)
    {
      red_result = 0;
    }
    else
    {
      if (TEST_OPT_PROT)
        message((strat->honey ? strat->P.ecart : 0) + strat->P.pFDeg(),
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
      if (TEST_OPT_INTSTRATEGY)
      {
        strat->P.pCleardenom();
        if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
        {
          strat->P.p = redtailBba(&(strat->P),pos-1,strat, withT);
          strat->P.pCleardenom();
        }
      }
      else
      {
        strat->P.pNorm();
        if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
          strat->P.p = redtailBba(&(strat->P),pos-1,strat, withT);
      }

      // here we must shrink again! and optionally reduce again
      // or build shrink into redtailBba!

#ifdef KDEBUG
      if (TEST_OPT_DEBUG){PrintS("new s:");strat->P.wrp();PrintLn();}
#endif

      // min_std stuff
      if ((strat->P.p1==NULL) && (strat->minim>0))
      {
        if (strat->minim==1)
        {
          strat->M->m[minimcnt]=p_Copy(strat->P.p,currRing,strat->tailRing);
          p_Delete(&strat->P.p2, currRing, strat->tailRing);
        }
        else
        {
          strat->M->m[minimcnt]=strat->P.p2;
          strat->P.p2=NULL;
        }
        if (strat->tailRing!=currRing && pNext(strat->M->m[minimcnt])!=NULL)
          pNext(strat->M->m[minimcnt])
            = strat->p_shallow_copy_delete(pNext(strat->M->m[minimcnt]),
                                           strat->tailRing, currRing,
                                           currRing->PolyBin);
        minimcnt++;
      }

    /* here in the nonhomog case we shrink the reduced poly AGAIN */

    if ( ! strat->homog)
    {
      strat->P.GetP(strat->lmBin); // because shifts are counted with .p structure
      /* assume strat->P.t_p != NULL */
      /* in the nonhomog case we have to shrink the polynomial */
      assume(strat->P.t_p!=NULL); // poly qq defined above
      qq = p_Shrink(strat->P.t_p, lV, strat->tailRing); // direct shrink
      if (qq != NULL)
      {
         /* we're here if Shrink is nonzero */
        //         strat->P.p =  NULL;
        //        strat->P.Delete(); /* deletes P.p and P.t_p */ //error
        strat->P.p   =  NULL; // is not set by Delete
        strat->P.t_p =  qq;
        strat->P.GetP(strat->lmBin);
        // update sev and length
        strat->initEcart(&(strat->P));
        strat->P.sev = pGetShortExpVector(strat->P.p);
      }
      else
      {
         /* Shrink is zero, like y(1)*y(2) - y(1)*y(3)*/
#ifdef PDEBUG
         if (TEST_OPT_DEBUG){PrintS("nonzero s shrinks to 0");PrintLn();}
#endif
         //         strat->P.Delete();  // cause error
         strat->P.p = NULL;
         strat->P.t_p = NULL;
           //         strat->P.p = NULL; // or delete strat->P.p ?
         goto     red_shrink2zero;
       }
    }
      /* end shrinking poly AGAIN in the nonhomog case */


      // enter into S, L, and T
      //if ((!TEST_OPT_IDLIFT) || (pGetComp(strat->P.p) <= strat->syzComp))
      //        enterT(strat->P, strat); // this was here before Shift stuff
      //enterTShift(LObject p, kStrategy strat, int atT, int uptodeg, int lV); // syntax
      // the default value for atT = -1 as in bba
      /*   strat->P.GetP(); */
      // because shifts are counted with .p structure // done before, but ?
      enterTShift(strat->P,strat,-1,uptodeg, lV);
      enterpairsShift(strat->P.p,strat->sl,strat->P.ecart,pos,strat, strat->tl,uptodeg,lV);
      //      enterpairsShift(vw,strat->sl,strat->P.ecart,pos,strat, strat->tl,uptodeg,lV);
      // posInS only depends on the leading term
      strat->enterS(strat->P, pos, strat, strat->tl);

      if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);
//      Print("[%d]",hilbeledeg);
      if (strat->P.lcm!=NULL) pLmFree(strat->P.lcm);
      if (strat->sl>srmax) srmax = strat->sl;
    }
    else
    {
    red_shrink2zero:
      if (strat->P.p1 == NULL && strat->minim > 0)
      {
        p_Delete(&strat->P.p2, currRing, strat->tailRing);
      }
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
  /*  shift case: look for elt's in S such that they are divisible by elt in T */
  //  if (TEST_OPT_SB_1)
  if (TEST_OPT_REDSB)
  {
    int k=0;
    int j=-1;
    while(k<=strat->sl)
    {
//       loop
//       {
//         if (j>=k) break;
//         clearS(strat->S[j],strat->sevS[j],&k,&j,strat);
//         j++;
//       }
      LObject Ln (strat->S[k],currRing, strat->tailRing);
      Ln.SetShortExpVector();
      j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, &Ln, j+1);
      if (j<0) {  k++; j=-1;}
      else
      {
        if ( pLmCmp(strat->S[k],strat->T[j].p) == 0)
        {
          j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, &Ln, j+1);
          if (j<0) {  k++; j=-1;}
          else
          {
            deleteInS(k,strat);
          }
        }
        else
        {
          deleteInS(k,strat);
        }
      }
    }
  }

  if (TEST_OPT_REDSB)
  {    completeReduce(strat, TRUE); //shift: withT = TRUE
    if (strat->completeReduce_retry)
    {
      // completeReduce needed larger exponents, retry
      // to reduce with S (instead of T)
      // and in currRing (instead of strat->tailRing)
      cleanT(strat);strat->tailRing=currRing;
      int i;
      for(i=strat->sl;i>=0;i--) strat->S_2_R[i]=-1;
      completeReduce(strat, TRUE);
    }
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
  if (TEST_OPT_PROT) messageStat(srmax,lrmax,hilbcount,strat);
  if (Q!=NULL) updateResult(strat->Shdl,Q,strat);
  return (strat->Shdl);
}


ideal freegb(ideal I, int uptodeg, int lVblock)
{
  /* todo main call */

  /* assume: ring is prepared, ideal is copied into shifted ring */
  /* uptodeg and lVblock are correct - test them! */

  /* check whether the ideal is in V */

//  if (0)
  if (! ideal_isInV(I,lVblock) )
  {
    WerrorS("The input ideal contains incorrectly encoded elements! ");
    return(NULL);
  }

  //  kStrategy strat = new skStrategy;
  /* ideal bbaShift(ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat, int uptodeg, int lV) */
  /* at the moment:
- no quotient (check)
- no *w, no *hilb
  */
  /* ideal F, ideal Q, tHomog h,intvec ** w, intvec *hilb,int syzComp,
     int newIdeal, intvec *vw) */
  ideal RS = kStdShift(I,NULL, testHomog, NULL,NULL,0,0,NULL, uptodeg, lVblock);
    //bbaShift(I,NULL, NULL, NULL, strat, uptodeg, lVblock);
  idSkipZeroes(RS);
  return(RS);
}

/*2
*reduces h with elements from T choosing  the first possible
* element in t with respect to the given pDivisibleBy
*/
int redFirstShift (LObject* h,kStrategy strat)
{
  if (h->IsNull()) return 0;

  int at, reddeg,d;
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
    if (!h->IsNull())
    {
      poly qq=p_Shrink(h->GetTP(),strat->lV,strat->tailRing);
      h->p=NULL;
      h->t_p=qq;
      if (qq!=NULL) h->GetP(strat->lmBin);
    }

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
          /*          if (kFindDivisibleByInS(strat,&dummy, h) < 0) */
          if (kFindDivisibleByInT(strat->T,strat->sevT, dummy, h) < 0)
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
        Print(".%d",d);mflush();
      }
    }
  }
}

void initBbaShift(ideal F,kStrategy strat)
{
  int i;
  idhdl h;
 /* setting global variables ------------------- */
  strat->enterS = enterSBba; /* remains as is, we change enterT! */

  strat->red = redFirstShift; /* no redHomog ! */

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
#endif
