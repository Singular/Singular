/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_polys.h
 *  Purpose: declaration of poly stuf which are independent of
 *           currRing
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 9/00
 *  Version: $Id$
 *******************************************************************/
#ifndef P_POLYS_H
#define P_POLYS_H

#include <kernel/structs.h>
#include <kernel/polys-impl.h>
/*
 Some general remarks:
 We divide poly operations into roughly 4 categories:
 Level 2: operations on monomials/polynomials with constant time,
          or operations which are just dispatchers to other
          poly routines
          - implemented in: pInline2.h
          - debugging only if PDEBUG >= 2
          - normally inlined, unless PDEBUG >= 2 || NO_INLINE2
 Level 1: operations on monomials with time proportional to length
          - implemented in: pInline1.h
          - debugging only if PDEBUG >= 1
          - normally inlined, unless PDEBUG >= 1 || NO_INLINE1
 Level 0: short operations on polynomials with time proportional to
          length of poly
          - implemented in pInline0.cc
          - debugging if PDEBUG
          - normally _not_ inlined: can be forced with
            #define DO_PINLINE0
            #include "pInline0.h"
 Misc   : operations on polynomials which do not fit in any of the
          above categories
          - implemented in: polys*.cc
          - never inlined
          - debugging if PDEBUG >= 0

 You can set PDEBUG on a per-file basis, before including "mod2.h" like
   #define PDEBUG 2
   #include "mod2.h"
 However, PDEBUG will only be in effect, if !NDEBUG.
*/

/***************************************************************
 *
 * Primitives for accessing and setting fields of a poly
 * poly must be != NULL
 *
 ***************************************************************/
// next
#define pNext(p)            ((p)->next)
#define pIter(p)            ((p) = (p)->next)

// coeff
#define pGetCoeff(p)        ((p)->coef)
// deletes old coeff before setting the new one
#define pSetCoeff0(p,n)     (p)->coef=(n)
#define p_GetCoeff(p,r)     pGetCoeff(p)
#define p_SetCoeff0(p,n,r)  pSetCoeff0(p,n)
// deletes old p->coef and sets new one
PINLINE2 number p_SetCoeff(poly p, number n, ring r);

// get Order
PINLINE2 long p_GetOrder(poly p, ring r);

// Component
#define p_GetComp(p, r)     _p_GetComp(p, r)
PINLINE2 unsigned long p_SetComp(poly p, unsigned long c, ring r);
PINLINE2 unsigned long p_AddComp(poly p, unsigned long v, ring r);
PINLINE2 unsigned long p_SubComp(poly p, unsigned long v, ring r);

// Exponent
PINLINE2 long p_GetExp(poly p, int v, ring r);
PINLINE2 long p_SetExp(poly p, int v, long e, ring r);
PINLINE2 long p_IncrExp(poly p, int v, ring r);
PINLINE2 long p_DecrExp(poly p, int v, ring r);
PINLINE2 long p_AddExp(poly p, int v, long ee, ring r);
PINLINE2 long p_SubExp(poly p, int v, long ee, ring r);
PINLINE2 long p_MultExp(poly p, int v, long ee, ring r);
PINLINE2 long p_GetExpSum(poly p1, poly p2, int i, ring r);
PINLINE2 long p_GetExpDiff(poly p1, poly p2, int i, ring r);

/***************************************************************
 *
 * Allocation/Initalization/Deletion
 * except for pHead, all polys must be != NULL
 *
 ***************************************************************/
PINLINE2 poly p_New(ring r);
PINLINE2 poly p_New(ring r, omBin bin);
PINLINE1 poly p_Init(ring r);
PINLINE1 poly p_Init(ring r, omBin bin);
PINLINE1 poly p_LmInit(poly p, ring r);
PINLINE1 poly p_LmInit(poly s_p, ring s_r, ring d_p);
PINLINE1 poly p_LmInit(poly s_p, ring s_r, ring d_p, omBin d_bin);
PINLINE1 poly p_Head(poly p, ring r);
PINLINE2 void p_LmFree(poly p, ring r);
PINLINE2 void p_LmFree(poly *p, ring r);
PINLINE2 poly p_LmFreeAndNext(poly p, ring r);
PINLINE2 void p_LmDelete(poly p, ring r);
PINLINE2 void p_LmDelete(poly *p, ring r);
PINLINE2 poly p_LmDeleteAndNext(poly p, ring r);

