/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: frwalk: interpreter link
*/

//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <Singular/mod2.h>
#include <Singular/tok.h>
#include <kernel/options.h>
#include <Singular/ipid.h>
#include <kernel/intvec.h>
#include <omalloc.h>
#include <kernel/febase.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <kernel/matpol.h>
#include <kernel/kstd1.h>
#include <kernel/ring.h>
#include <Singular/subexpr.h>
#include <kernel/maps.h>
#include <kernel/syz.h>
#include <kernel/numbers.h>
#include <Singular/lists.h>
#include <Singular/attrib.h>
#include <Singular/ipconv.h>
#include <Singular/silink.h>
#include <kernel/stairc.h>
#include <kernel/weight.h>
#include <kernel/semic.h>
#include <kernel/splist.h>
#include <kernel/spectrum.h>
#include <kernel/gnumpfl.h>
#include <kernel/mpr_base.h>
#include <kernel/ffields.h>
#include <kernel/clapsing.h>
#include <kernel/hutil.h>
#include <Singular/ipshell.h>
#include <kernel/walkMain.h>
#include <kernel/walkProc.h>
#include <kernel/walkSupport.h>
#include <kernel/prCopy.h>

///////////////////////////////////////////////////////////////////
//walkProc
///////////////////////////////////////////////////////////////////
//Description: The main function for the Walk-Algorithm. Checks the
//input-data, and calls walk64 (see walkMain.cc). Returns the new
//groebner basis or something else if an error occoured.
///////////////////////////////////////////////////////////////////
//Uses: omAlloc0,walkConsistency,rGetGlobalOrderWeightVec,
//omFreeSize,sizeof,IDIDEAL,walk64,rSetHdl,idrMoveR,Werror,idInit
///////////////////////////////////////////////////////////////////

ideal
walkProc(leftv first, leftv second)
{
    WalkState state = WalkOk;
    BITSET saveTest=test;
    test &= (~Sy_bit(OPT_REDSB)); //make sure option noredSB is set

    idhdl destRingHdl = currRingHdl;
    ring destRing = currRing;
    ideal destIdeal = NULL;
    idhdl sourceRingHdl = (idhdl)first->data;
    rSetHdl( sourceRingHdl );
    ring sourceRing = currRing;

    if(state==WalkOk)
    {
      int * vperm = (int *)omAlloc0( (pVariables+1)*sizeof( int ) );
      state= walkConsistency( IDRING(sourceRingHdl), IDRING(destRingHdl), vperm );
      omFreeSize( (ADDRESS)vperm, (pVariables+1)*sizeof(int) );
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

    test=saveTest;//making sure options are as before function call

    ring almostDestRing=currRing;
    rSetHdl(destRingHdl);

    switch (state) {
        case WalkOk:
          destIdeal=idrMoveR(destIdeal,almostDestRing);
          break;

        case WalkIncompatibleRings:
          Werror("ring %s and current ring are incompatible\n",
                 first->Name() );
          destIdeal= idInit(0,0);
          break;

        case WalkIncompatibleDestRing:
          Werror( "Order of basering not allowed,\n must be a combination of a,A,lp,dp,Dp,wp,Wp,M and C.\n");
          destIdeal= idInit(0,0);
          break;

        case WalkIncompatibleSourceRing:
          Werror( "Order of %s not allowed,\n must be a combination of a,A,lp,dp,Dp,wp,Wp,M and C.\n",first->Name());
          rSetHdl(destRingHdl);
          destIdeal= idInit(0,0);
          break;

        case WalkNoIdeal:
          Werror( "Can't find ideal %s in ring %s.\n",
                   second->Name(), first->Name() );
          destIdeal= idInit(0,0);
          break;

        case WalkOverFlowError:
          Werror( "Overflow occured.\n");
          destIdeal= idInit(0,0);
          break;

        default:
           destIdeal= idInit(1,1);
    }

    return destIdeal;
}

///////////////////////////////////////////////////////////////////
//fractalWalkProc
///////////////////////////////////////////////////////////////////
//Description: The main function for the Fractalwalk-Algorithm.
//Responsible for contact between user and walk64. Checks the
//input-data, and calls fractalWalk64. Returns the new groebner
//basis or something else if an error occured.
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
    BITSET saveTest=test;
    test &= (~Sy_bit(OPT_REDSB)); //make sure option noredSB is set

    idhdl destRingHdl = currRingHdl;
    ring destRing = currRing;
    ideal destIdeal = NULL;
    idhdl sourceRingHdl = (idhdl)first->data;
    rSetHdl( sourceRingHdl );
    ring sourceRing = currRing;

    int * vperm = (int *)omAlloc0( (pVariables+1)*sizeof( int ) );
    state= fractalWalkConsistency( IDRING(sourceRingHdl), IDRING(destRingHdl), vperm );
    omFreeSize( (ADDRESS)vperm, (pVariables+1)*sizeof(int) );

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

    test=saveTest;//making sure options are as before functiocall

     if ( state == WalkOk )
     {
       ring almostDestRing=currRing;
       rSetHdl(destRingHdl);
       destIdeal=idrMoveR(destIdeal,almostDestRing);
     }


     switch (state) {

        case WalkOk:
            destIdeal=sortRedSB(destIdeal);
            return(destIdeal);
            break;

        case WalkIncompatibleRings:
            Werror( "ring %s and current ring are incompatible\n",
                     first->Name() );
            rSetHdl(destRingHdl);
            destIdeal= idInit(0,0);
            return destIdeal;
            break;

        case WalkIncompatibleDestRing:
            Werror( "Order of basering not allowed,\n must be a combination of lp,dp,Dp,wp,Wp and C or just M.\n");
            rSetHdl(destRingHdl);
            destIdeal= idInit(0,0);
            return destIdeal;
            break;

        case WalkIncompatibleSourceRing:
            Werror( "Order of %s not allowed,\n must be a combination of lp,dp,Dp,wp,Wp and C or just M.\n",
                     first->Name());
            rSetHdl(destRingHdl);
            destIdeal= idInit(0,0);
            return destIdeal;
            break;

        case WalkNoIdeal:
            Werror( "Can't find ideal %s in ring %s.\n",
                     second->Name(), first->Name() );
            rSetHdl(destRingHdl);
            destIdeal= idInit(0,0);
            return destIdeal;
            break;

        case WalkOverFlowError:
            Werror( "Overflow occured in ring %s.\n", first->Name() );
            rSetHdl(destRingHdl);
            destIdeal= idInit(0,0);
            return destIdeal;
            break;

        default:
            rSetHdl(destRingHdl);
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
