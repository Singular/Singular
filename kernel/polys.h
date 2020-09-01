/*! \file kernel/polys.h Compatiblity layer for legacy polynomial operations (over @ref currRing)

 Macro defines for legacy polynomial operations used in @ref kernel_page and @ref singular_page.
 They take no ring argument since they work with @ref currRing by default.
 Notice that they have different prefix: `p` instead of `p_`.

 See also related global ring variable and the correct ring changeing routine:
 - \ref currRing
 - \ref rChangeCurrRing
*/

#ifndef POLYS_H
#define POLYS_H

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"

EXTERN_VAR ring currRing;
void rChangeCurrRing(ring r);

#include "coeffs/numbers.h"

/***************************************************************
 *
 * Primitives for accessing and setting fields of a poly
 * poly must be != NULL
 *
 ***************************************************************/

/// deletes old coeff before setting the new one
#define pSetCoeff(p,n)      p_SetCoeff(p,n,currRing)

/// Order
#define pGetOrder(p)        p_GetOrder(p, currRing)

/// Component
#define pGetComp(p)         (int)__p_GetComp(p, currRing)
#define pSetComp(p,v)       p_SetComp(p,v, currRing)

/// Exponent
#define pGetExp(p,i)        p_GetExp(p, i, currRing)
#define pSetExp(p,i,v)      p_SetExp(p, i, v, currRing)
#define pIncrExp(p,i)       p_IncrExp(p,i, currRing)
#define pDecrExp(p,i)       p_DecrExp(p,i, currRing)
#define pAddExp(p,i,v)      p_AddExp(p,i,v, currRing)
#define pSubExp(p,i,v)      p_SubExp(p,i,v, currRing)
#define pMultExp(p,i,v)     p_MultExp(p,i,v, currRing)
#define pGetExpSum(p1, p2, i)    p_GetExpSum(p1, p2, i, currRing)
#define pGetExpDiff(p1, p2, i)   p_GetExpDiff(p1, p2, i, currRing)


/***************************************************************
 *
 * Allocation/Initalization/Deletion
 * except for pHead, all polys must be != NULL
 *
 ***************************************************************/
/// allocates the space for a new monomial -- no initialization !!!
#define pNew()          p_New(currRing)
/// allocates a new monomial and initializes everything to 0
#define pInit()         p_Init(currRing,currRing->PolyBin)
/// like pInit, except that expvector is initialized to that of p,
/// p must be != NULL
#define pLmInit(p)  p_LmInit(p, currRing)
/// returns newly allocated copy of Lm(p), coef is copied, next=NULL,
/// p might be NULL
#define pHead(p)        p_Head(p, currRing)
/// frees the space of the monomial m, assumes m != NULL
/// coef is not freed, m is not advanced
static inline void pLmFree(poly p)    {p_LmFree(p, currRing);}
/// like pLmFree, but advances p
static inline void pLmFree(poly *p)   {p_LmFree(p, currRing);}
/// assumes p != NULL, deletes p, returns pNext(p)
#define pLmFreeAndNext(p) p_LmFreeAndNext(p, currRing)
/// assume p != NULL, deletes Lm(p)->coef and Lm(p)
#define pLmDelete(p)    p_LmDelete(p, currRing)
/// like pLmDelete, returns pNext(p)
#define pLmDeleteAndNext(p) p_LmDeleteAndNext(p, currRing)

/***************************************************************
 *
 * Operation on ExpVectors: assumes polys != NULL
 *
 ***************************************************************/

#define pExpVectorCopy(d_p, s_p)      p_ExpVectorCopy(d_p, s_p, currRing)
#define pExpVectorAdd(p1, p2)         p_ExpVectorAdd(p1, p2, currRing)
#define pExpVectorSub(p1, p2)         p_ExpVectorSub(p1, p2, currRing)
#define pExpVectorAddSub(p1, p2, p3)  p_ExpVectorAddSub(p1, p2, p3, currRing)
#define pExpVectorSum(pr, p1, p2)     p_ExpVectorSum(pr, p1, p2, currRing)
#define pExpVectorDiff(pr, p1, p2)    p_ExpVectorDiff(pr, p1, p2, currRing)

