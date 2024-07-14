#ifndef KUTIL_H
#define KUTIL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: kernel: utils for kStd
*/


#include <string.h>

#include "omalloc/omalloc.h"
#ifdef HAVE_OMALLOC
#include "omalloc/omallocClass.h"
#endif

#include "misc/mylimits.h"

#include "kernel/polys.h"
#include "polys/operations/pShallowCopyDelete.h"

#include "kernel/structs.h"
#include "kernel/GBEngine/kstd1.h"   /* for s_poly_proc_t */

// define if tailrings should be used
#define HAVE_TAIL_RING

#define setmax 128
#define setmaxL ((4096-12)/sizeof(LObject))
#define setmaxLinc ((4096)/sizeof(LObject))

#define setmaxT ((4096-12)/sizeof(TObject))
#define setmaxTinc ((4096)/sizeof(TObject))

#define RED_CANONICALIZE 200
#define REDNF_CANONICALIZE 60
#define REDTAIL_CANONICALIZE 100

// if you want std computations as in Singular version < 2:
// This disables RedThrough, tailReductions against T (bba),
// sets posInT = posInT15 (bba, strat->honey), and enables redFirst with LDeg
// NOTE: can be achieved with option(oldStd)

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

typedef struct denominator_list_s denominator_list_s;
typedef denominator_list_s *denominator_list;

struct denominator_list_s{number n; denominator_list next;};
EXTERN_VAR denominator_list DENOMINATOR_LIST;

class sTObject
{
public:
  unsigned long sevSig;
  poly sig;   // the signature of the element
  poly p;       // Lm(p) \in currRing Tail(p) \in tailRing
  poly t_p;     // t_p \in tailRing: as monomials Lm(t_p) == Lm(p)
  poly max_exp;     // p_GetMaxExpP(pNext(p))
  ring tailRing;
  long FDeg;    // pFDeg(p)
  int ecart,
    length,     // as of pLDeg
    pLength,    // either == 0, or == pLength(p)
    i_r;        // index of TObject in R set, or -1 if not in T

#ifdef HAVE_SHIFTBBA
  int shift;
#endif

  /*BOOLEAN*/ char is_normalized; // true, if pNorm was called on p, false otherwise
  // used in incremental sba() with F5C:
  // we know some of the redundant elements in
  // strat->T beforehand, so we can just discard
  // them and do not need to consider them in the
  // interreduction process
  /*BOOLEAN*/ char is_redundant;
  // used in sba's sig-safe reduction:
  // sometimes we already know that a reducer
  // is sig-safe, so no need for a real
  // sig-safeness check
  /*BOOLEAN*/ char is_sigsafe;


#ifdef HAVE_PLURAL
  /*BOOLEAN*/ char is_special; // true, it is a new special S-poly (e.g. for SCA)
#endif

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
  KINLINE void pContent();

#ifdef KDEBUG
  void wrp();
#endif
};

EXTERN_VAR int strat_nr;

class sLObject : public sTObject
{

public:
  unsigned long sev;
  poly  p1,p2; /*- the pair p comes from,
                 lm(pi) in currRing, tail(pi) in tailring -*/

  poly  lcm;   /*- the lcm of p1,p2 -*/
  kBucket_pt bucket;
  int   i_r1, i_r2;
  unsigned checked; // this is the index of S up to which
                      // the corresponding LObject was already checked in
                      // critical pair creation => when entering the
                      // reduction process it is enough to start a second
                      // rewritten criterion check from checked+1 onwards
  BOOLEAN prod_crit;
                      // NOTE: If prod_crit = TRUE then the corresponding pair is
                      // detected by Buchberger's Product Criterion and can be
                      // deleted

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
                         int use_bucket, ring r);
  KINLINE void Tail_Minus_mm_Mult_qq(poly m, poly qq, int lq, poly spNoether);
  KINLINE void Tail_Mult_nn(number n);
  // deletes bucket, makes sure that p and t_p exists
  KINLINE poly GetP(omBin lmBin = (omBin)NULL);
  // similar, except that only t_p exists
  KINLINE poly GetTP();

  // does not delete bucket, just canonicalizes it
  // returned poly is such that Lm(p) \in currRing, Tail(p) \in tailRing
  KINLINE void CanonicalizeP();

  // makes a copy of the poly of L
  KINLINE void Copy();

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

EXTERN_VAR int HCord;

class skStrategy
#ifdef HAVE_OMALLOC
                 : public omallocClass
