/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pInline2.h
 *  Purpose: implementation of poly procs which are of constant time
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/
#ifndef PINLINE2_H
#define PINLINE2_H

/***************************************************************
 *
 * Primitives for accessing and setting fields of a poly
 *
 ***************************************************************/
#if !defined(NO_PINLINE2) || defined(PINLINE2_CC)

#include <kernel/structs.h>
#include <omalloc/omalloc.h>
#include <kernel/numbers.h>
#include <kernel/p_Procs.h>
#include <kernel/sbuckets.h>
#ifdef HAVE_PLURAL
#include <kernel/gring.h>
#include <kernel/ring.h>
#endif

PINLINE2 number p_SetCoeff(poly p, number n, ring r)
{
  p_LmCheckPolyRing2(p, r);
  n_Delete(&(p->coef), r);
  (p)->coef=n;
  return n;
}

// order
PINLINE2 long p_GetOrder(poly p, ring r)
{
  p_LmCheckPolyRing2(p, r);
  if (r->typ==NULL) return ((p)->exp[r->pOrdIndex]);
  int i=0;
  loop
  {
    switch(r->typ[i].ord_typ)
    {
      case ro_am:
      case ro_wp_neg:
        return (((long)((p)->exp[r->pOrdIndex]))-POLY_NEGWEIGHT_OFFSET);
      case ro_syzcomp:
      case ro_syz: 
      case ro_cp:
        i++;
        break;
      //case ro_dp:
      //case ro_wp:
      default:
        return ((p)->exp[r->pOrdIndex]);
    }
  }
}

// Setm
PINLINE2 void p_Setm(poly p, const ring r)
{
  p_CheckRing2(r);
  r->p_Setm(p, r);
}

// component
PINLINE2  unsigned long p_SetComp(poly p, unsigned long c, ring r)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(rRing_has_Comp(r));
  __p_GetComp(p,r) = c;
  return c;
}
PINLINE2 unsigned long p_AddComp(poly p, unsigned long v, ring r)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(rRing_has_Comp(r));
  return __p_GetComp(p,r) += v;
}
PINLINE2 unsigned long p_SubComp(poly p, unsigned long v, ring r)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(rRing_has_Comp(r));
  pPolyAssume2(__p_GetComp(p,r) >= v,p,r);
  return __p_GetComp(p,r) -= v;
}
PINLINE2 int p_Comp_k_n(poly a, poly b, int k, ring r)
{
  if ((a==NULL) || (b==NULL) ) return FALSE;
  p_LmCheckPolyRing2(a, r);
  p_LmCheckPolyRing2(b, r);
  pAssume2(k > 0 && k <= r->N);
  int i=k;
  for(;i<=r->N;i++)
  {
    if (p_GetExp(a,i,r) != p_GetExp(b,i,r)) return FALSE;
    //    if (a->exp[(r->VarOffset[i] & 0xffffff)] != b->exp[(r->VarOffset[i] & 0xffffff)]) return FALSE;
  }
  return TRUE;
}

#ifndef HAVE_EXPSIZES

/// get a single variable exponent
/// @Note:
/// the integer VarOffset encodes:
/// 1. the position of a variable in the exponent vector p->exp (lower 24 bits)
/// 2. number of bits to shift to the right in the upper 8 bits (which takes at most 6 bits for 64 bit)
/// Thus VarOffset always has 2 zero higher bits!
PINLINE2 long p_GetExp(const poly p, const unsigned long iBitmask, const int VarOffset)
{
  pAssume2((VarOffset >> (24 + 6)) == 0);
#if 0
  int pos=(VarOffset & 0xffffff);
  int bitpos=(VarOffset >> 24);
  unsigned long exp=(p->exp[pos] >> bitmask) & iBitmask;
  return exp;
#else
  return (long)
         ((p->exp[(VarOffset & 0xffffff)] >> (VarOffset >> 24))
          & iBitmask);
#endif
}


