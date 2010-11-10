#ifndef POLYS_H
#define POLYS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT - all basic methods to manipulate polynomials of the
             currRing
*/

#include <kernel/p_polys.h>
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

 All p_* operations take as last argument a ring
 and are ring independent. Their corresponding p* operations are usually
 just macros to the respective p_*(..,currRing).

*/

/***************************************************************
 *
 * Primitives for accessing and setting fields of a poly
 * poly must be != NULL
 *
 ***************************************************************/
// deletes old coeff before setting the new one
#define pSetCoeff(p,n)      p_SetCoeff(p,n,currRing)

// Order
#define pGetOrder(p)        p_GetOrder(p, currRing)

// Component
#define pGetComp(p)         _p_GetComp(p, currRing)
#define pSetComp(p,v)       p_SetComp(p,v, currRing)
#define pAddComp(p,v)       p_AddComp(p,v,currRing)
#define pSubComp(p,v)       p_SubComp(p,v,currRing)

// Exponent
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
// allocates the space for a new monomial -- no initialization !!!
#define pNew()          p_New(currRing)
// allocates a new monomial and initializes everything to 0
#define pInit()         p_Init(currRing)
// like pInit, except that expvector is initialized to that of p,
// p must be != NULL
#define pLmInit(p)  p_LmInit(p, currRing)
// returns newly allocated copy of Lm(p), coef is copied, next=NULL,
// p might be NULL
#define pHead(p)        p_Head(p, currRing)
// frees the space of the monomial m, assumes m != NULL
// coef is not freed, m is not advanced
static inline void pLmFree(poly p)    {p_LmFree(p, currRing);}
// like pLmFree, but advances p
static inline void pLmFree(poly *p)   {p_LmFree(p, currRing);}
// assumes p != NULL, deletes p, returns pNext(p)
#define pLmFreeAndNext(p) p_LmFreeAndNext(p, currRing)
// assume p != NULL, deletes Lm(p)->coef and Lm(p)
#define pLmDelete(p)    p_LmDelete(p, currRing)
// like pLmDelete, returns pNext(p)
#define pLmDeleteAndNext(p) p_LmDeleteAndNext(p, currRing)
// used by iparith.cc
extern poly pHeadProc(poly p);

/***************************************************************
 *
 * Operation on ExpVectors: assumes polys != NULL
 *
 ***************************************************************/

#define pExpVectorCopy(d_p, s_p)    p_ExpVectorCopy(d_p, s_p, currRing)
#define pExpVectorAdd(p1, p2)       p_ExpVectorAdd(p1, p2, currRing)
#define pExpVectorSub(p1, p2)       p_ExpVectorSub(p1, p2, currRing)
#define pExpVectorAddSub(p1, p2, p3)p_ExpVectorAddSub(p1, p2, p3, currRing)
#define pExpVectorSum(pr, p1, p2)   p_ExpVectorSum(pr, p1, p2, currRing)
#define pExpVectorDiff(pr, p1, p2)  p_ExpVectorDiff(pr, p1, p2, currRing)
#define pExpVectorEqual(p1, p2)     p_ExpVectorEqual(p1, p2, currRing)

// Gets a copy of (resp. set) the exponent vector, where e is assumed
// to point to (r->N +1)*sizeof(long) memory. Exponents are
// filled in as follows: comp, e_1, .., e_n
#define pGetExpV(p, e)      p_GetExpV(p, e, currRing)
#define pSetExpV(p, e)      p_SetExpV(p, e, currRing)

/***************************************************************
 *
 * Comparisons: they are all done without regarding coeffs
 *
 ***************************************************************/
// returns 0|1|-1 if p=q|p>q|p<q w.r.t monomial ordering
#define pLmCmp(p,q)         p_LmCmp(p,q,currRing)
// executes axtionE|actionG|actionS if p=q|p>q|p<q w.r.t monomial ordering
// action should be a "goto ..."
#define pLmCmpAction(p,q, actionE, actionG, actionS)  \
  _p_LmCmpAction(p,q,currRing, actionE, actionG,actionS)

#define pLmEqual(p1, p2)     pExpVectorEqual(p1, p2)

// pCmp: args may be NULL
// returns: (p2==NULL ? 1 : (p1 == NULL ? -1 : p_LmCmp(p1, p2)))
#define pCmp(p1, p2)    p_Cmp(p1, p2, currRing)


/***************************************************************
 *
 * Divisiblity tests, args must be != NULL, except for
 * pDivisbleBy
 *
 ***************************************************************/
// returns TRUE, if leading monom of a divides leading monom of b
// i.e., if there exists a expvector c > 0, s.t. b = a + c;
#define pDivisibleBy(a, b)  p_DivisibleBy(a,b,currRing)
// like pDivisibleBy, except that it is assumed that a!=NULL, b!=NULL
#define pLmDivisibleBy(a,b)  p_LmDivisibleBy(a,b,currRing)
// like pLmDivisibleBy, does not check components
#define pLmDivisibleByNoComp(a, b) p_LmDivisibleByNoComp(a,b,currRing)
// Divisibility tests based on Short Exponent vectors
// sev_a     == pGetShortExpVector(a)
// not_sev_b == ~ pGetShortExpVector(b)
#define pLmShortDivisibleBy(a, sev_a, b, not_sev_b) \
  p_LmShortDivisibleBy(a, sev_a, b, not_sev_b, currRing)