/***************************************************************
 *
 * Operation on ExpVectors: assumes polys != NULL
 *
 ***************************************************************/
// ExpVextor(d_p) = ExpVector(s_p)
PINLINE1 void p_ExpVectorCopy(poly d_p, poly s_p, ring r);
// adjustments for negative weights
PINLINE1 void p_MemAdd_NegWeightAdjust(poly p, ring r);
PINLINE1 void p_MemSub_NegWeightAdjust(poly p, ring r);
// ExpVector(p1) += ExpVector(p2)
PINLINE1 void p_ExpVectorAdd(poly p1, poly p2, ring r);
// ExpVector(p1) -= ExpVector(p2)
PINLINE1 void p_ExpVectorSub(poly p1, poly p2, ring r);
// ExpVector(p1) += ExpVector(p2) - ExpVector(p3)
PINLINE1 void p_ExpVectorAddSub(poly p1, poly p2, poly p3, ring r);
// ExpVector(pr) = ExpVector(p1) + ExpVector(p2)
PINLINE1 void p_ExpVectorSum(poly pr, poly p1, poly p2, ring r);
// ExpVector(pr) = ExpVector(p1) + ExpVector(p2)
PINLINE1 void p_ExpVectorDiff(poly pr, poly p1, poly p2, ring r);
// returns TRUE if ExpVector(p1) == ExpVector(p2), FALSE, otherwise
PINLINE1 BOOLEAN p_ExpVectorEqual(poly p1, poly p2, ring r);
// returns sum of all exponents of p
PINLINE1 long p_Totaldegree(poly p, ring r);

PINLINE1 void p_GetExpV(poly p, int *ev, ring r);
PINLINE1 void p_SetExpV(poly p, int *ev, ring r);


/***************************************************************
 *
 * Comparisons: they are all done without regarding coeffs
 *
 ***************************************************************/
PINLINE1 int p_LmCmp(poly p, poly q, ring r);
#define p_LmCmpAction(p, q, r, actionE, actionG, actionS) \
  _p_LmCmpAction(p, q, r, actionE, actionG, actionS)

// returns 1 if ExpVector(p)==ExpVector(q): does not compare numbers !!
#define p_LmEqual(p1, p2, r) p_ExpVectorEqual(p1, p2, r)

// pCmp: args may be NULL
// returns: (p2==NULL ? 1 : (p1 == NULL ? -1 : p_LmCmp(p1, p2)))
PINLINE2 int p_Cmp(poly p1, poly p2, ring r);


/***************************************************************
 *
 * Divisiblity tests, args must be != NULL, except for
 * pDivisbleBy
 *
 ***************************************************************/
PINLINE1 BOOLEAN p_DivisibleBy(poly a, poly b, ring r);
PINLINE1 BOOLEAN p_LmDivisibleBy(poly a, poly b, ring r);
PINLINE1 BOOLEAN p_LmDivisibleByNoComp(poly a, poly b, ring r);
unsigned long p_GetShortExpVector(poly a, ring r);
PINLINE1 BOOLEAN p_LmShortDivisibleBy(poly a, unsigned long sev_a,
                                      poly b, unsigned long not_sev_b, ring r);

PINLINE1 BOOLEAN p_DivisibleBy(poly a, ring r_a, poly b, ring r_b);
PINLINE1 BOOLEAN p_LmDivisibleBy(poly a, ring r_a, poly b, ring r_b);
PINLINE1 BOOLEAN p_LmShortDivisibleBy(poly a, unsigned long sev_a, ring r_a,
                                      poly b, unsigned long not_sev_b, ring r_b);

/***************************************************************
 *
 * Misc things on Lm
 *
 ***************************************************************/
