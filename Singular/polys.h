#ifndef POLYS_H
#define POLYS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys.h,v 1.15 1998-06-02 15:30:02 Singular Exp $ */
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
extern int pLexSgn;
extern int pComponentOrder;

#ifdef DRING
// D=k[x,d,y] is the Weyl-Algebra [y], y commuting with all others
// M=k[x,x^(-1),y] is a D-module
// all x(1..n),d,x(1..n)^(-1),y(1..k) are considered as "ring variables" v(1..N)
// the map from x(i) to v:
#define pdX(i)  (i)
// d(i)
#define pdDX(i) (pdN+i)
// x(i)^(-1)
#define pdIX(i) (pdN+i)
// y(i)
#define pdY(i)  (pdN*2+i+1)
// a monomial m belongs to a D-module M iff pdDFlag(m)==0
// a monomial m belongs to an ideal in the Weyl-Algebra D iff pdDFlag(m)==1
#define pdDFlag(m) pGetExp(m,pdN*2+1)
#define pdSetDFlag(m,i) pSetExp(m,pdN*2+1,i)

extern int      pdN;
extern int      pdK;
extern BOOLEAN  pDRING;
poly   pdSpolyCreate(poly a, poly b);
void   pdLcm(poly a, poly b, poly m);
BOOLEAN pdIsConstantComp(poly p);
void   spModuleToPoly(poly a1);
void   pdSetDFlagP(poly p,int i);
#endif
#ifdef SRING
extern int      pAltVars;
extern BOOLEAN  pSRING;
#endif
#ifdef SDRING
void   psAug(poly q, poly done, polyset *s, int *l, int *m);
void   pdAug(poly q, polyset *s, int *l, int *m);
#endif
#ifdef SDRING
extern BOOLEAN pSDRING;
#endif

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


/*-------------ring management:----------------------*/
extern void   pChangeRing(int N, int OrdSgn,
                         int* ord, int* block0, int* block1,
                         short** wv);
extern void pSetGlobals(ring r, BOOLEAN complete = TRUE);

/*-----------the ordering of monomials:-------------*/
extern pSetmProc pSetm;
extern pCompProc pComp0;
// this is needed here as long as monomials with negative exponents might be
// compared (see in spolys.cc)
extern pCompProc t_pComp0;
int    pComp(poly p1,poly p2);
extern pLDegProc pLDeg;
extern pFDegProc pFDeg;
int pDeg(poly p);
int pTotaldegree(poly p);
int pWTotaldegree(poly p);

/*-------------pComp for syzygies:-------------------*/

void pSetModDeg(intvec *w);
void pSetSchreyerOrdB(polyset nextorder, int length);
void pSetSchreyerOrdM(polyset nextorder, int length, int comps);
int  pModuleOrder();

/*-------------storage management:-------------------*/
// allocates the space for a new monomial
#define pNew()      _pNew()
// allocates a new monomial and initializes everything to 0
#define pInit()     _pInit()

// frees the space of the monomial m
#define pFree1(m)       _pFree1(m)
// frees the space of monoial and frees coefficient
#define pDelete1(m)     _pDelete1(m)
// deletes the whole polynomial p
#define pDelete(p)      _pDelete(p)

// makes a simple memcopy of m2 into m1 -- does _not_ allocate memory for m1
#define pCopy2(m1, m2)  _pCopy2(m1, m2)
// Returns a newly allocated copy of the monomial m, initializes coefficient
// and sets the next-fieled to NULL
#define pCopy1(m)       _pCopy1(m)
// Returns a newly allocated copy of the monomial m, sets the coefficient to 0,
// and sets the next-fieled to NULL
#define pHead0(p)       _pHead0(p)
// Returns a newly allocated copy of the monomial m, copy includes copy of ceoff
// and sets the next-fieled to NULL
#define pHead(p)        _pHead(p)
extern  poly pHeadProc(poly p);
// Returns copy of the whole polynomial
#define pCopy(p)        _pCopy(p)
// Returns a converted copy (in the sense that returned poly is in
// poly of currRing) of poly p which is from ring r -- assumes that
// currRing and r have the same number of variables, i.e. that polys
// from r can be "fetched" into currRing
#define pFetchCopy(r,p)     _pFetchCopy(r,p)


// Adds exponents of p2 to exponents of p1; updates Order field
// assumes that exponents > 0 and < MAX_EXPONENT / 2
#define pMonAddFast(p1, p2) _pMonAddFast(p1, p2)
// Is equivalent to pCopy2(p1, p2);pMonAddFast(p1, p3);
#define pCopyAddFast(p1, p2, p3)    _pCopyAddFast(p1, p2, p3)
// Similar to pCopyAddFast, except that we do not care about the next field
#define pCopyAddFast0(p1, p2, p3)  _pCopyAddFast0(p1, p2, p3)
// Similar to pCopyAddFast0, except that we do not recompute the Order,
// but assume that it is the sum of the Order of p2 and p3
#define pCopyAddFastHomog(p1, p2, p3, Order)  \
  _pCopyAddFastHomog(p1, p2, p3, Order)

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


poly      pDivide(poly a, poly b);
poly      pDivideM(poly a, poly b);
void      pLcm(poly a, poly b, poly m);
poly      pDiff(poly a, int k);
poly      pDiffOp(poly a, poly b,BOOLEAN multiply);

int       pLength(poly a);
int       pMaxComp(poly p);
int       pMinComp(poly p);
int       pWeight(int c);
void      pSetCompP(poly a, int i);

// Quer sum (total degree) of all exponents of leading monomial
#define pExpQuerSum(p1)             _pExpQuerSum(p1)
// sum from 1st to "to"th exponent (including the "to" one)
#define pExpQuerSum1(p1, to)        _pExpQuerSum1(p1, to)
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
poly      pTakeOutComp(poly * p, int k);
poly      pTakeOutComp1(poly * p, int k);
void      pDeleteComp(poly * p,int k);
void      pNorm(poly p);
void      pNormalize(poly p);
poly      pSubst(poly p, int n, poly e);
poly      pJet(poly p, int m);
poly      pJetW(poly p, int m, short * iv);
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


#ifdef PDEBUG
#define pTest(A) pDBTest(A,__FILE__,__LINE__)
BOOLEAN pDBTest(poly p, char *f, int l);
#else
#define pTest(A)
#define pDBTest(A,B,C)
#endif
#endif


