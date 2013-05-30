/* file:        SDBase.cc
 * authors:     Grischa Studzinski & Benjamin Schnitzler
 * created:     ask git
 * last change: ask git
 *
 * DESCRIPTION:
 * This file contains extensions to some structures and
 * functions defined in kutil.h (...Object, ...Strategy, etc.).
 *
 * TODO:
 * 1. clean up, remove unecessary comments and old uncommented
 *    code; instead comment, when applicable, from where a
 *    function stems from
 * 2. divide this file in smaller portions
 * 3. move to current singular version; adapt functions;
 *    currRing is no longer global; look up, if some of these
 *    functions changed in the newest version and how they
 *    changed, adapt these changes if possible
 * 4. look for all TODO remarks in this file and try to resolve
 *    the TODO
 * 5. (always) pray; sacrifice some lambs to the goddess of
 *    code
 * 6. things from the global TODO list
 * 7. contemplate about what I have forgotten here
 */

// define to enable tailRings (TODO: do this properly)
#define HAVE_TAIL_RING

//Needed because fo include order; should rather be included in
//ring.h
#include <kernel/mod2.h>

//For various declarations
#include <kernel/febase.h>
#include <kernel/ideals.h>
#include <misc/options.h>
#include <kernel/kstd1.h>
#include <kernel/khstd.h>
#include <polys/weight.h>
#include <kernel/timer.h>
#include <kernel/polys.h>
#include <misc/intvec.h>

#include <kernel/shiftgb.h>

#include <kernel/kutil.h>
//#include <kernel/SDkutil.h>//do this via kutil.h otherwise...

#include <kernel/SDBase.h>
#include <kernel/SDDebug.h>

//now our adapted multiplications for:
//- ksCreateSpoly
//- ksReducePoly
//- ksReducePolyTail
//- etc. ?
#include <kernel/SDMultiplication.h>

//No debug version at the moment
//#undef KDEBUG
#undef HAVE_RINGS //We do not have rings
#undef HAVE_PLURAL //Nope

#define HAVE_SEV 2
//HAVE_SEV == 0 - This includes no sev-stuff and will certainly
//                produce syntax errors
//HAVE_SEV == 1 - This should include the absolutely necessary 
//                sev-part.
//HAVE_SEV == 2 - This should include everything about sev,
//                which possibly should be included.
//HAVE_SEV == 3 - This could include sev-stuff, we possibly do
//                not need.
//HAVE_SEV >= 4 - This could include some sev stuff, which
//                should maybe not included.

namespace ShiftDVec
{
  uint divisibleBy
    ( const sTObject * t1, 
      const sTObject * t2, int numVars );

  uint findRightOverlaps
    ( const sTObject * t1, 
      const sTObject * t2, 
      int numVars, int maxDeg, uint ** overlaps );

  bool createSPviolatesDeg
    (poly a, poly b, uint shift, int uptodeg);

  typedef skStrategy* kStrategy;
};



//extracted inlines from kutil.cc


/*2
*utilities for TSet, LSet
*/
//BOCO: static function copied from kutil.cc
inline static intset initec (const int maxnr)
{ return (intset)omAlloc(maxnr*sizeof(int)); }

//BOCO: static function copied from kutil.cc
static inline void enlargeL
  (LSet* L,int* length,const int incr)
{
  namespace SD = ShiftDVec;

  assume((*L)!=NULL);
  assume((length+incr)>0);

  *L = (LSet)omReallocSize
          ( (*L), 
            (*length)*sizeof(LObject),
            ((*length)+incr)*sizeof(LObject) );
  (*length) += incr;
}

//BOCO: static function copied from kutil.cc
inline static unsigned long* initsevS (const int maxnr)
{ return 
    (unsigned long*)omAlloc0(maxnr*sizeof(unsigned long)); }

//BOCO: static function copied from kutil.cc
inline static int* initS_2_R (const int maxnr)
{ return (int*)omAlloc0(maxnr*sizeof(int)); }


//the work begins


//ideal freegb(ideal I, int uptodeg, int lVblock)
ideal ShiftDVec::freegbdvc
  (ideal I, int uptodeg, int lVblock, long deBoGriFlags)
{
  namespace SD = ShiftDVec;

#if DEBOGRI > 0
  SD::deBoGri = deBoGriFlags;
  SD::lpDVCase = 1;
#endif
  /* todo main call */

  /* assume: ring is prepared, ideal is copied into shifted ring */
  /* uptodeg and lVblock are correct - test them! */

  /* check whether the ideal is in V */

  if (! ideal_isInV(I,lVblock) )
  {
    WerrorS("The input ideal contains incorrectly encoded elements! ");
    return(NULL);
  }

  ideal RS = SD::kStd
    (I,NULL, testHomog, NULL,NULL,0,0,NULL, uptodeg, lVblock);

  idSkipZeroes(RS);
  return(RS);
}


