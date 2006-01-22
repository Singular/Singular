/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kstd2.cc,v 1.9 2006-01-22 04:29:37 wienand Exp $ */
/*
*  ABSTRACT -  Kernel: alg. of Buchberger
*/

// #define PDEBUG 2
// define to enable tailRings
#define HAVE_TAIL_RING
// define if no buckets should be used
// #define NO_BUCKETS

#include "mod2.h"
#include "kutil.h"
#include "structs.h"
#include "omalloc.h"
#include "polys.h"
#include "ideals.h"
#include "febase.h"
#include "kstd1.h"
#include "khstd.h"
#include "kbuckets.h"
//#include "cntrlc.h"
#include "weight.h"
#include "intvec.h"
#ifdef HAVE_PLURAL
#include "gring.h"
#endif
// #include "timer.h"

// return -1 if no divisor is found
//        number of first divisor, otherwise
int kFindDivisibleByInT(const TSet &T, const unsigned long* sevT,
                        const int tl, const LObject* L, const int start)
{
  unsigned long not_sev = ~L->sev;
  int j = start;
  poly p;
  ring r;
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
int kFindDivisibleByInS(const polyset &S, const unsigned long* sev, const int sl, LObject* L)
{
  unsigned long not_sev = ~L->sev;
  poly p = L->GetLmCurrRing();
  int j = 0;

  pAssume(~not_sev == p_GetShortExpVector(p, currRing));
  loop
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

#ifdef HAVE_RING2TOM
/*        Obsolute since changes to pLmDiv
// return -1 if no divisor is found
//        number of first divisor, otherwise
int kRingFindDivisibleByInT(const TSet &T, const unsigned long* sevT,
                        const int tl, const LObject* L, const int start)
{
  unsigned long not_sev = ~L->sev;
  int j = start;
  poly p;
  ring r;
  L->GetLm(p, r);

  pAssume(~not_sev == p_GetShortExpVector(p, r));

  {
    loop
    {
      if (j > tl) return -1;
#if defined(PDEBUG) || defined(PDIV_DEBUG)
      if (p_LmRingShortDivisibleBy(T[j].p, sevT[j],
                               p, not_sev, r))
        return j;
#else
      if ( !(sevT[j] & not_sev) &&
           p_LmRingDivisibleBy(T[j].p, p, r) )
        return j;
#endif
      j++;
    }
  }
  return -1;
}

// same as above, only with set S
int kRingFindDivisibleByInS(const polyset &S, const unsigned long* sev, const int sl, LObject* L)
{
  unsigned long not_sev = ~L->sev;
  poly p = L->GetLmCurrRing();
  int j = 0;
  //PrintS("FindDiv: p="); wrp(p); PrintLn();
  pAssume(~not_sev == p_GetShortExpVector(p, currRing));
  loop
  {
    //PrintS("FindDiv: S[j]="); wrp(S[j]); PrintLn();
    if (j > sl) return -1;
#if defined(PDEBUG) || defined(PDIV_DEBUG)
    if (p_LmRingShortDivisibleBy(S[j], sev[j],
                             p, not_sev, currRing))
        return j;
#else
    if ( !(sev[j] & not_sev) &&
         p_LmRingDivisibleBy(S[j], p, currRing) )
      return j;
#endif
    j++;
  }
}
*/

/* now in kutil.cc
long twoPow(long arg) {
  long t = arg;
  long result = 1;
  while (t > 0) {
    result = 2 * result;
    t--;
  }
  return result;
}
*/

long factorial(long arg)
{
   long tmp = 1; arg++;
   for (int i = 2; i < arg; i++)
   {
     tmp = tmp * i;
   }
   return tmp;
}

poly kFindZeroPoly(poly input_p, ring leadRing, ring tailRing)
{
  // m = currRing->ch

  if (input_p == NULL) return NULL;

  int k_ind2 = 0;
  int a_ind2 = 0;

  poly p = input_p;
  poly zeroPoly = NULL;
  long a = (long) pGetCoeff(p);
  long k = 1;

  while (a%2 == 0)
  {
    a = a / 2;
    a_ind2++;
  }

  for (int i = 1; i <= leadRing->N; i++) 
  {
    a = factorial(p_GetExp(p, i, leadRing));
    k = k * a;
    while (a%2 == 0)
    {
      a = a / 2;
      k_ind2++;
    }
  }
  a = (long) pGetCoeff(p);

  number tmp1;
  poly tmp2, tmp3;
  if (leadRing->ch <= k_ind2 + a_ind2)
  {
    zeroPoly = p_ISet(a, tailRing);
    for (int i = 1; i <= leadRing->N; i++)
    {
      for (long j = 1; j <= p_GetExp(p, i,leadRing); j++)
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
          tmp3 = p_ISet((long) tmp1, tailRing);
          zeroPoly = p_Mult_q(zeroPoly, p_Add_q(tmp2, tmp3, tailRing), tailRing);
        }
      }
    }
    tmp2 = p_ISet((long) pGetCoeff(zeroPoly), leadRing);
    for (int i = 1; i <= leadRing->N; i++) {
      pSetExp(tmp2, i, p_GetExp(zeroPoly, i, tailRing));
    }
    p_Setm(tmp2, leadRing);
    zeroPoly = p_LmDeleteAndNext(zeroPoly, tailRing);
    pNext(tmp2) = zeroPoly;
    return tmp2;
  }
  long alpha_k = twoPow(leadRing->ch - k_ind2);
  if (alpha_k <= a) {
    zeroPoly = p_ISet((a / alpha_k)*alpha_k, tailRing);
    for (int i = 1; i <= leadRing->N; i++) {
      for (long j = 1; j <= p_GetExp(p, i, leadRing); j++) {
        tmp1 = nInit(j);
        tmp2 = p_ISet(1, tailRing);
        p_SetExp(tmp2, i, 1, tailRing);
        p_Setm(tmp2, tailRing);
        if (nIsZero(tmp1)) {
          zeroPoly = p_Mult_q(zeroPoly, tmp2, tailRing);
        }
        else {
          tmp3 = p_ISet((long) tmp1, tailRing);
          zeroPoly = p_Mult_q(zeroPoly, p_Add_q(tmp2, tmp3, tailRing), tailRing);
        }
      }
    }
    tmp2 = p_ISet((long) pGetCoeff(zeroPoly), leadRing);
    for (int i = 1; i <= leadRing->N; i++) {
      pSetExp(tmp2, i, p_GetExp(zeroPoly, i, tailRing));
    }
    p_Setm(tmp2, leadRing);
    zeroPoly = p_LmDeleteAndNext(zeroPoly, tailRing);
    pNext(tmp2) = zeroPoly;
    return tmp2;
  }
  return NULL;
}

