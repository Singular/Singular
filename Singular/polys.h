#ifndef POLYS_H
#define POLYS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys.h,v 1.4 1997-07-02 16:44:12 Singular Exp $ */
/*
* ABSTRACT - all basic methods to manipulate polynomials
*/
#include "structs.h"

#define VARS (100)   /*max. number of variables as constant*/

typedef short bmonomial;
typedef bmonomial monomial[VARS+1];
typedef bmonomial * pmonomial;

struct  spolyrec
{
  poly     next;
#define pNext(p) ((p)->next)
#define pIter(p) ((p) = (p)->next)
  number   coef;
//number  pGetCoeff(poly p);
#define   pGetCoeff(p)     ((p)->coef)
//void    pSetCoeff(poly p, number n);
#define   pSetCoeff(p,n)   {nDelete(&((p)->coef));(p)->coef=n;}
//void    pSetCoeff0(poly p, number n);
#define   pSetCoeff0(p,n)  (p)->coef=n
  int      Order;
#define   pGetOrder(p)     ((p)->Order)
#define POLYSIZE (sizeof(poly) + sizeof(number) + sizeof(int))
  monomial exp;
};
typedef poly*   polyset;

extern int      pVariables;
extern int      pOrdSgn;
extern BOOLEAN  pLexOrder;
extern BOOLEAN  pMixedOrder;
extern poly     ppNoether;
extern BOOLEAN  pVectorOut;

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
/*-------- several acces procedures to monomials ---- */
//void    pGetExpV(poly p, short * exp);
#define   pGetExpV(p,e)    memcpy((e),(p)->exp,(pVariables+1)*sizeof(short));
//void    pSetExpV(poly p, short * exp);
#define pSetExpV(p,e) {memcpy((p)->exp,(e),(pVariables+1)*sizeof(short));pSetm(p);}
//void    pSetExp(poly p, int varnum, int exp);
#define   pSetExp(p,v,e)   (p)->exp[(v)]=(e)
//int     pGetExp(poly p, int varnum);
#define   pGetExp(p,v)     ((p)->exp[(v)])
//void    pSetComp(poly p, int k);
#define   pSetComp(p,k)    pSetExp(p,0,k)
//int     pGetComp(poly p);
#define   pGetComp(p)      pGetExp(p,0)
BOOLEAN   pIsConstant(poly p);
BOOLEAN   pIsConstantComp(poly p);
int       pIsPurePower(poly p);
//BOOLEAN pIsVector(poly p);
#define   pIsVector(p)     (pGetComp(p)!=0)
BOOLEAN   pHasNotCF(poly p1, poly p2);   /*has no common factor ?*/
void      pSplit(poly p, poly * r);   /*p => IN(p), r => REST(p) */
//void    pCat(poly p, poly r);
#define   pCat(p,r)        ((p)->next=(r))

/*-------------ring management:----------------------*/
extern void   pChangeRing(int N, int OrdSgn,
                         int* ord, int* block0, int* block1,
                         short** wv);

/*-----------the ordering of monomials:-------------*/
extern pSetmProc pSetm;
extern pCompProc pComp0;
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
#ifdef MDEBUG
poly pDBNew(char *f, int l);
#define pNew() pDBNew(__FILE__,__LINE__)
poly pDBInit(char * f,int l);
#define pInit() pDBInit(__FILE__,__LINE__)
#else
//poly    pNew(void);
#define   pNew() (poly)mmAllocSpecialized()
poly      pInit(void);
#endif

poly      pmInit(char *s, BOOLEAN &ok);   /* monom -> poly */
poly      pOne(void);
#ifdef MDEBUG
poly      pDBCopy(poly a, char *f, int l);
#define   pCopy(A) pDBCopy(A,__FILE__,__LINE__)
#else
poly      pCopy(poly a);
#endif

void      ppDelete(poly * a, ring r);
#ifdef MDEBUG
#define   pDelete(a)             pDBDelete((a),__FILE__,__LINE__)
void      pDBDelete(poly * a, char * f, int l);
#define   pDelete1(a)            pDBDelete1((a),__FILE__,__LINE__)
void      pDBDelete1(poly * a, char * f, int l);
#define   pFree1(a)              {pDBFree1((a),__FILE__,__LINE__);(a)=NULL;}
void      pDBFree1(poly a, char * f, int l);
#else
void      pDelete(poly * a);
void      pDelete1(poly * a);
#define   pFree1(a)               mmFreeSpecialized((ADDRESS)a)
#endif

/*-------------operations on polynomials:------------*/
poly      pAdd(poly p1, poly p2);
poly      pNeg(poly p);
poly      pSub(poly a, poly b);
poly      pMult(poly a, poly b);
void      pMultN(poly a, number c);
poly      pMultCopyN(poly a, number c);
poly      pPower(poly p, int i);

BOOLEAN   pEqual(poly a, poly b);

#ifdef macintosh
BOOLEAN   pDivisibleBy(poly a, poly b);
#else
inline BOOLEAN pDivisibleBy(poly a, poly b)
{
  if ((a!=NULL)&&((a->exp[0]==0) || (a->exp[0] == b->exp[0])))
  {
    int i=pVariables;
    short *e1=&(a->exp[1]);
    short *e2=&(b->exp[1]);
    if ((*e1) > (*e2)) return FALSE;
    do
    {
      i--;
      if (i == 0) return TRUE;
      e1++;
      e2++;
    } while ((*e1) <= (*e2));
  }
  return FALSE;
}
#endif

poly      pDivide(poly a, poly b);
poly      pDivideM(poly a, poly b);
void      pLcm(poly a, poly b, poly m);
poly      pDiff(poly a, int k);
poly      pDiffOp(poly a, poly b,BOOLEAN multiply);

int       pLength(poly a);
#ifdef MDEBUG
poly pDBHead(poly p,char *f, int l);
#define pHead(A) pDBHead(A,__FILE__,__LINE__)
poly pDBHead0(poly p,char *f, int l);
#define pHead0(A) pDBHead0(A,__FILE__,__LINE__)
#else
poly pHead(poly p);
poly pHead0(poly p);
#endif
int       pMaxComp(poly p);
int       pMinComp(poly p);
int       pWeight(int c);
void      pSetCompP(poly a, int i);

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
extern int pMonomSize;

poly    pMultT(poly a, poly e);
poly    pCopy1(poly p); // copy of the head monomial: coeff is 0

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

