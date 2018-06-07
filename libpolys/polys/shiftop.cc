#include "shiftop.h"
#include "templates/p_MemCopy.h"

/* #define SHIFT_MULT_DEBUG */
#define SHIFT_MULT_WARNINGS
#define SHIFT_MULT_COMPAT_MODE

#ifdef SHIFT_MULT_DEBUG
#include "../kernel/polys.h"
#endif

poly shift_pp_Mult_mm(poly p, const poly m, const ring ri)
{
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_pp_Mult_mm: "); p_wrp(p, ri, ri); PrintS(" * "); p_wrp(m, ri, ri);
#endif

  p_Test(p, ri);
  p_LmTest(m, ri);
  if (p == NULL)
  {
    return NULL;
  }

  int lV = ri->isLPring;
  poly _m = m; // temp hack because m is const
#ifdef SHIFT_MULT_COMPAT_MODE
  _m = p_Copy(_m, ri);
  p_mLPUnShift(_m, ri);
  p = p_Copy(p, ri);
  poly pCopyHead = p; // used to delete p later
  p_LPUnShift(p, ri);
#else
  assume(p_mFirstVblock(_m, lV, ri) <= 1);
  assume(p_FirstVblock(p, lV, ri) <= 1); // TODO check that each block is <=1
#endif
  // at this point _m and p are shifted to 1

  spolyrec rp;
  poly q = &rp; // we use p for iterating and q for the result
  number mCoeff = pGetCoeff(_m);
  omBin bin = ri->PolyBin;
  pAssume(!n_IsZero(mCoeff, ri->cf));
  pAssume1(p_GetComp(m, ri) == 0 || p_MaxComp(p, ri) == 0);

  int mLast = p_mLastVblock(_m, lV, ri);
  int *mExpV = (int *) omAlloc0((ri->N+1)*sizeof(int));
  p_GetExpV(_m,mExpV,ri);
  do
  {
    p_AllocBin(pNext(q), bin, ri);
    pIter(q);
    pSetCoeff0(q, n_Mult(mCoeff, pGetCoeff(p), ri->cf));

    int *pExpV = (int *) omAlloc0((ri->N+1)*sizeof(int));
    p_GetExpV(p, pExpV, ri);
    p_LPExpVappend(pExpV, mExpV, p_mLastVblock(p, lV, ri), mLast, ri);
    p_MemCopy_LengthGeneral(q->exp, p->exp, ri->ExpL_Size); // otherwise q is not initialized correctly
    p_SetExpV(q, pExpV, ri);
    omFreeSize((ADDRESS) pExpV, (ri->N+1)*sizeof(int));

    pIter(p);
  }
  while (p != NULL);
  omFreeSize((ADDRESS) mExpV, (ri->N+1)*sizeof(int));
  pNext(q) = NULL;
#ifdef SHIFT_MULT_COMPAT_MODE
  p_Delete(&_m, ri); // in this case we copied _m before
  p_Delete(&pCopyHead, ri); // in this case we copied p before
#endif
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_pp_Mult_mm result: "); p_wrp(pNext(&rp), ri, ri); PrintLn();
#endif
  p_Test(pNext(&rp), ri);
  return pNext(&rp);
}

