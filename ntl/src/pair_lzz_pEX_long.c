
#include <NTL/pair_lzz_pEX_long.h>

#include <NTL/new.h>

NTL_START_IMPL

NTL_pair_impl(zz_pEX,long,pair_zz_pEX_long)
NTL_pair_io_impl(zz_pEX,long,pair_zz_pEX_long)
NTL_pair_eq_impl(zz_pEX,long,pair_zz_pEX_long)

NTL_vector_impl(pair_zz_pEX_long,vec_pair_zz_pEX_long)
NTL_io_vector_impl(pair_zz_pEX_long,vec_pair_zz_pEX_long)
NTL_eq_vector_impl(pair_zz_pEX_long,vec_pair_zz_pEX_long)

NTL_END_IMPL
