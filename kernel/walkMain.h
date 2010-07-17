#ifndef WALKMAIN_H
#define WALKMAIN_H

#include <kernel/intvec.h>
#include <kernel/int64vec.h>

enum WalkState{
    WalkNoIdeal,
    WalkIncompatibleRings,
    WalkIntvecProblem,
    WalkOverFlowError,
    /*
    these could be defined to make error management more elegant
    WalkOverFlowError1,
    WalkOverFlowError2,
    WalkOverFlowError3,
    WalkOverFlowError4,
    WalkOverFlowError5,
    WalkOverFlowError6,
    WalkOverFlowError7,
    WalkOverFlowError8,
    WalkOverFlowError9,
    WalkOverFlowError10,
    WalkOverFlowError11,
    WalkOverFlowError12,
    WalkOverFlowError13,
    */
    WalkIncompatibleDestRing,
    WalkIncompatibleSourceRing,
    WalkOk

};

/*
overflow_error table
 1: Miv64DotProduct mult
 2: Miv64DotProduct add
 3: gett64 zaehler mult
 4: gett64 zaehler add (not necessarily overflow but quite probable)
 5: gett64 nenner mult
 6: gett64 nenner add (not necessarily overflow but quite probable)
 7: nextw64 mult a
 8: nextw64 mult b
 9: nextw64 add (not necessarily overflow but quite probable)
10: getinveps64 mult
11: getinveps64 add
12: gettaun64 mult
13: gettaun64 add (not necessarily overflow but quite probable)
*/


WalkState walkstep64(ideal & G,int64vec* currw,int step);
WalkState walk64(ideal I,int64vec* currw64,ring destRing,int64vec* destVec64,ideal  & destIdeal,BOOLEAN sourceIsSB=FALSE);

//ANOTHER INPUT-VARIABLE ADDED: unperturbedStartVectorStrategy
//THIS SHOULD BE SET IN walkProc.cc BY THE USER
WalkState fractalWalk64(ideal sourceIdeal,ring destRing,ideal & destIdeal,BOOLEAN sourceIsSB,BOOLEAN unperturbedStartVectorStrategy);
//REPLACES firstWalkStep64 FOR fractalWalk64
WalkState unperturbedFirstStep64(ideal & G,int64vec* currw64, ring destRing);

#endif
