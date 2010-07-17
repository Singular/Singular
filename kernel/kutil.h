#ifndef KUTIL_H
#define KUTIL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: kernel: utils for kStd
*/


#include <string.h>
#include <mylimits.h>

#include <kernel/structs.h>
#include <omalloc.h>
#include <kernel/ring.h>
#include <kernel/structs.h>
#include <kernel/pShallowCopyDelete.h>

#if 1
#define setmax 16
#define setmaxL ((4096-12)/sizeof(LObject))
#define setmaxLinc ((4096)/sizeof(LObject))

#define setmaxT 64
#define setmaxTinc 32
#else
#define setmax 16
#define setmaxL 16
#define setmaxLinc 16
#define setmaxT 16
#define setmaxTinc 16
#endif

// define if you want std computations as in Singular version < 2
// This disbales RedThrough, tailReductions against T (bba),
// sets posInT = posInT15 (bba, strat->honey), and enables redFirst with LDeg
// NOTE: the same effect can be achieved with option(oldStd)
// #define HAVE_OLD_STD

#ifdef HAVE_OLD_STD
#define K_TEST_OPT_REDTHROUGH 0
#define K_TEST_OPT_OLDSTD 1
#else
#define K_TEST_OPT_REDTHROUGH TEST_OPT_REDTHROUGH
#define K_TEST_OPT_OLDSTD     TEST_OPT_OLDSTD
#endif

#undef NO_KINLINE
#if !defined(KDEBUG) && !defined(NO_INLINE)
#define KINLINE inline
#else
#define KINLINE
#define NO_KINLINE 1
#endif

typedef int* intset;
typedef int64  wlen_type;
typedef wlen_type* wlen_set;

typedef class sTObject TObject;
typedef class sLObject LObject;
typedef TObject * TSet;
typedef LObject * LSet;

class sTObject
{
public:
  poly p;       // Lm(p) \in currRing Tail(p) \in tailRing
  poly t_p;     // t_p \in tailRing: as monomials Lm(t_p) == Lm(p)
  poly max;     // p_GetMaxExpP(pNext(p))
  ring tailRing;
  long FDeg;    // pFDeg(p)
  int ecart,
    length,     // as of pLDeg
    pLength,    // either == 0, or == pLength(p)
    i_r;        // index of TObject in R set, or -1 if not in T
  BOOLEAN is_normalized; // true, if pNorm was called on p, false otherwise

  // initialization
  KINLINE void Init(ring r = currRing);
  KINLINE sTObject(ring tailRing = currRing);
  KINLINE sTObject(poly p, ring tailRing = currRing);
  KINLINE sTObject(poly p, ring c_r, ring tailRing);
  KINLINE sTObject(sTObject* T, int copy);

  KINLINE void Set(ring r=currRing);
  KINLINE void Set(poly p_in, ring r=currRing);
  KINLINE void Set(poly p_in, ring c_r, ring t_r);

  // Frees the polys of T
  KINLINE void Delete();
  // Sets polys to NULL
  KINLINE void Clear();
  // makes a copy of the poly of T
  KINLINE void Copy();

  // ring-dependent Lm access: these might result in allocation of monomials
  KINLINE poly GetLmCurrRing();
  KINLINE poly GetLmTailRing();
  KINLINE poly GetLm(ring r);
  // this returns Lm and ring r (preferably from tailRing), but does not
  // allocate a new poly
  KINLINE void GetLm(poly &p, ring &r) const;

#ifdef OLIVER_PRIVAT_LT
  // routines for calc. with rings
  KINLINE poly GetLtCurrRing();
  KINLINE poly GetLtTailRing();
  KINLINE poly GetLt(ring r);
  KINLINE void GetLt(poly &p, ring &r) const;
#endif

  KINLINE BOOLEAN IsNull() const;

  KINLINE int GetpLength();

  // makes sure that T.p exists
  KINLINE void SetLmCurrRing();

  // Iterations
  // simply get the next monomial
  KINLINE poly Next();
  KINLINE void LmDeleteAndIter();

  // deg stuff
  // compute pTotalDegree
  KINLINE long pTotalDeg() const;
  // computes pFDeg
  KINLINE long pFDeg() const;
  // computes and sets FDeg
  KINLINE long SetpFDeg();
  // gets stored FDeg
  KINLINE long GetpFDeg() const;

  // computes pLDeg
  KINLINE long pLDeg();
  // sets length, FDeg, returns LDeg
  KINLINE long SetDegStuffReturnLDeg();