/// set a single variable exponent
/// @Note:
/// VarOffset encodes the position in p->exp @see p_GetExp
PINLINE2 unsigned long p_SetExp(poly p, const unsigned long e, const unsigned long iBitmask, const int VarOffset)
{
  pAssume2(e>=0);
  pAssume2(e<=iBitmask);
  pAssume2((VarOffset >> (24 + 6)) == 0);

  // shift e to the left:
  register int shift = VarOffset >> 24;
  unsigned long ee = e << shift /*(VarOffset >> 24)*/;
  // find the bits in the exponent vector
  register int offset = (VarOffset & 0xffffff);
  // clear the bits in the exponent vector:
  p->exp[offset]  &= ~( iBitmask << shift );
  // insert e with |
  p->exp[ offset ] |= ee;
  return e;
}


#else // #ifdef HAVE_EXPSIZES // EXPERIMENTAL!!!

PINLINE2 unsigned long BitMask(unsigned long bitmask, int twobits)
{
  // bitmask = 00000111111111111
  // 0 must give bitmask!
  // 1, 2, 3 - anything like 00011..11
  pAssume2((twobits >> 2) == 0);
  static const unsigned long _bitmasks[4] = {-1, 0x7fff, 0x7f, 0x3};
  return bitmask & _bitmasks[twobits];
}


/// @Note: we may add some more info (6 ) into VarOffset and thus encode
PINLINE2 long p_GetExp(const poly p, const unsigned long iBitmask, const int VarOffset)
{
  int pos  =(VarOffset & 0xffffff);
  int hbyte= (VarOffset >> 24); // the highest byte
  int bitpos = hbyte & 0x3f; // last 6 bits
  long bitmask = BitMask(iBitmask, hbyte >> 6);

  long exp=(p->exp[pos] >> bitpos) & bitmask;
  return exp;

}

PINLINE2 long p_SetExp(poly p, const long e, const unsigned long iBitmask, const int VarOffset)
{
  pAssume2(e>=0);
  pAssume2(e <= BitMask(iBitmask, VarOffset >> 30));

  // shift e to the left:
  register int hbyte = VarOffset >> 24;
  int bitmask = BitMask(iBitmask, hbyte >> 6);
  register int shift = hbyte & 0x3f;
  long ee = e << shift;
  // find the bits in the exponent vector
  register int offset = (VarOffset & 0xffffff);
  // clear the bits in the exponent vector:
  p->exp[offset]  &= ~( bitmask << shift );
  // insert e with |
  p->exp[ offset ] |= ee;
  return e;
}

#endif // #ifndef HAVE_EXPSIZES


PINLINE2 long p_GetExp(const poly p, const ring r, const int VarOffset)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(VarOffset != -1);
  return p_GetExp(p, r->bitmask, VarOffset);
}

PINLINE2 long p_SetExp(poly p, const long e, const ring r, const int VarOffset)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(VarOffset != -1);
  return p_SetExp(p, e, r->bitmask, VarOffset);
}



/// get v^th exponent for a monomial
PINLINE2 long p_GetExp(const poly p, const int v, const ring r)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(v>0 && v <= r->N);
  pAssume2(r->VarOffset[v] != -1);
  return p_GetExp(p, r->bitmask, r->VarOffset[v]);
}


/// set v^th exponent for a monomial
PINLINE2 long p_SetExp(poly p, const int v, const long e, const ring r)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(v>0 && v <= r->N);
  pAssume2(r->VarOffset[v] != -1);
  return p_SetExp(p, e, r->bitmask, r->VarOffset[v]);
}





