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
  void InitSDMultiplication( ring r );

#if 0 //The following is currently not working/used

  poly pp_Mult_mm__T
    (poly p, const poly m, const ring ri, poly &last, int lV);
  poly pp_Mult_mm_Noether__T
    ( poly p, const poly m, const poly spNoether, 
      int &ll, const ring ri, poly &last, int lV  );
  poly p_Minus_mm_Mult_qq__T
    ( poly p, poly m, poly q, int& Shorter, 
      const poly spNoether, const ring r, poly &last, int lV );

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