  // arithmetic
  KINLINE void Mult_nn(number n);
  KINLINE void ShallowCopyDelete(ring new_tailRing, omBin new_tailBin,
                                 pShallowCopyDeleteProc p_shallow_copy_delete,
                                 BOOLEAN set_max = TRUE);
  // manipulations
  KINLINE void pNorm();
  KINLINE void pCleardenom();

#ifdef KDEBUG
  void wrp();
#endif
};

#ifndef NDEBUG
extern int strat_nr;
extern int strat_fac_debug;
#endif

class sLObject : public sTObject
{

public:
  unsigned long sev;
  poly  p1,p2; /*- the pair p comes from,
                 lm(pi) in currRing, tail(pi) in tailring -*/

  poly  lcm;   /*- the lcm of p1,p2 -*/
  poly last;   // pLast(p) during reductions
  kBucket_pt bucket;
  int   i_r1, i_r2;

  // initialization
  KINLINE void Init(ring tailRing = currRing);
  KINLINE sLObject(ring tailRing = currRing);
  KINLINE sLObject(poly p, ring tailRing = currRing);
  KINLINE sLObject(poly p, ring c_r, ring tailRing);

  // Frees the polys of L
  KINLINE void Delete();
  KINLINE void Clear();

  // Iterations
  KINLINE void LmDeleteAndIter();
  KINLINE poly LmExtractAndIter();

  // spoly related things
  // preparation for reduction if not spoly
  KINLINE void PrepareRed(BOOLEAN use_bucket);
  KINLINE void SetLmTail(poly lm, poly new_p, int length,
                         int use_bucket, ring r, poly last);
  KINLINE void Tail_Minus_mm_Mult_qq(poly m, poly qq, int lq, poly spNoether);
  KINLINE void Tail_Mult_nn(number n);
  // deletes bucket, makes sure that p and t_p exists
  KINLINE poly GetP(omBin lmBin = NULL);
  // similar, except that only t_p exists
  KINLINE poly GetTP();

  // does not delete bucket, just canonicalizes it
  // returned poly is such that Lm(p) \in currRing, Tail(p) \in tailRing
  KINLINE poly CanonicalizeP();

  // makes a copy of the poly of L
  KINLINE void Copy();
  // gets the poly and makes a copy of it
  KINLINE poly CopyGetP();

  KINLINE int GetpLength();
  KINLINE long pLDeg(BOOLEAN use_last);
  KINLINE long pLDeg();
  KINLINE int SetLength(BOOLEAN lengt_pLength = FALSE);
  KINLINE long SetDegStuffReturnLDeg();
  KINLINE long SetDegStuffReturnLDeg(BOOLEAN use_last);

  // returns minimal component of p
  KINLINE long MinComp();
  // returns component of p
  KINLINE long Comp();

  KINLINE void ShallowCopyDelete(ring new_tailRing,
                                 pShallowCopyDeleteProc p_shallow_copy_delete);

  // sets sev
  KINLINE void SetShortExpVector();

  // enable assignment from TObject
  KINLINE sLObject& operator=(const sTObject&);

  // get T's corresponding to p1, p2: they might return NULL
  KINLINE TObject* T_1(const skStrategy* strat);
  KINLINE TObject* T_2(const skStrategy* strat);
  KINLINE void     T_1_2(const skStrategy* strat,
                         TObject* &T_1, TObject* &T_2);

  // simplify coefficients
  KINLINE void Normalize();
  KINLINE void HeadNormalize();
};


extern int HCord;

class skStrategy;
typedef skStrategy * kStrategy;
class skStrategy
{
public:
  kStrategy next;
  int (*red)(LObject * L,kStrategy strat);
  void (*initEcart)(LObject * L);
  int (*posInT)(const TSet T,const int tl,LObject &h);
  int (*posInL)(const LSet set, const int length,
                LObject* L,const kStrategy strat);
  void (*enterS)(LObject h, int pos,kStrategy strat, int atR/* =-1*/ );
  void (*initEcartPair)(LObject * h, poly f, poly g, int ecartF, int ecartG);
  int (*posInLOld)(const LSet Ls,const int Ll,
                   LObject* Lo,const kStrategy strat);
  void (*enterOnePair) (int i,poly p,int ecart, int isFromQ,kStrategy strat, int atR /*= -1*/);
  void (*chainCrit) (poly p,int ecart,kStrategy strat);
  pFDegProc pOrigFDeg;
  pLDegProc pOrigLDeg;
  pFDegProc pOrigFDeg_TailRing;
  pLDegProc pOrigLDeg_TailRing;

