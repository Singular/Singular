/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: frwalk: interpreter link
*/

#include "kernel/mod2.h"
#include "Singular/tok.h"
#include "misc/options.h"
#include "Singular/ipid.h"
#include "misc/intvec.h"
#include "omalloc/omalloc.h"
#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "polys/matpol.h"
#include "kernel/GBEngine/kstd1.h"
#include "polys/monomials/ring.h"
#include "Singular/subexpr.h"
#include "polys/monomials/maps.h"
#include "kernel/GBEngine/syz.h"
#include "coeffs/numbers.h"
#include "Singular/lists.h"
#include "Singular/attrib.h"
#include "Singular/ipconv.h"
#include "Singular/links/silink.h"
#include "kernel/combinatorics/stairc.h"
#include "polys/weight.h"
#include "kernel/spectrum/semic.h"
#include "kernel/spectrum/splist.h"
#include "kernel/spectrum/spectrum.h"
//#include "kernel/gnumpfl.h"
//#include "kernel/mpr_base.h"
//#include "kernel/ffields.h"
#include "polys/clapsing.h"
#include "kernel/combinatorics/hutil.h"
#include "Singular/ipshell.h"
#include "kernel/groebner_walk/walkMain.h"
#include "kernel/groebner_walk/walkProc.h"
#include "kernel/groebner_walk/walkSupport.h"
#include "polys/prCopy.h"

///////////////////////////////////////////////////////////////////
//walkProc
///////////////////////////////////////////////////////////////////
//Description: The main function for the Walk-Algorithm. Checks the
//input-data, and calls walk64 (see walkMain.cc). Returns the new
//groebner basis or something else if an error occured.
///////////////////////////////////////////////////////////////////
//Uses: omAlloc0,walkConsistency,rGetGlobalOrderWeightVec,
//omFreeSize,sizeof,IDIDEAL,walk64,rSetHdl,idrMoveR,Werror,idInit
///////////////////////////////////////////////////////////////////

ideal
walkProc(leftv first, leftv second)
{
    WalkState state = WalkOk;
    BITSET save1,save2;
    SI_SAVE_OPT(save1,save2);
    si_opt_1 &= (~Sy_bit(OPT_REDSB)); //make sure option noredSB is set

    ring destRing = currRing;
    ideal destIdeal = NULL;
    idhdl sourceRingHdl = (idhdl)first->data;
    ring sourceRing = IDRING(sourceRingHdl);
    rChangeCurrRing( sourceRing );

    if(state==WalkOk)
    {
      int * vperm = (int *)omAlloc0( (currRing->N+1)*sizeof( int ) );
      state= walkConsistency( sourceRing, destRing, vperm );
      omFreeSize( (ADDRESS)vperm, (currRing->N+1)*sizeof(int) );
    }

    int64vec* currw64=rGetGlobalOrderWeightVec(sourceRing);
    int64vec* destVec64=rGetGlobalOrderWeightVec(destRing);

    ideal sourceIdeal;
    BOOLEAN sourcIdealIsSB=FALSE;
    if ( state == WalkOk )
    {
      idhdl ih = currRing->idroot->get( second->Name(), myynest );
      if ( (ih != NULL) && (IDTYP(ih)==IDEAL_CMD) )
      {
           sourceIdeal = idCopy(IDIDEAL( ih ));
           if(hasFlag((leftv)ih,FLAG_STD)){
              sourcIdealIsSB=TRUE;
           }
      }
      else
      {
        state=WalkNoIdeal;
      }
    }

    if ( state == WalkOk )
    {
      // Now the settings are compatible with Walk
      state=walk64(sourceIdeal,currw64,destRing,destVec64,
                   destIdeal,sourcIdealIsSB);
    }

    SI_RESTORE_OPT(save1,save2);//making sure options are as before function call

    ring almostDestRing=currRing;
    rChangeCurrRing(destRing);

    switch (state) {
        case WalkOk:
          destIdeal=idrMoveR(destIdeal,currRing,almostDestRing);
          break;

        case WalkIncompatibleRings:
          Werror("ring %s and current ring are incompatible\n",
                 first->Name() );
          destIdeal= NULL;
          break;

        case WalkIncompatibleDestRing:
          WerrorS( "Order of basering not allowed,\n must be a combination of a,A,lp,dp,Dp,wp,Wp,M and C.\n");
          destIdeal= NULL;
          break;

        case WalkIncompatibleSourceRing:
          Werror( "Order of %s not allowed,\n must be a combination of a,A,lp,dp,Dp,wp,Wp,M and C.\n",first->Name());
          rChangeCurrRing(destRing);
          destIdeal= NULL;
          break;

        case WalkNoIdeal:
          Werror( "Can't find ideal %s in ring %s.\n",
                   second->Name(), first->Name() );
          destIdeal= NULL;
          break;

        case WalkOverFlowError:
          WerrorS( "Overflow occurred.\n");
          destIdeal= NULL;
          break;

        default:
           destIdeal= NULL;
    }

    return destIdeal;
}

