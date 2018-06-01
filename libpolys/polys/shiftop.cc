#include "shiftop.h"

#define SHIFT_MULT_DEBUG

#ifdef SHIFT_MULT_DEBUG
#include "../kernel/polys.h"
#endif

poly shift_pp_Mult_mm(poly p, const poly m, const ring ri)
{
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_pp_Mult_mm: "); p_wrp(p, ri, ri); PrintS(" * "); p_wrp(m, ri, ri);
#endif

  int lV = ri->isLPring;
  if (p_mFirstVblock(m,lV,ri) < p_mFirstVblock(p,lV,ri)) {
    // TODO remove this
    PrintS("WARNING: auto switch to shift_pp_mm_Mult");
    return shift_pp_mm_Mult(p, m, ri);
  }

  // we just shift m and then call the commutative multiplication
  int uptodeg = ri->N/lV;
  int pshift = p_LastVblock(p, lV, ri);
  int mshift = p_mFirstVblock(m, lV, ri) - 1;
#ifdef SHIFT_MULT_DEBUG
  if (mshift > 0) {
    PrintLn();
    PrintS("WARNING: m is already shifted");
  }
#endif
  poly m2 = p_Copy(m,ri);
  p_mLPshift(m2, pshift - mshift, uptodeg, lV, ri);

  // this is the commutative multiplication procedure
  extern poly pp_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(spolyrec*, spolyrec*, ip_sring*);
  p = pp_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(p, m2, ri);
  p = p_Shrink(p, lV, ri);
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_pp_Mult_mm result: "); p_wrp(p, ri, ri); PrintLn();
#endif
  return p;
}

// destroys p
poly shift_p_Mult_mm(poly p, const poly m, const ring ri)
{
  // TODO remove this
  return shift_pp_Mult_mm(p,m,ri);
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_Mult_mm: "); p_wrp(p, ri, ri); PrintS(" * "); p_wrp(m, ri, ri); PrintLn();
#endif

  int lV = ri->isLPring;
  if (p_mFirstVblock(m,lV,ri) < p_mFirstVblock(p,lV,ri)) {
    // TODO remove this
    Print("WARNING: auto switch to shift_p_mm_Mult");
    return shift_p_mm_Mult(p, m, ri);
  }

  // we just shift m and then call the commutative multiplication
  int uptodeg = ri->N/lV;
  int pshift = p_LastVblock(p, lV, ri);
  int mshift = p_mFirstVblock(m, lV, ri) - 1;
#ifdef SHIFT_MULT_DEBUG
  if (mshift > 0) {
    PrintLn(); Print("WARNING: m is already shifted"); PrintLn();
  }
#endif
  poly m2 = p_Copy(m,ri);
  p_mLPshift(m2, pshift - mshift, uptodeg, lV, ri);

  // this is the commutative multiplication procedure
  extern poly p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(spolyrec*, spolyrec*, ip_sring*);

  p = p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(p, m2, ri);
  poly q = p_Shrink(p, lV, ri);
  p_Delete(&p, ri);
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_Mult_mm result: "); p_wrp(q, ri, ri); PrintLn();
#endif
  return q;
}

poly shift_pp_mm_Mult(poly p, const poly m, const ring ri)
{
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_pp_mm_Mult: "); p_wrp(m, ri, ri); PrintS(" * "); p_wrp(p, ri, ri); PrintLn();
#endif

  int lV = ri->isLPring;
  if (p_mFirstVblock(p,lV,ri) < p_mFirstVblock(m,lV,ri)) {
    // TODO remove this
    PrintS("WARNING: auto switch to shift_pp_Mult_mm");
    return shift_pp_Mult_mm(p, m, ri);
  }

  // we just shift m and then call the commutative multiplication
  int uptodeg = ri->N/lV;
  int mshift = p_mLastVblock(m, lV, ri);
  int pshift = p_FirstVblock(p, lV, ri) - 1;
#ifdef SHIFT_MULT_DEBUG
  if (pshift > 0) {
    PrintLn(); PrintS("WARNING: p is already shifted"); PrintLn();
  }
#endif
  p = p_Copy(p,ri);
  p_LPshift(p, mshift - pshift, uptodeg, lV, ri);

  // this is the commutative multiplication procedure
  extern poly pp_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(spolyrec*, spolyrec*, ip_sring*);
  p = pp_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(p, m, ri);

  p = p_Shrink(p, lV, ri);
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_pp_mm_Mult result: "); p_wrp(p, ri, ri); PrintLn();
#endif
  return p;
}

// destroys p
poly shift_p_mm_Mult(poly p, const poly m, const ring ri)
{
  return shift_pp_mm_Mult(p,m,ri);
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_mm_Mult: "); p_wrp(m, ri, ri); PrintS(" * "); p_wrp(p, ri, ri); PrintLn();
#endif

  int lV = ri->isLPring;
  if (p_mFirstVblock(p,lV,ri) < p_mFirstVblock(m,lV,ri)) {
    // TODO remove this
    Print("WARNING: auto switch to shift_p_Mult_mm");
    return shift_p_Mult_mm(p, m, ri);
  }

  // we just shift m and then call the commutative multiplication
  int uptodeg = ri->N/lV;
  int mshift = p_mLastVblock(m, lV, ri);
  int pshift = p_FirstVblock(p, lV, ri) - 1;
#ifdef SHIFT_MULT_DEBUG
  if (pshift > 0) {
    PrintLn(); PrintS("WARNING: p is already shifted"); PrintLn();
  }
#endif
  p_LPshift(p, mshift - pshift, uptodeg, lV, ri);

  // this is the commutative multiplication procedure
  extern poly p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(spolyrec*, spolyrec*, ip_sring*);

  p = p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(p, m, ri);
  poly q = p_Shrink(p, lV, ri);
  p_Delete(&p, ri);
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_mm_Mult resutl: "); p_wrp(q, ri, ri); PrintLn();
#endif
  return q;
}

// p - m*q destroys p
poly shift_p_Minus_mm_Mult_qq(poly p, poly m, poly q, int& Shorter, const poly spNoether, const ring ri) {

#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_Minus_mm_Mult_qq: "); p_wrp(p, ri, ri); PrintS(" - "); p_wrp(m, ri, ri); PrintS(" * "); p_wrp(q, ri, ri); PrintLn();
#endif

  Shorter = pLength(p) + pLength(q);

  poly qq = p_Add_q(p, shift_pp_mm_Mult(q, p_Neg(p_Copy(m, ri), ri), ri), ri);
  /* qq = p_Shrink(qq, lV, ri); // probably not neccessary */
  /* p_Delete(&p, ri); */ // just for testing disabled

#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_Minus_mm_Mult_qq result: "); p_wrp(qq, ri, ri); PrintLn();
#endif
  Shorter -= pLength(qq);
  return qq;
}