// test if the monomial is a constant as a vector component
// i.e., test if all exponents are zero
PINLINE1 BOOLEAN p_LmIsConstantComp(const poly p, const ring r);
PINLINE1 BOOLEAN p_LmIsConstant(const poly p, const ring r);

// return TRUE, if p_LmExpVectorAdd stays within ExpBound of ring r,
//       FALSE, otherwise
PINLINE1 BOOLEAN p_LmExpVectorAddIsOk(const poly p1, const poly p2, ring r);

/***************************************************************
 *
 * Misc things on polys
 *
 ***************************************************************/
// return the maximal exponent of p
PINLINE2 unsigned long p_GetMaxExp(poly p, ring r);
// return the maximal exponent of p in form of the maximal long var
unsigned long p_GetMaxExpL(poly p, const ring r, unsigned long l_max = 0);
// return monomial r such that GetExp(r,i) is maximum of all
// monomials in p; coeff == 0, next == NULL, ord is not set
poly p_GetMaxExpP(poly p, ring r);

// suppose that l is a long var in r, return maximal exponent of l
PINLINE2 unsigned long p_GetMaxExp(const unsigned long l, const ring r);

// return the TotalDegree of the long var l
PINLINE2 unsigned long p_GetTotalDegree(const unsigned long l, const ring r);
// return the total degree of the long var l containing number_of_exp exponents
PINLINE2 unsigned long p_GetTotalDegree(const unsigned long l, const ring r, const int number_of_exps);


// like the respective p_LmIs* routines, except that p might be empty
PINLINE1 BOOLEAN p_IsConstantComp(const poly p, const ring r);
PINLINE1 BOOLEAN p_IsConstant(const poly p, const ring r);
PINLINE0 BOOLEAN p_IsConstantPoly(const poly p, const ring r);

// return TRUE if all monoms have the same component
BOOLEAN   p_OneComp(poly p, ring r);

// return i, if head depends only on var(i)
int       p_IsPurePower(const poly p, const ring r);

// return i, if poly depends only on var(i)
int       p_IsUnivariate(poly p, const ring r);

// set entry e[i] to 1 if var(i) occurs in p, ignore var(j) if e[j]>0
// return #(e[i]>0)
int      p_GetVariables(poly p, int * e, const ring r);

// returns the poly representing the integer i
poly      p_ISet(int i, ring r);

// returns the poly representing the number n, destroys n
poly      p_NSet(number n, ring r);

/***************************************************************
 *
 * Copying/Deletion of polys: args may be NULL
 *
 ***************************************************************/
// returns a copy of p
PINLINE2 poly p_Copy(poly p, const ring r);
// returns a copy of p with Lm(p) from lmRing and Tail(p) from tailRing
PINLINE2 poly p_Copy(poly p, const ring lmRing, const ring tailRing);
// deletes *p, and sets *p to NULL
PINLINE2 void p_Delete(poly *p, const ring r);
PINLINE2 void p_Delete(poly *p, const ring lmRing, const ring tailRing);

// copys monomials of p, allocates new monomials from bin,
// deletes monomoals of p
PINLINE2 poly p_ShallowCopyDelete(poly p, const ring r, omBin bin);
// simial but does it only for leading monomial
PINLINE1 poly p_LmShallowCopyDelete(poly p, const ring r, omBin bin);
// simply deletes monomials, does not free coeffs
void p_ShallowDelete(poly *p, const ring r);

  

/***************************************************************
 *
 * Copying/Deleteion of polys: args may be NULL
 *  - p/q as arg mean a poly
 *  - m a monomial
 *  - n a number
 *  - pp (resp. qq, mm, nn) means arg is constant
 *  - p (resp, q, m, n)     means arg is destroyed
 *
 ***************************************************************/
// returns -p, p is destroyed
PINLINE2 poly p_Neg(poly p, const ring r);

// returns p*n, p is const (i.e. copied)
PINLINE2 poly pp_Mult_nn(poly p, number n, const ring r);
// returns p*n, destroys p
PINLINE2 poly p_Mult_nn(poly p, number n, const ring r);
PINLINE2 poly p_Mult_nn(poly p, number n, const ring lmRing, const ring tailRing);

