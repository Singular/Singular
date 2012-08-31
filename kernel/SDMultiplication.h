namespace ShiftDVec
{
  int ShiftDVec::InitOrderMapping( ring r );
  void ShiftDVec::InitSDMultiplication( ring r );

  void ShiftDVec::p_ExpSum_slow
    (poly rt, poly p, poly q, ring r, int lV);
  void ShiftDVec::p_ExpSum_dp
    (poly rt, poly p, poly q, ring r, int lV);

  poly ShiftDVec::pp_Mult_mm__T
    (poly p, const poly m, const ring ri, poly &last, int lV);
  poly ShiftDVec::pp_Mult_mm_Noether__T
    ( poly p, const poly m, const poly spNoether, 
      int &ll, const ring ri, poly &last, int lV  );
  poly ShiftDVec::p_Minus_mm_Mult_qq__T
    ( poly p, poly m, poly q, int& Shorter, 
      const poly spNoether, const ring r, poly &last, int lV );

  void ShiftDVec::ksCreateSpoly
    ( LObject* Pair, poly spNoether, int use_buckets, 
      ring tailRing, poly m1, poly m2,  TObject** R, int lV );
  int ShiftDVec::ksReducePoly
    ( LObject* PR, TObject* PW, 
      poly spNoether, number *coef, kStrategy strat );
  int ShiftDVec::ksReducePolyTail
    ( LObject* PR, TObject* PW, 
      poly Current, poly spNoether, int lV );
  int ShiftDVec::ksReducePolyTail
    ( LObject* PR, TObject* PW, LObject* Red, int lV );
  poly ShiftDVec::ksCreateShortSpoly
    ( poly p1, poly p2, ring tailRing );
}