poly kFindDivisibleByZeroPoly(LObject* h)
{
  return kFindZeroPoly(h->GetLmCurrRing(), currRing, h->tailRing);
}

#ifdef HAVE_RING2TOM
/*2
*  reduction procedure for the ring Z/2^m
*/
int redRing2toM (LObject* h,kStrategy strat)
{
  if (h->p == NULL && h->t_p == NULL) return 0; // spoly is zero (can only occure with zero divisors)

//  if (strat->tl<0) return 1;
  int at,d,i;
  int j = 0;
  int pass = 0;
  poly zeroPoly;

//#ifdef HAVE_RING2TOM
  h->SetpFDeg();
  assume(h->pFDeg() == h->FDeg);
  if (h->pFDeg() != h->FDeg)
  {
    Print("h->pFDeg()=%d =!= h->FDeg=%d\n", h->pFDeg(), h->FDeg);
  }
  long reddeg = h->SetpFDeg();
//#else
//  assume(h->pFDeg() == h->FDeg);
//  long reddeg = h->GetpFDeg();
//#endif

  h->SetShortExpVector(); 
  loop
  {
    zeroPoly = kFindDivisibleByZeroPoly(h);
    if (zeroPoly != NULL)
    {
      if (TEST_OPT_PROT)
      {
        PrintS("z");
      }
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS("zero poly created: ");
        wrp(zeroPoly);
        PrintLn();
        PrintS("zero red:");
      }
#endif
      LObject tmp_h(zeroPoly, currRing, strat->tailRing);
      tmp_h.SetShortExpVector();
      strat->initEcart(&tmp_h);
      tmp_h.sev = pGetShortExpVector(tmp_h.p);
      tmp_h.SetpFDeg();
/*      if (TEST_OPT_PROT)
      {
        tstcount ++;
        if (tstcount > 49)
        {
          PrintLn();
          p_wrp(zeroPoly,currRing);
          PrintLn();
          tstcount = 0;
        }
      } */

      enterT(tmp_h, strat, strat->tl + 1);
      j = strat->tl;
    }
    else
    {
      j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h);
      if (j < 0) return 1;
#ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        PrintS("T red:");
      }