// the following should be implemented more efficiently
PINLINE2  long p_IncrExp(poly p, int v, ring r)
{
  p_LmCheckPolyRing2(p, r);
  int e = p_GetExp(p,v,r);
  e++;
  return p_SetExp(p,v,e,r);
}
PINLINE2  long p_DecrExp(poly p, int v, ring r)
{
  p_LmCheckPolyRing2(p, r);
  int e = p_GetExp(p,v,r);
  pAssume2(e > 0);
  e--;
  return p_SetExp(p,v,e,r);
}
PINLINE2  long p_AddExp(poly p, int v, long ee, ring r)
{
  p_LmCheckPolyRing2(p, r);
  int e = p_GetExp(p,v,r);
  e += ee;
  return p_SetExp(p,v,e,r);
}
PINLINE2  long p_SubExp(poly p, int v, long ee, ring r)
{
  p_LmCheckPolyRing2(p, r);
  long e = p_GetExp(p,v,r);
  pAssume2(e >= ee);
  e -= ee;
  return p_SetExp(p,v,e,r);
}
PINLINE2  long p_MultExp(poly p, int v, long ee, ring r)
{
  p_LmCheckPolyRing2(p, r);
  long e = p_GetExp(p,v,r);
  e *= ee;
  return p_SetExp(p,v,e,r);
}

PINLINE2 long p_GetExpSum(poly p1, poly p2, int i, ring r)
{
  p_LmCheckPolyRing2(p1, r);
  p_LmCheckPolyRing2(p2, r);
  return p_GetExp(p1,i,r) + p_GetExp(p2,i,r);
}
PINLINE2 long p_GetExpDiff(poly p1, poly p2, int i, ring r)
{
  return p_GetExp(p1,i,r) - p_GetExp(p2,i,r);
}


/***************************************************************
 *
 * Allocation/Initalization/Deletion
 *
 ***************************************************************/
PINLINE2 poly p_New(ring r, omBin bin)
{
  p_CheckRing2(r);
  pAssume2(bin != NULL && r->PolyBin->sizeW == bin->sizeW);
  poly p;
  omTypeAllocBin(poly, p, bin);
  p_SetRingOfLm(p, r);
  return p;
}

PINLINE2 poly p_New(ring r)
{
  return p_New(r, r->PolyBin);
}

PINLINE2 void p_LmFree(poly p, ring r)
{
  p_LmCheckPolyRing2(p, r);
  omFreeBinAddr(p);
}
PINLINE2 void p_LmFree(poly *p, ring r)
{
  p_LmCheckPolyRing2(*p, r);
  poly h = *p;
  *p = pNext(h);
  omFreeBinAddr(h);
}
PINLINE2 poly p_LmFreeAndNext(poly p, ring r)
{
  p_LmCheckPolyRing2(p, r);
  poly pnext = pNext(p);
  omFreeBinAddr(p);
  return pnext;
}
PINLINE2 void p_LmDelete(poly p, ring r)
{
  p_LmCheckPolyRing2(p, r);
  n_Delete(&pGetCoeff(p), r);
  omFreeBinAddr(p);
}
PINLINE2 void p_LmDelete(poly *p, ring r)
{
  p_LmCheckPolyRing2(*p, r);
  poly h = *p;
  *p = pNext(h);
  n_Delete(&pGetCoeff(h), r);
  omFreeBinAddr(h);
}
PINLINE2 poly p_LmDeleteAndNext(poly p, ring r)
{
  p_LmCheckPolyRing2(p, r);
  poly pnext = pNext(p);
  n_Delete(&pGetCoeff(p), r);
  omFreeBinAddr(p);
  return pnext;
}

/***************************************************************
 *
 * Misc routines
 *
 ***************************************************************/
PINLINE2 int p_Cmp(poly p1, poly p2, ring r)
{
  if (p2==NULL)
    return 1;
  if (p1==NULL)
    return -1;
  return p_LmCmp(p1,p2,r);
}

PINLINE2 unsigned long p_GetMaxExp(const poly p, const ring r)
{
  return p_GetMaxExp(p_GetMaxExpL(p, r), r);
}

