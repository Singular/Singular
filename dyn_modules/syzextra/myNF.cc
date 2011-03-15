// -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file DebugPrint.cc
 * 
 * Here we implement dPrint-s.
 *
 * ABSTRACT: debug-detailed-printing
 *
 * @author Oleksandr Motsak
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

// include header file
#include <kernel/mod2.h>

#include "myNF.h"

#include <omalloc/omalloc.h>
#include <kernel/structs.h>
#include <kernel/febase.h>



#include <kernel/ideals.h>

#include <kernel/syz.h>
#include <kernel/longrat.h>
#include <kernel/kutil.h>
#include <kernel/kstd1.h>
#include <kernel/options.h>

#include <kernel/kbuckets.h>

#include <kernel/intvec.h>
#include <kernel/p_polys.h>
#include <kernel/polys.h>
#include <kernel/pInline2.h>


#ifdef HAVE_PLURAL
#define PLURAL_INTERNAL_DECLARATIONS 1
#endif
#include <kernel/gring.h>
#include <kernel/sca.h>



BEGIN_NAMESPACE()

///  reduction procedure for the normal form, which uses pLength instead of pSize!
static poly redNFLength (poly h,int &max_ind,int nonorm,kStrategy strat)
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
      Print("S[%d] (of size: %d): ", j, pLength(strat->S[j]));
      wrp(strat->S[j]);
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
        int sl=pLength(strat->S[j]);
        int jj=j;
        loop
        {
          int sll;
          jj=kFindNextDivisibleByInS(strat,jj+1,max_ind,&P);
          if (jj<0) break;
          sll=pLength(strat->S[jj]);
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
            Print("S[%d] (of size: %d): ", j, pLength(strat->S[j]));
            wrp(strat->S[j]);
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
          Print("S[%d] (of size: %d): ", j, pLength(strat->S[j]));
          wrp(strat->S[j]);
        }
      };
#endif

      return P.p;
    }
  }
}


poly kNF2Length (ideal F,ideal Q,poly q,kStrategy strat, int lazyReduce)
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
  p = redNFLength(pCopy(q),max_ind,lazyReduce & KSTD_NF_NONORM,strat);
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

END_NAMESPACE


BEGIN_NAMESPACE_SINGULARXX  BEGIN_NAMESPACE(NF)

poly kNFLength(ideal F, ideal Q, poly p,int syzComp, int lazyReduce)
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
    res=kNF2Length(F,Q,pp,strat,lazyReduce);
  delete(strat);

#ifdef HAVE_PLURAL
  if(pp != p)
    p_Delete(&pp, currRing);
#endif
  return res;
}

END_NAMESPACE               END_NAMESPACE_SINGULARXX

// Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab
