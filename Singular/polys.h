#ifndef POLYS_H
#define POLYS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys.h,v 1.24 1999-09-29 10:59:37 obachman Exp $ */
/*
* ABSTRACT - all basic methods to manipulate polynomials
*/
#include "polys-impl.h"

typedef poly*   polyset;
extern int      pVariables;
extern int      pOrdSgn;
extern BOOLEAN  pLexOrder;
extern BOOLEAN  pMixedOrder;
extern poly     ppNoether;
extern BOOLEAN  pVectorOut;
// 1 for lex ordering (except ls), -1 otherwise
extern int pComponentOrder;

/* function prototypes */

/* ---------------- General poly access and iteration macros -----------*/
// IMPORTANT: Do _NOT_ make _ANY_ assumptions about their actual implementation

#define pNext(p)            _pNext(p)
#define pIter(p)            _pIter(p)

#define pGetCoeff(p)        _pGetCoeff(p)
// deletes old coeff before setting the new
#define pSetCoeff(p,n)      _pSetCoeff(p,n)
// sets new coeff without deleting the old one
#define pSetCoeff0(p,n)     _pSetCoeff0(p,n)

#define pGetOrder(p)        _pGetOrder(p)

// Gets/Sets Component field w.r.t. of polys of global ring
#define pSetComp(p,k)       _pSetComp(p,k)
#define pGetComp(p)         _pGetComp(p)
#define pIncrComp(p)        _pIncrComp(p)
#define pDecrComp(p)        _pDecrComp(p)
#define pAddComp(p,v)       _pAddComp(p,v)
#define pSubComp(p,v)       _pSubComp(p,v)

// Gets/Sets Component field w.r.t. of polys of ring r
#define pRingSetComp(r,p,k)       _pRingSetComp(r,p,k)
#define pRingGetComp(r,p)         _pRingGetComp(r,p)

// Gets/Sets ith exponent poly of global ring
#define pGetExp(p,i)        _pGetExp(p,i)
#define pSetExp(p,i,v)      _pSetExp(p,i,v)

// Gets/Sets ith exponent of poly of ring r
#define pRingGetExp(r,p,i)        _pRingGetExp(r,p,i)
#define pRingSetExp(r,p,i,v)      _pRingSetExp(r,p,i,v)

// Increments/decrements ith exponent by one
#define pIncrExp(p,i)       _pIncrExp(p,i)
#define pDecrExp(p,i)       _pDecrExp(p,i)

// Gets Difference and sum of ith Exponent of m1, m2
#define pGetExpSum(m1, m2, i)  _pGetExpSum(p1, p2, i)
#define pGetExpDiff(p1, p2, i) _pGetExpDiff(p1, p2, i)

// Adds/Substracts v to/from the ith Exponent
#define pAddExp(p,i,v)      _pAddExp(p,i,v)
#define pSubExp(p,i,v)      _pSubExp(p,i,v)
#define pMultExp(p,i,v)     _pMultExp(p,i,v)

// Gets a copy of (resp. sets) the exponent vector, where e is assumed
// to point to (pVariables+1)*sizeof(Exponent_t) memory. Exponents are
// filled in as follows: comp, e_1, .., e_n
#define pGetExpV(p, e)      _pGetExpV(p, e)
#define pSetExpV(p, e)      _pSetExpV(p, e)


/*-------------predicate on polys ----------------------*/
BOOLEAN   pIsConstant(poly p);
BOOLEAN   pIsConstantComp(poly p);
int       pIsPurePower(poly p);
#define   pIsVector(p)     (pGetComp(p)!=0)
BOOLEAN   pHasNotCF(poly p1, poly p2);   /*has no common factor ?*/
void      pSplit(poly p, poly * r);   /*p => IN(p), r => REST(p) */
// Returns TRUE if m is monom of p, FALSE otherwise
BOOLEAN pIsMonomOf(poly p, poly m);


/*-------------ring management:----------------------*/
//extern void pChangeRing(ring newRing);
extern void pSetGlobals(ring r, BOOLEAN complete = TRUE);

