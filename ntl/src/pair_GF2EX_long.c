
#include <NTL/pair_GF2EX_long.h>

#include <NTL/new.h>

NTL_START_IMPL

NTL_pair_impl(GF2EX,long,pair_GF2EX_long)
NTL_pair_io_impl(GF2EX,long,pair_GF2EX_long)
NTL_pair_eq_impl(GF2EX,long,pair_GF2EX_long)

NTL_vector_impl(pair_GF2EX_long,vec_pair_GF2EX_long)
NTL_io_vector_impl(pair_GF2EX_long,vec_pair_GF2EX_long)
NTL_eq_vector_impl(pair_GF2EX_long,vec_pair_GF2EX_long)

NTL_END_IMPL