// destroys p
poly shift_p_Mult_mm(poly p, const poly m, const ring ri)
{
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_Mult_mm: "); p_wrp(p, ri, ri); PrintS(" * "); p_wrp(m, ri, ri); PrintLn();
#endif

  p_Test(p, ri);
  p_LmTest(m, ri);
  pAssume(m != NULL);
  assume(p!=NULL);

  int lV = ri->isLPring;
  poly _m = m; // temp hack because m is const
#ifdef SHIFT_MULT_COMPAT_MODE
  _m = p_Copy(_m, ri);
  p_mLPUnShift(_m, ri);
  p_LPUnShift(p, ri);
#else
  assume(p_mFirstVblock(_m, lV, ri) <= 1);
  assume(p_FirstVblock(p, lV, ri) <= 1); // TODO check that each block is <=1
#endif
  // at this point _m and p are shifted to 1

  poly q = p; // we use p for iterating and q for the result
  number mCoeff = pGetCoeff(_m);
  number pCoeff;
  pAssume(!n_IsZero(mCoeff, ri->cf));

  int mLast = p_mLastVblock(_m, lV, ri);
  int *mExpV = (int *) omAlloc0((ri->N+1)*sizeof(int));
  p_GetExpV(_m,mExpV,ri);
  while (p != NULL)
  {
    pCoeff = pGetCoeff(p);
    pSetCoeff0(p, n_Mult(mCoeff, pCoeff, ri->cf));
    n_Delete(&pCoeff, ri->cf); // delete the old coeff

    int *pExpV = (int *) omAlloc0((ri->N+1)*sizeof(int));
    p_GetExpV(p,pExpV,ri);
    p_LPExpVappend(pExpV, mExpV, p_mLastVblock(p, lV, ri), mLast, ri);
    p_SetExpV(p, pExpV, ri);
    omFreeSize((ADDRESS) pExpV, (ri->N+1)*sizeof(int));

    pIter(p);
  }
  omFreeSize((ADDRESS) mExpV, (ri->N+1)*sizeof(int));
#ifdef SHIFT_MULT_COMPAT_MODE
  p_Delete(&_m, ri); // in this case we copied _m before
#endif
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_Mult_mm result: "); p_wrp(q, ri, ri); PrintLn();
#endif
  p_Test(q, ri);
  return q;
}