/*-----------the ordering of monomials:-------------*/
extern pSetmProc pSetm;
extern pCompProc pComp0;
#define pSetmComp pSetm
// this is needed here as long as monomials with negative exponents might be
// compared (see in spolys.cc)
extern pCompProc t_pComp0;
int    pComp(poly p1,poly p2);
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

/*-------------storage management:-------------------*/
// allocates the space for a new monomial -- no initialization !!!
#define pNew()          _pNew(mm_specHeap)
// allocates a new monomial and initializes everything to 0
#define pInit()         _pInit(mm_specHeap)
// frees the space of the monomial m (coef is not freed)
#define pFree1(m)       _pFree1(m, mm_specHeap)
// frees the space of monomial and frees coefficient
#define pDelete1(m)     _pDelete1(m, mm_specHeap)
// deletes the whole polynomial p
#define pDelete(p)      _pDelete(p, mm_specHeap)

// similar to routines above, except that monomials are assumed to be from heap h (resp. are allocated from heap h)
#define pHeapNew(h)             _pNew(h)
#define pHeapInit(h)            _pInit(h)
#define pHeapFree1(m, h)        _pFree1(m, h)
#define pHeapDelete1(m, h)      _pDelete1(m, h)
#define pHeapDelete(p, h)       _pDelete(p, h)

// makes a simple memcpy of m2 into m1 -- does _not_ allocate memory for m1
#define pCopy2(m1, m2)  _pCopy2(m1, m2)
// Returns a newly allocated copy of the monomial m, initializes coefficient
// and sets the next-field to NULL
#define pCopy1(m)       _pCopy1(m)
// Returns a newly allocated copy of the monomial m, sets the coefficient to 0,
// and sets the next-field to NULL
#define pHead0(p)       _pHead0(p)
// Returns a newly allocated copy of the monomial, copy includes copy of coeff
// sets the next-field to NULL
#define pHead(p)        _pHead(mm_specHeap, p)
// Similar to pHead, except that new monom is taken from heap dest_heap
#define pHeapHead(dest_heap, p) _pHead(dest_heap, p)
// Returns a shallow copy (i.e. only monomial is copied) of head of source_p
// Monom of copy is from dest_heap, assumes monoms of p are from source_heap
// Head of source_p is deleted along the way
#define pShallowCopyDeleteHead(dest_heap, source_p, source_heap) \
  _pShallowCopyDeleteHead(dest_heap, source_p, source_heap)

extern  poly pHeadProc(poly p);
// Returns copy of the whole polynomial
#define pCopy(p)        _pCopy(mm_specHeap, p)
// Returns copy of the whole poly, new monomials are taken from dest_heap
#define pHeapCopy(dest_heap, p) _pCopy(dest_heap, p)

// Returns a shallow copy (i.e. only monomials are copied) of source_p
// Monoms of copy are from dest_heap, assumes monoms of p are from source_heap
// source_p is deleted along the way
#define pShallowCopyDelete(dest_heap, source_p, source_heap) \
  _pShallowCopyDelete(dest_heap, source_p, source_heap)


// Returns a converted copy (in the sense that returned poly is in
// poly of currRing) of poly p which is from ring r -- assumes that
// currRing and r have the same number of variables, i.e. that polys
// from r can be "fetched" into currRing
#define pFetchCopy(r,p)     _pFetchCopy(r,p)
// Similar to pFetchCopy, except that poly p is deleted
#define pFetchCopyDelete(r, p) _pFetchCopyDelete(r, p)
// Similar tp pFetchCopy resp. pHead, i.e., lead monomial of p which
// lives in (fetchable) ring r is copied
#define pFetchHead(r, p)    _pFetchHead(r, p)
// similar to pFetchHead, except that lead monom of p is deleted
#define pFetchHeadDelete(r, p) _pFetchHeadDelete(r, p)
// Similar to pFree1, except that m lives in ring r
#define pRingFree1(r, m)        _pRingFree1(r, m)
// similar to pDelete1, except that m lives in ring r, which MUST be a
// "fetchable" ring w.r.t. currRing
#define pRingDelete1(r, m)        _pRingDelete1(r, m)
// similar to pDelete, except that m lives in ring r, which MUST be a
// "fetchable" ring w.r.t. currRing
#define pRingDelete(r, m)        _pRingDelete(r, m)


