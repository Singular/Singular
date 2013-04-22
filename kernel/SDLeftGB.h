#ifndef SDLeftGB_H
#define SDLeftGB_H

#include <kernel/SDkutil.h>

namespace ShiftDVec
{

  void initenterpairs
    ( ShiftDVec::LObject* J,
      int ecart, int isFromQ, ShiftDVec::kStrategy strat, int atR );

  void GebauerMoeller
    ( uint** overlaps,
      uint* sizesOvls, ShiftDVec::LObject* J, ShiftDVec::kStrategy strat );

  void GMFilter
    ( ShiftDVec::LObject* J, ShiftDVec::TObject* p_i, ShiftDVec::TObject* p_k,
      uint* i_ovls_j, uint* k_ovls_j,uint size_i_ovls_j, uint size_k_ovls_j );

  void GMFilter
    ( ShiftDVec::LObject* L, ShiftDVec::LObject* J,
      ShiftDVec::LObject* p_k, uint* k_ovls_j, uint size_k_ovls_j, ShiftDVec::kStrategy strat );

  void GMFilter
    ( int index_L, ShiftDVec::LObject* J,
      ShiftDVec::LObject* p_k, uint* k_ovls_j, uint size_k_ovls_j, ShiftDVec::kStrategy strat );

  void enterOverlaps
    ( ShiftDVec::LObject* J, ShiftDVec::kStrategy strat, uint** overlaps,
      uint* ovl_sizes, int isFromQ, int ecart, int atR );

  typedef skStrategy* kStrategy;
}

#endif