poly shift_pp_mm_Mult(poly p, const poly m, const ring ri)
{
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_pp_mm_Mult: "); p_wrp(m, ri, ri); PrintS(" * "); p_wrp(p, ri, ri); PrintLn();
#endif

  int lV = ri->isLPring;

  // we just shift m and then call the commutative multiplication
  int uptodeg = ri->N/lV;
  int mshift = p_mLastVblock(m, lV, ri);
  int pshift = p_FirstVblock(p, lV, ri) - 1;

  assume(p_FirstVblock(p, lV, ri) <= 1);
  assume(p_mFirstVblock(m, lV, ri) <= 1);
#ifdef SHIFT_MULT_WARNINGS
  // only needed because our examples still use x(1)*y(2) etc on the interface level
  if (pshift > 0) {
    PrintLn(); WarnS("p is already shifted");
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
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_mm_Mult: "); p_wrp(m, ri, ri); PrintS(" * "); p_wrp(p, ri, ri); PrintLn();
#endif

  int lV = ri->isLPring;

  // we just shift m and then call the commutative multiplication
  int uptodeg = ri->N/lV;
  int mshift = p_mLastVblock(m, lV, ri);
  int pshift = p_FirstVblock(p, lV, ri) - 1;

  assume(p_FirstVblock(p, lV, ri) <= 1);
  assume(p_mFirstVblock(m, lV, ri) <= 1);
#ifdef SHIFT_MULT_WARNINGS
  // only needed because our examples still use x(1)*y(2) etc on the interface level
  if (pshift > 0) {
    PrintLn(); WarnS("p is already shifted");
  }
#endif
  p_LPshift(p, mshift - pshift, uptodeg, lV, ri);

  // this is the commutative multiplication procedure
  extern poly p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(spolyrec*, spolyrec*, ip_sring*);

  p = p_Mult_mm__FieldGeneral_LengthGeneral_OrdGeneral(p, m, ri);
  poly q = p_Shrink(p, lV, ri);
  p_Delete(&p, ri);
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_mm_Mult result: "); p_wrp(q, ri, ri); PrintLn();
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

#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_Minus_mm_Mult_qq result: "); p_wrp(qq, ri, ri); PrintLn();
#endif
  Shorter -= pLength(qq);
  return qq;
}

// Unsupported Operation STUBs
poly shift_pp_Mult_mm_Noether_STUB(poly p, const poly m, const poly spNoether, int &ll, const ring ri) {
  PrintLn(); WarnS("pp_Mult_mm_Noether is not supported yet by Letterplace. Ignoring spNoether. This might lead to unexpected behavior.");
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_pp_Mult_mm_Noether: "); p_wrp(m, ri, ri); PrintS(" * "); p_wrp(p, ri, ri); PrintLn();
#endif

  int pLen = 0;
  if (ll >= 0) {
    pLen = pLength(p);
  }

  p = shift_pp_Mult_mm(p, m, ri);

  if (ll >= 0) {
    ll = pLen - pLength(p);
  } else {
    ll = pLength(p);
  }

  return p;
}


poly shift_pp_Mult_Coeff_mm_DivSelectMult_STUB(poly p,const poly m, const poly a, const poly b, int &shorter,const ring r) {
  PrintLn(); WarnS("pp_Mult_Coeff_mm_DivSelectMult is not supported yet by Letterplace. This might lead to unexpected behavior.");
  return NULL;
}

poly shift_pp_Mult_Coeff_mm_DivSelect_STUB(poly p, const poly m, int &shorter, const ring r) {
  PrintLn(); WarnS("pp_Mult_Coeff_mm_DivSelect is not supported yet by Letterplace. This might lead to unexpected behavior.");
  return NULL;
}

// auxiliary

// unshifts the monomial m
void p_mLPUnShift(poly m, const ring ri)
{
  if (m == NULL || p_LmIsConstant(m,ri)) return;

  int lV = ri->isLPring;

  int shift = p_mFirstVblock(m, lV, ri) - 1;

  if (shift == 0) return;

  int L = p_mLastVblock(m, lV, ri);

  int *e=(int *)omAlloc0((ri->N+1)*sizeof(int));
  int *s=(int *)omAlloc0((ri->N+1)*sizeof(int));
  p_GetExpV(m, e, ri);

  int expVoffset = shift*lV;
  for (int i = 1 + expVoffset; i <= L*lV; i++)
  {
    assume(e[i] <= 1);
    s[i - expVoffset] = e[i];
  }
  p_SetExpV(m,s,ri);
  omFreeSize((ADDRESS) e, (ri->N+1)*sizeof(int));
  omFreeSize((ADDRESS) s, (ri->N+1)*sizeof(int));
}

// unshifts the polynomial p, note: the ordering can be destroyed if the shifts for the monomials are not equal
void p_LPUnShift(poly p, const ring ri)
{
  while (p!=NULL)
  {
    p_mLPUnShift(p, ri);
    pIter(p);
  }
  p_Test(p, ri); // check if ordering was destroyed
}

// appends mExpV to pExpV where pLast is the last Vblock of p and mLast the last Vblock of m
void p_LPExpVappend(int *pExpV, int *mExpV, int pLast, int mLast, const ring ri) {
#ifdef SHIFT_MULT_DEBUG
  WriteLPExpV(pExpV, ri); PrintLn();
  WriteLPExpV(mExpV, ri); PrintLn();
#endif
  int appendOffset = pLast * ri->isLPring;
  int appendEnd = appendOffset + mLast * ri->isLPring;
  assume(appendOffset + appendEnd <= ri->N);
  for (int i = 1 + appendOffset; i < 1 + appendEnd; i++)
  {
    assume(mExpV[i - appendOffset] <= 1);
    pExpV[i] = mExpV[i - appendOffset];
  }
#ifdef SHIFT_MULT_DEBUG
  WriteLPExpV(pExpV, ri); PrintLn();
#endif
}

void WriteLPExpV(int *expV, ring ri) {
  for (int i = 0; i <= ri->N; i++) {
    Print("%d", expV[i]);
    if (i == 0) {
      Print("| ");
    }
    if (i % ri->isLPring == 0) {
      Print(" ");
    }
  }
}
