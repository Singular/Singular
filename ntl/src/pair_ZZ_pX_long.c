
#include <NTL/pair_ZZ_pX_long.h>

#include <NTL/new.h>

NTL_START_IMPL

NTL_pair_impl(ZZ_pX,long,pair_ZZ_pX_long)
NTL_pair_io_impl(ZZ_pX,long,pair_ZZ_pX_long)
NTL_pair_eq_impl(ZZ_pX,long,pair_ZZ_pX_long)

NTL_vector_impl(pair_ZZ_pX_long,vec_pair_ZZ_pX_long)
NTL_io_vector_impl(pair_ZZ_pX_long,vec_pair_ZZ_pX_long)
NTL_eq_vector_impl(pair_ZZ_pX_long,vec_pair_ZZ_pX_long)

NTL_END_IMPL
