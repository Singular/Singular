#ifndef WALKPROC_H
#define WALKPROC_H
#include "walkMain.h"
ideal walkProc(leftv first, leftv second);
ideal fractalWalkProc(leftv first, leftv second);
WalkState walkConsistency( idhdl sringHdl, idhdl dringHdl, int * vperm );
WalkState fractalWalkConsistency( idhdl sringHdl, idhdl dringHdl, int * vperm );
#endif

