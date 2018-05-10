#include "shiftop.h"

#define SHIFT_MULT_DEBUG

#ifdef SHIFT_MULT_DEBUG
#include "../kernel/polys.h"
#endif

poly shift_pp_Mult_mm(poly p, const poly m, const ring ri)
{
#ifdef SHIFT_MULT_DEBUG
  Print("shift_pp_Mult_mm p=%s m=%s", pString(p), pString(m));
  PrintLn();
#endif

  // we just shift m and then call the commutative multiplication
  int lV = ri->isLPring;
  int uptodeg = ri->N/lV;
  int shift = p_LastVblock(p, lV, ri);
  p_mLPshift(m, shift, uptodeg, lV, ri);

  // this is the commutative multiplication procedure
  extern poly pp_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(spolyrec*, spolyrec*, ip_sring*);
  p = pp_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(p, m, ri);
  return p_Shrink(p, lV, ri);
}

// destroys p
poly shift_p_Mult_mm(poly p, const poly m, const ring ri)
{
#ifdef SHIFT_MULT_DEBUG
  Print("shift_p_Mult_mm p=%s m=%s", pString(p), pString(m));
  PrintLn();
#endif

  // we just shift m and then call the commutative multiplication
  int lV = ri->isLPring;
  int uptodeg = ri->N/lV;
  int shift = p_LastVblock(p, lV, ri);
  p_mLPshift(m, shift, uptodeg, lV, ri);

  // this is the commutative multiplication procedure
  extern poly p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(spolyrec*, spolyrec*, ip_sring*);

  p = p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(p, m, ri);
  poly q = p_Shrink(p, lV, ri);
  p_Delete(&p, ri);
  return q;
}

poly shift_pp_mm_Mult(poly p, const poly m, const ring ri)
{
#ifdef SHIFT_MULT_DEBUG
  Print("shift_pp_mm_Mult p=%s m=%s", pString(p), pString(m));
  PrintLn();
#endif

  // we just shift m and then call the commutative multiplication
  int lV = ri->isLPring;
  int uptodeg = ri->N/lV;
  int shift = p_mLastVblock(m, lV, ri);
  p_LPshift(p, shift, uptodeg, lV, ri);

  // this is the commutative multiplication procedure
  extern poly pp_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(spolyrec*, spolyrec*, ip_sring*);
  p = pp_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(p, m, ri);
  return p_Shrink(p, lV, ri);
}

// destroys p
poly shift_p_mm_Mult(poly p, const poly m, const ring ri)
{
#ifdef SHIFT_MULT_DEBUG
  Print("shift_p_mm_Mult p=%s m=%s", pString(p), pString(m));
  PrintLn();
#endif

  // we just shift m and then call the commutative multiplication
  int lV = ri->isLPring;
  int uptodeg = ri->N/lV;
  int shift = p_mLastVblock(m, lV, ri);
  p_LPshift(p, shift, uptodeg, lV, ri);

  // this is the commutative multiplication procedure
  extern poly p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(spolyrec*, spolyrec*, ip_sring*);

  p = p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(p, m, ri);
  poly q = p_Shrink(p, lV, ri);
  p_Delete(&p, ri);
  return q;
}

// p - m*q destroys p
poly shift_p_Minus_mm_Mult_qq(poly p, poly m, poly q, int& Shorter, const poly spNoether, const ring ri) {

#ifdef SHIFT_MULT_DEBUG
  Print("shift_p_Minus_mm_Mult_qq p=%s m=%s q=%s spNoether=%s", pString(p), pString(m), pString(q));
  PrintLn();
#endif

  // we just shift m and then call the commutative multiplication
  int lV = ri->isLPring;
  int uptodeg = ri->N/lV;
  int shift = p_mLastVblock(m, lV, ri);
  p_LPshift(q, shift, uptodeg, lV, ri);

  // this is the commutative multiplication procedure
  extern poly p_Minus_mm_Mult_qq__FieldGeneral_LengthGeneral_OrdGeneral(spolyrec*, spolyrec*, spolyrec*, int&, spolyrec*, ip_sring*);

  p = p_Minus_mm_Mult_qq__FieldGeneral_LengthGeneral_OrdGeneral(p, m, q, Shorter, spNoether, ri);
  poly qq = p_Shrink(p, lV, ri);
  p_Delete(&p, ri);
  return qq;
}
