#ifndef KUTIL_H
#define KUTIL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kutil.h,v 1.21 1999-11-02 15:19:08 Singular Exp $ */
/*
* ABSTRACT: kernel: utils for kStd
*/
#include <string.h>
#include "mod2.h"
#include "structs.h"
#include "mmemory.h"
#include "ring.h"
#if HAVE_ASO == 1
#include "kutil.aso"
#endif

#define setmax 16

typedef int* intset;

class sTObject
{
public:
  poly  p;
  int ecart,length, pLength;
  memHeap heap;
  unsigned long sev;
  sTObject() { memset((void*) this, 0, sizeof(sTObject)); heap = mm_specHeap;}
  inline poly SetP(poly p_new);
};

class sLObject
{
public:
  poly  p;
  poly  p1,p2; /*- the pair p comes from -*/
  poly  lcm;   /*- the lcm of p1,p2 -*/
  memHeap heap;
  int ecart,length, pLength;
  unsigned long sev;
  sLObject() { memset((void*) this, 0, sizeof(sLObject));}
};

typedef class sTObject TObject;
typedef class sLObject LObject;
typedef TObject * TSet;
typedef LObject * LSet;

extern int HCord;

class skStrategy;
typedef skStrategy * kStrategy;
class skStrategy
{
public:
  kStrategy next;
  int (*red)(LObject * L,kStrategy strat);
  void (*initEcart)(LObject * L);
  int (*posInT)(const TSet T,const int tl,const LObject &h);
  int (*posInL)(const LSet set, const int length,
                const LObject &L,const kStrategy strat);
  void (*enterS)(LObject h, int pos,kStrategy strat);
  void (*initEcartPair)(LObject * h, poly f, poly g, int ecartF, int ecartG);
  int (*posInLOld)(const LSet Ls,const int Ll,
                   const LObject &Lo,const kStrategy strat);
  pFDegProc pOldFDeg;
  ideal Shdl;
  ideal D; /*V(S) is in D(D)*/
  ideal M; /*set of minimal generators*/
  polyset S;
  intset ecartS;
  intset fromQ;
  unsigned long* sevS;
  TSet T;
  LSet L;
  LSet    B;
  poly    kHEdge;
  poly    kNoether;
  BOOLEAN * NotUsedAxis;
  LObject P;
  poly tail;
  leftv kIdeal;
  intvec * kModW;
  intvec * kHomW;
  memHeap THeap;       // if != NULL, heap for monoms of T
  BOOLEAN use_redheap; // if TRUE, use extra heap for reductions
  BOOLEAN *pairtest;/*used for enterOnePair*/
  int cp,c3;
  int sl,mu;
  int tl,tmax;
  int Ll,Lmax;
  int Bl,Bmax;
  int ak,LazyDegree,LazyPass;
  int syzComp;
  int HCord;
  int lastAxis;
  int newIdeal;
  int minim;
  BOOLEAN interpt;
  BOOLEAN homog;
  BOOLEAN kHEdgeFound;
  BOOLEAN honey,sugarCrit;
  BOOLEAN Gebauer,noTailReduction;
  BOOLEAN fromT;
  BOOLEAN noetherSet;
  BOOLEAN update;
  BOOLEAN posInLOldFlag;
  /*FALSE, if posInL == posInL10*/
  char    redTailChange;
  char    news;
  char    newt;/*used for messageSets*/
};

void deleteHC(poly *p, int *e, int *l, kStrategy strat);
void deleteInS (int i,kStrategy strat);
void cleanT (kStrategy strat);
LSet initL ();
void deleteInL(LSet set, int *length, int j,kStrategy strat);
void enterL (LSet *set,int *length, int *LSetmax, LObject p,int at);
void initEcartPairBba (LObject* Lp,poly f,poly g,int ecartF,int ecartG);
void initEcartPairMora (LObject* Lp,poly f,poly g,int ecartF,int ecartG);
int posInS (polyset set,int length,poly p);
int posInT0 (const TSet set,const int length,const LObject &p);
int posInT1 (const TSet set,const int length,const LObject &p);
int posInT2 (const TSet set,const int length,const LObject &p);
int posInT11 (const TSet set,const int length,const LObject &p);
int posInT13 (const TSet set,const int length,const LObject &p);
int posInT15 (const TSet set,const int length,const LObject &p);
int posInT17 (const TSet set,const int length,const LObject &p);
int posInT19 (const TSet set,const int length,const LObject &p);
void reorderS (int* suc,kStrategy strat);
int posInL0 (const LSet set, const int length,
             const LObject &L,const kStrategy strat);
int posInL11 (const LSet set, const int length,
             const LObject &L,const kStrategy strat);
int posInL13 (const LSet set, const int length,
             const LObject &L,const kStrategy strat);
int posInL15 (const LSet set, const int length,
             const LObject &L,const kStrategy strat);
int posInL17 (const LSet set, const int length,
             const LObject &L,const kStrategy strat);
