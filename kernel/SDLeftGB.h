/*
  HALLO BO DIES IST EIN TEST
*/


#ifndef SDLeftGB_H
#define SDLeftGB_H

#include <kernel/SDkutil.h>

namespace ShiftDVec
{
  void initenterpairs
    ( LObject* J,
      int ecart, int isFromQ, ShiftDVec::kStrategy strat, int atR );

  void GebauerMoeller
    ( uint** overlaps,
      uint* sizesOvls, LObject* J, ShiftDVec::kStrategy strat );

  void GMFilter
    ( LObject* J, TObject* p_i, TObject* p_k,
      uint* i_ovls_j, uint* k_ovls_j,uint size_i_ovls_j, uint size_k_ovls_j );

  void GMFilter
    ( LObject* L, LObject* J,
      LObject* p_k, uint* k_ovls_j, uint size_k_ovls_j, ShiftDVec::kStrategy strat );

  void GMFilter
    ( int index_L, LObject* J,
      LObject* p_k, uint* k_ovls_j, uint size_k_ovls_j, ShiftDVec::kStrategy strat );

  void ShiftDVec::LeftGB::init_I
    ( ideal I, SD::kStrategy strat );

  void enterOverlaps
    ( LObject* J, ShiftDVec::kStrategy strat, uint** overlaps,
      uint* ovl_sizes, int isFromQ, int ecart, int atR );

  typedef skStrategy* kStrategy;
}

#endif
