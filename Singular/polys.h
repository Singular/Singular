#ifndef POLYS_H
#define POLYS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys.h,v 1.37 2000-09-14 13:04:40 obachman Exp $ */
/*
* ABSTRACT - all basic methods to manipulate polynomials
*/

#include "structs.h"
#include "polys-impl.h"
/***************************************************************
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

 You can set PDEBUG on a per-file basis, before including "mod2.h"
 However, PDEBUG will only be in effect, if !NDEBUG.

 All p_* operations take as last argument a ring 
 and are ring independent. Their corresponding p* operations are usually 
 just macros to the respective p_*(..,currRing).

 ***************************************************************/

/***************************************************************
 *
 * Primitives for accessing and setting fields of a poly
 * poly must be != NULL
 *
 ***************************************************************/
// next
#define pNext(p)            _pNext(p)
#define pIter(p)            _pIter(p)

// coeff
#define pGetCoeff(p)        _pGetCoeff(p)
// deletes old coeff before setting the new one
#define pSetCoeff(p,n)      p_SetCoeff(p,n,currRing)
#define pSetCoeff0(p,n)     _pSetCoeff0(p,n)
#define p_GetCoeff(p,r)     _pGetCoeff(p)
#define p_SetCoeff0(p,n,r)  _pSetCoeff0(p,n)
PINLINE2 number p_SetCoeff(poly p, number n, ring r);

// Order
#define pGetOrder(p)        p_GetOrder(p, currRing)
PINLINE2 Order_t p_GetOrder(poly p, ring r);
// don't use this
#define pSetOrder(p, o)     p_SetOrder(p, o, currRing)
PINLINE2 Order_t p_SetOrder(poly p, long order, ring r);

// Component 
#define pGetComp(p)         _p_GetComp(p, currRing)
#define pSetComp(p,v)       p_SetComp(p,v, currRing)
#define pIncrComp(p)        p_IncrComp(p,currRing)
#define pDecrComp(p)        p_DecrComp(p,currRing)
#define pAddComp(p,v)       p_AddComp(p,v,currRing)
#define pSubComp(p,v)       p_SubComp(p,v,currRing)
#define p_GetComp(p, r)     _p_GetComp(p, r)
PINLINE2 unsigned long p_SetComp(poly p, unsigned long c, ring r);
PINLINE2 unsigned long p_IncrComp(poly p, ring r);
PINLINE2 unsigned long p_DecrComp(poly p, ring r);
PINLINE2 unsigned long p_AddComp(poly p, unsigned long v, ring r);
PINLINE2 unsigned long p_SubComp(poly p, unsigned long v, ring r);

// Exponent
#define pGetExp(p,i)        p_GetExp(p, i, currRing)
#define pSetExp(p,i,v)      p_SetExp(p, i, v, currRing)
#define pIncrExp(p,i)       p_IncrExp(p,i, currRing)
#define pDecrExp(p,i)       p_DecrExp(p,i, currRing)
#define pAddExp(p,i,v)      p_AddExp(p,i,v, currRing)
#define pSubExp(p,i,v)      p_SubExp(p,i,v, currRing)
#define pMultExp(p,i,v)     p_MultExp(p,i,v, currRing)
PINLINE2 Exponent_t p_GetExp(poly p, int v, ring r);
PINLINE2 Exponent_t p_SetExp(poly p, int v, int e, ring r);
PINLINE2 Exponent_t p_IncrExp(poly p, int v, ring r);
PINLINE2 Exponent_t p_DecrExp(poly p, int v, ring r);
PINLINE2 Exponent_t p_AddExp(poly p, int v, Exponent_t ee, ring r);
PINLINE2 Exponent_t p_SubExp(poly p, int v, Exponent_t ee, ring r);
PINLINE2 Exponent_t p_MultExp(poly p, int v, Exponent_t ee, ring r);

// Gets Difference and sum of ith Exponent of m1, m2
#define pGetExpSum(m1, m2, i)  p_GetExpSum(p1, p2, i, currRing)
#define pGetExpDiff(p1, p2, i) p_GetExpDiff(p1, p2, i, currRing)

/***************************************************************
 *
 * Allocation/Initalization/Deletion 
 * except for pDelete1 and pHead, all polys must be != NULL
 ***************************************************************/