PINLINE2 unsigned long p_GetMaxExp(const unsigned long l, const ring r)
{
  unsigned long bitmask = r->bitmask;
  unsigned long max = (l & bitmask);
  unsigned long j = r->ExpPerLong - 1;

  if (j > 0)
  {
    unsigned long i = r->BitsPerExp;
    long e;
    loop
    {
      e = ((l >> i) & bitmask);
      if ((unsigned long) e > max)
        max = e;
      j--;
      if (j==0) break;
      i += r->BitsPerExp;
    }
  }
  return max;
}

PINLINE2 unsigned long
p_GetTotalDegree(const unsigned long l, const ring r, const int number_of_exps)
{
  const unsigned long bitmask = r->bitmask;
  unsigned long sum = (l & bitmask);
  unsigned long j = number_of_exps - 1;

  if (j > 0)
  {
    unsigned long i = r->BitsPerExp;
    loop
    {
      sum += ((l >> i) & bitmask);
      j--;
      if (j==0) break;
      i += r->BitsPerExp;
    }
  }
  return sum;
}

PINLINE2 unsigned long
p_GetTotalDegree(const unsigned long l, const ring r)
{
  return p_GetTotalDegree(l, r, r->ExpPerLong);
}

/***************************************************************
 *
 * Dispatcher to r->p_Procs, they do the tests/checks
 *
 ***************************************************************/
// returns a copy of p
PINLINE2 poly p_Copy(poly p, const ring r)
{
#ifdef PDEBUG
  poly pp= r->p_Procs->p_Copy(p, r);
  p_Test(pp,r);
  return pp;
#else
  return r->p_Procs->p_Copy(p, r);
#endif
}

PINLINE2 poly p_Copy(poly p, const ring lmRing, const ring tailRing)
{
#ifndef PDEBUG
  if (tailRing == lmRing)
    return tailRing->p_Procs->p_Copy(p, tailRing);
#endif
  if (p != NULL)
  {
    poly pres = p_Head(p, lmRing);
    pNext(pres) = tailRing->p_Procs->p_Copy(pNext(p), tailRing);
    return pres;
  }
  else
    return NULL;
}

// deletes *p, and sets *p to NULL
PINLINE2 void p_Delete(poly *p, const ring r)
{
  r->p_Procs->p_Delete(p, r);
}

PINLINE2 void p_Delete(poly *p,  const ring lmRing, const ring tailRing)
{
#ifndef PDEBUG
  if (tailRing == lmRing)
  {
    tailRing->p_Procs->p_Delete(p, tailRing);
    return;
  }
#endif
  if (*p != NULL)
  {
    if (pNext(*p) != NULL)
      tailRing->p_Procs->p_Delete(&pNext(*p), tailRing);
    p_LmDelete(p, lmRing);
  }
}

PINLINE2 poly p_ShallowCopyDelete(poly p, const ring r, omBin bin)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(r->PolyBin->sizeW == bin->sizeW);
  return r->p_Procs->p_ShallowCopyDelete(p, r, bin);
}

// returns p+q, destroys p and q
PINLINE2 poly p_Add_q(poly p, poly q, const ring r)
{
  int shorter;
  return r->p_Procs->p_Add_q(p, q, shorter, r);
}

PINLINE2 poly p_Add_q(poly p, poly q, int &lp, int lq, const ring r)
{
  int shorter;
  poly res = r->p_Procs->p_Add_q(p, q, shorter, r);
  lp = (lp + lq) - shorter;
  return res;
}

// returns p*n, destroys p
PINLINE2 poly p_Mult_nn(poly p, number n, const ring r)
{
  if (n_IsOne(n, r))
    return p;
  else
    return r->p_Procs->p_Mult_nn(p, n, r);
}

PINLINE2 poly p_Mult_nn(poly p, number n, const ring lmRing,
                        const ring tailRing)
{
#ifndef PDEBUG
  if (lmRing == tailRing)
  {
    return p_Mult_nn(p, n, tailRing);
  }
#endif
  poly pnext = pNext(p);
  pNext(p) = NULL;
  p = lmRing->p_Procs->p_Mult_nn(p, n, lmRing);
  pNext(p) = tailRing->p_Procs->p_Mult_nn(pnext, n, tailRing);
  return p;
}