/* BOCO: original header (replaced)
 * TODO: CLEANUP: remove comment
ideal kStd
  ( ideal F, ideal Q, tHomog h, intvec ** w, 
    intvec *hilb, int syzComp, int newIdeal, intvec *vw )
*/
ideal ShiftDVec::kStd
  ( ideal F, ideal Q, tHomog h, intvec ** w, intvec *hilb,
    int syzComp, int newIdeal, intvec *vw, int uptodeg, int lV )
{
  namespace SD = ShiftDVec;
  if(idIs0(F))
    return idInit(1,F->rank);

  ideal r;
  BOOLEAN b=currRing->pLexOrder,toReset=FALSE;
  BOOLEAN delete_w=(w==NULL);
  SD::skStrategy * strat=new SD::skStrategy;

#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
  strat->init_lV(lV);
  strat->init_uptodeg(uptodeg);
#endif

  if(!TEST_OPT_RETURN_SB)
    strat->syzComp = syzComp;
  if (TEST_OPT_SB_1)
    strat->newIdeal = newIdeal;
  if (rField_has_simple_inverse(currRing))
    strat->LazyPass=20;
  else
    strat->LazyPass=2;
  strat->LazyDegree = 1;
#if 0 //BOCO: original code (replaced)
      //TODO: CLEANUP: remove this
  strat->enterOnePair=enterOnePairNormal;
  strat->chainCrit=chainCritNormal;
#else //BOCO: replacement
  strat->enterOnePair=NULL;
  strat->chainCrit=NULL;
#endif
  strat->ak = id_RankFreeModule(F,currRing);
  strat->kModW=kModW=NULL;
  strat->kHomW=kHomW=NULL;
  if (vw != NULL)
  {
    currRing->pLexOrder=FALSE;
    strat->kHomW=kHomW=vw;
    strat->pOrigFDeg = currRing->pFDeg;
    strat->pOrigLDeg = currRing->pLDeg;
    pSetDegProcs(currRing,kHomModDeg);
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
  currRing->pLexOrder=b;
  if (h==isHomog)
  {
    if (strat->ak > 0 && (w!=NULL) && (*w!=NULL))
    {
      strat->kModW = kModW = *w;
      if (vw == NULL)
      {
        strat->pOrigFDeg = currRing->pFDeg;
        strat->pOrigLDeg = currRing->pLDeg;
        pSetDegProcs(currRing,kModDeg);
        toReset = TRUE;
      }
    }
    currRing->pLexOrder = TRUE;
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
/* BOCO: (deleted) not of interest for us
#ifdef HAVE_PLURAL
  if (rIsPluralRing(currRing))
  {
    const BOOLEAN bIsSCA  = rIsSCA(currRing) && strat->z2homog; // for Z_2 prod-crit
    strat->no_prod_crit   = ! bIsSCA;
    if (w!=NULL)
      r = nc_GB(F, Q, *w, hilb, strat, currRing);
    else
      r = nc_GB(F, Q, NULL, hilb, strat, currRing);
  }
  else
#endif
#ifdef HAVE_RINGS
  if (rField_is_Ring(currRing))
    r=bba(F,Q,NULL,hilb,strat);
  else
#endif
  {
    if (currRing->OrdSgn==-1)
    {
      if (w!=NULL)
        r=mora(F,Q,*w,hilb,strat);
      else
        r=mora(F,Q,NULL,hilb,strat);
    }
    else
*/
    {
      ideal I = NULL; assume(0); //BOCO: TODO
      if (w!=NULL)
        r=SD::bba(F,Q,I,*w,hilb,strat);
      else
        r=SD::bba(F,Q,I,NULL,hilb,strat);
    }
//  } //BOCO: (deleted) see above
#ifdef KDEBUG
  idTest(r);
#endif
  if (toReset)
  {
    kModW = NULL;
    pRestoreDegProcs(currRing,strat->pOrigFDeg, strat->pOrigLDeg);
  }
  currRing->pLexOrder = b;
//Print("%d reductions canceled \n",strat->cel);
  HCord=strat->HCord;
  delete(strat);
  if ((delete_w)&&(w!=NULL)&&(*w!=NULL)) delete *w;
  return r;
}


#ifdef KDEBUG
static int bba_count = 0;
#endif /* KDEBUG */
void kDebugPrint(kStrategy strat);


ideal ShiftDVec::bba
  ( ideal F, ideal Q, ideal I,
    intvec *w,intvec *hilb,ShiftDVec::kStrategy strat )
{
  /* BOCO:
   * If I != NULL we want to calculate the GB of the left ideal
   * F in the factor algebra K<X>/I.
   */
  namespace SD = ShiftDVec;

  initDeBoGri
    ( ShiftDVec::indent, "Entering bba", "Leaving bba", 4096 );

#ifdef KDEBUG
  bba_count++;
  int loop_count = 0;
#endif /* KDEBUG */
  om_Opts.MinTrack = 5;
  int   red_result = 1;
  int   olddeg,reduc;
  int hilbeledeg=1,hilbcount=0,minimcnt=0;
  BOOLEAN withT = FALSE;

  SD::initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  initBuchMoraPos(strat);
  initHilbCrit(F,Q,&hilb,strat);
  SD::initBba(F,strat);
  
  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  /*Shdl=*/SD::initBuchMora(F, Q,strat);

  if (strat->minim>0) strat->M=idInit(IDELEMS(F),F->rank);
  reduc = olddeg = 0;

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
  if(!idIs0(F) &&(!rField_is_Ring(currRing)))  // create strong gcd poly computes with tailring and S[i] ->to be fixed
    kStratInitChangeTailRing(strat);
#endif
  if (BVERBOSE(23))
  {
    if (test_PosInT!=NULL) strat->posInT=test_PosInT;
    if (test_PosInL!=NULL) strat->posInL=test_PosInL;
    kDebugPrint(strat);
  }

  ShiftDVec::InitSDMultiplication(strat->tailRing, strat);
  ShiftDVec::InitSDMultiplication(currRing, strat);

#ifdef KDEBUG
  //kDebugPrint(strat);
#endif
  /* compute------------------------------------------------------- */
  while (strat->Ll >= 0)
  {
    #ifdef KDEBUG
      loop_count++;
      if (TEST_OPT_DEBUG) messageSets(strat);
    #endif
    if (strat->Ll== 0) strat->interpt=TRUE;
    if (TEST_OPT_DEGBOUND
        && ((strat->honey && (strat->L[strat->Ll].ecart+currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
            || ((!strat->honey) && (currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))))
    {
      /*
       *stops computation if
       * 24 IN test and the degree +ecart of L[strat->Ll] is bigger then
       *a predefined number Kstd1_deg
       */
      while ((strat->Ll >= 0)
        && (strat->L[strat->Ll].p1!=NULL) && (strat->L[strat->Ll].p2!=NULL)
        && ((strat->honey && (strat->L[strat->Ll].ecart+currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
            || ((!strat->honey) && (currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg)))
        )
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      if (strat->Ll<0) break;
      else strat->noClearS=TRUE;
    }
    /* picks the last element from the lazyset L */
    strat->P = strat->L[strat->Ll];
    strat->Ll--;

    deBoGriTTest(strat);
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
      // BOCO: TODO: check if this check does the right thing
      // for letterplace rings (and if we need it -> didn't we
      // check for degree violations before entering the Pair
      // into the pair set???); BOCO: now we have a slightly
      // adapted version, but the question, if we need it at
      // all, is yet to be answered; Partial answer: We need it
      // at least for getting m1 and m2
      while 
        ( strat->tailRing != currRing &&
          !ShiftDVec::kCheckSpolyCreation
            (&(strat->P), strat, m1, m2)  )
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
      ShiftDVec::ksCreateSpoly
        ( &(strat->P), NULL, strat->use_buckets,
           strat->tailRing, m1, m2, strat->R );
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

      /* reduction of the element choosen from L */
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
      PrintS("New S: "); p_DebugPrint(strat->P.p, currRing); PrintLn();
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
          deBoGriPrint("Red Tail.\n", 2048);
          strat->P.p = 
            SD::redtailBba(&(strat->P),pos-1,strat, withT);
          strat->P.pCleardenom();
        }
      }
      else
      {
        strat->P.pNorm();
        if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
          deBoGriPrint("Red Tail.\n", 2048);
          strat->P.p = 
            SD::redtailBba(&(strat->P),pos-1,strat, withT);
      }

#ifdef KDEBUG
      if (TEST_OPT_DEBUG){PrintS("new s:");strat->P.wrp();PrintLn();}
#if MYTEST
      PrintS("New (reduced) S: "); p_DebugPrint(strat->P.p, currRing); PrintLn();
#endif /* MYTEST */
#endif /* KDEBUG */

      // min_std stuff
      if ((strat->P.p1==NULL) && (strat->minim>0))
      {
        if (strat->minim==1)
        {
          strat->M->m[minimcnt]=p_Copy(strat->P.p,currRing,strat->tailRing);
          loGriToFile("p_Delete in bba in shiftDVec.cc ",0 ,1024, (void*)strat->P.p2); 
          p_Delete(&strat->P.p2, currRing, strat->tailRing);
          //strat->P.p2 = NULL;  //BOCO: Did I add that? Why?
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
      
      strat->P.SD_Ext()->SetDVecIfNULL();
      enterT(strat->P, strat);

#ifdef HAVE_RINGS
      if (rField_is_Ring(currRing))
        superenterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat, strat->tl);
      else
#endif

      /* BOCO: we added strat->P, thus we should only 
       * increment up to strat->sl-1 
       * Update: no, we no longer add strat->sl before
       * invoking this funktion. It was an Idea to simplify
       * the code ... but it didn't work with singular. */
      SD::enterpairs( &(strat->P),strat->sl,strat->P.ecart,
                        pos,strat,strat->tl                    );

      // posInS only depends on the leading term
      strat->enterS(strat->P, pos, strat, strat->tl);
#if 0 //BOCO: this was not uncommented by me, nevertheless:
      //TODO: CLEANUP: remove this
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
      {
#ifdef HAVE_RINGS
        pLmDelete(strat->P.lcm);
#else
        pLmFree(strat->P.lcm);
#endif
      }
    }else{
#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
      strat->P.SD_Ext()->freeDVec();  
#endif
    }

    //BOCO: we always want to delete the second poly, because
    //      it is a shift and will not be used anywhere else
    if(strat->P.p2)
    {
      pDelete(&strat->P.p2);
      strat->P.p2 = NULL;
    }

    deBoGriTTest(strat);
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

  //BOCO: delete dvecs
  //  we have to do that manually, since destructors for
  //  sTObjects are mostly useless in Singular
  for(int j = 0; j <= strat->tl; ++j)
    strat->T[j].SD_Ext()->freeDVec();

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
        LObject tmpL(strat->S[j]);
        SD::clearS(&tmpL,strat->sevS[j],&k,&j,strat);
        j++;
      }
      k++;
    }
  }

  /* complete reduction of the standard basis--------- */
  if (TEST_OPT_REDSB)
  {
    SD::completeReduce(strat);
#ifdef HAVE_TAIL_RING
    if (strat->completeReduce_retry)
    {
      // completeReduce needed larger exponents, retry
      // to reduce with S (instead of T)
      // and in currRing (instead of strat->tailRing)
      cleanT(strat);strat->tailRing=currRing;
      int i;
      for(i=strat->sl;i>=0;i--) strat->S_2_R[i]=-1;
      SD::completeReduce(strat);
    }
#endif
  }
  else if (TEST_OPT_PROT) PrintLn();

  /* release temp data-------------------------------- */
  exitBuchMora(strat);
//  if (TEST_OPT_WEIGHTM)
//  {
//    pRestoreDegProcs(currRing,pFDegOld, pLDegOld);
//    if (ecartWeights)
//    {
//      omFreeSize((ADDRESS)ecartWeights,((currRing->N)+1)*sizeof(short));
//      ecartWeights=NULL;
//    }
//  }
  if (TEST_OPT_PROT) messageStat(hilbcount,strat);
#if 0 //BOCO: original code (replaced)
      //TODO: CLEANUP: remove this
  if (Q!=NULL) updateResult(strat->Shdl,Q,strat);
#else
  if (Q!=NULL) SD::updateResult(strat->Shdl,Q,strat);
#endif

#ifdef KDEBUG
#if MYTEST
  PrintS("bba_end: currRing: "); rWrite(currRing);
#endif /* MYTEST */
#endif /* KDEBUG */
  idTest(strat->Shdl);

  return (strat->Shdl);
}


/*2
 * in the case of a standardbase of a module over a qring:
 * replace polynomials in i by ak vectors,
 * (the polynomial * unit vectors gen(1)..gen(ak)
 * in every case (also for ideals:)
 * deletes divisible vectors/polynomials
 * BOCO: original resides in kutil.cc
 */
void ShiftDVec::updateResult(ideal r,ideal Q, ShiftDVec::kStrategy strat)
{
  using namespace ShiftDVec;
  int l;
  if (strat->ak>0)
  {
    for (l=IDELEMS(r)-1;l>=0;l--)
    {
      if ((r->m[l]!=NULL) && (pGetComp(r->m[l])==0))
      {
        loGriToFile("pDelete in updateResult on &r->m[1]", 0,1024, r->m[1]);
        pDelete(&r->m[l]); // and set it to NULL
      }
    }
    int q;
    poly p;
    for (l=IDELEMS(r)-1;l>=0;l--)
    {
      if ((r->m[l]!=NULL)
      //&& (strat->syzComp>0)
      //&& (pGetComp(r->m[l])<=strat->syzComp)
      )
      {
        for(q=IDELEMS(Q)-1; q>=0;q--)
        {
#if 0 //BOCO: original code (replaced)
      //TODO: CLEANUP: remove this
          if ((Q->m[q]!=NULL)
          &&(pLmDivisibleBy(Q->m[q],r->m[l])))
#else //replacement
          /*BOCO:
           * The original pLmDivisibleBy checks if second arg
           * divides first arg, we check if first arg is
           * divisibly by second arg! */
          sTObject t1(r->m[l]);
          sTObject t2(Q->m[q]); 
          uint shift = p_LmDivisibleBy
            (&t1, &t2, currRing, strat->get_lV());
          if ( (Q->m[q]!=NULL) && shift < UINT_MAX &&
               ( strat->homog ||
                !redViolatesDeg
                  ( r->m[l], r->m[q], strat->get_uptodeg(), 
                    currRing, currRing, strat->tailRing ) ) )
#endif
          {
            if (TEST_OPT_REDSB)
            {
              p=r->m[l];
              r->m[l]=kNF(Q,NULL,p);
              pDelete(&p);
            }
            else
              pDelete(&r->m[l]); // and set it to NULL
            break;
          }
        }
      }
    }
  }
  else
  {
    int q;
    poly p;
    BOOLEAN reduction_found=FALSE;
    for (l=IDELEMS(r)-1;l>=0;l--)
    {
      if (r->m[l]!=NULL)
      {
        for(q=IDELEMS(Q)-1; q>=0;q--)
        {
          if ((Q->m[q]!=NULL)
          &&(pLmEqual(r->m[l],Q->m[q])))
          {
            if (TEST_OPT_REDSB)
            {
              p=r->m[l];
              r->m[l]=kNF(Q,NULL,p);
              pDelete(&p);
              reduction_found=TRUE;
            }
            else
              pDelete(&r->m[l]); // and set it to NULL
            break;
          }
        }
      }
    }
    if (/*TEST_OPT_REDSB &&*/ reduction_found)
    {
      for (l=IDELEMS(r)-1;l>=0;l--)
      {
        if (r->m[l]!=NULL)
        {
          for(q=IDELEMS(r)-1;q>=0;q--)
          {
            /*BOCO:
             * The original pLmDivisibleBy checks if second arg
             * divides first arg, we check if first arg is
             * divisibly by second arg! */
            sTObject t1(r->m[q]); 
            sTObject t2(r->m[l]);

            uint shift =
              p_LmDivisibleBy
                ( &t1, &t2, currRing, strat->get_lV() );

            BOOLEAN reduction_violates_degree =
              redViolatesDeg
                ( r->m[q], r->m[l],
                  strat->get_uptodeg(),
                  currRing, currRing, strat->tailRing );

            if ( (l!=q) && 
                 (shift < UINT_MAX) && (r->m[q]!=NULL) &&
                 (strat->homog || !reduction_violates_degree) )
            { pDelete(&r->m[q]); }
          }
        }
      }
    }
  }
  idSkipZeroes(r);
}


#ifdef KDEBUG
static BOOLEAN sloppy_max = FALSE;
#endif /* KDEBUG */

#if 0 //BOCO: original code (replaced)
      //TODO: CLEANUP: remove this
void completeReduce (kStrategy strat, BOOLEAN withT)
#else //replacement
void ShiftDVec::completeReduce
  ( SD::kStrategy strat, BOOLEAN withT )
{
  namespace SD = ShiftDVec;

#endif
  int i;
  int low = (((currRing->OrdSgn==1) && (strat->ak==0)) ? 1 : 0);
  LObject L;

#ifdef KDEBUG
  // need to set this: during tailreductions of T[i], T[i].max 
  // is out of sync
  sloppy_max = TRUE;
#endif

  strat->noTailReduction = FALSE;
  if (TEST_OPT_PROT)
  {
    PrintLn();
    if (timerv) writeTime("standard base computed:");
  }
  if (TEST_OPT_PROT)
  {
    Print("(S:%d)",strat->sl);mflush();
  }
  for (i=strat->sl; i>=low; i--)
  {
    int end_pos=strat->sl;
    if ((strat->fromQ!=NULL) && (strat->fromQ[i]))
      continue; // do not reduce Q_i
    if (strat->ak==0) end_pos=i-1;
    TObject* T_j = strat->s_2_t(i);
    if ((T_j != NULL)&&(T_j->p==strat->S[i]))
      //BOCO: Why do they test here, if T_j->p==strat->S[i] ?
    {
      L = *T_j;
      #ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        Print("test S[%d]:",i);
        p_wrp(L.p,currRing,strat->tailRing);
        PrintLn();
      }
      #endif
      if (currRing->OrdSgn == 1)
        strat->S[i] =
          SD::redtailBba(&L, end_pos, strat, withT);
      else
        strat->S[i] = SD::redtail(&L, strat->sl, strat);
      #ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        Print("to (tailR) S[%d]:",i);
        p_wrp(strat->S[i],currRing,strat->tailRing);
        PrintLn();
      }
      #endif

      if (strat->redTailChange && strat->tailRing != currRing)
      {
        if (T_j->max != NULL) p_LmFree(T_j->max, strat->tailRing);
        if (pNext(T_j->p) != NULL)
          T_j->max = p_GetMaxExpP(pNext(T_j->p), strat->tailRing);
        else
          T_j->max = NULL;
      }
      if (TEST_OPT_INTSTRATEGY)
        T_j->pCleardenom();
    }
    else
    {
      assume(currRing == strat->tailRing);
      #ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        Print("test S[%d]:",i);
        p_wrp(strat->S[i],currRing,strat->tailRing);
        PrintLn();
      }
      #endif
      if (currRing->OrdSgn == 1)
        strat->S[i] =
          SD::redtailBba(strat->S[i], end_pos, strat, withT);
      else
        strat->S[i] =
          SD::redtail(strat->S[i], strat->sl, strat);
      if (TEST_OPT_INTSTRATEGY)
      {
        if (TEST_OPT_CONTENTSB)
        {
          number n;
          p_Cleardenom_n(strat->S[i], currRing, n);// also does a pContent
          if (!nIsOne(n))
          {
            denominator_list denom=(denominator_list)omAlloc(sizeof(denominator_list_s));
            denom->n=nInvers(n);
            denom->next=DENOMINATOR_LIST;
            DENOMINATOR_LIST=denom;
          }
          nDelete(&n);
        }
        else
        {
          //pContent(strat->S[i]);
          strat->S[i]=p_Cleardenom(strat->S[i], currRing);// also does a pContent
        }
      }
      #ifdef KDEBUG
      if (TEST_OPT_DEBUG)
      {
        Print("to (-tailR) S[%d]:",i);
        p_wrp(strat->S[i],currRing,strat->tailRing);
        PrintLn();
      }
      #endif
    }
    if (TEST_OPT_PROT)
      PrintS("-");
  }
  if (TEST_OPT_PROT) PrintLn();