/// Gets a copy of (resp. set) the exponent vector, where e is assumed
/// to point to (r->N +1)*sizeof(long) memory. Exponents are
/// filled in as follows: comp, e_1, .., e_n
#define pGetExpV(p, e)      p_GetExpV(p, e, currRing)
#define pSetExpV(p, e)      p_SetExpV(p, e, currRing)

/***************************************************************
 *
 * Comparisons: they are all done without regarding coeffs
 *
 ***************************************************************/
/// returns 0|1|-1 if p=q|p>q|p<q w.r.t monomial ordering
#define pLmCmp(p,q)            p_LmCmp(p,q,currRing)
/// executes axtionE|actionG|actionS if p=q|p>q|p<q w.r.t monomial ordering
/// action should be a "goto ..."
#define pLmCmpAction(p,q, actionE, actionG, actionS)  \
  _p_LmCmpAction(p,q,currRing, actionE, actionG,actionS)

#define pLmEqual(p1, p2)     p_ExpVectorEqual(p1, p2, currRing)

/// pCmp: args may be NULL
/// returns: (p2==NULL ? 1 : (p1 == NULL ? -1 : p_LmCmp(p1, p2)))
#define pCmp(p1, p2)    p_Cmp(p1, p2, currRing)

/***************************************************************
 *
 * Comparisons: these are all done regarding coeffs
 *
 ***************************************************************/

#define pLtCmp(p,q)            p_LtCmp(p,q,currRing)
#define pLtCmpNoAbs(p,q)       p_LtCmpNoAbs(p,q,currRing)
#define pLtCmpOrdSgnDiffM(p,q) p_LtCmpOrdSgnDiffM(p,q,currRing)
#define pLtCmpOrdSgnDiffP(p,q) p_LtCmpOrdSgnDiffP(p,q,currRing)
#define pLtCmpOrdSgnEqM(p,q)   p_LtCmpOrdSgnEqM(p,q,currRing)
#define pLtCmpOrdSgnEqP(p,q)   p_LtCmpOrdSgnEqP(p,q,currRing)

/***************************************************************
 *
 * Divisiblity tests, args must be != NULL, except for
 * pDivisbleBy
 *
 ***************************************************************/
/// returns TRUE, if leading monom of a divides leading monom of b
/// i.e., if there exists a expvector c > 0, s.t. b = a + c;
#define pDivisibleBy(a, b)  p_DivisibleBy(a,b,currRing)
/// like pDivisibleBy, except that it is assumed that a!=NULL, b!=NULL
#define pLmDivisibleBy(a,b)  p_LmDivisibleBy(a,b,currRing)
/// like pLmDivisibleBy, does not check components
#define pLmDivisibleByNoComp(a, b) p_LmDivisibleByNoComp(a,b,currRing)
/// Divisibility tests based on Short Exponent vectors
/// sev_a     == pGetShortExpVector(a)
/// not_sev_b == ~ pGetShortExpVector(b)
#define pLmShortDivisibleBy(a, sev_a, b, not_sev_b) \
  p_LmShortDivisibleBy(a, sev_a, b, not_sev_b, currRing)
#define pLmRingShortDivisibleBy(a, sev_a, b, not_sev_b) \
  p_LmRingShortDivisibleBy(a, sev_a, b, not_sev_b, currRing)
/// returns the "Short Exponent Vector" -- used to speed up divisibility
/// tests (see polys-impl.cc )
#define pGetShortExpVector(a)   p_GetShortExpVector(a, currRing)

