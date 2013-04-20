#ifndef SDLeftGB_H
#define SDLeftGB_H

#include <kernel/SDkutil.h>

namespace ShiftDVec
{
  namespace LeftGB
  {
    void initenterpairs
      ( LObject* J,
        int ecart, int isFromQ, kStrategy strat, int atR );

    void GebauerMoeller
      ( uint** overlaps,
        uint* sizesOvls, LObject* J, kStrategy strat );

    void GMFilter
      ( LObject* J, TObject* p_i, TObject* p_k,
        uint* i_ovls_j, uint* k_ovls_j,
        uint size_i_ovls_j, uint size_k_ovls_j );

    void GMFilter
      ( LObject* L, LObject* J,
        LObject* p_k, uint* k_ovls_j,
        uint size_k_ovls_j, SD_kStrat strat );

    void GMFilter
      ( int index_L, LObject* J,
        LObject* p_k, uint* k_ovls_j,
        uint size_k_ovls_j, SD_kStrat strat );

    void enterOverlaps
      ( LObject* J, kStrategy strat, uint** overlaps,
        uint* ovl_sizes, int isFromQ, int ecart, int atR );
  }
}

#endif
