#ifndef SDMULTIPLICATION_H
#define SDMULTIPLICATION_H

#include <kernel/kutil.h> //INCLUDE Magic

namespace ShiftDVec
{
  void p_ExpSum_slow
    (poly rt, poly p, poly q, ring r);
  void p_ExpSum_dp
    (poly rt, poly p, poly q, ring r);
  //void (*p_ExpSum) (poly rt, poly p, poly q, ring r);

  int InitOrderMapping( ring r );
  void InitSDMultiplication( ring r, kStrategy strat );
 
  int ksReducePoly(LObject* PR,
                 TObject* UPW,
                 TObject* SPW,
                 poly spNoether,
                 number *coef,
                 kStrategy strat)
  void ksCreateSpoly(LObject* Pair,   poly spNoether,
                   int use_buckets, ring tailRing,
                   poly m1, poly m2, TObject** R)

#if 0 //The following is currently not working/used
  void ksCreateSpoly
    ( LObject* Pair, poly spNoether, int use_buckets, 
      ring tailRing, poly m1, poly m2,  TObject** R, int lV );
  int ksReducePoly
    ( LObject* PR, TObject* PW, 
      int lV, poly spNoether = NULL, 
      number *coef = NULL, kStrategy strat = NULL );
  int ksReducePolyTail
    ( LObject* PR, TObject* PW, 
      poly Current, poly spNoether, int lV );
  int ksReducePolyTail
    ( LObject* PR, TObject* PW, LObject* Red, int lV );
  poly ksCreateShortSpoly( poly p1, poly p2, ring tailRing );
#endif
}
#endif
