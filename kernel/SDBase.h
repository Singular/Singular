#include <mod2.h>
#include <SDkutil.h>

#ifndef SDBASE_H
#define SDBASE_H

namespace ShiftDVec
{
  ideal freegbdvc
    (ideal I, int uptodeg, int lVblock, long deBoGriFlags);
  ideal kStd
    ( ideal F, ideal Q, tHomog h, intvec ** w, intvec *hilb,
      int syzComp, int newIdeal, intvec *vw, int uptodeg, int lV );
  ideal bba
    ( ideal F, ideal Q, ideal I,
      intvec *w,intvec *hilb,ShiftDVec::kStrategy strat );
  void initBba(ideal F,ShiftDVec::kStrategy strat);
  poly redtailBba
    (ShiftDVec::LObject* L, int pos, ShiftDVec::kStrategy strat, BOOLEAN withT=FALSE, 
     BOOLEAN normalize=FALSE                                   );
  poly redBba(ShiftDVec::LObject * h,int maxIndex,ShiftDVec::kStrategy strat);
  poly redtail (ShiftDVec::LObject* L, int pos, ShiftDVec::kStrategy strat);
  poly redtail(poly p, int pos, ShiftDVec::kStrategy strat);
  int redHomog (ShiftDVec::LObject* h,ShiftDVec::kStrategy strat);
  uint p_LmShortDivisibleBy
    ( ShiftDVec::TObject* t1, unsigned long sev_t1, ShiftDVec::TObject* t2, 
      unsigned long not_sev_t2, const ring r          );
  uint p_LmDivisibleBy
    (ShiftDVec::TObject * t1, ShiftDVec::TObject * t2, const ring r, int lV);
  static inline uint _p_LmDivisibleByNoComp
    (ShiftDVec::TObject * t1, ShiftDVec::TObject * t2, const ring r, int lV);
  int kFindDivisibleByInT
    ( const ShiftDVec::TSet& T, const unsigned long* sevT, ShiftDVec::LObject* L, 
      uint& shift, ShiftDVec::kStrategy strat, const int start=0       );
  TObject * kFindDivisibleByInS
    ( ShiftDVec::kStrategy strat, int pos, ShiftDVec::LObject* L, ShiftDVec::TObject* T, 
      uint & shift, int lV, int uptodeg=0, long ecart = LONG_MAX );
  int kFindDivisibleByInS
    (const ShiftDVec::kStrategy strat, int* max_ind, ShiftDVec::LObject* L, int shift);
  void updateSdVec(BOOLEAN toT,ShiftDVec::kStrategy strat);
  void completeReduce (ShiftDVec::kStrategy strat, BOOLEAN withT=FALSE);
  void updateResult(ideal r,ideal Q, ShiftDVec::kStrategy strat);
  KINLINE poly redtailBba
    (poly p,int pos,ShiftDVec::kStrategy strat,BOOLEAN normalize);
  void updateS(BOOLEAN toT,ShiftDVec::kStrategy strat);
  void clearS 
    ( ShiftDVec::LObject* H, unsigned long p_sev, int* at, int* k, 
      ShiftDVec::kStrategy strat                               );
  void enterpairs 
    ( LObject* H,int k,int ecart,int pos,kStrategy strat, 
      int atR                                             );
  LObject* enterOnePair
    ( poly p1, int atR1, int isFromQ1, int ecart1,
      poly p2, uint shift, int atR2, int isFromQ2, int ecart2, 
      ShiftDVec::kStrategy strat                                         );
  void initenterpairs
    ( ShiftDVec::LObject* H,int k,int ecart,int isFromQ,
      ShiftDVec::kStrategy strat, int atR = -1  );
  void enterRightOverlaps
    ( ShiftDVec::LObject* H, ShiftDVec::kStrategy strat, int k, uint** rightOvls, 
      uint* sizesRightOvls, int isFromQ, int ecart, int atR );
  void enterLeftOverlaps
    ( ShiftDVec::LObject* H, ShiftDVec::kStrategy strat, int k, uint** leftOvls, 
      uint* sizesLeftOvls, int isFromQ, int ecart, int atR );
  void enterSelfOverlaps
    ( ShiftDVec::LObject* H, ShiftDVec::kStrategy strat, uint* selfOvls,
      uint numSelfOvls, int isFromQ, int ecart, int atR );
  void GebauerMoeller
    ( uint* selfOvls, uint sizeSelfOvls,
      uint** rightOvls, uint* sizesRightOvls,
      uint** leftOvls, uint* sizesLeftOvls, 
      ShiftDVec::LObject* H, int k, ShiftDVec::kStrategy strat      );
  bool GMTestRight
    ( ShiftDVec::TObject* H1, ShiftDVec::TObject* H2, ShiftDVec::TObject* H3, uint shift2, 
      uint shift3, ShiftDVec::kStrategy strat, ring r = currRing     );
  bool GMTestLeft
    ( ShiftDVec::TObject* H1, ShiftDVec::TObject* H2, ShiftDVec::TObject* H3, uint shift2, 
      uint shift3, ShiftDVec::kStrategy strat, ring r = currRing     );
  bool GMTest
    ( ShiftDVec::TObject* H1, ShiftDVec::TObject* H2, uint sH2, TObject* H3, 
      uint* ovlH1H3, uint numOvlH1H3, 
      uint* ovlH3H2, uint numOvlH3H2                   );
  bool GM3
    ( ShiftDVec::LObject* H, uint shift1, uint shift2, 
      ShiftDVec::kStrategy strat, ring r = currRing  );
  bool GM3
    (ShiftDVec::LObject* H, ShiftDVec::LObject* L, ShiftDVec::kStrategy strat, ring r = currRing );
  void chainCrit
    ( ShiftDVec::LObject *H, int ecart, ShiftDVec::kStrategy strat );
  void initBuchMoraCrit(ShiftDVec::kStrategy strat);
  SD::LObject* enterL
    ( SD::LSet *set,
      int *length, int *LSetmax,
      SD::LObject p,int at, uint* dvec=NULL );
  void initSL(ideal F, ideal Q,ShiftDVec::kStrategy strat);
  void initBuchMora(ideal F,ideal Q,ShiftDVec::kStrategy strat);
  void kMergeBintoL(ShiftDVec::kStrategy strat);
  poly p_mLPshift(poly p, int sh, int uptodeg, int lV, const ring r);
  ideal initTestGM(ideal I, poly p, int uptodeg, int lVblock);
  void DeBoGriTestGM
    (ShiftDVec::TSet tset, int k, ShiftDVec::TObject* H, int uptodeg, int lVblock);
}

#endif