// allocates the space for a new monomial -- no initialization !!!
#define pNew()          p_New(currRing)
// allocates a new monomial and initializes everything to 0
PINLINE1 poly pInit();
// like pInit, except that expvector is initialized to that of p, p must be != NULL
PINLINE1 poly pInit(poly p);
// returns newly allocated copy of Lm(p), coef is copied, next=NULL, p might be NULL
#define pHead(p)        p_Head(p, currRing)
// if *p_ptr != NULL, delete *p_ptr->coef, *p_ptr, and set *p_ptr to pNext(*p_ptr)
#define pDelete1(p_ptr) p_Delete1(p_ptr, currRing)
// frees the space of the monomial m, assumes m != NULL
// coef is not freed, m is not advanced
#define pFree(m)        p_Free(m, currRing)
// assumes p != NULL, deletes p, returns pNext(p)
#define pFreeAndNext(p) p_FreeAndNext(p, currRing)
// assume p != NULL, deletes Lm(p)->coef and Lm(p)
#define pLmDelete(p)    p_LmDelete(p, currRing)
// like pLmDelete, returns pNext(p)
#define pLmDeleteAndNext(p) p_LmDeleteAndNext(p, currRing)

PINLINE2 poly p_New(ring r);
PINLINE1 poly p_Init(ring r);
PINLINE1 poly p_Init(poly p, ring r);
PINLINE1 poly p_Head(poly p, ring r);
PINLINE2 void p_Delete1(poly *p, ring r);
PINLINE2 void p_Free(poly p, ring r);
PINLINE2 poly p_LmFreeAndNext(poly p, ring r);
PINLINE2 void p_LmDelete(poly p, ring r);
PINLINE2 poly p_LmDeleteAndNext(poly p, ring r);
// used by iparith.cc
extern poly pHeadProc(poly p);

/***************************************************************
 *
 * Operation on ExpVectors: assumes polys != NULL
 *
 ***************************************************************/
// ExpVextor(d_p) = ExpVector(s_p)
PINLINE1 void p_ExpVectorCopy(poly d_p, poly s_p, ring r);
// ExpVector(p1) += ExpVector(p2)
PINLINE1 void p_ExpVectorAdd(poly p1, poly p2, ring r);
// ExpVector(p1) -= ExpVector(p2)
PINLINE1 void p_ExpVectorSub(poly p1, poly p2, ring r);
// ExpVector(pr) = ExpVector(p1) + ExpVector(p2)
PINLINE1 void p_ExpVectorSum(poly pr, poly p1, poly p2, ring r);
// ExpVector(pr) = ExpVector(p1) + ExpVector(p2)
PINLINE1 void p_ExpVectorDiff(poly pr, poly p1, poly p2, ring r);
// returns TRUE if ExpVector(p1) == ExpVector(p2), FALSE, otherwise
PINLINE1 BOOLEAN p_ExpVectorEqual(poly p1, poly p2, ring r);
// returns sum of all exponents
PINLINE1 unsigned long p_ExpVectorQuerSum(poly p, ring r);

#define pExpVectorCopy(d_p, s_p)    p_ExpVectorCopy(d_p, s_p, currRing)
#define pExpVectorAdd(p1, p2)       p_ExpVectorAdd(p1, p2, currRing)
#define pExpVectorSub(p1, p2)       p_ExpVectorSub(p1, p2, currRing)
#define pExpVectorSum(pr, p1, p2)   p_ExpVectorSum(pr, p1, p2, currRing)
#define pExpVectorDiff(pr, p1, p2)  p_ExpVectorDiff(pr, p1, p2, currRing)
#define pExpVectorEqual(p1, p2)     p_ExpVectorEqual(p1, p2, currRing)
#define pExpVectorQuerSum(p)        p_ExpVectorQuerSum(p, currRing)
#define pEqual(p1, p2)              pExpVectorEqual(p1, p2)

// Gets a copy of (resp. sets) the exponent vector, where e is assumed
// to point to (pVariables+1)*sizeof(Exponent_t) memory. Exponents are
// filled in as follows: comp, e_1, .., e_n
#define pGetExpV(p, e)      p_GetExpV(p, e, currRing)
#define pSetExpV(p, e)      p_SetExpV(p, e, currRing)
PINLINE1 void p_GetExpV(poly p, Exponent_t *ev, ring r);
PINLINE1 void p_SetExpV(poly p, Exponent_t *ev, ring r);



/***************************************************************
 *
 * Operations on Lm: assumes polys != NULL
 *
 ***************************************************************/