  LObject P;
  ideal Shdl;
  ideal D; /*V(S) is in D(D)*/
  ideal M; /*set of minimal generators*/
  polyset S;
  intset ecartS;
  intset lenS;
  wlen_set lenSw; /* for tgb.ccc */
  intset fromQ;
  unsigned long* sevS;
  unsigned long* sevT;
  TSet T;
  LSet L;
  LSet    B;
  poly    kHEdge;
  poly    kNoether;
  poly    t_kHEdge; // same polys in tailring
  KINLINE poly    kNoetherTail();
  poly    t_kNoether;
  BOOLEAN * NotUsedAxis;
  BOOLEAN * pairtest;/*used for enterOnePair*/
  poly tail;
  leftv kIdeal;
  intvec * kModW;
  intvec * kHomW;
  // procedure for ShalloCopy from tailRing  to currRing
  pShallowCopyDeleteProc p_shallow_copy_delete;
  // pointers to Tobjects R[i] is ith Tobject which is generated
  TObject**  R;
  // S_2_R[i] yields Tobject which corresponds to S[i]
  int*      S_2_R;
  ring tailRing;
  omBin lmBin;
  omBin tailBin;
#ifndef NDEBUG
  int nr;
#endif
  int cp,c3;
  int cv; // in shift bases: counting V criterion
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
  #ifdef HAVE_SHIFTBBA
  int lV;
  #endif
  BOOLEAN interpt;
  BOOLEAN homog;
#ifdef HAVE_PLURAL
  BOOLEAN z2homog; // Z_2 - homogeneous input allows product criterion in commutative and SCA cases!
#endif
  BOOLEAN kHEdgeFound;
  BOOLEAN honey,sugarCrit;
  BOOLEAN Gebauer,noTailReduction;
  BOOLEAN fromT;
  BOOLEAN noetherSet;
  BOOLEAN update;
  BOOLEAN posInLOldFlag;
  BOOLEAN use_buckets;
  BOOLEAN interred_flag;
  // if set, pLDeg(p, l) == (pFDeg(pLast(p), pLength)
  BOOLEAN LDegLast;
  // if set, then L.length == L.pLength
  BOOLEAN length_pLength;
  // if set, then posInL does not depend on L.length
  BOOLEAN posInLDependsOnLength;
  /*FALSE, if posInL == posInL10*/
#ifdef HAVE_PLURAL
  // set this flag to 1 to stop the product criteria
  // use ALLOW_PROD_CRIT(strat) to test
  BOOLEAN no_prod_crit;
#define ALLOW_PROD_CRIT(A) (!(A)->no_prod_crit)
#else
#define ALLOW_PROD_CRIT(A) (1)
#endif
  char    redTailChange;
  char    news;
  char    newt;/*used for messageSets*/
  char    noClearS;
  char    completeReduce_retry;
  char    overflow;

  skStrategy();
  ~skStrategy();

  // return TObject corresponding to S[i]: assume that it exists
  // i.e. no error checking is done
  KINLINE TObject* S_2_T(int i);
  // like S_2_T, except that NULL is returned if it can not be found
  KINLINE TObject* s_2_t(int i);
};

void deleteHC(poly *p, int *e, int *l, kStrategy strat);
void deleteHC(LObject* L, kStrategy strat, BOOLEAN fromNext = FALSE);
void deleteInS (int i,kStrategy strat);
void cleanT (kStrategy strat);
static inline LSet initL (int nr=setmaxL)
{ return (LSet)omAlloc(nr*sizeof(LObject)); };
void deleteInL(LSet set, int *length, int j,kStrategy strat);
void enterL (LSet *set,int *length, int *LSetmax, LObject p,int at);
void enterSBba (LObject p,int atS,kStrategy strat, int atR = -1);
void initEcartPairBba (LObject* Lp,poly f,poly g,int ecartF,int ecartG);
void initEcartPairMora (LObject* Lp,poly f,poly g,int ecartF,int ecartG);
int posInS (const kStrategy strat, const int length, const poly p, 
            const int ecart_p);
int posInT0 (const TSet set,const int length,LObject &p);
int posInT1 (const TSet set,const int length,LObject &p);
int posInT2 (const TSet set,const int length,LObject &p);
int posInT11 (const TSet set,const int length,LObject &p);
int posInT110 (const TSet set,const int length,LObject &p);
int posInT13 (const TSet set,const int length,LObject &p);
int posInT15 (const TSet set,const int length,LObject &p);
int posInT17 (const TSet set,const int length,LObject &p);
int posInT19 (const TSet set,const int length,LObject &p);
int posInT_EcartpLength(const TSet set,const int length,LObject &p);

