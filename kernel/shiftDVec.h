#include <mod2.h>

namespace ShiftDVec
{
  ideal freegbdvc
    (ideal I, int uptodeg, int lVblock, long deBoGriFlags);
  ideal kStd
    ( ideal F, ideal Q, tHomog h, intvec ** w, intvec *hilb,
      int syzComp, int newIdeal, intvec *vw, int uptodeg, int lV );
  ideal bba
    (ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat);
  void initBba(ideal F,kStrategy strat);
  poly redtailBba
    (LObject* L, int pos, kStrategy strat, BOOLEAN withT=FALSE, 
     BOOLEAN normalize=FALSE                                   );
  poly redBba(LObject * h,int maxIndex,kStrategy strat);
  poly redtail (LObject* L, int pos, kStrategy strat);
  poly redtail(poly p, int pos, kStrategy strat);
  int redHomog (LObject* h,kStrategy strat);
  uint p_LmShortDivisibleBy
    (const TObject* t1, unsigned long sev_t1, const TObject* t2, 
     unsigned long not_sev_t2, const ring r                     );
  uint p_LmDivisibleBy
    (TObject * t1, TObject * t2, const ring r, int lV);
  static inline uint _p_LmDivisibleByNoComp
    (TObject * t1,TObject * t2, const ring r, int lV);
  int kFindDivisibleByInT
    ( const TSet& T, const unsigned long* sevT, LObject* L, 
      uint& shift, kStrategy strat, const int start=0       );
  TObject * kFindDivisibleByInS
    ( kStrategy strat, int pos, LObject* L, TObject* T, 
      uint & shift, int lV, int uptodeg=0, long ecart = LONG_MAX );
  int kFindDivisibleByInS
    (const kStrategy strat, int* max_ind, LObject* L, int shift);
  void updateSdVec(BOOLEAN toT,kStrategy strat);
  void completeReduce (kStrategy strat, BOOLEAN withT=FALSE);
  void updateResult(ideal r,ideal Q, kStrategy strat);
  KINLINE poly redtailBba
    (poly p,int pos,kStrategy strat,BOOLEAN normalize);
  void updateS(BOOLEAN toT,kStrategy strat);
  void clearS 
    ( LObject* H, unsigned long p_sev, int* at, int* k, 
      kStrategy strat                               );
  void enterpairs 
    ( LObject* H,int k,int ecart,int pos,kStrategy strat, 
      int atR                                             );
  LObject* enterOnePair
    ( poly p1, int atR1, int isFromQ1, int ecart1,
      poly p2, uint shift, int atR2, int isFromQ2, int ecart2, 
      kStrategy strat                                         );
  void initenterpairs
    ( LObject* H,int k,int ecart,int isFromQ,
      kStrategy strat, int atR = -1  );
  void enterRightOverlaps
    ( LObject* H, kStrategy strat, int k, uint** rightOvls, 
      uint* sizesRightOvls, int isFromQ, int ecart, int atR );
  void enterLeftOverlaps
    ( LObject* H, kStrategy strat, int k, uint** leftOvls, 
      uint* sizesLeftOvls, int isFromQ, int ecart, int atR );
  void enterSelfOverlaps
    ( LObject* H, kStrategy strat, uint* selfOvls,
      uint numSelfOvls, int isFromQ, int ecart, int atR );
  void GebauerMoeller
    ( uint* selfOvls, uint sizeSelfOvls,
      uint** rightOvls, uint* sizesRightOvls,
      uint** leftOvls, uint* sizesLeftOvls, 
      LObject* H, int k, kStrategy strat      );
  bool GMTestRight
    ( TObject* H1, TObject* H2, TObject* H3, uint shift2, 
      uint shift3, kStrategy strat, ring r = currRing     );
  bool GMTestLeft
    ( TObject* H1, TObject* H2, TObject* H3, uint shift2, 
      uint shift3, kStrategy strat, ring r = currRing     );
  bool GMTest
    ( TObject* H1, TObject* H2, uint sH2, TObject* H3, 
      uint* ovlH1H3, uint numOvlH1H3, 
      uint* ovlH3H2, uint numOvlH3H2                   );
  bool GM3
    ( LObject* H, uint shift1, uint shift2, 
      kStrategy strat, ring r = currRing  );
  bool GM3
    (LObject* H, LObject* L, kStrategy strat, ring r = currRing );
  void chainCrit
    ( LObject *H, int ecart, kStrategy strat );
  void initBuchMoraCrit(kStrategy strat);
  LObject* enterL
    ( LSet *set,int *length, int *LSetmax, LObject p,int at, 
      uint* dvec=NULL                                        );
  void initSL(ideal F, ideal Q,kStrategy strat);
  void initBuchMora(ideal F,ideal Q,kStrategy strat);
  void kMergeBintoL(kStrategy strat);
  poly p_mLPshift(poly p, int sh, int uptodeg, int lV, const ring r);
  ideal initTestGM(ideal I, poly p, int uptodeg, int lVblock);
  void DeBoGriTestGM
    (TSet tset, int k, TObject* H, int uptodeg, int lVblock);
}