// returns 0|1|-1 if p=q|p>q|p<q w.r.t monomial ordering
#define pLmCmp(p,q)         p_LmCmp(p,q,currRing)
// executes axtionE|actionG|actionS if p=q|p>q|p<q w.r.t monomial ordering
// action should be a "goto ..."
#define pLmCmpAction(p,q, actionE, actionG, actionS)  \
  _p_LmCmpAction(p,q,currRing, actionE, actionG,actionS)
PINLINE1 int p_LmCmp(poly p, poly q, ring r);
#define p_LmCmpAction(p, q, r, actionE, actionG, actionS) \
  _p_LmCmpAction(p, q, r, actionE, actionG, actionS)

// returns 1 if Lm(p)=Lm(q) -- including comparison of coeff
PINLINE1 BOOLEAN p_LmEqual(poly p, poly q, ring r);
#define pLmEqual(p,q)       p_LmEqual(p,q,currRing)

// pCmp: args may be NULL
// returns: (p2==NULL ? 1 : (p1 == NULL ? -1 : p_LmCmp(p1, p2)))
#define pCmp(p1, p2)    p_Cmp(p1, p2, currRing)
PINLINE2 int p_Cmp(poly p1, poly p2, ring r);

  
/***************************************************************
 *
 * Divisiblity tests, args must be != NULL, except for 
 * pDivisbleBy
 *
 ***************************************************************/
// returns TRUE, if leading monom of a divides leading monom of b
// i.e., if there exists a expvector c > 0, s.t. b = a + c; assumes b != NULL
#define pDivisibleBy(a, b)  p_DivisibleBy(a,b,currRing)
// like pDivisibleBy, except that it is assumed that a!=NULL
#define pDivisibleBy1(a,b)  p_DivisibleBy1(a,b,currRing)
// like pDivisibleBy1, does not check components
#define pDivisibleBy2(a, b) p_DivisibleBy2(a,b,currRing)
// Divisibility tests based on Short Exponent vectors
// sev_a     == pGetShortExpVector(a)
// not_sev_b == ~ pGetShortExpVector(b)
PINLINE1 BOOLEAN p_DivisibleBy(poly a, poly b, ring r);
PINLINE1 BOOLEAN p_DivisibleBy1(poly a, poly b, ring r);
PINLINE1 BOOLEAN p_DivisibleBy2(poly a, poly b, ring r);
// returns the "Short Exponent Vector" -- used to speed up divisibility
// tests (see polys-impl.cc )
#define pGetShortExpVector(a)   p_GetShortExpVector(a, currRing)
#define pShortDivisibleBy(a, sev_a, b, not_sev_b) \
  p_ShortDivisibleBy(a, sev_a, b, not_sev_b, currRing)
unsigned long p_GetShortExpVector(poly a, ring r);
PINLINE1 BOOLEAN p_ShortDivisibleBy(poly a, unsigned long sev_a, 
                                    poly b, unsigned long not_sev_b, ring r);

/***************************************************************
 *
 * Copying/Deleteion of polys: args may be NULL
 *
 ***************************************************************/
// return a copy of the poly
#define pCopy(p) p_Copy(p, currRing)
// returns a copy of p
PINLINE2 poly p_Copy(poly p, const ring r);
// returns a copy of p with Lm(p) from lmRing and Tail(p) from tailRing
PINLINE2 poly p_Copy(poly p, const ring lmRing, const ring tailRing);
// deletes *p, and sets *p to NULL
PINLINE2 void p_Delete(poly *p, const ring r);
#define pDelete(p_ptr)  p_Delete(p_ptr, currRing)


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

// returns p*q, destroys p and q
PINLINE2 poly p_Mult_q(poly p, poly q, const ring r);
// returns p*q, does neither destroy p nor q
PINLINE2 poly pp_Mult_qq(poly p, poly q, const ring r);

#define pNeg(p)                     p_Neg(p, currRing)
#define ppMult_nn(p, n)             pp_Mult_nn(p, n, currRing)
#define pMult_nn(p, n)              p_Mult_nn(p, n, currRing)
#define ppMult_mm(p, m)             pp_Mult_mm(p, m, currRing)
#define pMult_mm(p, m)              p_Mult_mm(p, m, currRing)
#define pAdd(p, q)                  p_Add_q(p, q, currRing)
#define pMinus_mm_Mult_qq(p, m, q)  p_Minus_mm_Mult_qq(p, m, q, currRing)
#define pPlus_mm_Mult_qq(p, m, q)   p_Plus_mm_Mult_qq(p, m, q, currRing)
#define pMult(p, q)                 p_Mult_q(p, q, currRing)
#define ppMult_qq(p, q)             pp_Mult_qq(p, q, currRing)