#ifdef HAVE_RINGS
/// divisibility check over ground ring (which may contain zero divisors);
/// TRUE iff LT(f) divides LT(g), i.e., LT(f)*c*m = LT(g), for some
/// coefficient c and some monomial m;
/// does not take components into account */
#define  pDivisibleByRingCase(f,g) p_DivisibleByRingCase(f,g,currRing)
#endif

/// polynomial division a/b, ignoring the rest
/// via singclap_pdivide resp. idLift
/// destroyes a,b
poly p_Divide(poly a, poly b, const ring r);
/// polynomial division a/b, ignoring the rest
/// via singclap_pdivide resp. idLift
/// does not destroy a,b
poly pp_Divide(poly a, poly b, const ring r);
poly p_DivRem(poly a, poly b, poly &rest, const ring r); /*julia*/

/// polynomial gcd
/// via singclap_gcd_r resp. idSyzygies
/// destroys f and g
poly singclap_gcd ( poly f, poly g, const ring r );



/***************************************************************
 *
 * Copying/Deletion of polys: args may be NULL
 *
 ***************************************************************/
/// return a copy of the poly
#define pCopy(p) p_Copy(p, currRing)
#define pDelete(p_ptr)  p_Delete(p_ptr, currRing)

/***************************************************************
 *
 * Copying/Deletion of polys: args may be NULL
 *  - p/q as arg mean a poly
 *  - m a monomial
 *  - n a number
 *  - pp (resp. qq, mm, nn) means arg is constant
 *  - p (resp, q, m, n)     means arg is destroyed
 *
 ***************************************************************/
#define pNeg(p)                     p_Neg(p, currRing)
#define ppMult_nn(p, n)             pp_Mult_nn(p, n, currRing)
#define pMult_nn(p, n)              p_Mult_nn(p, n, currRing)
#define ppMult_mm(p, m)             pp_Mult_mm(p, m, currRing)
#define pMult_mm(p, m)              p_Mult_mm(p, m, currRing)
#define pAdd(p, q)                  p_Add_q(p, q, currRing)
#define pPower(p, q)                p_Power(p, q, currRing)
#define pMinus_mm_Mult_qq(p, m, q)  p_Minus_mm_Mult_qq(p, m, q, currRing)
#define pPlus_mm_Mult_qq(p, m, q)   p_Plus_mm_Mult_qq(p, m, q, currRing)
#define pMult(p, q)                 p_Mult_q(p, q, currRing)
#define ppMult_qq(p, q)             pp_Mult_qq(p, q, currRing)
// p*Coeff(m) for such monomials pm of p, for which m is divisble by pm
#define ppMult_Coeff_mm_DivSelect(p, m)   pp_Mult_Coeff_mm_DivSelect(p, m, currRing)
/*************************************************************************
 *
 * Sort routines
 *
 *************************************************************************/
/// sorts p, assumes all monomials in p are different
#define pSortMerger(p)          p_SortMerge(p, currRing)
#define pSort(p)                p_SortMerge(p, currRing)

/// sorts p, p may have equal monomials
#define pSortAdd(p)             p_SortAdd(p, currRing)


/// Assume: If considerd only as poly in any component of p
/// (say, monomials of other components of p are set to 0),
/// then p is already sorted correctly
#define pSortCompCorrect(p) pSort(p)

/***************************************************************
 *
 * Predicates on polys/Lm's
 *
 ***************************************************************/
/// return true if p is either NULL, or if all exponents
/// of p are 0, Comp of p might be != 0
#define   pIsConstantComp(p)        p_IsConstantComp(p, currRing)
/// like above, except that Comp must be 0
#define   pIsConstant(p)            p_IsConstant(p,currRing)
/// return true if the Lm is a constant <>0
#define   pIsUnit(p)            p_IsUnit(p,currRing)
/// like above, except that p must be != NULL
#define   pLmIsConstantComp(p)      p_LmIsConstantComp(p, currRing)
#define   pLmIsConstant(p)          p_LmIsConstant(p,currRing)