#ifdef HAVE_MORE_POS_IN_T
int posInT_EcartFDegpLength(const TSet set,const int length,LObject &p);
int posInT_FDegpLength(const TSet set,const int length,LObject &p);
int posInT_pLength(const TSet set,const int length,LObject &p);
#endif


void reorderS (int* suc,kStrategy strat);
int posInL0 (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL11 (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL13 (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL15 (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL17 (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL10 (const LSet set, const int length,
             LObject* L,const kStrategy strat);
KINLINE poly redtailBba (poly p,int pos,kStrategy strat,BOOLEAN normalize=FALSE);
#ifdef HAVE_RINGS
KINLINE poly redtailBba_Z (poly p,int pos,kStrategy strat);
poly redtailBba_Z (LObject* L, int pos, kStrategy strat );
#endif
poly redtailBba (LObject *L, int pos,kStrategy strat,
                 BOOLEAN withT = FALSE,BOOLEAN normalize=FALSE);
poly redtailBba (TObject *T, int pos,kStrategy strat);
poly redtail (poly p,int pos,kStrategy strat);
poly redtail (LObject *L,int pos,kStrategy strat);
poly redNF (poly h,int & max_ind,int nonorm,kStrategy strat);
int redNF0 (LObject *P,kStrategy strat);
poly redNFTail (poly h,const int sl,kStrategy strat);
int redHoney (LObject* h, kStrategy strat);
#ifdef HAVE_RINGS
int redRing (LObject* h,kStrategy strat);
void enterExtendedSpoly(poly h,kStrategy strat);
void superenterpairs (poly h,int k,int ecart,int pos,kStrategy strat, int atR = -1);
poly kCreateZeroPoly(long exp[], long cabsind, poly* t_p, ring leadRing, ring tailRing);
long ind2(long arg);

long ind_fact_2(long arg);
long twoPow(long arg);
ideal createG0();
#endif
int redLazy (LObject* h,kStrategy strat);
int redHomog (LObject* h,kStrategy strat);
void enterpairs (poly h, int k, int ec, int pos,kStrategy strat, int atR = -1);
void entersets (LObject h);
void pairs ();
void message (int i,int* reduc,int* olddeg,kStrategy strat,int red_result);
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
void enterT (LObject p,kStrategy strat, int atT = -1);
void cancelunit (LObject* p,BOOLEAN inNF=FALSE);
void HEckeTest (poly pp,kStrategy strat);
void initBuchMoraCrit(kStrategy strat);
void initHilbCrit(ideal F, ideal Q, intvec **hilb,kStrategy strat);
void initBuchMoraPos(kStrategy strat);
void initBuchMora (ideal F, ideal Q,kStrategy strat);
void exitBuchMora (kStrategy strat);
void updateResult(ideal r,ideal Q,kStrategy strat);
void completeReduce (kStrategy strat, BOOLEAN withT=FALSE);
void kFreeStrat(kStrategy strat);
void enterOnePairNormal (int i,poly p,int ecart, int isFromQ,kStrategy strat, int atR);
void chainCritNormal (poly p,int ecart,kStrategy strat);
BOOLEAN homogTest(polyset F, int Fmax);
BOOLEAN newHEdge(polyset S, kStrategy strat);
// returns index of p in TSet, or -1 if not found
int kFindInT(poly p, TSet T, int tlength);

// return -1 if no divisor is found
//        number of first divisor, otherwise
int kFindDivisibleByInT(const TSet &T, const unsigned long* sevT,
                        const int tl, const LObject* L, const int start=0);
// same with S
int kFindDivisibleByInS(const kStrategy strat, int *max_ind, LObject* L);

int kFindNextDivisibleByInS(const kStrategy strat, int start,int max_ind, LObject* L);
TObject*
kFindDivisibleByInS(kStrategy strat, int pos, LObject* L, TObject *T,
                    long ecart = LONG_MAX);

/***************************************************************
 *
 * stuff to be inlined
 *
 ***************************************************************/

KINLINE TSet initT ();
KINLINE TObject** initR();
KINLINE unsigned long* initsevT();
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
#ifdef HAVE_RINGS
KINLINE void k_GetStrongLeadTerms(const poly p1, const poly p2, const ring leadRing,
                               poly &m1, poly &m2, poly &lcm, const ring taiRing);
#endif
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
BOOLEAN kTest_T(TObject* T, ring tailRing = NULL, int tpos = -1, char TN = '?');
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
poly kFindZeroPoly(poly input_p, ring leadRing, ring tailRing);
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
// If strat != NULL, tailRing is changed if reduction would violate exp bound
// of tailRing
// Returns: 0 everything ok, no tailRing change
//          1 tailRing has successfully changed (strat != NULL)
//          2 no reduction performed, tailRing needs to be changed first
//            (strat == NULL)
//         -1 tailRing change could not be performed due to exceeding exp
//            bound of currRing
int ksReducePoly(LObject* PR,
                 TObject* PW,
                 poly spNoether = NULL,
                 number *coef = NULL,
                 kStrategy strat = NULL);

// Reduces PR at Current->next with PW
// Assumes PR != NULL, Current contained in PR
//         Current->next != NULL, LM(PW) devides LM(Current->next)
// Changes: PR
// Const:   PW
// Return: see ksReducePoly
int ksReducePolyTail(LObject* PR,
                     TObject* PW,
                     poly Current,
                     poly spNoether = NULL);

KINLINE int ksReducePolyTail(LObject* PR, TObject* PW, LObject* Red);

// Creates S-Poly of Pair
// Const:   Pair->p1, Pair->p2
// Changes: Pair->p == S-Poly of p1, p2
// Assume:  Pair->p1 != NULL && Pair->p2
void ksCreateSpoly(LObject* Pair, poly spNoether = NULL,
                   int use_buckets=0, ring tailRing=currRing,
                   poly m1 = NULL, poly m2 = NULL, TObject** R = NULL);

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
BOOLEAN kCheckSpolyCreation(LObject* L, kStrategy strat, poly &m1, poly &m2);
#ifdef HAVE_RINGS
// return TRUE if gcdpoly creation of R[atR] and S[atS] does not violate
//             exponent bound of strat->tailRing
//      FALSE, otherwise
BOOLEAN kCheckStrongCreation(int atR, poly m1, int atS, poly m2, kStrategy strat);
#endif
// change strat->tailRing and adjust all data in strat, L, and T:
// new tailRing has larger exponent bound
// do nothing and return FALSE if exponent bound increase would result in
// larger exponent bound that that of currRing
BOOLEAN kStratChangeTailRing(kStrategy strat,
                             LObject* L = NULL, TObject* T = NULL,
                             // take this as new_expbound: if 0
                             // new expbound is 2*expbound of tailRing
                             unsigned long new_expbound = 0);
// initiate a change of the tailRing of strat -- should be called
// right before main loop in bba
void kStratInitChangeTailRing(kStrategy strat);

KINLINE void clearS (poly p, unsigned long p_sev, int* at, int* k,
  kStrategy strat);
#include <kernel/kInline.cc>

/* shiftgb stuff */
#include <kernel/shiftgb.h>

poly pMove2CurrTail(poly p, kStrategy strat);

poly pMoveCurrTail2poly(poly p, kStrategy strat);

poly pCopyL2p(LObject h, kStrategy strat);

void enterTShift(LObject p, kStrategy strat, int atT, int uptodeg, int lV);

void initBuchMoraShift (ideal F,ideal Q,kStrategy strat);

void enterOnePairManyShifts (int i, poly p, int ecart, int isFromQ, kStrategy strat, int atR, int uptodeg, int lV); // ok

void enterOnePairSelfShifts (poly qq, poly p, int ecart, int isFromQ, kStrategy strat, int atR, int uptodeg, int lV);

void enterOnePairShift (poly q, poly p, int ecart, int isFromQ, kStrategy strat, int atR, int ecartq, int qisFromQ, int shiftcount, int ifromS, int uptodeg, int lV); // ok

void enterpairsShift (poly h,int k,int ecart,int pos,kStrategy strat, int atR,int uptodeg, int lV);

void initenterpairsShift (poly h,int k,int ecart,int isFromQ,kStrategy strat, int atR,int uptodeg, int lV); 

void updateSShift(kStrategy strat,int uptodeg,int lV);

void initBbaShift(ideal F,kStrategy strat);

poly redtailBbaShift (LObject* L, int pos, kStrategy strat, BOOLEAN withT, BOOLEAN normalize);

int redFirstShift (LObject* h,kStrategy strat); // ok

ideal freegb(ideal I, int uptodeg, int lVblock);

ideal bbaShift(ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat, int uptodeg, int lV);
// test syz strategy: // will be removed soon
extern  int (*test_PosInT)(const TSet T,const int tl,LObject &h);
extern  int (*test_PosInL)(const LSet set, const int length,
                LObject* L,const kStrategy strat);
#endif