/***************************************************************
 *
 * Old stuff
 *
 ***************************************************************/

#define pFetchCopy(r,p)     _pFetchCopy(r,p)
// Similar to pFetchCopy, except that poly p is deleted
#define pFetchCopyDelete(r, p) _pFetchCopyDelete(r, p)

typedef poly*   polyset;
extern int      pVariables;
extern int      pOrdSgn;
extern BOOLEAN  pLexOrder;
extern BOOLEAN  pMixedOrder;
extern poly     ppNoether;
extern BOOLEAN  pVectorOut;
// 1 for lex ordering (except ls), -1 otherwise
extern int pComponentOrder;

/*-------------predicate on polys ----------------------*/
BOOLEAN   pIsConstant(const poly p);
BOOLEAN   pIsConstantPoly(poly p);
BOOLEAN   pIsConstantComp(const poly p);
int       pIsPurePower(const poly p);
#define   pIsVector(p)     (pGetComp(p)!=0)
BOOLEAN   pHasNotCF(poly p1, poly p2);   /*has no common factor ?*/
void      pSplit(poly p, poly * r);   /*p => IN(p), r => REST(p) */



/*-------------ring management:----------------------*/
//extern void pChangeRing(ring newRing);
extern void pSetGlobals(ring r, BOOLEAN complete = TRUE);

/*-----------the ordering of monomials:-------------*/
#define pSetm(p)    p_Setm(p, currRing)
extern void p_Setm(poly p, ring r);
// TODO:
#define pSetmComp   pSetm
#define p_SetmComp  p_Setm

extern pLDegProc pLDeg;
extern pFDegProc pFDeg;
int pDeg(poly p);
int pTotaldegree(poly p);
int pWTotaldegree(poly p);
int pWDegree(poly p);

/*-------------pComp for syzygies:-------------------*/

void pSetModDeg(intvec *w);
void pSetSchreyerOrdB(polyset nextorder, int length);
void pSetSchreyerOrdM(polyset nextorder, int length, int comps);
int  pModuleOrder();




poly      pmInit(char *s, BOOLEAN &ok);   /* monom -> poly */
void      ppDelete(poly * a, ring r);

/*-------------operations on polynomials:------------*/
poly      pSub(poly a, poly b);
poly      pPower(poly p, int i);

// ----------------- define to enable new p_procs -----*/

poly      pDivide(poly a, poly b);
poly      pDivideM(poly a, poly b);
void      pLcm(poly a, poly b, poly m);
poly      pDiff(poly a, int k);
poly      pDiffOp(poly a, poly b,BOOLEAN multiply);

int       pLength(poly a);
int       pMaxComp(poly p);
int       pMinComp(poly p, ring r=currRing);
BOOLEAN   pOneComp(poly p);
int       pWeight(int c);
#define   pSetCompP(a,i)    p_SetCompP(a, i, currRing)
PINLINE0   void p_SetCompP(poly a, int i, ring r);
PINLINE0   void p_SetCompP(poly a, int i, ring lmRing, ring tailRing);


char*     pString(poly p);
char*     pString0(poly p);
void      pWrite(poly p);
void      pWrite0(poly p);
void      wrp(poly p);

void      pEnlargeSet(polyset *p, int length, int increment);
poly      pISet(int i);
#define   pOne()   pISet(1)

void      pContent(poly p);
void      pCleardenom(poly p);
void      pNormalize(poly p);

// homogenizes p by multiplying certain powers of the varnum-th variable
poly      pHomogen (poly p, int varnum);

// replaces the maximal powers of the leading monomial of p2 in p1 by
// the same powers of n, utility for dehomogenization
poly      pDehomogen (poly p1,poly p2,number n);
BOOLEAN   pIsHomogeneous (poly p);

// returns the leading monomial of p1 divided by the maximal power of
// that of p2
poly      pDivByMonom (poly p1,poly p2);

// Returns as i-th entry of P the coefficient of the (i-1) power of
// the leading monomial of p2 in p1
void      pCancelPolyByMonom (poly p1,poly p2,polyset * P,int * SizeOfSet);