// returns p*n, does not destroy p
PINLINE2 poly pp_Mult_nn(poly p, number n, const ring r)
{
  if (n_IsOne(n, r))
    return p_Copy(p, r);
  else
    return r->p_Procs->pp_Mult_nn(p, n, r);
}

// returns Copy(p)*m, does neither destroy p nor m
PINLINE2 poly pp_Mult_mm(poly p, poly m, const ring r)
{
  if (p_LmIsConstant(m, r))
    return pp_Mult_nn(p, pGetCoeff(m), r);
  else
  {
    poly last;
    return r->p_Procs->pp_Mult_mm(p, m, r, last);
  }
}

// returns p*m, destroys p, const: m
PINLINE2 poly p_Mult_mm(poly p, poly m, const ring r)
{
  if (p_LmIsConstant(m, r))
    return p_Mult_nn(p, pGetCoeff(m), r);
  else
    return r->p_Procs->p_Mult_mm(p, m, r);
}

// return p - m*Copy(q), destroys p; const: p,m
PINLINE2 poly p_Minus_mm_Mult_qq(poly p, poly m, poly q, const ring r)
{
#ifdef HAVE_PLURAL
  if (rIsPluralRing(r))
  {
    int lp, lq;
    poly spNoether;
    return nc_p_Minus_mm_Mult_qq(p, m, q, lp, lq, spNoether, r);
  }
#endif

  int shorter;
  poly last;

  return r->p_Procs->p_Minus_mm_Mult_qq(p, m, q, shorter, NULL, r, last); // !!!
}

PINLINE2 poly p_Minus_mm_Mult_qq(poly p, poly m, poly q, int &lp, int lq,
                                 poly spNoether, const ring r)
{
#ifdef HAVE_PLURAL
  if (rIsPluralRing(r))
     return nc_p_Minus_mm_Mult_qq(p, m, q, lp, lq, spNoether, r);
#endif

  int shorter;
  poly last,res;
  res = r->p_Procs->p_Minus_mm_Mult_qq(p, m, q, shorter, spNoether, r, last);
  lp = (lp + lq) - shorter;
  return res;
}

PINLINE2 poly pp_Mult_Coeff_mm_DivSelect(poly p, const poly m, const ring r)
{
  int shorter;
  return r->p_Procs->pp_Mult_Coeff_mm_DivSelect(p, m, shorter, r);
}

PINLINE2 poly pp_Mult_Coeff_mm_DivSelect(poly p, int &lp, const poly m, const ring r)
{
  int shorter;
  poly pp = r->p_Procs->pp_Mult_Coeff_mm_DivSelect(p, m, shorter, r);
  lp -= shorter;
  return pp;
}

// returns -p, destroys p
PINLINE2 poly p_Neg(poly p, const ring r)
{
  return r->p_Procs->p_Neg(p, r);
}

extern poly  _p_Mult_q(poly p, poly q, const int copy, const ring r);
// returns p*q, destroys p and q
PINLINE2 poly p_Mult_q(poly p, poly q, const ring r)
{
  if (p == NULL)
  {
    r->p_Procs->p_Delete(&q, r);
    return NULL;
  }
  if (q == NULL)
  {
    r->p_Procs->p_Delete(&p, r);
    return NULL;
  }

  if (pNext(p) == NULL)
  {
#ifdef HAVE_PLURAL
    if (rIsPluralRing(r))
      q = nc_mm_Mult_p(p, q, r);
    else
#endif /* HAVE_PLURAL */
      q = r->p_Procs->p_Mult_mm(q, p, r);

    r->p_Procs->p_Delete(&p, r);
    return q;
  }

  if (pNext(q) == NULL)
  {
  // NEEDED
#ifdef HAVE_PLURAL
/*    if (rIsPluralRing(r))
      p = gnc_p_Mult_mm(p, q, r); // ???
    else*/
#endif /* HAVE_PLURAL */
      p = r->p_Procs->p_Mult_mm(p, q, r);

    r->p_Procs->p_Delete(&q, r);
    return p;
  }
#ifdef HAVE_PLURAL
  if (rIsPluralRing(r))
    return _nc_p_Mult_q(p, q, r);
  else
#endif
  return _p_Mult_q(p, q, 0, r);
}