// returns p*m, does neither destroy p nor m
PINLINE2 poly pp_Mult_mm(poly p, poly m, const ring r);
// returns p*m, destroys p, const: m
PINLINE2 poly p_Mult_mm(poly p, poly m, const ring r);

// returns p+q, destroys p and q
PINLINE2 poly p_Add_q(poly p, poly q, const ring r);
// like p_Add_q, except that if lp == pLength(lp) lq == pLength(lq)
// then lp == pLength(p+q)
PINLINE2 poly p_Add_q(poly p, poly q, int &lp, int lq, const ring r);

// return p - m*q, destroys p; const: q,m
PINLINE2 poly p_Minus_mm_Mult_qq(poly p, poly m, poly q, const ring r);
// like p_Minus_mm_Mult_qq, except that if lp == pLength(lp) lq == pLength(lq)
// then lp == pLength(p -m*q)
PINLINE2 poly p_Minus_mm_Mult_qq(poly p, poly m, poly q, int &lp, int lq,
                                 poly spNoether, const ring r);
// returns p + m*q destroys p, const: q, m
PINLINE2 poly p_Plus_mm_Mult_qq(poly p, poly m, poly q, const ring r);

// returns p + m*q destroys p, const: q, m
PINLINE2 poly p_Plus_mm_Mult_qq(poly p, poly m, poly q, int &lp, int lq,
                                const ring r);

// returns p*q, destroys p and q
PINLINE2 poly p_Mult_q(poly p, poly q, const ring r);
// returns p*q, does neither destroy p nor q
PINLINE2 poly pp_Mult_qq(poly p, poly q, const ring r);

// returns p*Coeff(m) for such monomials pm of p, for which m is divisble by pm
PINLINE2 poly pp_Mult_Coeff_mm_DivSelect(poly p, const poly m, const ring r);

// returns p*Coeff(m) for such monomials pm of p, for which m is divisble by pm
// if lp is length of p on input then lp is length of returned poly on output
PINLINE2 poly pp_Mult_Coeff_mm_DivSelect(poly p, int &lp, const poly m, const ring r);

// returns merged p and q, assumes p and q have no monomials which are equal
PINLINE2 poly p_Merge_q(poly p, poly c, const ring r);
// sorts p using bucket sort: returns sorted poly
// assumes that monomials of p are all different
// reverses it first, if revert == TRUE, use this if input p is "almost" sorted
// correctly
PINLINE2 poly p_SortMerge(poly p, const ring r, BOOLEAN revert = FALSE);
// like SortMerge, except that p may have equal monimals
PINLINE2 poly p_SortAdd(poly p, const ring r, BOOLEAN revert = FALSE);

/***************************************************************
 *
 * Misc stuff
 *
 ***************************************************************/
PINLINE2 void p_Setm(poly p, const ring r);
p_SetmProc p_GetSetmProc(ring r);

// TODO:
#define p_SetmComp  p_Setm

// sets component of poly a to i, returns length of a
PINLINE0   void p_SetCompP(poly a, int i, ring r);
PINLINE0   void p_SetCompP(poly a, int i, ring lmRing, ring tailRing);
PINLINE0   long p_MaxComp(poly p, ring lmRing, ring tailRing);
inline long p_MaxComp(poly p,ring lmRing) {return p_MaxComp(p,lmRing,lmRing);}
PINLINE0   long p_MinComp(poly p, ring lmRing, ring tailRing);
inline long p_MinComp(poly p,ring lmRing) {return p_MinComp(p,lmRing,lmRing);}

/***************************************************************
 *
 * poly things which are independent of ring
 *
 ***************************************************************/
PINLINE0 int       pLength(poly a);
PINLINE0 poly      pLast(poly a, int &length);
inline   poly      pLast(poly a) { int l; return pLast(a, l);}
PINLINE0 poly pReverse(poly p);


/***************************************************************
 *
 * I/O
 *
 ***************************************************************/
char*     p_String(poly p, ring lmRing, ring tailRing);
char*     p_String0(poly p, ring lmRing, ring tailRing);
void      p_Write(poly p, ring lmRing, ring tailRing);
void      p_Write0(poly p, ring lmRing, ring tailRing);
void      p_wrp(poly p, ring lmRing, ring tailRing);

