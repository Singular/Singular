#include "shiftop.h"
#include "templates/p_MemCopy.h"
#include "monomials/p_polys.h"

/* #define SHIFT_MULT_DEBUG */

/* enable compat mode until the user interface is updated to support xy instead of x(1)*y(2)
 * NOTE: it already works, but all tests and the libraries need to be updated first
 * -> wait until the new interface is released
*/
#define SHIFT_MULT_COMPAT_MODE 

#ifdef SHIFT_MULT_DEBUG
#include "../kernel/polys.h"
#endif

poly shift_pp_Mult_mm(poly p, const poly m, const ring ri)
{
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_pp_Mult_mm: ("); p_wrp(p, ri, ri); PrintS(") * "); p_wrp(m, ri, ri);
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
  p_mLPunshift(_m, ri);
  p = p_Copy(p, ri);
  poly pCopyHead = p; // used to delete p later
  p_LPunshift(p, ri);
#else
  assume(p_mFirstVblock(_m, ri) <= 1);
  assume(p_FirstVblock(p, ri) <= 1); // TODO check that each block is <=1
#endif
  // at this point _m and p are shifted to 1

  spolyrec rp;
  poly q = &rp; // we use p for iterating and q for the result
  number mCoeff = pGetCoeff(_m);
  omBin bin = ri->PolyBin;
  pAssume(!n_IsZero(mCoeff, ri->cf));
  pAssume1(p_GetComp(m, ri) == 0 || p_MaxComp(p, ri) == 0);

  int *mExpV = (int *) omAlloc0((ri->N+1)*sizeof(int));
  p_GetExpV(_m,mExpV,ri);
  int mLength = p_mLastVblock(_m, mExpV, ri) * lV;
  do
  {
    p_AllocBin(pNext(q), bin, ri);
    pIter(q);
    pSetCoeff0(q, n_Mult(mCoeff, pGetCoeff(p), ri->cf));

    int *pExpV = (int *) omAlloc0((ri->N+1)*sizeof(int));
    p_GetExpV(p, pExpV, ri);
    p_LPExpVappend(pExpV, mExpV, p_mLastVblock(p, pExpV, ri) * lV, mLength, ri);
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
  PrintLn(); PrintS("shift_p_Mult_mm: ("); p_wrp(p, ri, ri); PrintS(") * "); p_wrp(m, ri, ri);
#endif

  p_Test(p, ri);
  p_LmTest(m, ri);
  pAssume(m != NULL);
  assume(p!=NULL);

  int lV = ri->isLPring;
  poly _m = m; // temp hack because m is const
#ifdef SHIFT_MULT_COMPAT_MODE
  _m = p_Copy(_m, ri);
  p_mLPunshift(_m, ri);
  p_LPunshift(p, ri);
#else
  assume(p_mFirstVblock(_m, ri) <= 1);
  assume(p_FirstVblock(p, ri) <= 1); // TODO check that each block is <=1
#endif
  // at this point _m and p are shifted to 1

  poly q = p; // we use p for iterating and q for the result
  number mCoeff = pGetCoeff(_m);
  number pCoeff;
  pAssume(!n_IsZero(mCoeff, ri->cf));

  int *mExpV = (int *) omAlloc0((ri->N+1)*sizeof(int));
  p_GetExpV(_m,mExpV,ri);
  int mLength = p_mLastVblock(_m, mExpV, ri) * lV;
  while (p != NULL)
  {
    pCoeff = pGetCoeff(p);
    pSetCoeff0(p, n_Mult(mCoeff, pCoeff, ri->cf));
    n_Delete(&pCoeff, ri->cf); // delete the old coeff

    int *pExpV = (int *) omAlloc0((ri->N+1)*sizeof(int));
    p_GetExpV(p,pExpV,ri);
    p_LPExpVappend(pExpV, mExpV, p_mLastVblock(p, pExpV, ri) * lV, mLength, ri);
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
  PrintLn(); PrintS("shift_pp_mm_Mult: "); p_wrp(m, ri, ri); PrintS(" * ("); p_wrp(p, ri, ri); PrintS(")");
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
  p_mLPunshift(_m, ri);
  p = p_Copy(p, ri);
  poly pCopyHead = p; // used to delete p later
  p_LPunshift(p, ri);
#else
  assume(p_mFirstVblock(_m, ri) <= 1);
  assume(p_FirstVblock(p, ri) <= 1); // TODO check that each block is <=1
#endif
  // at this point _m and p are shifted to 1

  spolyrec rp;
  poly q = &rp; // we use p for iterating and q for the result
  number mCoeff = pGetCoeff(_m);
  omBin bin = ri->PolyBin;
  pAssume(!n_IsZero(mCoeff, ri->cf));
  pAssume1(p_GetComp(m, ri) == 0 || p_MaxComp(p, ri) == 0);

  int *mExpV = (int *) omAlloc0((ri->N+1)*sizeof(int));
  p_GetExpV(_m,mExpV,ri);
  int mLength = p_mLastVblock(_m, mExpV, ri) * lV;
  do
  {
    p_AllocBin(pNext(q), bin, ri);
    pIter(q);
    pSetCoeff0(q, n_Mult(mCoeff, pGetCoeff(p), ri->cf));

    int *pExpV = (int *) omAlloc0((ri->N+1)*sizeof(int));
    p_GetExpV(p, pExpV, ri);
    p_LPExpVprepend(pExpV, mExpV, p_mLastVblock(p, pExpV, ri) * lV, mLength, ri);
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
  PrintLn(); PrintS("shift_pp_mm_Mult result: "); p_wrp(pNext(&rp), ri, ri); PrintLn();
#endif
  p_Test(pNext(&rp), ri);
  return pNext(&rp);
}

// destroys p
poly shift_p_mm_Mult(poly p, const poly m, const ring ri)
{
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_mm_Mult: "); p_wrp(m, ri, ri); PrintS(" * ("); p_wrp(p, ri, ri); PrintS(")");
#endif

  p_Test(p, ri);
  p_LmTest(m, ri);
  pAssume(m != NULL);
  assume(p!=NULL);

  int lV = ri->isLPring;
  poly _m = m; // temp hack because m is const
#ifdef SHIFT_MULT_COMPAT_MODE
  _m = p_Copy(_m, ri);
  p_mLPunshift(_m, ri);
  p_LPunshift(p, ri);
#else
  assume(p_mFirstVblock(_m, ri) <= 1);
  assume(p_FirstVblock(p, ri) <= 1); // TODO check that each block is <=1
#endif
  // at this point _m and p are shifted to 1

  poly q = p; // we use p for iterating and q for the result
  number mCoeff = pGetCoeff(_m);
  number pCoeff;
  pAssume(!n_IsZero(mCoeff, ri->cf));

  int *mExpV = (int *) omAlloc0((ri->N+1)*sizeof(int));
  p_GetExpV(_m,mExpV,ri);
  int mLength = p_mLastVblock(_m, mExpV, ri) * lV;
  while (p != NULL)
  {
    pCoeff = pGetCoeff(p);
    pSetCoeff0(p, n_Mult(mCoeff, pCoeff, ri->cf));
    n_Delete(&pCoeff, ri->cf); // delete the old coeff

    int *pExpV = (int *) omAlloc0((ri->N+1)*sizeof(int));
    p_GetExpV(p,pExpV,ri);
    p_LPExpVprepend(pExpV, mExpV, p_mLastVblock(p, pExpV, ri) * lV, mLength, ri);
    p_SetExpV(p, pExpV, ri);
    omFreeSize((ADDRESS) pExpV, (ri->N+1)*sizeof(int));

    pIter(p);
  }
  omFreeSize((ADDRESS) mExpV, (ri->N+1)*sizeof(int));
#ifdef SHIFT_MULT_COMPAT_MODE
  p_Delete(&_m, ri); // in this case we copied _m before
#endif
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_mm_Mult result: "); p_wrp(q, ri, ri); PrintLn();
#endif
  p_Test(q, ri);
  return q;
}

// p - m*q destroys p
poly shift_p_Minus_mm_Mult_qq(poly p, poly m, poly q, int& Shorter, const poly spNoether, const ring ri) {
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("shift_p_Minus_mm_Mult_qq: "); p_wrp(p, ri, ri); PrintS(" - "); p_wrp(m, ri, ri); PrintS(" * "); p_wrp(q, ri, ri);
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
  PrintLn(); WarnS("pp_Mult_mm_Noether is not supported yet by Letterplace. Ignoring spNoether and using pp_Mult_mm. This might lead to unexpected behavior.");

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
void p_mLPunshift(poly m, const ring ri)
{
  if (m == NULL || p_LmIsConstantComp(m,ri)) return;

  int lV = ri->isLPring;

  int shift = p_mFirstVblock(m, ri) - 1;

  if (shift == 0) return;

  int *e=(int *)omAlloc0((ri->N+1)*sizeof(int));
  int *s=(int *)omAlloc0((ri->N+1)*sizeof(int));
  p_GetExpV(m, e, ri);

  int expVoffset = shift*lV;
  for (int i = 1 + expVoffset; i <= ri->N; i++)
  {
    assume(e[i] <= 1);
    s[i - expVoffset] = e[i];
  }
  p_SetExpV(m,s,ri);
  omFreeSize((ADDRESS) e, (ri->N+1)*sizeof(int));
  omFreeSize((ADDRESS) s, (ri->N+1)*sizeof(int));
}

// unshifts the polynomial p, note: the ordering can be destroyed if the shifts for the monomials are not equal
void p_LPunshift(poly p, const ring ri)
{
  while (p!=NULL)
  {
    p_mLPunshift(p, ri);
    pIter(p);
  }
}

void p_mLPshift(poly m, int sh, const ring ri)
{
  if (sh == 0 || m == NULL || p_LmIsConstantComp(m,ri)) return;

  int lV = ri->isLPring;

  assume(p_mFirstVblock(m,ri) + sh >= 1);
  assume(p_mLastVblock(m,ri) + sh <= ri->N/lV);

  int *e=(int *)omAlloc0((ri->N+1)*sizeof(int));
  int *s=(int *)omAlloc0((ri->N+1)*sizeof(int));
  p_GetExpV(m,e,ri);

  for (int i = 1; i <= ri->N; i++)
  {
    assume(e[i]<=1);
    if (e[i]==1)
    {
      assume(i + (sh*lV) <= r->N);
      assume(i + (sh*lV) >= 1);
      s[i + (sh*lV)] = e[i]; /* actually 1 */
    }
  }
  p_SetExpV(m,s,ri);
  omFreeSize((ADDRESS) e, (ri->N+1)*sizeof(int));
  omFreeSize((ADDRESS) s, (ri->N+1)*sizeof(int));
}

void p_LPshift(poly p, int sh, const ring ri)
{
  if (sh == 0) return;

  while (p!=NULL)
  {
    p_mLPshift(p, sh, ri);
    pIter(p);
  }
}

/* returns the number of maximal block */
/* appearing among the monomials of p */
/* the 0th block is the 1st one */
int p_LastVblock(poly p, const ring r)
{
  poly q = p;
  int ans = 0;
  while (q!=NULL)
  {
    int ansnew = p_mLastVblock(q, r);
    ans    = si_max(ans,ansnew);
    pIter(q);
  }
  return(ans);
}

/* for a monomial p, returns the number of the last block */
/* where a nonzero exponent is sitting */
int p_mLastVblock(poly p, const ring ri)
{
  if (p == NULL || p_LmIsConstantComp(p,ri))
  {
    return(0);
  }

  int *e=(int *)omAlloc0((ri->N+1)*sizeof(int));
  p_GetExpV(p,e,ri);
  int b = p_mLastVblock(p, e, ri);
  omFreeSize((ADDRESS) e, (ri->N+1)*sizeof(int));
  return b;
}

/* for a monomial p with exponent vector expV, returns the number of the last block */
/* where a nonzero exponent is sitting */
int p_mLastVblock(poly p, int *expV, const ring ri)
{
  if (p == NULL || p_LmIsConstantComp(p,ri))
  {
    return(0);
  }

  int lV = ri->isLPring;
  int j,b;
  j = ri->N;
  while ( (!expV[j]) && (j>=1) ) j--;
  assume(j>0);
  b = (int)((j+lV-1)/lV); /* the number of the block, >=1 */
  return b;
}

/* returns the number of maximal block */
/* appearing among the monomials of p */
/* the 0th block is the 1st one */
int p_FirstVblock(poly p, const ring r)
{
  if (p == NULL) {
    return 0;
  }

  poly q = p;
  int ans = p_mFirstVblock(q, r);
  while (q!=NULL)
  {
    int ansnew = p_mFirstVblock(q, r);
    if (ansnew > 0) { // don't count constants
      ans = si_min(ans,ansnew);
    }
    pIter(q);
  }
  /* do not need to delete q */
  return(ans);
}

/* for a monomial p, returns the number of the first block */
/* where a nonzero exponent is sitting */
int p_mFirstVblock(poly p, const ring ri)
{
  if (p == NULL || p_LmIsConstantComp(p,ri))
  {
    return(0);
  }

  int *e=(int *)omAlloc0((ri->N+1)*sizeof(int));
  p_GetExpV(p,e,ri);
  int b = p_mFirstVblock(p, e, ri);
  omFreeSize((ADDRESS) e, (ri->N+1)*sizeof(int));
  return b;
}

/* for a monomial p with exponent vector expV, returns the number of the first block */
/* where a nonzero exponent is sitting */
int p_mFirstVblock(poly p, int *expV, const ring ri)
{
  if (p == NULL || p_LmIsConstantComp(p,ri))
  {
    return(0);
  }

  int lV = ri->isLPring;
  int j,b;
  j = 1;
  while ( (!expV[j]) && (j<=ri->N-1) ) j++;
  assume(j <= r->N);
  b = (int)(j+lV-1)/lV; /* the number of the block, 1<= b <= r->N  */
  return b;
}

// appends m2ExpV to m1ExpV, also adds their components (one of them is always zero)
void p_LPExpVappend(int *m1ExpV, int *m2ExpV, int m1Length, int m2Length, const ring ri) {
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("Append");
  PrintLn(); WriteLPExpV(m1ExpV, ri);
  PrintLn(); WriteLPExpV(m2ExpV, ri);
#endif
  assume(m1Length + m2Length <= ri->N); // always throw an error?
  for (int i = 1 + m1Length; i < 1 + m1Length + m2Length; ++i)
  {
    assume(m2ExpV[i - m1Length] <= 1);
    m1ExpV[i] = m2ExpV[i - m1Length];
  }

  assume(m1ExpV[0] == 0 || m2ExpV[0] == 0); // one component should be zero (otherwise this doesn't make any sense)
  m1ExpV[0] += m2ExpV[0]; // as in the commutative variant (they use MemAdd)
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); WriteLPExpV(m1ExpV, ri);
#endif
}

// prepends m2ExpV to m1ExpV, also adds their components (one of them is always zero)
void p_LPExpVprepend(int *m1ExpV, int *m2ExpV, int m1Length, int m2Length, const ring ri) {
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("Prepend");
  PrintLn(); WriteLPExpV(m1ExpV, ri);
  PrintLn(); WriteLPExpV(m2ExpV, ri);
#endif
  assume(m1Length + m2Length <= ri->N); // always throw an error?

  // shift m1 by m2Length
  for (int i = m2Length + m1Length; i >= 1 + m2Length; --i)
  {
    m1ExpV[i] = m1ExpV[i - m2Length];
  }

  // write m2 to m1
  for (int i = 1; i < 1 + m2Length; ++i)
  {
    assume(m2ExpV[i] <= 1);
    m1ExpV[i] = m2ExpV[i];
  }

  assume(m1ExpV[0] == 0 || m2ExpV[0] == 0); // one component should be zero (otherwise this doesn't make any sense)
  m1ExpV[0] += m2ExpV[0]; // as in the commutative variant (they use MemAdd)
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); WriteLPExpV(m1ExpV, ri);
#endif
}

void WriteLPExpV(int *expV, ring ri) {
  for (int i = 0; i <= ri->N; ++i) {
    Print("%d", expV[i]);
    if (i == 0) {
      Print("| ");
    }
    if (i % ri->isLPring == 0) {
      Print(" ");
    }
  }
}
