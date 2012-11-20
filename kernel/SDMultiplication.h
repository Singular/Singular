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
 
  extern int red_count;
  extern int create_count;

  int ksReducePoly
    ( LObject* PR, TObject* UPW, TObject* SPW,
      poly spNoether = NULL, 
      number *coef = NULL, kStrategy strat = NULL );

  void ksCreateSpoly
    ( LObject* Pair, poly spNoether, int use_buckets, 
      ring tailRing, poly m1, poly m2, TObject** R    );

  int ksReducePolyTail
    ( LObject* PR, TObject* UPW, TObject* SPW, 
      poly Current = NULL, 
      poly spNoether = NULL, kStrategy strat = NULL );

  int ksReducePolyTail
    ( LObject* PR, TObject* UPW, TObject* SPW, LObject* Red );

  BOOLEAN kCheckSpolyCreation
    ( LObject *L, kStrategy strat, poly &m1, poly &m2 );

  BOOLEAN k_GetLeadTerms
    ( const poly p1, const poly p2, 
      const ring p_r, poly &m1, poly &m2, const ring m_r );
}
#endif
