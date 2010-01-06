#ifndef WALKPROC_H
#define WALKPROC_H
#include "walkMain.h"
ideal walkProc(leftv first, leftv second);
ideal fractalWalkProc(leftv first, leftv second);
WalkState walkConsistency( ring sring, ring dring, int * vperm );
WalkState fractalWalkConsistency( ring sring, ring dring, int * vperm );
#endif

