#ifndef SPSPOLYLOOP_H
#define SPSPOLYLOOP_H

// throw this out, as soon as we got rid off COMP_FAST
#include "polys-impl.h"
typedef void (*spSpolyLoopProc)(poly p1, poly p2, poly m, poly spNoether);

#ifdef COMP_FAST
#include "structs.h"
#include "ring.h"
#include "kutil.h"

/*
 * assume m = L(m)
 * pNext(m) = result = a2-a1*m
 * do not destroy a1, but a2
 */

spSpolyLoopProc spGetSpolyLoop(ring r, rOrderType_t ot, BOOLEAN homog);
void spSpolyLoop_General (poly a1, poly a2, poly monom, poly spNoether);

inline spSpolyLoopProc spGetSpolyLoop(ring r, int modrank, int syzComp, 
                                      BOOLEAN homog)
{
  return spGetSpolyLoop(r, spGetOrderType(r, modrank, syzComp), homog);
}

inline spSpolyLoopProc spGetSpolyLoop(ring r, kStrategy strat)
{
  return spGetSpolyLoop(r, spGetOrderType(r, strat), 
                        (strat->homog && strat->ak == 0));
}

inline spSpolyLoopProc spGetSpolyLoop(ring r)
{
  return spGetSpolyLoop(r, rGetOrderType(r), FALSE);
}

#endif


#endif // SPSPOLYLOOP_H
