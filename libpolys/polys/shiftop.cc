#include "shiftop.h"

#ifdef HAVE_SHIFTBBA

#include "templates/p_MemCopy.h"
#include "monomials/p_polys.h"
#include "polys/simpleideals.h"

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

  int *mExpV = (int *) omAlloc((ri->N+1)*sizeof(int));
  p_GetExpV(_m,mExpV,ri);
  int mLength = p_mLastVblock(_m, mExpV, ri) * lV;
  int *pExpV = (int *) omAlloc((ri->N+1)*sizeof(int));
  do
  {
    p_AllocBin(pNext(q), bin, ri);
    pIter(q);
    pNext(q)=NULL;
    pSetCoeff0(q, n_Mult(mCoeff, pGetCoeff(p), ri->cf));

    p_GetExpV(p, pExpV, ri);
    p_LPExpVappend(pExpV, mExpV, p_mLastVblock(p, pExpV, ri) * lV, mLength, ri);
    p_MemCopy_LengthGeneral(q->exp, p->exp, ri->ExpL_Size); // otherwise q is not initialized correctly
    p_SetExpV(q, pExpV, ri);

    pIter(p);
  }
  while (p != NULL);
  omFreeSize((ADDRESS) pExpV, (ri->N+1)*sizeof(int));
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

  int *mExpV = (int *) omAlloc((ri->N+1)*sizeof(int));
  p_GetExpV(_m,mExpV,ri);
  int mLength = p_mLastVblock(_m, mExpV, ri) * lV;
  int *pExpV = (int *) omAlloc((ri->N+1)*sizeof(int));
  while (p != NULL)
  {
    pCoeff = pGetCoeff(p);
    pSetCoeff0(p, n_Mult(mCoeff, pCoeff, ri->cf));
    n_Delete(&pCoeff, ri->cf); // delete the old coeff

    p_GetExpV(p,pExpV,ri);
    p_LPExpVappend(pExpV, mExpV, p_mLastVblock(p, pExpV, ri) * lV, mLength, ri);
    p_SetExpV(p, pExpV, ri);

    pIter(p);
  }
  omFreeSize((ADDRESS) pExpV, (ri->N+1)*sizeof(int));
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

  int *mExpV = (int *) omAlloc((ri->N+1)*sizeof(int));
  p_GetExpV(_m,mExpV,ri);
  int mLength = p_mLastVblock(_m, mExpV, ri) * lV;
  int *pExpV = (int *) omAlloc((ri->N+1)*sizeof(int));
  do
  {
    p_AllocBin(pNext(q), bin, ri);
    pIter(q);
    pNext(q)=NULL;
    pSetCoeff0(q, n_Mult(mCoeff, pGetCoeff(p), ri->cf));

    p_GetExpV(p, pExpV, ri);
    p_LPExpVprepend(pExpV, mExpV, p_mLastVblock(p, pExpV, ri) * lV, mLength, ri);
    p_MemCopy_LengthGeneral(q->exp, p->exp, ri->ExpL_Size); // otherwise q is not initialized correctly
    p_SetExpV(q, pExpV, ri);

    pIter(p);
  }
  while (p != NULL);
  omFreeSize((ADDRESS) pExpV, (ri->N+1)*sizeof(int));
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

  int *mExpV = (int *) omAlloc((ri->N+1)*sizeof(int));
  p_GetExpV(_m,mExpV,ri);
  int mLength = p_mLastVblock(_m, mExpV, ri) * lV;
  int *pExpV = (int *) omAlloc((ri->N+1)*sizeof(int));
  while (p != NULL)
  {
    pCoeff = pGetCoeff(p);
    pSetCoeff0(p, n_Mult(mCoeff, pCoeff, ri->cf));
    n_Delete(&pCoeff, ri->cf); // delete the old coeff

    p_GetExpV(p,pExpV,ri);
    p_LPExpVprepend(pExpV, mExpV, p_mLastVblock(p, pExpV, ri) * lV, mLength, ri);
    p_SetExpV(p, pExpV, ri);

    pIter(p);
  }
  omFreeSize((ADDRESS) pExpV, (ri->N+1)*sizeof(int));
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
  if (ll >= 0)
  {
    pLen = pLength(p);
  }

  p = shift_pp_Mult_mm(p, m, ri);

  if (ll >= 0)
  {
    ll = pLen - pLength(p);
  }
  else
  {
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

  int *e=(int *)omAlloc((ri->N+1)*sizeof(int));
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

  int *e=(int *)omAlloc((ri->N+1)*sizeof(int));
  int *s=(int *)omAlloc0((ri->N+1)*sizeof(int));
  p_GetExpV(m,e,ri);

  if (p_mLastVblock(m, e, ri) + sh > ri->N/lV)
  {
    Werror("degree bound of Letterplace ring is %d, but at least %d is needed for this shift", ri->N/lV, p_mLastVblock(m, e, ri) + sh);
  }
  for (int i = ri->N - sh*lV; i > 0; i--)
  {
    assume(e[i]<=1);
    if (e[i]==1)
    {
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

  int *e=(int *)omAlloc((ri->N+1)*sizeof(int));
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

  int *e=(int *)omAlloc((ri->N+1)*sizeof(int));
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
  assume(j <= ri->N);
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
  int last = m1Length + m2Length;
  if (last > ri->N)
  {
    Werror("degree bound of Letterplace ring is %d, but at least %d is needed for this multiplication", ri->N/ri->isLPring, last/ri->isLPring);
    last = ri->N;
  }
  for (int i = 1 + m1Length; i < 1 + last; ++i)
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
void p_LPExpVprepend(int *m1ExpV, int *m2ExpV, int m1Length, int m2Length, const ring ri)
{
#ifdef SHIFT_MULT_DEBUG
  PrintLn(); PrintS("Prepend");
  PrintLn(); WriteLPExpV(m1ExpV, ri);
  PrintLn(); WriteLPExpV(m2ExpV, ri);
#endif
  int last = m1Length + m2Length;
  if (last > ri->N)
  {
    Werror("degree bound of Letterplace ring is %d, but at least %d is needed for this multiplication", ri->N/ri->isLPring, last/ri->isLPring);
    last = ri->N;
  }

  // shift m1 by m2Length
  for (int i = last; i >= 1 + m2Length; --i)
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

void WriteLPExpV(int *expV, ring ri)
{
  char *s = LPExpVString(expV, ri);
  PrintS(s);
  omFree(s);
}

char* LPExpVString(int *expV, ring ri)
{
  StringSetS("");
  for (int i = 0; i <= ri->N; ++i)
  {
    StringAppend("%d", expV[i]);
    if (i == 0)
    {
      StringAppendS("| ");
    }
    if (i % ri->isLPring == 0 && i != ri->N)
    {
      StringAppendS(" ");
    }
  }
  return StringEndS();
}

// splits a frame (e.g. x(1)*y(5)) m1 into m1 and m2 (e.g. m1=x(1) and m2=y(1))
// according to p which is inside the frame
void k_SplitFrame(poly &m1, poly &m2, int at, const ring r)
{
  int lV = r->isLPring;

  number m1Coeff = pGetCoeff(m1);

  int hole = lV * at;
  m2 = p_GetExp_k_n(m1, 1, hole, r);
  m1 = p_GetExp_k_n(m1, hole, r->N, r);

  p_mLPunshift(m2, r);
  p_SetCoeff(m1, m1Coeff, r);

  assume(p_FirstVblock(m1,r) <= 1);
  assume(p_FirstVblock(m2,r) <= 1);
}

/* tests whether each polynomial of an ideal I lies in in V */
int id_IsInV(ideal I, const ring r)
{
  int i;
  int s    = IDELEMS(I)-1;
  for(i = 0; i <= s; i++)
  {
    if ( !p_IsInV(I->m[i], r) )
    {
      return(0);
    }
  }
  return(1);
}

/* tests whether the whole polynomial p in in V */
int p_IsInV(poly p, const ring r)
{
  poly q = p;
  while (q!=NULL)
  {
    if ( !p_mIsInV(q, r) )
    {
      return(0);
    }
    q = pNext(q);
  }
  return(1);
}

/* there should be two routines: */
/* 1. test place-squarefreeness: in homog this suffices: isInV */
/* 2. test the presence of a hole -> in the tail??? */

int p_mIsInV(poly p, const ring r)
{
  int lV = r->isLPring;
  /* investigate only the leading monomial of p in currRing */
  if ( p_Totaldegree(p, r)==0 ) return(1);
  /* returns 1 iff p is in V */
  /* that is in each block up to a certain one there is only one nonzero exponent */
  /* lV = the length of V = the number of orig vars */
  int *e = (int *)omAlloc((r->N+1)*sizeof(int));
  int  b = (int)((r->N+lV-1)/lV); /* the number of blocks */
  //int b  = (int)(currRing->N)/lV;
  int *B = (int *)omAlloc0((b+1)*sizeof(int)); /* the num of elements in a block */
  p_GetExpV(p,e,r);
  int i,j;
  for (j=1; j<=b; j++)
  {
    /* we go through all the vars */
    /* by blocks in lV vars */
    for (i=(j-1)*lV + 1; i<= j*lV; i++)
    {
      if (e[i]) B[j] = B[j]+1;
    }
  }
  //  j = b;
  //  while ( (!B[j]) && (j>=1)) j--;
  for (j=b; j>=1; j--)
  {
    if (B[j]!=0) break;
  }
  /* do not need e anymore */
  omFreeSize((ADDRESS) e, (r->N+1)*sizeof(int));

  if (j==0) goto ret_true;
//   {
//     /* it is a zero exp vector, which is in V */
//     freeT(B, b);
//     return(1);
//   }
  /* now B[j] != 0 and we test place-squarefreeness */
  for (; j>=1; j--)
  {
    if (B[j]!=1)
    {
      omFreeSize((ADDRESS) B, (b+1)*sizeof(int));
      return(0);
    }
  }
 ret_true:
  omFreeSize((ADDRESS) B, (b+1)*sizeof(int));
  return(1);
}

BOOLEAN p_LPDivisibleBy(poly a, poly b, const ring r)
{
  pIfThen1(b!=NULL, p_LmCheckPolyRing1(b, r));
  pIfThen1(a!=NULL, p_LmCheckPolyRing1(a, r));

  if (b == NULL) return TRUE;
  if (a != NULL && (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r)))
      return _p_LPLmDivisibleByNoComp(a,b,r);
  return FALSE;
}

BOOLEAN p_LPLmDivisibleBy(poly a, poly b, const ring r)
{
  p_LmCheckPolyRing1(b, r);
  pIfThen1(a != NULL, p_LmCheckPolyRing1(b, r));
  if (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r))
    return _p_LPLmDivisibleByNoComp(a, b, r);
  return FALSE;
}

BOOLEAN _p_LPLmDivisibleByNoComp(poly a, poly b, const ring r)
{
  if(p_LmIsConstantComp(a, r))
    return TRUE;
#ifdef SHIFT_MULT_COMPAT_MODE
  a = p_Head(a, r);
  p_mLPunshift(a, r);
  b = p_Head(b, r);
  p_mLPunshift(b, r);
#endif
  int i = (r->N / r->isLPring) - p_LastVblock(a, r);
  do {
    int j = r->N - (i * r->isLPring);
    bool divisible = true;
    do
    {
      if (p_GetExp(a, j, r) > p_GetExp(b, j + (i * r->isLPring), r))
      {
        divisible = false;
        break;
      }
      j--;
    }
    while (j);
    if (divisible) return TRUE;
    i--;
  }
  while (i > -1);
#ifdef SHIFT_MULT_COMPAT_MODE
  p_Delete(&a, r);
  p_Delete(&b, r);
#endif
  return FALSE;
}

poly p_LPVarAt(poly p, int pos, const ring r)
{
  if (p == NULL || pos < 1 || pos > (r->N / r->isLPring)) return NULL;
  poly v = p_One(r);
  for (int i = (pos-1) * r->isLPring + 1; i <= pos * r->isLPring; i++) {
    if (p_GetExp(p, i, r)) {
      p_SetExp(v, i - (pos-1) * r->isLPring, 1, r);
      return v;
    }
  }
  return v;
}

/// substitute weights from orderings a,wp,Wp
/// by d copies of it at position p
static BOOLEAN freeAlgebra_weights(const ring old_ring, ring new_ring, int p, int d)
{
  omFree(new_ring->wvhdl[p]);
  int *w=(int*)omAlloc(new_ring->N*sizeof(int));
  for(int b=0;b<d;b++)
  {
    for(int i=old_ring->N-1;i>=0;i--)
    {
      if (old_ring->wvhdl[p][i]<-0) return TRUE;
      w[b*old_ring->N+i]=old_ring->wvhdl[p][i];
    }
  }
  new_ring->wvhdl[p]=w;
  new_ring->block1[p]=new_ring->N;
  return FALSE;
}

ring freeAlgebra(ring r, int d)
{
  ring R=rCopy0(r);
  int p;
  if((r->order[0]==ringorder_C)
  ||(r->order[0]==ringorder_c))
    p=1;
  else
    p=0;
  // create R->N
  R->N=r->N*d;
  R->isLPring=r->N;
  // create R->order
  BOOLEAN has_order_a=FALSE;
  while (r->order[p]==ringorder_a)
  {
    if (freeAlgebra_weights(r,R,p,d))
    {
      WerrorS("weights must be positive");
      return NULL;
    }
    has_order_a=TRUE;
    p++;
  }
  R->block1[p]=R->N; /* only dp,Dp,wp,Wp; will be discarded for lp*/
  switch(r->order[p])
  {
    case ringorder_dp:
    case ringorder_Dp:
      break;
    case ringorder_wp:
    case ringorder_Wp:
      if (freeAlgebra_weights(r,R,p,d))
      {
        WerrorS("weights must be positive");
        return NULL;
      }
      break;
    case ringorder_lp:
    case ringorder_rp:
    {
      if(has_order_a)
      {
        WerrorS("ordering (a(..),lp/rp not implemented for Letterplace rings");
        return NULL;
      }
      int ** wvhdl=(int**)omAlloc0((r->N+3)*sizeof(int*));
      rRingOrder_t* ord=(rRingOrder_t*)omAlloc0((r->N+3)*sizeof(rRingOrder_t));
      int* blk0=(int*)omAlloc0((r->N+3)*sizeof(int));
      int* blk1=(int*)omAlloc0((r->N+3)*sizeof(int));
      omFree(R->wvhdl);  R->wvhdl=wvhdl;
      omFree(R->order);  R->order=ord;
      omFree(R->block0); R->block0=blk0;
      omFree(R->block1); R->block1=blk1;
      for(int i=0;i<r->N;i++)
      {
        ord[i+p]=ringorder_a;
        //Print("entry:%d->a\n",i+p);
        blk0[i+p]=1;
        blk1[i+p]=R->N;
        wvhdl[i+p]=(int*)omAlloc0(R->N*sizeof(int));
        for(int j=0;j<d;j++)
        {
          assume(j*r->N+i<R->N);
          if (r->order[p]==ringorder_lp)
            wvhdl[i+p][j*r->N+i]=1;
         else
            wvhdl[i+p][(j+1)*r->N-i-1]=1;
        }
      }
      ord[r->N+p]=r->order[p]; /* lp or rp */
      //Print("entry:%d->lp\n",r->N+p);
      blk0[r->N+p]=1;
      blk1[r->N+p]=R->N;
      // copy component order
      if (p==1) ord[0]=r->order[0];
      else if (p==0) ord[r->N+1]=r->order[1];
      else
      { // should never happen:
        WerrorS("ordering not implemented for Letterplace rings");
        return NULL;
      }
      //if (p==1) PrintS("entry:0 ->c/C\n");
      //else if (p==0) Print("entry:%d ->c/C\n",r->N+1);
      break;
    }
    default: WerrorS("ordering not implemented for Letterplace rings");
      return NULL;
  }
  // create R->names
  char **names=(char**)omAlloc(R->N*sizeof(char*));
  for(int b=0;b<d;b++)
  {
    for(int i=r->N-1;i>=0;i--)
      names[b*r->N+i]=omStrDup(r->names[i]);
  }
  for(int i=r->N-1;i>=0;i--) omFree(R->names[i]);
  omFree(R->names);
  R->names=names;

  rComplete(R,TRUE);
  return R;
}
#endif