#endif
    }
#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      h->wrp();
      PrintS(" with ");
      strat->T[j].wrp();
    }
#endif

    ksReducePoly(h, &(strat->T[j]), NULL, NULL, strat);
//    if (zeroPoly != NULL) {
//      strat->tl--;
//    }

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
      if (h->lcm!=NULL) pLmFree(h->lcm);
#ifdef KDEBUG
      h->lcm=NULL;
#endif
      return 0;
    }
    h->SetShortExpVector();
    d = h->SetpFDeg();
    /*- try to reduce the s-polynomial -*/
    pass++;
    if (!K_TEST_OPT_REDTHROUGH &&
        (strat->Ll >= 0) && ((d > reddeg) || (pass > strat->LazyPass)))
    {
      h->SetLmCurrRing();
      at = strat->posInL(strat->L,strat->Ll,h,strat);
      if (at <= strat->Ll)
      {
#if 0
        if (kRingFindDivisibleByInS(strat->S, strat->sevS, strat->sl, h) < 0)
          return 1;
#endif
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) Print(" ->L[%d]\n",at);
#endif
        enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);     // NOT RING CHECKED OLIVER
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
#endif
#endif

/*2
*  reduction procedure for the homogeneous case
*  and the case of a degree-ordering
*/
int redHomog (LObject* h,kStrategy strat)
{
//  if (strat->tl<0) return 1;
#ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    PrintS("red:");
    h->wrp();
    PrintS(" ");
  }
#endif
  int j;
  int pass = 0;
  loop
  {
    // find a poly with which we can reduce
    h->SetShortExpVector();
    j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h);
    if (j < 0)
    {
      h->SetpFDeg();
      return 1;
    }

    // now we found one which is divisible -- reduce it
    ksReducePoly(h, &(strat->T[j]), NULL, NULL, strat);

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      Print("\nto ", h->t_p);
      h->wrp();
      PrintLn();
    }