#endif
{
public:
  kStrategy next;
  int (*red)(LObject * L,kStrategy strat);
  int (*red2)(LObject * L,kStrategy strat);
  void (*initEcart)(TObject * L);
  int (*posInT)(const TSet T,const int tl,LObject &h);
  int (*posInLSba)(const LSet set, const int length,
                LObject* L,const kStrategy strat);
  int (*posInL)(const LSet set, const int length,
                LObject* L,const kStrategy strat);
  void (*enterS)(LObject &h, int pos,kStrategy strat, int atR/* =-1*/ );
  void (*initEcartPair)(LObject * h, poly f, poly g, int ecartF, int ecartG);
  int (*posInLOld)(const LSet Ls,const int Ll,
                   LObject* Lo,const kStrategy strat);
  void (*enterOnePair) (int i,poly p,int ecart, int isFromQ,kStrategy strat, int atR /*= -1*/);
  void (*chainCrit) (poly p,int ecart,kStrategy strat);
  BOOLEAN (*syzCrit) (poly sig, unsigned long not_sevSig, kStrategy strat);
  BOOLEAN (*rewCrit1) (poly sig, unsigned long not_sevSig, poly lm, kStrategy strat, int start /*= 0*/);
  BOOLEAN (*rewCrit2) (poly sig, unsigned long not_sevSig, poly lm, kStrategy strat, int start /*= 0*/);
  BOOLEAN (*rewCrit3) (poly sig, unsigned long not_sevSig, poly lm, kStrategy strat, int start /*= 0*/);
  pFDegProc pOrigFDeg;
  pLDegProc pOrigLDeg;
  pFDegProc pOrigFDeg_TailRing;
  pLDegProc pOrigLDeg_TailRing;
  s_poly_proc_t s_poly;

  LObject P;
  ideal Shdl;
  ideal D; /*V(S) is in D(D)*/
  ideal M; /*set of minimal generators*/
  polyset S;
  polyset syz;
  polyset sig;
  intset ecartS;
  intset fromS; // from which S[i] S[j] comes from
                // this is important for signature-based
                // algorithms
  intset syzIdx;// index in the syz array at which the first
                // syzygy of component i comes up
                // important for signature-based algorithms
  unsigned sbaOrder;
  int currIdx;
  int max_lower_index;
  intset lenS;
  wlen_set lenSw; /* for tgb.ccc */
  intset fromQ;
  unsigned long* sevS;
  unsigned long* sevSyz;
  unsigned long* sevSig;
  unsigned long* sevT;
  TSet T;
  LSet L;
  LSet    B;
  poly    kNoether;
  poly    t_kNoether; // same polys in tailring
  KINLINE poly    kNoetherTail();
  BOOLEAN * NotUsedAxis;
  BOOLEAN * pairtest;/*used for enterOnePair*/
  poly tail;
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
  int nr;
  int cp,c3;
  int sl,mu;
  int syzl,syzmax,syzidxmax;
  int tl,tmax;
  int Ll,Lmax;
  int Bl,Bmax;
  int ak,LazyDegree,LazyPass;
  int syzComp;
  int lastAxis;
  int newIdeal;
  int minim;
  bool sigdrop; //This is used to check sigdrop in sba over Z
  int nrsyzcrit; // counts how many pairs are deleted by SyzCrit
  int nrrewcrit; // counts how many pairs are deleted by FaugereRewCrit
  int sbaEnterS; // sba over Z strategy: if sigdrop element has _*gen(sbaEnterS+1), then
                 // add directly sbaEnterS elements into S
  int blockred;  // counter for blocked reductions in redSig
  int blockredmax;
  #ifdef HAVE_SHIFTBBA
  int cv; // in shift bases: counting V criterion
  /*BOOLEAN*/ char rightGB;
  #endif
  /*BOOLEAN*/ char interpt;
  /*BOOLEAN*/ char homog;
#ifdef HAVE_PLURAL
  /*BOOLEAN*/ char z2homog; // Z_2 - homogeneous input allows product criterion in commutative and SCA cases!
#endif
  /*BOOLEAN*/ char kAllAxis; // all axis are used -> (re)compute noether
  /*BOOLEAN*/ char honey,sugarCrit;
  /*BOOLEAN*/ char Gebauer,noTailReduction;
  /*BOOLEAN*/ char fromT;
  /*BOOLEAN*/ char noetherSet;
  /*BOOLEAN*/ char update;
  /*BOOLEAN*/ char posInLOldFlag;
  /*BOOLEAN*/ char use_buckets;
  // if set, pLDeg(p, l) == (pFDeg(pLast(p), pLength)
  /*BOOLEAN*/ char LDegLast;
  // if set, then L.length == L.pLength
  /*BOOLEAN*/ char length_pLength;
  // if set, then posInL does not depend on L.length
  /*BOOLEAN*/ char posInLDependsOnLength;
  /*FALSE, if posInL == posInL10*/
#ifdef HAVE_PLURAL
  // set this flag to 1 to stop the product criteria
  // use ALLOW_PROD_CRIT(strat) to test
  /*BOOLEAN*/ char no_prod_crit;
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
{ return (LSet)omAlloc(nr*sizeof(LObject)); }
void deleteInL(LSet set, int *length, int j,kStrategy strat);
void enterL (LSet *set,int *length, int *LSetmax, LObject p,int at);
void enterSBba (LObject &p,int atS,kStrategy strat, int atR = -1);
void enterSBbaShift (LObject &p,int atS,kStrategy strat, int atR = -1);
void enterSSba (LObject &p,int atS,kStrategy strat, int atR = -1);
void initEcartPairBba (LObject* Lp,poly f,poly g,int ecartF,int ecartG);
void initEcartPairMora (LObject* Lp,poly f,poly g,int ecartF,int ecartG);
int posInS (const kStrategy strat, const int length, const poly p,
            const int ecart_p);
int posInSMonFirst (const kStrategy strat, const int length, const poly p);
int posInIdealMonFirst (const ideal F, const poly p,int start = 0,int end = -1);
int posInT0 (const TSet set,const int length,LObject &p);
int posInT1 (const TSet set,const int length,LObject &p);
int posInT2 (const TSet set,const int length,LObject &p);
int posInT11 (const TSet set,const int length,LObject &p);
int posInTSig (const TSet set,const int length,LObject &p);
int posInT110 (const TSet set,const int length,LObject &p);
int posInT13 (const TSet set,const int length,LObject &p);
int posInT15 (const TSet set,const int length,LObject &p);
int posInT17 (const TSet set,const int length,LObject &p);
int posInT17_c (const TSet set,const int length,LObject &p);
int posInT19 (const TSet set,const int length,LObject &p);
int posInT_EcartpLength(const TSet set,const int length,LObject &p);
int posInT_EcartFDegpLength(const TSet set,const int length,LObject &p);
int posInT_FDegpLength(const TSet set,const int length,LObject &p);
int posInT_pLength(const TSet set,const int length,LObject &p);

#ifdef HAVE_MORE_POS_IN_T
int posInT_EcartFDegpLength(const TSet set,const int length,LObject &p);
int posInT_FDegpLength(const TSet set,const int length,LObject &p);
int posInT_pLength(const TSet set,const int length,LObject &p);
#endif


void reorderS (int* suc,kStrategy strat);
int posInLF5C (const LSet set, const int length,
               LObject* L,const kStrategy strat);
int posInLSig (const LSet set, const int length,
               LObject* L,const kStrategy strat);
int posInLSigRing (const LSet set, const int length,
               LObject* L,const kStrategy strat);
int posInSyz (const kStrategy strat, const poly sig);
int posInL0 (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL11 (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL11Ring (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInLF5CRing (const LSet set, int start , const int length,
             LObject* L,const kStrategy strat);
int posInL11Ringls (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL13 (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL15 (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL15Ring (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL17 (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL10 (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL10Ring (const LSet set, const int length,
             LObject* L,const kStrategy strat);
int posInL110 (const LSet set, const int length,
             LObject* L,const kStrategy strat);
KINLINE poly redtailBba (poly p,int end_pos,kStrategy strat,BOOLEAN normalize=FALSE);
KINLINE poly redtailBbaBound (poly p,int end_pos,kStrategy strat,int bound,BOOLEAN normalize=FALSE);
KINLINE poly redtailBba_Ring (poly p,int end_pos,kStrategy strat);
KINLINE poly redtailBba_Z (poly p,int end_pos,kStrategy strat);
poly redtailBba_NF (poly p, kStrategy strat );
poly redtailBba_Ring (LObject* L, int end_pos, kStrategy strat );
poly redtailBba_Z (LObject* L, int end_pos, kStrategy strat );
void redtailBbaAlsoLC_Z (LObject* L, int end_pos, kStrategy strat );
poly redtailBba (LObject *L, int end_pos,kStrategy strat,
                 BOOLEAN withT = FALSE,BOOLEAN normalize=FALSE);
poly redtailBbaBound (LObject *L, int end_pos,kStrategy strat,int bound,
                 BOOLEAN withT = FALSE,BOOLEAN normalize=FALSE);
poly redtailSba (LObject *L, int end_pos,kStrategy strat,
                 BOOLEAN withT = FALSE,BOOLEAN normalize=FALSE);
poly redtailBba (TObject *T, int end_pos,kStrategy strat);
poly redtail (poly p,int end_pos,kStrategy strat);
poly redtail (LObject *L,int end_pos,kStrategy strat);
poly redNF (poly h,int & max_ind,int nonorm,kStrategy strat);
int redNF0 (LObject *P,kStrategy strat);
poly redNFTail (poly h,const int sl,kStrategy strat);
int redHoney (LObject* h, kStrategy strat);
int redHoneyM (LObject* h, kStrategy strat);
int redLiftstd (LObject* h, kStrategy strat);
int redRing (LObject* h,kStrategy strat);
int redRing_Z (LObject* h,kStrategy strat);
int redRiloc (LObject* h,kStrategy strat);
void enterExtendedSpoly(poly h,kStrategy strat);
void enterExtendedSpolySig(poly h,poly hSig,kStrategy strat);
void superenterpairs (poly h,int k,int ecart,int pos,kStrategy strat, int atR = -1);
void superenterpairsSig (poly h,poly hSig,int hFrom,int k,int ecart,int pos,kStrategy strat, int atR = -1);
int redLazy (LObject* h,kStrategy strat);
int redHomog (LObject* h,kStrategy strat);
int redSig (LObject* h,kStrategy strat);
int redSigRing (LObject* h,kStrategy strat);
//adds hSig to be able to check with F5's criteria when entering pairs!
void enterpairsSig (poly h, poly hSig, int from, int k, int ec, int pos,kStrategy strat, int atR = -1);
void enterpairs (poly h, int k, int ec, int pos,kStrategy strat, int atR = -1);
void entersets (LObject h);
void pairs ();
BOOLEAN sbaCheckGcdPair (LObject* h,kStrategy strat);
void message (int i,int* reduc,int* olddeg,kStrategy strat,int red_result);
void messageStat (int hilbcount,kStrategy strat);
void messageStatSBA (int hilbcount,kStrategy strat);
#ifdef KDEBUG
void messageSets (kStrategy strat);
#else
#define messageSets(s)  do {} while (0)
#endif

void initEcartNormal (TObject* h);
void initEcartBBA (TObject* h);
void initS (ideal F, ideal Q,kStrategy strat);
void initSL (ideal F, ideal Q,kStrategy strat);
void initSLSba (ideal F, ideal Q,kStrategy strat);
/*************************************************
 * when initializing a new bunch of principal
 * syzygies at the beginning of a new iteration
 * step in a signature-based algorithm we
 * compute ONLY the leading elements of those
 * syzygies, NOT the whole syzygy
 * NOTE: this needs to be adjusted for a more
 * general approach on signature-based algorithms
 ***********************************************/
void initSyzRules (kStrategy strat);
void updateS(BOOLEAN toT,kStrategy strat);
void enterSyz (LObject &p,kStrategy strat, int atT);
void enterT (LObject &p,kStrategy strat, int atT = -1);
void replaceInLAndSAndT(LObject &p, int tj, kStrategy strat);
void enterT_strong (LObject &p,kStrategy strat, int atT = -1);
void cancelunit (LObject* p,BOOLEAN inNF=FALSE);
void HEckeTest (poly pp,kStrategy strat);
void initBuchMoraCrit(kStrategy strat);
void initSbaCrit(kStrategy strat);
void initHilbCrit(ideal F, ideal Q, intvec **hilb,kStrategy strat);
void initBuchMoraPos(kStrategy strat);
void initBuchMoraPosRing(kStrategy strat);
void initSbaPos(kStrategy strat);
void initBuchMora (ideal F, ideal Q,kStrategy strat);
void initSbaBuchMora (ideal F, ideal Q,kStrategy strat);
void exitBuchMora (kStrategy strat);
void exitSba (kStrategy strat);
void updateResult(ideal r,ideal Q,kStrategy strat);
void completeReduce (kStrategy strat, BOOLEAN withT=FALSE);
void kFreeStrat(kStrategy strat);
void enterOnePairNormal (int i,poly p,int ecart, int isFromQ,kStrategy strat, int atR);
void chainCritNormal (poly p,int ecart,kStrategy strat);
void chainCritOpt_1 (poly,int,kStrategy strat);
void chainCritSig (poly p,int ecart,kStrategy strat);
BOOLEAN homogTest(polyset F, int Fmax);
BOOLEAN newHEdge(kStrategy strat);
BOOLEAN syzCriterion(poly sig, unsigned long not_sevSig, kStrategy strat);
BOOLEAN syzCriterionInc(poly sig, unsigned long not_sevSig, kStrategy strat);
KINLINE BOOLEAN arriRewDummy(poly sig, unsigned long not_sevSig, poly lm, kStrategy strat, int start);
BOOLEAN arriRewCriterion(poly sig, unsigned long not_sevSig, poly lm, kStrategy strat, int start);
BOOLEAN arriRewCriterionPre(poly sig, unsigned long not_sevSig, poly lm, kStrategy strat, int start);
BOOLEAN faugereRewCriterion(poly sig, unsigned long not_sevSig, poly lm, kStrategy strat, int start);
BOOLEAN findMinLMPair(poly sig, unsigned long not_sevSig, kStrategy strat, int start);

/// returns index of p in TSet, or -1 if not found
int kFindInT(poly p, TSet T, int tlength);
#ifdef HAVE_SHIFTBBA
int kFindInTShift(poly p, TSet T, int tlength);
#endif

/// return -1 if no divisor is found
///        number of first divisor in T, otherwise
int kFindDivisibleByInT(const kStrategy strat, const LObject* L, const int start=0);
int kFindDivisibleByInT_ecart(const kStrategy strat, const LObject* L, const int ecart);
int kFindDivisibleByInT_Z(const kStrategy strat, const LObject* L, const int start=0);
int kFindSameLMInT_Z(const kStrategy strat, const LObject* L, const int start=0);

/// tests if T[0] divides the leading monomial of L, returns -1 if not
int kTestDivisibleByT0_Z(const kStrategy strat, const LObject* L);
/// return -1 if no divisor is found
///        number of first divisor in S, otherwise
int kFindDivisibleByInS(const kStrategy strat, int *max_ind, LObject* L);

int kFindNextDivisibleByInS(const kStrategy strat, int start,int max_ind, LObject* L);
TObject* kFindDivisibleByInS_T(kStrategy strat, int end_pos, LObject* L, TObject *T, long ecart = LONG_MAX);

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
KINLINE void k_GetStrongLeadTerms(const poly p1, const poly p2, const ring leadRing,
                               poly &m1, poly &m2, poly &lcm, const ring taiRing);
#ifdef KDEBUG
// test strat
BOOLEAN kTest(kStrategy strat);
// test strat, and test that S is contained in T
BOOLEAN kTest_TS(kStrategy strat);
// test LObject
BOOLEAN kTest_L(LObject* L, kStrategy strat,
                 BOOLEAN testp = FALSE, int lpos = -1,
                 TSet T = NULL, int tlength = -1);
// test TObject
BOOLEAN kTest_T(TObject* T, kStrategy strat, int tpos = -1, char TN = '?');
// test set strat->SevS
BOOLEAN kTest_S(kStrategy strat);
#else
#define kTest(A)        (TRUE)
#define kTest_TS(A)     (TRUE)
#define kTest_T(T,S)    (TRUE)
#define kTest_S(T)      (TRUE)
#define kTest_L(T,R)    (TRUE)
#endif


/***************************************************************
 *
 * From kstd2.cc
 *
 ***************************************************************/
poly kFindZeroPoly(poly input_p, ring leadRing, ring tailRing);
ideal bba (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat);
ideal sba (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat);
poly kNF2 (ideal F, ideal Q, poly q, kStrategy strat, int lazyReduce);
ideal kNF2 (ideal F,ideal Q,ideal q, kStrategy strat, int lazyReduce);
poly kNF2Bound (ideal F, ideal Q, poly q,int bound, kStrategy strat, int lazyReduce);
ideal kNF2Bound (ideal F,ideal Q,ideal q,int bound, kStrategy strat, int lazyReduce);
void initBba(kStrategy strat);
void initSba(ideal F,kStrategy strat);
void f5c (kStrategy strat, int& olddeg, int& minimcnt, int& hilbeledeg,
          int& hilbcount, int& srmax, int& lrmax, int& reduc, ideal Q,
          intvec *w,intvec *hilb );

/***************************************************************
 *
 * From kspoly.cc
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
//  reduce should be set inside "kNF" (only relevant for rings)
int ksReducePoly(LObject* PR,
                 TObject* PW,
                 poly spNoether = NULL,
                 number *coef = NULL,
                 poly *mon =NULL,
                 kStrategy strat = NULL,
                 BOOLEAN redtail = FALSE);

/* like ksReducePoly, but if the reducer has only 1 term we still
 * compute a possible coefficient multiplier for PR. this comes from
 * a special situation in redRing_Z and it is used only there. */
int ksReducePolyZ(LObject* PR,
                 TObject* PW,
                 poly spNoether = NULL,
                 number *coef = NULL,
                 kStrategy strat = NULL);

int ksReducePolyLC(LObject* PR,
                 TObject* PW,
                 poly spNoether = NULL,
                 number *coef = NULL,
                 kStrategy strat = NULL);


int ksReducePolyGCD(LObject* PR,
                 TObject* PW,
                 poly spNoether = NULL,
                 number *coef = NULL,
                 kStrategy strat = NULL);

int ksReducePolyBound(LObject* PR,
                 TObject* PW,
                 int bound,
                 poly spNoether = NULL,
                 number *coef = NULL,
                 kStrategy strat = NULL);

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
//          3 no reduction performed, not sig-safe!!!
//         -1 tailRing change could not be performed due to exceeding exp
//            bound of currRing
int ksReducePolySig(LObject* PR,
                 TObject* PW,
                 long idx,
                 poly spNoether = NULL,
                 number *coef = NULL,
                 kStrategy strat = NULL);

int ksReducePolySigRing(LObject* PR,
                 TObject* PW,
                 long idx,
                 poly spNoether = NULL,
                 number *coef = NULL,
                 kStrategy strat = NULL);

// Reduces PR at Current->next with PW
// Assumes PR != NULL, Current contained in PR
//         Current->next != NULL, LM(PW) divides LM(Current->next)
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
// return TRUE if gcdpoly creation of R[atR] and S[atS] does not violate
//             exponent bound of strat->tailRing
//      FALSE, otherwise
BOOLEAN kCheckStrongCreation(int atR, poly m1, int atS, poly m2, kStrategy strat);
poly preIntegerCheck(ideal F, ideal Q);
void postReduceByMon(LObject* h, kStrategy strat);
void postReduceByMonSig(LObject* h, kStrategy strat);
void finalReduceByMon(kStrategy strat);
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

/// Output some debug info about a given strategy
void kDebugPrint(kStrategy strat);

// getting sb order for sba computations
ring sbaRing(kStrategy strat, const ring r=currRing, BOOLEAN complete=TRUE, int sgn=1);

KINLINE void clearS (poly p, unsigned long p_sev, int* at, int* k,
  kStrategy strat);

#include "kernel/GBEngine/kInline.h"

/* shiftgb stuff */
#include "kernel/GBEngine/shiftgb.h"

#ifdef HAVE_SHIFTBBA
static inline int kFindInL1(const poly p, const kStrategy strat)
{
  for(int i=strat->Ll;i>=0;i--)
  {
    if (p==strat->L[i].p1) return i;
  }
  return -1;
}

void enterTShift(LObject p, kStrategy strat, int atT = -1);

BOOLEAN enterOnePairShift (poly q, poly p, int ecart, int isFromQ, kStrategy strat, int atR, int ecartq, int qisFromQ, int shiftcount, int ifromS);

void enterpairsShift (poly h,int k,int ecart,int pos,kStrategy strat, int atR);

void superenterpairsShift (poly h,int k,int ecart,int pos,kStrategy strat, int atR);

poly redtailBbaShift (LObject* L, int pos, kStrategy strat, BOOLEAN withT, BOOLEAN normalize);

int redFirstShift (LObject* h,kStrategy strat); // ok

ideal bbaShift(ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat);
#endif

// test syz strategy: // will be removed soon
EXTERN_VAR int (*test_PosInT)(const TSet T,const int tl,LObject &h);
EXTERN_VAR int (*test_PosInL)(const LSet set, const int length,
                LObject* L,const kStrategy strat);

static inline void kDeleteLcm(LObject *P)
{
 if (P->lcm!=NULL)
 {
   if (rField_is_Ring(currRing))
     pLmDelete(P->lcm);
   else
     pLmFree(P->lcm);
   P->lcm=NULL;
 }
}

void initenterpairs (poly h,int k,int ecart,int isFromQ,kStrategy strat, int atR = -1);
#endif