///////////////////////////////////////////////////////////////////
//fractalWalkProc
///////////////////////////////////////////////////////////////////
//Description: The main function for the Fractalwalk-Algorithm.
//Responsible for contact between user and walk64. Checks the
//input-data, and calls fractalWalk64. Returns the new groebner
//basis or something else if an error occurred.
///////////////////////////////////////////////////////////////////
//Uses: omAlloc0,fractalWalkConsistency,omFreeSize,sizeof,IDIDEAL,
//fractalWalk64,rSetHdl,idrMoveR,Werror,idInit
///////////////////////////////////////////////////////////////////

ideal
fractalWalkProc(leftv first, leftv second)
{

  //unperturbedStartVectorStrategy SHOULD BE SET BY THE USER THROUGH
  //A THIRD ARGUMENT. TRUE MEANS THAT THE UNPERTURBED START
  //VECTOR STRATEGY IS USED AND FALSE THAT THE START VECTOR IS
  //MAXIMALLY PERTURBED

    BOOLEAN unperturbedStartVectorStrategy=TRUE;

    WalkState state = WalkOk;
    BITSET save1,save2;
    SI_SAVE_OPT(save1,save2);
    si_opt_1 &= (~Sy_bit(OPT_REDSB)); //make sure option noredSB is set

    ring destRing = currRing;
    ideal destIdeal = NULL;
    idhdl sourceRingHdl = (idhdl)first->data;
    rSetHdl( sourceRingHdl );
    ring sourceRing = currRing;

    int * vperm = (int *)omAlloc0( (currRing->N+1)*sizeof( int ) );
    state= fractalWalkConsistency( sourceRing, destRing, vperm );
    omFreeSize( (ADDRESS)vperm, (currRing->N+1)*sizeof(int) );

    ideal sourceIdeal;
    BOOLEAN sourcIdealIsSB=FALSE;
    if ( state == WalkOk ) {
      idhdl ih = currRing->idroot->get( second->Name(), myynest );
      if ( (ih != NULL) && (IDTYP(ih)==IDEAL_CMD) ) {
           sourceIdeal = IDIDEAL( ih );
           if(hasFlag((leftv)ih,FLAG_STD)){
              sourcIdealIsSB=TRUE;
           }
      }
      else {
        state=WalkNoIdeal;
      }
    }

    if ( state == WalkOk ) {
      // Now the settings are compatible with Walk
      state=fractalWalk64(sourceIdeal,destRing,destIdeal,
                          sourcIdealIsSB,
                          unperturbedStartVectorStrategy);
    }

    SI_RESTORE_OPT(save1,save2);//making sure options are as before functiocall

     if ( state == WalkOk )
     {
       ring almostDestRing=currRing;
       rChangeCurrRing(destRing);
       destIdeal=idrMoveR(destIdeal, almostDestRing, destRing);
     }


     switch (state) {

        case WalkOk:
            destIdeal=sortRedSB(destIdeal);
            return(destIdeal);
            break;

        case WalkIncompatibleRings:
            Werror( "ring %s and current ring are incompatible\n",
                     first->Name() );
            rChangeCurrRing(destRing);
            destIdeal= NULL;
            return destIdeal;
            break;

        case WalkIncompatibleDestRing:
            Werror( "Order of basering not allowed,\n must be a combination of lp,dp,Dp,wp,Wp and C or just M.\n");
            rChangeCurrRing(destRing);
            destIdeal= NULL;
            return destIdeal;
            break;

        case WalkIncompatibleSourceRing:
            Werror( "Order of %s not allowed,\n must be a combination of lp,dp,Dp,wp,Wp and C or just M.\n",
                     first->Name());
            rChangeCurrRing(destRing);
            destIdeal= NULL;
            return destIdeal;
            break;

        case WalkNoIdeal:
            Werror( "Can't find ideal %s in ring %s.\n",
                     second->Name(), first->Name() );
            rChangeCurrRing(destRing);
            destIdeal= NULL;
            return destIdeal;
            break;

        case WalkOverFlowError:
            Werror( "Overflow occurred in ring %s.\n", first->Name() );
            rChangeCurrRing(destRing);
            destIdeal= NULL;
            return destIdeal;
            break;

        default:
            rChangeCurrRing(destRing);
            destIdeal= idInit(1,1);
            return destIdeal;
    }


  return NULL;
}


///////////////////////////////////////////////////////////////////
//getiv64
///////////////////////////////////////////////////////////////////
//Description: retrieves the int64vec from input list l
///////////////////////////////////////////////////////////////////
//Assumes: that the first entry of l is an int64vec
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

int64vec* getiv64(lists l)
{
  return (int64vec*)(l->m[0].data);
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//getint64
///////////////////////////////////////////////////////////////////
//Description: retrieves the int64 from input list l
///////////////////////////////////////////////////////////////////
//Assumes: that the second entry of l is an int64
///////////////////////////////////////////////////////////////////
//Uses: none
///////////////////////////////////////////////////////////////////

// not used, bad impl.
//int64 getint64(lists l)
//{
//  return (int64)(long)(l->m[1].data);
//}

///////////////////////////////////////////////////////////////////
