#ifndef KUTIL_H
#define KUTIL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kutil.h,v 1.38 2000-10-26 16:31:36 obachman Exp $ */
/*
* ABSTRACT: kernel: utils for kStd
*/


#include <string.h>
#include "mod2.h"

#include "structs.h"
#include "omalloc.h"
#include "ring.h"
#include "pShallowCopyDelete.h"

#define setmax 16

#undef NO_KINLINE
#if !defined(KDEBUG) && !defined(NO_INLINE)
#define KINLINE inline
#else
#define KINLINE
#define NO_KINLINE 1
#endif

typedef int* intset;

class sTObject  
{
public:
  poly p;       // Lm(p) \in currRing Tail(p) \in tailRing
  poly t_p;     // t_p \in tailRing
  ring tailRing;
  int ecart, length, pLength;
  poly max; // p_MaxExp(pNext(p))
  unsigned long sev;
  

  // initialization
  KINLINE void Init(ring r = currRing);
  KINLINE sTObject(ring tailRing = currRing);
  KINLINE sTObject(poly p, ring tailRing = currRing);
  KINLINE sTObject(poly p, ring c_r, ring tailRing);
  KINLINE sTObject(sTObject* T, int copy);

  KINLINE void Set(ring r=currRing);
  KINLINE void Set(poly p_in, ring r=currRing);
  KINLINE void Set(poly p_in, ring c_r, ring t_r);

  KINLINE void sTObject::Delete();
  
  // ring-dependent Lm access
  KINLINE poly GetLm(); // equivalent to GetLmCurrRing
  KINLINE poly GetLmCurrRing();
  KINLINE poly GetLmTailRing();
  KINLINE poly GetLm(ring r);

  // makes sure that T.p exists
  KINLINE void SetLmCurrRing();

  // Iterations
  KINLINE void LmDeleteAndIter();

  // deg stuff
  KINLINE int pFDeg();

  // arithmetic
  KINLINE void Mult_nn(number n);
  KINLINE void ShallowCopyDelete(ring new_tailRing, omBin new_tailBin,
                                 pShallowCopyDeleteProc p_shallow_copy_delete);

  // manipulations
  KINLINE void pNorm();
  KINLINE void pCleardenom();
  
#ifdef KDEBUG
  void wrp();
#endif
  
};

class sLObject : public sTObject
{

public:
  poly  p1,p2; /*- the pair p comes from -*/
  
  poly  lcm;   /*- the lcm of p1,p2 -*/
  kBucket_pt bucket;

  // initialization
  KINLINE void Init(ring tailRing = currRing);
  KINLINE sLObject(ring tailRing = currRing);
  KINLINE sLObject(poly p, ring tailRing = currRing);
  KINLINE sLObject(poly p, ring c_r, ring tailRing);

  // Iterations
  KINLINE void LmDeleteAndIter();

  // spoly related things
  KINLINE void SetLmTail(poly lm, poly new_p, int use_bucket, ring r);
  KINLINE void Tail_Minus_mm_Mult_qq(poly m, poly qq, int lq, poly spNoether);
  KINLINE void Tail_Mult_nn(number n);
  KINLINE poly GetP(omBin lmBin = NULL);
  KINLINE void CanonicalizeP();

  KINLINE void ShallowCopyDelete(ring new_tailRing,  
                                 pShallowCopyDeleteProc p_shallow_copy_delete);
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
  BOOLEAN use_buckets;
  ring tailRing;
  omBin lmBin;
  omBin tailBin;
  /*FALSE, if posInL == posInL10*/
  char    redTailChange;
  char    news;
  char    newt;/*used for messageSets*/