// returns p*q, does neither destroy p nor q
PINLINE2 poly pp_Mult_qq(poly p, poly q, const ring r)
{
  poly last;
  if (p == NULL || q == NULL) return NULL;

  if (pNext(p) == NULL)
  {
#ifdef HAVE_PLURAL
    if (rIsPluralRing(r))
      return nc_mm_Mult_pp(p, q, r);
#endif
    return r->p_Procs->pp_Mult_mm(q, p, r, last);
  }

  if (pNext(q) == NULL)
  {
    return r->p_Procs->pp_Mult_mm(p, q, r, last);
  }

  poly qq = q;
  if (p == q)
    qq = p_Copy(q, r);

  poly res;
#ifdef HAVE_PLURAL
  if (rIsPluralRing(r))
    res = _nc_pp_Mult_qq(p, qq, r);
  else
#endif
    res = _p_Mult_q(p, qq, 1, r);

  if (qq != q)
    p_Delete(&qq, r);
  return res;
}

// returns p + m*q destroys p, const: q, m
PINLINE2 poly p_Plus_mm_Mult_qq(poly p, poly m, poly q, int &lp, int lq,
                                const ring r)
{
#ifdef HAVE_PLURAL
  if (rIsPluralRing(r))
    return nc_p_Plus_mm_Mult_qq(p, m, q, lp, lq, r);
#endif

// this should be implemented more efficiently
  poly res, last;
  int shorter;
  number n_old = pGetCoeff(m);
  number n_neg = n_Copy(n_old, r);
  n_neg = n_Neg(n_neg, r);
  pSetCoeff0(m, n_neg);
  res = r->p_Procs->p_Minus_mm_Mult_qq(p, m, q, shorter, NULL, r, last);
  lp = (lp + lq) - shorter;
  pSetCoeff0(m, n_old);
  n_Delete(&n_neg, r);
  return res;
}

PINLINE2 poly p_Plus_mm_Mult_qq(poly p, poly m, poly q, const ring r)
{
  int lp = 0, lq = 0;
  return p_Plus_mm_Mult_qq(p, m, q, lp, lq, r);
}

PINLINE2 poly p_Merge_q(poly p, poly q, const ring r)
{
  return r->p_Procs->p_Merge_q(p, q, r);
}

PINLINE2 poly p_SortAdd(poly p, const ring r, BOOLEAN revert)
{
  if (revert) p = pReverse(p);
  return sBucketSortAdd(p, r);
}

PINLINE2 poly p_SortMerge(poly p, const ring r, BOOLEAN revert)
{
  if (revert) p = pReverse(p);
  return sBucketSortMerge(p, r);
}

/***************************************************************
 *
 * I/O
 *
 ***************************************************************/
PINLINE2 char*     p_String(poly p, ring p_ring)
{
  return p_String(p, p_ring, p_ring);
}
PINLINE2 char*     p_String0(poly p, ring p_ring)
{
  return p_String0(p, p_ring, p_ring);
}
PINLINE2 void      p_Write(poly p, ring p_ring)
{
  p_Write(p, p_ring, p_ring);
}
PINLINE2 void      p_Write0(poly p, ring p_ring)
{
  p_Write0(p, p_ring, p_ring);
}
PINLINE2 void      p_wrp(poly p, ring p_ring)
{
  p_wrp(p, p_ring, p_ring);
}
#endif // !defined(NO_PINLINE2) || defined(POLYS_IMPL_CC)
#endif // PINLINE2_H
