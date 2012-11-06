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
#include <kernel/kutil2.h> //already included in kutil.h
#include <kernel/shiftDVec.h>

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

namespace ShiftDVec{
  class sTObject;

  uint divisibleBy
    ( const ShiftDVec::sTObject * t1, 
      const ShiftDVec::sTObject * t2, int numVars );

  uint findRightOverlaps
    ( const ShiftDVec::sTObject * t1, 
      const ShiftDVec::sTObject * t2, 
      int numVars, int maxDeg, uint ** overlaps );

//  bool redViolatesDeg
//    ( poly a, poly b, int uptodeg, ring ar, ring br );
  bool createSPviolatesDeg
    (poly a, poly b, uint shift, int uptodeg);
};


//extracted inlines from kutil.cc


/*2
*utilities for TSet, LSet
*/
//BOCO: copied from kutil.cc
inline static intset initec (const int maxnr)
{
  //loGriToFile("omAlloc in intset initec by return ", maxnr*sizeof(int),1024);
  return (intset)omAlloc(maxnr*sizeof(int));
}

//BOCO: copied from kutil.cc
static inline void enlargeL (LSet* L,int* length,const int incr)
{
  assume((*L)!=NULL);
  assume((length+incr)>0);

  *L = (LSet)omReallocSize((*L),(*length)*sizeof(LObject),
                                   ((*length)+incr)*sizeof(LObject));
  (*length) += incr;
}

//BOCO: copied from kutil.cc
inline static unsigned long* initsevS (const int maxnr)
{
  //loGriToFile("omAlloc0 in initsevS by return ",maxnr*sizeof(unsigned long),1024);
  return (unsigned long*)omAlloc0(maxnr*sizeof(unsigned long));
}

//BOCO: copied from kutil.cc
inline static int* initS_2_R (const int maxnr)
{
  //loGriToFile("omAlloc0 in initS_2_R by return ",maxnr*sizeof(int),1024);
  return (int*)omAlloc0(maxnr*sizeof(int));
}


//the work begins