/// return TRUE if all monomials of p are constant
#define   pIsConstantPoly(p)        p_IsConstant(p, currRing)

#define   pIsPurePower(p)   p_IsPurePower(p, currRing)
#define   pIsUnivariate(p)  p_IsUnivariate(p, currRing)
#define   pIsVector(p)      (pGetComp(p)>0)
#define   pGetVariables(p,e)  p_GetVariables(p, e, currRing)

/***************************************************************
 *
 * Old stuff
 *
 ***************************************************************/

typedef poly*   polyset;

/*-------------predicate on polys ----------------------*/
#define  pHasNotCFRing(p1,p2)   p_HasNotCFRing(p1,p2,currRing)
#define  pHasNotCF(p1,p2)   p_HasNotCF(p1,p2,currRing)
                                /*has no common factor ?*/
#define  pSplit(p,r)        p_Split(p,r)
                                /*p => IN(p), r => REST(p) */



/*-----------the ordering of monomials:-------------*/
#define pSetm(p)    p_Setm(p, currRing)
/// TODO:
#define pSetmComp(p)   p_Setm(p, currRing)

/***************************************************************
 *
 * Degree stuff -- see p_polys.cc for explainations
 *
 ***************************************************************/
#define pWeight(i)       p_Weight(i,currRing)

static inline long pTotaldegree(poly p) { return p_Totaldegree(p,currRing); }
#define pWTotaldegree(p) p_WTotaldegree(p,currRing)
#define pWDegree(p) p_WDegree(p,currRing)

/*-------------operations on polynomials:------------*/
#define   pSub(a,b) p_Sub(a,b,currRing)

#define pmInit(a,b) p_mInit(a,b,currRing)

/* ----------------- define to enable new p_procs -----*/

#define pMDivide(a,b) p_MDivide(a,b,currRing)
#define pDivideM(a,b) p_DivideM(a,b,currRing)
#define pLcm(a,b,m) p_Lcm(a,b,m,currRing)
#define pDiff(a,b)  p_Diff(a,b,currRing)
#define pDiffOp(a,b,m) p_DiffOp(a,b,m,currRing)

#define   pMaxComp(p)   p_MaxComp(p, currRing)
#define   pMinComp(p)   p_MinComp(p, currRing)

#define   pOneComp(p)       p_OneComp(p, currRing)
#define   pSetCompP(a,i)    p_SetCompP(a, i, currRing)

// let's inline those, so that we can call them from the debugger
inline char*   pString(poly p)    {return p_String(p, currRing, currRing);}
inline void    pString0(poly p)   {p_String0(p, currRing, currRing);}
inline void    pWrite(poly p)     {p_Write(p, currRing, currRing);}
inline void    pWrite0(poly p)    {p_Write0(p, currRing, currRing);}
inline void    wrp(poly p)        {p_wrp(p, currRing, currRing);}

#define   pISet(i) p_ISet(i,currRing)
#define   pNSet(n) p_NSet(n,currRing)

#define   pOne()   p_One(currRing)

#define   pNormalize(p) p_Normalize(p,currRing)
#define   pSize(p)      p_Size(p,currRing)


/// homogenizes p by multiplying certain powers of the varnum-th variable
#define  pHomogen(p,varnum) p_Homogen(p,varnum,currRing)

BOOLEAN   pIsHomogeneous (poly p);
// // replaces the maximal powers of the leading monomial of p2 in p1 by
// // the same powers of n, utility for dehomogenization
// #define   pDehomogen(p1,p2,n) p_Dehomgen(p1,p2,n,currRing)
// #define   pIsHomogen(p)       p_IsHomggen(p,currRing)
#define   pIsHomogen(p)       p_IsHomogen(p,currRing)

/*BOOLEAN   pVectorHasUnitM(poly p, int * k);*/
#define   pVectorHasUnitB(p,k) p_VectorHasUnitB(p,k,currRing)
#define   pVectorHasUnit(p,k,l) p_VectorHasUnit(p,k,l,currRing)
#define   pTakeOutComp1(p,k)    p_TakeOutComp1(p,k,currRing)