poly redtailBba (poly p,int pos,kStrategy strat);
poly redtailSyz (poly p,int pos,kStrategy strat);
poly redtail (poly p,int pos,kStrategy strat);
void enterpairs (poly h, int k, int ec, int pos,kStrategy strat);
void entersets (LObject h);
void pairs ();
void message (int i,int* reduc,int* olddeg,kStrategy strat);
void messageStat (int srmax,int lrmax,int hilbcount,kStrategy strat);
void messageSets (kStrategy strat);
void initEcartNormal (LObject* h);
void initEcartBBA (LObject* h);
void initS (ideal F, ideal Q,kStrategy strat);
void initSL (ideal F, ideal Q,kStrategy strat);
void updateS(BOOLEAN toT,kStrategy strat);
void enterSBba (LObject p, int pos,kStrategy strat);
void enterT (LObject p,kStrategy strat);
void enterTBba (LObject p, int pos,kStrategy strat);
void cancelunit (LObject* p);
void HEckeTest (poly pp,kStrategy strat);
void redtailS (poly* h,int maxIndex);
void redtailMora (poly* h,int maxIndex);
void kinitBbaHeaps(kStrategy heap);
void initBuchMoraCrit(kStrategy strat);
void initHilbCrit(ideal F, ideal Q, intvec **hilb,kStrategy strat);
void initBuchMoraPos(kStrategy strat);
void initBuchMora (ideal F, ideal Q,kStrategy strat);
void exitBuchMora (kStrategy strat);
void updateResult(ideal r,ideal Q,kStrategy strat);
void completeReduce (kStrategy strat);
void kFreeStrat(kStrategy strat);
BOOLEAN homogTest(polyset F, int Fmax);
BOOLEAN newHEdge(polyset S, int ak,kStrategy strat);

inline TSet initT () { return (TSet)Alloc0(setmax*sizeof(TObject)); }

#ifdef KDEBUG
#define kTest(A) K_Test(__FILE__,__LINE__,A)
#define kTest_TS(A) K_Test(__FILE__,__LINE__,A)
#define kTest_T(T) K_Test_T(__FILE__,__LINE__,T)
#define kTest_L(L) K_Test_L(__FILE__,__LINE__,L)
#define kTest_S(strat) K_Test_S(__FILE__,__LINE__,strat)
#define kTest_Pref(L) K_Test(__FILE__,__LINE__,L, 2)
BOOLEAN K_Test(char *f, int l,kStrategy strat, int pref=0);
BOOLEAN K_Test_TS(char *f, int l,kStrategy strat);
BOOLEAN K_Test_T(char *f, int l, TObject* T, int tpos = -1);
BOOLEAN K_Test_L(char* f, int l, LObject* L, 
                 BOOLEAN testp = FALSE, int lpos = -1, 
                 TSet T = NULL, int tlength = -1);
BOOLEAN K_Test_S(char* f, int l, kStrategy strat);
#else
#define kTest(A)    (TRUE)
#define kTest_Pref(A) (TRUE)
#define kTest_TS(A) (TRUE)
#define kTest_T(T)  (TRUE)
#define kTest_S(T)  (TRUE)
#define kTest_L(T)  (TRUE)
#endif
#endif

/***************************************************************
 *
 * From kstd2.cc
 * 
 ***************************************************************/
ideal bba (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat);
poly kNF2 (ideal F, ideal Q, poly q, kStrategy strat, int lazyReduce);
ideal kNF2 (ideal F,ideal Q,ideal q, kStrategy strat, int lazyReduce);
void initBba(ideal F,kStrategy strat);

/***************************************************************
 *
 * From kSpolys.cc
 * 
 ***************************************************************/
// Reduces PR with PW
// Assumes PR != NULL, PW != NULL, Lm(PW) divides Lm(PR)
// Changes: PR
// Const:   PW
// If coef != NULL, then *coef is a/gcd(a,b), where a = LC(PR), b = LC(PW)
void ksReducePoly(LObject* PR,
                  TObject* PW,
                  poly spNoether = NULL,
                  number *coef = NULL);

// Reduces PR at Current->next with PW
// Assumes PR != NULL, Current contained in PR 
//         Current->next != NULL, LM(PW) devides LM(Current->next)
// Changes: PR
// Const:   PW
void ksReducePolyTail(LObject* PR,
                      TObject* PW,
                      poly Current,
                      poly spNoether = NULL);

// Creates S-Poly of Pair
// Const:   Pair->p1, Pair->p2
// Changes: Pair->p == S-Poly of p1, p2
// Assume:  Pair->p1 != NULL && Pair->p2
void ksCreateSpoly(LObject* Pair, 
                   poly spNoether = NULL);


/*2
* creates the leading term of the S-polynomial of p1 and p2
* do not destroy p1 and p2
* remarks:
*   1. the coefficient is 0 (nNew)
*   2. pNext is undefined
*/
poly ksCreateShortSpoly(poly p1, poly p2);


/*
* input - output: a, b
* returns:
*   a := a/gcd(a,b), b := b/gcd(a,b)
*   and return value
*       0  ->  a != 1,  b != 1
*       1  ->  a == 1,  b != 1
*       2  ->  a != 1,  b == 1
*       3  ->  a == 1,  b == 1
*   this value is used to control the spolys
*/
int ksCheckCoeff(number *a, number *b);

// old stuff
poly ksOldSpolyRed(poly p1, poly p2, poly spNoether = NULL);
poly ksOldSpolyRedNew(poly p1, poly p2, poly spNoether = NULL);
poly ksOldCreateSpoly(poly p1, poly p2, poly spNoether = NULL);
void ksOldSpolyTail(poly p1, poly q, poly q2, poly spNoether);


#include "polys.h"
// Methods for tobjects
inline poly sTObject::SetP(poly new_p)
{
  poly old_p = p;
  memset((void*) this, 0, sizeof(sTObject));
  p = new_p;
  sev = pGetShortExpVector(p);
  return p;
}