#if 0 //BOCO: original header (replaced)
ideal freegb(ideal I, int uptodeg, int lVblock)
{
#else //BOCO: replacement
ideal ShiftDVec::freegbdvc
  (ideal I, int uptodeg, int lVblock, long deBoGriFlags)
{
  namespace SD = ShiftDVec;

#if DEBOGRI > 0
  SD::deBoGri = deBoGriFlags;
  SD::lpDVCase = 1;
#endif

#endif
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
#if 0 //BOCO: original code (replaced)
  ideal RS = 
    kStdShift(I,NULL, testHomog, NULL,NULL,0,0,NULL, uptodeg, lVblock);
#else //BOCO: replacement
  ideal RS = SD::kStd
    (I,NULL, testHomog, NULL,NULL,0,0,NULL, uptodeg, lVblock);
#endif
    //bbaShift(I,NULL, NULL, NULL, strat, uptodeg, lVblock);
  idSkipZeroes(RS);
  return(RS);
}


#if 0 //BOCO: original header (replaced)
ideal kStd
  ( ideal F, ideal Q, tHomog h, intvec ** w, 
    intvec *hilb, int syzComp, int newIdeal, intvec *vw )
{
#else //BOCO: replacement
ideal ShiftDVec::kStd
  ( ideal F, ideal Q, tHomog h, intvec ** w, intvec *hilb,
    int syzComp, int newIdeal, intvec *vw, int uptodeg, int lV )
{
  namespace SD = ShiftDVec;
#endif
  if(idIs0(F))
    return idInit(1,F->rank);

  ideal r;
  BOOLEAN b=currRing->pLexOrder,toReset=FALSE;
  BOOLEAN delete_w=(w==NULL);
  kStrategy strat=new skStrategy;

#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
  strat->lV = lV;
  strat->uptodeg = uptodeg;
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
#if 0 //BOCO: replaced
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
      if (w!=NULL)
        r=SD::bba(F,Q,*w,hilb,strat);
      else
        r=SD::bba(F,Q,NULL,hilb,strat);
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


#if 0 //BOCO: original header (replaced)
ideal bba
  (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat)
{
#else //replacement
ideal ShiftDVec::bba
  (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat)
{
  namespace SD = ShiftDVec;
#endif
#ifdef KDEBUG
  bba_count++;
  int loop_count = 0;
#endif /* KDEBUG */
  om_Opts.MinTrack = 5;
  int   red_result = 1;
  int   olddeg,reduc;
  int hilbeledeg=1,hilbcount=0,minimcnt=0;
  BOOLEAN withT = FALSE;
#if 0 //BOCO: original code (replaced)
  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
#else //BOCO: replacement
  SD::initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
#endif
  initBuchMoraPos(strat);
  initHilbCrit(F,Q,&hilb,strat);
#if 0 //BOCO: original code (replaced)
  initBba(F,strat);
  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  /*Shdl=*/initBuchMora(F, Q,strat);
#else //BOCO: replacement
  SD::initBba(F,strat);
  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  /*Shdl=*/SD::initBuchMora(F, Q,strat);
#endif
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

//#define DEBOGRI_GROBITEST
#ifdef DEBOGRI_GROBITEST
  for(int i = 0; i <= strat->Ll; ++i)
  {
    deBoGriPrint("------------------------------------", 1024);
    deBoGriPrint(strat->L[i].p, "strat->L[i].p: ", 1024);
//    poly pTest = p_LPshiftT
//      ( strat->L[i].p, 1, 
//        strat->uptodeg, strat->lV, strat, currRing );
    poly pTest = ShiftDVec::p_mLPshift
      ( pNext(strat->L[i].p), 1, 
        strat->uptodeg, strat->lV, strat->tailRing );
    deBoGriPrint(pTest, "pTest: ", 1024);
    pDelete(&pTest);
    deBoGriPrint(strat->L[i].p, "strat->L[i].p: ", 1024);
    deBoGriPrint("------------------------------------", 1024);
  }
#endif

ShiftDVec::InitSDMultiplication(strat->tailRing, strat);
ShiftDVec::InitSDMultiplication(currRing, strat);

#ifdef KDEBUG
  //kDebugPrint(strat);
#endif
  /* compute------------------------------------------------------- */
  while (strat->Ll >= 0)
  {
    deBoGriTTest(strat);
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

    deBoGriTTest(strat);
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
      deBoGriTTest(strat);
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
#if 0 //BOCO: original code (replaced)
          strat->P.p = 
            redtailBba(&(strat->P),pos-1,strat, withT);
#else //replacement

          strat->P.p = 
            SD::redtailBba(&(strat->P),pos-1,strat, withT);
#endif
          strat->P.pCleardenom();
        }
      }
      else
      {
        strat->P.pNorm();
        if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
          deBoGriPrint("Red Tail.\n", 2048);
#if 0 //BOCO: original code (replaced)
          strat->P.p = 
            redtailBba(&(strat->P),pos-1,strat, withT);
#else //replacement
          strat->P.p = 
            SD::redtailBba(&(strat->P),pos-1,strat, withT);
#endif
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
      
      deBoGriTTest(strat);
      strat->P.SetDVecIfNULL();
      enterT(strat->P, strat);

#if 0 //BOCO: original code - this was once a (bad?) idea
      /* BOCO: this piece of code was moved up from below (i
       * hope that works) */
      // posInS only depends on the leading term
      strat->enterS(strat->P, pos, strat, strat->tl);
#endif

#ifdef HAVE_RINGS
      if (rField_is_Ring(currRing))
        superenterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat, strat->tl);
      else
#endif
      deBoGriTTest(strat);
#if 0 //BOCO: original code (replaced) 
        enterpairs
          ( strat->P.p,strat->sl,
            strat->P.ecart,pos,strat, strat->tl );
#else //BOCO: replacement
        /* BOCO: we added strat->P, thus we should only 
         * increment up to strat->sl-1 
         * Update: no, we no longer add strat->sl before
         * invoking this funktion. It was an Idea to simplify
         * the code ... but it didn't work with singular. */
        SD::enterpairs( &(strat->P),strat->sl,strat->P.ecart,
                        pos,strat,strat->tl                    );
#endif
      deBoGriTTest(strat);
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
      deBoGriTTest(strat);
      if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);
//      Print("[%d]",hilbeledeg);
      if (strat->P.lcm!=NULL)
      {
#ifdef HAVE_RINGS
        pLmDelete(strat->P.lcm);
#else
        loGriToFile("pLmFree in bba in shiftDVec.cc ",0 ,1024, (void*)strat->P.lcm);
        pLmFree(strat->P.lcm);
#endif
      }
    }else{
#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
      strat->P.freeDVec();  
#endif
    }
    deBoGriTTest(strat);

#if 0 //BOCO: replaced
    else if (strat->P.p1 == NULL && strat->minim > 0)
    {
      p_Delete(&strat->P.p2, currRing, strat->tailRing);
    }
#else //BOCO: replacement
      //BOCO: we always want to delete the second poly, because
      //      it is a shift and will not be used anywhere else
    //deBoGriTTest(strat);
    if(strat->P.p2){
      loGriToFile("p_Delete in bba in shiftDVec.cc ",0 ,1024, (void*)strat->P.p2);
      //deBoGriTTest(strat);
      deBoGriPrint("------------------------------------", 1024);
      deBoGriPrint("Printing strat->R[strat->P.i_r2]->p:", 1024);
      deBoGriPrint(strat->R[strat->P.i_r2]->p, "", 1024);
      deBoGriPrint("Printing address of strat->R[strat->P.i_r2]->p:", 1024);
      deBoGriPrint(&(strat->R[strat->P.i_r2]->p), "", 1024);
      deBoGriPrint("Printing strat->P.p2:", 1024);
      deBoGriPrint(strat->P.p2, "", 1024);
      deBoGriPrint("Printing address of strat->P.p2:", 1024);
      deBoGriPrint(&(strat->P.p2), "", 1024);
      pDelete(&strat->P.p2);
      deBoGriPrint("After p_delete(strat->P.p2)", 1024);
      deBoGriPrint("Printing strat->R[strat->P.i_r2]->p:", 1024);
      deBoGriPrint(strat->R[strat->P.i_r2]->p, "", 1024);
      deBoGriPrint("------------------------------------", 1024);
      //deBoGriTTest(strat);
      strat->P.p2 = NULL;
    }
#endif

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

  //BOCO: delete dvecs - have to do that manually, 
  //destructors for TObjects seem not compatible with singulars
  //philosophy - at least not at the time, i did this
  for(int j = 0; j <= strat->tl; ++j)
  {
    strat->T[j].freeDVec();
  }

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
#if 0 //BOCO: original code (replaced)
    completeReduce(strat);
#else
    SD::completeReduce(strat);
#endif
#ifdef HAVE_TAIL_RING
    if (strat->completeReduce_retry)
    {
      // completeReduce needed larger exponents, retry
      // to reduce with S (instead of T)
      // and in currRing (instead of strat->tailRing)
      cleanT(strat);strat->tailRing=currRing;
      int i;
      for(i=strat->sl;i>=0;i--) strat->S_2_R[i]=-1;
#if 0 //BOCO: original code (replaced)
      completeReduce(strat);
#else
      SD::completeReduce(strat);
#endif
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


ideal ShiftDVec::initTestGM
  (ideal I, poly p, int uptodeg, int lVblock)
{
  namespace SD = ShiftDVec;

  if (! ideal_isInV(I,lVblock) )
  {
    WerrorS("The input ideal contains incorrectly encoded elements! ");
    return(NULL);
  }

  TObject* H = new TObject(p);
  TSet tset = new TObject[IDELEMS(I)];

  for(int i=0; i < IDELEMS(I); ++i)
    tset[i].Set(I->m[i], currRing);

  SD::DeBoGriTestGM(tset,IDELEMS(I), H,uptodeg,lVblock);

  delete [] tset;
  delete H;
  return I;

}


/*2
 * in the case of a standardbase of a module over a qring:
 * replace polynomials in i by ak vectors,
 * (the polynomial * unit vectors gen(1)..gen(ak)
 * in every case (also for ideals:)
 * deletes divisible vectors/polynomials
 * BOCO: original resides in kutil.cc
 */
void ShiftDVec::updateResult(ideal r,ideal Q, kStrategy strat)
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
#if 0 //BOCO: original code replaced
          if ((Q->m[q]!=NULL)
          &&(pLmDivisibleBy(Q->m[q],r->m[l])))
#else //replacement
          /*BOCO:
           * The original pLmDivisibleBy checks if second arg
           * divides first arg, we check if first arg is
           * divisibly by second arg! */
          ShiftDVec::sTObject t1(r->m[l]);
          ShiftDVec::sTObject t2(Q->m[q]); 
          uint shift = 
            p_LmDivisibleBy(&t1, &t2, currRing, strat->lV);
          if ( (Q->m[q]!=NULL) && shift < UINT_MAX &&
               ( strat->homog ||
                !redViolatesDeg
                  ( r->m[l], r->m[q], strat->uptodeg, 
                    currRing, currRing, strat->tailRing ) ) )
#endif
          {
            if (TEST_OPT_REDSB)
            {
              p=r->m[l];
              r->m[l]=kNF(Q,NULL,p);
              loGriToFile("pDelete(&p) in updateResult ", 0,1024,(void*)p);
              pDelete(&p);
            }
            else
            {
              loGriToFile("pDelete(&p) in updateResult ", 0,1024,(void*) r->m[1]);
              pDelete(&r->m[l]); // and set it to NULL
            }
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
              loGriToFile("pDelete(&p) in updateResult ", 0,1024,(void*)p);
              pDelete(&p);
              reduction_found=TRUE;
            }
            else
            {
              loGriToFile("pDelete(&r->m[1]) in updateResult ", 0,1024,(void*) r->m[1]);
              pDelete(&r->m[l]); // and set it to NULL
            }
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
#if 0 //BOCO: original code replaced
            if ((l!=q)
            && (r->m[q]!=NULL)
            &&(pLmDivisibleBy(r->m[l],r->m[q])))
#else //replacement
          /*BOCO:
           * The original pLmDivisibleBy checks if second arg
           * divides first arg, we check if first arg is
           * divisibly by second arg! */
          ShiftDVec::sTObject t1(r->m[q]); 
          ShiftDVec::sTObject t2(r->m[l]);
          uint shift = 
            p_LmDivisibleBy(&t1, &t2, currRing, strat->lV);
          if ( (l!=q) && 
               (shift < UINT_MAX) && (r->m[q]!=NULL) &&
               ( strat->homog ||
                 !redViolatesDeg
                   ( r->m[q], r->m[l], strat->uptodeg,
                     currRing, currRing, strat->tailRing ) ) )
#endif
            {
              loGriToFile("pDelete(&r->m[q]) in updateResult ", 0,1024,(void*) r->m[q]);
              pDelete(&r->m[q]);
            }
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

#if 0 //BOCO: original header (replaced)
void completeReduce (kStrategy strat, BOOLEAN withT)
{
#else //replacement
void ShiftDVec::completeReduce (kStrategy strat, BOOLEAN withT)
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
    if ((strat->fromQ!=NULL) && (strat->fromQ[i])) continue; // do not reduce Q_i
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
#if 0 //BOCO: original code (replaced)
        strat->S[i] = redtailBba(&L, end_pos, strat, withT);
#else //replacement
        strat->S[i] = SD::redtailBba(&L, end_pos, strat, withT);
#endif
      else
#if 0 //BOCO: original code (replaced)
        strat->S[i] = redtail(&L, strat->sl, strat);
#else //replacement
        strat->S[i] = SD::redtail(&L, strat->sl, strat);
#endif
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
#if 0 //BOCO: original code (replaced)
        strat->S[i] = redtailBba(strat->S[i], end_pos, strat, withT);
#else //replacement
        strat->S[i] = SD::redtailBba(strat->S[i], end_pos, strat, withT);
#endif
      else
#if 0 //BOCO: original code (replaced)
        strat->S[i] = redtail(strat->S[i], strat->sl, strat);
#else //replacement
        strat->S[i] = SD::redtail(strat->S[i], strat->sl, strat);
#endif
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
#if 0 //BOCO: original header (replaced)
poly redtail (LObject* L, int pos, kStrategy strat)
{
#else //replacement
poly ShiftDVec::redtail (LObject* L, int pos, kStrategy strat)
{
  namespace SD = ShiftDVec;
#endif
  poly h, hn;
#if (HAVE_SEV > 0) //BOCO: comments/uncomments sev
  unsigned long not_sev;
#endif //#if (HAVE_SEV > 0)
  strat->redTailChange=FALSE;

  poly p = L->p;
  if (strat->noTailReduction || pNext(p) == NULL)
    return p;

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
#if 0 //BOCO: original code (replaced)
      if (strat->kHEdgeFound)
        With = kFindDivisibleByInS(strat, pos, &Ln, &With_s);
      else
        With = kFindDivisibleByInS(strat, pos, &Ln, &With_s, e);
      if (With == NULL) break;
#else //replacement
        TObject * WithTmp = SD::kFindDivisibleByInS
          (strat, pos, &Ln, &With_s, shift, strat->lV, ecart);
        if (WithTmp == NULL) break;
        if(shift == 0){ With = WithTmp; } //no problem
        else //Our divisor is a shift (and thus not in T or S)
        {
          With = new TObject(*WithTmp); //should do just a shallow copy
          With->p = p_LPshiftT
            ( WithTmp->p, shift, strat->uptodeg, strat->lV, 
              strat, currRing                               );
          loGriToFile("p_LPshiftT in redtail ", 0,1024,(void*) With->p);
          /* BOCO: With->p has to be deleted later; i hope we
           * can just choose currRing for p_LPshift */
          
          //BOCO: Is that right?:
          With_s.p = With->p;
        }
#endif
      With->length=0;
      With->pLength=0;
      strat->redTailChange=TRUE;

      if ( ShiftDVec::ksReducePolyTail
             (L, WithTmp, With, h, strat->kNoetherTail()), strat )
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
#if 1 //original code (replaced)
      hn = pNext(h);
#else //replacement shrink not needed so rereplaced @GRICO
      if (pNext(h) != NULL && !strat->homog ){
        hn=p_Shrink(pNext(h),strat->lV,strat->tailRing);
      }
#endif
      if (hn == NULL) goto all_done;
      op = strat->tailRing->pFDeg(hn, strat->tailRing);
      if ((Kstd1_deg>0)&&(op>Kstd1_deg)) goto all_done;
      e = strat->tailRing->pLDeg(hn, &l, strat->tailRing) - op;
      if(shift != 0){ 
        loGriToFile("pDelete in redtail ", 0,1024, (void*) With->p);
        pDelete(&With->p); 
        shift=0; 
        With->dvec = NULL; 
        delete With;
      }
    }
    h = hn;
    hn = pNext(h);
  }

  all_done:
  if(shift != 0)
  { 
    loGriToFile("pDelete in redtail ", 0,1024, (void*) With->p);
    pDelete(&With->p); 
    With->dvec = NULL;
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

#if 0 //BOCO: original header (replaced) from kutil.cc
poly redtail (poly p, int pos, kStrategy strat)
{
#else //replacement
poly ShiftDVec::redtail(poly p, int pos, kStrategy strat)
{
  namespace SD = ShiftDVec;
#endif
  LObject L(p, currRing);
  return SD::redtail(&L, pos, strat);
}


#if 0 //BOCO: original header (replaced) from kstd1.cc
void initBba(ideal F,kStrategy strat)
#else
void ShiftDVec::initBba(ideal F,kStrategy strat)
#endif
{
#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
  namespace SD = ShiftDVec;
#endif

  int i;
  idhdl h;
 /* setting global variables ------------------- */
  strat->enterS = enterSBba;
    strat->red = redHoney;
#if 0 //BOCO: original code (deleted)
      //BOCO: We do not consider redHoney/redLazy at the moment
  if (strat->honey)
    strat->red = redHoney;
  else if (currRing->pLexOrder && !strat->homog)
    strat->red = redLazy;
  else
  {
#endif
    strat->LazyPass *=4;
#if 0 //BOCO: original code (replaced)
    strat->red = redHomog;
#else //BOCO: replacement
    strat->red = SD::redHomog;
#endif
//} //BOCO: original code (deleted) see above

#if 0 //BOCO: original code (deleted) 
      //BOCO: We do not consider this option
#ifdef HAVE_RINGS  //TODO Oliver
  if (rField_is_Ring(currRing))
  {
    strat->red = redRing;
  }
#endif
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
//    //interred  machen   Aenderung
//    strat->pOrigFDeg=pFDeg;
//    strat->pOrigLDeg=pLDeg;
//    //h=ggetid("ecart");
//    //if ((h!=NULL) /*&& (IDTYP(h)==INTVEC_CMD)*/)
//    //{
//    //  ecartWeights=iv2array(IDINTVEC(h));
//    //}
//    //else
//    {
//      ecartWeights=(short *)omAlloc(((currRing->N)+1)*sizeof(short));
//      /*uses automatic computation of the ecartWeights to set them*/
//      kEcartWeights(F->m,IDELEMS(F)-1,ecartWeights);
//    }
//    pRestoreDegProcs(currRing,totaldegreeWecart, maxdegreeWecart);
//    if (TEST_OPT_PROT)
//    {
//      for(i=1; i<=(currRing->N); i++)
//        Print(" %d",ecartWeights[i]);
//      PrintLn();
//      mflush();
//    }
//  }
}


/* reduction procedure for the homogeneous case
 * and the case of a degree-ordering
 * original resides in kstd2.cc
 */
int ShiftDVec::redHomog (LObject* h,kStrategy strat)
{
  initDeBoGri
    ( ShiftDVec::indent, 
      "Entering redHomog", "Leaving redHomog", 1024 );
#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
  namespace SD = ShiftDVec;
#endif

  if (strat->tl<0) return 1;
  //if (h->GetLmTailRing()==NULL) return 0; // HS: SHOULD NOT BE NEEDED!
  assume(h->FDeg == h->pFDeg());

  poly h_p;
  int i,j,at,pass, ii;
#if (HAVE_SEV > 0) //BOCO: comments/uncomments sev
  unsigned long not_sev;
#endif //#if (HAVE_SEV > 0)
  long reddeg,d;

  pass = j = 0;
  d = reddeg = h->GetpFDeg();
#if (HAVE_SEV > 1)
  h->SetShortExpVector();
#endif //#if (HAVE_SEV > 1)
  int li;
  h_p = h->GetLmTailRing();
#if (HAVE_SEV > 1) //BOCO: comments/uncomments sev
  not_sev = ~ h->sev;
#endif //#if (HAVE_SEV > 1)
  loop
  {
#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
    uint shift = 0;
#endif
#if 0 //BOCO: original code (replaced)
    j=kFindDivisibleByInT(strat->T, strat->sevT, strat->tl, h);
#else //BOCO: replacement
    j = SD::kFindDivisibleByInT
      ( strat->T, strat->sevT, h, shift, strat );
#endif
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
#if 0 //BOCO: original code (replaced)
  if ((strat->T[i].pLength < li)
     &&
      p_LmShortDivisibleBy(strat->T[i].GetLmTailRing(), strat->sevT[i],
                           h_p, not_sev, strat->tailRing))
#endif //BOCO: replacement (I hope, that works...)
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
        if(shift < UINT_MAX){
          li = strat->T[i].pLength;
          ii = i;
        }
        t_h.freeDVec();
        t_i.freeDVec();
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

#if 0 //BOCO: original code (replaced)
    ksReducePoly(h, &(strat->T[ii]), NULL, NULL, strat);
#else //BOCO: replacement
#if 0 //BOCO: old and bad idea (does not work with tailRing)
    poly pTemp;
    if(shift > 0){
      pTemp = p_LPshiftT
        ( strat->T[ii].p, shift, strat->uptodeg, strat->lV, 
          strat, currRing                                   );
      loGriToFile("p_LPshiftT in redHomoq ", 0,1024,(void*) pTemp);
    } else pTemp = strat->T[ii].p;
    TObject tTemp(pTemp);
    ksReducePoly(h,&tTemp,strat->kNoetherTail());
    h->freeDVec(); //BOCO: after reduction lm has changed

    /* BOCO: We are in homogenous case, aren't we? So we do
     * not have to shrink.
     * BOCO: this was copied from redFirstShrink
     * i only additionally test for strat->homog - why wasn't
     * that done in redFirstShrink
     * some other time, i should review redBbaShift
    if (!h->IsNull() && !strat->homog ){
      poly qq=p_Shrink(h->GetTP(),strat->lV,strat->tailRing);
      h->p=NULL;
      h->t_p=qq;
      if (qq!=NULL) h->GetP(strat->lmBin);
    }
     */

    if(shift > 0)
    { 
      loGriToFile("pDelete in redHomoq ", 0,1024,(void*) pTemp);
      //deBoGriPrint(strat->T[0].p, "Printing strat->T[0]: ", 1024);
      //deBoGriPrint(shift, "Printing shift of pTemp: ", 1024);
      //deBoGriPrint(pTemp, "Printing pTemp: ", 1024);
      pDelete(&pTemp); 
      //deBoGriPrint(strat->T[0].p, "Printing strat->T[0]: ", 1024);
    }
#else

    TObject tmp; //save the unshifted poly
    tmp.p = strat->T[ii].p;
    tmp.t_p = strat->T[ii].t_p;
    if(shift > 0)
    {
      strat->T[ii].t_p = p_LPshiftT
        ( tmp.t_p, shift, 
          strat->uptodeg, strat->lV, strat, strat->tailRing );
      strat->T[ii].p = p_LPshiftT
        ( tmp.p, shift, 
          strat->uptodeg, strat->lV, strat, currRing );
    }
    SD::ksReducePoly(h, &tmp, &(strat->T[ii]) );
    h->dvec = NULL;

    //BOCO: why did i change it to that?:
    //ksReducePoly(h,strat->T[ii],strat->kNoetherTail());

    /* BOCO: We are in homogenous case, aren't we? So we do
     * not have to shrink. -> We will care for inhomogenous
     * input later.
     * BOCO: this was copied from redFirstShrink
     * i only additionally test for strat->homog - why wasn't
     * that done in redFirstShrink
     * some other time, i should review redBbaShift
    if (!h->IsNull() && !strat->homog ){
      poly qq=p_Shrink(h->GetTP(),strat->lV,strat->tailRing);
      h->p=NULL;
      h->t_p=qq;
      if (qq!=NULL) h->GetP(strat->lmBin);
    }
     */

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
#endif
#endif

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

#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
      h->freeLcmDVec(); //BOCO: maybe this is not necessary ?
#endif

#ifdef KDEBUG
      h->lcm=NULL;
#endif
      return 0;
    }
    deBoGriPrint(strat->T[0].p, "Printing strat->T[0]: ", 1024);
    h->SetShortExpVector();
    deBoGriPrint(strat->T[0].p, "Printing strat->T[0]: ", 1024);
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
#if 0 //BOCO: original code (replaced)
        int dummy=strat->sl;
        if (kFindDivisibleByInS(strat, &dummy, h) < 0)
#else //replacement
        int dummy=strat->sl; int shift_dummy;
        j = SD::kFindDivisibleByInS(strat, &dummy, h, shift_dummy);
        if (j < 0)
#endif
          return 1;
        SD::enterL(&strat->L,&strat->Ll,&strat->Lmax,*h,at);
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

//BOCO: used in ShiftDVec::redBba
#if 0 //BOCO: original header (replaced)
int kFindDivisibleByInS
  (const kStrategy strat, int* max_ind, LObject* L)
{
#else //BOCO: replacement
int ShiftDVec::kFindDivisibleByInS
  (const kStrategy strat, int* max_ind, LObject* L, int shift)
{
  namespace SD = ShiftDVec;
#endif
#if (HAVE_SEV > 0) //BOCO: comments/uncomments sev
  unsigned long not_sev = ~L->sev;
#endif //#if (HAVE_SEV > 0)
  poly p = L->GetLmCurrRing();
  int j = 0;

#if (HAVE_SEV > 0) //BOCO: comments/uncomments sev
  pAssume(~not_sev == p_GetShortExpVector(p, currRing));
#endif //#if (HAVE_SEV > 0)
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
/*BOCO: We do have no debug version at the moment
 * original code:
#if defined(PDEBUG) || defined(PDIV_DEBUG)
    if (p_LmShortDivisibleBy(strat->S[j], strat->sevS[j],
                             p, not_sev, currRing))
        return j;
#else
*/
#if 0 //BOCO: original code (replaced)
    if ( !(strat->sevS[j] & not_sev) &&
         p_LmDivisibleBy(strat->S[j], p, currRing))
#else //replacement
    /*BOCO:
     * The original p_LmDivisibleBy checks if second arg
     * divides first arg, we check if first arg is divisibly
     * by second arg! */
    TObject t_Sj(strat->S[j]); TObject t_p(p);
    shift = p_LmDivisibleBy(&t_p, &t_Sj, currRing, strat->lV);
    if ( shift < UINT_MAX && 
         ( strat->homog ||
           !redViolatesDeg
             ( p, strat->S[j], strat->uptodeg,
               currRing, currRing, strat->tailRing ) ) )
    {
      return j;
    }
#endif
/*BOCO: We do have no debug version at the moment (see above)
 * original code:
#endif
*/
    j++;
  }
}


/*2
 *BOCO:
 * This function will overload the redBba function of the
 * bba-Algorithm for the non-commutative dvec-case. The original
 * comment does still apply; however, h is no longer of type
 * poly, but of type TObject *, which expands to sTObjectDVec in
 * the dvec-case (,which we do consider here). Same goes for the
 * return value of this function.
 *BOCO original comment from kutil.cc:
 * reduces h using the set S
 * procedure used in updateS
 */
#if 0 //BOCO: original code (replaced)
static poly redBba (poly h,int maxIndex,kStrategy strat)
#else //BOCO: replacement
poly ShiftDVec::redBba
  (LObject * h,int maxIndex,kStrategy strat)
#endif
{
#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
  namespace SD = ShiftDVec;
#endif

  int j = 0;


#if (HAVE_SEV > 1) //BOCO: comments/uncomments sev
  unsigned long not_sev = ~ pGetShortExpVector(h->p);
#else
  unsigned long not_sev = 0;
#endif //#if (HAVE_SEV > 0)

  while (j <= maxIndex)
  {
#if 0 //BOCO: original code (replaced)
    /*BOCO: pLmShortDivisibleBy is a macro and defined by:
     *
     * #define pLmShortDivisibleBy(a, sev_a, b, not_sev_b) \
     *  p_LmShortDivisibleBy(a, sev_a, b, not_sev_b, currRing)
     *
     * in polys.h.
     */
    if (pLmShortDivisibleBy(strat->S[j],strat->sevS[j], h, not_sev))
#else //BOCO: replacement
    /*BOCO:
     * The original p_LmShortDivisibleBy checks if second arg
     * divides first arg, we check if first arg is divisibly
     * by second arg! */
    TObject * t;
    if(!(t = strat->s_2_t(j))) 
      { TObject tt(strat->S[j]); t = &tt; }
    uint shift = 
      SD::p_LmShortDivisibleBy(h, strat->sevS[j], t, not_sev, currRing);
    if( shift < UINT_MAX && 
        ( strat->homog ||
          !redViolatesDeg( h, t, strat->uptodeg, currRing ) ) )
#endif
    {
#if 0 //BOCO: original code (replaced)
      h = ksOldSpolyRed(strat->S[j],h,strat->kNoetherTail());
#else //BOCO: replacement
      poly pTemp = p_LPshiftT
        ( strat->S[j], shift, 
          strat->uptodeg, strat->lV, strat, currRing);
      loGriToFile("p_LPshiftT poly in redBba ", 0,1024);
      TObject tTemp(pTemp);
      TObject noShift(strat->S[j]);
      SD::ksReducePoly(h, &noShift, &tTemp, strat->kNoetherTail());

      /* BOCO: this was copied from redFirstShrink
       * i only additionally test for strat->homog - why wasn't
       * that done in redFirstShrink
       * some other time, i should review redBbaShift
       */
      /*
      *GRICO: this is the old shrink process
      *which should be no longer needed, so h will not be changed
      if (!h->IsNull() && !strat->homog ){
        poly qq=p_Shrink(h->GetTP(),strat->lV,strat->tailRing);
        h->p=NULL;
        h->t_p=qq;
        if (qq!=NULL) h->GetP(strat->lmBin);
      }*/

      h->freeDVec(); //BOCO: after reduction lm has changed
      if(shift > 0)
      { 
        loGriToFile
          ("pDelete in kFindDivisibleByInS ", 0,1024, pTemp);
        pDelete(&pTemp); 
      }
#endif
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
#if 0 //BOCO: original code (replaced) from pInline1.h
PINLINE1 BOOLEAN p_LmShortDivisibleBy
  ( poly a, unsigned long sev_a, poly b, 
    unsigned long not_sev_b, const ring r )
#else //BOCO: replacement
uint ShiftDVec::p_LmShortDivisibleBy
  (const TObject* t1, unsigned long sev_t1, const TObject* t2,
   unsigned long not_sev_t2, const ring r                     )
#endif
{
#if 0 //BOCO: original code (replaced)
  p_LmCheckPolyRing1(a, r);
  p_LmCheckPolyRing1(b, r);
#else //BOCO: replacement
  p_LmCheckPolyRing1(t1->p, r);
  p_LmCheckPolyRing1(t2->p, r);
#endif

/*BOCO: We do have no debug version at the moment
 * original code:
#ifndef PDIV_DEBUG
*/
#if (HAVE_SEV > 2) //BOCO: comments/uncomments sev
  pPolyAssume2(p_GetShortExpVector(t1->p, r) == sev_t1, t1->p, r);
  pPolyAssume2
    (p_GetShortExpVector(t2->p, r) == ~ not_sev_t2, t2->p, r);

  if (sev_t1 & not_sev_t2)
  {
    pAssume1(p_LmDivisibleByNoComp(t1->p, t2->p, r) == FALSE);
    return FALSE;
  }
#endif //#if (HAVE_SEV > 0)

  return p_LmDivisibleBy(t1->p, t2->p, r);

/*BOCO: We do have no debug version at the moment
 * original code:
#else
  return pDebugLmShortDivisibleBy(a, sev_a, r, b, not_sev_b, r);
#endif
*/
}


/*BOCO:
 * This Function replaces 
 * p_LmDivisibleBy ( poly, poly, const ring ) 
 * Tests if t1 is divisible by t2
 */
#if 0 //BOCO: original code (replaced) from pInline1.h
PINLINE1 BOOLEAN p_LmDivisibleBy ( poly a, poly b, const ring r )
#else //BOCO: replacement
uint ShiftDVec::p_LmDivisibleBy
  ( TObject * t1, TObject * t2, const ring r, int lV )
{
  namespace SD = ShiftDVec;
#endif
#if 0 //BOCO: original code (replaced)
  p_LmCheckPolyRing1(b, r);
  pIfThen1(a != NULL, p_LmCheckPolyRing1(b, r));
  if (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r))
    return _p_LmDivisibleByNoComp(a, b, r);
  return FALSE;
#else //BOCO: replacement
  p_LmCheckPolyRing1(t2->p, r);
  pIfThen1(t1->p != NULL, p_LmCheckPolyRing1(t2->p, r));
  if (p_GetComp(t1->p, r) == 0 || p_GetComp(t1->p,r) == p_GetComp(t2->p,r))
    return SD::_p_LmDivisibleByNoComp(t1, t2, r, lV);
  return UINT_MAX;
#endif
}


/*BOCO:
 * This Function replaces 
 * _p_LmDivisibleByNoComp 
 * in the non-commutative dvec-case by overloading it.
 *
 * Most of this function, which considered r->VarL_LowIndex, was
 * just replaced by divisibleByShiftDVec. We do not consider 
 * r->VarL_LowIndex at the moment.
 * returns shift if shifted t2 divides t1, or UINT_MAX if no
 * divisor was found.
 *
 * IMPORTANT:
 * We may have to care better for the provided Ring!
 *
 *BOCO: original comment
 * return: FALSE, if there exists i, such that a->exp[i] > b->exp[i]
 *         TRUE, otherwise
 * (1) Consider long vars, instead of single exponents
 * (2) Clearly, if la > lb, then FALSE
 * (3) Suppose la <= lb, and consider first bits of single exponents in l:
 *     if TRUE, then value of these bits is la ^ lb
 *     if FALSE, then la-lb causes an "overflow" into one of those bits, i.e.,
 *               la ^ lb != la - lb
 */
#if 0 //BOCO: original code (replaced)
static inline BOOLEAN _p_LmDivisibleByNoComp
  ( poly a, poly b, const ring r )
#else //BOCO: replacement
static inline uint ShiftDVec::_p_LmDivisibleByNoComp
  (TObject * t1,TObject * t2, const ring r, int lV)
#endif
{
#if 0 //BOCO: original code (deleted)
  int i=r->VarL_Size - 1;
  unsigned long divmask = r->divmask;
  unsigned long la, lb;

  if (r->VarL_LowIndex >= 0)
  {
    i += r->VarL_LowIndex;
    do
    {
      la = a->exp[i];
      lb = b->exp[i];
      if ((la > lb) ||
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
        pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == FALSE);
        return FALSE;
      }
      i--;
    }
    while (i>=r->VarL_LowIndex);
  }
  else
  {
    do
    {
      la = a->exp[r->VarL_Offset[i]];
      lb = b->exp[r->VarL_Offset[i]];
      if ((la > lb) ||
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
        pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == FALSE);
        return FALSE;
      }
      i--;
    }
    while (i>=0);
  }
#endif

 /*BOCO: we do not consider HAVE_RINGS at the moment
  * original code:
#ifdef HAVE_RINGS
  pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == nDivBy(p_GetCoeff(b, r), p_GetCoeff(a, r)));
  return (!rField_is_Ring(r)) || nDivBy(p_GetCoeff(b, r), p_GetCoeff(a, r));
#else
  */
 /*BOCO: we may care for debugging functions later
  * original code:
  pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == TRUE);
#endif
  */

  //BOCO: Well, thats all it does at the moment!
  return t1->divisibleBy(t2, lV);
}


/*BOCO: 
 * If a shifted lm of a poly T[j] divides lm of L->p, the
 * position of the unshifted polynomial will be returned,
 * otherwise -1 will be returned. Be aware: If the shift of a
 * found divisor is non-zero, then the divisor may not be in T!
 * If a divisor is found, shift will be set to the shift of the
 * divisor (with respect to the corresponding TObject in T).  As
 * usual, the function simply overloads kFindDivisibleByInT.
 *   The sevT Argument is not in use. It was just added, in case
 * we need some of the sev stuff later.
 * If uptodeg == 0, we will not care, if the reduction of L->p
 * by T[j] violates the degree bound, otherwise we do (by not
 * returning T[j], if the reduction violates the degree bound).
 *
 *original comment:
 * return -1 if no divisor is found
 *        number of first divisor, otherwise
 */
#if 0 //BOCO: original header
int kFindDivisibleByInT
  ( const TSet &T, const unsigned long* sevT, const int tl, 
    const LObject* L, const int start                       )
#else
int ShiftDVec::kFindDivisibleByInT
  ( const TSet &T, const unsigned long* sevT, LObject * L, 
    uint & shift, kStrategy strat, const int start         )
{
  namespace SD = ShiftDVec;
#endif
#if (HAVE_SEV > 0) //BOCO: comments/uncomments sev
  unsigned long not_sev = ~L->sev;
#endif //#if (HAVE_SEV > 0)

  int j = start;
  int ret;  //BOCO: added

  poly p=L->p;
  ring r=currRing;

  //BOCO: TODO: 
  //I think the following line is souperflous - remove from code!
  if (p==NULL)  { r=L->tailRing; p=L->t_p; }

  L->GetLm(p, r);

  //BOCO: added following line
  L->SetDVecIfNULL(p, r);


#if (HAVE_SEV > 2) //BOCO: comments/uncomments sev
  pAssume(~not_sev == p_GetShortExpVector(p, r));
#endif //#if (HAVE_SEV > 0)

  if (r == currRing)
  {
    loop
    {
#if 0  //BOCO: original code (replaced)
      if (j > tl) return -1;
#else  //BOCO: replacement
      if (j > strat->tl) {ret = -1; break;};
#endif
/* BOCO: we have no debug version at the moment
#if defined(PDEBUG) || defined(PDIV_DEBUG)
      if (p_LmShortDivisibleBy(T[j].p, sevT[j],
                               p, not_sev, r))
        return j;
#else
*/
#if 0 //BOCO: original code
      if (!(sevT[j] & not_sev) &&
          p_LmDivisibleBy(T[j].p, p, r))
        return j;
#else //BOCO: replacement
      /*BOCO:
       * The original p_LmDivisibleBy checks if second arg
       * divides first arg, we check if first arg is divisibly
       * by second arg! */
      T[j].SetDVecIfNULL(p, r);
      shift = SD::p_LmDivisibleBy(L, &T[j], r, strat->lV);
      if ( shift < UINT_MAX && 
#if (HAVE_SEV > 3) //BOCO: comments/uncomments sev
           !(sevT[j] & not_sev) &&
#endif //#if (HAVE_SEV > 3)
           ( strat->homog ||
             !redViolatesDeg
               ( L, &T[j], strat->uptodeg, currRing ) ) )
      {ret = j; break;}
#endif //#if 0
/* BOCO: we have no debug version at the moment
#endif
*/
      j++;
    }
  }
  else //BOCO: could do both cases in one with function pointer
  {
    loop
    {
#if 0 //BOCO: original code (replaced)
      if (j > tl) return -1;
#else //replacement
      if (j > strat->tl) {ret = -1; break;}
#endif
/* BOCO: we have no debug version at the moment
#if defined(PDEBUG) || defined(PDIV_DEBUG)
      if (p_LmShortDivisibleBy(T[j].t_p, sevT[j],
                               p, not_sev, r))
        return j;
#else
*/
#if 0 //BOCO: original code
      if (!(sevT[j] & not_sev) &&
          p_LmDivisibleBy(T[j].t_p, p, r))
        return j;
#else //BOCO: replacement
      /*BOCO:
       * The original p_LmDivisibleBy checks if second arg
       * divides first arg, we check if first arg is divisibly
       * by second arg! */
      //TObject tTemp(T[j].t_p);
      T[j].SetDVecIfNULL(p, r);
      shift = p_LmDivisibleBy(L, &T[j], r, strat->lV);
      if ( shift < UINT_MAX && 
#if (HAVE_SEV > 3) //BOCO: comments/uncomments sev
           !(sevT[j] & not_sev) &&
#endif //#if (HAVE_SEV > 3)
           ( strat->homog ||
             !redViolatesDeg
               ( L, &T[j], strat->uptodeg, currRing ) ) )
      {ret = j; break;}
#endif
/* BOCO: we have no debug version at the moment
#endif
*/
      j++;
    }
  }

  return ret;  //BOCO: added
}


/* BOCO: 
 * This version of the function is used in redtailBba.
 * It overloads its non-dvec version.
 * WARNING:
 * The Returned object may have to be shifted, to devide L,
 * therefor shift will be set.
 * If uptodeg == 0, we will not care, if the reduction of L->p
 * with a found poly p in S violates the degree bound, otherwise
 * we do (by not returning p, if the reduction violates the
 * degree bound).
 */
#if 0 //BOCO: original header from kutil.cc
TObject * kFindDivisibleByInS
  (kStrategy strat, int pos, LObject* L, TObject* T, long ecart)
#else
TObject * ShiftDVec::kFindDivisibleByInS
  ( kStrategy strat, int pos, LObject* L, TObject* T, 
    uint & shift, int lV, int uptodeg, long ecart     )
{
  initDeBoGri
    ( ShiftDVec::indent, 
      "Entering kFindDivisibleByInS", 
      "Leaving kFindDivisibleByInS", 256 );
#endif
  int j = 0;
#if (HAVE_SEV > 0) //BOCO: comments/uncomments sev
  const unsigned long not_sev = ~L->sev;
  const unsigned long* sev = strat->sevS;
#endif //#if (HAVE_SEV > 0)
  poly p;
  ring r;
  L->GetLm(p, r);
  L->SetDVecIfNULL(p, r);

#if (HAVE_SEV > 2) //BOCO: comments/uncomments sev
  assume(~not_sev == p_GetShortExpVector(p, r));
#endif //#if (HAVE_SEV > 0)

  if (r == currRing)
  {
    deBoGriPrint("Ring is currRing.", 256);
    loop
    {
      if (j > pos) return NULL;
/* BOCO: debugging version may be later introduced
#if defined(PDEBUG) || defined(PDIV_DEBUG)
      if (p_LmShortDivisibleBy(strat->S[j], sev[j], p, not_sev, r) &&
          (ecart== LONG_MAX || ecart>= strat->ecartS[j]))
        break;
#else
*/
#if 0 //BOCO: original code (replaced)
      if (!(sev[j] & not_sev) &&
          (ecart== LONG_MAX || ecart>= strat->ecartS[j]) &&
          p_LmDivisibleBy(strat->S[j], p, r))
        break;
#else //BOCO: replacement (hopefully it works)
      if ( strat->tl < 0 || strat->S_2_R[j] == -1
#if (HAVE_SEV > 3) //BOCO: comments/uncomments sev
           && !(sev[j] & not_sev)                       
#endif //#if (HAVE_SEV > 0)
         )
      {
        deBoGriPrint("If Clause", 256);
        /*BOCO:
         * The original p_LmDivisibleBy checks if second arg
         * divides first arg, we check if first arg is divisibly
         * by second arg! */
        ShiftDVec::sTObject t(strat->S[j]);
        t.SetDVec();
        shift = p_LmDivisibleBy(L, &t, r, lV);
        t.freeDVec();
      } else {
        deBoGriPrint("Else Clause", 256);
        strat->S_2_T(j)->SetDVecIfNULL();
        shift = p_LmDivisibleBy(L, strat->S_2_T(j), r, lV);
      }
      if ( shift < UINT_MAX && 
           ( strat->homog ||
             !redViolatesDeg
               ( L->p, strat->S[j], uptodeg,
                 currRing, currRing, strat->tailRing ) ) )
      {
        deBoGriPrint(strat->S[j], "Found Divisor: ", 256);
        break;
      }
#endif
/* BOCO: debugging version may be later introduced
#endif
*/
      j++;
    }
    // if called from NF, T objects do not exist:
    if (strat->tl < 0 || strat->S_2_R[j] == -1)
    {
      T->Set(strat->S[j], r, strat->tailRing);
      deBoGriPrint("Return 1", 256);
      return T;
    }
    else
    {
/////      assume (j >= 0 && j <= strat->tl && strat->S_2_T(j) != NULL
/////      && strat->S_2_T(j)->p == strat->S[j]); // wrong?
//      assume (j >= 0 && j <= strat->sl && strat->S_2_T(j) != NULL && strat->S_2_T(j)->p == strat->S[j]);
      deBoGriPrint("Return 2", 256);
      return strat->S_2_T(j);
    }
  }
  else
  {
    deBoGriPrint("Ring is not currRing.", 256);
    TObject* t;
    loop
    {
      if (j > pos) return NULL;
      assume(strat->S_2_R[j] != -1);
/* BOCO: debugging version may be later introduced
#if defined(PDEBUG) || defined(PDIV_DEBUG)
      t = strat->S_2_T(j);
      assume(t != NULL && t->t_p != NULL && t->tailRing == r);
      if (p_LmShortDivisibleBy(t->t_p, sev[j], p, not_sev, r) &&
          (ecart== LONG_MAX || ecart>= strat->ecartS[j]))
        return t;
#else
*/
#if (HAVE_SEV > 3) //BOCO: comments/uncomments sev
      if (! (sev[j] & not_sev) && (ecart== LONG_MAX || ecart>= strat->ecartS[j]))
#else //BOCO: replacement
      //BOCO: TODO: Why do we have Problems with this?
//      if ( !(ecart== LONG_MAX || ecart>= strat->ecartS[j]) )
      if(true)
#endif //#if (HAVE_SEV > 3)
      {
        t = strat->S_2_T(j);
        assume(t != NULL && t->t_p != NULL && t->tailRing == r && t->p == strat->S[j]);
        strat->S_2_T(j)->SetDVecIfNULL(t->t_p, r); //Sets dvec, if dvec == NULL
#if 0 //BOCO: original code (replaced)
        if (p_LmDivisibleBy(t->t_p, p, r)) return t;
#else //BOCO: replacement
        /*BOCO:
         * The original p_LmDivisibleBy checks if second arg
         * divides first arg, we check if first arg is divisibly
         * by second arg! */
        shift = p_LmDivisibleBy(L, t, r, lV);
        if ( shift < UINT_MAX && 
             ( strat->homog || 
               !redViolatesDeg(p, t->t_p, uptodeg, r, r, r) ) )
        {
          deBoGriPrint("Return 3", 256);
          return t;
        }
#endif
      }
/* BOCO: debugging version may be later introduced
#endif
*/
      j++;
    }
  }

/*BOCO: - undefined return value? -
 * I think it may happen, that the return Value of this
 * function is undefined. Maybe it would be clearer to return
 * something
 return NULL;
 */
}


#if 0 //BOCO: original header from kutil.cc
poly redtailBba
  ( LObject* L, int pos, kStrategy strat, BOOLEAN withT, 
    BOOLEAN normalize                                    )
{
#else //replacement
poly ShiftDVec::redtailBba
  ( LObject* L, int pos, kStrategy strat, BOOLEAN withT, 
    BOOLEAN normalize                                    )
{
  namespace SD = ShiftDVec;

  initDeBoGri
    ( ShiftDVec::indent, 
      "Entering redtailBba", "Leaving redtailBba", 256 );
#endif
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
  Ln.dvec = NULL;
  Ln.pLength = L->GetpLength() - 1;
  deBoGriPrint(h, "h at the Beginning: ", 2048);
  deBoGriPrint(L->p, "L.p at the Beginning: ", 2048);
  deBoGriPrint(Ln.p, "Ln.p at the Beginning: ", 2048);

  pNext(h) = NULL;
  if (L->p != NULL) pNext(L->p) = NULL;
  L->pLength = 1;

  Ln.PrepareRed(strat->use_buckets);

  int cnt=REDTAIL_CANONICALIZE;
#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
  uint shift = 0;
#endif

  //BOCO: TODO: remove all these tests
  assume(Ln.p == NULL || pTotaldegree(Ln.p) < 1000); 

  TObject uTmp;
  uTmp.t_p = NULL;
  uTmp.p = NULL;
  static int deBoGriCnt2 = 0; //TODO: remove
  while(!Ln.IsNull())
  {
    assume(h != NULL);
    assume(Ln.p == NULL || pTotaldegree(Ln.p) < 1000);
#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
    shift = 0;
#endif
    loop
    {
      deBoGriPrint
        (h, "h at Beginning of 'loop' : ", 2048);
      assume(h != NULL);
      ++deBoGriCnt2; //TODO: remove
      //assume(deBoGriCnt2 < 1000);
      assume(Ln.p == NULL || pTotaldegree(Ln.p) < 1000);
/* BOCO: original code (deleted because of sev usage)
      Ln.SetShortExpVector();
*/
      if (withT)
      {
        //BOCO: added debug output
        deBoGriPrint( "Reducing with T", 2048 );
        int j;
#if 0 //BOCO: original code (replaced)
        j = kFindDivisibleByInT
          (strat->T, strat->sevT, strat->tl, &Ln);
#else //BOCO: replacement
        j = kFindDivisibleByInT
          ( strat->T, strat->sevT, &Ln, shift, strat );
        deBoGriPrint(j, "j after kFindDivisibleByInT: ", 2048);
        deBoGriPrint
          ( strat->T[j].p, "strat->T[j]: ", 2048, j < 0 );
        assume(h != NULL);
        //BOCO may not be needed, but want to be sure
        Ln.freeDVec(); 
#endif
        if (j < 0)
        {
          assume(Ln.p == NULL || pTotaldegree(Ln.p) < 1000);
          assume(h != NULL);
          break;
        }
        assume(shift < UINT_MAX);
        With = &(strat->T[j]);
#if 1 //BOCO: added code
        if(shift != 0)
        //Our divisor is a shift (and thus not in T or S)
        {
          poly p_tmp = With->p;
          poly t_p_tmp = With->t_p;
          With->t_p = p_LPshiftT
            ( t_p_tmp, shift, strat->uptodeg, strat->lV, 
              strat, strat->tailRing                     );
          With->p = ::p_mLPshift
            ( p_tmp, shift, strat->uptodeg, strat->lV, 
              currRing                                 );
          if(With->t_p) With->p->next = With->t_p->next;
          loGriToFile("p_LPshiftT in redtailBba ", 0,1024,(void*)With->t_p);
          //BOCO: With->p and With->t_p have to be deleted later, 
          //see below
        }
#endif
        assume(h != NULL);
      }
      else
      {
        deBoGriPrint( "Not reducing with T", 256 );

//TObject * kFindDivisibleByInS
//  ( kStrategy strat, int pos, LObject* L, TObject* T, 
//    uint & shift, long ecart = LONG_MAX               )

#if 0 //BOCO: original code (replaced)
        With = kFindDivisibleByInS(strat, pos, &Ln, &With_s);
        if (With == NULL) break;
#else //BOCO: replacement
        assume(Ln.p == NULL || pTotaldegree(Ln.p) < 1000);
        With = kFindDivisibleByInS
          ( strat, pos, &Ln, &With_s, shift, strat->lV );
        assume(Ln.p == NULL || pTotaldegree(Ln.p) < 1000);

        if (With == NULL) {
          assume(Ln.p == NULL || pTotaldegree(Ln.p) < 1000);
          deBoGriPrint
            (Ln.p, "The polynomial after break loop: ", 2048, h != NULL);
          deBoGriPrint
            (Ln.t_p, "Ln is NULL, printing its tail {at break}: ", 2048, h == NULL);
          break;
        }

        //BOCO: added debug output
        deBoGriPrint( Ln.p, "Reducing: ", 256 );
        deBoGriPrint
          ( Ln.GetDVec(), Ln.GetDVsize(), "DVec: ", 256 );
        deBoGriPrint( With->p, "With: ", 256 );
        deBoGriPrint
          (With->GetDVec(), With->GetDVsize(),"DVec: ",256);
        deBoGriPrint( shift, "Shift needed: ", 256 );

        if(shift != 0)
        //Our divisor is a shift (and thus not in T or S)
        {
//          With = new TObject(*WithTmp); //should do just a shallow copy
//          With->t_p = p_LPshiftT
//            ( WithTmp->t_p, shift, strat->uptodeg, strat->lV, 
//              strat, strat->tailRing                          );
          uTmp.p = With->p;
          uTmp.t_p = With->p;
          With->t_p = p_LPshiftT
            ( uTmp.t_p, shift, strat->uptodeg, strat->lV, 
              strat, strat->tailRing                     );
          With->p = ::p_mLPshift
            ( uTmp.p, shift, strat->uptodeg, strat->lV, 
              currRing                                 );
          if(With->t_p) With->p->next = With->t_p->next;
          loGriToFile("p_LPshiftT in redtailBba ", 0,1024,(void*)With->t_p);
          //BOCO: With->p has to be deleted later, see below

          /*BOCO: do we need to set With_s.p, or is With_s just
           * a dummy?
           */
          With_s.p = With->p;
          With_s.freeDVec();
        }
#endif
      }
      cnt--;
      if (cnt==0)
      {
        cnt=REDTAIL_CANONICALIZE;
        /*poly tmp=*/Ln.CanonicalizeP();
        if (normalize)
        {
          Ln.Normalize();
          //pNormalize(tmp);
          //if (TEST_OPT_PROT) { PrintS("n"); mflush(); }
        }
      }
      if (normalize && (!TEST_OPT_INTSTRATEGY) && (!nIsOne(pGetCoeff(With->p))))
      {
        With->pNorm();
      }
      strat->redTailChange=TRUE;

      /* BOCO: do we need to test for violation of exp bound?
       * does an exp bound still make sense?
       */
      assume(Ln.p == NULL || pTotaldegree(Ln.p) < 1000);
      if ( ShiftDVec::ksReducePolyTail(L, &uTmp, With, With->p, Ln.p, strat ))
      {
        // reducing the tail would violate the exp bound
        //  set a flag and hope for a retry (in bba)
        strat->completeReduce_retry=TRUE;
        if ((Ln.p != NULL) && (Ln.t_p != NULL)) Ln.p=NULL;
        do
        {
          pNext(h) = Ln.LmExtractAndIter();
          pIter(h);
          L->pLength++;
        } while (!Ln.IsNull());
        assume(Ln.p == NULL || pTotaldegree(Ln.p) < 1000);

        goto all_done;

      }
      assume(Ln.p == NULL || pTotaldegree(Ln.p) < 1000);

      /* BOCO: this was copied from redFirstShrink and adjusted
       * i only additionally test for strat->homog - why wasn't
       * that done in redFirstShrink
       * some other time, i should review redBbaShift, so it
       * seems unused...?
       * I hope, this fits right in here, but i am not shure.
       */
      /*GRICO: this is the old shrink process
      *which should be no longer needed, so h will not be changed
       if (!Ln.IsNull() && !strat->homog ){
        poly qq=p_Shrink(Ln.GetTP(),strat->lV,strat->tailRing);
        Ln.p=NULL;
        Ln.t_p=qq;
        if (qq!=NULL) Ln.GetP(strat->lmBin);
      }
      */
     
      if (Ln.IsNull()){
        assume(Ln.p == NULL);
        goto all_done;
      }
      if (! withT) With_s.Init(currRing);
#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
      /* BOCO: p does not correspond to any object in S,T or R,
       * if the shift is not zero.
       */
      if(shift != 0 && shift < UINT_MAX)
      {
        loGriToFile
          ( "pDelete of With->t_p in redTailBba",
            0,1024,(void*) With->t_p              );
        pDelete(&(With->t_p)); 
        With->t_p = uTmp.t_p; 
        if(With->p)
        {
          loGriToFile
            ( "pLmFree of With->p in redTailBba",
              0,1024,(void*) With->p              );
          pLmFree(&(With->p)); 
        }
        With->p = uTmp.p; 
        shift = 0;
      }
      assume(Ln.p == NULL || pTotaldegree(Ln.p) < 1000);
#endif
      Ln.freeDVec();
      deBoGriPrint
        (Ln.p, "The polynomial after inner loop: ", 2048, h != NULL);
      /*
      deBoGriPrint
        (Ln.p->next, "Ln.p->next is NULL, printing its tail: ", 2048, h == NULL);
      */
    }
    deBoGriPrint(Ln.p, "Ln.p reduced to: ", 2048);
    assume(h != NULL);

    //BOCO: LmExtr. makes second monomial the lm of Ln
    pNext(h) = Ln.LmExtractAndIter(); 

    deBoGriPrint
      (h, "The polynomial after pNext(h) = ... : ", 2048);

    Ln.freeDVec();

    pIter(h);
    deBoGriPrint
      (deBoGriCnt2, "deBoGriCnt2 before pNormalize(h): ", 2048);
    deBoGriPrint
      (h, "The polynomial before pNormalize(h): ", 2048);
    assume(Ln.p == NULL || pTotaldegree(Ln.p) < 1000);
    pNormalize(h); 
    /* BOCO: why do we have h? Can't we just use Ln.p?
     * This is confusing. */

    L->pLength++;
  }

  all_done:
#ifdef HAVE_SHIFTBBADVEC //BOCO: added code
  /* BOCO: p does not correspond to any object in S,T or R, if
   * the shift is not zero.
   */
  if(shift != 0 && shift < UINT_MAX)
  {
    loGriToFile
      ( "pDelete of With->t_p in redTailBba",
        0,1024,(void*) With->t_p              );
    pDelete(&(With->t_p)); 
    With->t_p = uTmp.t_p; 
    if(With->p)
    {
      loGriToFile
        ( "pLmFree of With->p in redTailBba",
          0,1024,(void*) With->p              );
      pLmFree(&(With->p)); 
    }
    With->p = uTmp.p; 
    shift = 0;
  }
#endif
  Ln.Delete();
  Ln.freeDVec();
  if (L->p != NULL) pNext(L->p) = pNext(p);

  if (strat->redTailChange)
  {
    L->last = NULL;
    L->length = 0;
  }

  //if (TEST_OPT_PROT) { PrintS("N"); mflush(); }
  //L->Normalize(); // HANNES: should have a test
  kTest_L(L);
  With_s.freeDVec();
  return L->GetLmCurrRing();
}


#if 0 //BOCO: original header
KINLINE poly redtailBba (poly p,int pos,kStrategy strat,BOOLEAN normalize)
{
#else //replacement
KINLINE poly ShiftDVec::redtailBba
  (poly p,int pos,kStrategy strat,BOOLEAN normalize)
{
  namespace SD = ShiftDVec;
#endif
  LObject L(p, currRing, strat->tailRing);
  return SD::redtailBba(&L, pos, strat,FALSE, normalize);
}


/*2
*updates S:
*the result is a set of polynomials which are in
*normalform with respect to S
*/
#if 0 //BOCO: original code          
void updateS(BOOLEAN toT,kStrategy strat)
#else //replacement
void ShiftDVec::updateS(BOOLEAN toT,kStrategy strat)
{
  namespace SD = ShiftDVec;
#endif
  LObject h;
  int i, suc=0;
  poly redSi=NULL;
  BOOLEAN change,any_change;
//  Print("nach initS: updateS start mit sl=%d\n",(strat->sl));
//  for (i=0; i<=(strat->sl); i++)
//  {
//    Print("s%d:",i);
//    if (strat->fromQ!=NULL) Print("(Q:%d) ",strat->fromQ[i]);
//    pWrite(strat->S[i]);
//  }
//  Print("currRing->OrdSgn=%d\n", currRing->OrdSgn);
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
#if 0 //BOCO: original code          
          strat->S[i] = redBba(strat->S[i],i-1,strat);
#else
          LObject h(strat->S[i]);
          strat->S[i] = redBba(&h,i-1,strat);
#endif
          //if ((strat->ak!=0)&&(strat->S[i]!=NULL))
          //  strat->S[i]=redQ(strat->S[i],i+1,strat); /*reduce S[i] mod Q*/
          /*BOCO: wouldnt it be more efficient, if the reduction
           * algorithm would notice, if strat->S[i] was reduced
           * and would inform about that?
           */
          if (pCmp(redSi,strat->S[i])!=0)
          {
            change=TRUE;
            any_change=TRUE;
            /* BOCO: we do not have a debug version at the moment
            #ifdef KDEBUG
            if (TEST_OPT_DEBUG)
            {
              PrintS("reduce:");
              wrp(redSi);PrintS(" to ");p_wrp(strat->S[i], currRing, strat->tailRing);PrintLn();
            }
            #endif
            */
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
#if 0  //BOCO: ATTENTION: Changed in spielwiese Version
              //pContent(strat->S[i]);
              strat->S[i]=p_Cleardenom(strat->S[i], currRing);// also does a pContent
#else //the changes
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
#endif
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
      /*BOCO: 
       * Commentary from kutil.cc for reoderS:
       * reoders S with respect to posInS, suc is the first
       * changed index or zero. 
       *BOCO to this commentary:
       * So in the next iteration of the
       * reduction of S, we will not try to reduce those
       * polynomials in S, which have not changed their position
       * in the previous pass, as they could, due to the order of
       * S, only be interreduced by themselves, as the other
       * polynomials should have a higher order, but as they did
       * not change their positions, they didnt interreduce
       * themselves in the previous pass and would thus never
       * interreduce themselves again.
       * This is at least my understanding of the code. (I may
       * be wrong.)
       */
      if (any_change) reorderS(&suc,strat);
      else break;
    }
    if (toT)
      /*BOCO: 
       * Seems to put the reduced Elements also to T, but
       * without deleting the old Elements from T, which are
       * not reduced (is the latter really true?).
       * Again there are some things i dont understand yet here.
       */
    {
      for (i=0; i<=strat->sl; i++)
      {
        if ((strat->fromQ==NULL) || (strat->fromQ[i]==0))
        {
#if 0 //BOCO: original code          
          h.p = redtailBba(strat->S[i],i-1,strat);
#else
          LObject L(h.p, currRing, strat->tailRing); 
          h.p = SD::redtailBba(&L, i-1, strat, FALSE, FALSE);
#endif
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
#if 0 //BOCO: original code (we do not consider pOrdSgn != 1)
  else
  {
    while (suc != -1)
    {
      i=suc;
      while (i<=strat->sl)
      {
        change=FALSE;
        if (((strat->fromQ==NULL) || (strat->fromQ[i]==0)) && (i>0))
        {
          redSi=pHead((strat->S)[i]);
          (strat->S)[i] = redMora((strat->S)[i],i-1,strat);
          if ((strat->S)[i]==NULL)
          {
            deleteInS(i,strat);
            i--;
          }
          else if (pCmp((strat->S)[i],redSi)!=0)
          {
            any_change=TRUE;
            h.p = strat->S[i];
            strat->initEcart(&h);
            strat->ecartS[i] = h.ecart;
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
              pNorm(strat->S[i]); // == h.p
            }
            h.sev =  pGetShortExpVector(h.p);
            strat->sevS[i] = h.sev;
          }
          pLmDelete(&redSi);
          kTest(strat);
        }
        i++;
      }
#ifdef KDEBUG
      kTest(strat);
#endif
      if (any_change) reorderS(&suc,strat);
      else { suc=-1; break; }
      if (h.p!=NULL)
      {
        if (!strat->kHEdgeFound)
        {
          /*strat->kHEdgeFound =*/ HEckeTest(h.p,strat);
        }
        if (strat->kHEdgeFound)
          newHEdge(strat);
      }
    }
    for (i=0; i<=strat->sl; i++)
    {
      if ((strat->fromQ==NULL) || (strat->fromQ[i]==0))
      {
        strat->S[i] = h.p = redtail(strat->S[i],strat->sl,strat);
        strat->initEcart(&h);
        strat->ecartS[i] = h.ecart;
        h.sev = pGetShortExpVector(h.p);
        strat->sevS[i] = h.sev;
      }
      else
      {
        h.p = strat->S[i];
        h.ecart=strat->ecartS[i];
        h.sev = strat->sevS[i];
        h.length = h.pLength = pLength(h.p);
      }
      if ((strat->fromQ==NULL) || (strat->fromQ[i]==0))
        cancelunit1(&h,&suc,strat->sl,strat);
      h.SetpFDeg();
      /*puts the elements of S also to T*/
      enterT(h,strat);
      strat->S_2_R[i] = strat->tl;
    }
    if (suc!= -1) updateS(toT,strat);
  }
#endif
#ifdef KDEBUG
  kTest(strat);
#endif
}


#if 0 //BOCO: original code and comment from kutil.cc
void initBuchMoraCrit(kStrategy strat)
#else //replacement
void ShiftDVec::initBuchMoraCrit(kStrategy strat)
{
  namespace SD = ShiftDVec;
#endif

#if 0 //BOCO: original code (replaced)
  strat->enterOnePair=enterOnePairNormal;
  strat->chainCrit=chainCritNormal;
#else //replacement
  strat->enterOnePair=NULL;
  strat->chainCrit=NULL;
#endif

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


/*BOCO: original comment from kutil.cc
 * enters p at position at in L
 *BOCO:
 * - We have to initialize the dvec of [[p]]. 
 * - We will return a pointer to the LObject in [[set]] . */
#if 0 //BOCO: original code and comment
void enterL (LSet *set,int *length, int *LSetmax, LObject p,int at)
{
#else //BOCO: replacement
LObject* ShiftDVec::enterL
  ( LSet *set,int *length, int *LSetmax, LObject p,int at, 
    uint* dvec                                             )
{
#endif
  int i;
  // this should be corrected
  assume(p.FDeg == p.pFDeg());

  if ((*length)>=0)
  {
    if ((*length) == (*LSetmax)-1) enlargeL(set,LSetmax,setmaxLinc);
    if (at <= (*length))
#ifdef ENTER_USE_MEMMOVE
      memmove(&((*set)[at+1]), &((*set)[at]), ((*length)-at+1)*sizeof(LObject));
#else
    for (i=(*length)+1; i>=at+1; i--) (*set)[i] = (*set)[i-1];
#endif
  }
  else at = 0;

  (*length)++;
  (*set)[at] = p;

  return &(*set)[at];
}


//functions with nearly no change


//BOCO: nearly no change in this function
#if 0 //BOCO: original code and comment
void initSL (ideal F, ideal Q,kStrategy strat)
{
#else
void ShiftDVec::initSL(ideal F, ideal Q,kStrategy strat)
{
  namespace SD = ShiftDVec;
#endif
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
      LObject h; //BOCO: dvec is set to NULL by constructor

      //BOCO: TODO: should be set to NULL by constructor
      h.SetLcmDvecToNULL(); 

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
#if 0 //BOCO: original code (replaced)
          enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);
#else
          SD::enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);
#endif
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

//BOCO: nearly no change in this function
#if 0 //BOCO: original code and comment from kutil.cc
void initBuchMora(ideal F,ideal Q,kStrategy strat)
{
#else
void ShiftDVec::initBuchMora(ideal F,ideal Q,kStrategy strat)
{
  namespace SD = ShiftDVec;
#endif
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
  strat->L = initL(((IDELEMS(F)+setmaxLinc-1)/setmaxLinc)*setmaxLinc);
  /*- set B -*/
  strat->Bmax = setmaxL;
  strat->Bl = -1;
  strat->B = initL();
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
#if 0 //BOCO: removed -> maybe later?
  if(TEST_OPT_SB_1)
  {
    int i;
    ideal P=idInit(IDELEMS(F)-strat->newIdeal,F->rank);
    for (i=strat->newIdeal;i<IDELEMS(F);i++)
    {
      P->m[i-strat->newIdeal] = F->m[i];
      F->m[i] = NULL;
    }
    initSSpecial(F,Q,P,strat);
    for (i=strat->newIdeal;i<IDELEMS(F);i++)
    {
      F->m[i] = P->m[i-strat->newIdeal];
      P->m[i-strat->newIdeal] = NULL;
    }
    idDelete(&P);
  }
  else
#endif
  {
#if 0 //BOCO: original code (replaced)
    /*Shdl=*/initSL(F, Q,strat); /*sets also S, ecartS, fromQ */
    // /*Shdl=*/initS(F, Q,strat); /*sets also S, ecartS, fromQ */
#else
    /*Shdl=*/SD::initSL(F, Q,strat); /*sets also S, ecartS, fromQ */
#endif
  }
  strat->fromT = FALSE;
  strat->noTailReduction = !TEST_OPT_REDTAIL;
  if (!TEST_OPT_SB_1)
  {
    SD::updateS(TRUE,strat);
  }
  if (strat->fromQ!=NULL)
  {
    loGriToFile("omFreeSize strat->fromQ in initBuchMora ", IDELEMS(strat->Shdl)*sizeof(int),1024);
    omFreeSize(strat->fromQ,IDELEMS(strat->Shdl)*sizeof(int));
  }
  strat->fromQ=NULL;
}