// orders monoms of poly using insertion sort, performs pSetm on each
// monom (i.e. sets Order field)
poly      pOrdPolyInsertSetm(poly p);

// orders monoms of poly using merge sort (ususally faster than
// insertion sort). ASSUMES that pSetm was performed on monoms
// (i.e. that Order field is set correctly)
poly      pOrdPolyMerge(poly p);

poly      pPermPoly (poly p, int * perm, ring OldRing,
                     int *par_perm=NULL, int OldPar=0);

/*BOOLEAN   pVectorHasUnitM(poly p, int * k);*/
BOOLEAN   pVectorHasUnitB(poly p, int * k);
void      pVectorHasUnit(poly p, int * k, int * len);
poly      pTakeOutComp1(poly * p, int k);
// Splits *p into two polys: *q which consists of all monoms with
// component == comp and *p of all other monoms *lq == pLength(*q)
// On rreturn all components pf *q == 0
void pTakeOutComp(poly *p, Exponent_t comp, poly *q, int *lq);
// Similar to pTakeOutComp, except that only those components are
// taken out whose Order == order
// ASSUME: monomial ordering is Order compatible, i.e., if m1, m2 Monoms then
//         m1 >= m2 ==> pGetOrder(m1) >= pGetOrder(m2)
void pDecrOrdTakeOutComp(poly *p, Exponent_t comp, Order_t order,
                         poly *q, int *lq);
// This is something weird -- Don't use it, unless you know what you are doing
poly      pTakeOutComp(poly * p, int k);
void      pSetPolyComp(poly p, int comp);
void      pDeleteComp(poly * p,int k);
void      pNorm(poly p);
poly      pSubst(poly p, int n, poly e);
poly      pJet(poly p, int m);
poly      pJetW(poly p, int m, short * iv);
// maximum weigthed degree of all monomials of p, w is indexed from
// 1..pVariables
int       pDegW(poly p, short *w);

/*-----------type conversions ----------------------------*/
poly  pPolys2Vec(polyset p, int len);
void  pVec2Polys(poly v, polyset *p, int *len);
int   pVar(poly m);

/*-----------specials for spoly-computations--------------*/
int     pDivComp(poly p, poly q);
BOOLEAN pCompareChain (poly p,poly p1,poly p2,poly lcm);
BOOLEAN pEqualPolys(poly p1,poly p2);
BOOLEAN pComparePolys(poly p1,poly p2);

/*-----------Misc stuff-----------------------------------*/
// reverses the monomials of p
PINLINE0 poly pReverse(poly p);


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

// check if poly p is from ring r
BOOLEAN p_CheckIsFromRing(poly p, ring r);
// check if p != NULL, r != NULL and initialized && p is from r 
BOOLEAN p_CheckPolyRing(poly p, ring r);
// check if r != NULL and initialized
BOOLEAN p_CheckRing(ring r);
// only do check if cond
#define pIfThen(cond, check) do {if (cond) {check;}} while (0)

BOOLEAN _p_Test(poly p, ring r, int level);
BOOLEAN _p_LmTest(poly p, ring r, int level);
BOOLEAN _pp_Test(poly p, ring lmRing, ring tailRing, int level);

#define pTest(p)        _p_Test(p, currRing, PDEBUG)
#define p_Test(p,r)     _p_Test(p, r, PDEBUG)
#define pLmTest(p)      _p_LmTest(p, currRing, PDEBUG)
#define p_LmTest(p,r)   _p_LmTest(p, r, PDEBUG)
#define pp_Test(p, lmRing, tailRing)    _pp_Test(p, lmRing, tailRing, PDEBUG)

#else // ! PDEBUG


#define pIsMonomOf(p, q)        (TRUE)
#define pHaveCommonMonoms(p, q) (TRUE)
#define p_CheckIsFromRing(p,r)  ((void)0)
#define p_CheckPolyRing(p,r)    ((void)0)
#define p_CheckRing(r)          ((void)0)
#define P_CheckIf(cond, check)  ((void)0)

#define pTest(p)        ((void)0)
#define p_Test(p,r)     ((void)0)
#define pLmTest(p)      ((void)0)
#define p_LmTest(p,r)   ((void)0)
#define pp_Test(p, lmRing, tailRing) ((void)0)

#endif

#endif // POLYS_H

#include "pInline2.h"
#include "pInline1.h"
