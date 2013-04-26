#ifndef SDMULTIPLICATION_H
#define SDMULTIPLICATION_H

#include <kernel/kutil.h> //INCLUDE Magic

namespace ShiftDVec
{
  namespace SD = ShiftDVec;

  class sLObject;
  class sTObject;
  class skStrategy;
  typedef skStrategy* kStrategy;
  typedef sTObject TObject;
  typedef sLObject LObject;

  void p_ExpSum_slow
    (poly rt, poly p, poly q, ring r);
  void p_ExpSum_dp
    (poly rt, poly p, poly q, ring r);
  //void (*p_ExpSum) (poly rt, poly p, poly q, ring r);

  int get_shift_of(poly p, ring r);
  void get_division_cofactors
    ( poly p, poly divides_p, 
      int divides_p_shift, poly* ml, poly* mr, ring r );

  int InitOrderMapping( ring r );
  void InitSDMultiplication
    ( ring r, ShiftDVec::kStrategy strat );
 
  extern int red_count;
  extern int create_count;

  int ksReducePoly
    ( ShiftDVec::LObject* PR, ShiftDVec::TObject* UPW, ShiftDVec::TObject* SPW,
      poly spNoether = NULL, 
      number *coef = NULL, SD::kStrategy strat = NULL );

  void ksCreateSpoly
    ( ShiftDVec::LObject* Pair, poly spNoether, int use_buckets, 
      ring tailRing, poly m1, poly m2, ShiftDVec::TObject** R    );

  int ksReducePolyTail
    ( ShiftDVec::LObject* PR, ShiftDVec::TObject* UPW, ShiftDVec::TObject* SPW, 
      poly Current = NULL, 
      poly spNoether = NULL, ShiftDVec::kStrategy strat = NULL );

  int ksReducePolyTail
    ( ShiftDVec::LObject* PR, ShiftDVec::TObject* UPW, ShiftDVec::TObject* SPW, ShiftDVec::LObject* Red );

  BOOLEAN kCheckSpolyCreation
    ( ShiftDVec::LObject *L, ShiftDVec::kStrategy strat, poly &m1, poly &m2 );

  BOOLEAN k_GetLeadTerms
    ( const poly p1, const poly p2, 
      const ring p_r, poly &m1, poly &m2, const ring m_r );

  typedef skStrategy* kStrategy;

}
#endif