/// Splits *p into two polys: *q which consists of all monoms with
/// component == comp and *p of all other monoms *lq == pLength(*q)
/// On return all components pf *q == 0
inline void pTakeOutComp(poly *p, long comp, poly *q, int *lq, const ring R = currRing)
{
  return p_TakeOutComp(p, comp, q, lq, R);
}


/// This is something weird -- Don't use it, unless you know what you are doing
inline poly      pTakeOutComp(poly * p, int k, const ring R = currRing)
{
  return p_TakeOutComp(p, k, R);
}

/* old spielwiese
#define   pTakeOutComp(p,k,q,lq)    p_TakeOutComp(p,k,q,lq,currRing)

// Similar to pTakeOutComp, except that only those components are
// taken out whose Order == order
// ASSUME: monomial ordering is Order compatible, i.e., if m1, m2 Monoms then
//         m1 >= m2 ==> pGetOrder(m1) >= pGetOrder(m2)
#define   pDecrOrdTakeOutComp(p,c,o,q,lq) p_DecrOrdTakeOutComp(p,c,o,q,lq,currRing)
*/
void      pSetPolyComp(poly p, int comp);
#define   pDeleteComp(p,k) p_DeleteComp(p,k,currRing)

inline void pNorm(poly p, const ring R = currRing){ p_Norm(p, R); }


#define   pSubst(p,n,e) p_Subst(p,n,e,currRing)
#define   ppJet(p,m) pp_Jet(p,m,currRing)
#define   pJet(p,m)  p_Jet(p,m,currRing)
#define   ppJetW(p,m,iv) pp_JetW(p,m,iv,currRing)
#define   pJetW(p,m,iv) p_JetW(p,m,iv,currRing)
#define   pMinDeg(p,w) p_MinDeg(p,w,currRing)
#define   pSeries(n,p,u,w) p_Series(n,p,u,w,currRing)
// maximum weigthed degree of all monomials of p, w is indexed from
// 1..pVariables

/// Deprecated: only for compatibility with older code!
#define    pDegW(p,w) p_DegW(p,w,currRing)

/*-----------type conversions ----------------------------*/
// void  pVec2Polys(poly v, polyset *p, int *len);
#define   pVar(m) p_Var(m,currRing)

/*-----------specials for spoly-computations--------------*/

/// Returns TRUE if
///      * LM(p) | LM(lcm)
///      * LC(p) | LC(lcm) only if ring
///      * Exists i, j:
///          * LE(p, i)  != LE(lcm, i)
///          * LE(p1, i) != LE(lcm, i)   ==> LCM(p1, p) != lcm
///          * LE(p, j)  != LE(lcm, j)
///          * LE(p2, j) != LE(lcm, j)   ==> LCM(p2, p) != lcm
BOOLEAN pCompareChain (poly p, poly p1, poly p2, poly lcm, const ring R = currRing);

#ifdef HAVE_RATGRING
BOOLEAN pCompareChainPart (poly p, poly p1, poly p2, poly lcm, const ring R = currRing);
#endif


#define  pEqualPolys(p1,p2) p_EqualPolys(p1,p2,currRing)



/// returns the length of a polynomial (numbers of monomials)
/// respect syzComp
static inline poly pLast(poly a, int &length) { return p_Last (a, length, currRing); }
static inline poly pLast(poly a) { int l; return pLast(a, l); }

/***************************************************************
 *
 * PDEBUG stuff
 *
 ***************************************************************/
#ifdef PDEBUG
#define pTest(p)        _p_Test(p, currRing, PDEBUG)
#define pLmTest(p)      _p_LmTest(p, currRing, PDEBUG)

#else // ! PDEBUG

#define pTest(p)        do {} while (0)
#define pLmTest(p)      do {} while (0)
#endif

#endif // POLYS_H