#ifdef KDEBUG
  sloppy_max = FALSE;
#endif
}


/* BOCO: used in completeReduce
 * WARNING: We only consider the homogenous case at the moment.
 * The original resides in kutil.cc
 */
poly ShiftDVec::redtail
  (LObject* L, int pos, ShiftDVec::kStrategy strat)
{
  namespace SD = ShiftDVec;
  poly h, hn;
#if (HAVE_SEV > 0) //BOCO: comments/uncomments sev
  unsigned long not_sev;
#endif
  strat->redTailChange=FALSE;

  poly p = L->p;
  if (strat->noTailReduction || pNext(p) == NULL) return p;

  LObject Ln(strat->tailRing);
  TObject* With;
  // placeholder in case strat->tl < 0
  TObject  With_s(strat->tailRing);
  h = p;
  hn = pNext(h);
  long op = strat->tailRing->pFDeg(hn, strat->tailRing);
  long e;
  int l;
  BOOLEAN save_HE=strat->kHEdgeFound;
  strat->kHEdgeFound |=
    ((Kstd1_deg>0) && (op<=Kstd1_deg)) || TEST_OPT_INFREDTAIL;

  uint shift = 0;
  while(hn != NULL)
  {
    op = strat->tailRing->pFDeg(hn, strat->tailRing);
    if ((Kstd1_deg>0)&&(op>Kstd1_deg)) goto all_done;
    e = strat->tailRing->pLDeg(hn, &l, strat->tailRing) - op;
    loop
    {
      Ln.Set(hn, strat->tailRing);
#if (HAVE_SEV > 1) //BOCO: comments/uncomments sev
      Ln.sev = p_GetShortExpVector(hn, strat->tailRing);
#endif //#if (HAVE_SEV > 0)
#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
      long ecart = strat->kHEdgeFound ? LONG_MAX : e;
#endif
      TObject * WithTmp = SD::kFindDivisibleByInS
        ( strat, pos, &Ln,
          &With_s, shift, strat->get_lV(), ecart );
      if (WithTmp == NULL) break;
      if(shift == 0){ With = WithTmp; } //no problem
      else //Our divisor is a shift (and thus not in T or S)
      {
        With = new TObject(*WithTmp); //should do just a shallow copy
        With->p = p_LPshiftT
          ( WithTmp->p, shift,
            strat->get_uptodeg(),
            strat->get_lV(), strat, currRing );
        /* BOCO: With->p has to be deleted later; i hope we
         * can just choose currRing for p_LPshift here */
        
        //BOCO: Is that right?:
        With_s.p = With->p;
      }
      With->tailRing = WithTmp->tailRing; //BOCO: added...
      With->length=0;
      With->pLength=0;
      strat->redTailChange=TRUE;

      if ( SD::ksReducePolyTail(L, WithTmp, With, h,
                                strat->kNoetherTail(), strat) )
      {
        // reducing the tail would violate the exp bound
        if (kStratChangeTailRing(strat, L))
        {
          strat->kHEdgeFound = save_HE;
          return SD::redtail(L, pos, strat);
        }
        else
          return NULL;
      }
      hn = pNext(h);
      if (hn == NULL) goto all_done;
      op = strat->tailRing->pFDeg(hn, strat->tailRing);
      if ((Kstd1_deg>0)&&(op>Kstd1_deg)) goto all_done;
      e = strat->tailRing->pLDeg(hn, &l, strat->tailRing) - op;
      if(shift != 0)
      {
        pDelete(&With->p); 
        shift=0; 
        With->SD_Ext()->dvec = NULL; 
        delete With;
      }
    }
    h = hn;
    hn = pNext(h);
  }

  all_done:
  if(shift != 0)
  { 
    pDelete(&With->p); 
    With->SD_Ext()->freeDVec();
    delete With;
  }
  if (strat->redTailChange)
  {
    L->last = NULL;
    L->pLength = 0;
  }
  strat->kHEdgeFound = save_HE;
  return p;
}