PINLINE2 char*     p_String(poly p, ring p_ring);
PINLINE2 char*     p_String0(poly p, ring p_ring);
PINLINE2 void      p_Write(poly p, ring p_ring);
PINLINE2 void      p_Write0(poly p, ring p_ring);
PINLINE2 void      p_wrp(poly p, ring p_ring);


/***************************************************************
 *
 * Degree stuff -- see p_polys.cc for explainations
 *
 ***************************************************************/
extern pLDegProc pLDeg;
extern pFDegProc pFDeg;
int  pWeight(int i, ring r);
long pDeg(poly p, ring r);
long pWFirstTotalDegree(poly p, ring r);
long pWTotaldegree(poly p, ring r);
long pWDegree(poly p, ring r);
long pLDeg0(poly p,int *l, ring r);
long pLDeg0c(poly p,int *l, ring r);
long pLDegb(poly p,int *l, ring r);
long pLDeg1(poly p,int *l, ring r);
long pLDeg1c(poly p,int *l, ring r);
long pLDeg1_Deg(poly p,int *l, ring r);
long pLDeg1c_Deg(poly p,int *l, ring r);
long pLDeg1_Totaldegree(poly p,int *l, ring r);
long pLDeg1c_Totaldegree(poly p,int *l, ring r);
long pLDeg1_WFirstTotalDegree(poly p,int *l, ring r);
long pLDeg1c_WFirstTotalDegree(poly p,int *l, ring r);
BOOLEAN p_EqualPolys(poly p1, poly p2, const ring r);
/***************************************************************
 *
 * PDEBUG stuff
 *
 ***************************************************************/
#ifdef PDEBUG
// Returns TRUE if m is monom of p, FALSE otherwise
BOOLEAN pIsMonomOf(poly p, poly m);
// Returns TRUE if p and q have common monoms
BOOLEAN pHaveCommonMonoms(poly p, poly q);

// p_Check* routines return TRUE if everything is ok,
// else, they report error message and return false

// check if Lm(p) is from ring r
BOOLEAN p_LmCheckIsFromRing(poly p, ring r);
// check if Lm(p) != NULL, r != NULL and initialized && Lm(p) is from r
BOOLEAN p_LmCheckPolyRing(poly p, ring r);
// check if all monoms of p are from ring r
BOOLEAN p_CheckIsFromRing(poly p, ring r);
// check r != NULL and initialized && all monoms of p are from r
BOOLEAN p_CheckPolyRing(poly p, ring r);
// check if r != NULL and initialized
BOOLEAN p_CheckRing(ring r);
// only do check if cond


#define pIfThen(cond, check) do {if (cond) {check;}} while (0)

BOOLEAN _p_Test(poly p, ring r, int level);
BOOLEAN _p_LmTest(poly p, ring r, int level);
BOOLEAN _pp_Test(poly p, ring lmRing, ring tailRing, int level);

#define p_Test(p,r)     _p_Test(p, r, PDEBUG)
#define p_LmTest(p,r)   _p_LmTest(p, r, PDEBUG)
#define pp_Test(p, lmRing, tailRing)    _pp_Test(p, lmRing, tailRing, PDEBUG)

#else // ! PDEBUG

#define pIsMonomOf(p, q)        (TRUE)
#define pHaveCommonMonoms(p, q) (TRUE)
#define p_LmCheckIsFromRing(p,r)  ((void)0)
#define p_LmCheckPolyRing(p,r)    ((void)0)
#define p_CheckIsFromRing(p,r)  ((void)0)
#define p_CheckPolyRing(p,r)    ((void)0)
#define p_CheckRing(r)          ((void)0)
#define P_CheckIf(cond, check)  ((void)0)

#define p_Test(p,r)     (1)
#define p_LmTest(p,r)   (1)
#define pp_Test(p, lmRing, tailRing) (1)

#endif

#include <kernel/pInline2.h>
#include <kernel/pInline1.h>

#endif // P_POLYS_H