#define pLmRingShortDivisibleBy(a, sev_a, b, not_sev_b) \
  p_LmRingShortDivisibleBy(a, sev_a, b, not_sev_b, currRing)
// returns the "Short Exponent Vector" -- used to speed up divisibility
// tests (see polys-impl.cc )
#define pGetShortExpVector(a)   p_GetShortExpVector(a, currRing)

/***************************************************************
 *
 * Copying/Deleteion of polys: args may be NULL
 *
 ***************************************************************/
// return a copy of the poly
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
// sorts p, assumes all monomials in p are different
#define pSortMerger(p)          pSort(p)
#define pSort(p)                p_SortMerge(p, currRing)

// sorts p, p may have equal monomials
#define pSortAdd(p)             p_SortAdd(p, currRing)


// Assume: If considerd only as poly in any component of p
// (say, monomials of other components of p are set to 0),
// then p is already sorted correctly
#define pSortCompCorrect(p) pSort(p)

/***************************************************************
 *
 * Predicates on polys/Lm's
 *
 ***************************************************************/
// return true if all p is eihter NULL, or if all exponents
// of p are 0 and Comp of p is zero
#define   pIsConstantComp(p)        p_IsConstantComp(p, currRing)
// like above, except that Comp might be != 0
#define   pIsConstant(p)            p_IsConstant(p,currRing)
// return true if the Lm is a constant <>0
#define   pIsUnit(p)            p_IsUnit(p,currRing)
// like above, except that p must be != NULL
#define   pLmIsConstantComp(p)      p_LmIsConstantComp(p, currRing)
#define   pLmIsConstant(p)          p_LmIsConstant(p,currRing)

// return TRUE if all monomials of p are constant
#define   pIsConstantPoly(p)        p_IsConstantPoly(p, currRing)

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
extern int      pVariables;
extern int      pOrdSgn;
extern BOOLEAN  pLexOrder;
extern poly     ppNoether;
extern BOOLEAN  pVectorOut;

/*-------------predicate on polys ----------------------*/
BOOLEAN   pHasNotCF(poly p1, poly p2);   /*has no common factor ?*/
void      pSplit(poly p, poly * r);   /*p => IN(p), r => REST(p) */



/*-------------ring management:----------------------*/
//extern void pChangeRing(ring newRing);
extern void pSetGlobals(const ring r, BOOLEAN complete = TRUE);
// resets the pFDeg and pLDeg: if pLDeg is not given, it is
// set to currRing->pLDegOrig, i.e. to the respective LDegProc which
// only uses pFDeg (and not pDeg, or pTotalDegree, etc).
// If you use this, make sure your procs does not make any assumptions
// on oredering and/or OrdIndex -- otherwise they might return wrong results
// on strat->tailRing
extern void pSetDegProcs(pFDegProc new_FDeg, pLDegProc new_lDeg = NULL);
// restores pFDeg and pLDeg:
extern void pRestoreDegProcs(pFDegProc old_FDeg, pLDegProc old_lDeg);

/*-----------the ordering of monomials:-------------*/
#define pSetm(p)    p_Setm(p, currRing)
// TODO:
#define pSetmComp   pSetm

/***************************************************************
 *
 * Degree stuff -- see p_polys.cc for explainations
 *
 ***************************************************************/
extern pLDegProc pLDeg;
extern pFDegProc pFDeg;
int  pWeight(int c, const ring r = currRing);
long pDeg(poly p,const ring r = currRing);
static inline long pTotaldegree(poly p) { return p_Totaldegree(p,currRing); }
long pWTotaldegree(poly p,const ring r = currRing);
long pWDegree(poly p,const ring r = currRing);
long pLDeg0(poly p,int *l,const ring r = currRing);
long pLDeg0c(poly p,int *l,const ring r = currRing);
long pLDegb(poly p,int *l,const ring r = currRing);
long pLDeg1(poly p,int *l,const ring r = currRing);
long pLDeg1c(poly p,int *l,const ring r = currRing);
long pLDeg1_Deg(poly p,int *l,const ring r = currRing);
long pLDeg1c_Deg(poly p,int *l,const ring r = currRing);
long pLDeg1_Totaldegree(poly p,int *l,const ring r = currRing);
long pLDeg1c_Totaldegree(poly p,int *l,const ring r = currRing);
long pLDeg1_WFirstTotalDegree(poly p,int *l,const ring r=currRing);
long pLDeg1c_WFirstTotalDegree(poly p,int *l,const ring r=currRing);

/*-------------pComp for syzygies:-------------------*/

void pSetModDeg(intvec *w);




poly      pmInit(const char *s, BOOLEAN &ok); /* monom -> poly, interpreter */
const char *    p_Read(const char *s, poly &p,const ring r); /* monom -> poly */