  KINLINE skStrategy();
  KINLINE ~skStrategy();
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
KINLINE poly redtailBba (poly p,int pos,kStrategy strat);
poly redtailBba (LObject *L, int pos,kStrategy strat);
poly redtailSyz (poly p,int pos,kStrategy strat);
poly redtail (poly p,int pos,kStrategy strat);
void enterpairs (poly h, int k, int ec, int pos,kStrategy strat);
void entersets (LObject h);
void pairs ();
void message (int i,int* reduc,int* olddeg,kStrategy strat);
void messageStat (int srmax,int lrmax,int hilbcount,kStrategy strat);
#ifdef KDEBUG
void messageSets (kStrategy strat);
#else
#define messageSets(s)  ((void) 0)
#endif

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

/***************************************************************
 *
 * stuff to be inlined
 *
 ***************************************************************/

KINLINE TSet initT ();
KINLINE poly k_LmInit_currRing_2_tailRing(poly p, ring tailRing, omBin bin);
KINLINE poly k_LmInit_tailRing_2_currRing(poly p, ring tailRing, omBin bin);
KINLINE poly k_LmShallowCopyDelete_currRing_2_tailRing(poly p, ring tailRing, omBin bin);
KINLINE poly k_LmShallowCopyDelete_tailRing_2_currRing(poly p, ring tailRing,  omBin bin);

KINLINE poly k_LmInit_currRing_2_tailRing(poly p, ring tailRing);
KINLINE poly k_LmInit_tailRing_2_currRing(poly p, ring tailRing);
KINLINE poly k_LmShallowCopyDelete_currRing_2_tailRing(poly p, ring tailRing);
KINLINE poly k_LmShallowCopyDelete_tailRing_2_currRing(poly p, ring tailRing);

// if exp bound is not violated, return TRUE and
//                               get m1 = LCM(LM(p1), LM(p2))/LM(p1)
//                                   m2 = LCM(LM(p1), LM(p2))/LM(p2)
// return FALSE and m1 == NULL, m2 == NULL     , otherwise 
KINLINE BOOLEAN k_GetLeadTerms(const poly p1, const poly p2, const ring p_r, 
                               poly &m1, poly &m2, const ring m_r);
#ifdef KDEBUG
// test strat
BOOLEAN kTest(kStrategy strat);
// test strat, and test that S is contained in T
BOOLEAN kTest_TS(kStrategy strat);
// test LObject 
BOOLEAN kTest_L(LObject* L, ring tailRing = NULL,
                 BOOLEAN testp = FALSE, int lpos = -1,
                 TSet T = NULL, int tlength = -1);
// test TObject
BOOLEAN kTest_T(TObject* T, ring tailRing = NULL, int tpos = -1, char T = '?');
// test set strat->SevS
BOOLEAN kTest_S(kStrategy strat);
#else
#define kTest(A)        ((void)0)
#define kTest_TS(A)     ((void)0)
#define kTest_T(T)      ((void)0)
#define kTest_S(T)      ((void)0)
#define kTest_L(T)      ((void)0)
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
BOOLEAN ksReducePoly(LObject* PR,
                     TObject* PW,
                     poly spNoether = NULL,
                     number *coef = NULL, 
                     kStrategy strat = NULL);

// Reduces PR at Current->next with PW
// Assumes PR != NULL, Current contained in PR
//         Current->next != NULL, LM(PW) devides LM(Current->next)
// Changes: PR
// Const:   PW
BOOLEAN ksReducePolyTail(LObject* PR,
                         TObject* PW,
                         poly Current,
                         poly spNoether = NULL);

// Creates S-Poly of Pair
// Const:   Pair->p1, Pair->p2
// Changes: Pair->p == S-Poly of p1, p2
// Assume:  Pair->p1 != NULL && Pair->p2
void ksCreateSpoly(LObject* Pair, poly spNoether = NULL, 
                   int use_buckets=0, ring tailRing=currRing, 
                   poly m1 = NULL, poly m2 = NULL);


/*2
* creates the leading term of the S-polynomial of p1 and p2
* do not destroy p1 and p2
* remarks:
*   1. the coefficient is 0 (nNew)
*   2. pNext is undefined
*/
poly ksCreateShortSpoly(poly p1, poly p2, ring tailRing);


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
KINLINE poly ksOldSpolyRed(poly p1, poly p2, poly spNoether = NULL);
KINLINE poly ksOldSpolyRedNew(poly p1, poly p2, poly spNoether = NULL);
KINLINE poly ksOldCreateSpoly(poly p1, poly p2, poly spNoether = NULL, ring r = currRing);
KINLINE void ksOldSpolyTail(poly p1, poly q, poly q2, poly spNoether, ring r = currRing);

/***************************************************************
 *
 * Routines related for ring changes during std computations
 *
 ***************************************************************/
// return TRUE and set m1, m2 to k_GetLcmTerms, 
//             if spoly creation of strat->P does not violate 
//             exponent bound of strat->tailRing
//      FALSE, otherwise
BOOLEAN kCheckSpolyCreation(kStrategy strat, poly &m1, poly &m2);
// change the strat such that strat strat->tailRing == new_tailRing
void kStratChangeTailRing(kStrategy strat, ring new_tailRing);
// initiate a change of the tailRing of strat -- should be called
// right before main loop in bba
void kStratInitChangeTailRing(kStrategy strat);

#include "kInline.cc"


#endif