// Adds exponents of p2 to exponents of p1
// assumes that exponents >= 0 and and one Component != 0
#define pMonAddOn(p1, p2)   _pMonAddOn(p1, p2)
// Similar to pMonAddOn, excpet that new exponents are written in p1
#define pMonAdd(p1, p2, p3) _pMonAdd(p1, p2, p3)
// Subtracts exponetns of p1 from p1, assumes both Components are Equal
#define pMonSubFrom(p1, p2) _pMonSubFrom(p1, p2)

poly      pmInit(char *s, BOOLEAN &ok);   /* monom -> poly */
void      ppDelete(poly * a, ring r);

/*-------------operations on polynomials:------------*/
poly      pAdd(poly p1, poly p2);
poly      pNeg(poly p);
poly      pSub(poly a, poly b);
poly      pMult(poly a, poly b);
void      pMultN(poly a, number c);
poly      pMultCopyN(poly a, number c);
poly      pPower(poly p, int i);

// return TRUE, if exponent and component of Lm(p1) and Lm(p2) are equal,
// FALSE otherwise;
#define pEqual(p1, p2) _pEqual(p1, p2) // Assumes p1 != NULL & p2 != NULL
inline BOOLEAN pLmEqual(poly p1, poly p2) // no assumptions
{
  if (p1 != NULL)
  {
    if (p2 != NULL)
      return pEqual(p1, p2);
    return FALSE;
  }
  if (p2 == NULL)
    return TRUE;
  return FALSE;
}

// returns TRUE, if leading monom of a divides leading monom of b
// i.e., if there exists a expvector c > 0, s.t. b = a + c; assumes b != NULL
#define pDivisibleBy(a, b)  _pDivisibleBy(a,b)
// like pDivisibleBy, except that it is assumed that a!=NULL
#define pDivisibleBy1(a,b)   _pDivisibleBy1(a,b)
// like pDivisibleBy1, does not check components
#define pDivisibleBy2(a, b) _pDivisibleBy2(a,b)

// Divisibility tests based on Short Exponent vectors
// sev_a     == pGetShortExpVector(a)
// not_sev_b == ~ pGetShortExpVector(b)
#define pShortDivisibleBy(a, sev_a, b, not_sev_b) \
  _pShortDivisibleBy(a, sev_a, b, not_sev_b)


poly      pDivide(poly a, poly b);
poly      pDivideM(poly a, poly b);
void      pLcm(poly a, poly b, poly m);
poly      pDiff(poly a, int k);
poly      pDiffOp(poly a, poly b,BOOLEAN multiply);

int       pLength(poly a);
int       pMaxComp(poly p);
int       pMinComp(poly p);
BOOLEAN   pOneComp(poly p);
int       pWeight(int c);
void      pSetCompP(poly a, int i);

// Quer sum (total degree) of all exponents of leading monomial
#define pExpQuerSum(p1)             _pExpQuerSum(p1)
// sum from "from"th to "to"th exponent (including borders)
#define pExpQuerSum2(p1, from, to)  _pExpQuerSum2(p1, from, to)

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
void      pSetSyzComp(int k);

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
void      pNormalize(poly p);
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
poly    pMultT(poly a, poly e);
int     pDivComp(poly p, poly q);
BOOLEAN pCompareChain (poly p,poly p1,poly p2,poly lcm);
BOOLEAN pEqualPolys(poly p1,poly p2);
BOOLEAN pComparePolys(poly p1,poly p2);

/*-----------Misc stuff-----------------------------------*/
// returns the "Short Exponent Vector" -- used to speed up divisibility
// tests (see polys-impl.cc )
unsigned long pGetShortExpVector(poly p);




#ifdef PDEBUG
#define pHeapTest(A,B)  pDBTest(A, B, __FILE__,__LINE__)
#define pTest(A) pDBTest(A, mm_specHeap, __FILE__,__LINE__)
BOOLEAN pDBTest(poly p, char *f, int l);
BOOLEAN pDBTest(poly p, memHeap tail_heap, char *f, int l);
BOOLEAN pDBTest(poly p,  memHeap tail_heap, memHeap lm_heap, char *f, int l);
#else
#define pHeapTest(A,B)
#define pTest(A)
#define pDBTest(A,B,C)
#endif
#endif
