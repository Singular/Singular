#define PLURAL_INTERNAL_DECLARATIONS

#include "kernel/mod2.h"

#include "misc/options.h"

#include "polys/simpleideals.h"
#include "polys/prCopy.h"

#include "polys/nc/sca.h"
#include "polys/nc/gb_hack.h"

#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/kutil.h"

#include "kernel/GBEngine/nc.h"

/// nc_gr_initBba is needed for sca_gr_bba and gr_bba.
void nc_gr_initBba(ideal F, kStrategy strat); // from gr_kstd2.cc!

void addLObject(LObject& h, kStrategy& strat)
{
  if(h.IsNull()) return;

  strat->initEcart(&h);
  h.sev=0; // pGetShortExpVector(h.p);

  // add h into S and L
  int pos=posInS(strat, strat->sl, h.p, h.ecart);

  if ( (pos <= strat->sl) && (p_ComparePolys(h.p, strat->S[pos], currRing)) )
  {
    if (TEST_OPT_PROT)
      PrintS("d\n");
  }
  else
  {
    if (TEST_OPT_INTSTRATEGY)
    {
      p_Cleardenom(h.p, currRing);
    }
    else
    {
      pNorm(h.p);
    }

    if ((strat->syzComp==0)||(!strat->homog))
    {
      h.p = redtailBba(h.p,pos-1,strat);

      if (TEST_OPT_INTSTRATEGY)
      {
        p_Cleardenom(h.p,currRing);
      }
      else
      {
        pNorm(h.p);
      }
    }

    if(h.IsNull()) return;

    // statistic
    if (TEST_OPT_PROT)
    {
      PrintS("s\n");
    }

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("new s:");
      wrp(h.p);
      PrintLn();
    }
#endif

    enterpairs(h.p, strat->sl, h.ecart, 0, strat);

    pos=0;

    if (strat->sl!=-1) pos = posInS(strat, strat->sl, h.p, h.ecart);
    strat->enterS(h, pos, strat, -1);
//    enterT(h, strat); // ?!

    kDeleteLcm(&h);
  }


}