#endif
    if (h->GetLmTailRing() == NULL)
    {
      if (h->lcm!=NULL) pLmFree(h->lcm);
#ifdef KDEBUG
      h->lcm=NULL;
#endif
      return 0;
    }
    if (!K_TEST_OPT_REDTHROUGH &&
        (strat->Ll >= 0) && (pass > strat->LazyPass))
    {
      h->SetLmCurrRing();
      int at = strat->posInL(strat->L,strat->Ll,h,strat);
      if (at <= strat->Ll)
      {
#ifdef KDEBUG
        if (TEST_OPT_DEBUG) Print(" ->L[%d]\n",at);
#endif
        enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
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
  int at,d,i;
  int j = 0;
  int pass = 0;
  assume(h->pFDeg() == h->FDeg);
  long reddeg = h->GetpFDeg();

  h->SetShortExpVector();
  loop
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

    ksReducePoly(h, &(strat->T[j]), NULL, NULL, strat);

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
      if (h->lcm!=NULL) pLmFree(h->lcm);
#ifdef KDEBUG
      h->lcm=NULL;
#endif
      return 0;
    }
    h->SetShortExpVector();
    d = h->SetpFDeg();
    /*- try to reduce the s-polynomial -*/
    pass++;
    if (!K_TEST_OPT_REDTHROUGH &&
        (strat->Ll >= 0) && ((d > reddeg) || (pass > strat->LazyPass)))
    {
      h->SetLmCurrRing();
      at = strat->posInL(strat->L,strat->Ll,h,strat);
      if (at <= strat->Ll)
      {
#if 0
        if (kFindDivisibleByInS(strat->S, strat->sevS, strat->sl, h) < 0)
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
int redHoney (LObject* h, kStrategy strat)
{
  if (strat->tl<0) return 1;
  assume(h->FDeg == h->pFDeg());

  poly h_p;
  int i,j,at,pass,ei, ii, h_d;
  unsigned long not_sev;
  long reddeg,d;

  pass = j = 0;
  d = reddeg = h->GetpFDeg() + h->ecart;
  h->SetShortExpVector();
  h_p = h->GetLmTailRing();
  not_sev = ~ h->sev;
  loop
  {
    j = kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h);
    if (j < 0) return 1;

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
      if (ei <= h->ecart)
        break;
      if ((strat->T[i].ecart < ei) &&
          p_LmShortDivisibleBy(strat->T[i].GetLmTailRing(), strat->sevT[i],
                               h_p, not_sev, strat->tailRing))
      {
        /*
         * the polynomial to reduce with is now;
         */
        ei = strat->T[i].ecart;
        ii = i;
      }
    }

    /*
     * end of search: have to reduce with pi
     */
    if (!K_TEST_OPT_REDTHROUGH && (pass!=0) && (ei > h->ecart))
    {
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
    if (!K_TEST_OPT_REDTHROUGH && (strat->Ll >= 0) && ((d > reddeg) || (pass > strat->LazyPass)))
    {
      h->SetLmCurrRing();
      at = strat->posInL(strat->L,strat->Ll,h,strat);
      if (at <= strat->Ll)
      {
        if (kFindDivisibleByInS(strat->S, strat->sevS, strat->sl, h) < 0)
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
    else if (TEST_OPT_PROT && (strat->Ll < 0) && (d > reddeg))
    {
      reddeg = d;
      Print(".%d",d); mflush();
    }
  }
}
/*2
*  reduction procedure for the normal form
*/

poly redNF (poly h,kStrategy strat)
{
  if (h==NULL) return NULL;
  int j;

  if (0 > strat->sl)
  {
    return h;
  }
  LObject P(h);
  P.SetShortExpVector();
  P.bucket = kBucketCreate(currRing);
  kBucketInit(P.bucket,P.p,pLength(P.p));
  loop
  {
/* Obsolete since change in pLmDiv
#ifdef HAVE_RING2TOM
    if (currRing->cring == 1) {
      j=kRingFindDivisibleByInS(strat->S,strat->sevS,strat->sl,&P);
    }
    else
#endif
*/
      j=kFindDivisibleByInS(strat->S,strat->sevS,strat->sl,&P);
    if (j>=0)
    {
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
        return NULL;
      }
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
      return P.p;
    }
  }
}

#ifdef KDEBUG
static int bba_count = 0;
#endif

ideal bba (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat)
{
#ifdef KDEBUG
  bba_count++;
  int loop_count = 0;
#endif
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
  if (!K_TEST_OPT_OLDSTD)
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
    loop_count++;
#ifdef HAVE_RING2TOM
    if (TEST_OPT_DEBUG) PrintS("--- next step ---\n");
#endif
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
      while (strat->Ll >= 0) deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      break;
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

#ifdef HAVE_RING2TOM
    if (strat->P.p == NULL && strat->P.t_p == NULL) {
      red_result = 0;
    }
    else
#endif
    {
      if (TEST_OPT_PROT)
        message((strat->honey ? strat->P.ecart : 0) + strat->P.pFDeg(),
                &olddeg,&reduc,strat, red_result);

      /* reduction of the element choosen from L */
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

      // enter into S, L, and T
      enterT(strat->P, strat);
      enterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat, strat->tl);
      // posInS only depends on the leading term
      strat->enterS(strat->P, pos, strat, strat->tl);
      if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);
//      Print("[%d]",hilbeledeg);
      if (strat->P.lcm!=NULL) pLmFree(strat->P.lcm);
      if (strat->sl>srmax) srmax = strat->sl;
    }
    else if (strat->P.p1 == NULL && strat->minim > 0)
    {
      p_Delete(&strat->P.p2, currRing, strat->tailRing);
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
  //if ((TEST_OPT_INTSTRATEGY)&&(lazyReduce==0))
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
  poly   p;
  int   i;
  ideal res;

  if (idIs0(q))
    return idInit(IDELEMS(q),q->rank);
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
  //if ((TEST_OPT_INTSTRATEGY)&&(lazyReduce==0))
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
