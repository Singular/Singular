#ifndef POLYS_H
#define POLYS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys.h,v 1.5 1997-12-03 16:58:59 obachman Exp $ */
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

#ifdef COMP_FAST
extern int pComponentOrder;
#endif

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
#define pdDFlag(m) ((m)->exp[pdN*2+1])

extern int      pdN;
extern int      pdK;
extern BOOLEAN  pDRING;
poly   pdSpolyCreate(poly a, poly b);
void   pdLcm(poly a, poly b, poly m);
BOOLEAN pdIsConstantComp(poly p);
void   spModuleToPoly(poly a1);
void   pdSetDFlag(poly p,int i);
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

// Gets/Sets Component field
#define pSetComp(p,k)       _pSetComp(p,k)    
#define pGetComp(p)         _pGetComp(p)
#define pIncrComp(p)        _pIncrComp(p)
#define pDecrComp(p)        _pDecrComp(p)
#define pAddComp(p,v)       _pAddComp(p,v)
#define pSubComp(p,v)       _pSubComp(p,v)

// Gets/Sets ith exponent
#define pGetExp(p,i)        _pGetExp(p,i)
#define pSetExp(p,i,v)      _pSetExp(p,i,v)

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

// Is equivalent to pCopy2(p1, p2);pMonAddFast(p1, p3);
#define pCopyAddFast(p1, p2, p3)    _pCopyAddFast(p1, p2, p3)
// Similar to pCopyAddFast, except that components of p2, and p3 must be 0
#define pCopyAddFast1(p1, p2, p3)    _pCopyAddFast1(p1, p2, p3)

poly      pmInit(char *s, BOOLEAN &ok);   /* monom -> poly */
poly      pOne(void);
void      ppDelete(poly * a, ring r);

/*-------------operations on polynomials:------------*/
poly      pAdd(poly p1, poly p2);
poly      pNeg(poly p);
poly      pSub(poly a, poly b);
poly      pMult(poly a, poly b);
void      pMultN(poly a, number c);
poly      pMultCopyN(poly a, number c);
poly      pPower(poly p, int i);

// return TRUE, if exponent and component of p1 and p2 are equal,
// FALSE otherwise
#define pEqual(p1, p2) _pEqual(p1, p2)

// returns TRUE, if leading monom of a is dividble be leading monom of b
#if defined(macintosh) || defined(DIV_COUNT)
BOOLEAN   pDivisibleBy(poly a, poly b);
#else
#define pDivisibleBy(a, b)  _pDivisibleBy(a,b)
#endif
// like pDivisibleBy, except that it is assumed that a!=NULL
#define pDivisibleBy1(a,b)   _pDivisibleBy1(a,b)
// returns TRUE, if leading monom of a is dividble be leading monom of b
// assumes a != NULL, b != NULL, and does not check components
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

void      pContent(poly p);
void      pCleardenom(poly p);

poly      pHomogen (poly p, int varnum);
  /*- homogenizes p by multiplying certain powers of the varnum-th variable -*/
poly      pDehomogen (poly p1,poly p2,number n);
  /*replaces the maximal powers of the leading monomial of p2 in p1 by
  * the same powers of n, utility for dehomogenization*/
BOOLEAN   pIsHomogeneous (poly p);
poly      pDivByMonom (poly p1,poly p2);
  /*returns the leading monomial of p1 divided by the maximal power of that
  *of p2*/
void      pCancelPolyByMonom (poly p1,poly p2,polyset * P,int * SizeOfSet);
  /*Returns as i-th entry of P the coefficient of the (i-1) power of
  * the leading monomial of p2 in p1*/
poly      pOrdPoly (poly p);
poly      pPermPoly (poly p, int * perm, int OldPvariables,
  int *par_perm=NULL, int OldPar=0);
poly      pOrdPolySchreyer(poly p);
  /*- re-orders a polynomial -*/
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