/*-------------operations on polynomials:------------*/
poly      pSub(poly a, poly b);
poly      p_Power(poly p, int i, const ring r);

// ----------------- define to enable new p_procs -----*/

poly      pDivide(poly a, poly b);
poly      pDivideM(poly a, poly b);
void      pLcm(poly a, poly b, poly m);
poly      pDiff(poly a, int k);
poly      pDiffOp(poly a, poly b,BOOLEAN multiply);

#define   pMaxComp(p)   p_MaxComp(p, currRing)
#define   pMinComp(p)   p_MinComp(p, currRing)
int pMaxCompProc(poly p);

#define   pOneComp(p)       p_OneComp(p, currRing)
#define   pSetCompP(a,i)    p_SetCompP(a, i, currRing)

// let's inline those, so that we can call them from the debugger
inline char*   pString(poly p)    {return p_String(p, currRing, currRing);}
inline char*   pString0(poly p)   {return p_String0(p, currRing, currRing);}
inline void    pWrite(poly p)     {p_Write(p, currRing, currRing);}
inline void    pWrite0(poly p)    {p_Write0(p, currRing, currRing);}
inline void    wrp(poly p)        {p_wrp(p, currRing, currRing);}

void      pEnlargeSet(polyset *p, int length, int increment);
#define   pISet(i) p_ISet(i,currRing)
#define   pNSet(n) p_NSet(n,currRing)

poly p_One(const ring r);
#define   pOne()   p_One(currRing)

void      p_Content(poly p, const ring r);
void      pSimpleContent(poly p, int s);
poly      p_Cleardenom(poly p, const ring r);
void      p_Cleardenom_n(poly p, const ring r,number &c);
void      p_Normalize(poly p,const ring r);
number    p_GetAllDenom(poly ph, const ring r);
#define   pNormalize(p) p_Normalize(p,currRing)
int       pSize( poly p );


// homogenizes p by multiplying certain powers of the varnum-th variable
poly      pHomogen (poly p, int varnum);

// replaces the maximal powers of the leading monomial of p2 in p1 by
// the same powers of n, utility for dehomogenization
poly      pDehomogen (poly p1,poly p2,number n);
BOOLEAN   pIsHomogeneous (poly p);

poly      pPermPoly (poly p, int * perm,const ring OldRing, nMapFunc nMap,
                     int *par_perm=NULL, int OldPar=0);

/*BOOLEAN   pVectorHasUnitM(poly p, int * k);*/
BOOLEAN   pVectorHasUnitB(poly p, int * k);
void      pVectorHasUnit(poly p, int * k, int * len);
poly      pTakeOutComp1(poly * p, int k);
// Splits *p into two polys: *q which consists of all monoms with
// component == comp and *p of all other monoms *lq == pLength(*q)
// On return all components pf *q == 0
void pTakeOutComp(poly *p, long comp, poly *q, int *lq);
// Similar to pTakeOutComp, except that only those components are
// taken out whose Order == order
// ASSUME: monomial ordering is Order compatible, i.e., if m1, m2 Monoms then
//         m1 >= m2 ==> pGetOrder(m1) >= pGetOrder(m2)
void pDecrOrdTakeOutComp(poly *p, long comp, long order,
                         poly *q, int *lq);
// This is something weird -- Don't use it, unless you know what you are doing
poly      pTakeOutComp(poly * p, int k);
void      pSetPolyComp(poly p, int comp);
void      pDeleteComp(poly * p,int k);
void      pNorm(poly p);
poly      pSubst(poly p, int n, poly e);
poly      ppJet(poly p, int m);
poly      pJet(poly p, int m);
poly      ppJetW(poly p, int m, short * iv);
poly      pJetW(poly p, int m, short * iv);
int pMinDeg(poly p,intvec *w=NULL);
poly      pSeries(int n,poly p,poly u=NULL,intvec *w=NULL);
poly      pInvers(int n, poly p,intvec *w=NULL);
// maximum weigthed degree of all monomials of p, w is indexed from
// 1..pVariables
long      pDegW(poly p, const short *w);

/*-----------type conversions ----------------------------*/
poly  pPolys2Vec(polyset p, int len);
void  pVec2Polys(poly v, polyset *p, int *len);
int   p_Var(poly mi,const ring r);
#define   pVar(m) p_Var(m,currRing)

/*-----------specials for spoly-computations--------------*/
BOOLEAN pCompareChain (poly p,poly p1,poly p2,poly lcm);
BOOLEAN pCompareChainPart (poly p,poly p1,poly p2,poly lcm);
#define  pEqualPolys(p1,p2) p_EqualPolys(p1,p2,currRing)
BOOLEAN pComparePolys(poly p1,poly p2);



/***************************************************************
 *
 * PDEBUG stuff
 *
 ***************************************************************/
#ifdef PDEBUG
#define pTest(p)        _p_Test(p, currRing, PDEBUG)
#define pLmTest(p)      _p_LmTest(p, currRing, PDEBUG)

#else // ! PDEBUG

#define pTest(p)        ((void)0)
#define pLmTest(p)      ((void)0)
#endif

#endif // POLYS_H
