#ifndef SPSPOLYLOOP_H
#define SPSPOLYLOOP_H

#include "structs.h"
#include "ring.h"
#include "kutil.h"

/*
 * assume monom = L(monom)
 * pNext(m) = result = p2-p1*m
 * do not destroy p1, but p2
 */
void spSpolyLoop_General (poly a1, poly a2, poly monom, poly spNoether);
typedef void (*spSpolyLoopProc)(poly p1, poly p2, poly m, poly spNoether);


spSpolyLoopProc spGetSpolyLoop(ring r, rOrderType_t ot, BOOLEAN homog);

inline spSpolyLoopProc spGetSpolyLoop(ring r, int modrank, int syzComp,
                                      BOOLEAN homog)
{
  return spGetSpolyLoop(r, spGetOrderType(r, modrank, syzComp), homog);
}

inline spSpolyLoopProc spGetSpolyLoop(ring r, kStrategy strat)
{
  return spGetSpolyLoop(r, spGetOrderType(r, strat->ak, strat->syzComp),
                        ((strat->homog && strat->kModW==NULL &&
                          ! rHasSimpleLexOrder(r))));
}

inline spSpolyLoopProc spGetSpolyLoop(ring r, kStrategy strat, int syzComp)
{
  return spGetSpolyLoop(r, spGetOrderType(r, strat->ak, syzComp),
                        ((strat->homog && strat->kModW==NULL &&
                          ! rHasSimpleLexOrder(r))));
}


inline spSpolyLoopProc spGetSpolyLoop(ring r)
{
  return spGetSpolyLoop(r, rGetOrderType(r), FALSE);
}

#endif // SPSPOLYLOOP_H