//BOCO: original resides in kutil.cc
poly ShiftDVec::redtail(poly p, int pos, SD::kStrategy strat)
{
  namespace SD = ShiftDVec;
  LObject L(p, currRing);
  return SD::redtail(&L, pos, strat);
}


//BOCO: original resides in kstd1.cc
void ShiftDVec::initBba( ideal F, SD::kStrategy strat )
{
  namespace SD = ShiftDVec;

  int i;
  idhdl h;
  /* setting global variables ------------------- */
  strat->enterS = enterSBba;

  //BOCO:
  //  We do not use redHoney/redLazy/redRing at the moment;
  //  See original code for reference
  //TODO:
  //  redHomog also applies to the inhomogenous case
  //  -> rename it!
  strat->red = SD::redHomog;

  if (currRing->pLexOrder && strat->honey)
    strat->initEcart = initEcartNormal;
  else
    strat->initEcart = initEcartBBA;
  if (strat->honey)
    strat->initEcartPair = initEcartPairMora;
  else
    strat->initEcartPair = initEcartPairBba;
}


/* reduction procedure for the homogeneous case
 * and the case of a degree-ordering
 * original resides in kstd2.cc
 */
int ShiftDVec::redHomog( LObject* h, SD::kStrategy strat )
{
  namespace SD = ShiftDVec;

  if (strat->tl<0) return 1;
  assume(h->FDeg == h->pFDeg());

  poly h_p;
  int i,j,at,pass, ii;
#if (HAVE_SEV > 0) //BOCO: comments/uncomments sev
  unsigned long not_sev;
#endif
  long reddeg,d;

  pass = j = 0;
  d = reddeg = h->GetpFDeg();
#if (HAVE_SEV > 1)
  h->SetShortExpVector();
#endif
  int li;
  h_p = h->GetLmTailRing();
#if (HAVE_SEV > 1) //BOCO: comments/uncomments sev
  not_sev = ~ h->sev;
#endif
  loop
  {
#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
    uint shift = 0;
#endif
    j = SD::kFindDivisibleByInT
          ( strat->T, strat->sevT, h, shift, strat );
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
      /*BOCO:
       * The original p_LmShortDivisibleBy checks if second arg
       * divides first arg, we check if first arg is divisibly
       * by second arg! */
      if(strat->T[i].pLength < li){
        TObject t_h(h_p);
        TObject t_i(strat->T[i].GetLmTailRing());
        shift = SD::p_LmShortDivisibleBy
          (&t_h, strat->sevT[i], &t_i, not_sev, strat->tailRing);
        /*
         * the polynomial to reduce with is now;
         */
        if(shift < UINT_MAX)
        {
          li = strat->T[i].pLength;
          ii = i;
        }
        t_h.SD_Ext()->freeDVec();
        t_i.SD_Ext()->freeDVec();
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

    TObject tmp; //save the unshifted poly
    tmp.p = strat->T[ii].p;
    tmp.t_p = strat->T[ii].t_p;
    if(shift > 0)
    {
      strat->T[ii].t_p = p_LPshiftT
        ( tmp.t_p, shift, 
          strat->get_uptodeg(),
          strat->get_lV(), strat, strat->tailRing );
      strat->T[ii].p = p_LPshiftT
        ( tmp.p, shift, 
          strat->get_uptodeg(),
          strat->get_lV(), strat, currRing );
    }
    tmp.tailRing = strat->T[ii].tailRing; //BOCO: added...
    SD::ksReducePoly(h, &tmp, &(strat->T[ii]) );
    h->SD_Ext()->freeDVec();

    //BOCO: why did i change it to that?:
    //ksReducePoly(h,strat->T[ii],strat->kNoetherTail());

    if(shift > 0)
    {
      p_Delete(&strat->T[ii].t_p, strat->tailRing); 
      if(strat->T[ii].p)
      {
        strat->T[ii].p->next = NULL;
        pDelete(&strat->T[ii].p);
      }

      strat->T[ii].p = tmp.p;
      strat->T[ii].t_p = tmp.t_p;
    }

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

      h->SD_Ext()->freeDVec(); //BOCO: maybe this is not necessary ?

#ifdef KDEBUG
      h->lcm=NULL;
#endif
      return 0;
    }
    h->SetShortExpVector();
#if (HAVE_SEV > 1) //BOCO: comments/uncomments sev
    not_sev = ~ h->sev;
#endif //#if (HAVE_SEV > 1)
    /*
     * try to reduce the s-polynomial h
     *test first whether h should go to the lazyset L
     *-if the degree jumps
     *-if the number of pre-defined reductions jumps
     */
    pass++;
    if ( !TEST_OPT_REDTHROUGH && (strat->Ll >= 0) && 
         (pass > strat->LazyPass)                     )
    {
      h->SetLmCurrRing();
      at = strat->posInL(strat->L,strat->Ll,h,strat);
      if (at <= strat->Ll)
      {
        int dummy=strat->sl; int shift_dummy;
        j = SD::kFindDivisibleByInS(strat, &dummy, h, shift_dummy);
        if (j < 0) return 1;

        h->SD_Object_Extension = NULL;
        SD::enterL( &strat->L,&strat->Ll,&strat->Lmax,h,at );
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

int ShiftDVec::kFindDivisibleByInS
  ( const SD::kStrategy strat,
    int* max_ind, LObject* L, int shift )
{
  namespace SD = ShiftDVec;
#if (HAVE_SEV > 0) //BOCO: comments/uncomments sev
  unsigned long not_sev = ~L->sev;
#endif
  poly p = L->GetLmCurrRing();
  int j = 0;

#if (HAVE_SEV > 0) //BOCO: comments/uncomments sev
  pAssume(~not_sev == p_GetShortExpVector(p, currRing));
#endif
#if 1
  int ende;
  if ((strat->ak>0) || currRing->pLexOrder) ende=strat->sl;
  else ende=posInS(strat,*max_ind,p,0)+1;
  if (ende>(*max_ind)) ende=(*max_ind);
#else
  int ende=strat->sl;
#endif
  (*max_ind)=ende;
  loop
  {
    if (j > ende) return -1;

    /*BOCO:
     * The original p_LmDivisibleBy checks if second arg
     * divides first arg, we check if first arg is divisibly
     * by second arg! */
    TObject t_p(p);
    TObject t_Sj(strat->S[j]);
    shift = p_LmDivisibleBy
      ( &t_p, &t_Sj, currRing, strat->get_lV() );
    int reduction_violates_degree = redViolatesDeg
      ( p, strat->S[j],
        strat->get_uptodeg(),
        currRing, currRing, strat->tailRing );
    if ( shift < UINT_MAX && 
         ( strat->homog || !reduction_violates_degree ) )
      return j;

    j++;
  }
}


/*2
 *BOCO:
 *  This function will overload the redBba function of the
 *  bba-Algorithm for the non-commutative dvec-case. The original
 *  comment does still apply; however, h is no longer of type
 *  poly, but of type TObject *, which expands to sTObjectDVec in
 *  the dvec-case (,which we do consider here). Same goes for the
 *  return value of this function.
 *BOCO original comment from kutil.cc:
 *  reduces h using the set S
 *  procedure used in updateS
 */
poly ShiftDVec::redBba
  ( LObject * h, int maxIndex, SD::kStrategy strat )
{
  namespace SD = ShiftDVec;

  int j = 0;

#if (HAVE_SEV > 1) //BOCO: comments/uncomments sev
  unsigned long not_sev = ~ pGetShortExpVector(h->p);
#else
  unsigned long not_sev = 0;
#endif //#if (HAVE_SEV > 0)

  while (j <= maxIndex)
  {
    /*BOCO:
     * The original p_LmShortDivisibleBy checks if second arg
     * divides first arg, we check if first arg is divisibly
     * by second arg! */
    TObject * t;
    if(!(t = strat->s_2_t(j))) 
    { TObject tt(strat->S[j]); t = &tt; }
    uint shift = SD::p_LmShortDivisibleBy
      (h, strat->sevS[j], t, not_sev, currRing);
    int reduction_violates_degree = redViolatesDeg
      ( h, t, strat->get_uptodeg(), currRing );
    if( shift < UINT_MAX && 
        (strat->homog || !reduction_violates_degree) )
    {
      poly pTemp = p_LPshiftT( strat->S[j], shift, 
                               strat->get_uptodeg(),
                               strat->get_lV(),strat,currRing );
      TObject tTemp(pTemp);
      TObject noShift(strat->S[j]);

      //BOCO: hope that is ok... is it nescessary?...
      tTemp.tailRing = noShift.tailRing = strat->tailRing;
      SD::ksReducePoly( h, &noShift,
                        &tTemp, strat->kNoetherTail() );

      //BOCO: after reduction lm has changed:
      h->SD_Ext()->freeDVec();
      if(shift > 0) pDelete(&pTemp); 
      if (h->p==NULL) return NULL;
      j = 0;
#if (HAVE_SEV > 1) //BOCO: comments/uncomments sev
      not_sev = ~ pGetShortExpVector(h->p);
#endif //#if (HAVE_SEV > 1)
    }
    else j++;
  }
  return h->p;
}


/*BOCO:
 * Tests, if t1 is divisible by t2
 * This function will overload the p_LmShortDivisibleBy
 * function. It is used for the non-commutative dvec-case,
 * whereever 
 * p_LmShortDivisibleBy
 *   (poly, unsigned long, poly, unsigned long, ring)
 * was used.
 * Currently the "*sev*"-Variables are not in use.
 */
uint ShiftDVec::p_LmShortDivisibleBy
  ( TObject* t1, unsigned long sev_t1,
    TObject* t2, unsigned long not_sev_t2, const ring r )
{
  namespace SD = ShiftDVec;

  p_LmCheckPolyRing1(t1->p, r);
  p_LmCheckPolyRing1(t2->p, r);

#if (HAVE_SEV > 2) //BOCO: comments/uncomments sev
  pPolyAssume2
    (p_GetShortExpVector(t1->p, r) == sev_t1, t1->p, r);
  pPolyAssume2
    (p_GetShortExpVector(t2->p, r) == ~ not_sev_t2, t2->p, r);

  if (sev_t1 & not_sev_t2)
  {
    pAssume1(p_LmDivisibleByNoComp(t1->p, t2->p, r) == FALSE);
    return FALSE;
  }
#endif //#if (HAVE_SEV > 0)

  return SD::p_LmDivisibleBy(t1, t2, r, r->isLPring);
}


/* Tests if t1 is divisible by t2
 *
 *BOCO:
 *  original p_LmDivisibleBy( poly, poly, const ring ) resides
 *  in pInline1.h
 */
uint ShiftDVec::p_LmDivisibleBy
  ( TObject * t1, TObject * t2, const ring r, int lV )
{
  namespace SD = ShiftDVec;
  p_LmCheckPolyRing1(t2->p, r);
  pIfThen1(t1->p != NULL, p_LmCheckPolyRing1(t2->p, r));
  if (p_GetComp(t1->p, r) == 0 || p_GetComp(t1->p,r) == p_GetComp(t2->p,r))
    return SD::_p_LmDivisibleByNoComp(t1, t2, r, lV);
  return UINT_MAX;
}


/* returns shift if shifted t2 divides t1, or UINT_MAX if no
 * divisor was found.
 *
 * BOCO:
 *   This Function replaces _p_LmDivisibleByNoComp 
 *   Most of this function, which considered r->VarL_LowIndex,
 *   was just replaced by divisibleByShiftDVec. We do not
 *   consider r->VarL_LowIndex at the moment.
 *
 * IMPORTANT:
 *   We may have to care better for the provided Ring!
 *
 * BOCO original comment:
 *   return: FALSE, if there exists i, such that a->exp[i] > b->exp[i]
 *           TRUE, otherwise
 *   (1) Consider long vars, instead of single exponents
 *   (2) Clearly, if la > lb, then FALSE
 *   (3) Suppose la <= lb, and consider first bits of single exponents in l:
 *       if TRUE, then value of these bits is la ^ lb
 *       if FALSE, then la-lb causes an "overflow" into one of those bits, i.e.,
 *                 la ^ lb != la - lb
 */
static inline uint ShiftDVec::_p_LmDivisibleByNoComp
  ( TObject * t1, TObject * t2, const ring r, int lV )
{
  //BOCO: Well, thats all it does at the moment!
  return t1->SD_Ext()->divisibleBy( t2, lV );
}


/* BOCO:
 * If a shifted lm of a poly T[j] divides lm of L->p, the
 * position of the unshifted polynomial will be returned,
 * otherwise -1 will be returned. Be aware: If the shift of a
 * found divisor is non-zero, then the divisor may not be in T!
 * If a divisor is found, shift will be set to the shift of the
 * divisor (with respect to the corresponding TObject in T). 
 * As usual, the function simply overloads kFindDivisibleByInT.
 *
 * The sevT Argument is not in use. It was just added, in case
 * we need some of the sev stuff later.
 * If uptodeg == 0, we will not care, if the reduction of L->p
 * by T[j] violates the degree bound, otherwise we do (by not
 * returning T[j], if the reduction violates the degree bound).
 *
 * original comment:
 * return -1 if no divisor is found
 *        number of first divisor, otherwise
 */
int ShiftDVec::kFindDivisibleByInT
  ( const TSet &T,
    const unsigned long* sevT, LObject * L, 
    uint & shift, SD::kStrategy strat, const int start )
{
  namespace SD = ShiftDVec;

#if (HAVE_SEV > 0) //BOCO: comments/uncomments sev
  unsigned long not_sev = ~L->sev;
#endif //#if (HAVE_SEV > 0)

  int j = start;
  int ret;  //BOCO: added

  poly p=L->p;
  ring r=currRing;

  //BOCO: TODO: 
  //I think the following line is souperflous -> remove it!
  if (p==NULL)  { r=L->tailRing; p=L->t_p; }

  L->GetLm(p, r);

  //BOCO: added following line
  L->SD_Ext()->SetDVecIfNULL(p, r);


#if (HAVE_SEV > 2) //BOCO: comments/uncomments sev
  pAssume(~not_sev == p_GetShortExpVector(p, r));
#endif //#if (HAVE_SEV > 0)

  if (r == currRing)
  {
    loop
    {
      if (j > strat->tl) {ret = -1; break;};
      /*BOCO:
       * The original p_LmDivisibleBy checks if second arg
       * divides first arg, we check if first arg is divisibly
       * by second arg! */
      T[j].SD_Ext()->SetDVecIfNULL(p, r);
      shift = SD::p_LmDivisibleBy(L,&T[j], r, strat->get_lV());
      int reduction_violates_degree = redViolatesDeg
        ( L, &T[j], strat->get_uptodeg(), currRing );
      if ( shift < UINT_MAX && 
#if (HAVE_SEV > 3) //BOCO: comments/uncomments sev
           !(sevT[j] & not_sev) &&
#endif //#if (HAVE_SEV > 3)
           (strat->homog || !reduction_violates_degree) )
      { ret = j; break; }
      j++;
    }
  }
  else //BOCO: could do both cases in one with function pointer
  {
    loop
    {
      if (j > strat->tl) {ret = -1; break;}
      /*BOCO:
       * The original p_LmDivisibleBy checks if second arg
       * divides first arg, we check if first arg is divisibly
       * by second arg! */
      //TObject tTemp(T[j].t_p);
      shift = p_LmDivisibleBy( L, &T[j], r, strat->get_lV() );
      if ( shift < UINT_MAX && 
#if (HAVE_SEV > 3) //BOCO: comments/uncomments sev
           !(sevT[j] & not_sev) &&
#endif //#if (HAVE_SEV > 3)
           ( strat->homog ||
             !redViolatesDeg
               ( L, &T[j], strat->get_uptodeg(), currRing ) ) )
      {ret = j; break;}
      j++;
    }
  }

  return ret;  //BOCO: added
}


/* BOCO: 
 * This version of the function is used in redtailBba.
 * It overloads its non-dvec version.
 *
 * WARNING:
 * The Returned object may have to be shifted, to devide L,
 * therefor shift will be set.
 * If uptodeg == 0, we will not care, if the reduction of L->p
 * with a found poly p in S violates the degree bound,
 * otherwise we do (by not returning p, if the reduction
 * violates the degree bound).
 */
TObject * ShiftDVec::kFindDivisibleByInS
  ( SD::kStrategy strat, int pos,
    LObject* L, TObject* T, 
    uint & shift, int lV, int uptodeg, long ecart )
{
  namespace SD = ShiftDVec;

  int j = 0;

#if (HAVE_SEV > 0) //BOCO: comments/uncomments sev
  const unsigned long not_sev = ~L->sev;
  const unsigned long* sev = strat->sevS;
#endif //#if (HAVE_SEV > 0)
  poly p;
  ring r;
  L->GetLm(p, r);
  L->SD_Ext()->SetDVecIfNULL(p, r);

#if (HAVE_SEV > 2) //BOCO: comments/uncomments sev
  assume(~not_sev == p_GetShortExpVector(p, r));
#endif //#if (HAVE_SEV > 0)

  if (r == currRing)
  {
    loop
    {
      if (j > pos) return NULL;
      if ( strat->tl < 0 || strat->S_2_R[j] == -1
#if (HAVE_SEV > 3) //BOCO: comments/uncomments sev
           && !(sev[j] & not_sev)                       
#endif //#if (HAVE_SEV > 0)
         )
      {
        /*BOCO:
         * The original p_LmDivisibleBy checks if second arg
         * divides first arg, we check if first arg is divisibly
         * by second arg! */
        sTObject t(strat->S[j]);
        t.SD_Ext()->SetDVec();
        shift = p_LmDivisibleBy(L, &t, r, lV);
        t.SD_Ext()->freeDVec();
      }
      else
      {
        strat->S_2_T(j)->SD_Ext()->SetDVecIfNULL();
        shift = p_LmDivisibleBy(L, strat->S_2_T(j), r, lV);
      }
      if ( shift < UINT_MAX && 
           ( strat->homog ||
             !redViolatesDeg
               ( L->p, strat->S[j], uptodeg,
                 currRing, currRing, strat->tailRing ) ) )
      { break; }
      j++;
    }
    // if called from NF, T objects do not exist:
    if (strat->tl < 0 || strat->S_2_R[j] == -1)
    {
      T->Set(strat->S[j], r, strat->tailRing);
      return T;
    }
    else
      return strat->S_2_T(j);
  }
  else
  {
    TObject* t;
    loop
    {
      if (j > pos) return NULL;
      assume(strat->S_2_R[j] != -1);
#if (HAVE_SEV > 3) //BOCO: comments/uncomments sev
      if (! (sev[j] & not_sev) && (ecart== LONG_MAX || ecart>= strat->ecartS[j]))
#else //BOCO: replacement
      /*BOCO: TODO: Why do we have Problems with this?
      if ( !(ecart== LONG_MAX || ecart>= strat->ecartS[j]) )
      */
      if(true)
#endif //#if (HAVE_SEV > 3)
      {
        t = strat->S_2_T(j);
        assume( t != NULL && t->t_p != NULL &&
                t->tailRing == r && t->p == strat->S[j] );
        strat->S_2_T(j)->SD_Ext()->SetDVecIfNULL(t->t_p, r);
        /*BOCO:
         * The original p_LmDivisibleBy checks if second arg
         * divides first arg, we check if first arg is divisibly
         * by second arg! */
        shift = p_LmDivisibleBy(L, t, r, lV);
        if ( shift < UINT_MAX && 
             ( strat->homog || 
               !redViolatesDeg(p, t->t_p, uptodeg, r, r, r) ) )
        { return t; }
      }
      j++;
    }
  }

/*BOCO: - undefined return value? -
 *  I think it may happen, that the return Value of this
 *  function is undefined. Maybe it would be clearer to return
 *  something
 return NULL;
 */
}


poly ShiftDVec::redtailBba
  ( LObject* L, int pos,
    SD::kStrategy strat, BOOLEAN withT, BOOLEAN normalize )
{
  namespace SD = ShiftDVec;

#define REDTAIL_CANONICALIZE 100
  strat->redTailChange=FALSE;
  if (strat->noTailReduction) return L->GetLmCurrRing();
  poly h, p;
  p = h = L->GetLmTailRing();
  if ((h==NULL) || (pNext(h)==NULL))
    return L->GetLmCurrRing();

  TObject* With;
  // placeholder in case strat->tl < 0
  TObject  With_s(strat->tailRing);

  //BOCO: I think, this initializes Ln.t_p, but not Ln.p
  LObject Ln(pNext(h), strat->tailRing);
  Ln.SD_Ext_Init();
  Ln.pLength = L->GetpLength() - 1;

  pNext(h) = NULL;
  if (L->p != NULL) pNext(L->p) = NULL;
  L->pLength = 1;

  Ln.PrepareRed(strat->use_buckets);

  int cnt=REDTAIL_CANONICALIZE;
  uint shift = 0;

  TObject uTmp;
  uTmp.t_p = NULL;
  uTmp.p = NULL;
  while(!Ln.IsNull())
  {
    assume(h != NULL);
    assume(Ln.p == NULL || pTotaldegree(Ln.p) < 1000);
#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
    shift = 0;
#endif
    loop
    {
      assume(h != NULL);
/* BOCO: original code (deleted because of sev usage)
      Ln.SetShortExpVector();
*/
      if (withT)
      {
        int j;
        j = kFindDivisibleByInT
          ( strat->T, strat->sevT, &Ln, shift, strat );
        assume(h != NULL);

        //BOCO may not be needed, but want to be sure
        Ln.SD_Ext()->freeDVec(); 

        if (j < 0)
        {
          assume(h != NULL);
          break;
        }
        assume(shift < UINT_MAX);

        With = &(strat->T[j]);
        uTmp.p = With->p;
        uTmp.t_p = With->t_p;
        uTmp.tailRing = With->tailRing; //BOCO:may not need that
        if(shift != 0)
        //Our divisor is a shift (and thus not in T or S)
        {
          With->t_p = p_LPshiftT
            ( uTmp.t_p, shift,
              strat->get_uptodeg(),
              strat->get_lV(), strat, strat->tailRing );
          With->p = ::p_mLPshift
            ( uTmp.p, shift,
              strat->get_uptodeg(),
              strat->get_lV(), currRing );
          if(With->t_p) With->p->next = With->t_p->next;
          //BOCO: With->p and With->t_p have to be deleted later, 
          //see below
        }
        assume(h != NULL);
      }
      else
      {
        With = kFindDivisibleByInS
          ( strat, pos, &Ln, &With_s, shift, strat->get_lV() );

        if (With == NULL) { break; }

        uTmp.p = With->p;
        uTmp.t_p = With->t_p;
        if(shift != 0)
        //BOCO: Our divisor is a shift (and thus not in T or S)
        {
          With->t_p = p_LPshiftT
            ( uTmp.t_p, shift,
              strat->get_uptodeg(),
              strat->get_lV(), strat, strat->tailRing );
          With->p = ::p_mLPshift
            ( uTmp.p, shift,
              strat->get_uptodeg(),
              strat->get_lV(), currRing );
          if(With->t_p) With->p->next = With->t_p->next;
          //BOCO: With->p has to be deleted later, see below

          // BOCO: do we need to set With_s.p, or is With_s
          //       just a dummy?
          With_s.p = With->p;
          With_s.SD_Ext()->freeDVec();
        }
        uTmp.tailRing = With->tailRing; //BOCO: added...
      }
      cnt--;
      if (cnt==0)
      {
        cnt=REDTAIL_CANONICALIZE;
        /*poly tmp=*/Ln.CanonicalizeP();
        if (normalize) { Ln.Normalize(); }
      }
      if (normalize && (!TEST_OPT_INTSTRATEGY) && (!nIsOne(pGetCoeff(With->p))))
      {
        With->pNorm();
      }
      strat->redTailChange=TRUE;

      // BOCO: do we need to test for violation of exp bound?
      //       does an exp bound still make sense?

      if ( ShiftDVec::ksReducePolyTail(L, &uTmp, With, &Ln ) )
      {
        // reducing the tail would violate the exp bound
        // set a flag and hope for a retry (in bba)
        strat->completeReduce_retry=TRUE;
        if ((Ln.p != NULL) && (Ln.t_p != NULL)) Ln.p=NULL;
        do
        {
          pNext(h) = Ln.LmExtractAndIter();
          pIter(h);
          L->pLength++;
        } while (!Ln.IsNull());

        goto all_done;

      }

      if (Ln.IsNull())
      {
        assume(Ln.p == NULL);
        goto all_done;
      }
      if (! withT) With_s.Init(currRing);
      // BOCO: p does not correspond to any object in S,T or R,
      //       if the shift is not zero.
      if(shift != 0 && shift < UINT_MAX)
      {
        pDelete(&(With->t_p)); 
        With->t_p = uTmp.t_p; 
        if(With->p) { pLmFree(&(With->p)); }
        With->p = uTmp.p; 
        shift = 0;
      }

      Ln.SD_Ext()->freeDVec();
    }
    assume(h != NULL);

    //BOCO: LmExtr. makes second monomial the lm of Ln
    pNext(h) = Ln.LmExtractAndIter(); 

    Ln.SD_Ext()->freeDVec();

    pIter(h);
    pNormalize(h); 
    // BOCO: why do we have h? Can't we just use Ln.p?
    //       This is confusing.

    L->pLength++;
  }

  all_done:

  if(shift != 0 && shift < UINT_MAX)
  {
    pDelete(&(With->t_p)); 
    With->t_p = uTmp.t_p; 
    if(With->p) { pLmFree(&(With->p)); }
    With->p = uTmp.p; 
    shift = 0;
  }
  Ln.Delete();
  Ln.SD_Ext()->freeDVec();
  if (L->p != NULL) pNext(L->p) = pNext(p);

  if (strat->redTailChange)
  {
    L->last = NULL;
    L->length = 0;
  }

  kTest_L(L);
  With_s.SD_Ext()->freeDVec();
  return L->GetLmCurrRing();
}


KINLINE poly ShiftDVec::redtailBba
  (poly p,int pos,ShiftDVec::kStrategy strat,BOOLEAN normalize)
{
  namespace SD = ShiftDVec;

  LObject L(p, currRing, strat->tailRing);
  return SD::redtailBba(&L, pos, strat,FALSE, normalize);
}


/*2
 * BOCO: original comment
 * updates S:
 * the result is a set of polynomials which are in
 * normalform with respect to S
 */
void ShiftDVec::updateS( BOOLEAN toT, SD::kStrategy strat )
{
  namespace SD = ShiftDVec;

  LObject h;
  int i, suc=0;
  poly redSi=NULL;
  BOOLEAN change,any_change;
  any_change=FALSE;
  if (currRing->OrdSgn==1)
  {
    while (suc != -1)
    {
      i=suc+1;
      while (i<=strat->sl)
      {
        change=FALSE;
        if (((strat->fromQ==NULL) || (strat->fromQ[i]==0)) && (i>0))
        {
          redSi = pHead(strat->S[i]);
          LObject h(strat->S[i]);
          strat->S[i] = redBba(&h,i-1,strat);
          /*BOCO:
           *   wouldnt it be more efficient, if the reduction
           *   algorithm would notice, if strat->S[i] was
           *   reduced and would inform about that?
           */
          if (pCmp(redSi,strat->S[i])!=0)
          {
            change=TRUE;
            any_change=TRUE;
            if (TEST_OPT_PROT)
            {
              if (strat->S[i]==NULL)
                PrintS("V");
              else
                PrintS("v");
              mflush();
            }
          }
          pLmDelete(&redSi);
          if (strat->S[i]==NULL)
          {
            deleteInS(i,strat);
            i--;
          }
          else if (change)
          {
            if (TEST_OPT_INTSTRATEGY)
            {
              if (TEST_OPT_CONTENTSB)
                {
                  number n;
                  p_Cleardenom_n(strat->S[i], currRing, n);// also does a pContent
                  if (!nIsOne(n))
                    {
                      denominator_list denom=(denominator_list)omAlloc(sizeof(denominator_list_s));
                      denom->n=nInvers(n);
                      denom->next=DENOMINATOR_LIST;
                      DENOMINATOR_LIST=denom;
                    }
                  nDelete(&n);
                }
              else
                {
                  //pContent(strat->S[i]);
                  strat->S[i]=p_Cleardenom(strat->S[i], currRing);// also does a pContent
                }
            }
            else
            {
              pNorm(strat->S[i]);
            }
#if (HAVE_SEV > 1) //BOCO: comments/uncomments sev
            strat->sevS[i] = pGetShortExpVector(strat->S[i]);
#endif //#if (HAVE_SEV > 1)
          }
        }
        i++;
      }
      /* BOCO: 
       *   Commentary from kutil.cc for reoderS:
       *   reoders S with respect to posInS, suc is the first
       *   changed index or zero. 
       * BOCO to this commentary:
       *   So in the next iteration of the reduction of S, we
       *   will not try to reduce those polynomials in S, which
       *   have not changed their position in the previous
       *   pass, as they could, due to the order of S, only be
       *   interreduced by themselves, as the other polynomials
       *   should have a higher order, but as they did not
       *   change their positions, they didnt interreduce
       *   themselves in the previous pass and would thus never
       *   interreduce themselves again.  This is at least my
       *   understanding of the code. (I may be wrong.)
       */
      if (any_change) reorderS(&suc,strat);
      else break;
    }
    if (toT)
      /* BOCO: 
       *   Seems to put the reduced Elements also to T, but
       *   without deleting the old Elements from T, which are
       *   not reduced (is the latter really true?).  Again
       *   there are some things i dont understand yet here.
       */
    {
      for (i=0; i<=strat->sl; i++)
      {
        if ((strat->fromQ==NULL) || (strat->fromQ[i]==0))
        {
          LObject L(h.p, currRing, strat->tailRing); 
          h.p = SD::redtailBba(&L, i-1, strat, FALSE, FALSE);
          if (TEST_OPT_INTSTRATEGY)
          {
            h.pCleardenom();// also does a pContent
          }
        }
        else
        {
          h.p = strat->S[i];
        }
        strat->initEcart(&h);
        if (strat->honey)
        {
          strat->ecartS[i] = h.ecart;
        }
#if (HAVE_SEV > 1) //BOCO: comments/uncomments sev
        if (strat->sevS[i] == 0) {strat->sevS[i] = pGetShortExpVector(h.p);}
        else assume(strat->sevS[i] == pGetShortExpVector(h.p));
        h.sev = strat->sevS[i];
#endif //#if (HAVE_SEV > 1)
        /*puts the elements of S also to T*/
        strat->initEcart(&h);
        enterT(h,strat);
        strat->S_2_R[i] = strat->tl;
      }
    }
  }

#ifdef KDEBUG
  kTest(strat);
#endif
}


void ShiftDVec::initBuchMoraCrit(ShiftDVec::kStrategy strat)
{
  namespace SD = ShiftDVec;

  strat->enterOnePair=NULL;
  strat->chainCrit=NULL;

#ifdef HAVE_RINGS //BOCO: we dont have them
  if (rField_is_Ring(currRing))
  {
    strat->enterOnePair=enterOnePairRing;
    strat->chainCrit=chainCritRing;
  }
#endif
#ifdef HAVE_RATGRING //BOCO: i think, we dont have that, too
  if (rIsRatGRing(currRing))
  {
     strat->chainCrit=chainCritPart;
     /* enterOnePairNormal get rational part in it */
  }
#endif

  strat->sugarCrit =        TEST_OPT_SUGARCRIT;
  strat->Gebauer =          strat->homog || strat->sugarCrit;
  strat->honey =            !strat->homog || strat->sugarCrit || TEST_OPT_WEIGHTM;
  if (TEST_OPT_NOT_SUGAR) strat->honey = FALSE;
  strat->pairtest = NULL;
  /* alway use tailreduction, except:
  * - in local rings, - in lex order case, -in ring over extensions */
  strat->noTailReduction = !TEST_OPT_REDTAIL;

#ifdef HAVE_PLURAL
  // and r is plural_ring
  //  hence this holds for r a rational_plural_ring
  if( rIsPluralRing(currRing) || (rIsSCA(currRing) && !strat->z2homog) )
  {    //or it has non-quasi-comm type... later
    strat->sugarCrit = FALSE;
    strat->Gebauer = FALSE;
    strat->honey = FALSE;
  }
#endif

#ifdef HAVE_RINGS //BOCO: no, we don't
  // Coefficient ring?
  if (rField_is_Ring(currRing))
  {
    strat->sugarCrit = FALSE;
    strat->Gebauer = FALSE ;
    strat->honey = FALSE;
  }
#endif
  #ifdef KDEBUG
  if (TEST_OPT_DEBUG)
  {
    if (strat->homog) PrintS("ideal/module is homogeneous\n");
    else              PrintS("ideal/module is not homogeneous\n");
  }
  #endif
}


/* BOCO: original comment from kutil.cc
 *  enters p at position at in L
 * BOCO:
 *  - We will return a pointer to the LObject in [[set]] .
 * WARNING (if an error occeurs: check):
 *  p is now is passed as a pointer instead of passing it by
 *  value (may save us one copy).
 */
LObject* ShiftDVec::enterL
  ( LSet *set,
    int *length, int *LSetmax, LObject* p, int at )
{
  namespace SD = ShiftDVec;

  int i;
  // this should be corrected
  assume(p->FDeg == p->pFDeg());

  if ((*length)>=0)
  {
    if ( (*length) == (*LSetmax)-1 )
      enlargeL(set,LSetmax,setmaxLinc);
    if (at <= (*length))
#ifdef ENTER_USE_MEMMOVE
      memmove( &((*set)[at+1]), &((*set)[at]),
               ((*length)-at+1)*sizeof(LObject) );
#else
    for (i=(*length)+1; i>=at+1; i--) (*set)[i] = (*set)[i-1];
#endif
  }
  else at = 0;

  (*length)++;
  (*set)[at] = *p;

  return &(*set)[at];
}


//functions with nearly no change


void ShiftDVec::initSL( ideal F, ideal Q, SD::kStrategy strat )
{
  namespace SD = ShiftDVec;

  int   i,pos;

  if (Q!=NULL) i=((IDELEMS(Q)+(setmaxTinc-1))/setmaxTinc)*setmaxTinc;
  else i=setmaxT;
  strat->ecartS=initec(i);
  strat->sevS=initsevS(i);
  strat->S_2_R=initS_2_R(i);
  strat->fromQ=NULL;
  strat->Shdl=idInit(i,F->rank);
  strat->S=strat->Shdl->m;
  /*- put polys into S -*/
  if (Q!=NULL)
  {
    strat->fromQ=initec(i);
    memset(strat->fromQ,0,i*sizeof(int));
    for (i=0; i<IDELEMS(Q); i++)
    {
      if (Q->m[i]!=NULL)
      {
        LObject h;
        h.p = pCopy(Q->m[i]);
        if (currRing->OrdSgn==-1)
        {
          deleteHC(&h,strat);
        }
        if (TEST_OPT_INTSTRATEGY)
        {
          //pContent(h.p);
          h.pCleardenom(); // also does a pContent
        }
        else
        {
          h.pNorm();
        }
        if (h.p!=NULL)
        {
          strat->initEcart(&h);
          if (strat->sl==-1)
            pos =0;
          else
          {
            pos = posInS(strat,strat->sl,h.p,h.ecart);
          }
          h.sev = pGetShortExpVector(h.p);
          strat->enterS(h,pos,strat,-1);
          strat->fromQ[pos]=1;
        }
      }
    }
  }
  for (i=0; i<IDELEMS(F); i++)
  {
    if (F->m[i]!=NULL)
    {
      LObject h;

      h.p = pCopy(F->m[i]);
      if (h.p!=NULL)
      {
        if (currRing->OrdSgn==-1)
        {
          cancelunit(&h);  /*- tries to cancel a unit -*/
          deleteHC(&h, strat);
        }
        if (h.p!=NULL)
        {
          if (TEST_OPT_INTSTRATEGY)
          {
            //pContent(h.p);
            h.pCleardenom(); // also does a pContent
          }
          else
          {
            h.pNorm();
          }
          strat->initEcart(&h);
          if (strat->Ll==-1)
            pos =0;
          else
            pos = strat->posInL(strat->L,strat->Ll,&h,strat);
          h.sev = pGetShortExpVector(h.p);
          h.SD_Object_Extension = NULL;
          SD::enterL(&strat->L,&strat->Ll,&strat->Lmax,&h,pos);
        }
      }
    }
  }
  /*- test, if a unit is in F -*/

  if ((strat->Ll>=0)
#ifdef HAVE_RINGS
       && n_IsUnit(pGetCoeff(strat->L[strat->Ll].p), currRing->cf)
#endif
       && pIsConstant(strat->L[strat->Ll].p))
  {
    while (strat->Ll>0) deleteInL(strat->L,&strat->Ll,strat->Ll-1,strat);
  }
}

void ShiftDVec::initBuchMora
  ( ideal F,ideal Q,SD::kStrategy strat )
{
  namespace SD = ShiftDVec;

  strat->interpt = BTEST1(OPT_INTERRUPT);
  strat->kHEdge=NULL;
  if (currRing->OrdSgn==1) strat->kHEdgeFound=FALSE;
  /*- creating temp data structures------------------- -*/
  strat->cp = 0;
  strat->c3 = 0;
  strat->tail = pInit();
  /*- set s -*/
  strat->sl = -1;
  /*- set L -*/
  strat->Lmax = ((IDELEMS(F)+setmaxLinc-1)/setmaxLinc)*setmaxLinc;
  strat->Ll = -1;
  strat->initL(F);
  /*- set B -*/
  strat->Bmax = setmaxL;
  strat->Bl = -1;
  strat->initB();
  /*- set T -*/
  strat->tl = -1;
  strat->tmax = setmaxT;
  strat->T = initT();
  strat->R = initR();
  strat->sevT = initsevT();
  /*- init local data struct.---------------------------------------- -*/
  strat->P.ecart=0;
  strat->P.length=0;
  if (currRing->OrdSgn==-1)
  {
    if (strat->kHEdge!=NULL) pSetComp(strat->kHEdge, strat->ak);
    if (strat->kNoether!=NULL) pSetComp(strat->kNoetherTail(), strat->ak);
  }
  /*Shdl=*/SD::initSL(F, Q,strat); /*sets also S, ecartS, fromQ */
  //debug marker: after ShiftDVec::initSL
  strat->fromT = FALSE;
  strat->noTailReduction = !TEST_OPT_REDTAIL;
  if (!TEST_OPT_SB_1)
  {
    SD::updateS(TRUE,strat);
  }
  if (strat->fromQ!=NULL)
    omFreeSize(strat->fromQ,IDELEMS(strat->Shdl)*sizeof(int));
  strat->fromQ=NULL;
}
