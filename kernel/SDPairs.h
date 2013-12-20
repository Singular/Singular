/*! @file
 *
 * @brief Function declarations for functions in SDPairs.cc
 *        in namespace ShiftDVec
 *
 * This file is part of the Letterplace ShiftDVec Project
 *
 * @author Grischa Studzinski
 * @author Benjamin Schnitzler benjaminschnitzler@googlemail.com
 *
 * @copyright see main copyright notice for Singular
 */
#include <SDkutil.h>

#ifndef SDPAIRS_H
#define SDPAIRS_H

namespace ShiftDVec
{
  namespace SD = ShiftDVec;

  void enterpairs( LObject* H, int k,
                   int ecart, int pos,
                   SD::kStrategy strat, int atR );
  void clearS( LObject* H,
               unsigned long p_sev, int* at,
               int* k, ShiftDVec::kStrategy strat );
  void initenterpairs( LObject* H, int k,
                       int ecart, int isFromQ,
                       SD::kStrategy strat, int atR );
  void FindOvls( LObject* H, BOOLEAN right, int k,
                 uint** ovls, uint* sizes, kStrategy strat );
  void FindOvls( LObject* H,
                 uint** ovls, uint* size, kStrategy strat );
  void AllocOvls( uint*** overlaps, uint** sizes, int k );
  void FreeOvls( uint** overlaps, uint* sizes, int k  );
  void FreeOvlsPair( uint* overlaps, uint size  );
  void enterOverlaps( LObject* H, BOOLEAN from_right,
                      SD::kStrategy strat, int k,
                      uint** ovls, uint* sizes_ovls,
                      int isFromQ, int ecart, int atR );
  void enterSelfOverlaps( LObject* H, SD::kStrategy strat,
                          uint* selfOvls, uint numSelfOvls,
                          int isFromQ, int ecart, int atR   );
  bool GM3( LObject* H, LObject* Pair,
           SD::kStrategy strat, ring r );
  LObject* enterOnePair( poly p1, int atR1,
                         int isFromQ1, int ecart1, poly p2,
                         uint shift, int atR2, int isFromQ2,
                         int ecart2, SD::kStrategy strat     );
  void p_Lcm( poly p1, poly p2, uint shift_lm_p2,
              poly return_lcm, SD::kStrategy strat, ring r );
  void GebauerMoeller( uint* selfOvls,
                       uint sizeSelfOvls,
                       uint** rightOvls, uint* sizesRightOvls,
                       uint** leftOvls, uint* sizesLeftOvls, 
                       LObject* H, int k, SD::kStrategy strat );
  bool GMTest( TObject* H1,
               TObject* H2, uint sH2,
               TObject* H3, 
               uint* ovlH1H3, uint numOvlH1H3,
               uint* ovlH3H2, uint numOvlH3H2  );
  bool GMTestRight( TObject* H1,
                    TObject* H2, TObject* H3,
                    uint shift2, uint shift3,
                    SD::kStrategy strat, ring r );
  bool GMTestLeft( TObject* H1,
                   TObject* H2, TObject* H3, 
                   uint shift2, uint shift3,
                   SD::kStrategy strat, ring r );
  void kMergeBintoL( SD::kStrategy strat );
}

#endif

// vim: set foldmethod=syntax :
// vim: set textwidth=65 :
// vim: set colorcolumn=+1 :
