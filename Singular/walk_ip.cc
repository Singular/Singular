/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: walk_ip.cc,v 1.2 2008-09-22 11:43:59 Singular Exp $ */
/*
* ABSTRACT: frwalk: interpreter link
*/

//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "mod2.h"
#include "tok.h"
#include "ipid.h"
#include "intvec.h"
#include "omalloc.h"
#include "febase.h"
#include "polys.h"
#include "ideals.h"
#include "matpol.h"
#include "kstd1.h"
#include "ring.h"
#include "subexpr.h"
#include "maps.h"
#include "syz.h"
#include "numbers.h"
#include "lists.h"
#include "attrib.h"
#include "ipconv.h"
#include "silink.h"
#include "stairc.h"
#include "weight.h"
#include "semic.h"
#include "splist.h"
#include "spectrum.h"
#include "gnumpfl.h"
#include "mpr_base.h"
#include "ffields.h"
#include "clapsing.h"
#include "hutil.h"
#include "ipshell.h"
#include "walkMain.h"
#include "walkProc.h"
#include "walkSupport.h"
#include "prCopy.h"

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
      state= walkConsistency( sourceRingHdl, destRingHdl, vperm );
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
    state= fractalWalkConsistency( sourceRingHdl, destRingHdl, vperm );
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

int64 getint64(lists l)
{
  return (int64)(l->m[1].data);
}

///////////////////////////////////////////////////////////////////