ideal k_sca_gr_bba(const ideal F, const ideal Q, const intvec *, const bigintmat *, kStrategy strat, const ring _currRing)
{
  const ring save = currRing;
  if( currRing != _currRing ) rChangeCurrRing(_currRing);
  assume( currRing == _currRing );


#if MYTEST
   PrintS("<sca_gr_bba>\n");
#endif

  assume(rIsSCA(currRing));

#ifndef SING_NDEBUG
  idTest(F);
  idTest(Q);
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


  const unsigned int m_iFirstAltVar = scaFirstAltVar(currRing);
  const unsigned int m_iLastAltVar  = scaLastAltVar(currRing);

  ideal tempF = id_KillSquares(F, m_iFirstAltVar, m_iLastAltVar, currRing);
  ideal tempQ = Q;

  if(Q == currRing->qideal)
    tempQ = SCAQuotient(currRing);

  strat->z2homog = id_IsSCAHomogeneous(tempF, NULL, NULL, currRing); // wCx == wCy == NULL!
  // redo: no_prod_crit
  const BOOLEAN bIsSCA  = rIsSCA(currRing) && strat->z2homog; // for Z_2 prod-crit
  strat->no_prod_crit   = ! bIsSCA;

//  strat->homog = strat->homog && strat->z2homog; // ?

#if MYTEST
  {
    PrintS("ideal tempF: \n");
    idPrint(tempF);
    PrintS("ideal tempQ: \n");
    idPrint(tempQ);
  }
#endif

  int olddeg, reduc;
  int red_result = 1;
//  int hilbeledeg = 1, minimcnt = 0;
  int hilbcount = 0;

  initBuchMoraCrit(strat); // set Gebauer, honey, sugarCrit

  nc_gr_initBba(tempF,strat); // set enterS, red, initEcart, initEcartPair

  initBuchMoraPos(strat);


  // ?? set spSpolyShort, reduce ???

  initBuchMora(tempF, tempQ, strat); // SCAQuotient(currRing) instead of Q == squares!!!!!!!

  strat->posInT=posInT110; // !!!

  reduc = olddeg = 0;


  // compute-------------------------------------------------------
  for(; strat->Ll >= 0;
#ifdef KDEBUG
    strat->P.lcm = NULL,
#endif
    kTest(strat)
    )
  {
#ifdef KDEBUG
    if (TEST_OPT_DEBUG) messageSets(strat);
#endif

    if (strat->Ll== 0) strat->interpt=TRUE;

    if (TEST_OPT_DEGBOUND
    && ((strat->honey
    && (strat->L[strat->Ll].ecart+ currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
       || ((!strat->honey) && (currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))))
    {
      // stops computation if
      // 24 IN test and the degree +ecart of L[strat->Ll] is bigger then
      // a predefined number Kstd1_deg
      while (strat->Ll >= 0) deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      break;
    }

    // picks the last element from the lazyset L
    strat->P = strat->L[strat->Ll];
    strat->Ll--;

    //kTest(strat);

//    assume(pNext(strat->P.p) != strat->tail); // !???
    if(strat->P.IsNull()) continue;


    if( pNext(strat->P.p) == strat->tail )
    {
      // deletes the int spoly and computes SPoly
      pLmFree(strat->P.p); // ???
      strat->P.p = nc_CreateSpoly(strat->P.p1, strat->P.p2, currRing);
    }

    if(strat->P.IsNull()) continue;

//     poly save = NULL;
//
//     if(pNext(strat->P.p) != NULL)
//       save = p_Copy(strat->P.p, currRing);

    strat->initEcart(&strat->P); // remove it?

    if (TEST_OPT_PROT)
      message((strat->honey ? strat->P.ecart : 0) + strat->P.pFDeg(), &olddeg,&reduc,strat, red_result);

    // reduction of the element chosen from L wrt S
    strat->red(&strat->P,strat);

    if(strat->P.IsNull()) continue;

    addLObject(strat->P, strat);

    const poly save = strat->P.p;

#ifdef PDEBUG
      p_Test(save, currRing);
#endif
    assume( save != NULL );

    // SCA Specials:

    {
      const poly p_next = pNext(save);

      if( p_next != NULL )
      for( unsigned int i = m_iFirstAltVar; i <= m_iLastAltVar; i++ )
      if( p_GetExp(save, i, currRing) != 0 )
      {
        assume(p_GetExp(save, i, currRing) == 1);

        const poly tt = sca_pp_Mult_xi_pp(i, p_next, currRing);

#ifdef PDEBUG
        p_Test(tt, currRing);
#endif

        if( tt == NULL) continue;

        LObject h(tt); // h = x_i * P

        if (TEST_OPT_INTSTRATEGY)
        {
          h.pCleardenom(); // also removes Content
        }
        else
        {
          h.pNorm();
        }

        strat->initEcart(&h);


//         if (pOrdSgn==-1)
//         {
//           cancelunit(&h);  // tries to cancel a unit
//           deleteHC(&h, strat);
//         }

//         if(h.IsNull()) continue;

//         if (TEST_OPT_PROT)
//           message((strat->honey ? h.ecart : 0) + h.pFDeg(), &olddeg, &reduc, strat, red_result);

//         strat->red(&h, strat); // wrt S
//         if(h.IsNull()) continue;

//         poly save = p_Copy(h.p, currRing);

        int pos;

        if (strat->Ll==-1)
          pos =0;
        else
          pos = strat->posInL(strat->L,strat->Ll,&h,strat);

        h.sev = pGetShortExpVector(h.p);
        enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);

  //       h.p = save;
  //       addLObject(h, strat);
      }

      // p_Delete( &save, currRing );
    }


  } // for(;;)


#ifdef KDEBUG
  if (TEST_OPT_DEBUG) messageSets(strat);
#endif

  if (TEST_OPT_REDSB){
    completeReduce(strat); // ???
  }

  // release temp data--------------------------------
  exitBuchMora(strat);

//  if (TEST_OPT_WEIGHTM)
//  {
//    pRestoreDegProcs(currRing,pFDegOld, pLDegOld);
//     if (ecartWeights)
//     {
//       omFreeSize((ADDRESS)ecartWeights,(rVar(currRing)+1)*sizeof(int));
//       ecartWeights=NULL;
//     }
//  }

  if (TEST_OPT_PROT) messageStat(hilbcount,strat);

  if (tempQ!=NULL) updateResult(strat->Shdl,tempQ,strat);

  id_Delete(&tempF, currRing);


  // complete reduction of the standard basis---------
  if (TEST_OPT_REDSB){
    ideal I = strat->Shdl;
    ideal erg = kInterRedOld(I,tempQ);
    assume(I!=erg);
    id_Delete(&I, currRing);
    strat->Shdl = erg;
  }


#if MYTEST
//   PrintS("</sca_gr_bba>\n");
#endif

  if( currRing != save )     rChangeCurrRing(save);

  return (strat->Shdl);
}



///////////////////////////////////////////////////////////////////////////////////////
//************* SCA BBA *************************************************************//
///////////////////////////////////////////////////////////////////////////////////////

// Under development!!!
ideal k_sca_bba (const ideal F, const ideal Q, const intvec */*w*/, const bigintmat * /*hilb*/, kStrategy strat, const ring _currRing)
{
  const ring save = currRing;
  if( currRing != _currRing ) rChangeCurrRing(_currRing);
  assume( currRing == _currRing );

#if MYTEST
  PrintS("\n\n<sca_bba>\n\n");
#endif

  assume(rIsSCA(currRing));

#ifndef SING_NDEBUG
  idTest(F);
  idTest(Q);
#endif

#if MYTEST
  PrintS("\ncurrRing: \n");
  rWrite(currRing);
#ifdef RDEBUG
//  rDebugPrint(currRing);
#endif

  PrintS("\n\nF: \n");
  idPrint(F);
  PrintS("\n\nQ: \n");
  idPrint(Q);

  PrintLn();
#endif


  const unsigned int m_iFirstAltVar = scaFirstAltVar(currRing);
  const unsigned int m_iLastAltVar  = scaLastAltVar(currRing);

  ideal tempF = id_KillSquares(F, m_iFirstAltVar, m_iLastAltVar, currRing);

  ideal tempQ = Q;

  if(Q == currRing->qideal)
    tempQ = SCAQuotient(currRing);

  // Q or tempQ will not be used below :(((


#if MYTEST

  PrintS("tempF: \n");
  idPrint(tempF);
  PrintS("tempQ: \n");
  idPrint(tempQ);
#endif

  strat->z2homog = id_IsSCAHomogeneous(tempF, NULL, NULL, currRing); // wCx == wCy == NULL!
   // redo no_prod_crit:
  const BOOLEAN bIsSCA  = rIsSCA(currRing) && strat->z2homog; // for Z_2 prod-crit
  strat->no_prod_crit   = ! bIsSCA;

//  strat->homog = strat->homog && strat->z2homog; // ?

  int   red_result = 1;
  int   olddeg, reduc;

//  int hilbeledeg = 1, minimcnt = 0;
  int hilbcount = 0;

  BOOLEAN withT = FALSE;

  initBuchMoraCrit(strat); // sets Gebauer, honey, sugarCrit // sca - ok???
  initBuchMoraPos(strat); // sets strat->posInL, strat->posInT // check!! (Plural's: )

//   initHilbCrit(F, Q, &hilb, strat);

//  nc_gr_initBba(F,strat);
  initBba(strat); // set enterS, red, initEcart, initEcartPair

  // set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair
  // ?? set spSpolyShort, reduce ???
  initBuchMora(tempF, tempQ, strat); // tempQ = Q without squares!!!

//   if (strat->minim>0) strat->M = idInit(IDELEMS(F),F->rank);

  reduc = olddeg = 0;

#define NO_BUCKETS

#ifndef NO_BUCKETS
  if (!TEST_OPT_NOT_BUCKETS)
    strat->use_buckets = 1;
#endif

  // redtailBBa against T for inhomogeneous input
  if (!TEST_OPT_OLDSTD)
    withT = ! strat->homog;

  // strat->posInT = posInT_pLength;
  kTest_TS(strat);

#undef HAVE_TAIL_RING

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


  ///////////////////////////////////////////////////////////////
  // SCA:

  //  due to std( SB, p).
  // Note that after initBuchMora :: initSSpecial all these additional
  // elements are in S and T (and some pairs are in L, which also has no initial
  // elements!!!)
  if(TEST_OPT_SB_1)
  {
    // For all additional elements...
    for (int iNewElement = strat->newIdeal; iNewElement < IDELEMS(tempF); iNewElement++)
    {
      const poly pSave = tempF->m[iNewElement];

      if( pSave != NULL )
      {
//        tempF->m[iNewElement] = NULL;

        const poly p_next = pNext(pSave);

        if(p_next != NULL)
          for( unsigned int i = m_iFirstAltVar; i <= m_iLastAltVar; i++ )
            if( p_GetExp(pSave, i, currRing) != 0 )
            {
              assume(p_GetExp(pSave, i, currRing) == 1);

              const poly p_new = sca_pp_Mult_xi_pp(i, p_next, currRing);

#ifdef PDEBUG
              p_Test(p_new, currRing);
#endif

              if( p_new == NULL) continue;

              LObject h(p_new); // h = x_i * strat->P
              h.is_special = TRUE;

              if (TEST_OPT_INTSTRATEGY)
                h.pCleardenom(); // also removes Content
              else
                h.pNorm();

              strat->initEcart(&h);
              h.sev = pGetShortExpVector(h.p);

              int pos = 0;

              if (strat->Ll != -1)
                pos = strat->posInL(strat->L,strat->Ll,&h,strat);

              enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);
            }
      }
    }
  }

  // compute-------------------------------------------------------
  while (strat->Ll >= 0)
  {
#ifdef KDEBUG
    if (TEST_OPT_DEBUG) messageSets(strat);
#endif

    if (strat->Ll== 0) strat->interpt=TRUE;

    if (TEST_OPT_DEGBOUND
        && ((strat->honey && (strat->L[strat->Ll].ecart+currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
            || ((!strat->honey) && (currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))))
    {

#ifdef KDEBUG
//      if (TEST_OPT_DEBUG){PrintS("^^^^?");}
#endif

      // *stops computation if
      // * 24 IN test and the degree +ecart of L[strat->Ll] is bigger then
      // *a predefined number Kstd1_deg
      while ((strat->Ll >= 0)
        && ( (strat->homog==isHomog) || strat->L[strat->Ll].is_special || ((strat->L[strat->Ll].p1!=NULL) && (strat->L[strat->Ll].p2!=NULL)) )
        && ((strat->honey && (strat->L[strat->Ll].ecart+currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
            || ((!strat->honey) && (currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg)))
            )
      {
#ifdef KDEBUG
//        if (TEST_OPT_DEBUG){PrintS("^^^^^^^^^^^^!!!!");}
#endif
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
//        if (TEST_OPT_PROT) PrintS("^!");
      }
      if (strat->Ll<0) break;
      else strat->noClearS=TRUE;
    }

    // picks the last element from the lazyset L
    strat->P = strat->L[strat->Ll];
    strat->Ll--;


//    assume(pNext(strat->P.p) != strat->tail);

    if(strat->P.IsNull()) continue;

    if (pNext(strat->P.p) == strat->tail)
    {
      // deletes the short spoly
      pLmFree(strat->P.p);

      strat->P.p = nc_CreateSpoly(strat->P.p1, strat->P.p2, currRing);
      if (strat->P.p!=NULL) strat->initEcart(&strat->P);
    }//    else


    if(strat->P.IsNull()) continue;

    if (strat->P.p1 == NULL)
    {
//       if (strat->minim > 0)
//         strat->P.p2=p_Copy(strat->P.p, currRing, strat->tailRing);


      // for input polys, prepare reduction
        strat->P.PrepareRed(strat->use_buckets);
    }

    if (TEST_OPT_PROT)
      message((strat->honey ? strat->P.ecart : 0) + strat->P.pFDeg(),
              &olddeg,&reduc,strat, red_result);

    // reduction of the element chosen from L
    red_result = strat->red(&strat->P,strat);


    // reduction to non-zero new poly
    if (red_result == 1)
    {
      // statistic
      if (TEST_OPT_PROT) PrintS("s");

      // get the polynomial (canonicalize bucket, make sure P.p is set)
      strat->P.GetP(strat->lmBin);

      int pos = posInS(strat,strat->sl,strat->P.p,strat->P.ecart);

      // reduce the tail and normalize poly
      if (TEST_OPT_INTSTRATEGY)
      {
        strat->P.pCleardenom();
        if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
        {
          strat->P.p = redtailBba(&(strat->P),pos-1,strat, withT); // !!!
          strat->P.pCleardenom();
        }
      }
      else
      {
        strat->P.pNorm();
        if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
          strat->P.p = redtailBba(&(strat->P),pos-1,strat, withT);
      }
      strat->P.is_normalized=nIsOne(pGetCoeff(strat->P.p));

#ifdef KDEBUG
      if (TEST_OPT_DEBUG){PrintS(" ns:");p_wrp(strat->P.p,currRing);PrintLn();}
#endif

//       // min_std stuff
//       if ((strat->P.p1==NULL) && (strat->minim>0))
//       {
//         if (strat->minim==1)
//         {
//           strat->M->m[minimcnt]=p_Copy(strat->P.p,currRing,strat->tailRing);
//           p_Delete(&strat->P.p2, currRing, strat->tailRing);
//         }
//         else
//         {
//           strat->M->m[minimcnt]=strat->P.p2;
//           strat->P.p2=NULL;
//         }
//         if (strat->tailRing!=currRing && pNext(strat->M->m[minimcnt])!=NULL)
//           pNext(strat->M->m[minimcnt])
//             = strat->p_shallow_copy_delete(pNext(strat->M->m[minimcnt]),
//                                            strat->tailRing, currRing,
//                                            currRing->PolyBin);
//         minimcnt++;
//       }

      // enter into S, L, and T
      //if(withT)
      {
        strat->P.SetpFDeg();
        enterT(strat->P, strat);
      }

      // L
      enterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat, strat->tl);

      // posInS only depends on the leading term
      strat->enterS(strat->P, pos, strat, strat->tl);

//       if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);

//      Print("[%d]",hilbeledeg);
      kDeleteLcm(&strat->P);

      // //////////////////////////////////////////////////////////
      // SCA:
      const poly pSave = strat->P.p;
      const poly p_next = pNext(pSave);

//       if(0)
      if( p_next != NULL )
      for( unsigned int i = m_iFirstAltVar; i <= m_iLastAltVar; i++ )
      if( p_GetExp(pSave, i, currRing) != 0 )
      {
        assume(p_GetExp(pSave, i, currRing) == 1);
        const poly p_new = sca_pp_Mult_xi_pp(i, p_next, currRing);

#ifdef PDEBUG
        p_Test(p_new, currRing);
#endif

        if( p_new == NULL) continue;

        LObject h(p_new); // h = x_i * strat->P

        h.is_special = TRUE;

        if (TEST_OPT_INTSTRATEGY)
        {
          h.pCleardenom(); // also removes Content
        }
        else
        {
          h.pNorm();
        }

        strat->initEcart(&h);
        h.sev = pGetShortExpVector(h.p);

        int pos = 0;

        if (strat->Ll != -1)
          pos = strat->posInL(strat->L,strat->Ll,&h,strat);

        enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);




#if 0
        h.sev = pGetShortExpVector(h.p);
        strat->initEcart(&h);

        h.PrepareRed(strat->use_buckets);

        // reduction of the element chosen from L(?)
        red_result = strat->red(&h,strat);

        // reduction to non-zero new poly
        if (red_result != 1) continue;


        int pos = posInS(strat,strat->sl,h.p,h.ecart);

        // reduce the tail and normalize poly
        if (TEST_OPT_INTSTRATEGY)
        {
          h.pCleardenom();
          if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
          {
            h.p = redtailBba(&(h),pos-1,strat, withT); // !!!
            h.pCleardenom();
          }
        }
        else
        {
          h.pNorm();
          if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
            h.p = redtailBba(&(h),pos-1,strat, withT);
        }

#ifdef KDEBUG
        if (TEST_OPT_DEBUG){PrintS(" N:");h.wrp();PrintLn();}
#endif

//        h.PrepareRed(strat->use_buckets); // ???

        h.sev = pGetShortExpVector(h.p);
        strat->initEcart(&h);

        if (strat->Ll==-1)
          pos = 0;
        else
          pos = strat->posInL(strat->L,strat->Ll,&h,strat);

         enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);
// the end of "#if 0" (comment)
#endif

      } // for all x_i \in Ann(lm(P))
    } // if red(P) != NULL

//     else if (strat->P.p1 == NULL && strat->minim > 0)
//     {
//       p_Delete(&strat->P.p2, currRing, strat->tailRing);
//     }

#ifdef KDEBUG
//    memset(&(strat->P), 0, sizeof(strat->P));
#endif

    kTest_TS(strat); // even of T is not used!

//     Print("\n$\n");

  }

#ifdef KDEBUG
  if (TEST_OPT_DEBUG) messageSets(strat);
#endif

  // complete reduction of the standard basis---------

  if (TEST_OPT_REDSB)
  {
    completeReduce(strat);
  }

  //release temp data--------------------------------

  exitBuchMora(strat); // cleanT!

  id_Delete(&tempF, currRing);

//  if (TEST_OPT_WEIGHTM)
//  {
//    pRestoreDegProcs(currRing, pFDegOld, pLDegOld);
//     if (ecartWeights)
//     {
//       omFreeSize((ADDRESS)ecartWeights,(rVar(currRing)+1)*sizeof(short));
//       ecartWeights=NULL;
//     }
//  }

  if (TEST_OPT_PROT) messageStat(hilbcount,strat);



  if (tempQ!=NULL) updateResult(strat->Shdl,tempQ,strat);


  if (TEST_OPT_REDSB) // ???
  {
    // must be at the very end (after exitBuchMora) as it changes the S set!!!
    ideal I = strat->Shdl;
    ideal erg = kInterRedOld(I,tempQ);
    assume(I!=erg);
    id_Delete(&I, currRing);
    strat->Shdl = erg;
  }

#if MYTEST
  PrintS("\n\n</sca_bba>\n\n");
#endif

  if( currRing != save )     rChangeCurrRing(save);

  return (strat->Shdl);
}

// //////////////////////////////////////////////////////////////////////////////
// sca mora...

// returns TRUE if mora should use buckets, false otherwise
#ifdef MORA_USE_BUCKETS
static BOOLEAN kMoraUseBucket(kStrategy strat)
#else
static BOOLEAN kMoraUseBucket(kStrategy)
#endif
{
#ifdef MORA_USE_BUCKETS
  if (TEST_OPT_NOT_BUCKETS)
    return FALSE;
  if (strat->red == redFirst)
  {
#ifdef NO_LDEG
    if (!strat->syzComp)
      return TRUE;
#else
    if ((strat->homog || strat->honey) && !strat->syzComp)
      return TRUE;
#endif
  }
  else
  {
    assume(strat->red == redEcart);
    if (strat->honey && !strat->syzComp)
      return TRUE;
  }
#endif
  return FALSE;
}

// ideal sca_mora (ideal F, ideal Q, intvec *w, intvec *, kStrategy strat)
ideal k_sca_mora(const ideal F, const ideal Q, const intvec */*w*/, const bigintmat *, kStrategy strat, const ring _currRing)
{
  const ring save = currRing;
  if( currRing != _currRing ) rChangeCurrRing(_currRing);
  assume( currRing == _currRing );

  assume(rIsSCA(currRing));

  const unsigned int m_iFirstAltVar = scaFirstAltVar(currRing);
  const unsigned int m_iLastAltVar  = scaLastAltVar(currRing);

  ideal tempF = id_KillSquares(F, m_iFirstAltVar, m_iLastAltVar, currRing);

  ideal tempQ = Q;

  if(Q == currRing->qideal)
    tempQ = SCAQuotient(currRing);

  bool bIdHomog = id_IsSCAHomogeneous(tempF, NULL, NULL, currRing); // wCx == wCy == NULL!

  assume( !bIdHomog || strat->homog ); //  bIdHomog =====[implies]>>>>> strat->homog

  strat->homog = strat->homog && bIdHomog;

#ifdef PDEBUG
  assume( strat->homog == bIdHomog );
#endif

  strat->update = TRUE;
  //- setting global variables ------------------- -
  initBuchMoraCrit(strat);
//   initHilbCrit(F,NULL,&hilb,strat); // no Q!
  initMora(tempF, strat);
  initBuchMoraPos(strat);
  //Shdl=
    initBuchMora(tempF, tempQ, strat); // temp Q, F!
//   if (TEST_OPT_FASTHC) missingAxis(&strat->lastAxis,strat);
  // updateS in initBuchMora has Hecketest
  // * and could have put strat->kHEdgdeFound FALSE
#if 0
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
#endif
  strat->use_buckets = kMoraUseBucket(strat);

  kTest_TS(strat);


  int olddeg = 0;
  int reduc = 0;
  int red_result = 1;
//  int hilbeledeg=1;
  int hilbcount=0;


  //- compute-------------------------------------------

#undef HAVE_TAIL_RING

#ifdef HAVE_TAIL_RING
//  if (strat->homog && strat->red == redFirst)
//     kStratInitChangeTailRing(strat);
#endif





//  due to std( SB, p)
  if(TEST_OPT_SB_1)
  {
    for (int iNewElement = strat->newIdeal; iNewElement < IDELEMS(tempF); iNewElement++)
    {

      const poly pSave = tempF->m[iNewElement];

      if( pSave != NULL )
      {
//        tempF->m[iNewElement] = NULL;

        const poly p_next = pNext(pSave);

        if(p_next != NULL)
          for( unsigned int i = m_iFirstAltVar; i <= m_iLastAltVar; i++ )
            if( p_GetExp(pSave, i, currRing) != 0 )
            {

              assume(p_GetExp(pSave, i, currRing) == 1);

              const poly p_new = sca_pp_Mult_xi_pp(i, p_next, currRing);

#ifdef PDEBUG
              p_Test(p_new, currRing);
#endif

              if( p_new == NULL) continue;

              LObject h(p_new); // h = x_i * strat->P

              if (TEST_OPT_INTSTRATEGY)
                h.pCleardenom(); // also removes Content
              else
                h.pNorm();

              strat->initEcart(&h);
              h.sev = pGetShortExpVector(h.p);

              int pos = 0;

              if (strat->Ll != -1)
                pos = strat->posInL(strat->L,strat->Ll,&h,strat);

              enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);
            }
      }

    }
  }

  while (strat->Ll >= 0)
  {
    //test_int_std(strat->kIdeal);
#ifdef KDEBUG
    if (TEST_OPT_DEBUG) messageSets(strat);
#endif
    if (TEST_OPT_DEGBOUND
    && (strat->L[strat->Ll].ecart+strat->L[strat->Ll].GetpFDeg()> Kstd1_deg))
    {
      // * stops computation if
      // * - 24 (degBound)
      // *   && upper degree is bigger than Kstd1_deg
      while ((strat->Ll >= 0)
        && (strat->L[strat->Ll].p1!=NULL) && (strat->L[strat->Ll].p2!=NULL)
        && (strat->L[strat->Ll].ecart+strat->L[strat->Ll].GetpFDeg()> Kstd1_deg)
      )
      {
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
        //if (TEST_OPT_PROT)
        //{
        //   PrintS("D"); mflush();
        //}
      }
      if (strat->Ll<0) break;
      else strat->noClearS=TRUE;
    }
    strat->P = strat->L[strat->Ll];// - picks the last element from the lazyset L -
    if (strat->Ll==0) strat->interpt=TRUE;
    strat->Ll--;

    // create the real Spoly
//    assume(pNext(strat->P.p) != strat->tail);

    if(strat->P.IsNull()) continue;


    if( pNext(strat->P.p) == strat->tail )
    {
      // deletes the int spoly and computes SPoly
      pLmFree(strat->P.p); // ???
      strat->P.p = nc_CreateSpoly(strat->P.p1, strat->P.p2, currRing);
    }



    if (strat->P.p1 == NULL)
    {
      // for input polys, prepare reduction (buckets !)
      strat->P.SetLength(strat->length_pLength);
      strat->P.PrepareRed(strat->use_buckets);
    }

    if (!strat->P.IsNull())
    {
      // might be NULL from noether !!!
      if (TEST_OPT_PROT)
        message(strat->P.ecart+strat->P.GetpFDeg(),&olddeg,&reduc,strat, red_result);
      // reduce
      red_result = strat->red(&strat->P,strat);
    }

    if (! strat->P.IsNull())
    {
      strat->P.GetP();
      // statistics
      if (TEST_OPT_PROT) PrintS("s");
      // normalization
      if (!TEST_OPT_INTSTRATEGY)
        strat->P.pNorm();
      // tailreduction
      strat->P.p = redtail(&(strat->P),strat->sl,strat);
      // set ecart -- might have changed because of tail reductions
      if ((!strat->noTailReduction) && (!strat->honey))
        strat->initEcart(&strat->P);
      // cancel unit
      cancelunit(&strat->P);
      // for char 0, clear denominators
      if (TEST_OPT_INTSTRATEGY)
        strat->P.pCleardenom();

      // put in T
      enterT(strat->P,strat);
      // build new pairs
      enterpairs(strat->P.p,strat->sl,strat->P.ecart,0,strat, strat->tl);
      // put in S
      strat->enterS(strat->P,
                    posInS(strat,strat->sl,strat->P.p, strat->P.ecart),
                    strat, strat->tl);


      // clear strat->P
      kDeleteLcm(&strat->P);

      // //////////////////////////////////////////////////////////
      // SCA:
      const poly pSave = strat->P.p;
      const poly p_next = pNext(pSave);

      if(p_next != NULL)
      for( unsigned int i = m_iFirstAltVar; i <= m_iLastAltVar; i++ )
      if( p_GetExp(pSave, i, currRing) != 0 )
      {

        assume(p_GetExp(pSave, i, currRing) == 1);

        const poly p_new = sca_pp_Mult_xi_pp(i, p_next, currRing);

#ifdef PDEBUG
        p_Test(p_new, currRing);
#endif

        if( p_new == NULL) continue;

        LObject h(p_new); // h = x_i * strat->P

        if (TEST_OPT_INTSTRATEGY)
           h.pCleardenom(); // also removes Content
        else
          h.pNorm();

        strat->initEcart(&h);
        h.sev = pGetShortExpVector(h.p);

        int pos = 0;

        if (strat->Ll != -1)
          pos = strat->posInL(strat->L,strat->Ll,&h,strat);

        enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);
      }

#ifdef KDEBUG
      // make sure kTest_TS does not complain about strat->P
      strat->P.Init();
#endif
    }
#if 0
    if (strat->kHEdgeFound)
    {
      if ((TEST_OPT_FINDET)
      || ((TEST_OPT_MULTBOUND) && (scMult0Int((strat->Shdl)) < mu)))
      {
        // obachman: is this still used ???
        // * stops computation if strat->kHEdgeFound and
        // * - 27 (finiteDeterminacyTest)
        // * or
        // * - 23
        // *   (multBound)
        // *   && multiplicity of the ideal is smaller then a predefined number mu
        while (strat->Ll >= 0) deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      }
    }
#endif
    kTest_TS(strat);
  }
  // - complete reduction of the standard basis------------------------ -
  if (TEST_OPT_REDSB) completeReduce(strat);
  // - release temp data------------------------------- -
  exitBuchMora(strat);
  // - polynomials used for HECKE: HC, noether -
  if (TEST_OPT_FINDET)
  {
    if (strat->kNoether!=NULL)
      Kstd1_mu=currRing->pFDeg(strat->kNoether,currRing);
    else
      Kstd1_mu=-1;
  }
  if(strat->kNoether!=NULL) pLmFree(&strat->kNoether);
  strat->update = TRUE; //???
  strat->lastAxis = 0; //???
  omFreeSize((ADDRESS)strat->NotUsedAxis,(rVar(currRing)+1)*sizeof(BOOLEAN));
  if (TEST_OPT_PROT) messageStat(hilbcount,strat);
//  if (TEST_OPT_WEIGHTM)
//  {
//    pRestoreDegProcs(currRing, pFDegOld, pLDegOld);
//     if (ecartWeights)
//     {
//       omFreeSize((ADDRESS)ecartWeights,(rVar(currRing)+1)*sizeof(short));
//       ecartWeights=NULL;
//     }
//  }
  if (tempQ!=NULL) updateResult(strat->Shdl,tempQ,strat);
  idTest(strat->Shdl);

  id_Delete( &tempF, currRing);

  if( currRing != save )     rChangeCurrRing(save);

  return (strat->Shdl);
}

